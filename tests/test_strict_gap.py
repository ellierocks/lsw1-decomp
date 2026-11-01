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


if __name__ == "__main__":
    unittest.main()
