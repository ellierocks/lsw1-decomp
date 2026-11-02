#!/usr/bin/env python3
"""Unattended m2c sweep: draft, compile and strict-score every ready target in parallel.

    python3 tools/sweep.py                          # all ready targets, report only
    python3 tools/sweep.py --max-size 0x400 -j 16   # bound the sweep
    python3 tools/sweep.py --unit auto/game/ --apply

For each `next_target.py` target this runs m2c in `--valid-syntax` mode on the retail
assembly, rewrites m2c's macros into plain C, and scores the draft with `score_fn.py
--body` against the function's unit source (appended in a temporary copy, so SOURCE is
untouched). A draft that redeclares a function or global with conflicting types is
retried without m2c's declaration. No model or agent is involved.

m2c also reads the unit source, preprocessed, as `--context`: its structs, globals and
prototypes (including the unit's headers) type the draft's field accesses and calls. Each
target is drafted with and without that context and the better-scoring draft is kept, so
typing a header can only raise the sweep's yield.

`--apply` writes strict MATCHes into their unit sources one at a time, re-scoring each
against the updated file with `score_fn.py --apply`; run `bash build.sh` and
`sprint_finish.py` afterwards for the build-level strict verification. Everything else is
ranked in `work/sweep/results.json` (and printed) by unmatched bytes closest to a match:
that ranking is the agent queue, biggest progress first. Task packs go to
`work/sweep/NAME/`, which does not claim the target for `next_target.py`.
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import tempfile
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import draft_fn  # noqa: E402

OUT = ROOT / "work" / "sweep"
UNKNOWN_TYPES = {"M2C_UNK": "s32", "M2C_UNK8": "s8", "M2C_UNK16": "s16", "M2C_UNK32": "s32",
                 "M2C_UNK64": "s64"}
# Macros that only stand for an instruction m2c could not express; such a draft cannot match.
UNHANDLED = re.compile(r"\bM2C_(?:ERROR|TRAP_IF|BREAK|SYNC|DCACHE|ICACHE|PREFETCH|CARRY|OVERFLOW|"
                       r"LWL|FIRST3BYTES|UNALIGNED32|LOAD_|STORE_)|\b(?:GLUE_F64|MULT_HI|MULTU_HI|"
                       r"CLZ|ROTATE_RIGHT|BSWAP\d+)\(")


def split_args(text: str, start: int) -> tuple[list[str], int]:
    """Comma-separated arguments of the call whose `(` is at text[start]; returns (args, end)."""
    depth, args, cur = 0, [], start + 1
    for i in range(start, len(text)):
        c = text[i]
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
            if depth == 0:
                args.append(text[cur:i])
                return [a.strip() for a in args], i + 1
        elif c == "," and depth == 1:
            args.append(text[cur:i])
            cur = i + 1
    raise ValueError("unbalanced macro call")


def expand_macros(text: str) -> str:
    """Rewrite m2c valid-syntax macros as plain C casts so a matched draft needs no header."""
    for name, ctype in UNKNOWN_TYPES.items():
        text = re.sub(rf"\b{name}\b", ctype, text)
    # With a typed context m2c reads the first member of a struct local as
    # M2C_BITWISE(f32, sp8); a cast cannot convert a struct, so reinterpret its storage.
    struct_locals = set(re.findall(r"^\s+struct \w+ (\w+);", text, flags=re.M))
    while (m := re.search(r"\bM2C_BITWISE\s*\(", text)):
        args, end = split_args(text, m.end() - 1)
        fmt = "(*({0} *)&({1}))" if args[1] in struct_locals else "(({0})({1}))"
        text = text[:m.start()] + fmt.format(*args) + text[end:]
    for macro, fmt in (("M2C_FIELD", "(*({1})((s8 *)({0}) + ({2})))"),
                       ("M2C_STRUCT_COPY", "memcpy({0}, {1}, {2})"),
                       ("M2C_MEMCPY_ALIGNED", "memcpy({0}, {1}, {2})")):
        while (m := re.search(rf"\b{macro}\s*\(", text)):
            args, end = split_args(text, m.end() - 1)
            text = text[:m.start()] + fmt.format(*args) + text[end:]
    return text


# ProDG emits float `<=`/`>=` as `fcmpu; cror un, eq, lt|gt; bso|bns`. m2c cannot read the
# cror, but folding it into the branch is exact, including the unordered case:
# un|eq|lt set <=> !gt, so bso -> ble and bns -> bgt (and with gt: bso -> bge, bns -> blt).
CROR = re.compile(r"^(.*?)\bcror un, eq, (lt|gt)\s*$")
FOLD = {("lt", "bso"): "ble", ("lt", "bns"): "bgt", ("gt", "bso"): "bge", ("gt", "bns"): "blt"}


def fold_cror(asm: str) -> str:
    """Rewrite cror-based float comparisons into branches m2c understands."""
    lines = asm.splitlines()
    for i, line in enumerate(lines):
        if not (m := CROR.match(line)):
            continue
        nxt = lines[i + 1] if i + 1 < len(lines) else ""
        b = re.search(r"\t(bso|bns)(lr)?\b", nxt)
        if not b:
            return asm  # not the compiler's pattern: leave the cror for m2c to report
        lines[i + 1] = nxt[:b.start() + 1] + FOLD[(m.group(2), b.group(1))] + (b.group(2) or "") + nxt[b.end():]
        lines[i] = None
    return "\n".join(ln for ln in lines if ln is not None) + "\n"


# m2c writes `*(base + off)` when it cannot type the pointer; ProDG rejects a deref of an
# integer or void *. Most such accesses are words, so give them an s32 pointer. Binary `*`
# is always spaced in m2c output, and an existing `*(T *)` cast is left alone.
UNTYPED_DEREF = re.compile(r"\*\((?![\w ]+\*+\))")


def type_derefs(text: str) -> str:
    return UNTYPED_DEREF.sub("*(s32 *)(", text)


_CONTEXTS: dict[str, Path | None] = {}
_CONTEXT_LOCK = threading.Lock()
CPP = ["cpp", "-P", "-nostdinc", "-Isrc", "-Iinclude", "-Ibuild/GL5E4F/include", "-D__attribute__(x)="]


def unit_context(source: str) -> Path | None:
    """SOURCE preprocessed for m2c's `--context` (cached per sweep), or None if cpp fails."""
    with _CONTEXT_LOCK:
        if source not in _CONTEXTS:
            out = OUT / "ctx" / (source.replace("/", "_") + ".i")
            out.parent.mkdir(parents=True, exist_ok=True)
            try:
                r = subprocess.run(CPP + [source], cwd=ROOT, capture_output=True, text=True)
            except OSError:
                r = None
            if r is not None and r.returncode == 0:
                out.write_text(r.stdout)
                _CONTEXTS[source] = out
            else:
                _CONTEXTS[source] = None
        return _CONTEXTS[source]


