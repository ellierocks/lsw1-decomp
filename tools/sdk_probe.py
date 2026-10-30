#!/usr/bin/env python3
"""Compile Dolphin SDK donor sources with MWCC and strictly verify them against retail.

LSW1 links Nintendo's April 2004 Dolphin SDK libraries, which were built with
CodeWarrior GC/1.2.5n, not ProDG. doldecomp/dolsdk2004 reconstructs those
libraries. This probe compiles donor files with the dolsdk2004 flags, anchors
each emitted function to a retail address and runs the strict verifier.

Anchoring: a function whose name is in symbols.txt anchors directly. Every
other function in the same object is placed by its offset from the nearest
anchored function (SDK objects are linked contiguously), and is accepted only
when symbols.txt has a function of the same size at that address.

Usage:
    python3 tools/sdk_probe.py --sdk ../dolsdk2004 os/OSAlarm vi/vi
    python3 tools/sdk_probe.py --sdk ../dolsdk2004 --all --tsv work/sdk_probe.tsv

Classes per function:
    MATCH        strict match, every reference verified
    UNVERIFIED   code identical; some references only mapped by consistency
    CODE_SAME    every instruction word identical outside relocated fields;
                 relocation names/targets disagree (usually missing symbol names)
    MISMATCH     code differs (version drift, flags or inlining)
    NO_ANCHOR    no retail address could be established
    ERROR        compiler or verifier error

The probe writes nothing under src/ or config/; it is discovery evidence for
importing SDK units, not promotion.
"""

from __future__ import annotations

import argparse
import contextlib
import io
import json
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import verify_fn as vf  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
MWCC = BUILD / "compilers" / "GC" / "1.2.5n" / "mwcceppc.exe"
WIBO = BUILD / "tools" / "wibo"
DTK = BUILD / "tools" / "dtk_patched"

# Same as doldecomp/dolsdk2004's Makefile release flags.
BASE_FLAGS = ["-nodefaults", "-proc", "gekko", "-fp", "hard", "-Cpp_exceptions", "off",
              "-enum", "int", "-warn", "pragmas", "-requireprotos", "-pragma", "cats off",
              "-O4,p", "-inline", "auto", "-D__GEKKO__", "-DSDK_REVISION=1", "-DRELEASE"]
SIGNED_CHAR = {"mtx/mtx", "mtx/mtx44", "card/CARDOpen"}
FLAG_OVERRIDES = {"exi/EXIBios": {"-O4,p": "-O3,p"}}


def compile_unit(sdk: Path, unit: str, out: Path) -> str:
    flags = [FLAG_OVERRIDES.get(unit, {}).get(f, f) for f in BASE_FLAGS]
    signed = unit.startswith("dvd/") or unit in SIGNED_CHAR
    flags += ["-char", "signed" if signed else "unsigned", "-I-",
              "-i", str(sdk / "include"), "-i", str(sdk / "include" / "libc")]
    for d in sorted((sdk / "src").iterdir()):
        if d.is_dir():
            flags += ["-i", str(d)]
    raw = out.with_suffix(".raw.o")
    cmd = [str(WIBO), str(MWCC), *flags, "-c", str(sdk / "src" / f"{unit}.c"), "-o", str(raw)]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return (r.stdout + r.stderr).strip()[-800:] or "compiler failed"
    r = subprocess.run([str(DTK), "elf", "fixup", str(raw), str(out)], capture_output=True, text=True)
    if r.returncode != 0:
        return f"dtk elf fixup failed: {r.stderr.strip()[-400:]}"
    return ""


def anchor(funcs: list[vf.Symbol], symbols: vf.SymbolTable) -> dict[str, tuple[str, str]]:
    """Map compiled function name -> (retail symbol name, how).

    Walks outward from each name anchor through the retail function list. A
    compiled function takes the next retail function when sizes agree;
    otherwise it is assumed dead-stripped by the linker and skipped.
    """
    retail = sorted((e.addr, n, e.size) for n, e in symbols.entries.items()
                    if e.is_function and e.section in vf.CODE_SECTIONS and n not in symbols.duplicates)
    index = {n: i for i, (_, n, _) in enumerate(retail)}
    funcs = sorted(funcs, key=lambda f: f.value)
    out: dict[str, tuple[str, str]] = {}
    for f in funcs:
        if f.name in index:
            out[f.name] = (f.name, "name")
    for i, f in enumerate(funcs):
        if out.get(f.name, ("", ""))[1] != "name":
            continue
        for step in (1, -1):
            r = index[f.name] + step
            j = i + step
            while 0 <= j < len(funcs) and 0 <= r < len(retail):
                g = funcs[j]
                if g.name in out:
                    break
                if retail[r][2] == g.size:
                    out[g.name] = (retail[r][1], f"layout from {f.name}")
                    r += step
                j += step
    return out


