"""Regression tests for tools/verify_fn.py.

Synthetic tests build tiny PowerPC ELF objects and run anywhere. The
`GenuineProdgTests` class uses the real ProDG-compiled objects, the DTK-split
retail target objects and the retail DOL; it is skipped when `bash build.sh`
has not been run (portable CI) and runs in the retail comparison job.
"""
import contextlib
import hashlib
import io
import json
import os
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import verify_fn as vf

LWZ, ADDI, BLR, BL = 0x80640000, 0x38630001, 0x4E800020, 0x48000001
ADDI_R5 = 0x38A30001          # same mnemonic as ADDI, different destination register
LIS_R9, LFS_F5 = 0x3D200000, 0xC0A90000
SDA_LWZ = 0x80600000          # lwz r3, sym@sda21(r0)
SDA_LI = 0x38600000           # li r3, sym@sda21   (address-taken, no width)
R_REL24, R_HA, R_LO, R_SDA21 = 10, 6, 4, 109


def build_elf(funcs, undefined=(), relocs=(), rodata=b"", bss=0, extra_symbols=()):
    """funcs: [(name, [words])] laid out back to back in .text.
    relocs: (text_offset, type, symbol_name, addend); symbol '.rodata' means the section symbol.
    extra_symbols: (name, section, value, size) for defined data symbols."""
    text, fsyms = b"", []
    for name, words in funcs:
        fsyms.append((name, len(text), len(words) * 4))
        text += struct.pack(f">{len(words)}I", *words)
    shstr, secnames = b"\0", {}

    def add_shname(n):
        nonlocal shstr
        secnames[n] = len(shstr)
        shstr += n.encode() + b"\0"

    sections = [".text", ".rela.text"] + ([".rodata"] if rodata else []) + \
        ([".sbss"] if bss else []) + [".symtab", ".strtab", ".shstrtab"]
    for n in sections:
        add_shname(n)
    idx = {n: i + 1 for i, n in enumerate(sections)}
    strtab, names = b"\0", {}

    def add_str(n):
        nonlocal strtab
        if n not in names:
            names[n] = len(strtab)
            strtab += n.encode() + b"\0"
        return names[n]

    syms = [(0, 0, 0, 0, 0)]
    symidx = {}
    if rodata:
        symidx[".rodata"] = len(syms)
        syms.append((0, 0, 0, 3, idx[".rodata"]))
    for name, off, size in fsyms:
        symidx[name] = len(syms)
        syms.append((add_str(name), off, size, 0x12, idx[".text"]))
    for name, sec, value, size in extra_symbols:
        symidx[name] = len(syms)
        syms.append((add_str(name), value, size, 0x11, idx[sec]))
    for name in undefined:
        symidx[name] = len(syms)
        syms.append((add_str(name), 0, 0, 0x10, 0))
    symtab = b"".join(struct.pack(">IIIBBH", n, v, sz, i, 0, sh) for n, v, sz, i, sh in syms)
    rela = b"".join(struct.pack(">IIi", off + (2 if t in (3, 4, 5, 6) else 0),
                                (symidx[sym] << 8) | t, add) for off, t, sym, add in relocs)
    bodies = {".text": text, ".rela.text": rela, ".rodata": rodata, ".sbss": b"",
              ".symtab": symtab, ".strtab": strtab, ".shstrtab": shstr}
    out = bytearray(52)
    offsets, sizes = {}, {}
    for n in sections:
        out += b"\0" * (-len(out) % 4)
        offsets[n] = len(out)
        sizes[n] = bss if n == ".sbss" else len(bodies[n])
        out += bodies[n]
    out += b"\0" * (-len(out) % 4)
    shoff = len(out)
    out += b"\0" * 40
    for n in sections:
        typ = {".rela.text": 4, ".symtab": 2, ".strtab": 3, ".shstrtab": 3, ".sbss": 8}.get(n, 1)
        flags = {".text": 6, ".rodata": 2, ".sbss": 3}.get(n, 0)
        link = idx[".symtab"] if n == ".rela.text" else (idx[".strtab"] if n == ".symtab" else 0)
        info = idx[".text"] if n == ".rela.text" else (len(syms) if n == ".symtab" else 0)
        out += struct.pack(">10I", secnames[n], typ, flags, 0, offsets[n], sizes[n], link,
                           info, 4, 12 if n == ".rela.text" else (16 if n == ".symtab" else 0))
    out[0:16] = b"\x7fELF\x01\x02\x01" + bytes(9)
    out[16:52] = struct.pack(">HHIIIIIHHHHHH", 1, 20, 1, 0, 0, shoff, 0, 52, 0, 0, 40,
                             len(sections) + 1, idx[".shstrtab"])
    return bytes(out)


