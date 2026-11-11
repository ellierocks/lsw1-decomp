import unittest
from types import SimpleNamespace as NS

from tools import verify_unit
from tools.verify_unit import prove_unit, split_ranges
from tools.verify_fn import Comparer, Report


class FakeElf:
    def __init__(self, data, relocs=(), symbols=(), size=None, section_type=1):
        self.sections = [NS(index=0, name='', flags=0, size=0, sh_type=0),
                         NS(index=1, name='.data', flags=3, size=len(data) if size is None else size,
                            sh_type=section_type)]
        self.data = data
        self.relocs = {1: list(relocs)}
        self.symbols = list(symbols)

    def section_bytes(self, section):
        return self.data


class Symbols:
    def addr(self, name):
        return {'external': 0x1020, '__start': 0x3000}.get(name)


class Retail:
    def __init__(self, data):
        self.data = data

    def read(self, address, size):
        return self.data if address == 0x1000 and size == len(self.data) else None


class VerifyUnitTests(unittest.TestCase):
    def setUp(self):
        self.ranges = {'.data': (0x1000, 0x1008)}
        self.sym = NS(shndx=0, name='external', value=0)
        self.rel = NS(offset=0, rtype=1, sym=0, addend=4)
        self.data = bytes(4) + b'ABCD'
        self.retail = Retail((0x1024).to_bytes(4, 'big') + b'ABCD')
        self.target = FakeElf(self.data, [self.rel], [self.sym])
        self.mine = FakeElf(self.data, [self.rel], [self.sym])

    def proof(self):
        return prove_unit(self.target, self.mine, self.ranges, Symbols(), self.retail)

    def test_identical_complete_unit_with_resolved_relocation(self):
        self.assertEqual(self.proof(), {'.data': 0x1000})

    def test_section_symbol_addend_resolving_to_same_address_is_accepted(self):
        self.mine.symbols = [NS(shndx=1, name='.data', value=0)]
        self.mine.relocs[1] = [NS(offset=0, rtype=1, sym=0, addend=0x24)]
        self.assertEqual(self.proof(), {'.data': 0x1000})
        self.mine.relocs[1] = [NS(offset=0, rtype=1, sym=0, addend=0x28)]
        self.assertIsNone(self.proof())

    def test_changed_bytes_and_relocation_are_rejected(self):
        self.mine.data = bytes(4) + b'ABCE'
        self.assertIsNone(self.proof())
        self.assertIn('bytes differ at +0x7', verify_unit.LAST_REFUSAL)
        self.mine.data = self.data
        self.mine.relocs[1] = [NS(offset=0, rtype=1, sym=0, addend=8)]
        self.assertIsNone(self.proof())
        self.assertIn('relocations differ at +0x0', verify_unit.LAST_REFUSAL)
        self.mine.relocs[1] = [NS(offset=0, rtype=6, sym=0, addend=4)]
        self.assertIsNone(self.proof())
        self.mine.relocs[1] = [NS(offset=4, rtype=1, sym=0, addend=4)]
        self.assertIsNone(self.proof())

    def test_stale_retail_unknown_symbol_and_incomplete_layout_are_rejected(self):
        self.retail = Retail(bytes(8))
        self.assertIsNone(self.proof())
        self.retail = Retail((0x1024).to_bytes(4, 'big') + b'ABCD')
        self.mine.symbols = [NS(shndx=0, name='unknown', value=0)]
        self.assertIsNone(self.proof())
        self.mine = FakeElf(self.data, [self.rel], [self.sym], size=4)
        self.assertIsNone(self.proof())
        self.mine = FakeElf(self.data, [self.rel], [self.sym], section_type=8)
        self.target = FakeElf(self.data, [self.rel], [self.sym], section_type=8)
        self.assertIsNone(self.proof())

    def test_address_taken_mapping_requires_proof_and_correct_offset(self):
        mine = NS(elf=self.mine, data_widths={})
        cmp = Comparer(Symbols(), self.retail, self.proof())
        cmp.pending_data[("local", (".data", 4), 0x1004)] = 0
        cmp.verify_data(mine, Report("test"))
        self.assertTrue(cmp.mappings["local"].startswith("verified:"))
        cmp = Comparer(Symbols(), self.retail, self.proof())
        cmp.pending_data[("local", (".data", 4), 0x1005)] = 0
        cmp.verify_data(mine, Report("test"))
        self.assertTrue(cmp.mappings["local"].startswith("unverified:"))

    def test_code_sda_and_branch_with_data_pointer_table(self):
        ranges = {'.text': (0x1000, 0x1008), '.data': (0x2000, 0x2004)}
        sections = [NS(index=0, name='', flags=0, size=0, sh_type=0),
                    NS(index=1, name='.text', flags=6, size=8, sh_type=1),
                    NS(index=2, name='.data', flags=3, size=4, sh_type=1)]
        syms = [NS(shndx=2, name='table', value=0), NS(shndx=0, name='external', value=0),
                NS(shndx=1, name='handler', value=0)]
        def elf(sda_offset):
            obj = NS(sections=sections, symbols=syms,
                     relocs={1: [NS(offset=sda_offset, rtype=109, sym=0, addend=0),
                                  NS(offset=4, rtype=10, sym=1, addend=0)],
                             2: [NS(offset=0, rtype=1, sym=2, addend=0)]})
            obj.section_bytes = lambda section: {1: bytes.fromhex('3860000048000000'),
                                                  2: bytes(4)}[section.index]
            return obj
        class UnitRetail:
            def read(self, address, size):
                return {0x1000: bytes.fromhex('386d12344800001c'),
                        0x2000: bytes.fromhex('00001000'),
                        0x3010: bytes.fromhex('3da0000061ad0dcc')}.get(address)
        target, mine, retail = elf(0), elf(2), UnitRetail()
        self.assertEqual(prove_unit(target, mine, ranges, Symbols(), retail),
                         {'.text': 0x1000, '.data': 0x2000})
        class WrongBranch(UnitRetail):
            def read(self, address, size):
                if address == 0x1000:
                    return bytes.fromhex('386d123448000020')
                return super().read(address, size)
        self.assertIsNone(prove_unit(target, mine, ranges, Symbols(), WrongBranch()))
        class WrongSDA(UnitRetail):
            def read(self, address, size):
                if address == 0x1000:
                    return bytes.fromhex('386d12384800001c')
                return super().read(address, size)
        self.assertIsNone(prove_unit(target, mine, ranges, Symbols(), WrongSDA()))
        mine.relocs[2][0].addend = 4
        self.assertIsNone(prove_unit(target, mine, ranges, Symbols(), retail))
        mine.relocs[2][0].addend = 0
        mine.relocs[1][1].rtype = 255
        self.assertIsNone(prove_unit(target, mine, ranges, Symbols(), retail))

    def test_self_branch_relocation_requires_assembled_destination(self):
        ranges = {'.text': (0x1000, 0x1008)}
        sections = [NS(index=0, name='', flags=0, size=0, sh_type=0),
                    NS(index=1, name='.text', flags=6, size=8, sh_type=1)]
        syms = [NS(shndx=1, name='entry', value=0)]

        def elf(code, relocs):
            obj = NS(sections=sections, symbols=syms, relocs={1: relocs})
            obj.section_bytes = lambda section: code
            return obj
        rel = NS(offset=4, rtype=11, sym=0, addend=0)
        target = elf(bytes.fromhex('7c0000004082' '0000'), [rel])
        retail = NS(read=lambda address, size: bytes.fromhex('7c0000004082fffc')
                    if address == 0x1000 else None)
        class Symbols1:
            def addr(self, name):
                return None
        good = elf(bytes.fromhex('7c0000004082fffc'), [])
        self.assertEqual(prove_unit(target, good, ranges, Symbols1(), retail), {'.text': 0x1000})
        bad = elf(bytes.fromhex('7c0000004082fff8'), [])
        self.assertIsNone(prove_unit(target, bad, ranges, Symbols1(), retail))

    def test_short_zero_alignment_tail_is_accepted_but_not_other_gaps(self):
        data = b'ABCD'
        ranges = {'.data': (0x1000, 0x1008)}
        target = FakeElf(data + bytes(4), size=8)
        mine = FakeElf(data, size=4)
        retail = Retail(data + bytes(4))
        self.assertEqual(prove_unit(target, mine, ranges, Symbols(), retail), {'.data': 0x1000})
        self.assertIsNone(prove_unit(target, mine, ranges, Symbols(), Retail(data + b'\0\0\0\1')))
        target = FakeElf(data + b'\0\0\0\1', size=8)
        self.assertIsNone(prove_unit(target, mine, ranges, Symbols(), Retail(data + b'\0\0\0\1')))
        target = FakeElf(data + bytes(4), size=8)
        mine = FakeElf(b'ABCE', size=4)
        self.assertIsNone(prove_unit(target, mine, ranges, Symbols(), retail))
        big = {'.data': (0x1000, 0x1000 + 4 + 32)}
        target = FakeElf(data + bytes(32), size=36)
        self.assertIsNone(prove_unit(target, FakeElf(data, size=4), big, Symbols(),
                                     Retail(data + bytes(32))))

    def bss_elf(self, size, objects, name='.bss'):
        elf = FakeElf(b'', size=size, section_type=8)
        elf.sections[1] = NS(index=1, name=name, flags=3, size=size, sh_type=8)
        elf.relocs = {}
        elf.symbols = [NS(index=i, name=n, value=o, size=s, sym_type=1, bind=0, shndx=1)
                       for i, (o, s, n) in enumerate(objects)]
        return elf

    def bss_proof(self, target, mine, ranges=None):
        return prove_unit(target, mine, ranges or {'.bss': (0x2000, 0x2000 + target.sections[1].size)},
                          Symbols(), Retail(b''))

    def test_bss_is_proved_by_layout_not_by_zero_contents(self):
        objects = [(0, 8, 'a'), (8, 0x20, 'b')]
        self.assertEqual(self.bss_proof(self.bss_elf(0x28, objects), self.bss_elf(0x28, objects)),
                         {'.bss': 0x2000})
        # A retail symbol may exceed its donor size only by alignment padding before the next object.
        target = self.bss_elf(0x28, [(0, 8, 'a'), (8, 0x18, 'b'), (0x20, 8, 'c')])
        mine = self.bss_elf(0x28, [(0, 8, 'a'), (8, 0x16, 'b'), (0x20, 8, 'c')])
        self.assertEqual(self.bss_proof(target, mine), {'.bss': 0x2000})

    def test_sized_bss_reference_requires_complete_layout_proof(self):
        objects = [(0, 4, 'a'), (4, 4, 'b')]
        target = self.bss_elf(8, objects)
        mine = self.bss_elf(8, objects)
        # ELF's NOBITS reader supplies zeroes, but the DOL has no BSS bytes.
        mine.section_bytes = lambda section: bytes(section.size)
        retail = NS(read=lambda address, size: None)
        func = NS(elf=mine, data_widths={('.bss', 4): 4})
        for proof, address, expected in [
            (self.bss_proof(target, mine), 0x2004, 'verified:'),
            (None, 0x2004, 'unverified:'),
            (self.bss_proof(target, mine), 0x2000, 'unverified:'),
            (self.bss_proof(target, self.bss_elf(8, [(0, 8, 'a')])), 0x2004, 'unverified:'),
        ]:
            with self.subTest(proof=proof, address=address):
                cmp = Comparer(Symbols(), retail, proof)
                cmp.pending_data[('b', ('.bss', 4), address)] = 0
                cmp.verify_data(func, Report('test'))
                self.assertTrue(cmp.mappings['b'].startswith(expected), cmp.mappings)

    def test_bss_gap_symbols_are_accepted_only_as_alignment_padding(self):
        mine = self.bss_elf(0x28, [(0, 0xc, 'a'), (0x10, 0x18, 'b')])
        ok = self.bss_elf(0x28, [(0, 0xc, 'a'), (0xc, 4, 'gap_04_0000200C_bss'), (0x10, 0x18, 'b')])
        self.assertEqual(self.bss_proof(ok, mine), {'.bss': 0x2000})
        # A gap that does not follow a compiled object and end at the next one is unexplained.
        far = self.bss_elf(0x28, [(0, 0x8, 'a'), (0x8, 8, 'gap_04_00002008_bss'), (0x10, 0x18, 'b')])
        self.assertIsNone(self.bss_proof(far, self.bss_elf(0x28, [(0, 0xc, 'a'), (0x10, 0x18, 'b')])))
        big = self.bss_elf(0x80, [(0, 8, 'a'), (8, 0x40, 'gap_04_00002008_bss'), (0x48, 0x38, 'b')])
        self.assertIsNone(self.bss_proof(big, self.bss_elf(0x80, [(0, 8, 'a'), (0x48, 0x38, 'b')])))
        self.assertIn('no compiled counterpart', verify_unit.LAST_REFUSAL)

    def test_mwcc_section_anchor_is_not_an_object(self):
        objects = [(0, 8, 'a'), (8, 0x20, 'b')]
        mine = self.bss_elf(0x28, objects)
        mine.symbols.append(NS(index=9, name='...bss.0', value=0, size=0, sym_type=0, bind=0, shndx=1))
        self.assertEqual(self.bss_proof(self.bss_elf(0x28, objects), mine), {'.bss': 0x2000})
        mine.symbols.append(NS(index=10, name='nosize', value=0, size=0, sym_type=1, bind=0, shndx=1))
        self.assertIsNone(self.bss_proof(self.bss_elf(0x28, objects), mine))

    def test_bss_layout_mismatches_are_rejected(self):
        objects = [(0, 8, 'a'), (8, 0x20, 'b')]
        target = self.bss_elf(0x28, objects)
        cases = {
            'compiled section is 0x20 bytes': self.bss_elf(0x20, [(0, 8, 'a'), (8, 0x18, 'b')]),
            'no retail symbol at that offset': self.bss_elf(0x28, [(0, 8, 'a'), (0x10, 0x18, 'b')]),
            'is b in the retail split': self.bss_elf(0x28, [(0, 8, 'a'), (8, 0x20, 'c')]),
            'not alignment padding': self.bss_elf(0x28, [(0, 8, 'a'), (8, 0x10, 'b'), (0x18, 8, 'x')]),
            'no compiled counterpart': self.bss_elf(0x28, [(0, 8, 'a')]),
            'defines no symbol': self.bss_elf(0x28, []),
        }
        for reason, mine in cases.items():
            with self.subTest(reason):
                self.assertIsNone(self.bss_proof(target, mine))
                self.assertIn(reason, verify_unit.LAST_REFUSAL)
        # Padding beyond the alignment bound, an unexplained retail gap and a retail extra symbol.
        big = self.bss_elf(0x80, [(0, 8, 'a'), (8, 0x78, 'b')])
        self.assertIsNone(self.bss_proof(big, self.bss_elf(0x80, [(0, 8, 'a'), (8, 0x20, 'b'), (0x28, 0x58, 'c')])))
        self.assertIsNone(self.bss_proof(self.bss_elf(0x28, [(0, 8, 'a'), (0x10, 0x18, 'b')]),
                                         self.bss_elf(0x28, [(0, 8, 'a'), (0x10, 0x18, 'b')])))
        self.assertIn('unexplained gap', verify_unit.LAST_REFUSAL)
        self.assertIsNone(self.bss_proof(self.bss_elf(0x28, objects + [(0x28, 8, 'z')]),
                                         self.bss_elf(0x28, objects)))
        self.assertIsNone(self.bss_proof(self.bss_elf(0x28, objects), self.bss_elf(0x28, objects),
                                         {'.bss': (0x2000, 0x2030)}))

    def test_bss_symbols_must_be_linked_at_their_retail_addresses(self):
        mine = self.bss_elf(0x28, [(0, 8, 'a'), (8, 0x20, 'b')])
        ranges = {'.bss': (0x2000, 0x2028)}
        self.assertIsNone(verify_unit.check_linked_bss(ranges, mine, {'a': {0x2000}, 'b': {0x2008, 0x9}}))
        self.assertIn('b is not linked at retail 0x00002008',
                      verify_unit.check_linked_bss(ranges, mine, {'a': {0x2000}, 'b': {0x2010}}))
        self.assertIn('nowhere', verify_unit.check_linked_bss(ranges, mine, {'a': {0x2000}}))

    def test_split_parser_requires_unambiguous_ranges(self):
        text = 'unit.c:\n  .text start:0x1000 end:0x1004\n  .data start:0x2000 end:0x2008\nother.c:\n  .text start:0x3000 end:0x3004\n'
        self.assertEqual(split_ranges(text, 'unit'), {'.text': (0x1000, 0x1004), '.data': (0x2000, 0x2008)})
        self.assertIsNone(split_ranges(text.replace('0x2000', '0x1002'), 'unit'))
        self.assertIsNone(split_ranges(text + 'unit.c:\n  .data start:0x2000 end:0x2008\n', 'unit'))


if __name__ == '__main__':
    unittest.main()
