#!/usr/bin/env python3
"""
Match unnamed LSW1 GC functions to named Nu2 engine functions via DOL body comparison.

Supports multiple source ELFs (Crash WoC retail, Crash WoC prototype, Finding Nemo).
All three games share the Nu2 engine compiled with Metrowerks CodeWarrior GC/1.3.2.

Two matching modes:
  exact    — MD5 hash of raw function bytes must match
  norm     — normalise branch targets and r13/r2 SDA offsets before hashing

Outputs:
  docs/symbol_donors/nu2_fn_matches.tsv       — all raw match data (all sources)
  docs/symbol_donors/nu2_fn_rename_queue.tsv  — filtered rename candidates
"""
from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

SOURCES = [
    {
        "id": "crashwoc_retail",
        "elf":  ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/files/crashwoc.elf",
        "dol":  ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/sys/main.dol",
    },
    {
        "id": "crashwoc_proto",
        "elf":  ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/prototype/prototype/files/crashwoc.elf",
        "dol":  ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/prototype/prototype/sys/main.dol",
    },
    {
        "id": "gcnemo",
        "elf":  ROOT / "orig/nu2/Disney-Pixar Finding Nemo (USA)/extracted/files/GCNemo.elf",
        "dol":  ROOT / "orig/nu2/Disney-Pixar Finding Nemo (USA)/extracted/sys/main.dol",
    },
]

LSW1_DOL    = ROOT / "orig/GL5E4F/sys/main.dol"
SYMBOLS_TXT = ROOT / "config/GL5E4F/symbols.txt"

OUT_TSV   = ROOT / "docs/symbol_donors/nu2_fn_matches.tsv"
OUT_QUEUE = ROOT / "docs/symbol_donors/nu2_fn_rename_queue.tsv"

OS_SDK_PREFIXES = (
    "OS", "__OS", "AR", "__AR", "DVD", "__DVD", "CARD", "__CARD",
    "DSP", "__DSP", "VI", "GX", "__GX", "AI", "__AI", "EXI", "__EXI",
    "VerifyFAT", "VerifyDir", "entryToPath", "DBWrite", "InsertAlarm",
    "DecrementerException", "WriteUARTN", "DBOpen", "DBClose",
)


# ---------------------------------------------------------------------------
# DOL reader
# ---------------------------------------------------------------------------

class DolReader:
    def __init__(self, path: Path):
        self.data = path.read_bytes()
        hdr = self.data
        self.sections: list[tuple[int, int, int]] = []
        for i in range(7):
            off  = struct.unpack_from(">I", hdr, i*4)[0]
            vma  = struct.unpack_from(">I", hdr, 0x48 + i*4)[0]
            size = struct.unpack_from(">I", hdr, 0x90 + i*4)[0]
            if off and vma and size:
                self.sections.append((off, vma, size))
        for i in range(11):
            off  = struct.unpack_from(">I", hdr, 0x1C + i*4)[0]
            vma  = struct.unpack_from(">I", hdr, 0x64 + i*4)[0]
            size = struct.unpack_from(">I", hdr, 0xAC + i*4)[0]
            if off and vma and size:
                self.sections.append((off, vma, size))

    def read_vma(self, vma: int, size: int) -> bytes | None:
        for foff, sec_vma, sec_size in self.sections:
            if sec_vma <= vma < sec_vma + sec_size:
                delta = vma - sec_vma
                if delta + size > sec_size:
                    return None
                return self.data[foff + delta: foff + delta + size]
        return None


# ---------------------------------------------------------------------------
# ELF symbol loader
# ---------------------------------------------------------------------------

def load_elf_functions(elf_path: Path) -> list[tuple[int, int, str]]:
    e = elf_path.read_bytes()
    u32 = lambda o: struct.unpack_from(">I", e, o)[0]
    u16 = lambda o: struct.unpack_from(">H", e, o)[0]
    shoff    = u32(32)
    shnum    = u16(48)
    shstrndx = u16(50)

    def shdr(i):
        o = shoff + i * 40
        return u32(o), u32(o+4), u32(o+16), u32(o+20), u32(o+24)  # name,type,off,size,link

    shstr_s = shdr(shstrndx)
    shstr = e[shstr_s[2]: shstr_s[2] + shstr_s[3]]

    symtab_s = strtab_s = None
    for i in range(shnum):
        s = shdr(i)
        if s[1] == 2:
            symtab_s = s
        nm = shstr[s[0]: shstr.index(b"\x00", s[0])].decode()
        if nm == ".strtab":
            strtab_s = s

    if not symtab_s or not strtab_s:
        return []

    strtab = e[strtab_s[2]: strtab_s[2] + strtab_s[3]]

    def str_at(off: int) -> str:
        end = strtab.index(b"\x00", off)
        return strtab[off:end].decode("ascii", errors="replace")

    funcs = []
    n = symtab_s[3] // 16
    base = symtab_s[2]
    for i in range(n):
        o = base + i * 16
        st_name  = u32(o)
        st_value = u32(o + 4)
        st_size  = u32(o + 8)
        st_info  = e[o + 12]
        if (st_info & 0xF) == 2 and st_value != 0 and st_name != 0 and st_size >= 4:
            funcs.append((st_value, st_size, str_at(st_name)))
    return funcs


