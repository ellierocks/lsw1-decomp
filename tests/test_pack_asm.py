"""Task packs refresh asm.s from the current build asm and keep hand-written files."""
import contextlib
import io
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from tools import pack_asm

tp = pack_asm._task_pack()

OLD = ".fn Foo, global\n\tlis r3, lbl_old@ha\n\tblr\n.endfn Foo\n"
NEW = ".fn Foo, global\n\tlis r3, lbl_new@ha\n\tblr\n.endfn Foo\n"


class PackAsmTest(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)
        self.build = self.root / "build/GL5E4F"
        self.unit = self.build / "asm/auto/game/foo.s"
        self.unit.parent.mkdir(parents=True)
        self.unit.write_text(".fn Bar, global\n\tblr\n.endfn Bar\n" + OLD)
        (self.build / "config.json").write_text(json.dumps({"units": [
            {"object": "build/GL5E4F/obj/auto/game/foo.o", "name": "auto/game/foo.c"}]}))
        symtab = self.root / "symbols.txt"
        symtab.write_text("Foo = .text:0x80000000; // type:function size:0x8\n")
        self.out = self.root / "work"
        for patch in (mock.patch.object(tp, "ASM_FILE", self.build / "asm/auto_01_800034A0_text.s"),
                      mock.patch.object(tp, "SYMTAB", symtab),
                      mock.patch.dict(tp._ASM_INDEX, clear=True),
                      mock.patch.dict(pack_asm._SYMBOLS, clear=True)):
            patch.start()
            self.addCleanup(patch.stop)

    def pack(self, asm=OLD):
        task = self.out / "Foo"
        task.mkdir(parents=True)
        (task / "asm.s").write_text(asm)
        (task / "candidate.c").write_text("void Foo(void) {}\n")
        (task / "status.md").write_text("IN_PROGRESS: notes\n")
        (task / "asm_m2c.s").write_text("derived\n")
        return task

    def ensure(self):
        err = io.StringIO()
        with contextlib.redirect_stderr(err):
            asm = pack_asm.ensure_asm("Foo", self.out)
        return asm, err.getvalue()

    def bump(self, path, text):
        st = path.stat()
        path.write_text(text)
        os.utime(path, ns=(st.st_atime_ns, st.st_mtime_ns + 1_000_000_000))

    def test_missing_pack_is_created_and_stamped(self):
        def fake_create(name, out_root):
            (out_root / name).mkdir(parents=True)
            (out_root / name / "asm.s").write_text(OLD)

        with mock.patch.object(pack_asm, "create", side_effect=fake_create) as create:
            asm, err = self.ensure()
        create.assert_called_once()
        self.assertEqual(asm.read_text(), OLD)
        self.assertEqual(err, "")
        self.assertTrue(pack_asm.stamp_current(asm.parent))

    def test_asm_only_pack_is_written_in_process(self):
        with mock.patch.object(pack_asm, "create", side_effect=AssertionError("subprocess")):
            asm = pack_asm.ensure_asm("Foo", self.out, asm_only=True)
            missing = pack_asm.ensure_asm("Nope", self.out, asm_only=True)
        self.assertEqual(asm.read_text(), tp.locate_asm(0x80000000, 8, "Foo")[0])
        self.assertEqual(sorted(p.name for p in asm.parent.iterdir()), ["asm.s", pack_asm.STAMP])
        self.assertTrue(pack_asm.stamp_current(asm.parent))
        self.assertFalse(missing.parent.exists())

    def test_ghidra_directory_without_asm_is_completed(self):
        task = self.out / "Foo"
        task.mkdir(parents=True)
        (task / "ghidra.c").write_text("reference output\n")
        (task / "status.md").write_text("research notes\n")
        # ls1_task_pack refuses a populated directory without writing asm.s.
        with mock.patch.object(pack_asm, "create") as create:
            asm, err = self.ensure()
        create.assert_called_once()
        self.assertEqual(asm.read_text(), OLD)
        self.assertTrue(pack_asm.stamp_current(task))
        self.assertEqual((task / "ghidra.c").read_text(), "reference output\n")
        self.assertEqual((task / "status.md").read_text(), "research notes\n")
        self.assertEqual(err, "")

    def test_unchanged_pack_is_not_reextracted(self):
        task = self.pack()
        self.ensure()  # first sight of a stampless pack: validated once
        self.assertTrue((task / pack_asm.STAMP).is_file())
        before = (task / "asm.s").stat().st_mtime_ns
        with mock.patch.object(tp, "locate_asm", side_effect=AssertionError("re-extracted")), \
                mock.patch.object(tp, "load_symtab", side_effect=AssertionError("re-read symbols")):
            asm, err = self.ensure()
        self.assertEqual((asm.read_text(), err), (OLD, ""))
        self.assertEqual(asm.stat().st_mtime_ns, before)
        self.assertTrue((task / "asm_m2c.s").is_file())

    def test_changed_build_asm_refreshes_only_asm(self):
        task = self.pack()
        self.ensure()
        self.bump(self.unit, self.unit.read_text().replace(OLD, NEW))
        asm, err = self.ensure()
        self.assertEqual(asm.read_text(), NEW)
        self.assertIn("refreshed", err)
        self.assertEqual(err.count("\n"), 1)
        self.assertFalse((task / "asm_m2c.s").exists())
        self.assertEqual((task / "candidate.c").read_text(), "void Foo(void) {}\n")
        self.assertEqual((task / "status.md").read_text(), "IN_PROGRESS: notes\n")
        self.assertEqual(self.ensure()[1], "")

    def test_stampless_stale_pack_is_refreshed(self):
        self.pack()
        self.unit.write_text(NEW)
        asm, err = self.ensure()  # the pack predates the build: no stamp, so it is checked
        self.assertEqual(asm.read_text(), NEW)
        self.assertIn("refreshed", err)

    def test_hand_edited_asm_is_restored(self):
        task = self.pack()
        self.ensure()
        self.bump(task / "asm.s", "garbage\n")
        self.assertEqual(self.ensure()[0].read_text(), OLD)

    def test_missing_build_asm_keeps_pack(self):
        task = self.pack("kept\n")
        for path in sorted(self.build.rglob("*"), reverse=True):
            path.rmdir() if path.is_dir() else path.unlink()
        asm, err = self.ensure()
        self.assertEqual((asm.read_text(), err), ("kept\n", ""))
        self.assertFalse((task / pack_asm.STAMP).exists())
        self.assertTrue((task / "asm_m2c.s").is_file())

    def test_leftover_split_asm_is_ignored(self):
        tp.ASM_FILE.write_text(".fn Foo, global\n\tstale\n.endfn Foo\n")  # an earlier split's file
        self.assertEqual(tp.extract_asm(0x80000000, 8, "Foo"), OLD)
        (self.build / "config.json").unlink()  # no build config: every .s, ASM_FILE first
        self.assertEqual(tp.extract_asm(0x80000000, 8, "Foo"), ".fn Foo, global\n\tstale\n.endfn Foo\n")


if __name__ == "__main__":
    unittest.main()
