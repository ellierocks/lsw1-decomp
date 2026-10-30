#!/usr/bin/env python3
"""Reproduce SN dead stripping for MWCC SDK objects.

Ported from adonis-singh/re4 tools/strip_unused.py, commit
9b76ff003081d6ece8b838453fb8310743a972c7 (MWCC path only).
Keep symbols listed in the DTK target object for the unit; preserve local
pooled data and the reference implementation's eight-byte data granularity.
Abort if surviving code references anything removed. Run after dtk elf fixup.
"""
import argparse
import struct
import sys
from pathlib import Path
SHT_SYMTAB = 2
SHT_RELA = 4
SHT_NOBITS = 8
STT_OBJECT = 1
STT_FUNC = 2
STT_SECTION = 3
STB_LOCAL = 0
DATA_GRANULE = 8

class Elf:

    def __init__(self, data):
        self.data = bytearray(data)
        self.shoff, = struct.unpack('>I', data[32:36])
        self.shentsize, self.shnum, self.shstrndx = struct.unpack('>HHH', data[46:52])
        self.sections = []
        for i in range(self.shnum):
            off = self.shoff + i * self.shentsize
            self.sections.append(list(struct.unpack('>IIIIIIIIII', data[off:off + 40])))
        shstr = self.sec_bytes(self.shstrndx)
        self.names = []
        for sh in self.sections:
            end = shstr.index(b'\x00', sh[0])
            self.names.append(shstr[sh[0]:end].decode())
        self.contents = [bytearray(self.sec_bytes(i)) for i in range(self.shnum)]

    def sec_bytes(self, i):
        sh = self.sections[i]
        if sh[1] == SHT_NOBITS:
            return b'\x00' * sh[5]
        return self.data[sh[4]:sh[4] + sh[5]]

    def index(self, name):
        return self.names.index(name)

    def write(self, path):
        out = bytearray(self.data[:52])
        pos = 52
        for i in range(1, self.shnum):
            sh = self.sections[i]
            body = self.contents[i]
            align = max(sh[8], 1)
            pos = (pos + align - 1) // align * align
            sh[4] = pos
            sh[5] = len(body)
            if sh[1] != SHT_NOBITS:
                out += b'\x00' * (pos - len(out))
                out += body
                pos += len(body)
        pos = (pos + 3) // 4 * 4
        out += b'\x00' * (pos - len(out))
        struct.pack_into('>I', out, 32, pos)
        for sh in self.sections:
            out += struct.pack('>IIIIIIIIII', *sh)
        with open(path, 'wb') as f:
            f.write(out)

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--target', required=True, type=Path)
    ap.add_argument('object', type=Path)
    args = ap.parse_args()
    args.unit = str(args.target)
    target = Elf(args.target.read_bytes())
    sti = target.index('.symtab')
    strings = target.contents[target.sections[sti][6]]
    keep = {}
    for off in range(0, len(target.contents[sti]), 16):
        name, value, size, info, other, sec = struct.unpack('>IIIBBH', target.contents[sti][off:off + 16])
        if sec == 0 or sec >= target.shnum:
            continue
        nm = strings[name:strings.index(b'\x00', name)].decode()
        keep.setdefault(target.names[sec], set()).add(nm)
    if not keep:
        sys.exit('strip_unused: target has no defined symbols')
    elf = Elf(open(args.object, 'rb').read())
    symtab = elf.index('.symtab')
    strtab = elf.sections[symtab][6]
    syms = [list(struct.unpack('>IIIBBH', elf.contents[symtab][o:o + 16])) for o in range(0, len(elf.contents[symtab]), 16)]
    strs = elf.contents[strtab]

    def sym_name(s):
        end = strs.index(b'\x00', s[0])
        return strs[s[0]:end].decode()
    refcount = [0] * len(syms)
    pooled_sections = set()
    for i, sh in enumerate(elf.sections):
        if sh[1] != SHT_RELA:
            continue
        for o in range(0, len(elf.contents[i]), 12):
            r_off, r_info = struct.unpack('>II', elf.contents[i][o:o + 8])
            sym = r_info >> 8
            refcount[sym] += 1
            s = syms[sym]
            if s[3] & 15 in (STT_SECTION, 0) and s[5] != 0:
                pooled_sections.add(s[5])
    removed = {}
    dead = {}
    removed_idx = set()
    for i, s in enumerate(syms):
        shndx, stype, bind = (s[5], s[3] & 15, s[3] >> 4)
        if shndx == 0 or shndx >= elf.shnum:
            continue
        if stype not in (STT_FUNC, STT_OBJECT):
            continue
        secname = elf.names[shndx]
        name = sym_name(s)
        if name in keep.get(secname, ()):
            continue
        if name.startswith('_GLOBAL_.I.') and '_GLOBAL_.I.*' in keep.get(secname, ()):
            continue
        if name.startswith('_GLOBAL_.D.') and '_GLOBAL_.D.*' in keep.get(secname, ()):
            continue
        if stype == STT_FUNC:
            removed.setdefault(shndx, []).append((s[1], s[1] + s[2]))
        elif bind != STB_LOCAL or (refcount[i] == 0 and shndx not in pooled_sections):
            cut = s[2] - s[2] % DATA_GRANULE
            if cut:
                removed.setdefault(shndx, []).append((s[1], s[1] + cut))
        else:
            continue
        dead.setdefault(shndx, []).append((s[1], s[1] + s[2]))
        removed_idx.add(i)
    # MWCC marks small constants writable; the SN input convention and DTK
    # target mark .sdata2 read-only. This changes ELF metadata only.
    for index, name in enumerate(elf.names):
        if name == '.sdata2':
            elf.sections[index][2] &= ~1
    if not removed_idx:
        elf.write(args.object)
        return
    for r in removed.values():
        r.sort()

    def in_dead(shndx, off):
        return any((start <= off < end for start, end in dead.get(shndx, ())))

    def shift(shndx, off):
        """New offset for an old offset in a section (must not be inside a removed range)."""
        d = 0
        for start, end in removed.get(shndx, ()):
            if start <= off < end:
                raise ValueError(f'offset {off:#x} in {elf.names[shndx]} lies inside a removed range')
            if end <= off:
                d += end - start
        return off - d
    for shndx, ranges in removed.items():
        old = elf.contents[shndx]
        new = bytearray()
        pos = 0
        for start, end in ranges:
            new += old[pos:start]
            pos = end
        new += old[pos:]
        elf.contents[shndx] = new
    remap = {}
    new_syms = []
    for i, s in enumerate(syms):
        if i in removed_idx:
            continue
        if i != 0 and s[5] in removed and (s[3] & 15 != STT_SECTION):
            try:
                s[1] = shift(s[5], s[1])
            except ValueError as e:
                if s[3] & 15 in (STT_FUNC, STT_OBJECT):
                    sys.exit(f'strip_unused: symbol {sym_name(s)}: {e}')
                d = 0
                for start, end in removed[s[5]]:
                    if end <= s[1]:
                        d += end - start
                    elif start <= s[1]:
                        d += s[1] - start
                s[1] -= d
        remap[i] = len(new_syms)
        new_syms.append(s)
    elf.contents[symtab] = bytearray().join((struct.pack('>IIIBBH', *s) for s in new_syms))
    first_global = next((k for k, s in enumerate(new_syms) if s[3] >> 4 != 0), len(new_syms))
    elf.sections[symtab][7] = first_global
    for i, sh in enumerate(elf.sections):
        if sh[1] != SHT_RELA:
            continue
        target = sh[7]
        out = bytearray()
        for o in range(0, len(elf.contents[i]), 12):
            r_off, r_info, r_add = struct.unpack('>IIi', elf.contents[i][o:o + 12])
            sym, rtype = (r_info >> 8, r_info & 255)
            if in_dead(target, r_off):
                continue
            if target in removed:
                r_off = shift(target, r_off)
            if sym in removed_idx:
                if True or elf.sections[target][2] & 4:
                    sys.exit(f'strip_unused: {elf.names[target]} still references removed symbol {sym_name(syms[sym])} (unit {args.unit})')
                elf.contents[target][r_off:r_off + 4] = b'\x00\x00\x00\x00'
                continue
            s = syms[sym]
            if s[3] & 15 == STT_SECTION and s[5] in removed:
                try:
                    r_add = shift(s[5], r_add)
                except ValueError as e:
                    sys.exit(f'strip_unused: {elf.names[target]} relocation: {e}')
            out += struct.pack('>IIi', r_off, remap[sym] << 8 | rtype, r_add)
        elf.contents[i] = out
    if '.comment' in elf.names:
        ci = elf.index('.comment')
        body = elf.contents[ci]
        hdr = len(body) - 8 * len(syms)
        if hdr >= 0:
            recs = [body[hdr + 8 * k:hdr + 8 * k + 8] for k in range(len(syms))]
            elf.contents[ci] = bytearray(body[:hdr]) + bytearray().join((r for k, r in enumerate(recs) if k not in removed_idx))
    elf.write(args.object)
if __name__ == '__main__':
    main()
