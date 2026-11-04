import json
import subprocess
import tempfile
import unittest
from pathlib import Path
from tools import sprint_finish
from tools import sprint_metrics as metrics
from tools.sprint_finish import validate_queue, phase, rebase_refusal, merge_main


class SprintFinishTests(unittest.TestCase):
    def test_pr52_omitted_candidates_are_rejected(self):
        matched = ['fn_8000556C', 'fn_800034A0', 'fn_800055EC', 'NuFileBeginBlkRead']
        queue = matched + ['NuDatFileOpenSize', 'NuFileSeek', 'NuDatFileSeek',
                           'NuDatFileLoadBufferLsn', 'NuFilePos', 'NuFileClose']
        with self.assertRaisesRegex(ValueError, 'unresolved=.*NuDatFileSeek'):
            validate_queue(queue, matched, [])
        parked = [f'{n}=1/2: register mismatch; next: inspect lifetime' for n in queue[4:]]
        validate_queue(queue, matched, parked)

    def test_cannot_double_count_or_silently_omit_queue(self):
        for queue, matched, parked in [([], ['a'], []), (['a'], ['a', 'a'], []),
                                        (['a'], ['a'], ['a=1/2; next: inspect']),
                                        (['a'], [], ['a=unfinished'])]:
            with self.assertRaises(ValueError):
                validate_queue(queue, matched, parked)

    def test_phase_totals_exclude_paused_waits(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'timing.json'
            phase(path, 'setup', 100)
            phase(path, 'matching', 110)
            phase(path, 'paused', 130)
            phase(path, 'matching', 500)
            state = phase(path, 'finish', 505)
            self.assertEqual(state['seconds'], {'setup': 10, 'matching': 25})

    def test_deferral_needs_no_time_cutoff(self):
        validate_queue(['a', 'b'], ['a'], [], ['b'])
        with self.assertRaises(ValueError):
            validate_queue(['a', 'b'], ['a'], ['b=1/2: regs; next: x'], ['b'])

    def test_fetch_timeout_kills_child_processes(self):
        import time
        from tools.sprint_finish import fetch
        start = time.monotonic()
        self.assertFalse(fetch(0.5, ("sh", "-c", "sleep 30 & wait")))
        self.assertLess(time.monotonic() - start, 5)
        self.assertTrue(fetch(5, ("true",)))

    def test_timing_is_reported_not_budgeted(self):
        from tools.sprint_metrics import timing_report, near_miss
        rep = timing_report({'seconds': {'setup': 10, 'matching': 900}})
        self.assertEqual(rep['active_seconds'], 910)
        self.assertNotIn('finish_due', rep)
        self.assertTrue(near_miss(70, 71))
        self.assertFalse(near_miss(54, 62))
        self.assertFalse(near_miss(71, 71))

    def test_usage_reimport_does_not_double_count_or_invent_cost(self):
        from tools.sprint_metrics import record_usage
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'usage.json'
            record = dict(run_id='run1', model='reported-model', source='provider export',
                          input_tokens=100, cached_input_tokens=80, output_tokens=20)
            record_usage(path, record)
            result = record_usage(path, record)
            self.assertEqual(result['input_tokens'], 100)
            self.assertIsNone(result['cost_usd'])
            record['cached_input_tokens'] = 101
            with self.assertRaises(ValueError):
                record_usage(path, record)


class IdleAwareTimingTests(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.dir.cleanup)
        self.path = Path(self.dir.name) / 'timing.json'

    def test_long_gap_counts_only_up_to_cap(self):
        phase(self.path, 'matching', 0)
        metrics.record_activity(self.path, 600)           # 10 min gap: fully active
        metrics.record_activity(self.path, 600 + 36000)   # 10 h stall: capped at 15 min
        phase(self.path, 'tooling', 36000 + 600 + 60)
        report = metrics.timing_report(phase(self.path, 'paused', 36000 + 600 + 120))
        self.assertEqual(report['active_seconds'], 600 + 900 + 60 + 60)
        self.assertEqual(report['wall_seconds'], 36000 + 600 + 60 + 60)
        self.assertEqual(report['phases']['matching'], 600 + 900 + 60)
        self.assertEqual(report['idle_excluded_seconds'], 36000 - 900)
        self.assertEqual(report['idle_cap_seconds'], 900)

    def test_switching_phase_after_a_stall_is_capped_too(self):
        phase(self.path, 'matching', 0)
        state = phase(self.path, 'tooling', 99383)
        self.assertEqual(state['seconds'], {'matching': 900})
        self.assertEqual(state['wall'], {'matching': 99383})

    def test_cap_from_flag_env_and_saved_state(self):
        self.assertEqual(metrics.idle_cap(environ={}), 900)
        self.assertEqual(metrics.idle_cap(environ={'LSW1_IDLE_CAP': '60'}), 60)
        self.assertEqual(metrics.idle_cap(30, environ={'LSW1_IDLE_CAP': '60'}), 30)
        self.assertEqual(metrics.idle_cap(environ={}, state={'idle_cap': 120}), 120)
        with self.assertRaises(ValueError):
            metrics.idle_cap(0)
        phase(self.path, 'matching', 0, cap=60)
        state = phase(self.path, 'paused', 1000)
        self.assertEqual(state['seconds']['matching'], 60)

    def test_paused_stays_paused_and_ignores_activity(self):
        phase(self.path, 'matching', 0)
        phase(self.path, 'paused', 100)
        self.assertIsNone(metrics.record_activity(self.path, 50000))
        state = phase(self.path, 'matching', 90000)
        self.assertEqual(state['seconds'], {'matching': 100})

    def test_legacy_state_without_wall_fields_is_kept(self):
        self.path.write_text(json.dumps({'seconds': {'setup': 10}, 'active': 'matching', 'since': 100}))
        state = phase(self.path, 'paused', 160)
        self.assertEqual(state['seconds'], {'setup': 10, 'matching': 60})
        self.assertEqual(state['wall'], {'setup': 10, 'matching': 60})
        report = metrics.timing_report({'seconds': {'setup': 10}})
        self.assertEqual(report['wall_seconds'], 10)
        self.assertEqual(report['active_seconds'], 10)

    def test_backwards_clock_is_rejected(self):
        phase(self.path, 'matching', 100)
        with self.assertRaises(ValueError):
            phase(self.path, 'tooling', 50)


class QueueTests(unittest.TestCase):
    def test_queue_add_appends_missing_and_dedupes(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'baseline.json'
            self.assertIsNone(metrics.queue_add(path, ['x']))
            self.assertFalse(path.exists())
            path.write_text(json.dumps({'queue': ['a', 'b', 'a'], 'functions': 3}))
            self.assertEqual(metrics.queue_add(path, ['b', 'c', 'c', 'd']), ['c', 'd'])
            saved = json.loads(path.read_text())
            self.assertEqual(saved['queue'], ['a', 'b', 'c', 'd'])
            self.assertEqual(saved['functions'], 3)
            self.assertEqual(metrics.queue_add(path, ['a']), [])

    def test_queue_flag_tolerates_duplicates_but_unresolved_still_fails(self):
        validate_queue(['a', 'b', 'a'], ['a'], ['b=1/2: regs; next: x'])
        with self.assertRaisesRegex(ValueError, 'unresolved=.*b'):
            validate_queue(['a', 'b', 'a'], ['a'], [])
        with self.assertRaisesRegex(ValueError, 'outside queue=.*z'):
            validate_queue(['a'], ['a', 'z'], [])

    def test_score_fn_adds_scored_function(self):
        from tools import score_fn
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'baseline.json'
            path.write_text(json.dumps({'queue': ['a']}))
            score_fn.note_queue('fn_new', path)
            score_fn.note_queue('a', path)
            self.assertEqual(json.loads(path.read_text())['queue'], ['a', 'fn_new'])
            score_fn.note_queue('b', Path(directory) / 'missing.json')
            self.assertFalse((Path(directory) / 'missing.json').exists())


class MergeModeTests(unittest.TestCase):
    def git(self, cwd, *args):
        subprocess.run(['git', '-C', str(cwd), '-c', 'user.name=t', '-c', 'user.email=t@t',
                        '-c', 'commit.gpgsign=false', '-c', 'core.hooksPath=/dev/null', *args], check=True, capture_output=True)

    def setUp(self):
        self.dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.dir.cleanup)
        root = Path(self.dir.name)
        self.origin, self.work = root / 'origin.git', root / 'work'
        subprocess.run(['git', 'init', '-q', '--bare', '-b', 'main', str(self.origin)], check=True)
        subprocess.run(['git', 'clone', '-q', str(self.origin), str(self.work)], check=True, capture_output=True)
        # merge_main() runs plain git, so the clone needs its own identity (CI has none).
        for key, value in [('user.name', 't'), ('user.email', 't@t'), ('commit.gpgsign', 'false'),
                           ('core.hooksPath', '/dev/null')]:
            subprocess.run(['git', '-C', str(self.work), 'config', key, value], check=True)
        (self.work / 'f').write_text('1\n')
        self.git(self.work, 'add', 'f')
        self.git(self.work, 'commit', '-qm', 'base')
        self.git(self.work, 'push', '-q', 'origin', 'HEAD:main')
        self.git(self.work, 'checkout', '-qb', 'feature')
        old = sprint_finish.ROOT
        sprint_finish.ROOT = self.work
        self.addCleanup(setattr, sprint_finish, 'ROOT', old)

    def commit(self, name, text='x\n'):
        (self.work / name).write_text(text)
        self.git(self.work, 'add', name)
        self.git(self.work, 'commit', '-qm', name)

    def test_unpushed_commits_may_be_rebased(self):
        self.commit('a')
        self.git(self.work, 'fetch', '-q', 'origin')
        self.assertEqual(sprint_finish.pushed_commits(), [])
        self.assertIsNone(rebase_refusal(sprint_finish.pushed_commits()))

    def test_pushed_commits_refuse_rebase_and_name_merge(self):
        self.commit('a')
        self.git(self.work, 'push', '-q', 'origin', 'feature')
        self.commit('b')            # local-only commit on top does not make the pushed one safe
        pushed = sprint_finish.pushed_commits()
        self.assertEqual(len(pushed), 1)
        message = rebase_refusal(pushed)
        self.assertIn('--merge', message)
        self.assertIn('force push', message)

    def test_merge_brings_in_main_without_rewriting(self):
        self.commit('a')
        self.git(self.work, 'push', '-q', 'origin', 'feature')
        before = subprocess.run(['git', '-C', str(self.work), 'rev-parse', 'HEAD'], capture_output=True, text=True).stdout.strip()
        other = Path(self.dir.name) / 'other'
        subprocess.run(['git', 'clone', '-q', str(self.origin), str(other)], check=True, capture_output=True)
        (other / 'g').write_text('g\n')
        self.git(other, 'add', 'g')
        self.git(other, 'commit', '-qm', 'drift')
        self.git(other, 'push', '-q', 'origin', 'HEAD:main')
        self.assertEqual(merge_main(), (True, ''))
        self.assertTrue((self.work / 'g').exists())
        ancestor = subprocess.run(['git', '-C', str(self.work), 'merge-base', '--is-ancestor', before, 'HEAD'])
        self.assertEqual(ancestor.returncode, 0)

    def test_merge_conflict_aborts_cleanly(self):
        self.commit('f', 'mine\n')
        other = Path(self.dir.name) / 'other'
        subprocess.run(['git', 'clone', '-q', str(self.origin), str(other)], check=True, capture_output=True)
        (other / 'f').write_text('theirs\n')
        self.git(other, 'add', 'f')
        self.git(other, 'commit', '-qm', 'conflict')
        self.git(other, 'push', '-q', 'origin', 'HEAD:main')
        ok, why = merge_main()
        self.assertFalse(ok)
        self.assertIn('aborted', why)
        self.assertEqual((self.work / 'f').read_text(), 'mine\n')
        self.assertFalse((self.work / '.git' / 'MERGE_HEAD').exists())
        status = subprocess.run(['git', '-C', str(self.work), 'status', '--porcelain'], capture_output=True, text=True).stdout
        self.assertEqual(status, '')
