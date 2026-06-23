#!/usr/bin/env python3
"""Verify a decompiled function against the target, without `dtk dol split`.

The objdiff target-object workflow needs `dtk dol split`, which panics on this
project. This helper sidesteps it: it disassembles your compiled source object
and compares it, instruction-by-instruction, against the DOL-accurate target
disassembly already present under build/GL5E4F/asm/.

Usage:
    python tools/verify_fn.py NuVecMtxTransformH
    python tools/verify_fn.py NuVecMtxTransformH --unit numath/nuvec

It maps the symbol -> address/size (symbols.txt) and -> unit (splits.txt),
disassembles build/GL5E4F/src/<unit>.o, pulls the target instructions by
absolute address range from the asm dump, then prints them side-by-side with
the first mnemonic-level mismatch flagged.
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
SPLITS = ROOT / "config/GL5E4F/splits.txt"
ASM_DIR = ROOT / "build/GL5E4F/asm"
SRC_OBJ_DIR = ROOT / "build/GL5E4F/src"
DTK = ROOT / "build/tools/dtk_patched"

# /* VADDR FILEOFF BB BB BB BB */\tmnemonic operands
ASM_LINE = re.compile(
    r"^\s*/\*\s*([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+((?:[0-9A-Fa-f]{2}\s)+)\*/\s*(.*?)\s*$"
)


def lookup_symbol(name):
    pat = re.compile(rf"^{re.escape(name)}\s*=\s*\.text:0x([0-9A-Fa-f]+);.*size:0x([0-9A-Fa-f]+)")
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        m = pat.match(line)
        if m:
            return int(m.group(1), 16), int(m.group(2), 16)
    sys.exit(f"error: '{name}' not found as a .text function in symbols.txt")


def lookup_unit(addr):
    """Return the unit path (e.g. 'numath/nuvec') whose split covers addr."""
    cur = None
    start_re = re.compile(r"start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)")
    for raw in SPLITS.read_text().splitlines():
        s = raw.strip()
        if not s or s.startswith("#"):
            continue
        if not raw.startswith((" ", "\t")):
            # header line, e.g. "numath/nurand.c:"
            name = s.rstrip(":").strip()
            cur = name if not name.startswith(".") else cur
            continue
        m = start_re.search(s)
        if m and cur and s.startswith(".text"):
            lo, hi = int(m.group(1), 16), int(m.group(2), 16)
            if lo <= addr < hi:
                return cur.rsplit(".", 1)[0]  # strip .c
    return None


def split_insn(text):
    """'stwu r1, -0x18(r1)' -> ('stwu', 'r1, -0x18(r1)')."""
    parts = text.split(None, 1)
    return (parts[0], parts[1] if len(parts) > 1 else "")


def disasm_src(unit, name):
    obj = SRC_OBJ_DIR / f"{unit}.o"
    if not obj.is_file():
        sys.exit(f"error: {obj} not built. Run: ninja {obj.relative_to(ROOT)}")
    out = subprocess.run([str(DTK), "elf", "disasm", str(obj), "-"],
                         capture_output=True, text=True)
    text = out.stdout if out.stdout.strip() else _disasm_tofile(obj)
    insns, grab = [], False
    for line in text.splitlines():
        s = line.strip()
        if s.startswith(".fn ") and s.split()[1].rstrip(",") == name:
            grab = True
            continue
        if grab and s.startswith(".endfn"):
            break
        if grab:
            m = ASM_LINE.match(line)
            if m:
                insns.append(split_insn(m.group(3)))
    return insns


def _disasm_tofile(obj):
    tmp = Path("/tmp/_verify_src.s")
    subprocess.run([str(DTK), "elf", "disasm", str(obj), str(tmp)],
                   capture_output=True, text=True)
    return tmp.read_text(errors="replace")


def target_insns(addr, size):
    end = addr + size
    found = {}
    for asm in ASM_DIR.rglob("*.s"):
        for line in asm.read_text(errors="replace").splitlines():
            m = ASM_LINE.match(line)
            if not m:
                continue
            va = int(m.group(1), 16)
            if addr <= va < end:
                found[va] = split_insn(m.group(3))
    return [found[k] for k in sorted(found)]


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name")
    ap.add_argument("--unit", help="override unit path, e.g. numath/nuvec")
    ap.add_argument("--compiled-symbol", help="symbol emitted by the source object")
    args = ap.parse_args()

    addr, size = lookup_symbol(args.name)
    unit = args.unit or lookup_unit(addr)
    if not unit:
        sys.exit(f"error: no split unit covers 0x{addr:08X}; pass --unit")

    mine = disasm_src(unit, args.compiled_symbol or args.name)
    tgt = target_insns(addr, size)
    if not tgt:
        sys.exit(f"error: no target asm found for 0x{addr:08X}..0x{addr+size:08X}")

    print(f"{args.name}  @0x{addr:08X}  size 0x{size:X}  unit {unit}")
    print(f"target: {len(tgt)} insns   mine: {len(mine)} insns\n")

    # Align the two instruction streams (mnemonic only) so equivalent
    # instructions line up even when scheduling/length differs.
    import difflib
    tkeys = [t[0] for t in tgt]
    mkeys = [m[0] for m in mine]
    sm = difflib.SequenceMatcher(a=tkeys, b=mkeys, autojunk=False)

    def fmt(pair):
        return f"{pair[0]} {pair[1]}".strip()[:33] if pair else ""

    print(f"{'':>2} {'TARGET':<34} {'MINE':<34}")
    aligned = 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for k in range(i2 - i1):
                aligned += 1
                print(f"   {fmt(tgt[i1+k]):<34} {fmt(mine[j1+k]):<34}")
        else:  # replace / delete / insert -> show side by side, marked
            span = max(i2 - i1, j2 - j1)
            for k in range(span):
                t = tgt[i1+k] if i1+k < i2 else None
                m = mine[j1+k] if j1+k < j2 else None
                print(f">> {fmt(t):<34} {fmt(m):<34}")

    print(f"\naligned (same mnemonic, same order): {aligned}/{len(tgt)} target insns"
          f"   mine={len(mine)}")
    if aligned == len(tgt) == len(mine):
        print("=> mnemonic sequence MATCHES (verify operands/relocs above for full match)")
    else:
        print("=> NonMatching: differs by "
              f"{len(tgt) - aligned} target insns not aligned "
              f"({'length differs' if len(tgt) != len(mine) else 'reordered/substituted'})")


if __name__ == "__main__":
    main()
