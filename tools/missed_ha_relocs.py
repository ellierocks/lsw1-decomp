#!/usr/bin/env python3
"""Propose missing DTK lis @ha relocations from generated function assembly.

Run after build.sh: python3 tools/missed_ha_relocs.py > work/missing-ha.yml
The output is a proposal for manual review, never an automatic config edit.
Unknown instructions, calls, indirect control flow, cycles, exits, register
reads without an @l relocation, and clobbers before the use all fail closed.
Every successor must reach a first @l use of the identical symbol/addend.

Schema: pinned DTK e4219e7, src/cmd/dol.rs AddRelocationConfig and
src/obj/relocations.rs ObjRelocKind. DTK uses the instruction address (not
the ELF halfword offset) for source, and "ha" for PpcAddr16Ha.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
INSTRUCTION = re.compile(
    r"^/\* ([0-9A-Fa-f]{8}) [0-9A-Fa-f]{8}  "
    r"((?:[0-9A-Fa-f]{2} ){3}[0-9A-Fa-f]{2}) \*/\s*(\S+)\s*(.*?)\s*$")
SYMBOL = re.compile(r"^(\S+)\s*=\s*(\.[\w.]+):0x([0-9A-Fa-f]+);")
EXPRESSION = r"([\w.$]+)(?:([+-])((?:0x[\da-fA-F]+)|\d+))?"
LOW = re.compile(EXPRESSION + r"@l(?:\((r\d+)\))?$")
LIS = re.compile(r"(r\d+),\s*(-?(?:0x[\da-fA-F]+|\d+))$")

# Explicitly model only instructions without implicit GPR effects. Merely
# checking the first operand misses update loads/stores, lmw and string loads.
SAFE = set("""
    add add. addc addc. adde adde. addi addic addic. addis addme addze
    subf subf. subfc subfe subfic subfme subfze neg neg.
    mullw mullw. mulli mulhw mulhwu divw divwu
    and and. andc andc. andi. andis. or or. orc xor xor. xori xoris ori oris
    nand nor eqv slw srw sraw srawi rlwinm rlwinm. rlwimi rlwnm
    clrlwi clrlwi. clrrwi rotlw rotlwi extsh extsh. extsb extsb.
    li lis mr cmpwi cmplwi cmpw cmplw
    lbz lbzu lbzx lbzux lhz lhzu lhzx lhzux lha lhau lhax lhaux
    lwz lwzu lwzx lwzux stb stbu stbx stbux sth sthu sthx sthux
    stw stwu stwx stwux lfs lfsu lfsx lfsux lfd lfdu lfdx lfdux
    stfs stfsu stfsx stfsux stfd stfdu stfdx stfdux
    fmr fneg fabs fnabs frsp fctiwz fcmpu fcmpo
    fadd fadds fsub fsubs fmul fmuls fdiv fdivs fmadd fmadds
    fmsub fmsubs fnmadd fnmadds fnmsub fnmsubs fsel fres frsqrte
    mflr mtlr mfctr mtctr mfcr mtcrf mfxer mtxer
    cror crand crxor crnor crandc creqv crorc crnand crclr crset crmove
    nop sync isync eieio
