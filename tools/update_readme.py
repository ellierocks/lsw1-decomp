#!/usr/bin/env python3
"""Render README progress and publish checked README-only updates on Forgejo."""

import argparse
import base64
import json
import os
from pathlib import Path
import subprocess
import urllib.request
import urllib.parse

import progress

START = '<!-- progress:start -->'
END = '<!-- progress:end -->'
BRANCH_PREFIX = 'ci/readme-progress-'
MESSAGE = 'docs: update generated progress figures'
# Forgejo performs these writes as internal git pushes without the Actions token's
# permission, so its pre-receive hook rejects them; update refs with git() instead.
API_PUSHES = ('/branches', '/contents/', '/merge', '/update')


def render(symbols, measures):
    lines = [START, '**Matched code** (objdiff report with strict relocation comparison):', '',
             '| Measure | Matched | Total | Progress |',
             '|---------|---------|-------|----------|']
    for label, key in [('Code bytes', 'code'), ('Functions', 'functions')]:
        matched, total = int(measures['matched_' + key]), int(measures['total_' + key])
        if not 0 <= matched <= total or total <= 0:
            raise ValueError(f'Invalid {key} counts in report')
        lines.append(f'| {label} | {matched:,} | {total:,} | {matched / total * 100:.2f}% |')
    lines += ['', '**Symbol naming** (`config/GL5E4F/symbols.txt`):', '',
              '| Section | Named | Total | Progress |',
              '|---------|-------|-------|----------|']
    for section in ('text', 'rodata', 'bss'):
        stats = symbols['sections'][section]
        named, total = stats['named'], stats['total']
        label = '`.text` (functions)' if section == 'text' else f'`.{section}`'
        pct = f'{named / total * 100:.1f}%' if total else 'N/A'
        lines.append(f'| {label} | {named:,} | {total:,} | {pct} |')
    return '\n'.join(lines + [END])


def update(text, block):
    if text.count(START) != 1 or text.count(END) != 1:
        raise ValueError('README must contain exactly one progress marker pair')
    start, end = text.index(START), text.index(END)
    if end < start:
        raise ValueError('README progress markers are reversed')
    return text[:start] + block + text[end + len(END):]


def check_api_call(method, path):
    if method != 'GET' and any(marker in path.split('?')[0] for marker in API_PUSHES):
        raise ValueError(f'{method} {path} pushes without the Actions token permission; use git push')


class Forgejo:
    def __init__(self):
        self.url = os.environ['GITHUB_SERVER_URL'].rstrip('/') + '/api/v1/repos/' + os.environ['GITHUB_REPOSITORY']
        self.token = os.environ['FORGEJO_TOKEN']

    def request(self, method, path, data=None):
        check_api_call(method, path)
        req = urllib.request.Request(self.url + path,
            data=json.dumps(data).encode() if data is not None else None,
            headers={'Authorization': 'token ' + self.token, 'Content-Type': 'application/json'}, method=method)
        with urllib.request.urlopen(req, timeout=30) as response:
            body = response.read()
            return json.loads(body) if body else None


def git(*args):
    return subprocess.run(['git', *args], check=True, capture_output=True, text=True).stdout.strip()