def draft(name: str, context: Path | None = None) -> tuple[str, str]:
    """(status, C body) for NAME; status is 'ok', 'no-asm', 'm2c-failed' or 'unhandled'."""
    task = OUT / name
    asm = task / "asm.s"
    if not asm.is_file():
        subprocess.run([sys.executable, str(ROOT / "tools" / "ls1_task_pack.py"), name,
                        "--out-dir", str(OUT)], cwd=ROOT, capture_output=True)
    if not asm.is_file() or not asm.read_text().strip():
        return "no-asm", ""
    m2c = draft_fn.m2c_dir()
    m2c_in = task / "asm_m2c.s"
    m2c_in.write_text(fold_cror(asm.read_text()))
    cmd = [sys.executable, str(m2c / "m2c.py"), "-t", draft_fn.TARGET, "--valid-syntax"]
    if context is not None:
        cmd += ["--no-cache", "--context", str(context)]
    r = subprocess.run(cmd + [str(m2c_in.resolve())], capture_output=True, text=True)
    if r.returncode or not r.stdout.strip():
        return "m2c-failed", ""
    if UNHANDLED.search(r.stdout):
        return "unhandled", ""
    try:
        body = type_derefs(expand_macros(r.stdout))
    except ValueError:
        return "m2c-failed", ""
    body = draft_fn.clean(body, name, asm.read_text())
    body = body.replace('#include "types.h"\n', "", 1).strip() + "\n"
    return "ok", body


# m2c phrases a conditional store as `v = A; if (c) { v = B; } g = v;`. ProDG then hoists
# `v = A` above the call in c into a callee-saved register; the retail code usually
# comes from storing in both arms, so a second variant stores B or A directly.
PRESET_STORE = re.compile(r"(?P<ind>[ \t]*)(?P<v>var_r\w+) = (?P<a>[^;\n]+);\n(?P=ind)if \((?P<c>.*)\) \{\n"
                          r"[ \t]*(?P=v) = (?P<b>[^;\n]+);\n(?P=ind)\}\n(?P=ind)(?P<g>[\w\[\]]+) = (?P=v);\n")


