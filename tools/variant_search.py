#!/usr/bin/env python3
"""Try the cheap, known source rewrites for one function and score each one.

Most ProDG plateaus in Phase 3 were solved by one of a few mechanical changes. Instead of
trying them by hand, point this tool at the unit source (no markers needed) and it varies
only the named function. `--body FILE` searches a draft that is not in the unit yet
(spliced in as `score_fn.py --body` does):

  - declaration order of the leading locals, initialized ones included (stack slot layout,
    register order and prologue scheduling; the first MAX_DECLS when there are more),
  - `x = c ? a : b;` against an explicit if/else with the store in each branch,
  - `s32`/`u32`, `s16`/`u16`, `s8`/`u8` for each parameter and local,
  - `volatile` on each address-taken local (its stack slot moves to declaration order).
  - an extra `case K:` on a switch's `default:` arm, K next to or between the other cases
    (it changes GCC's case-tree balancing without changing behavior; fn_80063F80).

    python3 tools/variant_search.py NuAnimCurve2SetApplyToMatrix src/nu3dx/nuanim.c -j 8

Variants are tried fewest-changes first, so the smallest edit that matches wins. Every one is
compiled with `score_fn.py` (strict verifier, same score order as hillclimb.py). The search
stops at the first strict MATCH. At most `--max-changes` dimensions (default 2; a
whole declaration permutation or all ternaries together count as one) change at once. A space
larger than `--limit` (default 2000, about 5 minutes serially, under a minute with -j 8) is
truncated to its first `--limit` variants, which are the fewest-change ones, instead of being
refused. The best variant is written to
`work/variant_search/NAME/best.c` (or `-o`); the checkout is never edited. Accept it only
through `verify_fn.py --build --strict-symbols` after porting it into the unit source.

Use `order_search.py` for statement order and `permute_setup.py` for everything else.
"""

from __future__ import annotations

import argparse
import itertools
import math
import re
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import order_search as osr  # noqa: E402

ROOT = osr.ROOT
DEFAULT_LIMIT = 2000
MAX_DECLS = 6
SIGN_PAIRS = {"s32": "u32", "u32": "s32", "s16": "u16", "u16": "s16", "s8": "u8", "u8": "s8"}
DECL_RE = re.compile(r"^(?P<indent>[ \t]+)(?P<type>(?:struct \w+|[A-Za-z_]\w*)\s*\**)\s*(?P<name>\w+)"
                     r"(?P<array>\[\w*\])?(?:\s*=\s*[^;]+)?;[ \t]*$")
TERNARY_RE = re.compile(r"^(?P<indent>[ \t]+)(?P<lhs>[^=?;{}()]+?)\s=\s(?P<cond>[^?;{}]+?)\s\?\s"
                        r"(?P<a>[^?:;{}]+?)\s:\s(?P<b>[^?:;{}]+?);[ \t]*$")
TYPE_RE = re.compile(r"\b(s32|u32|s16|u16|s8|u8)\b")


class VariantError(Exception):
    pass


@dataclass
class Function:
    head: str           # text up to and including `{`
    body: str           # text between the braces
    tail: str           # closing `}` and everything after
    before: str         # file text before the function
    signature: str      # the parameter list text


@dataclass
class Dimension:
    name: str
    kind: str           # "order", "lines" or "param"
    slot: int           # first line of a permuted block, or character offset in the signature
    options: list       # option 0 is the original; "lines" options map line index -> new text


def find_function(text: str, name: str) -> Function:
    # A definition starts at column 0 and has no `(` or `=` before the name: that keeps a call
    # such as `    if (f(x) != 0) {` in an earlier function from being taken for f's definition.
    pat = re.compile(rf"^(?=[A-Za-z_])[^\n;{{}}()=]*\b{re.escape(name)}\s*\((?P<params>[^;{{}}]*)\)\s*\{{", re.M)
    m = pat.search(text)
    if not m:
        raise VariantError(f"no definition of {name} found")
    depth, i = 1, m.end()
    while i < len(text) and depth:
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
        i += 1
    if depth:
        raise VariantError(f"unbalanced braces in {name}")
    return Function(text[m.start():m.end()], text[m.end():i - 1], text[i - 1:], text[:m.start()],
                    m.group("params"))


def leading_decls(lines: list[str]) -> tuple[int, int]:
    """Half-open line range of the plain declarations at the top of the body."""
    start = 0
    while start < len(lines) and not lines[start].strip():
        start += 1
    end = start
    while end < len(lines) and DECL_RE.match(lines[end]):
        end += 1
    return start, end