def publish(text):
    api = Forgejo()
    ref = os.environ.get('GITHUB_REF', '')
    if os.environ['GITHUB_EVENT_NAME'] == 'workflow_dispatch' and ref.startswith('refs/heads/' + BRANCH_PREFIX):
        branch = ref.removeprefix('refs/heads/')
        pulls = api.request('GET', '/pulls?state=open&limit=50')
        candidates = [p for p in pulls if p['head']['ref'] == branch]
        if not candidates:
            print('No progress PR yet; waiting for the README commit run')
            return
        if len(candidates) != 1:
            raise ValueError('Expected one open progress PR for this branch')
        pr = candidates[0]
        # Only merge our own README-only branch, with freshly reproduced figures.
        if not pr['head']['ref'].startswith(BRANCH_PREFIX):
            return
        current = api.request('GET', f"/pulls/{pr['number']}")
        if current['head']['sha'] != os.environ['GITHUB_SHA'] or current['base']['ref'] != 'main':
            return
        if current['base']['sha'] != branch.removeprefix(BRANCH_PREFIX):
            print('Main advanced since this progress branch; skipping stale update')
            return
        if current['head']['repo']['id'] != current['base']['repo']['id']:
            return
        files = api.request('GET', f"/pulls/{pr['number']}/files")
        if [f['filename'] for f in files] != ['README.md']:
            raise ValueError('Automatic progress PR must change only README.md')
        base = api.request('GET', '/contents/README.md?ref=' + current['base']['sha'])
        expected = update(base64.b64decode(base['content']).decode(), text[text.index(START):text.index(END) + len(END)])
        if text != expected or Path('README.md').read_text() != text:
            raise ValueError('Progress PR differs from freshly generated README')
        git('push', 'origin', current['head']['sha'] + ':refs/heads/main')
        if not api.request('GET', f"/pulls/{pr['number']}")['merged']:
            api.request('PATCH', f"/pulls/{pr['number']}", {'state': 'closed'})
        git('push', 'origin', '--delete', 'refs/heads/' + branch)
        print('Merged checked README progress PR:', current['html_url'])
        return
    if os.environ['GITHUB_EVENT_NAME'] != 'push' or os.environ['GITHUB_REF'] != 'refs/heads/main':
        return
    sha = os.environ['GITHUB_SHA']
    if api.request('GET', '/branches/main')['commit']['id'] != sha:
        print('Main advanced; leaving publication to its newer run')
        return
    existing = api.request('GET', '/contents/README.md?ref=' + sha)
    if base64.b64decode(existing['content']).decode() == text:
        print('README progress is current')
        return
    branch = BRANCH_PREFIX + sha
    # Retry a run without rewriting history or opening duplicate PRs.
    pulls = api.request('GET', '/pulls?state=open&limit=50')
    if any(p['head']['ref'] == branch for p in pulls):
        api.request('POST', '/actions/workflows/build.yml/dispatches', {'ref': branch})
        print('Dispatched verification for existing progress PR')
        return
    if git('ls-remote', '--heads', 'origin', 'refs/heads/' + branch):
        branch_file = api.request('GET', '/contents/README.md?ref=' + urllib.parse.quote(branch, safe=''))
        if base64.b64decode(branch_file['content']).decode() != text:
            raise ValueError('Progress branch contains unexpected README edits')
    else:
        if git('rev-parse', 'HEAD') != sha:
            raise ValueError('Checkout does not match the published main commit')
        Path('README.md').write_text(text)
        git('-c', 'user.name=forgejo-actions', '-c', 'user.email=forgejo-actions@noreply.localhost',
            'commit', '-q', '-m', MESSAGE, '--', 'README.md')
        git('push', 'origin', 'HEAD:refs/heads/' + branch)
    pr = api.request('POST', '/pulls', {'base': 'main', 'head': branch,
        'title': MESSAGE,
        'body': f'Update README progress from the verified retail report and symbols at {sha}. Automatically merged only after tooling and retail checks reproduce this README-only change.'})
    print('Created README progress PR:', pr['html_url'])
    api.request('POST', '/actions/workflows/build.yml/dispatches', {'ref': branch})


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, default=progress.REPORT_PATH)
    parser.add_argument('--readme', type=Path, default=progress.PROJECT_ROOT / 'README.md')
    parser.add_argument('--publish', action='store_true')
    args = parser.parse_args()
    measures = progress.read_report(args.report)
    if measures is None:
        raise FileNotFoundError(args.report)
    block = render(progress.compute_symbol_stats(progress.read_symbols(progress.SYMBOLS_PATH)), measures)
    text = update(args.readme.read_text(), block)
    if args.publish:
        publish(text)
    else:
        args.readme.write_text(text)


if __name__ == '__main__':
    main()
