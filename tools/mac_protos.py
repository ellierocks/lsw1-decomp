#!/usr/bin/env python3
"""C prototypes recovered from the Mac LSW1 demo's mangled symbols, for the sweep.

The Mac demo (`orig/mac/mac-debug-symbols/LEGO Star Wars Demo`, CodeWarrior, PPC Mach-O)
keeps the C++ mangled name of every function, and a mangled name carries the parameter
types. Codegen differs from ProDG but the source is the same, so the parameter *kinds*
(integer, float, double, pointer) and their order transfer. The return type is not
mangled: callers supply it (the sweep borrows m2c's).

    python3 tools/mac_protos.py --build          # rewrite docs/symbol_donors/mac_lsw1_demo_prototypes.tsv
    python3 tools/mac_protos.py NuVecDist fn_8000CCA4

`build` needs the Mac binary and `llvm-nm`/`llvm-cxxfilt`; everything else reads the
committed TSV only. Each row is `name  params  tier  mac_raw`:

* tier `name`: a Mac function whose C name is unique in the demo (overloads are dropped);
  it applies to the GC function of the same name.
* tier `MEDIUM`: the matcher's (`mac_code_match.py`) pairing for a GC function that is still
  called `fn_XXXXXXXX`. It is a guess, which is safe only because the sweep keeps a
  prototype only when it scores strictly better. (HIGH pairings are renamed, so the
  `name` tier covers them.)

Parameters are `s32 u32 s16 u16 s8 u8 f32 f64` or `void *`. A signature with a by-value
struct, 64-bit integer, function pointer or `...` is dropped.
"""
from __future__ import annotations

import argparse
import csv
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MAC_BIN = ROOT / "orig/mac/mac-debug-symbols/LEGO Star Wars Demo"
MATCHES = ROOT / "docs/symbol_donors/mac_code_match.tsv"
TSV = ROOT / "docs/symbol_donors/mac_lsw1_demo_prototypes.tsv"

SCALARS = {
    "int": "s32", "unsigned int": "u32", "unsigned": "u32", "long": "s32", "unsigned long": "u32",
    "short": "s16", "unsigned short": "u16", "char": "s8", "signed char": "s8",
    "unsigned char": "u8", "bool": "u8", "float": "f32", "double": "f64",
}
INT_KINDS = {"s32", "u32", "s16", "u16", "s8", "u8", "void *"}


def split_params(text: str) -> list[str] | None:
    """Top-level comma split of a demangled parameter list; None if nesting is too deep."""
    out, depth, cur = [], 0, ""
    for c in text:
        if c in "(<":
            depth += 1
        elif c in ")>":
            depth -= 1
        if c == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += c
    if cur.strip():
        out.append(cur.strip())
    return out if depth == 0 else None


def c_type(param: str) -> str | None:
    """The sweep's type for one demangled parameter, None when it cannot be expressed."""
    if "(" in param or "[" in param:
        return None
    if "*" in param or "&" in param:
        return "void *"
    words = " ".join(w for w in param.replace("const", " ").replace("volatile", " ").split())
    return SCALARS.get(words)


def parse(demangled: str) -> tuple[str, list[str]] | None:
    """(C name, parameter types) of a free function's demangled name."""
    m = re.fullmatch(r"([A-Za-z_]\w*)\((.*)\)", demangled)
    if not m:  # members, namespaces, templates, operators
        return None
    params = split_params(m[2])
    if params is None:
        return None
    if params == ["void"]:
        params = []
    types = [c_type(p) for p in params]
    return None if None in types else (m[1], types)


def mac_symbols() -> list[tuple[str, str]]:
    """(raw symbol, demangled) for every text symbol of the Mac demo."""
    nm = subprocess.run(["llvm-nm", str(MAC_BIN)], capture_output=True, text=True, errors="replace").stdout
    raws = sorted({p[2] for p in (ln.split(None, 2) for ln in nm.splitlines())
                   if len(p) == 3 and p[1] in ("t", "T") and p[2].startswith("__Z")})
    dem = subprocess.run(["llvm-cxxfilt"], input="\n".join(r[1:] for r in raws) + "\n",
                         capture_output=True, text=True).stdout.splitlines()
    if len(dem) != len(raws):
        raise SystemExit("llvm-cxxfilt output does not line up with its input")
    return list(zip(raws, dem))


