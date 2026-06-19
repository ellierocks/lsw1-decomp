#!/usr/bin/env python3
"""
Match unnamed LSW1 GC functions to named Crash WoC GC functions via DOL body comparison.

Crash WoC has a full debug ELF (crashwoc.elf) with 2,671 named functions.
Both games use Nu2 engine compiled with Metrowerks CodeWarrior GC/1.3.2,
so many engine functions are byte-for-byte identical or near-identical.

Two matching modes:
  exact    — MD5 hash of raw function bytes must match
  norm     — normalise branch targets and r13/r2 SDA offsets before hashing

Outputs:
  docs/symbol_donors/crashwoc_fn_matches.tsv   — all raw match data
  docs/symbol_donors/crashwoc_fn_rename_queue.tsv — filtered rename candidates
"""
from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

CRASH_DOL  = ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/sys/main.dol"
CRASH_ELF  = ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/files/crashwoc.elf"
LSW1_DOL   = ROOT / "orig/GL5E4F/sys/main.dol"
SYMBOLS_TXT = ROOT / "config/GL5E4F/symbols.txt"

OUT_TSV   = ROOT / "docs/symbol_donors/crashwoc_fn_matches.tsv"
OUT_MD    = ROOT / "docs/symbol_donors/crashwoc_fn_matches.md"
OUT_QUEUE = ROOT / "docs/symbol_donors/crashwoc_fn_rename_queue.tsv"

OS_SDK_PREFIXES = (
    "OS", "__OS", "AR", "__AR", "DVD", "__DVD", "CARD", "__CARD",
    "DSP", "__DSP", "VI", "GX", "__GX", "AI", "__AI", "EXI", "__EXI",
    "VerifyFAT", "VerifyDir", "entryToPath", "DBWrite", "InsertAlarm",
    "DecrementerException", "WriteUARTN", "DBOpen", "DBClose",
)

# ---------------------------------------------------------------------------
# DOL reader: map virtual address → bytes
# ---------------------------------------------------------------------------

class DolReader:
    def __init__(self, path: Path):
        self.data = path.read_bytes()
        hdr = self.data
        self.sections: list[tuple[int, int, int]] = []  # (file_off, vma, size)
        for i in range(7):   # text sections
            off  = struct.unpack_from(">I", hdr, i*4)[0]
            vma  = struct.unpack_from(">I", hdr, 0x48 + i*4)[0]
            size = struct.unpack_from(">I", hdr, 0x90 + i*4)[0]
            if off and vma and size:
                self.sections.append((off, vma, size))
        for i in range(11):  # data sections
            off  = struct.unpack_from(">I", hdr, 0x1C + i*4)[0]
            vma  = struct.unpack_from(">I", hdr, 0x64 + i*4)[0]
            size = struct.unpack_from(">I", hdr, 0xAC + i*4)[0]
            if off and vma and size:
                self.sections.append((off, vma, size))

    def read_vma(self, vma: int, size: int) -> bytes | None:
        for (foff, sec_vma, sec_size) in self.sections:
            if sec_vma <= vma < sec_vma + sec_size:
                delta = vma - sec_vma
                if delta + size > sec_size:
                    return None
                return self.data[foff + delta : foff + delta + size]
        return None


# ---------------------------------------------------------------------------
# ELF reader: extract named functions
# ---------------------------------------------------------------------------

