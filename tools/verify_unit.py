"""Conservative full-unit layout and contents proof for address-taken data.

All relocations are compared against the split target and independently
checked against the encoded retail destination. Unknown encodings fail closed.
"""
import re
from types import SimpleNamespace


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
LAST_REFUSAL = ""


def refuse(reason):
    """Record why prove_unit gave up, so the caller can say more than "did not succeed"."""
    global LAST_REFUSAL
    LAST_REFUSAL = reason
    return None
LOCAL_BRANCH_OPCODES = {10: 18, 11: 16}
STT_SECTION, STT_FILE = 3, 4
STT_NOTYPE, STB_LOCAL = 0, 0


def bss_symbols(elf, section):
    """Defined symbols of one uninitialised section as [(offset, size, name)], or None if unusable."""
    found = []
    for sym in elf.symbols:
        if sym.shndx != section.index or sym.sym_type in (STT_SECTION, STT_FILE):
            continue
        if sym.sym_type == 0 and not sym.size and sym.name.startswith("..."):
            continue  # MWCC section anchor such as `...bss.0`: marks no object
        if not sym.name or not sym.size:
            return None
        found.append((sym.value, sym.size, sym.name))
    return sorted(found)


def bss_layout_refusal(name, t, m, target, mine):
    """Why the compiled `.bss`/`.sbss` does not reproduce the retail layout, or None when it does.

    BSS has no retail bytes, so zero contents prove nothing. Proof is by layout:
    (a) the compiled section is exactly as large as the retail split range;
    (b) the split's symbols tile the range without a gap, and each compiled symbol has the
        same name and offset in the target, with the same size. Alignment padding the compiler
        left before the next object (under MAX_ALIGN_TAIL bytes, ending exactly at the next
        symbol) may be a dtk `gap_*` symbol or make the target symbol larger;
    the relocation and linked-address halves of the policy live in prove_unit and
    check_linked_bss.
    """
    if m.size != t.size:
        return f"{name}: compiled section is 0x{m.size:x} bytes, the retail split range 0x{t.size:x}"
    ts, ms = bss_symbols(target, t), bss_symbols(mine, m)
    if ts is None or ms is None:
        return f"{name}: a {'target' if ts is None else 'compiled'} symbol has no name or no size"
    if not ts or not ms:
        return f"{name}: {'target' if not ts else 'compiled'} object defines no symbol in the section"
    end = 0
    for offset, size, sym in ts:
        if offset != end:
            return (f"{name}: unexplained {'gap' if offset > end else 'overlap'} in the retail symbols "
                    f"before {sym} at +0x{offset:x} (previous symbol ends at +0x{end:x})")
        end = offset + size
    if end != t.size:
        return f"{name}: retail symbols end at +0x{end:x}, section is 0x{t.size:x} bytes"
    if len(ms) != len({offset for offset, _, _ in ms}):
        return f"{name}: two compiled symbols share an offset"
    # dtk names an unclaimed stretch `gap_NN_ADDR_section`. It is explained only as alignment
    # padding: it follows a compiled object directly, is shorter than MAX_ALIGN_TAIL and ends
    # exactly at the next compiled object (or the section end). Anything else stays a retail
    # symbol with no compiled counterpart.
    ends = {offset + size for offset, size, _ in ms}
    starts = sorted({offset for offset, _, _ in ms} | {m.size})
    ts = [(o, s, n) for o, s, n in ts
          if not (n.startswith("gap_") and o in ends and s < MAX_ALIGN_TAIL and o + s in starts)]
    retail = {offset: (size, sym) for offset, size, sym in ts}
    for index, (offset, size, sym) in enumerate(ms):
        if offset not in retail:
            return f"{name}: compiled symbol {sym} at +0x{offset:x} has no retail symbol at that offset"
        rsize, rname = retail[offset]
        if rname != sym:
            return f"{name}: compiled symbol {sym} at +0x{offset:x} is {rname} in the retail split"
        if rsize == size:
            continue
        following = ms[index + 1][0] if index + 1 < len(ms) else m.size
        if not size < rsize < size + MAX_ALIGN_TAIL or offset + rsize != following:
            return (f"{name}: {sym} at +0x{offset:x} is 0x{size:x} bytes, retail 0x{rsize:x} "
                    "(not alignment padding before the next object)")
    if len(ms) != len(ts):
        extra = sorted({o for o, _, _ in ts} - {o for o, _, _ in ms})
        return f"{name}: retail symbol at +0x{extra[0]:x} has no compiled counterpart ({len(ts) - len(ms)} missing)"
    return None


