#!/usr/bin/env python3
"""Render README progress and publish checked README-only updates on Forgejo."""

import argparse
import json
import os
from pathlib import Path
import subprocess
import urllib.request

import progress

START = '<!-- progress:start -->'
END = '<!-- progress:end -->'
BRANCH_PREFIX = 'ci/readme-progress-'
MESSAGE = 'docs: update generated progress figures'
# On this private repository Forgejo 17 rejects the Actions token for API writes that
# push internally (branches, contents, merges) or recheck user access (pull requests).
# Git over HTTP and workflow dispatch honour the token, so all ref updates use git().
API_WRITES = {('POST', '/actions/workflows/build.yml/dispatches')}


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
    if method != 'GET' and (method, path.split('?')[0]) not in API_WRITES:
        raise ValueError(f'{method} {path} is unavailable to the Actions token; use git push')


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


def git_file(revision):
    return subprocess.run(['git', 'show', revision + ':README.md'], check=True, capture_output=True, text=True).stdout


def publish(text):
    """Push generated README figures to a check branch; fast-forward main once it passes."""
    api = Forgejo()
    event, ref, sha = os.environ['GITHUB_EVENT_NAME'], os.environ.get('GITHUB_REF', ''), os.environ['GITHUB_SHA']
    if event == 'workflow_dispatch' and ref.startswith('refs/heads/' + BRANCH_PREFIX):
        branch = ref.removeprefix('refs/heads/')
        source = branch.removeprefix(BRANCH_PREFIX)
        if git('rev-parse', 'HEAD') != sha or git('rev-parse', 'HEAD^') != source:
            raise ValueError('Progress branch must be one commit on its source SHA')
        if api.request('GET', '/branches/main')['commit']['id'] != source:
            print('Main advanced since this progress branch; leaving it for the newer run')
            return
        if git('diff', '--name-only', source, sha).splitlines() != ['README.md']:
            raise ValueError('Progress branch must change only README.md')
        expected = update(git_file(source), text[text.index(START):text.index(END) + len(END)])
        if text != expected or Path('README.md').read_text() != text:
            raise ValueError('Progress branch differs from freshly generated README')
        git('push', 'origin', sha + ':refs/heads/main')
        git('push', 'origin', '--delete', 'refs/heads/' + branch)
        print('Fast-forwarded main to checked README progress', sha)
        return
    if event != 'push' or ref != 'refs/heads/main':
        return
    if api.request('GET', '/branches/main')['commit']['id'] != sha:
        print('Main advanced; leaving publication to its newer run')
        return
    if git_file(sha) == text:
        print('README progress is current')
        return
    branch = BRANCH_PREFIX + sha
    if git('ls-remote', '--heads', 'origin', 'refs/heads/' + branch):
        git('fetch', '-q', '--depth=1', 'origin', 'refs/heads/' + branch)
        if git_file('FETCH_HEAD') != text:
            raise ValueError('Progress branch contains unexpected README edits')
    else:
        if git('rev-parse', 'HEAD') != sha:
            raise ValueError('Checkout does not match the published main commit')
        Path('README.md').write_text(text)
        git('-c', 'user.name=forgejo-actions', '-c', 'user.email=forgejo-actions@noreply.localhost',
            'commit', '-q', '-m', MESSAGE, '--', 'README.md')
        git('push', 'origin', 'HEAD:refs/heads/' + branch)
    api.request('POST', '/actions/workflows/build.yml/dispatches', {'ref': branch})
    print('Dispatched checks for README progress branch', branch)


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
