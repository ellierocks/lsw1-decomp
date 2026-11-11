#!/usr/bin/env python3
"""Exhaustive statement-order search for one function, scored by score_fn.py.

GCC's scheduler does not follow source order, so independent stores and loads
often need a specific source order (and sometimes a shared constant local) to
reproduce retail. Mark the statements whose order is free in a copy of the
candidate C file, then let this tool try every ordering:

    void fn(struct s* p)
    {
        /* order: decl */
        /* order: begin */
        p->a = 0;
        p->b = -1;
        p->c = -1;
        /* order: end */
    }

    python3 tools/order_search.py fn_800127D8 scratch.c --const-locals -j 8

With `--body try.c`, the markers sit in a single draft definition that is spliced
into the unit source (as `score_fn.py --body` does), so no scratch copy of the
whole unit is needed:

    python3 tools/order_search.py fn_X src/auto/u.c --body try.c -j 4

Each block between `/* order: begin */` and `/* order: end */` is split into
top-level statements (a braced statement counts as one). Several blocks are
permuted independently and searched as a product. With `--const-locals`, an
integer constant that is the whole right-hand side of two or more assignments in
a block may also be replaced by one shared `s32` local; its declaration goes at
the `/* order: decl */` marker, or in a brace around the block when there is no
marker. Every variant is compiled and scored with `score_fn.py` (strict
verifier, same score order as hillclimb.py). The search stops at the first
strict MATCH. It refuses searches larger than `--limit` (default 7! = 5040,
about 13 minutes serially at 0.15 s per compile, or 2 minutes with -j 8).

Larger blocks (a 12-store descriptor fill is 479M orders) use `--climb`: start
from the source order, score every single-statement move (n*(n-1) per block),
take the best strict improvement and repeat; at a plateau, kick from the best
order with a few random moves and climb again. It stops at a strict MATCH or
after `--budget` scored variants (default 2000), and never rescores an order:

    python3 tools/order_search.py fn_800262BC scratch.c --climb -j 8 --budget 1500

The best variant is written to `work/order_search/NAME/best.c` (or `-o`).
A candidate is accepted only through `verify_fn.py --build --strict-symbols`
after porting it into the unit source; this tool never edits the checkout.
"""

from __future__ import annotations

import argparse
import itertools
import json
import math
import random
import re
import shlex
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MARK = re.compile(r"^[ \t]*/\*\s*order:\s*(begin|end|decl)\s*\*/[ \t]*\n?", re.M)
CONST_ASSIGN = re.compile(r"^(?P<lhs>[^=;]*[^=!<>+\-*/%&|^])=\s*(?P<const>-?(?:0[xX][0-9a-fA-F]+|\d+))\s*;$",
                          re.S)
DEFAULT_LIMIT = math.factorial(7)
DEFAULT_BUDGET = 2000


class SearchError(Exception):
    pass


@dataclass
class Block:
    start: int          # offset of the begin marker in the source
    end: int            # offset just past the end marker
    indent: str
    statements: list[str]


@dataclass
class Template:
    text: str
    blocks: list[Block]
    decl: tuple[int, int, str] | None   # (start, end, indent) of the decl marker


def split_statements(body: str) -> list[str]:
    """Top-level statements of a block body: `;`-terminated at depth 0, or a braced group."""
    out, cur, depth, i = [], [], 0, 0
    while i < len(body):
        c = body[i]
        if body.startswith("/*", i):
            j = body.find("*/", i + 2)
            if j < 0:
                raise SearchError("unterminated comment in order block")
            cur.append(body[i:j + 2])
            i = j + 2
            continue
        if body.startswith("//", i):
            j = body.find("\n", i)
            j = len(body) if j < 0 else j
            cur.append(body[i:j])
            i = j
            continue
        if c in "\"'":
            j = i + 1
            while j < len(body) and body[j] != c:
                j += 2 if body[j] == "\\" else 1
            cur.append(body[i:j + 1])
            i = j + 1
            continue
        cur.append(c)
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
            if depth < 0:
                raise SearchError("unbalanced brackets in order block")
        if depth == 0 and (c == ";" or (c == "}" and not _continues(body, i + 1))):
            stmt = "".join(cur).strip()
            if stmt and stmt != ";":
                out.append(stmt)
            cur = []
        i += 1
    if depth or "".join(cur).strip():
        raise SearchError(f"order block has an incomplete statement: {''.join(cur).strip()!r}")
    return out


