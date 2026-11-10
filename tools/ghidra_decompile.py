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
    python3 tools/ghidra_decompile.py setup-pc              # opt-in: import the Japanese PC exe (~2 min)
    python3 tools/ghidra_decompile.py decompile-pc FN ...   # x86 pair of each GC function -> work/FN/jp_ghidra.c

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
sys.path.insert(0, str(ROOT / "tools"))


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
# Japanese PC 1.0 exe (docs/research.md#pc-builds): a second, opt-in project with paired GC names.
PC_PROJECT_DIR = GHIDRA_DIR / "pc_project"
PC_PROJECT_NAME = "lsw1pc"
PC_NAMES = GHIDRA_DIR / "pc_names.tsv"
PC_STAMP = GHIDRA_DIR / "pc_names.sha1"  # digest of the pc_names.tsv last applied
PC_SUFFIX = "_jpinferred"  # marks names from inferred (not confirmed/strings) pairs
PC_HEADER = ("/* Ghidra decompile of the Japanese PC 1.0 (MSVC x86) pair; reference only, never matching source.\n"
             " * PC struct offsets, argument passing and frame layout do not transfer to GC. Names ending in\n"
             f" * {PC_SUFFIX} come from inferred pairs; check them against GC assembly. Keep local. */\n")

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


def headless(args: list[str], quiet: bool = True, project: tuple[Path, str] | None = None) -> str:
    project_dir, project_name = project or (PROJECT_DIR, PROJECT_NAME)
    cmd = [find_analyze_headless(), str(project_dir), project_name, "-scriptPath", str(SCRIPTS_DIR), *args]
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
    if args.if_missing:  # bootstrap: never fail the toolchain setup over an optional reference
        if project_ready():
            return 0
        try:
            find_analyze_headless()
        except SystemExit:
            print("note: Ghidra not installed; skipping the reference project (docs/research.md#ghidra-and-rea)")
            return 0
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


# --------------------------------------------------------------------------- Japanese PC exe


def pc_ready() -> bool:
    return (PC_PROJECT_DIR / f"{PC_PROJECT_NAME}.gpr").exists() and (PC_PROJECT_DIR / f"{PC_PROJECT_NAME}.rep").is_dir()


def pc_names(tables, gc_names: dict[int, str]) -> str:
    """ApplyPcNames.java input: paired GC function names and GC global labels at x86 addresses."""
    import pc_gc_body
    lines = ["# x86 address\tname\tkind (generated by tools/ghidra_decompile.py)"]
    seen: set[int] = set()
    for gc_addr, (pc, kind, _) in sorted(tables.gc_to_pc.items()):
        name = gc_names.get(gc_addr, f"fn_{gc_addr:08X}")
        lines.append(f"0x{pc:08X}\t{name}{'' if kind in pc_gc_body.TRUSTED else PC_SUFFIX}\tfunction")
        seen.add(pc)
    for pc, (gc_addr, source) in sorted(tables.globals.items()):
        if pc not in seen:
            name = gc_names.get(gc_addr, f"lbl_{gc_addr:08X}")
            lines.append(f"0x{pc:08X}\t{name}{'' if source == 'confirmed' else PC_SUFFIX}\tlabel")
    return "\n".join(lines) + "\n"


def pc_context():
    """(exe, pair tables, {GC address: current name}); exits with a reason when unavailable."""
    import pc_gc_anchors
    import pc_gc_body
    exe = pc_gc_anchors.find_exe()
    if exe is None:
        raise SystemExit("error: the Japanese PC exe is not present here or in the primary checkout"
                         " (docs/research.md#pc-builds)")
    problem = pc_gc_body.ensure_tables(exe)
    if problem:
        raise SystemExit(f"error: {problem}")
    names = {a: n for n, _, a, _, _, _ in parse_symbols(SYMBOLS)}
    return exe, pc_gc_body.load_tables(), names


def write_pc_names(tables, names) -> list[str]:
    """Write pc_names.tsv; return the postScript args that apply it if the project lacks it."""
    text = pc_names(tables, names)
    digest = hashlib.sha1(text.encode()).hexdigest()
    PC_NAMES.parent.mkdir(parents=True, exist_ok=True)
    PC_NAMES.write_text(text)
    if PC_STAMP.exists() and PC_STAMP.read_text().strip() == digest:
        return []
    return ["-postScript", "ApplyPcNames.java", str(PC_NAMES)]


