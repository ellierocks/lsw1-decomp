"""Reject tools whose module-level names are bound twice.

Large tools such as sweep.py are edited by many branches at once. When two
branches each add a module-level name (two `SWITCH` regexes once), the merge
silently keeps only the later binding. Only top-level def/class/assignment
statements are checked: rebinding inside `try`/`if` blocks (import fallbacks,
platform switches) is deliberate, and imports may legitimately repeat a package
(`import urllib.error` and `import urllib.request`).
"""

import ast
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1] / 'tools'


def bound_names(node):
    """Names a top-level statement binds by def, class or plain assignment."""
    if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef, ast.ClassDef)):
        return [node.name]
    if isinstance(node, ast.Assign):
        return [n.id for target in node.targets for n in ast.walk(target)
                if isinstance(n, ast.Name) and isinstance(n.ctx, ast.Store)]
    if isinstance(node, ast.AnnAssign) and node.value is not None and isinstance(node.target, ast.Name):
        return [node.target.id]
    return []


def duplicate_bindings(source, filename='<module>'):
    """(name, first line, later line) for each top-level name bound more than once."""
    first, duplicates = {}, []
    for node in ast.parse(source, filename).body:
        for name in bound_names(node):
            if name in first:
                duplicates.append((name, first[name], node.lineno))
            else:
                first[name] = node.lineno
    return duplicates


class ToolNamespaceTests(unittest.TestCase):
    def test_detector_reports_each_rebinding(self):
        source = ('import re\nSWITCH = re.compile("a")\ndef f():\n    SWITCH = 1\n'
                  'try:\n    import json\nexcept ImportError:\n    json = None\n'
                  'A, (B, SWITCH) = 1, (2, 3)\nclass f:\n    pass\nX: int\nX = 1\nX += 1\nB[0] = 4\n')
        self.assertEqual(duplicate_bindings(source), [('SWITCH', 2, 9), ('f', 3, 10)])

    def test_tools_bind_each_module_level_name_once(self):
        paths = sorted([*TOOLS.glob('*.py'), *TOOLS.glob('sweep_repairs/*.py')])
        self.assertTrue(paths)
        problems = [f'{path.relative_to(TOOLS.parent)}: {name!r} bound at line {old} and again at line {new}'
                    for path in paths
                    for name, old, new in duplicate_bindings(path.read_text(encoding='utf-8'), str(path))]
        self.assertEqual(problems, [], '\n' + '\n'.join(problems))


if __name__ == '__main__':
    unittest.main()
