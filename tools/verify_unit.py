"""Conservative full-unit layout and contents proof for address-taken data.

All relocations are compared against the split target and independently
checked against the encoded retail destination. Unknown encodings fail closed.
"""
import re


def retail_relocation_matches(kind, address, pc, word, symbols, dol):
    """Check the independently encoded relocation, without trusting target ELF fields."""
    if kind == 1:
        return word == address
    if kind in (3, 4):
        return word & 0xffff == address & 0xffff
    if kind == 5:
        return word & 0xffff == address >> 16
    if kind == 6:
        return word & 0xffff == ((address + 0x8000) >> 16) & 0xffff
    if kind in (10, 11, 12, 13):
        bits = 26 if kind == 10 else 16
        mask = ((1 << bits) - 1) & ~3
        displacement = address - pc
        return (not word & 2 and displacement % 4 == 0
                and -(1 << (bits - 1)) <= displacement < (1 << (bits - 1))
                and word & mask == displacement & mask)
    if kind == 26:
        return word == (address - pc) & 0xffffffff
    if kind == 109:
        register = (word >> 16) & 31
        if register == 0:
            base = 0
        elif register in (2, 13):
            entry = symbols.addr('__start')
            if entry is None:
                return False
            # The retail SN startup initializes r2/r13 with lis; ori pairs.
            raw = dol.read(entry + (8 if register == 2 else 16), 8)
            if raw is None or len(raw) != 8:
                return False
            high, low = int.from_bytes(raw[:4], 'big'), int.from_bytes(raw[4:], 'big')
            if (high & 0xffff0000 != 0x3c000000 | register << 21
                    or low & 0xffff0000 != 0x60000000 | register << 21 | register << 16):
                return False
            base = ((high & 0xffff) << 16) | (low & 0xffff)
        else:
            return False
        offset = word & 0xffff
        offset = offset - 0x10000 if offset & 0x8000 else offset
        return (base + offset) & 0xffffffff == address
    return False


def split_ranges(text, unit):
    ranges = {}
    active = False
    for line in text.splitlines():
        if not line.strip() or line.lstrip().startswith('#'):
            continue
        if not line[0].isspace():
            active = line.strip().rstrip(':').rsplit('.', 1)[0] == unit
            continue
        if active:
            match = re.match(r'\s*(\.[\w.]+)\s+start:0x([\da-fA-F]+)\s+end:0x([\da-fA-F]+)', line)
            if not match or match[1] in ranges:
                return None
            start, end = int(match[2], 16), int(match[3], 16)
            if end <= start:
                return None
            ranges[match[1]] = (start, end)
    if any(a[1] > b[0] for a, b in zip(sorted(ranges.values()), sorted(ranges.values())[1:])):
        return None
    return ranges or None


MAX_ALIGN_TAIL = 32
LOCAL_BRANCH_OPCODES = {10: 18, 11: 16}


def prove_unit(target, mine, ranges, symbols, dol):
    """Return proved section bases, or None. No name-only relocation matches."""
    if not ranges or dol is None:
        return None
    try:
        from tools.verify_fn import RELOC_TYPES, HALFWORD_TYPES, WORD_OR_HALF_TYPES
    except ModuleNotFoundError:
        from verify_fn import RELOC_TYPES, HALFWORD_TYPES, WORD_OR_HALF_TYPES
    def sections(elf):
        if any(section.sh_type == 9 for section in elf.sections):
            return None  # SHT_REL is not decoded by the function verifier.
        result = {}
        for section in elf.sections:
            if section.flags & 2 and section.size:
                if section.name in result or section.name not in ranges:
                    return None
                result[section.name] = section
        return result if set(result) == set(ranges) else None
    ts, ms = sections(target), sections(mine)
    if ts is None or ms is None:
        return None

    def resolve(elf, rel):
        if rel.sym >= len(elf.symbols):
            return None
        sym = elf.symbols[rel.sym]
        if sym.shndx == 0:
            addr = symbols.addr(sym.name)
        elif sym.shndx < len(elf.sections):
            section = elf.sections[sym.shndx]
            selected = ts if elf is target else ms
            if section.name not in ranges or selected.get(section.name) is not section:
                return None
            start, end = ranges[section.name]
            if not 0 <= sym.value <= end - start:
                return None
            addr = start + sym.value
        else:
            return None
        return None if addr is None else addr + rel.addend

    for name, (start, end) in ranges.items():
        t, m = ts[name], ms[name]
        if t.size != end - start or t.sh_type != m.sh_type or t.flags != m.flags:
            return None
        # The linker aligns the next object, so a donor data section may end
        # before its retail range. Only a short all-zero tail is accepted.
        pad = t.size - m.size
        if pad and not (0 < pad < MAX_ALIGN_TAIL and not t.flags & 4 and t.sh_type != 8):
            return None
        # BSS has no retail bytes: do not infer uninitialised layout from zeroes.
        if t.sh_type == 8:
            return None
        tb, mb, rb = bytearray(target.section_bytes(t)), bytearray(mine.section_bytes(m)), dol.read(start, t.size)
        if rb is None or len(tb) != t.size or len(mb) != m.size or len(rb) != t.size:
            return None
        if pad:
            if any(tb[m.size:]) or any(rb[m.size:]):
                return None
            mb += bytes(pad)
        tr, mr = target.relocs.get(t.index, []), mine.relocs.get(m.index, [])
        code = bool(t.flags & 4)

        def canonical(elf, relocs):
            result = {}
            for rel in relocs:
                if code:
                    if rel.rtype not in RELOC_TYPES:
                        return None
                    half = 2 if rel.rtype in HALFWORD_TYPES else 0
                    if rel.rtype in WORD_OR_HALF_TYPES and rel.offset % 4 == 2:
                        half = 2
                    if rel.offset % 4 != half:
                        return None
                    offset = rel.offset - half
                    mask = RELOC_TYPES[rel.rtype][1]
                else:
                    if rel.rtype != 1 or rel.offset % 4:
                        return None
                    offset, mask = rel.offset, 0xffffffff
                if offset < 0 or offset + 4 > t.size or offset in result:
                    return None
                address = resolve(elf, rel)
                if address is None or not 0 <= address <= 0xffffffff:
                    return None
                result[offset] = (rel.rtype, rel.addend, address, mask)
            return result

        tc, mc = canonical(target, tr), canonical(mine, mr)
        if tc is None or mc is None:
            return None
        if code:
            # DTK emits a relocation for a branch back to its own function
            # entry; the assembler resolves it. Accept it only when the
            # branch assembled in the source object lands on the same address.
            for offset, (kind, addend, address, mask) in tc.items():
                if offset in mc or kind not in LOCAL_BRANCH_OPCODES:
                    continue
                word = int.from_bytes(mb[offset:offset + 4], 'big')
                if word >> 26 != LOCAL_BRANCH_OPCODES[kind] or word & 3:
                    continue
                bits = 26 if kind == 10 else 16
                disp = word & mask
                disp -= (disp & (1 << (bits - 1))) << 1
                if (start + offset + disp) & 0xffffffff == address:
                    mc[offset] = (kind, addend, address, mask)
        # The resolved address is the proof: a section symbol plus offset and a
        # named symbol with a zero addend are the same relocation target.
        if {o: (k, a, m) for o, (k, _, a, m) in tc.items()} != {o: (k, a, m) for o, (k, _, a, m) in mc.items()}:
            return None
        clean_retail = bytearray(rb)
        for offset, (kind, _, address, mask) in tc.items():
            word = int.from_bytes(rb[offset:offset + 4], 'big')
            if not retail_relocation_matches(kind, address, start + offset, word, symbols, dol):
                return None
            for body in (tb, mb, clean_retail):
                word = int.from_bytes(body[offset:offset + 4], 'big') & ~mask & 0xffffffff
                body[offset:offset + 4] = word.to_bytes(4, 'big')
        if tb != mb or tb != clean_retail:
            return None
    return {name: start for name, (start, _) in ranges.items()}