""".split())
MEMORY_LOW = set("lbz lhz lha lwz stb sth stw lfs lfd stfs stfd".split())


@dataclass(frozen=True)
class Instruction:
    address: int
    word: int
    mnemonic: str
    operands: str


@dataclass
class Function:
    name: str
    section: str
    instructions: list[Instruction]


@dataclass(frozen=True)
class Proposal:
    section: str
    source: int
    target: str
    addend: int
    function: str
    uses: tuple[int, ...]


def parse_asm(text: str) -> list[Function]:
    functions = []
    section = ""
    current = None
    for line in text.splitlines():
        if line.startswith(".section "):
            section = line.split()[1].split(",")[0]
        elif line.strip() in {".text", ".init"}:
            section = line.strip()
        elif line.startswith(".fn "):
            current = Function(line.split()[1].rstrip(","), section, [])
        elif line.startswith(".endfn"):
            if current is not None:
                functions.append(current)
            current = None
        elif current is not None and (match := INSTRUCTION.match(line)):
            address, raw, mnemonic, operands = match.groups()
            current.instructions.append(Instruction(
                int(address, 16), int(raw.replace(" ", ""), 16), mnemonic, operands))
    return functions


def parse_symbols(text: str) -> dict[str, int]:
    symbols = {}
    duplicates = set()
    for line in text.splitlines():
        if match := SYMBOL.match(line):
            name, _, address = match.groups()
            if name in symbols:
                duplicates.add(name)
            symbols[name] = int(address, 16)
    return {name: address for name, address in symbols.items() if name not in duplicates}


def signed(value: int, bits: int) -> int:
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def low_use(insn: Instruction, register: str, symbols: dict[str, int]):
    """Recognize only signed-low address formation, checking the encoded fields."""
    operands = [part.strip() for part in insn.operands.split(",")]
    if insn.mnemonic in MEMORY_LOW and len(operands) == 2:
        expression = operands[1]
        base = register
    elif insn.mnemonic == "addi" and len(operands) == 3:
        expression = operands[2]
        base = operands[1]
    else:
        return None
    match = LOW.fullmatch(expression)
    if not match:
        return None
    name, sign, offset, memory_base = match.groups()
    if (memory_base or base) != register or name not in symbols:
        return None
    if insn.mnemonic in MEMORY_LOW and memory_base != register:
        return None
    addend = int(offset, 0) * (-1 if sign == "-" else 1) if offset else 0
    target = symbols[name] + addend
    if ((insn.word >> 16) & 31) != int(register[1:]) or insn.word & 0xFFFF != target & 0xFFFF:
        return None
    return name, addend, target


def propose(function: Function, symbols: dict[str, int]) -> list[Proposal]:
    instructions = {insn.address: insn for insn in function.instructions}
    proposals = []
    for lis in function.instructions:
        match = LIS.fullmatch(lis.operands) if lis.mnemonic == "lis" else None
        if not match or function.section not in {".text", ".init"}:
            continue
        register, immediate = match.groups()
        high = int(immediate, 0) & 0xFFFF
        if register == "r0" or lis.word != (15 << 26 | int(register[1:]) << 21 | high):
            continue
        active = set()
        memo = {}

        def visit(address):
            if address in active or address not in instructions:
                return None
            if address in memo:
                return memo[address]
            insn = instructions[address]
            use = low_use(insn, register, symbols)
            if use:
                name, addend, target = use
                if ((target + 0x8000) >> 16) & 0xFFFF != high:
                    return None
                return (name, addend, frozenset({address}))
            opcode = insn.word >> 26
            if opcode in {16, 18}:
                # A linked branch may clobber any register; don't cross calls.
                if insn.word & 1:
                    return None
                bits = 26 if opcode == 18 else 16
                displacement = signed(insn.word & ((1 << bits) - 4), bits)
                target = displacement if insn.word & 2 else address + displacement
                successors = [target]
                if opcode == 16 and ((insn.word >> 21) & 20) != 20:
                    successors.append(address + 4)
            else:
                if insn.mnemonic not in SAFE or re.search(rf"\b{register}\b", insn.operands):
                    return None
                successors = [address + 4]
            active.add(address)
            results = [visit(successor) for successor in successors]
            active.remove(address)
            result = None
            if all(results) and len({item[:2] for item in results}) == 1:
                result = (*results[0][:2], frozenset().union(*(item[2] for item in results)))
            memo[address] = result
            return result

        try:
            result = visit(lis.address + 4)
        except RecursionError:
            result = None  # An unusually long path cannot be proved by this pass.
        if result:
            name, addend, uses = result
            proposals.append(Proposal(function.section, lis.address, name, addend,
                                      function.name, tuple(sorted(uses))))
    return proposals


def render(proposals: list[Proposal]) -> str:
    lines = ["add_relocations:" if proposals else "add_relocations: []"]
    for item in proposals:
        uses = ", ".join(f"0x{address:08X}" for address in item.uses)
        lines.extend([f"# {item.function}: @l use(s) {uses}",
                      f"- source: {item.section}:0x{item.source:08X}",
                      "  type: ha", f"  target: {item.target}"])
        if item.addend:
            lines.append(f"  addend: {item.addend}")
    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--asm", type=Path, default=ROOT / "build/GL5E4F/asm")
    parser.add_argument("--symbols", type=Path, default=ROOT / "config/GL5E4F/symbols.txt")
    args = parser.parse_args()
    paths = sorted(args.asm.rglob("*.s")) if args.asm.is_dir() else [args.asm]
    if not paths:
        parser.error("no assembly found; run bash build.sh first")
    symbols = parse_symbols(args.symbols.read_text())
    proposals = []
    for path in paths:
        for function in parse_asm(path.read_text()):
            proposals.extend(propose(function, symbols))
    proposals.sort(key=lambda item: (item.section, item.source))
    if len({(item.section, item.source) for item in proposals}) != len(proposals):
        parser.error("overlapping assembly inputs; refusing duplicate sources")
    sys.stdout.write(render(proposals))
    print(f"{len(proposals)} proposals in {len({p.function for p in proposals})} functions",
          file=sys.stderr)


if __name__ == "__main__":
    main()