def load_crash_symbols(elf_path: Path) -> list[tuple[int, int, str]]:
    """Return [(vma, size, name)] for all STT_FUNC symbols in crashwoc.elf."""
    e = elf_path.read_bytes()
    u32 = lambda off: struct.unpack_from(">I", e, off)[0]
    u16 = lambda off: struct.unpack_from(">H", e, off)[0]

    shoff    = u32(32)
    shnum    = u16(48)
    shstrndx = u16(50)

    def shdr(i):
        o = shoff + i * 40
        return {
            "name_off": u32(o),
            "type":     u32(o + 4),
            "offset":   u32(o + 16),
            "size":     u32(o + 20),
            "link":     u32(o + 24),
            "entsize":  u32(o + 36),
        }

    shstr_s = shdr(shstrndx)
    shstr = e[shstr_s["offset"]: shstr_s["offset"] + shstr_s["size"]]

    symtab_s = strtab_s = None
    for i in range(shnum):
        s = shdr(i)
        if s["type"] == 2:   # SHT_SYMTAB
            symtab_s = s
        name = shstr[s["name_off"]: shstr.index(b"\x00", s["name_off"])].decode()
        if name == ".strtab":
            strtab_s = s

    if not symtab_s or not strtab_s:
        return []

    strtab = e[strtab_s["offset"]: strtab_s["offset"] + strtab_s["size"]]

    def str_at(off):
        end = strtab.index(b"\x00", off)
        return strtab[off:end].decode("ascii", errors="replace")

    funcs = []
    n = symtab_s["size"] // 16
    base = symtab_s["offset"]
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
# LSW1 symbol table: unnamed fn_* functions
# ---------------------------------------------------------------------------

SYM_RE = re.compile(
    r'^(fn_[0-9A-Fa-f]+)\s*=\s*\.[a-z0-9]+:0x([0-9A-Fa-f]+);\s*//.*?size:0x([0-9A-Fa-f]+)'
)

def load_lsw1_unnamed(sym_path: Path) -> list[tuple[int, int, str]]:
    """Return [(vma, size, 'fn_XXXXXXXX')] for all fn_* entries."""
    funcs = []
    for line in sym_path.read_text().splitlines():
        m = SYM_RE.match(line)
        if m:
            name = m.group(1)
            vma  = int(m.group(2), 16)
            size = int(m.group(3), 16)
            if size >= 4:
                funcs.append((vma, size, name))
    return funcs


# ---------------------------------------------------------------------------
# Normalisation: mask out branch targets and SDA offsets
# ---------------------------------------------------------------------------

def normalise(code: bytes) -> bytes:
    """
    Replace branch immediate offsets and r13/r2 SDA load immediates with zeros
    so that functions differing only in data/branch targets still match.
    """
    out = bytearray(code)
    n = len(code) // 4
    for i in range(n):
        w = struct.unpack_from(">I", code, i * 4)[0]
        op = w >> 26
        # b / bl / ba / bla (op=18): clear lower 26 bits (AA=0 assumed)
        if op == 18:
            struct.pack_into(">I", out, i * 4, w & 0xFC000003)
        # bc / bcl (op=16): clear lower 16 bits BD field
        elif op == 16:
            struct.pack_into(">I", out, i * 4, w & 0xFFFF0003)
        # lis  (op=15, addis rD,0,imm): keep as-is (hi16 of address)
        # addi (op=14, rA=r13 or r2): SDA-relative load — zero immediate
        elif op == 14:
            rA = (w >> 16) & 0x1F
            if rA in (2, 13):
                struct.pack_into(">I", out, i * 4, w & 0xFFFF0000)
        # lwz/stw (op=32/36) rA=r13/r2: SDA load — zero displacement
        elif op in (32, 33, 34, 36, 37, 38, 40, 44):
            rA = (w >> 16) & 0x1F
            if rA in (2, 13):
                struct.pack_into(">I", out, i * 4, w & 0xFFFF0000)
    return bytes(out)


