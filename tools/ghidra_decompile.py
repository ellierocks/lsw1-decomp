#!/usr/bin/env python3
"""Ghidra reference decompiles for the retail DOL, plus the ELF that REA opens.

Ghidra cannot load a DOL with names, so `elf` writes `main.elf`: the retail segments as
named sections (.init/.text/.rodata/.data/.sdata/.sdata2/.bss/.sbss), every
`config/GL5E4F/symbols.txt` name with its size and type in `.symtab`, and `_SDA_BASE_` /
`_SDA2_BASE_` read from the retail r13/r2 setup in `__start`. Ghidra's PowerPC ELF loader
uses those to resolve small-data accesses. REA opens the same file (see docs/research.md).

    python3 tools/ghidra_decompile.py setup                 # ELF + import + analysis (minutes, once)
    python3 tools/ghidra_decompile.py decompile FN1 FN2 ... # one JVM; writes work/FN/ghidra.c
    python3 tools/ghidra_decompile.py sync                  # re-apply names after symbols.txt changes
    python3 tools/ghidra_decompile.py status

State lives per checkout in `<primary checkout>/build/ghidra/<checkout>-<hash>/` (`status`
prints it; `LSW1_GHIDRA_DIR` overrides it but must have no '.'-prefixed path element). `analyzeHeadless` comes from PATH, `GHIDRA_INSTALL_DIR` or /opt/ghidra.
Ghidra output is a control-flow reference only, never matching source.
"""
from __future__ import annotations

import argparse
import hashlib
import os
import re
import shutil
import struct
import subprocess
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOL = ROOT / "orig/GL5E4F/sys/main.dol"
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
SCRIPTS_DIR = ROOT / "tools/ghidra_scripts"


def primary_checkout() -> Path:
    """The main checkout; a worktree's `.git` file points into `<primary>/.git/worktrees/`."""
    dot_git = ROOT / ".git"
    if dot_git.is_file():
        gitdir = dot_git.read_text().strip().removeprefix("gitdir:").strip()
        if "/.git/" in gitdir:
            return Path(gitdir.split("/.git/", 1)[0])
    return ROOT


# Ghidra rejects project locations with a '.'-prefixed element (~/.cache, .claude/worktrees),
# so every checkout keeps its own directory in the primary checkout's ignored build/.
GHIDRA_DIR = Path(os.environ.get("LSW1_GHIDRA_DIR") or primary_checkout() / "build/ghidra"
                  / f"{ROOT.name}-{hashlib.sha1(str(ROOT).encode()).hexdigest()[:8]}")
ELF = GHIDRA_DIR / "main.elf"
PROJECT_DIR = GHIDRA_DIR / "project"
PROJECT_NAME = "lsw1"
PROGRAM = ELF.name
STAMP = GHIDRA_DIR / "names.sha1"  # symbols.txt digest last applied to the project

SYMBOL_LINE = re.compile(
    r"^(?P<name>[^\s=]+)\s*=\s*(?P<sec>\.[A-Za-z0-9_]+):0x(?P<addr>[0-9A-Fa-f]+);\s*//(?P<attrs>.*)$")


# --------------------------------------------------------------------------- ELF


def parse_dol(data: bytes):
    """Return [(addr, file_offset, size, is_code)] for loaded segments, and (bss_addr, bss_size, entry)."""
    offs = struct.unpack_from(">18I", data, 0x00)
    addrs = struct.unpack_from(">18I", data, 0x48)
    sizes = struct.unpack_from(">18I", data, 0x90)
    segs = [(addrs[i], offs[i], sizes[i], i < 7) for i in range(18) if offs[i] and sizes[i]]
    return segs, struct.unpack_from(">III", data, 0xD8)


def read_word(data: bytes, segs, addr: int) -> int | None:
    for base, off, size, _ in segs:
        if base <= addr < base + size:
            return struct.unpack_from(">I", data, off + addr - base)[0]
    return None


def sda_bases(data: bytes, segs, entry: int) -> dict[int, int]:
    """Scan the entry code for `lis rN,hi` then `ori/addi rN,rN,lo` with N in (2, 13)."""
    regs: dict[int, int] = {}
    hi: dict[int, int] = {}
    for i in range(0x100):
        w = read_word(data, segs, entry + 4 * i)
        if w is None:
            break
        op, rd, ra, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
        if rd not in (2, 13) or rd in regs:
            continue
        if op == 15 and ra == 0:  # lis
            hi[rd] = imm << 16
        elif op == 24 and ra == rd and rd in hi:  # ori
            regs[rd] = hi[rd] | imm
        elif op == 14 and ra == rd and rd in hi:  # addi
            regs[rd] = (hi[rd] + (imm - 0x10000 if imm & 0x8000 else imm)) & 0xFFFFFFFF
        if len(regs) == 2:
            break
    return regs