def _continues(body: str, i: int) -> bool:
    """`} else ...` and `} while (...)` belong to the same statement."""
    rest = body[i:].lstrip()
    return rest.startswith("else") or rest.startswith("while") or rest.startswith(";")


def parse(text: str) -> Template:
    blocks, decl, begin = [], None, None
    for m in MARK.finditer(text):
        kind = m.group(1)
        indent = re.match(r"[ \t]*", m.group(0)).group(0)
        if kind == "decl":
            if decl is not None:
                raise SearchError("more than one /* order: decl */ marker")
            decl = (m.start(), m.end(), indent)
        elif kind == "begin":
            if begin is not None:
                raise SearchError("nested /* order: begin */")
            begin = m
        else:
            if begin is None:
                raise SearchError("/* order: end */ without begin")
            body = text[begin.end():m.start()]
            stmts = split_statements(body)
            if not stmts:
                raise SearchError("empty order block")
            ind = re.match(r"[ \t]*", begin.group(0)).group(0)
            blocks.append(Block(begin.start(), m.end(), ind, stmts))
            begin = None
    if begin is not None:
        raise SearchError("/* order: begin */ without end")
    if not blocks:
        raise SearchError("no /* order: begin */ ... /* order: end */ block in the source")
    if decl and any(b.start <= decl[0] < b.end for b in blocks):
        raise SearchError("/* order: decl */ must be outside the order blocks")
    return Template(text, blocks, decl)


def shared_constants(stmts: list[str]) -> list[str]:
    """Constants that are the whole right-hand side of two or more assignments."""
    counts: dict[str, int] = {}
    for s in stmts:
        m = CONST_ASSIGN.match(s)
        if m:
            counts[m.group("const")] = counts.get(m.group("const"), 0) + 1
    return [c for c, n in counts.items() if n >= 2]


def const_variants(template: Template, enabled: bool) -> list[tuple[tuple[int, str], ...]]:
    """Each variant is the set of (block index, constant) pairs that use a shared local."""
    if not enabled:
        return [()]
    options = [(bi, c) for bi, b in enumerate(template.blocks) for c in shared_constants(b.statements)]
    return [tuple(o for o, on in zip(options, bits) if on)
            for bits in itertools.product((False, True), repeat=len(options))]


def local_name(bi: int, const: str) -> str:
    tag = const.replace("-", "m").lower()
    return f"order_k{bi}_{tag}"


def render_block(block: Block, bi: int, order: tuple[int, ...], locals_: set[str],
                 wrap_decls: list[str]) -> str:
    lines = []
    for k in order:
        stmt = block.statements[k]
        m = CONST_ASSIGN.match(stmt)
        if m and m.group("const") in locals_:
            stmt = f"{m.group('lhs').rstrip()} = {local_name(bi, m.group('const'))};"
        lines.append(block.indent + stmt + "\n")
    if not wrap_decls:
        return "".join(lines)
    inner = "".join("    " + line for line in lines)
    decls = "".join(f"{block.indent}    {d}\n" for d in wrap_decls)
    return f"{block.indent}{{\n{decls}{inner}{block.indent}}}\n"