def build_dol(addr, data):
    hdr = [0] * 54
    hdr[0], hdr[18], hdr[36] = 0x100, addr, len(data)
    return struct.pack(">54I", *hdr) + bytes(0x100 - 216) + data


SYMBOLS_TXT = """\
Foo = .text:0x80001000; // type:function size:0x10
Bar = .text:0x80001010; // type:function size:0x8
ext_a = .text:0x80003000; // type:function size:0x4
ext_b = .text:0x80003004; // type:function size:0x4
alias_a = .text:0x80003000; // type:function size:0x4
lbl_80200000 = .rodata:0x80200000; // type:object size:0x8
lbl_80407B40 = .sdata:0x80407B40; // type:object size:0x4
"""
SPLITS_TXT = "unit/foo.c:\n\t.text       start:0x80001000 end:0x80001018\n"
FOO = [LWZ, ADDI, BL, BLR]
FOO_RELOCS = [(8, R_REL24, "ext_a", 0)]


class Env(unittest.TestCase):
    """Isolated project tree; patches the module's path globals."""

    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp(prefix="verify_fn_test_"))
        self.addCleanup(shutil.rmtree, self.tmp, ignore_errors=True)
        t = self.tmp
        (t / "config").mkdir()
        (t / "config/symbols.txt").write_text(SYMBOLS_TXT)
        (t / "config/splits.txt").write_text(SPLITS_TXT)
        patcher = mock.patch.multiple(
            vf, ROOT=t, SYMBOLS=t / "config/symbols.txt", SPLITS=t / "config/splits.txt",
            TARGET_OBJ_DIR=t / "build/obj", SRC_OBJ_DIR=t / "build/src", SRC_DIR=t / "src",
            DTK=t / "no-dtk", DOL_PATH=t / "main.dol")
        patcher.start()
        self.addCleanup(patcher.stop)

    def put(self, which, data, rel="unit/foo.o"):
        base = self.tmp / ("build/obj" if which == "target" else "build/src")
        (base / rel).parent.mkdir(parents=True, exist_ok=True)
        (base / rel).write_bytes(data)

    def target(self, funcs=None, **kw):
        funcs = funcs or [("Foo", FOO), ("Bar", [BLR, BLR])]
        kw.setdefault("undefined", ["ext_a"])
        kw.setdefault("relocs", FOO_RELOCS)
        self.put("target", build_elf(funcs, **kw))

    def mine(self, funcs=None, **kw):
        funcs = funcs or [("Foo", FOO), ("Bar", [BLR, BLR])]
        kw.setdefault("undefined", ["ext_a"])
        kw.setdefault("relocs", FOO_RELOCS)
        self.put("mine", build_elf(funcs, **kw))

    def run_tool(self, *args):
        out, err = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            code = vf.main(["Foo", "--no-dol", *args])
        return code, out.getvalue(), err.getvalue()


