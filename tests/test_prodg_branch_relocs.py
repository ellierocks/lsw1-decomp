import struct
import tempfile
import unittest
from pathlib import Path

from tests.test_strip_unused import make_elf
from tools.prodg_branch_relocs import add_branch_relocs
from tools.strip_unused import Elf


def branch(disp, link=True):
    return struct.pack(">I", 0x48000000 | (disp & 0x03FFFFFC) | int(link))


BLR = struct.pack(">I", 0x4E800020)
LOCAL = 0


class ProdgBranchRelocTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.path = Path(self.tmp.name) / "obj.o"

    def relocations(self, elf, name=".rela.text"):
        body = elf.contents[elf.index(name)]
        return [struct.unpack(">IIi", body[o:o + 12]) for o in range(0, len(body), 12)]

    def test_resolved_call_to_static_function_gets_rel24(self):
        # fread.c: `bl fread_copy` assembles with no relocation; DTK's target has one.
        symbols = [("helper", 0, 4, 2, 1, LOCAL), ("caller", 4, 12, 2, 1), ("external", 0, 0, 0, 0)]
        text = BLR + branch(-4) + branch(0) + branch(-12, link=False)
        make_elf(self.path, symbols, text=text, relocations=[(8, 3, 0)])
        elf = Elf(self.path.read_bytes())
        self.assertEqual(add_branch_relocs(elf), 2)
        self.assertEqual(self.relocations(elf), [(4, 1 << 8 | 10, 0), (8, 3 << 8 | 1, 0), (12, 1 << 8 | 10, 0)])
        self.assertEqual(elf.contents[elf.index(".text")], text)

    def test_loop_to_own_first_instruction_is_left_alone(self):
        symbols = [("spin", 0, 8, 2, 1, LOCAL)]
        make_elf(self.path, symbols, text=BLR + branch(-4, link=False))
        elf = Elf(self.path.read_bytes())
        self.assertEqual(add_branch_relocs(elf), 0)
        self.assertEqual(self.relocations(elf), [])

    def test_branch_into_a_function_body_is_left_alone(self):
        symbols = [("caller", 0, 8, 2, 1, LOCAL)]
        make_elf(self.path, symbols, text=branch(4) + BLR)
        elf = Elf(self.path.read_bytes())
        self.assertEqual(add_branch_relocs(elf), 0)

    def test_text_without_relocation_section_gets_one(self):
        symbols = [("helper", 0, 4, 2, 1, LOCAL), ("caller", 4, 4, 2, 1)]
        make_elf(self.path, symbols, text=BLR + branch(-4))
        data = bytearray(self.path.read_bytes())
        elf = Elf(bytes(data))
        rela = elf.index(".rela.text")
        # Turn the empty RELA section into PROGBITS, as if the assembler emitted none.
        shoff, = struct.unpack(">I", data[32:36])
        struct.pack_into(">I", data, shoff + rela * 40 + 4, 1)
        self.path.write_bytes(data)
        elf = Elf(self.path.read_bytes())
        self.assertEqual(add_branch_relocs(elf), 1)
        elf.write(self.path)
        elf = Elf(self.path.read_bytes())
        added = len(elf.sections) - 1
        self.assertEqual(elf.sections[added][1], 4)
        self.assertEqual(elf.sections[added][7], elf.index(".text"))
        body = elf.contents[added]
        self.assertEqual(struct.unpack(">IIi", body), (4, 1 << 8 | 10, 0))


if __name__ == "__main__":
    unittest.main()