def parse_symbols(path: Path):
    out = []
    for line in path.read_text().splitlines():
        m = SYMBOL_LINE.match(line.strip())
        if not m:
            continue
        attrs = dict(a.split(":", 1) for a in m["attrs"].split() if ":" in a)
        out.append((m["name"], m["sec"], int(m["addr"], 16), int(attrs.get("size", "0"), 16),
                    attrs.get("type", "label"), attrs.get("scope", "global")))
    return out


def build_elf(dol_path: Path, symbols_path: Path) -> bytes:
    data = dol_path.read_bytes()
    segs, (bss_addr, bss_size, entry) = parse_dol(data)
    symbols = parse_symbols(symbols_path)

    def section_name(lo: int, hi: int, default: str) -> str:
        votes = Counter(sec for _, sec, a, _, _, _ in symbols if lo <= a < hi)
        return votes.most_common(1)[0][0] if votes else default

    # (name, addr, size, file_offset|None, flags)  SHF_WRITE=1 SHF_ALLOC=2 SHF_EXECINSTR=4
    sections = []
    for addr, off, size, code in segs:
        name = section_name(addr, addr + size, ".text" if code else ".data")
        flags = 6 if code else (2 if name in (".rodata", ".sdata2") else 3)
        sections.append([name, addr, size, off, flags])
    # The DOL bss range also covers .sdata; the uncovered gaps are .bss and .sbss.
    cursor, end = bss_addr, bss_addr + bss_size
    for addr, _, size, _ in sorted(segs):
        if addr + size <= cursor or addr >= end:
            continue
        if addr > cursor:
            sections.append([section_name(cursor, addr, ".bss"), cursor, addr - cursor, None, 3])
        cursor = max(cursor, addr + size)
    if cursor < end:
        sections.append([section_name(cursor, end, ".bss"), cursor, end - cursor, None, 3])
    sections.sort(key=lambda s: s[1])

    def shndx(addr: int) -> int:
        for i, (_, a, size, _, _) in enumerate(sections, 1):
            if a <= addr < a + size:
                return i
        return 0xFFF1  # SHN_ABS

    strtab = bytearray(b"\0")

    def add_str(table: bytearray, s: str) -> int:
        idx = len(table)
        table += s.encode() + b"\0"
        return idx

    syms_local, syms_global = [], []
    for name, _, addr, size, typ, scope in symbols:
        st_type = 2 if typ == "function" else 1 if typ == "object" else 0  # FUNC / OBJECT / NOTYPE
        bind = {"local": 0, "weak": 2}.get(scope, 1)
        entry_ = (add_str(strtab, name), addr, size, (bind << 4) | st_type, shndx(addr))
        (syms_local if bind == 0 else syms_global).append(entry_)
    for reg, name in ((13, "_SDA_BASE_"), (2, "_SDA2_BASE_")):
        value = sda_bases(data, segs, entry).get(reg)
        if value is not None:
            syms_global.append((add_str(strtab, name), value, 0, (1 << 4) | 0, 0xFFF1))
    symtab = struct.pack(">IIIBBH", 0, 0, 0, 0, 0, 0)
    for nm, value, size, info, idx in syms_local + syms_global:
        symtab += struct.pack(">IIIBBH", nm, value, size, info, 0, idx)

    shstr = bytearray(b"\0")
    names = [add_str(shstr, s[0]) for s in sections]
    n_symtab, n_strtab, n_shstr = add_str(shstr, ".symtab"), add_str(shstr, ".strtab"), add_str(shstr, ".shstrtab")

    loaded = [s for s in sections if s[3] is not None]
    phnum = len(sections)
    out = bytearray(52 + 32 * phnum)
    file_offsets = {}
    for s in loaded:
        out += b"\0" * (-len(out) % 32)
        file_offsets[s[1]] = len(out)
        out += data[s[3]:s[3] + s[2]]
    out += b"\0" * (-len(out) % 4)
    symtab_off = len(out); out += symtab
    strtab_off = len(out); out += strtab
    shstr_off = len(out); out += shstr
    out += b"\0" * (-len(out) % 4)
    shoff = len(out)

    struct.pack_into(">4sBBBB8xHHIIIIIHHHHHH", out, 0, b"\x7fELF", 1, 2, 1, 0,
                     2, 20, 1, entry, 52, shoff, 0, 52, 32, phnum, 40, len(sections) + 4,
                     len(sections) + 3)
    for i, (name, addr, size, off, flags) in enumerate(sections):
        p_flags = (1 if flags & 4 else 0) | 4 | (2 if flags & 1 else 0)
        fo = file_offsets.get(addr, shoff)
        struct.pack_into(">8I", out, 52 + 32 * i, 1, fo, addr, addr, 0 if off is None else size, size, p_flags, 32)
    sh = struct.pack(">10I", *([0] * 10))
    for i, (name, addr, size, off, flags) in enumerate(sections):
        sh += struct.pack(">10I", names[i], 1 if off is not None else 8, flags, addr,
                          file_offsets.get(addr, shoff), size, 0, 0, 32, 0)
    strtab_idx = len(sections) + 2
    sh += struct.pack(">10I", n_symtab, 2, 0, 0, symtab_off, len(symtab), strtab_idx, len(syms_local) + 1, 4, 16)
    sh += struct.pack(">10I", n_strtab, 3, 0, 0, strtab_off, len(strtab), 0, 0, 1, 0)
    sh += struct.pack(">10I", n_shstr, 3, 0, 0, shstr_off, len(shstr), 0, 0, 1, 0)
    out += sh
    return bytes(out)


