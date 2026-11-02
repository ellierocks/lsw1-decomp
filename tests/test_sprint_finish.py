import tempfile
import unittest
from pathlib import Path
from tools.sprint_finish import validate_queue, phase


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

    def test_cutoff_defers_without_inventing_plateaus(self):
        from tools.sprint_metrics import budget_report
        self.assertFalse(budget_report({'seconds': {'matching': 509}})['finish_due'])
        due = budget_report({'seconds': {'setup': 10, 'matching': 500}})['finish_due']
        self.assertTrue(due)
        validate_queue(['a', 'b'], ['a'], [], ['b'], due)
        with self.assertRaises(ValueError):
            validate_queue(['a', 'b'], ['a'], [], ['b'], False)

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