def build_dimensions(fn: Function) -> tuple[list[Dimension], list[str]]:
    """Return the dimensions and the body lines they refer to."""
    lines = fn.body.split("\n")
    dims: list[Dimension] = []
    s, e = leading_decls(lines)
    # Initialized locals count too: their order steers prologue scheduling. Past MAX_DECLS,
    # only the leading ones are permuted (the factorial grows too fast for more).
    n = min(e - s, MAX_DECLS)
    if n >= 2:
        dims.append(Dimension(f"decl-order[{s}:{s + n}]", "order", s,
                              list(itertools.permutations(range(n)))))
    ternaries = {}
    for i, line in enumerate(lines):
        m = TERNARY_RE.match(line)
        if m and line.count("?") == 1:
            ind = m.group("indent")
            ternaries[i] = (f"{ind}if ({m.group('cond')}) {{\n{ind}    {m.group('lhs')} = {m.group('a')};\n"
                            f"{ind}}} else {{\n{ind}    {m.group('lhs')} = {m.group('b')};\n{ind}}}")
    if len(ternaries) <= 3:
        for i, expanded in ternaries.items():
            dims.append(Dimension(f"ternary@{i}", "lines", i, [{}, {i: expanded}]))
    if len(ternaries) >= 2:
        dims.append(Dimension("ternary-all", "lines", 0, [{}, dict(ternaries)]))
    dims += default_case_dims(lines)
    for i, line in enumerate(lines[s:e], s):
        if TYPE_RE.search(line) and DECL_RE.match(line):
            dims.append(Dimension(f"sign-local@{i}", "lines", i,
                                  [{}, {i: TYPE_RE.sub(lambda t: SIGN_PAIRS[t.group(1)], line, 1)}]))
    # A local whose address is taken gets its stack slot when the address is first expanded
    # (call arguments right to left); `volatile` gives it a slot at its declaration instead,
    # which swaps two address-taken locals' offsets (fn_8009D57C).
    for i, line in enumerate(lines[s:e], s):
        m = DECL_RE.match(line)
        if m and "volatile" not in line and not m.group("array") and re.search(rf"&\s*{m.group('name')}\b", fn.body):
            ind = m.group("indent")
            dims.append(Dimension(f"volatile-local@{i}", "lines", i,
                                  [{}, {i: f"{ind}volatile {line[len(ind):]}"}]))
    return dims, lines


CASE_RE = re.compile(r"^\s*case (-?(?:0x[0-9A-Fa-f]+|\d+))U?:")
DEFAULT_RE = re.compile(r"^(?P<indent>[ \t]*)default:")
MAX_CASE_OPTIONS = 8


def default_case_values(cases: list[int]) -> list[int]:
    """Labels worth adding to a `default:` arm: the gaps between the other cases and the
    values just outside them (0 and 1 first), at most MAX_CASE_OPTIONS."""
    lo, hi = min(cases), max(cases)
    near = [0, 1, -1, lo - 1, lo - 2, hi + 1, hi + 2] + [v for v in range(lo + 1, hi) if hi - lo <= 64]
    out = []
    for v in near:
        if v not in cases and v not in out:
            out.append(v)
    return out[:MAX_CASE_OPTIONS]


def default_case_dims(lines: list[str]) -> list[Dimension]:
    """One dimension per `default:` arm (of a switch whose case labels precede it): option k
    puts `case K:` in front of it."""
    dims, cases = [], []
    for i, line in enumerate(lines):
        if "switch (" in line:
            cases = []
        c = CASE_RE.match(line)
        if c:
            cases.append(int(c[1], 0))
        d = DEFAULT_RE.match(line)
        if d and cases:
            opts = [{}] + [{i: f"{d['indent']}case {v}:\n{line}"} for v in default_case_values(cases)]
            dims.append(Dimension(f"default-case@{i}", "lines", i, opts))
    return dims


def sign_param_dims(fn: Function) -> list[Dimension]:
    """One dimension per signed/unsigned integer parameter; options rewrite the signature."""
    dims = []
    for m in TYPE_RE.finditer(fn.signature):
        dims.append(Dimension(f"sign-param@{m.start()}", "param", m.start(), [m.group(1), SIGN_PAIRS[m.group(1)]]))
    return dims


def combos(dims: list[Dimension], max_changes: int):
    """Yield option-index tuples, fewest changed dimensions first, at most max_changes at once."""
    n = len(dims)
    yield (0,) * n
    for size in range(1, min(max_changes, n) + 1):
        for chosen in itertools.combinations(range(n), size):
            for ks in itertools.product(*[range(1, len(dims[i].options)) for i in chosen]):
                c = [0] * n
                for i, k in zip(chosen, ks):
                    c[i] = k
                yield tuple(c)


def count(dims: list[Dimension], max_changes: int) -> int:
    n = len(dims)
    total = 1
    for size in range(1, min(max_changes, n) + 1):
        for chosen in itertools.combinations(range(n), size):
            total += math.prod(len(dims[i].options) - 1 for i in chosen)
    return total


def render(fn: Function, dims: list[Dimension], body_lines: list[str], choice: tuple[int, ...]) -> str:
    lines = list(body_lines)
    signature = fn.signature
    order = None
    for dim, k in zip(dims, choice):
        if not k:
            continue
        if dim.kind == "lines":
            for idx, text in dim.options[k].items():
                lines[idx] = text
        elif dim.kind == "param":  # token lengths are equal in both signs, so offsets stay valid
            signature = signature[:dim.slot] + dim.options[k] + signature[dim.slot + len(dim.options[0]):]
        else:
            order = (dim.slot, dim.options[k])
    if order:
        start, perm = order
        block = lines[start:start + len(perm)]
        lines[start:start + len(perm)] = [block[i] for i in perm]
    head = fn.head.replace(fn.signature, signature, 1)
    return fn.before + head + "\n".join(lines) + fn.tail


