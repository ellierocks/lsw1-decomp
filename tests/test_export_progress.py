import copy
import json
from pathlib import Path
import tempfile
import unittest

from tools.export_progress import export, load_sweep, summarize_sweep


class ExportProgressTests(unittest.TestCase):
    def setUp(self):
        self.report = {'measures': {'total_code': '40', 'matched_code': '12',
                                    'total_functions': 2, 'matched_functions': 1},
                       'secret': 'must-not-publish', 'units': [{
                           'name': 'main/game/test', 'measures': {'total_code': '40', 'matched_code': '12'},
                           'metadata': {'source_path': '/private/path', 'progress_categories': ['game']},
                           'sections': [{'instructions': 'must-not-publish'}],
                           'functions': [{'name': 'done', 'size': '12', 'fuzzy_match_percent': 100,
                                          'instructions': 'must-not-publish'},
                                         {'name': 'todo', 'size': '28'}]}]}

    def run_export(self, **kwargs):
        return export(self.report, report_at='2026-11-10T00:00:00+00:00', **kwargs)

    def test_allowlist_and_numeric_normalization(self):
        data = self.run_export()
        self.assertEqual(data['latest']['measures']['matched_code'], 12)
        self.assertIsNone(data['latest']['revision'])
        self.assertIsNone(data['latest']['candidates'])
        self.assertNotIn('must-not-publish', json.dumps(data))
        self.assertNotIn('/private/path', json.dumps(data))

    def test_sweep_excludes_applied_and_already_matched(self):
        sweep = [dict(name=n, unit='game/test', verdict='MISMATCH', size=28, closeness=.95,
                      applied=n == 'applied') for n in ['done', 'todo', 'applied']]
        data = self.run_export(sweep=sweep)
        self.assertEqual([x['name'] for x in data['latest']['candidates']['rows']], ['todo'])
        self.assertEqual(data['latest']['candidates']['verdicts']['MISMATCH'], 3)

    def test_history_rerun_deduplicates_revision(self):
        first = self.run_export(revision='a'*40)
        second = self.run_export(revision='a'*40, history=first['history'])
        self.assertEqual(len(second['history']), 1)
        third = self.run_export(revision='b'*40, history=second['history'])
        self.assertEqual(len(third['history']), 2)

    def test_invalid_metrics_rejected(self):
        for value in [-1, 1.5, True, 'nan', 41]:
            with self.subTest(value=value):
                report = copy.deepcopy(self.report)
                report['measures']['matched_code'] = value
                with self.assertRaises(ValueError):
                    export(report, report_at='2026-11-10T00:00:00Z')

    def test_empty_report_is_valid(self):
        data = export({'measures': {}, 'units': []}, report_at='2026-11-10T00:00:00Z')
        self.assertEqual(data['latest']['measures']['total_code'], 0)
        self.assertEqual(data['latest']['units'], [])

    def test_units_and_functions_follow_load_addresses(self):
        fn = lambda name, at=None: dict(name=name, size='4', **({'metadata': {'virtual_address': str(at)}} if at else {}))
        unit = lambda name, functions: dict(name=f'main/{name}', measures={'total_code': '4'}, functions=functions)
        report = {'measures': {}, 'units': [
            unit('text', [fn('late', 0x80005000), fn('early', 0x80004000)]),
            unit('unknown', [fn('nowhere')]),
            unit('init', [fn('start', 0x80003100)])]}
        units = export(report, report_at='2026-11-10T00:00:00Z')['latest']['units']
        self.assertEqual([u['name'] for u in units], ['init', 'text', 'unknown'])
        self.assertEqual([f['name'] for f in units[1]['functions']], ['early', 'late'])

    def test_malformed_values_raise_value_error(self):
        report = copy.deepcopy(self.report)
        report['units'][0]['functions'][0]['fuzzy_match_percent'] = None
        with self.assertRaises(ValueError):
            export(report, report_at='2026-11-10T00:00:00Z')
        with self.assertRaises(ValueError):
            self.run_export(sweep=[dict(name='todo', unit='game/test', verdict='MISMATCH', size='big', closeness=.5)])

    def test_untrusted_provenance_rejected(self):
        with self.assertRaises(ValueError):
            self.run_export(revision='main')
        with self.assertRaises(ValueError):
            self.run_export(build_url='https://example.com/fake')


class SweepSummaryTests(unittest.TestCase):
    raw = [dict(name='todo', unit='game/test', source='int todo(void) {}', size=28, verdict='MISMATCH',
                exact=7, target='must-not-publish', padded=False, own_prototype=True,
                context='must-not-publish', closeness=.5)]

    def test_summary_keeps_only_allowlisted_fields(self):
        summary = summarize_sweep(self.raw, '2026-11-10T16:57:12Z')
        self.assertEqual(summary['swept_at'], '2026-11-10T16:57:12+00:00')
        self.assertEqual(summary['rows'], [dict(name='todo', unit='game/test', size=28,
                                                verdict='MISMATCH', closeness=.5)])
        self.assertNotIn('must-not-publish', json.dumps(summary))
        self.assertNotIn('todo(void)', json.dumps(summary))

    def test_summary_is_dated_by_swept_at_not_file_time(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp, 'sweep.json')
            path.write_text(json.dumps(summarize_sweep(self.raw, '2026-11-10T16:57:12+00:00')))
            rows, swept_at = load_sweep(path)
        self.assertEqual(swept_at, '2026-11-10T16:57:12+00:00')
        self.assertEqual(rows[0]['name'], 'todo')

    def test_invalid_summaries_rejected(self):
        for bad in [dict(rows=[]), dict(swept_at='2026-11-10T16:57:12', rows=[]),
                    dict(swept_at='yesterday', rows=[]), dict(swept_at='2026-11-10T16:57:12Z', rows={}),
                    dict(swept_at='2026-11-10T16:57:12Z', rows=[], extra=1), 'rows']:
            with self.subTest(bad=bad), tempfile.TemporaryDirectory() as tmp:
                path = Path(tmp, 'sweep.json')
                path.write_text(json.dumps(bad))
                with self.assertRaises(ValueError):
                    load_sweep(path)