def verify_functions(vf, unit):
    """Strictly verify every function of the unit's object in one process.

    Returns (name, ok, verdict) rows. The object must already be fresh.
    """
    import argparse
    import struct
    rows = []
    for fn in vf.Elf(vf.SRC_OBJ_DIR / f"{unit}.o").function_symbols():
        if fn.sym_type != vf.STT_FUNC or not fn.size:
            continue
        args = argparse.Namespace(name=fn.name, unit=unit, compiled_symbol=None, object=None,
                                  target_object=None, build=False, allow_stale=True, dol=None,
                                  no_dol=False, quiet=True)
        try:
            rep = vf.verify(args)[0]
        except (vf.ToolError, struct.error, ValueError, IndexError, OSError) as exc:
            rows.append((fn.name, False, f"ERROR {str(exc).splitlines()[0]}"))
            continue
        ok = rep.exit_code == vf.EXIT_MATCH and "unverified" not in rep.verdict
        rows.append((fn.name, ok, rep.verdict))
    return rows


def main(argv=None):
    import argparse
    import sys
    import struct

    try:
        from tools import verify_fn as vf
    except ModuleNotFoundError:
        import verify_fn as vf

    parser = argparse.ArgumentParser(description="Prove a complete split unit against retail.")
    parser.add_argument("unit", help="unit path without extension, for example dolphin/gx/GXPerf")
    parser.add_argument("--build", action="store_true", help="build the source object with ninja first")
    parser.add_argument("--functions", action="store_true",
                        help="also strictly verify every function in the unit (like verify_fn.py "
                             "--strict-symbols, in one process)")
    args = parser.parse_args(argv)
    if not re.fullmatch(r"[A-Za-z0-9_]+(?:/[A-Za-z0-9_]+)*", args.unit):
        print("FAIL: invalid unit path", file=sys.stderr)
        return 1
    target_path = vf.TARGET_OBJ_DIR / f"{args.unit}.o"
    source_path = vf.SRC_OBJ_DIR / f"{args.unit}.o"
    try:
        vf.ensure_fresh(source_path, args.unit, args.build, False)
        notes = []
        dol = vf.load_dol(vf.DOL_PATH, notes)
        ranges = split_ranges(vf.SPLITS.read_text(), args.unit)
        proof = prove_unit(vf.Elf(target_path), vf.Elf(source_path), ranges, vf.SymbolTable(), dol)
    except (vf.ToolError, OSError, ValueError, IndexError, TypeError, struct.error) as exc:
        print(f"FAIL: {args.unit}: {exc}", file=sys.stderr)
        return 1
    if proof is None:
        print(f"FAIL: {args.unit}: full section and relocation proof did not succeed")
        return 1
    print(f"PASS: {args.unit}: {len(proof)} complete sections match retail")
    if not args.functions:
        return 0
    rows = verify_functions(vf, args.unit)
    failed = [row for row in rows if not row[1]]
    for name, ok, verdict in rows:
        print(f"  {'PASS' if ok else 'FAIL'}: {name}: {verdict}")
    print(f"{'FAIL' if failed else 'PASS'}: {args.unit}: {len(rows) - len(failed)}/{len(rows)} functions strictly match")
    return 1 if failed or not rows else 0


if __name__ == "__main__":
    raise SystemExit(main())
