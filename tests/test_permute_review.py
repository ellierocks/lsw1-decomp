"""Exercise permute_review's declaration diff and suspicious-change flags, permwatch and the launcher."""
import os
import subprocess
import tempfile
import unittest
from pathlib import Path

from tools import permute_review as pr
from tools import permwatch as pw

ROOT = Path(__file__).resolve().parent.parent

BASE = """
typedef signed int s32;
typedef unsigned char u8;
typedef float f32;
struct Pair { s32 a, b; };
extern volatile s32 lbl_flag;
extern void *lbl_ptr;
s32 helper(void *, s32);
void other(void) { lbl_flag = 0; }
void target(void *arg0) {
    s32 i;
    u8 *p = arg0;
    helper(arg0 + 8, -1);
    for (i = 0; i < 3; i++) {
        lbl_flag += *(s32 *)(p + i);
    }
}
"""

# Same body as BASE; only the prototype's return type changed (the PR #65 shape).
PROTO_ONLY = """
typedef signed int s32;
typedef unsigned char u8;
typedef float f32;
struct Pair
{
  s32 a;
  s32 b;
};
extern volatile s32 lbl_flag;
extern void *lbl_ptr;
void helper(void *, s32);
void target(void *arg0)
{
  s32 i;
  u8 *p = (arg0);
  helper((arg0 + 8), (-1));
  for (i = 0; (i < 3); i++)
  {
    lbl_flag += (*((s32 *)(p + i)));
  }
}
"""


class ParenTests(unittest.TestCase):
    def norm(self, src):
        return pr.join_tokens(pr.strip_parens(pr.tokenize(src)))

    def test_redundant_parens_removed(self):
        self.assertEqual(self.norm("a = (b * c) + d;"), "a = b * c + d;")
        self.assertEqual(self.norm("x = * ((u8 * *) (s + 4));"), "x = * (u8 * *) (s + 4);")
        self.assertEqual(self.norm("if (((a && b) && c) || d) y = 1;"), "if (a && b && c || d) y = 1;")

    def test_meaningful_parens_kept(self):
        self.assertEqual(self.norm("z = (a + b) * c;"), "z = (a + b) * c;")
        self.assertEqual(self.norm("a = b - (c - d);"), "a = b - (c - d);")
        self.assertEqual(self.norm("y = (s32) z;"), "y = (s32) z;")
        self.assertEqual(self.norm("r = (T) (a + b);"), "r = (T) (a + b);")
        self.assertEqual(self.norm("q = (*p).x;"), "q = (* p).x;")
        self.assertEqual(self.norm("f((a, b));"), "f((a, b));")


class DeclarationTests(unittest.TestCase):
    def test_prototype_change_with_identical_body_is_reported(self):
        changes = pr.declaration_diff(BASE, PROTO_ONLY, "target")
        self.assertEqual([(c["kind"], c["name"]) for c in changes], [("proto", "helper")])
        self.assertEqual(changes[0]["removed"], ["s32 helper(void *, s32);"])
        self.assertEqual(changes[0]["added"], ["void helper(void *, s32);"])
        self.assertEqual(pr.body_diff(BASE, PROTO_ONLY, "target"), [])
        text, info = pr.render("target", "x", BASE, PROTO_ONLY)
        self.assertIn("body unchanged", text)
        self.assertEqual(info["flags"], [])

    def test_layout_only_changes_are_not_declaration_changes(self):
        reformatted = BASE.replace("s32 helper(void *, s32);", "s32   helper( void * ,s32 ) ;")
        self.assertEqual(pr.declaration_diff(BASE, reformatted, "target"), [])

    def test_extern_type_struct_and_signature_changes(self):
        out = (BASE.replace("extern void *lbl_ptr;", "extern u8 *lbl_ptr;")
               .replace("struct Pair { s32 a, b; };", "struct Pair { s32 a; f32 b; };")
               .replace("void target(void *arg0) {", "s32 target(void *arg0) {"))
        got = {(c["kind"], c["name"]): c for c in pr.declaration_diff(BASE, out, "target")}
        self.assertEqual(set(got), {("var", "lbl_ptr"), ("struct", "struct Pair"), ("signature", "target")})
        self.assertEqual(got[("signature", "target")]["added"], ["s32 target(void * arg0)"])
        self.assertIn("f32 b;", got[("struct", "struct Pair")]["added"][0])

    def test_split_declarators_match_combined(self):
        a = "struct S { s32 x, y; };\nextern f32 p[], q[];\n"
        b = "struct S { s32 x; s32 y; };\nextern f32 p[];\nextern f32 q[];\n"
        self.assertEqual(pr.declaration_diff(a, b, "none"), [])

    def test_prototype_added_for_pruned_definition_is_ignored(self):
        out = BASE.replace("void other(void) { lbl_flag = 0; }", "void other(void);")
        self.assertEqual(pr.declaration_diff(BASE, out, "target"), [])
        self.assertEqual(pr.declaration_diff(BASE.replace("void other(void) { lbl_flag = 0; }", ""), BASE, "target")
                         [0]["kind"], "def")

    def test_pruned_definitions_are_counted_not_listed(self):
        out = BASE.replace("void other(void) { lbl_flag = 0; }", "")
        self.assertEqual(pr.declaration_diff(BASE, out, "target"), [])
        self.assertEqual(pr.omitted_definitions(BASE, out, "target"), 1)


