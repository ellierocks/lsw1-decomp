"""score_fn's SN runtime flags follow configure.py's cflags_sn_libc."""

import ast
import shlex
import unittest
from pathlib import Path

from tools import score_fn

ROOT = Path(__file__).resolve().parent.parent


def configure_list(name: str) -> list[str]:
    """Evaluate a list assignment in configure.py, expanding `*other_list` entries."""
    tree = ast.parse((ROOT / "configure.py").read_text())
    lists = {}
    for node in tree.body:
        if isinstance(node, ast.Assign) and len(node.targets) == 1 and isinstance(node.value, ast.List):
            target = node.targets[0]
            if isinstance(target, ast.Name):
                lists[target.id] = node.value
    out = []
    for elt in lists[name].elts:
        if isinstance(elt, ast.Starred):
            out += configure_list(elt.value.id)
        else:
            out.append(ast.literal_eval(elt))
    return out


class ScoreFnSnFlags(unittest.TestCase):
    def test_sn_flags_match_configure(self):
        expected = [tok for flag in configure_list("cflags_sn_libc") for tok in shlex.split(flag)]
        self.assertEqual(score_fn.SN_FLAGS, expected)

    def test_default_compiler_by_path(self):
        self.assertEqual(score_fn.default_compiler(ROOT / "src/sn/libc/stdio/sprintf.c"), "sn")
        self.assertEqual(score_fn.default_compiler(ROOT / "src/auto/game/levels.c"), "prodg")
        self.assertEqual(score_fn.default_compiler(Path("/tmp/scratch.c")), "prodg")


if __name__ == "__main__":
    unittest.main()