def store_in_arms(body: str) -> str:
    """BODY with each PRESET_STORE written as an if/else of direct stores."""
    def arms(m):
        i = m["ind"]
        return (f"{i}if ({m['c']}) {{\n{i}    {m['g']} = {m['b']};\n{i}}} else {{\n"
                f"{i}    {m['g']} = {m['a']};\n{i}}}\n")
    out = PRESET_STORE.sub(arms, body)
    for v in {m["v"] for m in PRESET_STORE.finditer(body)}:
        decl = re.compile(rf"^[ \t]+\w+ {v};\n", flags=re.M)
        if len(re.findall(rf"\b{v}\b", out)) == len(decl.findall(out)):  # only declared now
            out = decl.sub("", out, count=1)
    return out


_RODATA: set[str] | None = None


def rodata_symbols() -> set[str]:
    """Names that config/GL5E4F/symbols.txt places in .rodata."""
    global _RODATA
    if _RODATA is None:
        text = (ROOT / "config" / "GL5E4F" / "symbols.txt").read_text()
        _RODATA = set(re.findall(r"^(\w+) = \.rodata:", text, flags=re.M))
    return _RODATA


def const_rodata(body: str) -> str:
    """BODY with its .rodata externs declared const. ProDG keeps a load from a non-const
    float array inside a loop that stores floats, but hoists it from a const one."""
    rodata = rodata_symbols()
    return re.sub(r"^extern (?!const )(\w[\w ]*?)\b(\w+)\[\];",
                  lambda m: f"extern const {m[1]}{m[2]}[];" if m[2] in rodata else m[0], body, flags=re.M)


def both_rewrites(body: str) -> str:
    return const_rodata(store_in_arms(body))


VARIANTS = (store_in_arms, const_rodata, both_rewrites)


CONFLICT = re.compile(r"conflicting types for [`'‘](\w+)|redefinition of [`'‘](\w+)|"
                      r"previous declaration of [`'‘](\w+)")


def drop_decl(body: str, sym: str, name: str) -> str | None:
    """Remove m2c's top-level declaration of SYM (never NAME's own definition)."""
    if sym == name:
        return None
    lines = body.splitlines()
    kept = [ln for ln in lines if not (ln.rstrip().endswith(";") and not ln.startswith((" ", "\t"))
                                        and re.search(rf"\b{re.escape(sym)}\b", ln))]
    return "\n".join(kept) + "\n" if len(kept) != len(lines) else None


def score(name: str, source: str, body: str, apply: bool = False) -> dict:
    with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False) as f:
        f.write(body)
        path = f.name
    try:
        cmd = [sys.executable, str(ROOT / "tools" / "score_fn.py"), name, source, "--body", path]
        cmd += ["--apply"] if apply else ["--json"]
        r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return {"verdict": "TIMEOUT"}
    finally:
        Path(path).unlink(missing_ok=True)
    if apply:
        return {"verdict": "MATCH" if r.returncode == 0 else "NOT-APPLIED", "log": r.stdout[-300:]}
    if r.returncode == 2 and not r.stdout.strip().startswith("{"):
        return {"verdict": "COMPILE-ERROR", "error": r.stderr}
    try:
        rep = json.loads(r.stdout)
    except ValueError:
        return {"verdict": "ERROR", "error": (r.stdout + r.stderr)[-400:]}
    return {"verdict": rep.get("verdict"), "exact": rep.get("exact_insns"), "target": rep.get("target_insns")}


RANK = {"MATCH": 3, "MISMATCH": 2}


def better(a: dict, b: dict) -> bool:
    """Whether score A beats score B (MATCH, then more exact instructions)."""
    return (RANK.get(a["verdict"], 0), a.get("exact") or 0) > (RANK.get(b["verdict"], 0), b.get("exact") or 0)


def sweep_one(t: dict) -> dict:
    name, source = t["name"], t["source"]
    row = {"name": name, "unit": t["unit"], "source": source, "size": t["size"]}
    best = None
    ctx = unit_context(source)
    for context in [None] + ([ctx] if ctx else []):
        status, body = draft(name, context)
        if status != "ok":
            res = {"verdict": status.upper()}
        else:
            res, body = score_draft(name, source, body)
            res["context"] = context is not None
            res, body = try_variants(name, source, res, body)
        if best is None or better(res, best[0]):
            best = (res, body)
        if res["verdict"] == "MATCH":
            break
    res, body = best
    if res["verdict"] in ("MATCH", "MISMATCH"):
        (OUT / name / "sweep_draft.c").write_text(body)
    err = res.pop("error", "").strip()
    if res["verdict"] in ("COMPILE-ERROR", "ERROR") and err:
        res["error"] = err.splitlines()[-1][:200]
    return {**row, **res}