class BodyTests(unittest.TestCase):
    def test_body_diff_shows_only_changed_lines(self):
        out = BASE.replace("i < 3", "i < 4")
        diff = pr.body_diff(BASE, out, "target")
        changed = [ln for ln in diff if ln[:1] in "+-" and not ln.startswith(("+++", "---"))]
        self.assertEqual(len(changed), 2)
        self.assertIn("i < 3", changed[0])
        self.assertIn("i < 4", changed[1])

    def test_missing_function_returns_none(self):
        self.assertIsNone(pr.body_diff(BASE, BASE, "nope"))


class SuspiciousTests(unittest.TestCase):
    def flags(self, out, base=BASE):
        return pr.suspicious(base, out, "target", pr.declaration_diff(base, out, "target"))

    def test_clean_change_has_no_flags(self):
        self.assertEqual(self.flags(BASE.replace("i < 3", "i < 4")), [])

    def test_void_pointer_arithmetic_is_flagged_once_introduced(self):
        out = BASE.replace("u8 *p = arg0;", "u8 *p = arg0;\n    void *q;\n    q = lbl_ptr + 4;")
        flags = self.flags(out)
        self.assertEqual(len(flags), 1)
        self.assertIn("void *", flags[0])
        self.assertIn("lbl_ptr + 4", flags[0])
        # The same arithmetic already present in base.c is not reported again.
        self.assertEqual(self.flags(out, base=out), [])

    def test_param_retyped_to_void_pointer_is_flagged(self):
        base = BASE.replace("void target(void *arg0)", "void target(u8 *arg0)")
        flags = pr.suspicious(base, BASE, "target", pr.declaration_diff(base, BASE, "target"))
        self.assertTrue(any("arg0 + 8" in f for f in flags), flags)

    def test_cast_to_void_pointer_arithmetic(self):
        out = BASE.replace("u8 *p = arg0;", "u8 *p = (void *)arg0 + 4;")
        self.assertTrue(any("void *" in f for f in self.flags(out)))

    def test_removed_volatile_is_flagged(self):
        flags = self.flags(BASE.replace("extern volatile s32 lbl_flag;", "extern s32 lbl_flag;"))
        self.assertTrue(any("volatile removed: 1 in base.c, 0 in source.c" in f for f in flags), flags)
        self.assertTrue(any("declaration `lbl_flag`" in f for f in flags), flags)

    def test_local_read_before_assignment(self):
        out = BASE.replace("u8 *p = arg0;", "u8 *p = arg0;\n    s32 t;\n    helper(p, t);\n    t = 1;")
        flags = self.flags(out)
        self.assertEqual(len(flags), 1)
        self.assertIn("`t`", flags[0])

    def test_assignment_first_or_address_taken_is_not_flagged(self):
        out = BASE.replace("u8 *p = arg0;", "u8 *p = arg0;\n    s32 t, u;\n    t = 1;\n    helper(&u, t);\n    helper(p, u);")
        self.assertEqual(self.flags(out), [])

    def test_loop_variable_assigned_in_for_header_is_not_flagged(self):
        self.assertEqual(pr.uninitialised_locals(pr.function_tokens(BASE, "target")), [])