def render(template: Template, orders: tuple[tuple[int, ...], ...],
           variant: tuple[tuple[int, str], ...]) -> str:
    """Source text for one ordering and constant-local choice; markers are dropped."""
    per_block: dict[int, set[str]] = {}
    for bi, c in variant:
        per_block.setdefault(bi, set()).add(c)
    decls = [f"s32 {local_name(bi, c)} = {c};" for bi, c in variant]
    pieces: list[tuple[int, int, str]] = []
    for bi, (block, order) in enumerate(zip(template.blocks, orders)):
        wrap = [] if template.decl else [f"s32 {local_name(bi, c)} = {c};"
                                         for c in sorted(per_block.get(bi, ()))]
        pieces.append((block.start, block.end, render_block(block, bi, order, per_block.get(bi, set()), wrap)))
    if template.decl:
        start, end, indent = template.decl
        pieces.append((start, end, "".join(f"{indent}{d}\n" for d in decls)))
    out, pos = [], 0
    for start, end, repl in sorted(pieces):
        out.append(template.text[pos:start])
        out.append(repl)
        pos = end
    out.append(template.text[pos:])
    return "".join(out)


def candidates(template: Template, const_locals: bool):
    """Yield (orders, variant) in a deterministic order: source order and no locals first."""
    perms = [list(itertools.permutations(range(len(b.statements)))) for b in template.blocks]
    for variant in const_variants(template, const_locals):
        for orders in itertools.product(*perms):
            yield orders, variant


def count(template: Template, const_locals: bool) -> int:
    n = len(const_variants(template, const_locals))
    for b in template.blocks:
        n *= math.factorial(len(b.statements))
    return n


def compiler_args(compiler: str | None, source: Path) -> list[str]:
    """Carry the original unit's SDK settings across temporary candidate paths.

    Without COMPILER, an SDK/middleware SOURCE (an mwcc edge in build.ninja) still selects
    sdk: score_fn.py's own detection cannot see the unit from a temporary path."""
    try:
        from .score_fn import unit_mw_settings
    except ImportError:
        from score_fn import unit_mw_settings
    settings = unit_mw_settings(source) if compiler in (None, "sdk") else None
    compiler = compiler or ("sdk" if settings else None)
    if not compiler:
        return []
    args = ["--compiler", compiler]
    if compiler == "sdk":
        if settings:
            version, flags = settings
            args += ["--mw-version", version, "--mw-flags", shlex.join(flags)]
    return args


def score_source(name: str, text: str, extra: list[str], tmpdir: Path, index: int) -> dict:
    """Compile and score one variant with score_fn.py; returns its JSON report or an error."""
    src = tmpdir / f"cand{index}.c"
    src.write_text(text)
    r = subprocess.run([sys.executable, str(ROOT / "tools" / "score_fn.py"), name, str(src), "--json", *extra],
                       cwd=ROOT, capture_output=True, text=True)
    src.unlink()
    if r.returncode == 2 or not r.stdout.strip():
        return {"verdict": "ERROR", "error": (r.stderr or r.stdout).strip()[-500:], "returncode": r.returncode}
    rep = json.loads(r.stdout)
    rep["returncode"] = r.returncode
    return rep


def search(template: Template, scorer, *, const_locals: bool, jobs: int, limit: int, log=None):
    """Score every variant (in parallel batches); stop at the first strict MATCH."""
    total = count(template, const_locals)
    if total > limit:
        raise SearchError(f"{total} variants exceed --limit {limit}; use --climb, shrink the blocks "
                          "or raise --limit")
    best = None
    tried = errors = 0
    it = candidates(template, const_locals)
    with ThreadPoolExecutor(max_workers=max(1, jobs)) as pool:
        while True:
            batch = list(itertools.islice(it, max(1, jobs) * 4))
            if not batch:
                break
            texts = [render(template, o, v) for o, v in batch]
            reports = list(pool.map(lambda a: scorer(a[1], a[0]), enumerate(texts, tried)))
            for (orders, variant), text, rep in zip(batch, texts, reports):
                tried += 1
                if rep.get("verdict") == "ERROR":
                    errors += 1
                    continue
                key = rep["score"]
                if best is None or key < best["score"]:
                    best = {"score": key, "orders": orders, "variant": variant, "text": text, "report": rep}
                    if log:
                        log(f"[{tried}/{total}] {rep['verdict']} score={key} "
                            f"exact={rep['exact_insns']}/{rep['target_insns']}")
            if best and best["report"].get("returncode") == 0 and best["report"]["verdict"] == "MATCH":
                break
    return {"total": total, "tried": tried, "errors": errors, "best": best}