class SyntheticVerdictTests(Env):
    def test_identical_function_matches(self):
        self.target(), self.mine()
        code, out, _ = self.run_tool()
        self.assertEqual(code, 0, out)
        self.assertIn("RESULT: MATCH", out)
        self.assertIn("exact instructions: 4/4", out)

    def test_same_mnemonics_different_register_is_not_a_match(self):
        """The old mnemonic-only aligner reported this as a match."""
        self.target()
        self.mine([("Foo", [LWZ, ADDI_R5, BL, BLR]), ("Bar", [BLR, BLR])])
        code, out, _ = self.run_tool("--json")
        self.assertEqual(code, 1)
        data = json.loads(out)
        self.assertEqual(data["verdict"], "MISMATCH")
        self.assertEqual(data["mnemonic_similarity"], 4)
        self.assertEqual(data["exact_insns"], 3)
        self.assertEqual(data["differences"][0]["kind"], "operand")

    def test_different_mnemonic_is_opcode_difference(self):
        self.target()
        self.mine([("Foo", [LWZ, 0x7C632214, BL, BLR]), ("Bar", [BLR, BLR])])  # add r3,r3,r4
        code, out, _ = self.run_tool("--json")
        self.assertEqual(code, 1)
        self.assertEqual(json.loads(out)["differences"][0]["kind"], "opcode")

    def test_longer_candidate_reports_length_difference(self):
        self.target()
        self.mine([("Foo", [LWZ, ADDI, ADDI, BL, BLR]), ("Bar", [BLR, BLR])],
                  relocs=[(12, R_REL24, "ext_a", 0)])
        code, out, _ = self.run_tool("--json")
        self.assertEqual(code, 1)
        kinds = [d["kind"] for d in json.loads(out)["differences"]]
        self.assertIn("length", kinds)

    def test_wrong_call_target_is_reloc_diff(self):
        self.target(undefined=["ext_a", "ext_b"])
        self.mine(undefined=["ext_a", "ext_b"], relocs=[(8, R_REL24, "ext_b", 0)])
        code, out, _ = self.run_tool("--json")
        self.assertEqual(code, 1)
        diff = json.loads(out)["differences"][0]
        self.assertEqual(diff["kind"], "reloc")
        self.assertIn("ext_b", diff["detail"])

    def test_different_symbol_name_same_address_is_alias(self):
        self.target()
        self.mine(undefined=["alias_a"], relocs=[(8, R_REL24, "alias_a", 0)])
        code, out, _ = self.run_tool("--strict-symbols")
        self.assertEqual(code, 0, out)
        self.assertIn("same address in symbols.txt", out)

    def test_known_symbol_with_wrong_offset_is_caught_by_address(self):
        self.target()
        self.mine(relocs=[(8, R_REL24, "ext_a", 4)])
        code, out, _ = self.run_tool()
        self.assertEqual(code, 1)
        self.assertIn("0x80003004", out)

    def test_addend_difference_on_unmapped_symbol(self):
        self.target()
        self.mine(undefined=["mystery"], relocs=[(8, R_REL24, "mystery", 4)])
        code, out, _ = self.run_tool()
        self.assertEqual(code, 1)
        self.assertIn("addend", out)

    def test_reloc_only_on_one_side(self):
        self.target()
        self.mine(relocs=[])
        code, out, _ = self.run_tool("--json")
        self.assertEqual(code, 1)
        self.assertIn("only target has a relocation", out)

    def test_unknown_names_map_by_consistency_and_are_flagged(self):
        self.target(undefined=["ext_a"])
        self.mine(undefined=["my_helper"], relocs=[(8, R_REL24, "my_helper", 0)])
        code, out, _ = self.run_tool()
        self.assertEqual(code, 0)
        self.assertIn("MATCH (unverified references)", out)
        code, _, _ = self.run_tool("--strict-symbols")
        self.assertEqual(code, 3)

    def test_inconsistent_symbol_mapping_is_a_mismatch(self):
        t = [("Foo", [BL, BL, BLR, BLR]), ("Bar", [BLR, BLR])]
        self.target(t, undefined=["ext_a", "ext_b"],
                    relocs=[(0, R_REL24, "ext_a", 0), (4, R_REL24, "ext_b", 0)])
        self.mine(t, undefined=["x"], relocs=[(0, R_REL24, "x", 0), (4, R_REL24, "x", 0)])
        code, out, _ = self.run_tool()
        self.assertEqual(code, 1)
        self.assertIn("already mapped", out)

    def test_branch_out_of_function_without_reloc_matches_symbolic_reloc(self):
        # bl from Foo+0 to Bar (Foo is 0x10 bytes) => displacement +0x10
        t_foo = [0x48000011, BLR, BLR, BLR]
        t = [("Foo", t_foo), ("Bar", [BLR, BLR])]
        self.put("target", build_elf(t, relocs=[(0, R_REL24, "Bar", 0)]))
        self.put("mine", build_elf(t, relocs=[]))
        code, out, _ = self.run_tool()
        self.assertEqual(code, 0, out)

    def test_branch_back_to_own_entry_matches_dtk_relocation(self):
        # bne Foo at Foo+4: DTK relocates it, the assembler resolves it.
        t = [("Foo", [ADDI, 0x40820000, BLR, BLR]), ("Bar", [BLR, BLR])]
        m = [("Foo", [ADDI, 0x4082FFFC, BLR, BLR]), ("Bar", [BLR, BLR])]
        self.put("target", build_elf(t, relocs=[(4, 11, "Foo", 0)]))
        self.put("mine", build_elf(m, relocs=[]))
        code, out, _ = self.run_tool()
        self.assertEqual(code, 0, out)
        m = [("Foo", [ADDI, 0x4082FFF8, BLR, BLR]), ("Bar", [BLR, BLR])]
        self.put("mine", build_elf(m, relocs=[]))
        code, out, _ = self.run_tool()
        self.assertEqual(code, 1, out)

    def test_mwcc_halfword_sda21_offset_matches_word_offset(self):
        """MWCC places EMB_SDA21 at insn+2; DTK and GNU as place it at insn+0."""
        funcs = [("Foo", [SDA_LWZ, ADDI, BLR, BLR]), ("Bar", [BLR, BLR])]
        self.target(funcs, undefined=["ext_a"], relocs=[(0, R_SDA21, "ext_a", 0)])
        self.mine(funcs, undefined=["ext_a"], relocs=[(2, R_SDA21, "ext_a", 0)])
        code, out, _ = self.run_tool()
        self.assertEqual(code, 0, out)
        self.mine(funcs, undefined=["ext_a"], relocs=[(1, R_SDA21, "ext_a", 0)])
        code, out, err = self.run_tool()
        self.assertEqual(code, 2, out)
        self.assertIn("misaligned", err)