def cmd_setup_pc(args) -> int:
    check_location()
    exe, tables, names = pc_context()
    apply = write_pc_names(tables, names) or ["-postScript", "ApplyPcNames.java", str(PC_NAMES)]
    project = (PC_PROJECT_DIR, PC_PROJECT_NAME)
    if pc_ready() and not args.force:
        print(f"PC project exists at {display(PC_PROJECT_DIR)}; re-applying names (use --force to re-import)")
        headless(["-process", exe.name, "-noanalysis", *apply], quiet=False, project=project)
    else:
        if PC_PROJECT_DIR.exists():
            shutil.rmtree(PC_PROJECT_DIR)
        PC_PROJECT_DIR.mkdir(parents=True)
        print("importing and analysing the Japanese PC exe (about 2 minutes)...")
        headless(["-import", str(exe), "-overwrite", "-analysisTimeoutPerFile", str(args.analysis_timeout),
                  "-max-cpu", str(args.max_cpu), *apply], quiet=False, project=project)
    PC_STAMP.write_text(hashlib.sha1(PC_NAMES.read_bytes()).hexdigest())
    print(f"ready: {display(PC_PROJECT_DIR)}")
    return 0


def cmd_decompile_pc(args) -> int:
    if not pc_ready():
        raise SystemExit("error: no PC Ghidra project; run tools/ghidra_decompile.py setup-pc (about 2 minutes)")
    exe, tables, names = pc_context()
    by_name = {n: a for a, n in names.items()}
    functions = [(a, a + max(size, 4), n) for n, _, a, size, typ, _ in parse_symbols(SYMBOLS) if typ == "function"]
    out_dir = args.out_dir.resolve() if args.out_dir else None
    jobs, outputs, failed = [], [], 0
    for target in args.targets:
        if target.lower().startswith("0x"):
            addr = int(target, 16)
            hit = next(((lo, n) for lo, hi, n in functions if lo <= addr < hi), None)
            gc_addr, label = hit if hit else (addr, target)
        else:
            gc_addr, label = by_name.get(target), target
        pair = tables.gc_to_pc.get(gc_addr) if gc_addr is not None else None
        if pair is None:
            print(f"error: {target} has no JP PC pair (build/pc_analysis/gc_pc_anchors.tsv)")
            failed += 1
            continue
        label = re.sub(r"[^A-Za-z0-9_.-]", "_", label)
        dest = (out_dir / f"{label}.c") if out_dir else ROOT / "work" / label / "jp_ghidra.c"
        jobs.append(f"0x{pair[0]:08X}={dest}")
        outputs.append((dest, pair))
    if not jobs:
        return 1
    apply = write_pc_names(tables, names)
    log = headless(["-process", exe.name, "-noanalysis", *(apply or ["-readOnly"]), "-postScript",
                    "ExportFunctionDecomp.java", "--create", str(args.timeout), *jobs],
                   project=(PC_PROJECT_DIR, PC_PROJECT_NAME))
    if apply:
        PC_STAMP.write_text(hashlib.sha1(PC_NAMES.read_bytes()).hexdigest())
    for line in log.splitlines():
        if "ExportFunctionDecomp.java>" in line and ("wrote " in line or "error:" in line):
            msg = line.split("> ", 1)[-1].replace("(GhidraScript)", "").strip()
            failed += msg.startswith("error:")
            print(msg.replace(str(ROOT) + "/", ""))
    for dest, (pc, kind, evidence) in outputs:
        if dest.exists() and not dest.read_text().startswith(PC_HEADER[:20]):
            pairing = f"/* x86 0x{pc:08X}, pairing method {kind}: {evidence[:90]} */\n"
            dest.write_text(PC_HEADER + pairing + dest.read_text())
            if args.stdout:
                print(dest.read_text())
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
    print(f"pc:      {display(PC_PROJECT_DIR)} {'ready' if pc_ready() else 'missing (optional: run setup-pc)'}")
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command", required=True)
    p = sub.add_parser("elf", help="write main.elf (for Ghidra and REA) and print its path")
    p.set_defaults(func=cmd_elf)
    p = sub.add_parser("setup", help="write the ELF, import and analyse it")
    p.add_argument("--force", action="store_true", help="discard and re-import an existing project")
    p.add_argument("--if-missing", action="store_true",
                   help="do nothing if the project exists or Ghidra is not installed (bootstrap)")
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
    p = sub.add_parser("setup-pc", help="import the Japanese PC exe and apply paired GC names (opt-in)")
    p.add_argument("--force", action="store_true", help="discard and re-import an existing PC project")
    p.add_argument("--analysis-timeout", type=int, default=1800, help="seconds")
    p.add_argument("--max-cpu", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    p.set_defaults(func=cmd_setup_pc)
    p = sub.add_parser("decompile-pc", help="decompile the x86 pair of GC functions (names or 0x addresses)")
    p.add_argument("targets", nargs="+")
    p.add_argument("--out-dir", type=Path, help="write NAME.c here instead of work/NAME/jp_ghidra.c")
    p.add_argument("--timeout", type=int, default=60, help="seconds per function")
    p.add_argument("--stdout", action="store_true", help="also print the output")
    p.set_defaults(func=cmd_decompile_pc)
    p = sub.add_parser("xrefs", help="list references to symbols or 0x addresses (resolves r13/r2 data)")
    p.add_argument("targets", nargs="+")
    p.set_defaults(func=cmd_xrefs)
    p = sub.add_parser("status")
    p.set_defaults(func=cmd_status)
    args = ap.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
