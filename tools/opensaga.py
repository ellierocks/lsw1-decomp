#!/usr/bin/env python3
"""Read-only OpenSaga evidence lookup. Fetch explicitly; matching tools stay offline."""
from __future__ import annotations

import argparse
from functools import lru_cache
import json
import os
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
PIN = ROOT / 'config/donors/opensaga.json'


def checkout_path():
    return Path(os.environ.get('OPENSAGA', str(
        Path(os.environ.get('LSW1_CACHE', Path.home() / '.cache/lsw1')) / 'opensaga'))).expanduser().resolve()


def git(path, *args):
    return subprocess.check_output(['git', '-C', str(path), *args], text=True, stderr=subprocess.PIPE).strip()


@lru_cache(maxsize=4)
def read_report(path, stamp, size):
    """Cache only parsed report data; checkout identity is checked on each lookup."""
    data = json.loads(Path(path).read_text())
    index = {}
    for unit in data['units']:
        source = unit.get('source', '')
        # Report paths are untrusted metadata, never commands or arbitrary file paths.
        relative = Path(source)
        if not source.startswith('src/') or '..' in relative.parts or relative.is_absolute():
            continue
        for function in unit.get('functions', []):
            name = function.get('demangled_name', function['name']).split('(', 1)[0]
            row = {'source': source, 'symbol': function['name'],
                   'donor_match_percent': function.get('match_percent')}
            if row not in index.setdefault(name, []):
                index[name].append(row)
    return index


def snapshot():
    pin = json.loads(PIN.read_text())
    path = checkout_path()
    result = {'repository': pin['repository'], 'revision': pin['revision'],
              'checkout': str(path), 'status': 'unavailable'}
    try:
        if git(path, 'rev-parse', 'HEAD') != pin['revision']:
            result['status'] = 'wrong-revision'
            return result, {}
        if git(path, 'status', '--porcelain', '--untracked-files=no'):
            result['status'] = 'modified'
            return result, {}
        report = path / 'matching.json'
        stat = report.stat()
        index = read_report(str(report), stat.st_mtime_ns, stat.st_size)
        result['status'] = 'ready'
        return result, index
    except (OSError, subprocess.CalledProcessError, ValueError, KeyError, TypeError) as exc:
        result['detail'] = f'{type(exc).__name__}: run python3 tools/opensaga.py fetch'
        return result, {}


def lookup(name, state=None):
    info, index = state if state is not None else snapshot()
    matches = index.get(name, [])
    return dict(info, function=name, matches=matches,
                result=('name-lead' if matches else 'no-name-hit') if info['status'] == 'ready' else 'unavailable')


def markdown(name):
    evidence = lookup(name)
    lines = ['# OpenSaga evidence', '', f"Pinned revision: `{evidence['revision']}`.",
             f"Lookup: **{evidence['result']}** (checkout: `{evidence['status']}`).", '',
             'Read the GC body first. Shared names and donor percentages do not prove identity.',
             'See `docs/symbol_donors/opensaga_audit.md` for known differences and the queue.', '']
    for match in evidence['matches']:
        source = match['source']
        lines.append(f"- `{source}`: `{match['symbol']}`; donor similarity {match['donor_match_percent']}%.")
        lines.append(f"  {evidence['repository'].removesuffix('.git')}/blob/{evidence['revision']}/{source}")
    lines += ['', f"Local donor: `{evidence['checkout']}`. Do not execute its scripts or copy its source.",
              'No name hit does not rule out an equivalent with another name. Search the target and',
              'its callees/keyword strings with `python3 tools/opensaga.py search TERM`.',
              'If unavailable, run `python3 tools/opensaga.py fetch`; if offline, record the limit.', '',
              'Before the first game/Nu2 attempt, record what you read, GC-confirmed semantics/fields,',
              'rejected differences, or why nothing applies using `hillclimb.py try --donor-note`.',
              'Retain that conclusion in the PR or plateau handoff; strict verification is unchanged.', '']
    return '\n'.join(lines)


def fetch():
    pin = json.loads(PIN.read_text())
    path = checkout_path()
    if path.exists():
        info, _ = snapshot()
        if info['status'] != 'ready':
            raise ValueError(f"{path}: {info['status']}; preserve this checkout; set OPENSAGA to a fresh path")
        print(f"OpenSaga ready at {path} ({pin['revision']})")
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(['git', 'clone', '--no-checkout', pin['repository'], str(path)], check=True)
    subprocess.run(['git', '-C', str(path), 'checkout', '--detach', pin['revision']], check=True)
    info, _ = snapshot()
    if info['status'] != 'ready':
        raise ValueError(f"OpenSaga checkout validation failed: {info['status']}")
    print(f"OpenSaga ready at {path} ({pin['revision']})")


def resolve(raw):
    if raw.startswith('0x'):
        address = int(raw, 16)
        text = (ROOT / 'config/GL5E4F/symbols.txt').read_text()
        for name, value in re.findall(r'^(\S+) = \.text:(0x[0-9A-Fa-f]+);', text, re.M):
            if int(value, 16) == address:
                return name
        raise ValueError(f'no current GC function at {raw}')
    return raw


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=('fetch', 'lookup', 'search'))
    parser.add_argument('target', nargs='?')
    parser.add_argument('--json', action='store_true')
    parser.add_argument('--limit', type=int, default=20)
    args = parser.parse_args(argv)
    try:
        if args.action == 'fetch':
            fetch()
            return 0
        if not args.target:
            parser.error('lookup/search requires a name, GC address or search term')
        if args.action == 'lookup':
            name = resolve(args.target)
            evidence = lookup(name)
            print(json.dumps(evidence, indent=2) if args.json else markdown(name))
            return 0 if evidence['status'] == 'ready' else 2
        info, _ = snapshot()
        if info['status'] != 'ready':
            raise ValueError(f"OpenSaga {info['status']}; run python3 tools/opensaga.py fetch")
        root = Path(info['checkout'])
        hits = []
        for path in sorted((root / 'src').rglob('*')):
            if path.suffix not in ('.c', '.cpp', '.h', '.hpp') or not path.is_file():
                continue
            # Do not follow a donor symlink outside the inspected source tree.
            if not path.resolve().is_relative_to(root):
                continue
            for number, line in enumerate(path.read_text(errors='replace').splitlines(), 1):
                if args.target.casefold() in line.casefold():
                    hits.append({'source': str(path.relative_to(root)), 'line': number})
                    if len(hits) >= max(1, args.limit):
                        break
            if len(hits) >= max(1, args.limit):
                break
        print(json.dumps(dict(info, term=args.target, hits=hits), indent=2))
        return 0
    except (OSError, ValueError, subprocess.CalledProcessError) as exc:
        print(f'error: {exc}', file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
