#!/usr/bin/env python3
"""Give assembler-resolved calls to static functions the relocation DTK's split has.

GNU as resolves a `bl` (or tail-call `b`) to a local function in the same section and
emits no relocation; DTK's split target carries an R_PPC_REL24 to the function
symbol for every such call. The bytes agree, but objdiff then scores the call as a
mismatch (findfp.c `__sinit` calling `std`, fread.c calling `fread_copy`). This adds
the R_PPC_REL24 against the destination function with addend 0, so the linker
writes the same displacement and objdiff, verify_unit and the link all see the
relocation the target has.

Usage: prodg_branch_relocs.py OBJECT   (rewrites OBJECT in place; prodg_cc.py runs it)
"""

from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from strip_unused import Elf, SHT_RELA, SHF_EXECINSTR, STT_FUNC  # noqa: E402

SHT_SYMTAB = 2
R_PPC_REL24 = 10


def resolved_calls(elf, shndx, functions, relocated):
    """(offset, symbol index) for each relocation-free branch to a function start."""
    body = elf.contents[shndx]
    starts = {}
    for index, (value, size) in functions.items():
        starts.setdefault(value, index)
    for off in range(0, len(body) - 3, 4):
        if off in relocated:
            continue
        word, = struct.unpack('>I', body[off:off + 4])
        if word >> 26 != 18 or word & 2:
            continue
        disp = word & 0x03FFFFFC
        disp -= (disp & 0x02000000) << 1
        dest = off + disp
        if dest not in starts:
            continue
        if not word & 1 and any(v <= off < v + s and v == dest for v, s in functions.values()):
            continue  # a loop back to its own function's first instruction
        yield off, starts[dest]


def add_branch_relocs(elf):
    symtab = next(i for i, sh in enumerate(elf.sections) if sh[1] == SHT_SYMTAB)
    syms = [struct.unpack('>IIIBBH', elf.contents[symtab][o:o + 16])
            for o in range(0, len(elf.contents[symtab]), 16)]
    rela_for = {sh[7]: i for i, sh in enumerate(elf.sections) if sh[1] == SHT_RELA}
    added = 0
    for shndx, sh in enumerate(elf.sections):
        if sh[1] != 1 or not sh[2] & SHF_EXECINSTR:
            continue
        functions = {i: (s[1], s[2]) for i, s in enumerate(syms)
                     if s[5] == shndx and s[3] & 15 == STT_FUNC}
        rela = rela_for.get(shndx)
        records = []
        if rela is not None:
            body = elf.contents[rela]
            records = [body[o:o + 12] for o in range(0, len(body), 12)]
        relocated = {struct.unpack('>I', r[:4])[0] & ~3 for r in records}
        new = [struct.pack('>IIi', off, sym << 8 | R_PPC_REL24, 0)
               for off, sym in resolved_calls(elf, shndx, functions, relocated)]
        if not new:
            continue
        if rela is None:
            rela = add_rela_section(elf, shndx, symtab)
        records += new
        records.sort(key=lambda r: struct.unpack('>I', r[:4])[0])
        elf.contents[rela] = bytearray().join(records)
        added += len(new)
    return added


def add_rela_section(elf, shndx, symtab):
    shstr = elf.contents[elf.shstrndx]
    name = len(shstr)
    shstr += f'.rela{elf.names[shndx]}'.encode() + b'\0'
    elf.sections.append([name, SHT_RELA, 0, 0, 0, 0, symtab, shndx, 4, 12])
    elf.names.append(f'.rela{elf.names[shndx]}')
    elf.contents.append(bytearray())
    elf.shnum += 1
    struct.pack_into('>H', elf.data, 48, elf.shnum)
    return elf.shnum - 1


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    path = Path(sys.argv[1])
    elf = Elf(path.read_bytes())
    if add_branch_relocs(elf):
        elf.write(path)


if __name__ == '__main__':
    main()