def search(name: str, text: str, scorer, *, jobs: int, limit: int, max_changes: int = 2, log=None):
    fn = find_function(text, name)
    dims, body_lines = build_dimensions(fn)
    dims += sign_param_dims(fn)
    if not dims:
        raise VariantError(f"{name} has no cheap variants (no leading locals, ternary stores or integer types)")
    total = count(dims, max_changes)
    truncated = total > limit
    if truncated and log:
        log(f"{total} variants exceed --limit {limit}; trying the first {limit} (fewest changes first)")
    best, tried, errors = None, 0, 0
    it = itertools.islice(combos(dims, max_changes), limit)
    with ThreadPoolExecutor(max_workers=max(1, jobs)) as pool:
        while True:
            batch = list(itertools.islice(it, max(1, jobs) * 4))
            if not batch:
                break
            texts = [render(fn, dims, body_lines, c) for c in batch]
            reports = list(pool.map(lambda a: scorer(a[1], a[0]), enumerate(texts, tried)))
            for choice, variant, rep in zip(batch, texts, reports):
                tried += 1
                if rep.get("verdict") == "ERROR":
                    errors += 1
                    continue
                if best is None or rep["score"] < best["score"]:
                    changes = [d.name for d, k in zip(dims, choice) if k]
                    best = {"score": rep["score"], "changes": changes, "text": variant, "report": rep}
                    if log:
                        log(f"[{tried}/{total}] {rep['verdict']} score={rep['score']} "
                            f"exact={rep['exact_insns']}/{rep['target_insns']} changes={changes or 'none'}")
            if best and best["report"].get("returncode") == 0 and best["report"]["verdict"] == "MATCH":
                break
    return {"total": total, "tried": tried, "errors": errors, "best": best,
            "truncated": truncated, "dimensions": [d.name for d in dims]}


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="retail function name in symbols.txt (also the C function name)")
    ap.add_argument("source", type=Path, help="C file containing the draft")
    ap.add_argument("-j", "--jobs", type=int, default=1, help="parallel compiles")
    ap.add_argument("--limit", type=int, default=DEFAULT_LIMIT, help="try at most this many variants, fewest changes first (default 2000)")
    ap.add_argument("--max-changes", type=int, default=2,
                    help="change at most this many dimensions at once (default 2)")
    ap.add_argument("-o", "--output", type=Path, help="write the best variant here")
    ap.add_argument("--compiled-symbol", help="passed to score_fn.py")
    ap.add_argument("--compiler", choices=("prodg", "sdk"), help="passed to score_fn.py")
    ap.add_argument("-I", dest="includes", action="append", default=[], help="passed to score_fn.py")
    ap.add_argument("--body", type=Path, metavar="FILE",
                    help="search a draft definition (as for score_fn.py --body) spliced into SOURCE, "
                         "without editing SOURCE")
    ap.add_argument("--dry-run", action="store_true", help="list the dimensions and variant count, then exit")
    args = ap.parse_args(argv)
    try:
        text = args.source.read_text()
        if args.body:
            from score_fn import splice_body
            text = splice_body(text, args.name, args.body.read_text())
        fn = find_function(text, args.name)
        dims, _ = build_dimensions(fn)
        dims += sign_param_dims(fn)
    except (OSError, VariantError) as e:
        print(f"error: {e}", file=sys.stderr)
        return 2
    total = count(dims, args.max_changes)
    print(f"{len(dims)} dimension(s), {total} variants: {', '.join(d.name for d in dims) or 'none'}")
    if args.dry_run:
        return 0
    extra = ["-I", str(args.source.resolve().parent)]
    for inc in args.includes:
        extra += ["-I", inc]
    if args.compiled_symbol:
        extra += ["--compiled-symbol", args.compiled_symbol]
    if args.compiler:
        extra += ["--compiler", args.compiler]
    with tempfile.TemporaryDirectory(prefix="variant_search_") as tmp:
        def scorer(variant, index):
            return osr.score_source(args.name, variant, extra, Path(tmp), index)
        try:
            result = search(args.name, text, scorer, jobs=args.jobs, limit=args.limit,
                            max_changes=args.max_changes, log=print)
        except VariantError as e:
            print(f"error: {e}", file=sys.stderr)
            return 2
    best = result["best"]
    print(f"tried {result['tried']}/{result['total']} variants, {result['errors']} compile errors")
    if best is None:
        print("error: no variant compiled", file=sys.stderr)
        return 2
    out = args.output or ROOT / "work" / "variant_search" / args.name / "best.c"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(best["text"])
    rep = best["report"]
    print(f"best: {rep['verdict']} score={best['score']} exact={rep['exact_insns']}/{rep['target_insns']} "
          f"changes={best['changes'] or 'none'}")
    print(f"wrote {out}")
    if rep["verdict"] == "MATCH" and rep["returncode"] == 0:
        print("Port it into the unit source, then accept with verify_fn.py --build --strict-symbols.")
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main())