def symbols_digest() -> str:
    return hashlib.sha1(SYMBOLS.read_bytes()).hexdigest()


def ensure_elf(force: bool = False) -> Path:
    if not DOL.exists():
        raise SystemExit(f"error: {DOL.relative_to(ROOT)} missing; run bash tools/bootstrap.sh")
    newest = max(DOL.stat().st_mtime, SYMBOLS.stat().st_mtime, Path(__file__).stat().st_mtime)
    if force or not ELF.exists() or ELF.stat().st_mtime < newest:
        ELF.parent.mkdir(parents=True, exist_ok=True)
        ELF.write_bytes(build_elf(DOL, SYMBOLS))
        print(f"wrote {display(ELF)}")
    return ELF


# --------------------------------------------------------------------------- headless


def display(path: Path) -> str:
    return str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path)


def find_analyze_headless() -> str:
    candidates = [shutil.which("analyzeHeadless"), shutil.which("ghidra-analyzeHeadless")]
    for base in (os.environ.get("GHIDRA_INSTALL_DIR"), "/opt/ghidra"):
        if base:
            candidates.append(str(Path(base) / "support/analyzeHeadless"))
    for c in candidates:
        if c and Path(c).exists():
            return c
    raise SystemExit("error: analyzeHeadless not found; install Ghidra and set GHIDRA_INSTALL_DIR")


def project_ready() -> bool:
    return (PROJECT_DIR / f"{PROJECT_NAME}.gpr").exists() and (PROJECT_DIR / f"{PROJECT_NAME}.rep").is_dir()


def headless(args: list[str], quiet: bool = True) -> str:
    cmd = [find_analyze_headless(), str(PROJECT_DIR), PROJECT_NAME, "-scriptPath", str(SCRIPTS_DIR), *args]
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    log = r.stdout + r.stderr
    (GHIDRA_DIR / "last_headless.log").write_text(log)
    lines = [l for l in log.splitlines() if "(GhidraScript)" in l or "ERROR" in l or "SEVERE" in l]
    if not quiet or r.returncode:
        print("\n".join(lines) if lines else log[-2000:])
    if r.returncode:
        raise SystemExit(f"error: analyzeHeadless exited {r.returncode}; see {display(GHIDRA_DIR / 'last_headless.log')}")
    if "Program is locked" in log or "Unable to lock project" in log:
        raise SystemExit("error: Ghidra project is locked by another process (close the GUI or wait)")
    return log


def check_location() -> None:
    bad = [part for part in GHIDRA_DIR.parts if part.startswith(".")]
    if bad:
        raise SystemExit(f"error: Ghidra rejects project paths with '.'-prefixed elements ({bad[0]}); "
                         "set LSW1_GHIDRA_DIR to a path without them")


def cmd_setup(args) -> int:
    check_location()
    ensure_elf(force=True)
    if project_ready() and not args.force:
        print(f"project exists at {display(PROJECT_DIR)}; running sync (use --force to re-import)")
        return cmd_sync(args)
    if PROJECT_DIR.exists():
        shutil.rmtree(PROJECT_DIR)
    PROJECT_DIR.mkdir(parents=True)
    print("importing and analysing main.elf (several minutes)...")
    headless(["-import", str(ELF), "-overwrite", "-processor", "PowerPC:BE:32:default",
              "-preScript", "SetSdaRegisters.java", "-analysisTimeoutPerFile", str(args.analysis_timeout),
              "-max-cpu", str(args.max_cpu), "-postScript", "ApplySymbolsTxt.java", str(SYMBOLS)], quiet=False)
    STAMP.write_text(symbols_digest())
    print(f"ready: {display(PROJECT_DIR)}")
    return 0