class SyntheticDataTests(Env):
    def install_dol(self, dol):
        self.dol_path = self.tmp / "main.dol"
        self.dol_path.write_bytes(dol)
        p = mock.patch.object(vf, "DOL_SHA1", hashlib.sha1(dol).hexdigest())
        p.start()
        self.addCleanup(p.stop)

    def float_funcs(self):
        return [("Foo", [LIS_R9, LFS_F5, BL, BLR]), ("Bar", [BLR, BLR])]

    def retail_with(self, target_bytes):
        """Target object + DOL whose text and rodata agree."""
        relocs_t = [(0, R_HA, "lbl_80200000", 0), (4, R_LO, "lbl_80200000", 0),
                    (8, R_REL24, "ext_a", 0)]
        self.target(self.float_funcs(), undefined=["lbl_80200000", "ext_a"], relocs=relocs_t)
        text = struct.pack(">4I", LIS_R9, LFS_F5, BL, BLR)
        image = bytearray(0x200000)
        image[0:len(text)] = text
        image[0x1FF000:0x1FF000 + len(target_bytes)] = target_bytes
        # one flat section: text at 0x80001000, lbl_80200000 at +0x1FF000
        self.install_dol(build_dol(0x80001000, bytes(image)))

    def mine_with_const(self, const):
        relocs_m = [(0, R_HA, ".rodata", 0), (4, R_LO, ".rodata", 0), (8, R_REL24, "ext_a", 0)]
        self.mine(self.float_funcs(), undefined=["ext_a"], relocs=relocs_m, rodata=const)

    def run_with_dol(self, *args):
        out, err = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            code = vf.main(["Foo", *args])
        return code, out.getvalue(), err.getvalue()

    def test_matching_constant_is_verified_against_retail_data(self):
        const = struct.pack(">f", 1.0)
        self.retail_with(const + bytes(4))
        self.mine_with_const(const + bytes(4))
        code, out, err = self.run_with_dol("--strict-symbols")
        self.assertEqual(code, 0, out + err)
        self.assertIn("verified: 4 bytes identical to retail", out)
        self.assertIn("retail DOL byte check: yes", out)

    def test_wrong_constant_is_a_mismatch(self):
        self.retail_with(struct.pack(">f", 1.0) + bytes(4))
        self.mine_with_const(struct.pack(">f", 2.0) + bytes(4))
        code, out, _ = self.run_with_dol("--json")
        self.assertEqual(code, 1)
        self.assertEqual(json.loads(out)["differences"][0]["kind"], "data")

    def test_locally_defined_symbol_with_target_name_is_still_data_checked(self):
        self.retail_with(struct.pack(">f", 1.0) + bytes(4))
        relocs_m = [(0, R_HA, "lbl_80200000", 0), (4, R_LO, "lbl_80200000", 0),
                    (8, R_REL24, "ext_a", 0)]
        self.mine(self.float_funcs(), undefined=["ext_a"], relocs=relocs_m,
                  rodata=struct.pack(">f", 2.0) + bytes(4),
                  extra_symbols=[("lbl_80200000", ".rodata", 0, 4)])
        code, out, _ = self.run_with_dol("--json")
        self.assertEqual(code, 1, out)
        self.assertEqual(json.loads(out)["differences"][0]["kind"], "data")

    def test_bss_in_mine_against_initialised_retail_data_stays_unverified(self):
        self.retail_with(bytes(4) + bytes(4))
        relocs_m = [(0, R_HA, "gvar", 0), (4, R_LO, "gvar", 0), (8, R_REL24, "ext_a", 0)]
        self.mine(self.float_funcs(), undefined=["ext_a"], relocs=relocs_m, bss=4,
                  extra_symbols=[("gvar", ".sbss", 0, 4)])
        code, out, err = self.run_with_dol("--strict-symbols")
        self.assertEqual(code, 3, out + err)
        self.assertIn("section placement does not)", out)

    def test_target_object_not_matching_dol_is_error(self):
        const = struct.pack(">f", 1.0)
        self.retail_with(const)
        corrupt = bytearray(self.dol_path.read_bytes())
        corrupt[0x100 + 4] ^= 0xFF  # retail LFS no longer matches the target object
        self.install_dol(bytes(corrupt))
        self.mine_with_const(const)
        code, _, err = self.run_with_dol()
        self.assertEqual(code, 2)
        self.assertIn("differs from the retail DOL", err)

    def test_wrong_dol_hash_is_refused(self):
        self.target(), self.mine()
        (self.tmp / "main.dol").write_bytes(build_dol(0x80001000, bytes(64)))
        code, _, err = self.run_with_dol()
        self.assertEqual(code, 2)
        self.assertIn("retail SHA-1", err)


