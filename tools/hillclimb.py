#!/usr/bin/env python3
"""Record bounded, reproducible C-matching experiments using the strict verifier.

Run from any directory. Paths are relative to the repository root. No model is
invoked, no source is restored automatically, and no build flags are changed.
"""
import argparse
from contextlib import contextmanager
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent


def score(report):
    """Lower is better; reference/data differences remain failures at 100% code."""
    return [0 if report['verdict'] == 'MATCH' else 1,
            len(report['differences']),
            abs(report['target_insns'] - report['mine_insns']),
            -report['exact_insns'],
            sum(r['status'].startswith('unverified') for r in report['references'])]


def summarize(attempts, patience):
    best = None
    stale = 0
    for attempt in attempts:
        if attempt['score'] is not None and (best is None or attempt['score'] < best['score']):
            best, stale = attempt, 0
        else:
            stale += 1
    state = 'READY' if not attempts else 'ITERATE'
    if best and best['returncode'] == 0:
        state = 'VERIFIED'
    elif stale >= patience:
        state = 'PARK'
    return {'state': state, 'attempts': len(attempts), 'without_improvement': stale,
            'best_attempt': best['attempt'] if best else None,
            'best_score': best['score'] if best else None}


@contextmanager
def lock(directory):
    """One writer per target; never truncate a prior experiment."""
    path = directory / '.lock'
    with path.open('x'):
        try:
            yield
        finally:
            path.unlink()


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('action', choices=['try', 'status'])
    ap.add_argument('function')
    ap.add_argument('--unit', required=True, help='configured comparison unit, e.g. numath/nuvec')
    ap.add_argument('--source', action='append', default=[], help='snapshot each edited source/header')
    ap.add_argument('--hypothesis', help='one concrete source/codegen hypothesis')
    ap.add_argument('--compiled-symbol')
    ap.add_argument('--ledger', default='work/hillclimb')
    ap.add_argument('--patience', type=int, default=8)
    ap.add_argument('--max-attempts', type=int, default=40)
    ap.add_argument('--timeout', type=int, default=120)
    args = ap.parse_args(argv)
    if min(args.patience, args.max_attempts, args.timeout) < 1:
        ap.error('budgets must be positive')
    # Hash identity instead of trusting symbol/unit strings as filesystem paths.
    identity = {'function': args.function, 'unit': args.unit,
                'compiled_symbol': args.compiled_symbol}
    key = hashlib.sha256(json.dumps(identity, sort_keys=True).encode()).hexdigest()[:20]
    directory = ROOT / args.ledger / key
    if args.action == 'try' and (not args.source or not args.hypothesis):
        ap.error('try requires --source and --hypothesis')
    directory.mkdir(parents=True, exist_ok=True)
    try:
        with lock(directory):
            attempts = [json.loads(p.read_text()) for p in sorted(directory.glob('[0-9]*.json'))]
            environment = {str(p): hashlib.sha256((ROOT / p).read_bytes()).hexdigest()
                           for p in ('configure.py', 'tools/prodg_cc.py', 'tools/verify_fn.py',
                                     'config/GL5E4F/symbols.txt', 'config/GL5E4F/splits.txt')}
            if attempts and attempts[-1]['environment'] != environment and args.action == 'try':
                ap.error('verification context changed; explain the change and use a new --ledger')
            summary = summarize(attempts, args.patience)
            if len(attempts) >= args.max_attempts and summary['state'] != 'VERIFIED':
                summary['state'] = 'PARK'
            if attempts and attempts[-1]['environment'] != environment:
                summary['state'] = 'CONTEXT_CHANGED'
            if args.action == 'status':
                print(json.dumps(dict(identity, **summary, ledger=str(directory)), indent=2))
                return 0
            if summary['state'] in ('VERIFIED', 'PARK') or len(attempts) >= args.max_attempts:
                ap.error('target verified or budget exhausted; inspect ledger and select another target')
            sources = {}
            for raw in args.source:
                path = (ROOT / raw).resolve()
                relative = path.relative_to(ROOT)
                data = path.read_bytes()
                sources[str(relative)] = {'sha256': hashlib.sha256(data).hexdigest(),
                                          'text': data.decode()}
            command = [sys.executable, str(ROOT / 'tools/verify_fn.py'), args.function,
                       '--unit', args.unit, '--build', '--strict-symbols', '--json']
            if args.compiled_symbol:
                command += ['--compiled-symbol', args.compiled_symbol]
            try:
                proc = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                                      timeout=args.timeout)
                output, errors, code = proc.stdout, proc.stderr, proc.returncode
            except subprocess.TimeoutExpired:
                output, errors, code = '', 'verifier timed out', 2
            try:
                report = json.loads(output)
                measured = score(report) if code in (0, 1, 3) else None
            except (ValueError, KeyError, TypeError):
                report, measured, code = {'error': 'invalid verifier output'}, None, 2
            record = dict(identity, attempt=len(attempts) + 1,
                          timestamp=datetime.now(timezone.utc).isoformat(),
                          hypothesis=args.hypothesis, sources=sources, command=command,
                          environment=environment,
                          head=subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
                          returncode=code, score=measured, report=report,
                          stdout=output, stderr=errors)
            with (directory / f'{record["attempt"]:04d}.json').open('x') as stream:
                json.dump(record, stream, indent=2)
                stream.write('\n')
            summary = summarize(attempts + [record], args.patience)
            print(json.dumps(dict(identity, **summary, ledger=str(directory)), indent=2))
            return code
    except (OSError, ValueError) as exc:
        print(f'error: {exc}', file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