def check_linked_bss(ranges, mine, linked):
    """Return an error string unless every compiled BSS symbol sits at its retail address in the link.

    `linked` maps names to sets of linked addresses. Every relocation into the section
    from any unit resolves through these symbols, so an object at the retail address
    here means references from other units resolve to the retail address too.
    MWCC's local `...bss.0` section anchors are skipped: only the unit's own
    relocations can use them, and the section relocation proof covers those.
    """
    for section in mine.sections:
        if section.sh_type != 8 or section.name not in ranges:
            continue
        start = ranges[section.name][0]
        for sym in mine.symbols:
            if sym.shndx != section.index or sym.sym_type in (STT_SECTION, STT_FILE):
                continue
            if sym.bind == STB_LOCAL and sym.sym_type == STT_NOTYPE and sym.name.startswith('...'):
                continue
            if start + sym.value not in linked.get(sym.name, ()):
                return (f"{section.name}: {sym.name} is not linked at retail 0x{start + sym.value:08x} "
                        f"(linked at {sorted(f'0x{a:08x}' for a in linked.get(sym.name, ())) or 'nowhere'})")
    return None


def prove_unit(target, mine, ranges, symbols, dol):
    """Return proved section bases, or None. No name-only relocation matches."""
    if not ranges or dol is None:
        return refuse("no split ranges or no retail DOL")
    try:
        from tools.verify_fn import RELOC_TYPES, HALFWORD_TYPES, WORD_OR_HALF_TYPES
    except ModuleNotFoundError:
        from verify_fn import RELOC_TYPES, HALFWORD_TYPES, WORD_OR_HALF_TYPES
    def sections(elf):
        if any(section.sh_type == 9 for section in elf.sections):
            return refuse("SHT_REL sections are not decoded")
        result = {}
        for section in elf.sections:
            if section.flags & 2 and section.size:
                if section.name in result or section.name not in ranges:
                    return refuse(f"{section.name} is duplicated or has no split range")
                result[section.name] = section
        if set(result) != set(ranges):
            return refuse(f"object sections {sorted(result)} differ from split ranges {sorted(ranges)}")
        return result
    ts, ms = sections(target), sections(mine)
    if ts is None or ms is None:
        return None
    # The linker places a compiled data section at its own alignment. When a split
    # range starts on a smaller boundary (dtk's "Alignment ... expected 8" warning),
    # the bytes before the aligned start are the previous object's padding.
    lead = {name: 0 if ms[name].flags & 4 or ms[name].sh_type == 8
            else -start % max(getattr(ms[name], 'align', 1), 1)
            for name, (start, _) in ranges.items()}

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
            if elf is not target:
                start += lead[section.name]
            if not 0 <= sym.value <= end - start:
                return None
            addr = start + sym.value
        else:
            return None
        return None if addr is None else addr + rel.addend

    for name, (start, end) in ranges.items():
        t, m = ts[name], ms[name]
        if t.size != end - start or t.sh_type != m.sh_type or t.flags != m.flags:
            return refuse(f"{name}: split range 0x{end - start:x}, target 0x{t.size:x}, donor 0x{m.size:x}, "
                          "or type/flags differ")
        # BSS has no retail bytes: do not infer uninitialised layout from zeroes. Prove it
        # by layout instead (size, symbol offsets and sizes); relocations into it are
        # resolved by address through the code and data sections that use them.
        if t.sh_type == 8:
            reason = bss_layout_refusal(name, t, m, target, mine)
            if reason:
                return refuse(reason)
            continue
        # The linker aligns the next object, so a donor data section may end
        # before its retail range. Only a short all-zero tail is accepted.
        head = lead[name]
        pad = t.size - m.size - head
        if pad < 0 or pad and not (0 < pad < MAX_ALIGN_TAIL and not t.flags & 4):
            return refuse(f"{name}: donor section is {pad} bytes short of the retail range")
        tb, mb, rb = bytearray(target.section_bytes(t)), bytearray(mine.section_bytes(m)), dol.read(start, t.size)
        if rb is None or len(tb) != t.size or len(mb) != m.size or len(rb) != t.size:
            return refuse(f"{name}: cannot read retail bytes")
        if any(tb[:head]) or any(rb[:head]) or any(tb[t.size - pad:]) or any(rb[t.size - pad:]):
            return refuse(f"{name}: alignment padding outside the compiled section is not zero")
        mb = bytearray(head) + mb + bytearray(pad)
        tr, mr = target.relocs.get(t.index, []), mine.relocs.get(m.index, [])
        code = bool(t.flags & 4)

        def canonical(elf, relocs):
            result = {}
            shift = 0 if elf is target else head
            for rel in relocs:
                rel = SimpleNamespace(**{**vars(rel), 'offset': rel.offset + shift}) if shift else rel
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
            return refuse(f"{name}: unsupported, duplicate or unresolved relocation in the "
                          f"{'target' if tc is None else 'donor'} object")
        if code:
            # DTK emits a relocation for a branch within the section (back to
            # its own function entry, or a call to a static function such as
            # fread's byte copier); the assembler resolves it. Accept it only
            # when the relative branch or call assembled in the source object
            # lands on the same address.
            for offset, (kind, addend, address, mask) in tc.items():
                if offset in mc or kind not in LOCAL_BRANCH_OPCODES:
                    continue
                word = int.from_bytes(mb[offset:offset + 4], 'big')
                if word >> 26 != LOCAL_BRANCH_OPCODES[kind] or word & 2:
                    continue
                bits = 26 if kind == 10 else 16
                disp = word & mask
                disp -= (disp & (1 << (bits - 1))) << 1
                if (start + offset + disp) & 0xffffffff == address:
                    mc[offset] = (kind, addend, address, mask)
        # The resolved address is the proof: a section symbol plus offset and a
        # named symbol with a zero addend are the same relocation target.
        if {o: (k, a, m) for o, (k, _, a, m) in tc.items()} != {o: (k, a, m) for o, (k, _, a, m) in mc.items()}:
            diff = sorted(o for o in set(tc) | set(mc) if tc.get(o) != mc.get(o))
            return refuse(f"{name}: relocations differ at +0x{diff[0]:x} (target {tc.get(diff[0])}, "
                          f"donor {mc.get(diff[0])}); {len(diff)} differing")
        clean_retail = bytearray(rb)
        for offset, (kind, _, address, mask) in tc.items():
            word = int.from_bytes(rb[offset:offset + 4], 'big')
            if not retail_relocation_matches(kind, address, start + offset, word, symbols, dol):
                return refuse(f"{name}: retail word at +0x{offset:x} does not encode relocation to 0x{address:x}")
            for body in (tb, mb, clean_retail):
                word = int.from_bytes(body[offset:offset + 4], 'big') & ~mask & 0xffffffff
                body[offset:offset + 4] = word.to_bytes(4, 'big')
        if tb != mb or tb != clean_retail:
            bad = next(i for i in range(len(tb)) if tb[i] != mb[i] or tb[i] != clean_retail[i])
            return refuse(f"{name}: bytes differ at +0x{bad:x} (target {tb[bad]:02x}, donor {mb[bad]:02x}, "
                          f"retail {clean_retail[bad]:02x})")
    return {name: start + lead[name] for name, (start, _) in ranges.items()}


def linked_symbol_addresses(elf):
    """{name: {address}} for the defined symbols of the linked image."""
    result = {}
    for sym in elf.symbols:
        if sym.shndx and sym.name and sym.sym_type not in (STT_SECTION, STT_FILE):
            result.setdefault(sym.name, set()).add(sym.value)
    return result


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
        mine = vf.Elf(source_path)
        proof = prove_unit(vf.Elf(target_path), mine, ranges, vf.SymbolTable(), dol)
        refusal = LAST_REFUSAL
        if proof is not None and any(s.sh_type == 8 and s.name in proof for s in mine.sections):
            # The BSS layout is proved; the link must also have put it at the retail addresses.
            linked_path = vf.ROOT / "build/GL5E4F/main.elf"
            vf.ensure_fresh(linked_path, args.unit, args.build, False)
            refusal = check_linked_bss(ranges, mine, linked_symbol_addresses(vf.Elf(linked_path)))
            proof = None if refusal else proof
    except (vf.ToolError, OSError, ValueError, IndexError, TypeError, struct.error) as exc:
        print(f"FAIL: {args.unit}: {exc}", file=sys.stderr)
        return 1
    if proof is None:
        print(f"FAIL: {args.unit}: full section and relocation proof did not succeed: {refusal}")
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