class UnitProofCoverageTests(unittest.TestCase):
    def comparer(self, proof):
        return vf.Comparer(None, None, proof)

    def test_only_proved_locations_are_covered(self):
        section = mock.Mock(size=8)
        comparer = self.comparer({".sdata2": 0x1000})
        self.assertTrue(comparer.unit_proof_covers((".sdata2", 4), section, 4, 0x1004))
        self.assertFalse(comparer.unit_proof_covers((".sdata2", 4), section, 4, 0x1008))
        self.assertFalse(comparer.unit_proof_covers((".sdata2", 6), section, 4, 0x1006))
        self.assertFalse(comparer.unit_proof_covers((".sdata", 0), section, 4, 0x1000))
        self.assertFalse(self.comparer(None).unit_proof_covers((".sdata2", 0), section, 4, 0x1000))


class ErrorHandlingTests(Env):
    def test_truncated_object_is_error_not_mismatch(self):
        self.target()
        good = build_elf([("Foo", FOO), ("Bar", [BLR, BLR])], undefined=["ext_a"],
                         relocs=FOO_RELOCS)
        self.put("mine", good[:len(good) - 40])
        code, _, err = self.run_tool()
        self.assertEqual(code, 2, err)

    def test_unknown_function(self):
        self.target(), self.mine()
        out, err = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            code = vf.main(["Nope", "--no-dol"])
        self.assertEqual(code, 2)
        self.assertIn("not a code symbol", err.getvalue())

    def test_missing_compiled_symbol_lists_alternatives(self):
        self.target()
        self.mine([("Foo2", FOO), ("Bar", [BLR, BLR])])
        code, _, err = self.run_tool()
        self.assertEqual(code, 2)
        self.assertIn("Foo2", err)

    def test_compiled_symbol_override(self):
        self.target()
        self.mine([("Foo2", FOO), ("Bar", [BLR, BLR])])
        code, out, _ = self.run_tool("--compiled-symbol", "Foo2")
        self.assertEqual(code, 0, out)

    def test_object_not_built(self):
        self.target()
        code, _, err = self.run_tool()
        self.assertEqual(code, 2)
        self.assertIn("not built", err)

    def test_stale_object_is_rejected_without_flag(self):
        self.target(), self.mine()
        src = self.tmp / "src/unit/foo.c"
        src.parent.mkdir(parents=True)
        src.write_text("int x;")
        old = (self.tmp / "build/src/unit/foo.o")
        os.utime(old, (1, 1))
        code, _, err = self.run_tool()
        self.assertEqual(code, 2)
        self.assertIn("stale", err)
        code, _, _ = self.run_tool("--allow-stale")
        self.assertEqual(code, 0)

    def test_ninja_dry_run_reports_stale_headers(self):
        self.target(), self.mine()
        (self.tmp / "build.ninja").write_text("")
        bindir = self.tmp / "bin"
        bindir.mkdir()
        ninja = bindir / "ninja"
        ninja.write_text("#!/bin/sh\necho 'ninja: build stopped'\n")
        ninja.chmod(0o755)
        # `ninja -n` printing anything but "no work to do" means the object is out of date
        with mock.patch.dict(os.environ, {"PATH": f"{bindir}:{os.environ['PATH']}"}):
            code, _, err = self.run_tool()
        self.assertEqual(code, 2)
        self.assertIn("stale", err)

    def test_build_failure_is_an_error_with_compiler_output(self):
        self.target(), self.mine()
        (self.tmp / "build.ninja").write_text("")
        bindir = self.tmp / "bin"
        bindir.mkdir()
        ninja = bindir / "ninja"
        ninja.write_text("#!/bin/sh\necho 'foo.c:3: error: boom' >&2\nexit 1\n")
        ninja.chmod(0o755)
        with mock.patch.dict(os.environ, {"PATH": f"{bindir}:{os.environ['PATH']}"}):
            code, out, err = self.run_tool("--build")
        self.assertEqual(code, 2)
        self.assertIn("boom", err)
        self.assertEqual(out, "")

    def test_failed_build_never_reuses_old_object(self):
        self.target(), self.mine()
        (self.tmp / "build.ninja").write_text("")
        bindir = self.tmp / "bin"
        bindir.mkdir()
        ninja = bindir / "ninja"
        ninja.write_text("#!/bin/sh\nexit 1\n")
        ninja.chmod(0o755)
        with mock.patch.dict(os.environ, {"PATH": f"{bindir}:{os.environ['PATH']}"}):
            code, out, _ = self.run_tool("--build")
        self.assertEqual(code, 2)
        self.assertNotIn("RESULT", out)

    def test_dtk_failure_does_not_change_the_verdict(self):
        self.target(), self.mine()
        dtk = self.tmp / "fake-dtk"
        dtk.write_text("#!/bin/sh\nexit 3\n")
        dtk.chmod(0o755)
        with mock.patch.object(vf, "DTK", dtk):
            out, err = io.StringIO(), io.StringIO()
            with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
                code = vf.main(["Foo", "--no-dol"])
        self.assertEqual(code, 0)
        self.assertIn("dtk disasm exited 3", out.getvalue())
        self.assertIn("RESULT: MATCH", out.getvalue())

    def test_unsupported_relocation_type_is_error(self):
        self.target()
        self.mine(relocs=[(8, 99, "ext_a", 0)])
        code, _, err = self.run_tool()
        self.assertEqual(code, 2)
        self.assertIn("unsupported relocation", err)

    def test_garbage_object_is_error(self):
        self.target()
        self.put("mine", b"not an elf at all" * 4)
        code, _, err = self.run_tool()
        self.assertEqual(code, 2)
        self.assertIn("not a 32-bit big-endian ELF", err)

    def test_target_size_disagreeing_with_symbols_is_error(self):
        self.target([("Foo", [LWZ, ADDI, BLR]), ("Bar", [BLR, BLR])], relocs=[])
        self.mine()
        code, _, err = self.run_tool()
        self.assertEqual(code, 2)
        self.assertIn("symbols.txt says", err)

    def test_no_shared_temp_file_is_used(self):
        self.assertNotIn("_verify_src", Path(vf.__file__).read_text())