# ---------------------------------------------------------------------------
# Normalisation
# ---------------------------------------------------------------------------

def normalise(code: bytes) -> bytes:
    out = bytearray(code)
    n = len(code) // 4
    for i in range(n):
        w = struct.unpack_from(">I", code, i * 4)[0]
        op = w >> 26
        if op == 18:
            struct.pack_into(">I", out, i * 4, w & 0xFC000003)
        elif op == 16:
            struct.pack_into(">I", out, i * 4, w & 0xFFFF0003)
        elif op == 14:
            if (w >> 16) & 0x1F in (2, 13):
                struct.pack_into(">I", out, i * 4, w & 0xFFFF0000)
        elif op in (32, 33, 34, 36, 37, 38, 40, 44):
            if (w >> 16) & 0x1F in (2, 13):
                struct.pack_into(">I", out, i * 4, w & 0xFFFF0000)
    return bytes(out)


def md5(b: bytes) -> str:
    return hashlib.md5(b).hexdigest()


# ---------------------------------------------------------------------------
# LSW1 unnamed functions
# ---------------------------------------------------------------------------

SYM_RE = re.compile(
    r"^(fn_[0-9A-Fa-f]+)\s*=\s*\.[a-z0-9]+:0x([0-9A-Fa-f]+);\s*//.*?size:0x([0-9A-Fa-f]+)"
)

