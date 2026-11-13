"""The nuanim evaluator needs source externs declared below its old stub."""
import subprocess
import tempfile
import unittest
from pathlib import Path

from tools import score_fn


class ForwardExternTests(unittest.TestCase):
    def test_later_source_types_replace_guesses_before_use(self):
        source = ('int target(void) { return 0; }\n'
                  'extern char message[];\n'
                  'extern int (*factory(char *))(int);\n')
        body = ('extern long message;\n'
                'extern void *factory(void);\n'
                'int target(void) { return factory(message)(3); }\n')
        out = score_fn.splice_body(source, 'target', body)
        self.assertTrue(out.startswith('extern char message[];\n'))
        self.assertNotIn('extern long', out)
        self.assertNotIn('extern void *', out)
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'candidate.c'
            path.write_text(out)
            subprocess.run(['cc', '-std=c89', '-Werror', '-c', str(path),
                            '-o', str(path.with_suffix('.o'))],
                           check=True, capture_output=True)

    def test_disabled_later_declaration_is_not_forwarded(self):
        source = ('int target(void) { return 0; }\n#if 0\n'
                  'extern long value;\n#endif\n')
        body = 'extern int value;\nint target(void) { return value; }\n'
        out = score_fn.splice_body(source, 'target', body)
        self.assertTrue(out.startswith('extern int value;'))
        self.assertIn('#if 0\nextern long value;\n#endif', out)

    def test_existing_visible_declaration_is_not_duplicated(self):
        source = 'extern int value;\nint target(void) { return 0; }\n'
        body = 'extern long value;\nint target(void) { return value; }\n'
        out = score_fn.splice_body(source, 'target', body)
        self.assertEqual(out.count('extern int value;'), 1)
        self.assertNotIn('extern long', out)