class OutputSelectionTests(unittest.TestCase):
    def test_best_output_is_lowest_score(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for n in ("output-300-1", "output-100-1", "output-100-2", "output-220-3", "other"):
                (root / "fn" / n).mkdir(parents=True)
            self.assertEqual(pr.pick_output("fn", None, root).name, "output-100-1")
            self.assertEqual(pr.pick_output("fn", "output-220-3", root).name, "output-220-3")
            with self.assertRaises(SystemExit):
                pr.pick_output("fn", "output-9-9", root)

    def test_main_end_to_end(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "target" / "output-0-1").mkdir(parents=True)
            (root / "target" / "base.c").write_text(BASE)
            (root / "target" / "output-0-1" / "source.c").write_text(PROTO_ONLY)
            import contextlib
            import io
            buf = io.StringIO()
            with contextlib.redirect_stdout(buf):
                self.assertEqual(pr.main(["target", "--root", str(root)]), 0)
            self.assertIn("void helper(void *, s32);", buf.getvalue())
            self.assertIn("output-0-1 (score 0)", buf.getvalue())


class PermwatchTests(unittest.TestCase):
    def test_targets_come_from_queue_file_or_flag(self):
        with tempfile.TemporaryDirectory() as tmp:
            Path(tmp, "queue_targets.txt").write_text("a\nb\n\nb\n")
            Path(tmp, "queue_targets-x.txt").write_text("c\n")
            def targets(unit, explicit=None):
                return pw.load_queue(tmp, unit, explicit)["targets"]

            self.assertEqual(targets(pw.DEFAULT_UNIT), ["a", "b"])
            self.assertEqual(targets("x"), ["c"])
            self.assertEqual(targets(pw.DEFAULT_UNIT, ["p,q", "r"]), ["p", "q", "r"])
            self.assertEqual(targets("missing"), [])

    def test_snapshot_states(self):
        with tempfile.TemporaryDirectory() as tmp:
            for name, log, outs in (("fresh", None, []), ("good", "base score = 500\n", ["output-0-1"]),
                                    ("improved", "base score = 500\n", ["output-200-1"])):
                d = Path(tmp, name)
                d.mkdir()
                for o in outs:
                    (d / o).mkdir()
                if log is not None:
                    (d / "overnight.log").write_text(log)
            info = pw.load_queue(tmp, pw.DEFAULT_UNIT, ["fresh", "good", "improved"])
            finished, matched, total, status, rows, active, pending = pw.progress(tmp, info)
            states = {r["name"]: r["state"] for r in rows}
            self.assertEqual(states, {"fresh": "queued", "good": "match", "improved": "done"})
            self.assertEqual((finished, matched, total, active, pending), (2, 1, 3, 0, 1))
            self.assertIn("improved 500→200", status)

    def test_once_without_targets_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            self.assertEqual(pw.main(["--once", "--root", tmp]), 2)


class LauncherTests(unittest.TestCase):
    def run_launcher(self, *args):
        return subprocess.run(["bash", str(ROOT / "tools" / "permute_long.sh"), *args], cwd=ROOT,
                              capture_output=True, text=True, env={**os.environ, "DECOMP_PERMUTER": ""})

    def test_help(self):
        r = self.run_launcher("--help")
        self.assertEqual(r.returncode, 0)
        self.assertIn("systemctl --user stop", r.stdout)

    def test_missing_target_directory_is_set_up_by_the_queue(self):
        r = self.run_launcher("--dry-run", "1", "1", "no_such_target_xyz")
        self.assertEqual(r.returncode, 0, r.stderr)
        self.assertIn("permute_setup.py no_such_target_xyz first", r.stdout)

    def test_dry_run_prints_command_and_queue_without_writing(self):
        perm = ROOT / "work" / "permute"
        made = perm / "zz_dry_run_target"
        had_work = (ROOT / "work").exists()
        made.mkdir(parents=True)
        try:
            r = self.run_launcher("--dry-run", "--unit", "second", "2", "1.5", "zz_dry_run_target")
            self.assertEqual(r.returncode, 0, r.stderr)
            self.assertIn("--unit=second", r.stdout)
            self.assertIn("systemd-inhibit --what=sleep:idle --mode=block", r.stdout)
            self.assertIn("zz_dry_run_target", r.stdout)
            self.assertIn("timeout -k 60 min(1.5h, time left)", r.stdout)
            self.assertIn("backlog-second.txt", r.stdout)
            self.assertFalse((perm / "queue_targets-second.txt").exists())
            self.assertFalse((perm / "backlog-second.txt").exists())
        finally:
            made.rmdir()
            if not had_work:
                for p in (perm, ROOT / "work"):
                    try:
                        p.rmdir()
                    except OSError:
                        pass


if __name__ == "__main__":
    unittest.main()