def moves(orders: tuple[tuple[int, ...], ...]):
    """Every order reachable by moving one statement to another position in its block."""
    seen = set()
    for bi, order in enumerate(orders):
        for i in range(len(order)):
            for j in range(len(order)):
                if i == j:
                    continue
                o = list(order)
                o.insert(j, o.pop(i))
                cand = orders[:bi] + (tuple(o),) + orders[bi + 1:]
                if cand not in seen:
                    seen.add(cand)
                    yield cand


def kick(orders: tuple[tuple[int, ...], ...], rng: random.Random, n: int):
    """Apply N random single-statement moves."""
    for _ in range(n):
        bi = rng.choice([b for b, o in enumerate(orders) if len(o) > 1])
        o = list(orders[bi])
        i, j = rng.sample(range(len(o)), 2)
        o.insert(j, o.pop(i))
        orders = orders[:bi] + (tuple(o),) + orders[bi + 1:]
    return orders


def climb(template: Template, scorer, *, jobs: int, budget: int, seed: int = 0, log=None):
    """Best-improvement hill climb over statement orders with random restarts from the best."""
    if all(len(b.statements) < 2 for b in template.blocks):
        raise SearchError("--climb needs a block with at least two statements")
    rng = random.Random(seed)
    seen: dict[tuple, dict | None] = {}
    state = {"tried": 0, "errors": 0, "best": None}
    pool = ThreadPoolExecutor(max_workers=max(1, jobs))

    def evaluate(cands: list[tuple]) -> list[tuple[tuple, dict | None]]:
        cands = [c for c in dict.fromkeys(cands) if c not in seen][:max(0, budget - state["tried"])]
        texts = [render(template, c, ()) for c in cands]
        reps = list(pool.map(lambda a: scorer(a[1], a[0]), enumerate(texts, state["tried"])))
        out = []
        for c, text, rep in zip(cands, texts, reps):
            state["tried"] += 1
            if rep.get("verdict") == "ERROR":
                state["errors"] += 1
                rep = None
            seen[c] = rep
            out.append((c, rep))
            best = state["best"]
            if rep and (best is None or rep["score"] < best["score"]):
                state["best"] = {"score": rep["score"], "orders": c, "variant": (), "text": text, "report": rep}
                if log:
                    log(f"[{state['tried']}/{budget}] {rep['verdict']} score={rep['score']} "
                        f"exact={rep['exact_insns']}/{rep['target_insns']}")
        return out

    def matched() -> bool:
        b = state["best"]
        return bool(b and b["report"].get("returncode") == 0 and b["report"]["verdict"] == "MATCH")

    try:
        start = tuple(tuple(range(len(b.statements))) for b in template.blocks)
        evaluate([start])
        if seen[start] is None:
            raise SearchError("the source order does not compile")
        cur, cur_score, kicks = start, seen[start]["score"], 0
        while state["tried"] < budget and not matched():
            neighbours = list(moves(cur))
            evaluate(neighbours)
            # Already-seen neighbours count too, so a revisited region still climbs.
            scored = [(c, seen[c]) for c in neighbours if seen.get(c)]
            nxt = min(scored, key=lambda cr: cr[1]["score"], default=None)
            if nxt and nxt[1]["score"] < cur_score:
                cur, cur_score = nxt[0], nxt[1]["score"]
                continue
            # Plateau: restart from the best order with 2-4 random moves.
            kicks += 1
            if kicks > 500:
                break
            for _ in range(50):
                cand = kick(state["best"]["orders"], rng, rng.randint(2, 4))
                if cand not in seen:
                    break
            evaluate([cand])
            if seen.get(cand) is None:
                continue
            cur, cur_score = cand, seen[cand]["score"]
            if log:
                log(f"[{state['tried']}/{budget}] plateau; kick {kicks} from the best order")
    finally:
        pool.shutdown()
    return {"total": budget, "tried": state["tried"], "errors": state["errors"], "best": state["best"]}


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="retail function name in symbols.txt")
    ap.add_argument("source", type=Path, help="candidate C file with order markers")
    ap.add_argument("--const-locals", action="store_true",
                    help="also try a shared s32 local for repeated assigned constants")
    ap.add_argument("-j", "--jobs", type=int, default=1, help="parallel compiles")
    ap.add_argument("--limit", type=int, default=DEFAULT_LIMIT, help="refuse larger searches (default 7!)")
    ap.add_argument("-o", "--output", type=Path, help="write the best variant here")
    ap.add_argument("--compiled-symbol", help="passed to score_fn.py")
    ap.add_argument("--compiler", choices=("prodg", "sdk"), help="passed to score_fn.py")
    ap.add_argument("-I", dest="includes", action="append", default=[], help="passed to score_fn.py")
    ap.add_argument("--body", type=Path, metavar="FILE",
                    help="take the marked definition from FILE (as for score_fn.py --body) and splice "
                         "it into SOURCE, without editing SOURCE")
    ap.add_argument("--dry-run", action="store_true", help="print the variant count and exit")
    ap.add_argument("--climb", action="store_true",
                    help="hill-climb with single-statement moves instead of trying every order")
    ap.add_argument("--budget", type=int, default=DEFAULT_BUDGET,
                    help=f"with --climb: stop after this many scored variants (default {DEFAULT_BUDGET})")
    ap.add_argument("--seed", type=int, default=0, help="with --climb: random seed for plateau kicks")
    args = ap.parse_args(argv)
    if args.climb and args.const_locals:
        ap.error("--climb does not combine with --const-locals; run the exhaustive search on the "
                 "climbed order instead")
    try:
        text = args.source.read_text()
        if args.body:
            sys.path.insert(0, str(Path(__file__).resolve().parent))
            from score_fn import splice_body
            text = splice_body(text, args.name, args.body.read_text())
        template = parse(text)
    except (OSError, SearchError) as e:
        print(f"error: {e}", file=sys.stderr)
        return 2
    total = count(template, args.const_locals)
    sizes = ", ".join(str(len(b.statements)) for b in template.blocks)
    mode = f"; climbing with a budget of {args.budget}" if args.climb else ""
    print(f"{len(template.blocks)} block(s) of {sizes} statements; {total} variants{mode}")
    if args.dry_run:
        return 0
    extra = ["-I", str(args.source.resolve().parent)]
    for inc in args.includes:
        extra += ["-I", inc]
    if args.compiled_symbol:
        extra += ["--compiled-symbol", args.compiled_symbol]
    extra += compiler_args(args.compiler, args.source)
    with tempfile.TemporaryDirectory(prefix="order_search_") as tmp:
        def scorer(text, index):
            return score_source(args.name, text, extra, Path(tmp), index)
        try:
            if args.climb:
                result = climb(template, scorer, jobs=args.jobs, budget=args.budget, seed=args.seed, log=print)
            else:
                result = search(template, scorer, const_locals=args.const_locals, jobs=args.jobs,
                                limit=args.limit, log=print)
        except SearchError as e:
            print(f"error: {e}", file=sys.stderr)
            return 2
    best = result["best"]
    print(f"tried {result['tried']}/{result['total']} variants, {result['errors']} compile errors")
    if best is None:
        print("error: no variant compiled", file=sys.stderr)
        return 2
    out = args.output or ROOT / "work" / "order_search" / args.name / "best.c"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(best["text"])
    rep = best["report"]
    locals_ = ", ".join(c for _, c in best["variant"]) or "none"
    print(f"best: {rep['verdict']} score={best['score']} exact={rep['exact_insns']}/{rep['target_insns']} "
          f"orders={[list(o) for o in best['orders']]} shared locals={locals_}")
    print(f"wrote {out}")
    if rep["verdict"] == "MATCH" and rep["returncode"] == 0:
        print("Port it into the unit source, then accept with verify_fn.py --build --strict-symbols.")
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main())