def md5(b: bytes) -> str:
    return hashlib.md5(b).hexdigest()


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print("Loading Crash WoC symbols from ELF …")
    crash_syms = load_crash_symbols(CRASH_ELF)
    print(f"  {len(crash_syms)} named functions")

    print("Loading Crash WoC DOL …")
    crash_dol = DolReader(CRASH_DOL)

    print("Loading LSW1 DOL …")
    lsw1_dol = DolReader(LSW1_DOL)

    print("Loading LSW1 unnamed functions …")
    lsw1_fns = load_lsw1_unnamed(SYMBOLS_TXT)
    print(f"  {len(lsw1_fns)} unnamed fn_* entries")

    # Build hash indices for LSW1
    print("Fingerprinting LSW1 functions …")
    lsw1_exact: dict[str, list[tuple[int, int, str]]] = {}  # hash → list of (vma,size,name)
    lsw1_norm:  dict[str, list[tuple[int, int, str]]] = {}
    for vma, size, name in lsw1_fns:
        code = lsw1_dol.read_vma(vma, size)
        if code is None:
            continue
        h = md5(code)
        lsw1_exact.setdefault(h, []).append((vma, size, name))
        hn = md5(normalise(code))
        lsw1_norm.setdefault(hn, []).append((vma, size, name))

    # Match Crash WoC → LSW1
    print("Matching Crash WoC → LSW1 …")
    results = []  # (crash_vma, crash_size, crash_name, lsw1_vma, lsw1_name, match_type)

    for crash_vma, crash_size, crash_name in crash_syms:
        code = crash_dol.read_vma(crash_vma, crash_size)
        if code is None:
            continue
        h = md5(code)
        hits_exact = lsw1_exact.get(h, [])
        if hits_exact:
            for lsw1_vma, lsw1_size, lsw1_name in hits_exact:
                results.append((crash_vma, crash_size, crash_name,
                                 lsw1_vma, lsw1_size, lsw1_name, "exact"))
            continue
        # Try normalised
        hn = md5(normalise(code))
        hits_norm = lsw1_norm.get(hn, [])
        for lsw1_vma, lsw1_size, lsw1_name in hits_norm:
            results.append((crash_vma, crash_size, crash_name,
                             lsw1_vma, lsw1_size, lsw1_name, "norm"))

    # Sort: exact first, then by crash address
    results.sort(key=lambda r: (r[6] != "exact", r[0]))

    exact_count = sum(1 for r in results if r[6] == "exact")
    norm_count  = sum(1 for r in results if r[6] == "norm")
    print(f"\nMatches: {exact_count} exact, {norm_count} normalised")

    # Write TSV
    header = "match_type\tcrash_name\tcrash_addr\tcrash_size\tlsw1_fn\tlsw1_addr\tlsw1_size\n"
    rows = []
    for crash_vma, crash_size, crash_name, lsw1_vma, lsw1_size, lsw1_name, mtype in results:
        rows.append(
            f"{mtype}\t{crash_name}\t{crash_vma:#010x}\t{crash_size:#06x}"
            f"\t{lsw1_name}\t{lsw1_vma:#010x}\t{lsw1_size:#06x}\n"
        )
    OUT_TSV.write_text(header + "".join(rows))
    print(f"Wrote {OUT_TSV}")

    # Write markdown summary
    nu_exact  = [(r) for r in results if r[6] == "exact" and r[2].startswith("Nu")]
    nu_norm   = [(r) for r in results if r[6] == "norm"  and r[2].startswith("Nu")]
    all_exact = [r for r in results if r[6] == "exact"]
    all_norm  = [r for r in results if r[6] == "norm"]

    lines = [
        "# Crash WoC → LSW1 Function Match Report\n\n",
        f"**Crash WoC named functions scanned**: {len(crash_syms)}  \n",
        f"**LSW1 unnamed fn_* functions**: {len(lsw1_fns)}  \n",
        f"**Exact matches**: {exact_count} (raw bytes identical)  \n",
        f"**Normalised matches**: {norm_count} (branch/SDA offsets masked)  \n",
        f"**Nu2 engine exact**: {len(nu_exact)}  \n",
        f"**Nu2 engine normalised**: {len(nu_norm)}  \n\n",
    ]

    def section(title, rows, limit=60):
        lines.append(f"## {title}\n\n")
        lines.append("| match | crash_name | crash_addr | lsw1_fn | lsw1_addr | size |\n")
        lines.append("|---|---|---|---|---|---|\n")
        for r in rows[:limit]:
            lines.append(
                f"| {r[6]} | {r[2]} | {r[0]:#010x} | {r[5]} | {r[3]:#010x} | {r[1]:#06x} |\n"
            )
        if len(rows) > limit:
            lines.append(f"\n*(+{len(rows)-limit} more — see TSV)*\n")
        lines.append("\n")

    section("Nu2 Engine Matches (exact)", nu_exact)
    section("Nu2 Engine Matches (normalised)", nu_norm)
    section("All Exact Matches (non-Nu)", [r for r in all_exact if not r[2].startswith("Nu")])

    OUT_MD.write_text("".join(lines))
    print(f"Wrote {OUT_MD}")

    # Build rename queue from current fn_* set
    print("\nBuilding rename queue …")
    fn_set: set[str] = set()
    sym_re = re.compile(r"^(fn_[0-9A-Fa-f]+)\s*=")
    for line in SYMBOLS_TXT.read_text().splitlines():
        m = sym_re.match(line)
        if m:
            fn_set.add(m.group(1))

    lsw1_counts_norm: dict[str, int] = {}
    crash_counts_norm: dict[str, int] = {}
    for crash_vma, crash_size, crash_name, lsw1_vma, lsw1_size, lsw1_name, mtype in results:
        if mtype == "norm":
            lsw1_counts_norm[lsw1_name] = lsw1_counts_norm.get(lsw1_name, 0) + 1
            crash_counts_norm[crash_name] = crash_counts_norm.get(crash_name, 0) + 1

    applied = {r[2] for r in results if r[6] == "exact"
               and r[2].startswith("Nu")
               and crash_counts_norm.get(r[2], 1) == 1}

    queue: list[tuple] = []
    for crash_vma, crash_size, crash_name, lsw1_vma, lsw1_size, lsw1_name, mtype in results:
        if mtype != "norm":
            continue
        if crash_name in applied:
            continue
        if lsw1_name not in fn_set:
            continue
        if lsw1_counts_norm.get(lsw1_name, 0) != 1:
            continue
        if crash_counts_norm.get(crash_name, 0) != 1:
            continue
        if crash_size < 0x20:
            continue
        is_os_sdk = any(crash_name.startswith(p) for p in OS_SDK_PREFIXES)
        is_nu = crash_name.startswith("Nu") or crash_name.startswith("instNu")
        if is_os_sdk and crash_size >= 0x40:
            conf = "MEDIUM"
        elif is_nu and crash_size >= 0x20:
            conf = "MEDIUM"
        else:
            conf = "LOW"
        module = "nu2_engine" if is_nu else ("sdk" if is_os_sdk else "gamecode")
        queue.append((conf, crash_name, lsw1_name, lsw1_vma, crash_size,
                      crash_vma, lsw1_size, f"norm-body-match; module={module}"))

    queue.sort(key=lambda x: (0 if x[0] == "MEDIUM" else 1, x[3]))

    with open(OUT_QUEUE, "w") as f:
        f.write("confidence\tcrash_name\tgc_fn\tgc_addr\tcrash_size\tcrash_addr\tgc_size\tnotes\n")
        for row in queue:
            conf, cname, lname, lvma, csize, cvma, lsize, note = row
            f.write(f"{conf}\t{cname}\t{lname}\t{lvma:#010x}\t{csize:#06x}\t{cvma:#010x}\t{lsize:#06x}\t{note}\n")

    med = sum(1 for q in queue if q[0] == "MEDIUM")
    low = sum(1 for q in queue if q[0] == "LOW")
    print(f"Queue: MEDIUM={med} LOW={low}")
    print(f"Wrote {OUT_QUEUE}")


if __name__ == "__main__":
    main()