def run_verify(retail: str, compiled: str, obj: Path) -> dict:
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf), contextlib.redirect_stderr(io.StringIO()):
        vf.main([retail, "--object", str(obj), "--compiled-symbol", compiled,
                 "--strict-symbols", "--json"])
    try:
        return json.loads(buf.getvalue())
    except json.JSONDecodeError:
        return {"verdict": "ERROR", "error": buf.getvalue()[-300:]}


def classify(rep: dict) -> str:
    v = rep.get("verdict", "ERROR")
    if v == "ERROR":
        return "ERROR"
    if v == "MATCH":
        return "MATCH"
    if v.startswith("MATCH"):
        return "UNVERIFIED"
    diffs = rep.get("differences", [])
    if rep.get("target_insns") == rep.get("mine_insns") and diffs and \
            all(d.get("kind") == "reloc" for d in diffs):
        return "CODE_SAME"
    return "MISMATCH"


def probe(sdk: Path, unit: str, tmp: Path, symbols: vf.SymbolTable) -> list[dict]:
    obj = tmp / (unit.replace("/", "__") + ".o")
    err = compile_unit(sdk, unit, obj)
    if err:
        return [{"unit": unit, "function": "-", "retail": "-", "class": "ERROR", "detail": err}]
    elf = vf.Elf(obj)
    funcs = [f for f in elf.function_symbols() if f.size]
    anchors = anchor(funcs, symbols)
    rows = []
    for f in funcs:
        if f.name not in anchors:
            rows.append({"unit": unit, "function": f.name, "retail": "-", "class": "NO_ANCHOR",
                         "size": f.size, "detail": ""})
            continue
        retail, how = anchors[f.name]
        rep = run_verify(retail, f.name, obj)
        rows.append({"unit": unit, "function": f.name, "retail": retail, "class": classify(rep),
                     "size": f.size, "detail": how if not rep.get("error") else rep["error"]})
    return rows


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("units", nargs="*", help="SDK source units relative to src/, e.g. os/OSAlarm")
    ap.add_argument("--sdk", required=True, type=Path, help="dolsdk2004 checkout")
    ap.add_argument("--all", action="store_true", help="probe every .c under the SDK src/ tree")
    ap.add_argument("--tsv", type=Path, help="write per-function rows here")
    args = ap.parse_args(argv)
    for tool in (MWCC, WIBO, DTK):
        if not tool.exists():
            print(f"error: missing {tool}; run bash build.sh first", file=sys.stderr)
            return 2
    units = args.units
    if args.all:
        units = sorted(str(p.relative_to(args.sdk / "src").with_suffix(""))
                       for p in (args.sdk / "src").glob("*/*.c"))
    if not units:
        ap.error("give units or --all")
    symbols = vf.SymbolTable()
    rows: list[dict] = []
    with tempfile.TemporaryDirectory(prefix="sdk_probe_") as d:
        for unit in units:
            unit_rows = probe(args.sdk.resolve(), unit, Path(d), symbols)
            rows += unit_rows
            counts: dict[str, int] = {}
            for r in unit_rows:
                counts[r["class"]] = counts.get(r["class"], 0) + 1
            print(f"{unit:28} " + " ".join(f"{k}={v}" for k, v in sorted(counts.items())))
    total: dict[str, list[int]] = {}
    for r in rows:
        t = total.setdefault(r["class"], [0, 0])
        t[0] += 1
        t[1] += r.get("size", 0)
    print("\ntotal: " + ", ".join(f"{k} {n} fns/{b} bytes" for k, (n, b) in sorted(total.items())))
    if args.tsv:
        args.tsv.parent.mkdir(parents=True, exist_ok=True)
        cols = ["unit", "function", "retail", "class", "size", "detail"]
        lines = ["\t".join(cols)] + ["\t".join(str(r.get(c, "")).replace("\t", " ").replace("\n", " ")
                                               for c in cols) for r in rows]
        args.tsv.write_text("\n".join(lines) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