def cmd_sync(args) -> int:
    ensure_elf()
    if not project_ready():
        raise SystemExit("error: no Ghidra project; run tools/ghidra_decompile.py setup")
    headless(["-process", PROGRAM, "-noanalysis", "-postScript", "ApplySymbolsTxt.java", str(SYMBOLS)], quiet=False)
    STAMP.write_text(symbols_digest())
    return 0


def cmd_decompile(args) -> int:
    if not project_ready():
        raise SystemExit("error: no Ghidra project; run tools/ghidra_decompile.py setup")
    sync = []
    if not STAMP.exists() or STAMP.read_text().strip() != symbols_digest():
        sync = ["-postScript", "ApplySymbolsTxt.java", str(SYMBOLS)]
    out_dir = args.out_dir.resolve() if args.out_dir else None
    functions = [(a, a + max(size, 4), name) for name, _, a, size, typ, _ in parse_symbols(SYMBOLS) if typ == "function"]
    jobs = []
    for target in args.targets:
        label = target
        if target.lower().startswith("0x"):
            addr = int(target, 16)
            label = next((n for lo, hi, n in functions if lo <= addr < hi), target)
        label = re.sub(r"[^A-Za-z0-9_.-]", "_", label)
        dest = (out_dir / f"{label}.c") if out_dir else ROOT / "work" / label / "ghidra.c"
        jobs.append(f"{target}={dest}")
    log = headless(["-process", PROGRAM, "-noanalysis", *(sync or ["-readOnly"]),
                    "-postScript", "ExportFunctionDecomp.java", str(args.timeout), *jobs])
    if sync:
        STAMP.write_text(symbols_digest())
    failed = 0
    for line in log.splitlines():
        if "ExportFunctionDecomp.java>" in line and ("wrote " in line or "error:" in line):
            msg = line.split("> ", 1)[-1].replace("(GhidraScript)", "").strip()
            failed += msg.startswith("error:")
            print(msg.replace(str(ROOT) + "/", ""))
    if args.stdout:
        for job in jobs:
            p = Path(job.split("=", 1)[1])
            if p.exists():
                print(p.read_text())
    return 1 if failed else 0


def cmd_xrefs(args) -> int:
    """References to globals, including r13/r2 small-data accesses that REA's import misses."""
    if not project_ready():
        raise SystemExit("error: no Ghidra project; run tools/ghidra_decompile.py setup")
    log = headless(["-process", PROGRAM, "-noanalysis", "-readOnly", "-postScript", "ListReferences.java", *args.targets])
    failed = 0
    for line in log.splitlines():
        if "ListReferences.java>" in line:
            msg = line.split("> ", 1)[-1].replace("(GhidraScript)", "").strip()
            failed += msg.startswith("error:")
            print(msg.removeprefix("xref "))
    return 1 if failed else 0


def cmd_elf(args) -> int:
    print(ensure_elf(force=True))
    return 0


def cmd_status(args) -> int:
    print(f"analyzeHeadless: {find_analyze_headless()}")
    print(f"elf:     {ELF} {'present' if ELF.exists() else 'missing'} (open this in REA)")
    print(f"project: {display(PROJECT_DIR)} {'ready' if project_ready() else 'missing (run setup)'}")
    if project_ready():
        current = STAMP.exists() and STAMP.read_text().strip() == symbols_digest()
        print(f"names:   {'current' if current else 'stale (decompile re-syncs automatically)'}")
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command", required=True)
    p = sub.add_parser("elf", help="write main.elf (for Ghidra and REA) and print its path")
    p.set_defaults(func=cmd_elf)
    p = sub.add_parser("setup", help="write the ELF, import and analyse it")
    p.add_argument("--force", action="store_true", help="discard and re-import an existing project")
    p.add_argument("--analysis-timeout", type=int, default=1800, help="seconds")
    p.add_argument("--max-cpu", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    p.set_defaults(func=cmd_setup)
    p = sub.add_parser("sync", help="re-apply symbols.txt names to the project")
    p.set_defaults(func=cmd_sync)
    p = sub.add_parser("decompile", help="export decompiler output (names or 0x addresses)")
    p.add_argument("targets", nargs="+")
    p.add_argument("--out-dir", type=Path, help="write NAME.c here instead of work/NAME/ghidra.c")
    p.add_argument("--timeout", type=int, default=60, help="seconds per function")
    p.add_argument("--stdout", action="store_true", help="also print the output")
    p.set_defaults(func=cmd_decompile)
    p = sub.add_parser("xrefs", help="list references to symbols or 0x addresses (resolves r13/r2 data)")
    p.add_argument("targets", nargs="+")
    p.set_defaults(func=cmd_xrefs)
    p = sub.add_parser("status")
    p.set_defaults(func=cmd_status)
    args = ap.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
