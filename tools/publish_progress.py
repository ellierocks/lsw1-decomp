#!/usr/bin/env python3
"""Publish an exported snapshot on progress-data without changing the checkout."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import subprocess


def merge(snapshot: dict, previous: dict | None) -> dict:
    if snapshot.get('schema_version') != 1 or (previous and previous.get('schema_version') != 1):
        raise ValueError('Unsupported progress schema')
    latest = snapshot['latest']
    if not latest['revision']:
        raise ValueError('Only a report with a recorded build revision can be published')
    points = {point['id']: point for point in (previous or {}).get('history', [])}
    for point in snapshot['history']:
        points[point['id']] = point
    history = sorted(points.values(), key=lambda point: point['report_at'])
    # Reject an older run rather than moving the live site backwards.
    if previous and latest['report_at'] < previous['latest']['report_at']:
        raise ValueError('Refusing to publish an older report')
    return {**snapshot, 'history': history}


def publish(snapshot: dict):
    def git(*args, content=None):
        return subprocess.check_output(['git', *args], input=content, text=True).strip()
    branch = 'refs/heads/progress-data'
    remote = git('ls-remote', '--heads', 'origin', branch)
    parent = None
    previous = None
    if remote:
        git('fetch', '--no-tags', 'origin', branch)
        parent = git('rev-parse', 'FETCH_HEAD')
        previous = json.loads(git('show', f'{parent}:progress.json'))
    payload = json.dumps(merge(snapshot, previous), separators=(',', ':')) + '\n'
    blob = git('hash-object', '-w', '--stdin', content=payload)
    tree = git('mktree', content=f'100644 blob {blob}\tprogress.json\n')
    if parent and tree == git('rev-parse', f'{parent}^{{tree}}'):
        print('Progress data unchanged')
        return
    args = ['commit-tree', tree]
    if parent:
        args += ['-p', parent]
    commit = git(*args, content=f'chore: publish progress for {snapshot["latest"]["revision"][:7]}\n')
    # No force: a competing writer must not silently lose another snapshot.
    subprocess.run(['git', 'push', 'origin', f'{commit}:{branch}'], check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('snapshot', type=Path)
    args = parser.parse_args()
    publish(json.loads(args.snapshot.read_text()))


if __name__ == '__main__':
    main()
