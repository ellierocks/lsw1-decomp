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
