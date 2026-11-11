import copy
import json
import unittest

from tools.export_progress import export


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
