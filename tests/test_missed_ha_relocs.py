"""Control-flow proof and conservative rejection of missing @ha relocations."""

import unittest

from tools import missed_ha_relocs as ha


START = 0x80010000
SYMBOLS = {"constant": 0x8019B914, "other": 0x8019B918}
LIS = (0x3D60801A, "lis r11, 0x801a")
LOAD = (0xC02BB914, "lfs f1, constant@l(r11)")
OTHER = (0xC02BB918, "lfs f1, other@l(r11)")
RETURN = (0x4E800020, "blr")
NOP = (0x60000000, "nop")


def branch(source_index, target_index, conditional=False, link=False):
    displacement = (target_index - source_index) * 4
    word = (0x41820000 | (displacement & 0xFFFC) if conditional
            else 0x48000000 | (displacement & 0x3FFFFFC)) | int(link)
    return word, f"{'beq' if conditional else 'bl' if link else 'b'} .L_target"


def assembly(rows, name="example", section=".text"):
    lines = [section, f".fn {name}, global"]
    for index, (word, instruction) in enumerate(rows):
        raw = " ".join(f"{byte:02X}" for byte in word.to_bytes(4, "big"))
        lines.append(f"/* {START + index * 4:08X} {index * 4:08X}  {raw} */\t{instruction}")
    return "\n".join([*lines, f".endfn {name}", ""])


def proposals(rows, symbols=None):
    function, = ha.parse_asm(assembly(rows))
    return ha.propose(function, SYMBOLS if symbols is None else symbols)


class MissedHaTests(unittest.TestCase):
    def test_shared_tail_uses_actual_branch_destination(self):
        result, = proposals([LIS, branch(1, 4), RETURN, OTHER, NOP, LOAD, RETURN])
        self.assertEqual(result.source, START)
        self.assertEqual(result.target, "constant")
        self.assertEqual(result.uses, (START + 20,))
        self.assertIn("- source: .text:0x80010000\n  type: ha\n  target: constant", ha.render([result]))

    def test_all_conditional_successors_same_target(self):
        result, = proposals([LIS, branch(1, 4, conditional=True), LOAD, RETURN, LOAD, RETURN])
        self.assertEqual(result.uses, (START + 8, START + 16))

    def test_conditional_successor_cannot_exit_clobber_or_use_other_target(self):
        for tail in [RETURN, (0x39600000, "li r11, 0x0"), OTHER]:
            with self.subTest(tail=tail):
                self.assertEqual(proposals([LIS, branch(1, 4, conditional=True), LOAD, RETURN, tail]), [])

    def test_backward_branch_can_reach_a_low_use(self):
        result, = proposals([branch(0, 3), LOAD, RETURN, LIS, branch(4, 1)])
        self.assertEqual(result.uses, (START + 4,))

    def test_cycles_including_a_conditional_exit_fail(self):
        self.assertEqual(proposals([LIS, branch(1, 1)]), [])
        self.assertEqual(proposals([LIS, branch(1, 1, conditional=True), LOAD]), [])

    def test_calls_unknown_instructions_and_indirect_branches_fail(self):
        for instruction in [branch(1, 3, link=True), (0x4E800421, "bctrl"),
                            (0x4E800420, "bctr"), (0xFFFFFFFF, "unknown"), RETURN,
                            (0xB9410000, "lmw r10, 0x0(r1)"),
                            (0x7D412CAA, "lswi r10, r5, 5")]:
            with self.subTest(instruction=instruction):
                self.assertEqual(proposals([LIS, instruction, LOAD, RETURN]), [])

    def test_unrelocated_read_or_write_and_update_addressing_fail(self):
        for instruction in [(0x7D695B78, "mr r9, r11"),
                            (0x81610000, "lwz r11, 0x0(r1)"),
                            (0x852B0004, "lwzu r9, 0x4(r11)"),
                            (0x956B0004, "stwu r11, 0x4(r11)"),
                            (0xC02BB914, "lfs f1, -0x46ec(r11)")]:
            with self.subTest(instruction=instruction):
                self.assertEqual(proposals([LIS, instruction, LOAD, RETURN]), [])

    def test_unrelated_loads_and_float_instructions_do_not_clobber_gpr(self):
        result = proposals([LIS, (0xC16DB914, "lfs f11, constant@sda21(r0)"),
                            (0x38A000FF, "li r5, 0xff"),
                            (0xFCA02090, "fmr f5, f4"), LOAD])
        self.assertEqual(len(result), 1)

    def test_high_adjustment_and_encoded_low_must_match(self):
        self.assertEqual(len(proposals([LIS, LOAD])), 1)  # carry from bit 15
        self.assertEqual(proposals([(0x3D608019, "lis r11, 0x8019"), LOAD]), [])
        self.assertEqual(proposals([LIS, (0xC02BB918, "lfs f1, constant@l(r11)")]), [])
        self.assertEqual(proposals([LIS, LOAD], {}), [])

    def test_addi_addend_signed_lis_and_yaml(self):
        result, = proposals([(0x3D60801A, "lis r11, -0x7fe6"),
                             (0x386BB918, "addi r3, r11, constant+0x4@l")])
        self.assertEqual(result.addend, 4)
        self.assertIn("  addend: 4\n", ha.render([result]))
        negative, = proposals([LIS, (0x386BB910, "addi r3, r11, constant-0x4@l")])
        self.assertEqual(negative.addend, -4)

    def test_different_addends_are_ambiguous_even_with_the_same_high(self):
        self.assertEqual(proposals([
            LIS, branch(1, 4, conditional=True), LOAD, RETURN,
            (0xC02BB918, "lfs f1, constant+0x4@l(r11)"), RETURN]), [])

    def test_existing_relocs_wrong_base_unsigned_low_and_r0_fail(self):
        cases = [ [(0x3D60801A, "lis r11, constant@ha"), LOAD],
                  [LIS, (0xC02AB914, "lfs f1, constant@l(r10)")],
                  [LIS, (0x6163B914, "ori r3, r11, constant@l")],
                  [(0x3C00801A, "lis r0, 0x801a"), (0xC020B914, "lfs f1, constant@l(r0)")] ]
        for rows in cases:
            with self.subTest(rows=rows):
                self.assertEqual(proposals(rows), [])

    def test_no_cross_function_fallthrough_or_jump(self):
        functions = ha.parse_asm(assembly([LIS], "first") + assembly([LOAD], "second"))
        self.assertEqual(ha.propose(functions[0], SYMBOLS), [])
        self.assertEqual(proposals([LIS, branch(1, 99)]), [])
        self.assertEqual(proposals([LIS]), [])

    def test_duplicate_symbol_names_are_unresolved(self):
        symbols = ha.parse_symbols("constant = .rodata:0x8019B914;\n"
                                   "constant = .data:0x8019B914;\n"
                                   "unique = .rodata:0x8019B918;\n")
        self.assertEqual(symbols, {"unique": 0x8019B918})

    def test_no_proposals_is_valid_empty_yaml(self):
        self.assertEqual(ha.render([]), "add_relocations: []\n")


if __name__ == "__main__":
    unittest.main()
