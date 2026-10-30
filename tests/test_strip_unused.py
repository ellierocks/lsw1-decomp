import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from tools.strip_unused import Elf


SCRIPT = Path(__file__).resolve().parents[1] / "tools" / "strip_unused.py"


def make_elf(path, symbols, text=b"", data=b"", relocations=(), data_name=".data"):
    """Build a minimal big-endian ELF32 with text, data, RELA, and MWCC comments."""
    names = ["", ".text", data_name, ".rela.text", ".symtab", ".strtab", ".comment", ".shstrtab"]
    shstr = b"\0"
    name_offsets = [0]
    for name in names[1:]:
        name_offsets.append(len(shstr))
        shstr += name.encode() + b"\0"
    strings = b"\0"
    entries = [struct.pack(">IIIBBH", 0, 0, 0, 0, 0, 0)]
    for name, value, size, kind, section in symbols:
        offset = len(strings)
        strings += name.encode() + b"\0"
        entries.append(struct.pack(">IIIBBH", offset, value, size, 0x10 | kind, 0, section))
    symtab = b"".join(entries)
    rela = b"".join(struct.pack(">IIi", off, sym << 8 | 1, addend)
                    for off, sym, addend in relocations)
    comment = b"HEADER!!" + b"".join(bytes([i]) * 8 for i in range(len(entries)))
    contents = [b"", text, data, rela, symtab, strings, comment, shstr]
    types = [0, 1, 1, 4, 2, 3, 1, 3]
    links = [0, 0, 0, 4, 5, 0, 0, 0]
    infos = [0, 0, 0, 1, 1, 0, 0, 0]
    header = bytearray(52)
    header[:16] = b"\x7fELF\x01\x02\x01" + bytes(9)
    struct.pack_into(">HHIIIIIHHHHHH", header, 16, 1, 20, 1, 0, 0, 0, 0, 52, 0, 0, 40, len(names), 7)
    output = header
    sections = [[0] * 10]
    for i in range(1, len(names)):
        output += bytes((-len(output)) % 4)
        offset = len(output)
        output += contents[i]
        sections.append([name_offsets[i], types[i], 6 if i == 1 else 3 if i == 2 else 0, 0, offset,
                         len(contents[i]), links[i], infos[i], 4, 16 if i == 4 else 12 if i == 3 else 0])
    output += bytes((-len(output)) % 4)
    struct.pack_into(">I", output, 32, len(output))
    for section in sections:
        output += struct.pack(">10I", *section)
    path.write_bytes(output)


def symbols(elf):
    index = elf.index(".symtab")
    strings = elf.contents[elf.sections[index][6]]
    result = []
    for offset in range(0, len(elf.contents[index]), 16):
        entry = struct.unpack_from(">IIIBBH", elf.contents[index], offset)
        name = strings[entry[0]:strings.index(0, entry[0])].decode()
        result.append((name, *entry[1:]))
    return result


class StripUnusedTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.target = Path(self.tmp.name) / "target.o"
        self.obj = Path(self.tmp.name) / "source.o"

    def run_strip(self):
        return subprocess.run([sys.executable, str(SCRIPT), "--target", str(self.target), str(self.obj)],
                              capture_output=True, text=True)

    def test_removes_function_and_shifts_relocation_and_comment_records(self):
        source = [("dead", 0, 4, 2, 1), ("kept", 4, 4, 2, 1), ("external", 0, 0, 0, 0)]
        make_elf(self.obj, source, text=b"DEADKEEP", relocations=[(4, 3, 0)])
        make_elf(self.target, [("kept", 0, 4, 2, 1)], text=b"KEEP")
        result = self.run_strip()
        self.assertEqual(result.returncode, 0, result.stderr)
        elf = Elf(self.obj.read_bytes())
        self.assertEqual(elf.contents[elf.index(".text")], b"KEEP")
        self.assertEqual([(s[0], s[1]) for s in symbols(elf)], [("", 0), ("kept", 0), ("external", 0)])
        self.assertEqual(struct.unpack(">IIi", elf.contents[elf.index(".rela.text")]), (0, 2 << 8 | 1, 0))
        self.assertEqual(elf.contents[elf.index(".comment")], b"HEADER!!" + bytes(8) + bytes([2]) * 8 + bytes([3]) * 8)

    def test_removed_data_leaves_remainder(self):
        source = [("dead_data", 0, 12, 1, 2), ("kept_data", 12, 4, 1, 2)]
        make_elf(self.obj, source, data=b"abcdefghijklKEEP")
        make_elf(self.target, [("kept_data", 0, 4, 1, 2)], data=b"KEEP")
        result = self.run_strip()
        self.assertEqual(result.returncode, 0, result.stderr)
        elf = Elf(self.obj.read_bytes())
        self.assertEqual(elf.contents[elf.index(".data")], b"ijklKEEP")
        self.assertEqual([(s[0], s[1]) for s in symbols(elf)], [("", 0), ("kept_data", 4)])

    def test_small_constants_are_readonly_even_without_stripping(self):
        definitions = [("constant", 0, 4, 1, 2)]
        make_elf(self.obj, definitions, data=b"KEEP", data_name=".sdata2")
        make_elf(self.target, definitions, data=b"KEEP", data_name=".sdata2")
        result = self.run_strip()
        self.assertEqual(result.returncode, 0, result.stderr)
        elf = Elf(self.obj.read_bytes())
        index = elf.index(".sdata2")
        self.assertEqual(elf.sections[index][2], 2)
        self.assertEqual(elf.contents[index], b"KEEP")

    def test_surviving_reference_to_removed_symbol_fails(self):
        source = [("dead", 0, 4, 2, 1), ("kept", 4, 4, 2, 1)]
        make_elf(self.obj, source, text=b"DEADKEEP", relocations=[(4, 1, 0)])
        make_elf(self.target, [("kept", 0, 4, 2, 1)], text=b"KEEP")
        before = self.obj.read_bytes()
        result = self.run_strip()
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("still references removed symbol dead", result.stderr)
        self.assertEqual(self.obj.read_bytes(), before)


if __name__ == "__main__":
    unittest.main()
