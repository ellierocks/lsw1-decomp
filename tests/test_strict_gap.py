"""The strict-gap tool only re-checks partially matched report functions."""
import unittest

from tools import strict_gap


class CandidateTests(unittest.TestCase):
    def test_only_partial_matches_in_range_are_candidates(self):
        report = {"units": [{"name": "main/u", "functions": [
            {"name": "full", "size": "8", "fuzzy_match_percent": 100.0},
            {"name": "gap", "size": "16", "fuzzy_match_percent": 99.4},
            {"name": "low", "size": "32", "fuzzy_match_percent": 40.0},
            {"name": "none", "size": "4"},
        ]}]}
        rows = strict_gap.candidates(report, 90.0)
        self.assertEqual([r["name"] for r in rows], ["gap"])
        self.assertEqual(rows[0]["size"], 16)


class CacheTests(unittest.TestCase):
    def test_verified_checks_each_name_once_per_report(self):
        import json, os, tempfile
        from pathlib import Path
        report = {"units": [{"name": "main/u", "functions": [
            {"name": "a", "size": "8", "fuzzy_match_percent": 99.0},
            {"name": "b", "size": "8", "fuzzy_match_percent": 98.0}]}]}
        calls = []
        check = lambda n: calls.append(n) or n == "a"
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / "report.json"
            path.write_text(json.dumps(report))
            self.assertEqual(strict_gap.verified(path, report, check=check), {"a"})
            self.assertEqual(strict_gap.verified(path, report, check=check), {"a"})
            self.assertEqual(calls, ["a", "b"])
            os.utime(path, ns=(1, 1))  # a rebuilt report invalidates the cache
            strict_gap.verified(path, report, check=check)
            self.assertEqual(calls, ["a", "b", "a", "b"])


if __name__ == "__main__":
    unittest.main()