def try_variants(name: str, source: str, res: dict, body: str) -> tuple[dict, str]:
    """Score each VARIANTS rewrite of a draft at least 80% exact; keep the best."""
    if res["verdict"] != "MISMATCH" or (res.get("exact") or 0) < 0.8 * (res.get("target") or 1):
        return res, body
    seen = {body}
    for variant in VARIANTS:
        alt = variant(body)
        if alt in seen:
            continue
        seen.add(alt)
        alt_res, alt = score_draft(name, source, alt)
        if better(alt_res, res):
            res, body = {**alt_res, "context": res.get("context"), "rewrite": variant.__name__}, alt
            if res["verdict"] == "MATCH":
                break
    return res, body


def score_draft(name: str, source: str, body: str) -> tuple[dict, str]:
    """Score BODY, dropping m2c declarations that conflict with SOURCE; (result, final body)."""
    for _ in range(4):
        res = score(name, source, body)
        if res["verdict"] != "COMPILE-ERROR":
            break
        syms = [next(g for g in m.groups() if g) for m in CONFLICT.finditer(res.get("error", ""))]
        for sym in syms:
            if (new := drop_decl(body, sym, name)):
                body = new
                break
        else:
            break
    else:
        res = score(name, source, body)
    return res, body


def targets(args) -> list[dict]:
    cmd = [sys.executable, str(ROOT / "tools" / "next_target.py"), "--json", "--no-remote",
           "--limit", "100000", "--max-size", args.max_size]
    if args.unit:
        cmd += ["--unit", args.unit]
    return json.loads(subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, check=True).stdout)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--unit", help="unit prefix, as for next_target.py")
    ap.add_argument("--max-size", default="0x100000", help="largest function to sweep")
    ap.add_argument("-j", type=int, default=16, help="parallel jobs")
    ap.add_argument("--apply", action="store_true", help="write strict MATCHes into their unit sources")
    ap.add_argument("--top", type=int, default=30, help="near-misses to print")
    args = ap.parse_args(argv)
    if not (draft_fn.m2c_dir() / "m2c.py").is_file():
        print("error: m2c not fetched; run bash tools/fetch_m2c.sh", file=sys.stderr)
        return 2
    todo = targets(args)
    OUT.mkdir(parents=True, exist_ok=True)
    rows = []
    with ThreadPoolExecutor(args.j) as pool:
        futures = {pool.submit(sweep_one, t): t for t in todo if t.get("source")}
        for i, fut in enumerate(as_completed(futures), 1):
            try:
                rows.append(fut.result())
            except Exception as exc:  # one bad target must not lose the sweep
                t = futures[fut]
                rows.append({"name": t["name"], "unit": t["unit"], "source": t["source"],
                             "size": t["size"], "verdict": "ERROR", "error": str(exc)[:200]})
            if i % 100 == 0:
                print(f"  {i}/{len(todo)}", file=sys.stderr)
    applied = []
    if args.apply:
        for row in sorted((r for r in rows if r["verdict"] == "MATCH"), key=lambda r: (r["source"], r["name"])):
            body = (OUT / row["name"] / "sweep_draft.c").read_text()
            if score(row["name"], row["source"], body, apply=True)["verdict"] == "MATCH":
                applied.append(row["name"])
                row["applied"] = True
    for r in rows:
        r["closeness"] = (r["exact"] / r["target"]) if r.get("target") else 0.0
    rows.sort(key=lambda r: (r["verdict"] != "MATCH", -(r["size"] * r["closeness"])))
    (OUT / "results.json").write_text(json.dumps(rows, indent=1) + "\n")
    counts: dict[str, int] = {}
    for r in rows:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
    match_bytes = sum(r["size"] for r in rows if r["verdict"] == "MATCH")
    print(f"swept {len(rows)} targets: " + ", ".join(f"{k} {v}" for k, v in sorted(counts.items())))
    print(f"strict MATCH: {counts.get('MATCH', 0)} functions, {match_bytes:,} bytes"
          + (f"; applied {len(applied)}" if args.apply else " (rerun with --apply to write them)"))
    near = [r for r in rows if r["verdict"] == "MISMATCH" and r["closeness"] >= 0.9]
    print(f"near misses (>=90% exact): {len(near)} functions, {sum(r['size'] for r in near):,} bytes")
    for r in sorted(near, key=lambda r: -r["size"] * r["closeness"])[:args.top]:
        print(f"  {r['name']:<28} {r['unit']:<28} 0x{r['size']:<5x} {r['exact']}/{r['target']}")
    print(f"results: {(OUT / 'results.json').relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
