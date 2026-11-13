import struct
import tempfile
import unittest
from pathlib import Path

from tools import ghidra_decompile as gd


def synthetic_dol() -> bytes:
    """One text segment (lis/ori r13 and lis/addi r2, then blr), one data segment and bss."""
    code = struct.pack(">5I", 0x3DA08040, 0x61ADFA40, 0x3C408042, 0x3842FA40, 0x4E800020)
    data = b"\x11" * 0x20
    header = bytearray(0x100)
    struct.pack_into(">I", header, 0x00, 0x100)                 # text0 offset
    struct.pack_into(">I", header, 0x1C, 0x100 + len(code))     # data0 offset
    struct.pack_into(">I", header, 0x48, 0x80003100)            # text0 address
    struct.pack_into(">I", header, 0x64, 0x80004000)            # data0 address
    struct.pack_into(">I", header, 0x90, len(code))
    struct.pack_into(">I", header, 0xAC, len(data))
    struct.pack_into(">III", header, 0xD8, 0x80004000, 0x100, 0x80003100)  # bss covers data
    return bytes(header) + code + data


SYMBOLS = """\
__start = .init:0x80003100; // type:function size:0x14 scope:global
lbl_80004000 = .sdata:0x80004000; // type:object size:0x4
lbl_80004080 = .sbss:0x80004080; // type:object size:0x4 scope:local
"""


def sections(elf: bytes):
    shoff, = struct.unpack_from(">I", elf, 0x20)
    shnum, shstrndx = struct.unpack_from(">HH", elf, 0x30)
    headers = [struct.unpack_from(">10I", elf, shoff + 40 * i) for i in range(shnum)]
    strtab = headers[shstrndx]
    name = lambda off: elf[strtab[4] + off:elf.index(b"\0", strtab[4] + off)].decode()
    return {name(h[0]): h for h in headers[1:]}


def symbols(elf: bytes):
    secs = sections(elf)
    symtab, strtab = secs[".symtab"], secs[".strtab"]
    out = {}
    for i in range(1, symtab[5] // 16):
        nm, value, size, info, _, shndx = struct.unpack_from(">IIIBBH", elf, symtab[4] + 16 * i)
        name = elf[strtab[4] + nm:elf.index(b"\0", strtab[4] + nm)].decode()
        out[name] = (value, size, info, shndx)
    return out, symtab[7]


class BuildElfTest(unittest.TestCase):
    def build(self) -> bytes:
        with tempfile.TemporaryDirectory() as tmp:
            dol, syms = Path(tmp) / "main.dol", Path(tmp) / "symbols.txt"
            dol.write_bytes(synthetic_dol())
            syms.write_text(SYMBOLS)
            return gd.build_elf(dol, syms)

    def test_sections_named_from_symbols_and_bss_split(self):
        secs = sections(self.build())
        self.assertEqual(secs[".init"][3], 0x80003100)
        self.assertEqual(secs[".sdata"][1], 1)          # PROGBITS, inside the DOL bss range
        self.assertEqual(secs[".sbss"][1], 8)           # NOBITS gap after it
        self.assertEqual((secs[".sbss"][3], secs[".sbss"][5]), (0x80004020, 0xE0))

    def test_symbols_and_sda_bases(self):
        elf = self.build()
        self.assertEqual(struct.unpack_from(">I", elf, 0x24)[0], 0)  # no EF_PPC_EMB (REA refuses it)
        syms, first_global = symbols(elf)
        self.assertEqual(syms["__start"][:3], (0x80003100, 0x14, 0x12))  # GLOBAL FUNC
        self.assertEqual(syms["lbl_80004080"][2] >> 4, 0)                # local binding
        self.assertEqual(first_global, 2)                                # null + one local
        self.assertEqual(syms["_SDA_BASE_"][0], 0x8040FA40)
        self.assertEqual(syms["_SDA2_BASE_"][0], 0x8041FA40)


if __name__ == "__main__":
    unittest.main()


class SeedTest(unittest.TestCase):
    def test_seed_key_ignores_edits_outside_the_import(self):
        """Editing this tool must not orphan the shared seed (it forced a minute-long re-import)."""
        source = Path(gd.__file__).read_text()
        self.assertNotIn("[Path(__file__)]", source)
        self.assertIn("IMPORT_KEY", source)

    def test_auto_setup_never_starts_a_full_import(self):
        from unittest import mock
        with tempfile.TemporaryDirectory() as tmp, \
                mock.patch.object(gd, "project_ready", return_value=False), \
                mock.patch.object(gd, "find_analyze_headless", return_value="/bin/true"), \
                mock.patch.object(gd, "check_location"), \
                mock.patch.object(gd, "seed_dir", return_value=Path(tmp) / "absent"), \
                mock.patch.object(gd, "DOL", Path(__file__)), \
                mock.patch.object(gd.subprocess, "run") as run:
            self.assertFalse(gd.auto_setup())
            run.assert_not_called()

    def test_auto_setup_respects_opt_out(self):
        from unittest import mock
        with mock.patch.object(gd, "project_ready", return_value=False), \
                mock.patch.dict(gd.os.environ, {"LSW1_NO_GHIDRA": "1"}), \
                mock.patch.object(gd.subprocess, "run") as run:
            self.assertFalse(gd.auto_setup())
            run.assert_not_called()
