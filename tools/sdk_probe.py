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
    python3 tools/sdk_probe.py os/OSAlarm vi/vi
    python3 tools/sdk_probe.py --all --tsv work/sdk_probe.tsv
    python3 tools/sdk_probe.py --pending --tsv work/sdk_pending.tsv

The donor defaults to $DOLSDK2004, else $LSW1_CACHE/dolsdk2004 (default
~/.cache/lsw1/dolsdk2004); tools/fetch_dolsdk.sh clones the pinned commit there.
--pending probes only units not yet under src/dolphin/ and ranks them by how much of
each is already anchored to retail, which is the import queue.

Classes per function:
    MATCH        strict match, every reference verified
    UNVERIFIED   code identical; some references only mapped by consistency
    CODE_SAME    every instruction word identical outside relocated fields;
                 relocation names/targets disagree (usually missing symbol names)
    MISMATCH     code differs (version drift, flags or inlining)
    NO_ANCHOR    no retail address could be established
    ERROR        compiler or verifier error

With --strip-imported, already imported units are stripped against their DTK
objects first, so their complete sections can prove address-taken data. This
mode reports retained functions only; the default remains donor discovery. With
--all it skips units that are not imported yet.

The probe writes nothing under src/ or config/; it is discovery evidence for
importing SDK units, not promotion.
"""

from __future__ import annotations

import argparse
import contextlib
import io
import json
import os
import subprocess
import sys
import tempfile
from collections.abc import Callable
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


def default_sdk() -> Path:
    cache = Path(os.environ.get("LSW1_CACHE", Path.home() / ".cache" / "lsw1"))
    return Path(os.environ.get("DOLSDK2004", cache / "dolsdk2004"))


def is_imported(unit: str) -> bool:
    return (ROOT / "src" / "dolphin" / f"{unit}.c").is_file()


def compile_unit(sdk: Path, unit: str, out: Path) -> str:
    flags = [FLAG_OVERRIDES.get(unit, {}).get(f, f) for f in BASE_FLAGS]
    signed = unit.startswith("dvd/") or unit in SIGNED_CHAR
    flags += ["-char", "signed" if signed else "unsigned", "-I-",
              "-i", str(sdk / "include"), "-i", str(sdk / "include" / "libc")]
    flags += ["-i", str(sdk / "src")]
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


def code_anchors(elf: vf.Elf, funcs: list[vf.Symbol], symbols: vf.SymbolTable,
                 dol: vf.Dol | None) -> dict[str, str]:
    """Map compiled name -> retail name when exactly one retail function of equal size
    has identical words outside relocated fields. Finds units whose retail functions
    are still unnamed (fn_XXXXXXXX)."""
    if dol is None:
        return {}
    by_size: dict[int, list[tuple[str, bytes]]] = {}
    for name, e in symbols.entries.items():
        if e.is_function and e.section in vf.CODE_SECTIONS and e.size and name not in symbols.duplicates:
            raw = dol.read(e.addr, e.size)
            if raw is not None:
                by_size.setdefault(e.size, []).append((name, raw))
    found: dict[str, str] = {}
    for f in funcs:
        cands = by_size.get(f.size)
        if not cands:
            continue
        try:
            insns = vf.extract_function(elf, f.name, f).insns
        except vf.ToolError:
            continue
        want = [(i.mask, i.cmp) for i in insns]
        hits = [name for name, raw in cands
                if all(int.from_bytes(raw[n * 4:n * 4 + 4], "big") & ~m & 0xFFFFFFFF == c
                       for n, (m, c) in enumerate(want))]
        if len(hits) == 1:
            found[f.name] = hits[0]
    return found


def prune_outliers(out: dict[str, tuple[str, str]], retail: list, index: dict[str, int],
                   total: int) -> None:
    """Keep only the densest group of anchors that fits in the unit's compiled size.

    A tiny function can match an unrelated retail function by chance, and a name can be
    reused elsewhere; the unit's real footprint is at most its compiled text size.
    """
    weight = {"name": 4, "code": 2}
    items = sorted((retail[index[r]][0], retail[index[r]][2], n, weight[h]) for n, (r, h) in out.items()
                   if h in weight)
    best, lo, run = (0, 0, 0), 0, 0
    for hi, (_, _, _, w) in enumerate(items):
        run += w
        while lo < hi and items[hi][0] + items[hi][1] - items[lo][0] > total:
            run -= items[lo][3]
            lo += 1
        if run > best[0]:
            best = (run, lo, hi)
    keep = {n for _, _, n, _ in items[best[1]:best[2] + 1]}
    for n in [n for n, (_, h) in out.items() if h in weight and n not in keep]:
        del out[n]


TINY = 16


def same_code(elf: vf.Elf, f: vf.Symbol, retail_name: str, symbols: vf.SymbolTable,
              dol: vf.Dol | None) -> bool:
    """True when f's words equal the retail function's outside relocated fields."""
    e = symbols.entries.get(retail_name)
    raw = dol.read(e.addr, e.size) if dol is not None and e is not None else None
    if raw is None or len(raw) != f.size:
        return False
    try:
        insns = vf.extract_function(elf, f.name, f).insns
    except vf.ToolError:
        return False
    return all(int.from_bytes(raw[n * 4:n * 4 + 4], "big") & ~i.mask & 0xFFFFFFFF == i.cmp
               for n, i in enumerate(insns))


def anchor(funcs: list[vf.Symbol], symbols: vf.SymbolTable,
           known: dict[str, str] | None = None,
           compatible: Callable[[vf.Symbol, str], bool] | None = None) -> dict[str, tuple[str, str]]:
    """Map compiled function name -> (retail symbol name, how).

    Walks outward from each name or code anchor through the retail function
    list. A compiled function takes the next retail function when sizes agree;
    otherwise it is assumed dead-stripped by the linker and skipped. A function of at
    most TINY bytes also needs `compatible` code, because a getter and a setter of
    one variable have the same size and both fit any slot of that size.
    """
    retail = sorted((e.addr, n, e.size) for n, e in symbols.entries.items()
                    if e.is_function and e.section in vf.CODE_SECTIONS and n not in symbols.duplicates)
    index = {n: i for i, (_, n, _) in enumerate(retail)}
    funcs = sorted(funcs, key=lambda f: f.value)
    out: dict[str, tuple[str, str]] = {}
    known = {n: r for n, r in (known or {}).items() if r in index}
    claimed = set(known.values())
    for f in funcs:
        by_code = known.get(f.name)
        if by_code is not None and f.name in index and by_code == f.name:
            out[f.name] = (f.name, "name")
        elif by_code is not None and (f.name not in index or f.size > TINY):
            # Identical code beats a name that sits on a different function: older heuristic
            # names are often shifted by one slot, so the donor's name points at its neighbour.
            out[f.name] = (by_code, "code")
        elif f.name in index and f.name not in claimed:
            out[f.name] = (f.name, "name")
    prune_outliers(out, retail, index, sum(f.size for f in funcs))
    owners = {r: n for n, (r, _) in out.items()}
    for i, f in enumerate(funcs):
        if out.get(f.name, ("", ""))[1] not in ("name", "code"):
            continue
        for step in (1, -1):
            r = index[out[f.name][0]] + step
            j = i + step
            while 0 <= j < len(funcs) and 0 <= r < len(retail):
                g = funcs[j]
                if g.name in out:
                    break
                if retail[r][1] in owners:
                    break
                if retail[r][2] == g.size and (g.size > TINY or compatible is None
                                               or compatible(g, retail[r][1])):
                    out[g.name] = (retail[r][1], f"layout from {f.name}")
                    owners[retail[r][1]] = g.name
                    r += step
                j += step
    return out


def anchor_unit(elf: vf.Elf, funcs: list[vf.Symbol], symbols: vf.SymbolTable,
                dol: vf.Dol | None) -> dict[str, tuple[str, str]]:
    return anchor(funcs, symbols, code_anchors(elf, funcs, symbols, dol),
                  lambda f, retail: same_code(elf, f, retail, symbols, dol))


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


def probe(sdk: Path, unit: str, tmp: Path, symbols: vf.SymbolTable,
          strip_imported: bool = False) -> list[dict]:
    obj = tmp / (unit.replace("/", "__") + ".o")
    err = compile_unit(sdk, unit, obj)
    if err:
        return [{"unit": unit, "function": "-", "retail": "-", "class": "ERROR", "detail": err}]
    if strip_imported:
        target = BUILD / "GL5E4F" / "obj" / "dolphin" / f"{unit}.o"
        if not target.is_file() or not (ROOT / "src" / "dolphin" / f"{unit}.c").is_file():
            return [{"unit": unit, "function": "-", "retail": "-", "class": "ERROR",
                     "detail": "--strip-imported requires an imported unit; run bash build.sh first"}]
        result = subprocess.run([sys.executable, str(ROOT / "tools" / "strip_unused.py"),
                                 "--target", str(target), str(obj)], capture_output=True, text=True)
        if result.returncode:
            return [{"unit": unit, "function": "-", "retail": "-", "class": "ERROR",
                     "detail": result.stderr.strip()[-800:]}]
    elf = vf.Elf(obj)
    funcs = [f for f in elf.function_symbols() if f.size]
    dol = vf.load_dol(vf.DOL_PATH, [])
    anchors = anchor_unit(elf, funcs, symbols, dol)
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


def print_queue(rows: list[dict]) -> None:
    """Rank pending units by the share of their code already anchored and identical to retail."""
    units: dict[str, dict[str, int]] = {}
    for r in rows:
        u = units.setdefault(r["unit"], {"fns": 0, "bytes": 0, "same": 0, "same_bytes": 0, "bad": 0})
        u["fns"] += 1
        u["bytes"] += r.get("size", 0)
        if r["class"] in ("MATCH", "UNVERIFIED", "CODE_SAME"):
            u["same"] += 1
            u["same_bytes"] += r.get("size", 0)
        elif r["class"] in ("MISMATCH", "ERROR"):
            u["bad"] += 1
    print("\nimport queue (pending units, most identical bytes first; NO_ANCHOR is usually dead-stripped or unnamed)")
    print(f"{'unit':28} {'same':>9} {'bad':>4} {'bytes':>7}")
    for name, u in sorted(units.items(), key=lambda kv: (kv[1]["bad"] > 0, -kv[1]["same_bytes"])):
        if u["same"]:
            print(f"{name:28} {u['same']:>4}/{u['fns']:<4} {u['bad']:>4} {u['same_bytes']:>7}")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("units", nargs="*", help="SDK source units relative to src/, e.g. os/OSAlarm")
    ap.add_argument("--sdk", type=Path, default=None,
                    help="dolsdk2004 checkout (default: $DOLSDK2004 or the shared cache)")
    ap.add_argument("--strip-imported", action="store_true",
                    help="strip imported units against their built DTK targets before verifying retained functions")
    ap.add_argument("--all", action="store_true", help="probe every .c under the SDK src/ tree")
    ap.add_argument("--pending", action="store_true",
                    help="probe only units not yet imported and rank them as an import queue")
    ap.add_argument("--tsv", type=Path, help="write per-function rows here")
    args = ap.parse_args(argv)
    for tool in (MWCC, WIBO, DTK):
        if not tool.exists():
            print(f"error: missing {tool}; run bash build.sh first", file=sys.stderr)
            return 2
    if args.sdk is None:
        args.sdk = default_sdk()
    if not (args.sdk / "src").is_dir():
        print(f"error: no dolsdk2004 checkout at {args.sdk}; run tools/fetch_dolsdk.sh", file=sys.stderr)
        return 2
    if not any((BUILD / "GL5E4F" / "obj").glob("**/*.o")):
        print("error: no target objects under build/GL5E4F/obj; run `bash build.sh` first", file=sys.stderr)
        return 2
    units = args.units
    if args.all or args.pending:
        units = sorted(str(p.relative_to(args.sdk / "src").with_suffix(""))
                       for p in (args.sdk / "src").glob("*/*.c"))
        if args.pending:
            units = [u for u in units if not is_imported(u)]
        elif args.strip_imported:
            units = [u for u in units if is_imported(u)]
    if not units:
        ap.error("give units, --all or --pending")
    symbols = vf.SymbolTable()
    rows: list[dict] = []
    with tempfile.TemporaryDirectory(prefix="sdk_probe_") as d:
        for unit in units:
            unit_rows = probe(args.sdk.resolve(), unit, Path(d), symbols, args.strip_imported)
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
    if args.pending:
        print_queue(rows)
    if args.tsv:
        args.tsv.parent.mkdir(parents=True, exist_ok=True)
        cols = ["unit", "function", "retail", "class", "size", "detail"]
        lines = ["\t".join(cols)] + ["\t".join(str(r.get(c, "")).replace("\t", " ").replace("\n", " ")
                                               for c in cols) for r in rows]
        args.tsv.write_text("\n".join(lines) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