# ------------------------------------------------- genuine ProDG + retail data

ROOT = Path(__file__).resolve().parent.parent
REAL = {
    "target": ROOT / "build/GL5E4F/obj/numath/nurand.o",
    "mine": ROOT / "build/GL5E4F/src/numath/nurand.o",
    "dol": ROOT / "orig/GL5E4F/sys/main.dol",
    "cc": ROOT / "build/compilers/ProDG/3.9.3",
}
HAVE_REAL = all(p.exists() for p in REAL.values()) and (ROOT / "build/tools/dtk_patched").exists()


def run_real(*args):
    return subprocess.run([sys.executable, str(ROOT / "tools/verify_fn.py"), *args],
                          cwd=ROOT, capture_output=True, text=True)


def prodg_compile(source: str, tmp: Path) -> Path:
    """Compile with the exact flags/toolchain ninja uses for numath/nurand.o."""
    lines = (ROOT / "build.ninja").read_text().splitlines()
    start = next(i for i, l in enumerate(lines)
                 if l.startswith("build build/GL5E4F/src/numath/nurand.o:"))
    cflags = next(l for l in lines[start:start + 8] if l.strip().startswith("cflags"))
    flags = cflags.split("=", 1)[1].replace("$", " ").split()
    src, out = tmp / "variant.c", tmp / "variant.o"
    src.write_text(source)
    subprocess.run([sys.executable, "tools/prodg_cc.py", "--compiler", "build/compilers/ProDG/3.9.3",
                    "--as-bin", "build/binutils/powerpc-eabi-as", "--dtk", "build/tools/dtk_patched",
                    "--wibo", "build/tools/wibo", "--dep", str(tmp / "variant.d"),
                    *flags, "-c", str(src), "-o", str(out)],
                   cwd=ROOT, check=True, capture_output=True, text=True)
    return out