def build() -> list[dict]:
    by_name: dict[str, list[tuple[str, list[str]]]] = {}
    by_raw: dict[str, tuple[str, list[str]]] = {}
    for raw, dem in mac_symbols():
        p = parse(dem)
        if p:
            by_name.setdefault(p[0], []).append((raw, p[1]))
            by_raw[raw] = p
    rows = []
    for name, ps in sorted(by_name.items()):
        if len({tuple(t) for _, t in ps}) == 1:  # overloads with differing types are ambiguous
            rows.append({"name": name, "params": ",".join(ps[0][1]), "tier": "name", "mac_raw": ps[0][0]})
    if MATCHES.exists():
        with MATCHES.open() as f:
            for r in csv.DictReader(f, delimiter="\t"):
                if r["confidence"] == "MEDIUM" and r["mac_raw"] in by_raw:
                    rows.append({"name": r["gc_old"], "params": ",".join(by_raw[r["mac_raw"]][1]),
                                 "tier": r["confidence"], "mac_raw": r["mac_raw"]})
    return rows


_TABLE: dict[str, list[str]] | None = None


def table() -> dict[str, list[str]]:
    """name -> parameter types from the committed TSV."""
    global _TABLE
    if _TABLE is None:
        _TABLE = {}
        if TSV.exists():
            for ln in TSV.read_text().splitlines()[1:]:
                name, params, tier, _ = ln.split("\t")
                if tier == "name" or name not in _TABLE:
                    _TABLE[name] = [p for p in params.split(",") if p]
    return _TABLE


def proto(fn: str, ret: str) -> tuple[str, list[str]] | None:
    """(return type, parameter types) for callee FN, or None. RET is the return type m2c
    declared: scalars and pointers pass through, anything unknown (`?`, structs) bails."""
    params = table().get(fn)
    if params is None:
        return None
    ret = ret.replace("const ", "").strip()
    if ret.endswith("*"):
        return "void *", params
    ret = SCALARS.get(ret, ret)
    if ret in ("s32", "u32", "s16", "u16", "s8", "u8", "f32", "f64", "void"):
        return ret, params
    return None


def pad_params(body: str, name: str) -> str:
    """BODY with NAME's parameter list extended to the Mac signature's length.

    m2c declares only the arguments a function reads, so trailing unused parameters are
    missing and the register setup differs. Parameters m2c did declare are kept (they
    carry its struct types); a Mac parameter of another kind at the same slot, or any Mac
    parameter m2c named out of order, leaves BODY unchanged.
    """
    mac = table().get(name)
    m = re.search(rf"^(\w[^\n;(]*?\b{re.escape(name)})\(([^)]*)\)(\s*\{{)", body, flags=re.M)
    if mac is None or not m:
        return body
    gpr = [i for i, t in enumerate(mac) if t in INT_KINDS]
    fpr = [i for i, t in enumerate(mac) if t not in INT_KINDS]
    if len(gpr) > 8 or len(fpr) > 8:
        return body
    have: dict[tuple[str, int], str] = {}
    for param in (p.strip() for p in m[2].split(",") if p.strip() not in ("", "void")):
        pm = re.search(r"\b(f?)arg(\d+)$", param)
        if not pm:
            return body
        have[("f" if pm[1] else "g", int(pm[2]))] = param
    out = []
    for kind, slots in (("g", gpr), ("f", fpr)):
        if any(n >= len(slots) for k, n in have if k == kind):
            return body  # m2c read more arguments than the Mac function takes
        for n, idx in enumerate(slots):
            if (kind, n) in have:
                out.append((idx, have[(kind, n)]))
            else:
                out.append((idx, f"{mac[idx]} {'fpad' if kind == 'f' else 'pad'}{n}"))
    new = ", ".join(p for _, p in sorted(out))
    return body if new == m[2].strip() else body[:m.start(2)] + new + body[m.end(2):]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--build", action="store_true", help="regenerate the TSV from the Mac binary")
    ap.add_argument("names", nargs="*", help="print the stored parameter types")
    args = ap.parse_args()
    if args.build:
        if not MAC_BIN.exists():
            print(f"{MAC_BIN} not found", file=sys.stderr)
            return 1
        rows = build()
        TSV.write_text("name\tparams\ttier\tmac_raw\n" + "".join(
            f"{r['name']}\t{r['params']}\t{r['tier']}\t{r['mac_raw']}\n" for r in rows))
        tiers = {t: sum(r["tier"] == t for r in rows) for t in ("name", "MEDIUM")}
        print(f"{len(rows)} rows -> {TSV.relative_to(ROOT)} {tiers}")
    for n in args.names:
        print(n, table().get(n))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