def load_lsw1_unnamed(sym_path: Path) -> list[tuple[int, int, str]]:
    funcs = []
    for line in sym_path.read_text().splitlines():
        m = SYM_RE.match(line)
        if m:
            vma  = int(m.group(2), 16)
            size = int(m.group(3), 16)
            if size >= 4:
                funcs.append((vma, size, m.group(1)))
    return funcs


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("Loading LSW1 DOL and unnamed functions …")
    lsw1_dol  = DolReader(LSW1_DOL)
    lsw1_fns  = load_lsw1_unnamed(SYMBOLS_TXT)
    print(f"  {len(lsw1_fns)} unnamed fn_* entries")

    print("Fingerprinting LSW1 functions …")
    lsw1_exact: dict[str, list[tuple[int, int, str]]] = {}
    lsw1_norm:  dict[str, list[tuple[int, int, str]]] = {}
    for vma, size, name in lsw1_fns:
        code = lsw1_dol.read_vma(vma, size)
        if code is None:
            continue
        lsw1_exact.setdefault(md5(code), []).append((vma, size, name))
        lsw1_norm.setdefault(md5(normalise(code)), []).append((vma, size, name))

    # Match all sources → LSW1
    all_results: list[tuple] = []  # (src_id, crash_vma, crash_size, crash_name, lsw1_vma, lsw1_size, lsw1_name, mtype)

    for src in SOURCES:
        if not src["elf"].exists() or not src["dol"].exists():
            print(f"  Skipping {src['id']} (files missing)")
            continue
        print(f"Loading {src['id']} …")
        src_syms = load_elf_functions(src["elf"])
        src_dol  = DolReader(src["dol"])
        print(f"  {len(src_syms)} named functions")

        src_exact_count = src_norm_count = 0
        for crash_vma, crash_size, crash_name in src_syms:
            code = src_dol.read_vma(crash_vma, crash_size)
            if code is None:
                continue
            h = md5(code)
            hits = lsw1_exact.get(h, [])
            if hits:
                for lsw1_vma, lsw1_size, lsw1_name in hits:
                    all_results.append((src["id"], crash_vma, crash_size, crash_name,
                                        lsw1_vma, lsw1_size, lsw1_name, "exact"))
                src_exact_count += len(hits)
                continue
            hn = md5(normalise(code))
            hits_n = lsw1_norm.get(hn, [])
            for lsw1_vma, lsw1_size, lsw1_name in hits_n:
                all_results.append((src["id"], crash_vma, crash_size, crash_name,
                                    lsw1_vma, lsw1_size, lsw1_name, "norm"))
            src_norm_count += len(hits_n)

        print(f"  exact={src_exact_count} norm={src_norm_count}")

    print(f"\nTotal matches: {len(all_results)}")

    # Write raw TSV
    with open(OUT_TSV, "w") as f:
        f.write("source\tmatch_type\tcrash_name\tcrash_addr\tcrash_size\tlsw1_fn\tlsw1_addr\tlsw1_size\n")
        for src_id, cv, cs, cn, lv, ls, ln, mt in sorted(all_results, key=lambda r: (r[7] != "exact", r[3])):
            f.write(f"{src_id}\t{mt}\t{cn}\t{cv:#010x}\t{cs:#06x}\t{ln}\t{lv:#010x}\t{ls:#06x}\n")
    print(f"Wrote {OUT_TSV}")

    # Build rename queue: best source wins, 1:1 only, size-filtered
    fn_set: set[str] = set()
    sym_re = re.compile(r"^(fn_[0-9A-Fa-f]+)\s*=")
    for line in SYMBOLS_TXT.read_text().splitlines():
        m = sym_re.match(line)
        if m:
            fn_set.add(m.group(1))

    # Priority: exact > norm; crashwoc_retail > crashwoc_proto > gcnemo
    src_priority = {"crashwoc_retail": 0, "crashwoc_proto": 1, "gcnemo": 2}

    # For each (lsw1_fn, crash_name) pair, keep best
    from collections import Counter, defaultdict

    # Separate by match type
    exact_results = [r for r in all_results if r[7] == "exact"]
    norm_results  = [r for r in all_results if r[7] == "norm"]

    # For disambiguation: count unique crash_names per lsw1_fn, unique lsw1_fns per crash_name
    # (across all sources, ignoring duplicates from different sources that agree)
    from collections import defaultdict as ddict
    lsw1_to_crash_names: dict[str, set[str]] = ddict(set)
    crash_to_lsw1_fns:   dict[str, set[str]] = ddict(set)
    for src_id, cv, cs, cn, lv, ls, ln, mt in norm_results:
        lsw1_to_crash_names[ln].add(cn)
        crash_to_lsw1_fns[cn].add(ln)

    lsw1_in_exact = {r[5] for r in exact_results}

    # queue_map: lsw1_fn → (src_id, conf, crash_name, lsw1_vma, crash_size, crash_vma, lsw1_size)
    queue_map: dict[str, tuple] = {}

    for src_id, cv, cs, cn, lv, ls, ln, mt in norm_results:
        if ln not in fn_set:
            continue
        # Unique if this lsw1_fn maps to only ONE crash name and vice versa
        if len(lsw1_to_crash_names[ln]) != 1 or len(crash_to_lsw1_fns[cn]) != 1:
            continue
        if cs < 0x20:
            continue
        is_os_sdk = any(cn.startswith(p) for p in OS_SDK_PREFIXES)
        is_nu = cn.startswith("Nu") or cn.startswith("instNu")
        if is_os_sdk and cs >= 0x40:
            conf = "MEDIUM"
        elif is_nu and cs >= 0x20:
            conf = "MEDIUM"
        else:
            conf = "LOW"
        if ln not in queue_map or src_priority.get(src_id, 9) < src_priority.get(queue_map[ln][0], 9):
            queue_map[ln] = (src_id, conf, cn, lv, cs, cv, ls)

    queue = sorted(queue_map.items(), key=lambda kv: (0 if kv[1][1] == "MEDIUM" else 1, kv[1][3]))

    med = sum(1 for _, v in queue if v[1] == "MEDIUM")
    low = sum(1 for _, v in queue if v[1] == "LOW")
    print(f"Queue: MEDIUM={med} LOW={low} (total {len(queue)})")

    with open(OUT_QUEUE, "w") as f:
        f.write("confidence\tsource\tcrash_name\tgc_fn\tgc_addr\tcrash_size\tcrash_addr\tgc_size\tnotes\n")
        for lsw1_fn, (src_id, conf, cn, lv, cs, cv, ls) in queue:
            is_nu = cn.startswith("Nu") or cn.startswith("instNu")
            is_os_sdk = any(cn.startswith(p) for p in OS_SDK_PREFIXES)
            module = "nu2_engine" if is_nu else ("sdk" if is_os_sdk else "gamecode")
            f.write(f"{conf}\t{src_id}\t{cn}\t{lsw1_fn}\t{lv:#010x}\t{cs:#06x}\t{cv:#010x}\t{ls:#06x}\tnorm-body-match; module={module}\n")
    print(f"Wrote {OUT_QUEUE}")


if __name__ == "__main__":
    main()