class RetailInputsPresent(unittest.TestCase):
    @unittest.skipUnless(os.environ.get("VERIFY_FN_REQUIRE_RETAIL"), "only enforced in retail CI")
    def test_genuine_tests_are_not_silently_skipped(self):
        self.assertTrue(HAVE_REAL, "retail CI must run bash build.sh before the verifier tests")


@unittest.skipUnless(HAVE_REAL, "needs `bash build.sh` outputs and the retail DOL")
class GenuineProdgTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        subprocess.run(["ninja", "build/GL5E4F/src/numath/nurand.o"], cwd=ROOT, check=True,
                       capture_output=True)

    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp(prefix="verify_fn_real_"))
        self.addCleanup(shutil.rmtree, self.tmp, ignore_errors=True)

    def test_matched_nurand_functions_match_the_retail_target(self):
        for fn in ("NuRandInt", "NuRandIntPtr", "NuRandSeed", "NuRandSetSeed", "NuRand"):
            with self.subTest(fn=fn):
                r = run_real(fn, "--quiet")
                self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
                self.assertIn("retail DOL byte check: yes", r.stdout)
                self.assertIn("RESULT: MATCH", r.stdout)

    def test_opensaga_parser_and_file_helpers_match_strictly(self):
        units = {
            "nucore/nufile": ("NuFileReadInt", "NuFileReadShort", "NuFileReadChar", "NuFileReadWChar"),
            "nucore/nufile_size": ("NuFileOpenSize",),
            "nucore/nufile_exists": ("NuFileExists",),
            "nucore/nufpar": ("NuAllocFPar", "NuFParCreate", "NuFParPushCom",
                               "NuFParPopCom", "NuFParGetInt", "NuFParGetFloat"),
        }
        for unit, functions in units.items():
            for function in functions:
                with self.subTest(function=function):
                    result = run_real(function, "--unit", unit, "--build", "--strict-symbols", "--json")
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    self.assertEqual(json.loads(result.stdout)["verdict"], "MATCH")
            with self.subTest(unit=unit):
                result = subprocess.run([sys.executable, str(ROOT / "tools/verify_unit.py"), unit, "--build"],
                                        cwd=ROOT, capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_global_reference_is_verified_against_retail_data(self):
        r = run_real("NuRandInt", "--quiet", "--strict-symbols")
        self.assertEqual(r.returncode, 0, r.stdout)
        self.assertIn("verified: 4 bytes identical to retail", r.stdout)

    def test_function_checked_against_another_functions_code_fails(self):
        r = run_real("NuRandInt", "--quiet", "--compiled-symbol", "NuRandIntPtr")
        self.assertEqual(r.returncode, 1, r.stdout)

    def test_register_swap_in_genuine_object_is_caught(self):
        elf = vf.Elf(REAL["mine"])
        sym = elf.find_function("NuRand")
        data = bytearray(REAL["mine"].read_bytes())
        sec = elf.sections[sym.shndx]
        pos = sec.offset + sym.value + 4 * 3          # `lwz r0, 0(r8)` -> `lwz r0, 0(r9)`
        word = struct.unpack(">I", data[pos:pos + 4])[0]
        data[pos:pos + 4] = struct.pack(">I", (word & ~(0x1F << 16)) | (9 << 16))
        mutated = self.tmp / "mutated.o"
        mutated.write_bytes(bytes(data))
        r = run_real("NuRand", "--object", str(mutated), "--quiet", "--json")
        self.assertEqual(r.returncode, 1, r.stdout)
        report = json.loads(r.stdout)
        self.assertEqual(report["mnemonic_similarity"], report["target_insns"])
        self.assertLess(report["exact_insns"], report["target_insns"])

    def test_freshly_compiled_prodg_variants_are_distinguished(self):
        base = (ROOT / "src/numath/nurand.c").read_text()
        cases = {
            "identical": (base, 0),
            "constant": (base.replace("0x19660d", "0x19660e", 1), 1),
            "extra_work": (base.replace("fseed = seed;", "fseed = seed + 1;"), 1),
        }
        for label, (source, expected) in cases.items():
            with self.subTest(label=label):
                obj = prodg_compile(source, self.tmp)
                r = run_real("NuRandInt" if label != "extra_work" else "NuRandSeed",
                             "--object", str(obj), "--quiet")
                self.assertEqual(r.returncode, expected, r.stdout + r.stderr)

    def test_missing_symbol_and_bad_unit_are_errors_not_stale_results(self):
        self.assertEqual(run_real("NoSuchFunction").returncode, 2)
        r = run_real("NuRandInt", "--unit", "numath/numtx", "--quiet")
        self.assertEqual(r.returncode, 2)
        self.assertNotIn("RESULT", r.stdout)


if __name__ == "__main__":
    unittest.main()
