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
typing a header can only raise the sweep's yield. m2c declares only the arguments a
function reads, so each draft is also scored with its signature padded to the registers
those arguments arrive in (`pad_signature`), keeping the better score.
Each draft's callee declarations are then repaired (`fix_prototypes`: the unit's or the
repo's prototype, or a variadic/unprototyped one where the target sets cr1eq) and its
undeclared stack slots folded into arrays (`stack_arrays`), keeping the better score.

`--apply` writes strict MATCHes into their unit sources one at a time, re-scoring each
against the updated file with `score_fn.py --apply`; run `bash build.sh` and
`sprint_finish.py` afterwards for the build-level strict verification. Everything else is
ranked in `work/sweep/results.json` (and printed) by unmatched bytes closest to a match:
that ranking is the agent queue, biggest progress first. Task packs go to
`work/sweep/NAME/`, which does not claim the target for `next_target.py`; their asm.s is
refreshed from the current build first (tools/pack_asm.py). A full sweep (no `--unit` or
`--retry`) also writes `work/sweep/meta.json`: the swept HEAD, time and verdict counts, which
`kickoff.py` reads to tell whether the results are stale.

m2c output and score verdicts are cached in `~/.cache/lsw1/sweep` (`LSW1_SWEEP_CACHE`),
shared by every checkout. A verdict's key covers the scoring tools, the unit source, its
preprocessed headers, the target assembly and the body; an m2c key covers the m2c commit,
flags, assembly and context text. sweep.py's own rewrites always rerun, so a sweep.py
change needs no invalidation. `--no-cache` bypasses both.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
import tempfile
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import draft_fn  # noqa: E402
import mac_protos  # noqa: E402
import pack_asm  # noqa: E402
from m2c_helpers import ASM_COMMENT as ASM_COMMENT_RE  # noqa: E402
from m2c_helpers import (split_args, expand_macros, fold_cror, fold_ps, fix_ps_calls,
                         moves_to_args, type_derefs, strip_vararg_saves, declare_unset,
                         call_sites, PS_CALLS, prepare_asm, repair_indirect_prototypes)
import sweep_repairs  # noqa: E402  (registers every repair module)
# Repairs live in tools/sweep_repairs/; re-exported for the tools and tests that use them here.
from sweep_repairs._common import call_args  # noqa: E402,F401
from sweep_repairs.callees import float_blocks, int_callees, void_callees, wide_return  # noqa: E402,F401
from sweep_repairs.frames import empty_frame, frame_buffer  # noqa: E402,F401
from sweep_repairs.guards import negate, positive_guards, split_top, unwrap  # noqa: E402,F401
from sweep_repairs.loops import countdown_loops, index_loops, loop_guard  # noqa: E402,F401
from sweep_repairs.pointers import base_pointer, byte_strides  # noqa: E402,F401
from sweep_repairs.prototypes import (adopt_own_prototype, expand_by_register,  # noqa: E402,F401
                                      find_protos, fix_prototypes, float_positions, parse_proto,
                                      passthrough, place_args, plain_type, raw_protos, repo_protos,
                                      variadic_calls)
from sweep_repairs.signature import add_params, pad_signature, passthrough_regs  # noqa: E402,F401
from sweep_repairs.stack import float_ge, native_shifts, stack_arrays, volatile_locals  # noqa: E402,F401
from sweep_repairs.stores import (both_rewrites, const_rodata, rodata_symbols,  # noqa: E402,F401
                                  sorted_stores, store_in_arms, swap_stores)
from sweep_repairs.switches import switch_chain, switch_pivots  # noqa: E402,F401

OUT = ROOT / "work" / "sweep"
# Macros that only stand for an instruction m2c could not express; such a draft cannot match.
UNHANDLED = re.compile(r"\bM2C_(?:ERROR|TRAP_IF|BREAK|SYNC|DCACHE|ICACHE|PREFETCH|CARRY|OVERFLOW|"
                       r"LWL|FIRST3BYTES|UNALIGNED32|LOAD_|STORE_)|\b(?:GLUE_F64|MULT_HI|MULTU_HI|"
                       r"CLZ|ROTATE_RIGHT|BSWAP\d+)\(")


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
                warm_m2c_cache(out)
                _CONTEXTS[source] = out
            else:
                _CONTEXTS[source] = None
        return _CONTEXTS[source]


def warm_m2c_cache(context: Path) -> None:
    """Build m2c's typemap cache (CONTEXT.m2c) once, so parallel drafts load it instead of
    each re-parsing the context (0.67 s -> 0.14 s per draft). m2c validates it by a hash of
    the context text, so a stale cache is rebuilt rather than used."""
    warm = context.with_suffix(".warm.s")
    warm.write_text("glabel warm\nblr\n")
    subprocess.run([sys.executable, str(draft_fn.m2c_dir() / "m2c.py"), "-t", draft_fn.TARGET,
                    "--context", str(context), str(warm)], capture_output=True)


def m2c_output(cmd: list[str], asm: Path, context: Path | None, sized: bool = False) -> str:
    """m2c's C for ASM ('' on failure), cached in CACHE_ROOT/m2c by m2c commit, flags
    (SIZED included), asm and context text: sweep.py's own post-processing still reruns."""
    h = hashlib.sha256(m2c_version().encode())
    for part in (" ".join(cmd[2:-1] if context else cmd[2:]) + (" sized" if sized else ""), asm.read_text(),
                 file_digest(context) if context else ""):
        h.update(len(part).to_bytes(8, "little") + part.encode())
    digest = h.hexdigest()
    path = CACHE_ROOT / "m2c" / digest[:2] / f"{digest}.c"
    if USE_CACHE:
        try:
            return path.read_text()
        except OSError:
            pass
    env = {**os.environ, "M2C_SIZED_DEREFS": "1"} if sized else None
    r = subprocess.run(cmd + [str(asm.resolve())], capture_output=True, text=True, env=env)
    out = "" if r.returncode or not r.stdout.strip() else r.stdout
    if USE_CACHE and out:
        write_atomic(path, out)
    return out


_M2C_VERSION: list[str] = []


def m2c_version() -> str:
    """m2c's checked-out commit (fetch_m2c.sh pins it), or a digest of its sources."""
    if not _M2C_VERSION:
        m2c = draft_fn.m2c_dir()
        r = subprocess.run(["git", "-C", str(m2c), "rev-parse", "HEAD"], capture_output=True, text=True)
        dirty = subprocess.run(["git", "-C", str(m2c), "status", "--porcelain", "--untracked-files=no"],
                               capture_output=True, text=True)
        if r.returncode == 0 and dirty.returncode == 0 and not dirty.stdout.strip():
            _M2C_VERSION.append(r.stdout.strip())
        else:
            h = hashlib.sha256()
            for f in sorted(m2c.rglob("*.py")):
                h.update(str(f.relative_to(m2c)).encode() + f.read_bytes())
            _M2C_VERSION.append(h.hexdigest())
    return _M2C_VERSION[0]


_DIGESTS: dict[tuple[Path, int, int], str] = {}


def file_digest(path: Path) -> str:
    """sha256 of PATH's bytes, memoised per (path, mtime, size) for cache keys."""
    st = path.stat()
    key = (path, st.st_mtime_ns, st.st_size)
    if key not in _DIGESTS:
        _DIGESTS[key] = hashlib.sha256(path.read_bytes()).hexdigest()
    return _DIGESTS[key]


def draft(name: str, context: Path | None = None, sized: bool = False) -> tuple[str, str]:
    """(status, C body) for NAME; status is 'ok', 'no-asm', 'm2c-failed' or 'unhandled'.

    SIZED sets M2C_SIZED_DEREFS (tools/patches/m2c-sized-derefs.patch): `*(a + b)` derefs
    keep their load width as `*(u8 *)(a + b)`, which the C types may otherwise lose."""
    task = OUT / name
    asm = pack_asm.ensure_asm(name, OUT, asm_only=True)  # created when missing, refreshed from the build when stale
    if not asm.is_file() or not asm.read_text().strip():
        return "no-asm", ""
    m2c = draft_fn.m2c_dir()
    m2c_in = task / "asm_m2c.s"
    m2c_in.write_text(prepare_asm(asm.read_text()))
    cmd = [sys.executable, str(m2c / "m2c.py"), "-t", draft_fn.TARGET, "--valid-syntax"]
    if context is not None:
        cmd += ["--context", str(context)]
    raw = m2c_output(cmd, m2c_in, context, sized)
    if not raw:
        return "m2c-failed", ""
    out = declare_unset(raw, name)
    if UNHANDLED.search(out):
        return "unhandled", ""
    try:
        body = type_derefs(expand_macros(out))
    except ValueError:
        return "m2c-failed", ""
    body = draft_fn.clean(body, name, asm.read_text())
    body = body.replace('#include "types.h"\n', "", 1).strip() + "\n"
    return "ok", body


CONFLICT = re.compile(r"conflicting types for [`'‘](\w+)|redefinition of [`'‘](\w+)|"
                      r"previous declaration of [`'‘](\w+)|[`'‘](\w+)' redeclared as different kind")


def drop_decl(body: str, sym: str, name: str) -> str | None:
    """Remove m2c's top-level declaration of SYM (never NAME's own definition)."""
    if sym == name:
        return None
    lines = body.splitlines()
    kept = [ln for ln in lines if not (ln.rstrip().endswith(";") and not ln.startswith((" ", "\t"))
                                        and re.search(rf"\b{re.escape(sym)}\b", ln))]
    return "\n".join(kept) + "\n" if len(kept) != len(lines) else None


DECL_LINE = re.compile(r"^(?:extern\s+)?(?P<decl>[^;{}()=#]*?\b(?P<sym>\w+)\b(?P<rest>\s*(?:\[[^\]]*\]\s*)*|\s*\([^;{}]*\)\s*));\s*$")


def alias_decl(body: str, sym: str, name: str) -> str | None:
    """Replace m2c's top-level declaration of SYM, which conflicts with the unit's, by a macro
    that reads SYM through m2c's type: `#define g (*(T *(*))&g)` for data and
    `#define f ((R (*)(A))f)` for a function. The body keeps m2c's typing, the code generated
    for the access is the same, and an `#undef` after the body restores the unit's type."""
    if sym == name:
        return None
    lines = body.splitlines()
    for i, ln in enumerate(lines):
        if ln.startswith((" ", "\t")) or not re.search(rf"\b{re.escape(sym)}\b", ln):
            continue
        m = DECL_LINE.match(ln.strip())
        if not m or m["sym"] != sym or m["decl"].lstrip().startswith(("static", "typedef")):
            continue
        decl = re.sub(r"^extern\s+", "", ln.strip()[:-1].strip())
        abstract = re.sub(rf"\b{re.escape(sym)}\b", "(*)", decl, count=1)
        if m["rest"].strip().startswith("("):
            lines[i] = f"#define {sym} (({abstract}){sym})"
        else:
            lines[i] = f"#define {sym} (*({abstract})&{sym})"
        return "\n".join(lines) + f"\n#undef {sym}\n"
    return None


_SCORES: dict[tuple[str, str, str], dict] = {}
# Verdicts and m2c output shared by every checkout and sweep run, keyed by their inputs.
CACHE_ROOT = Path(os.environ.get("LSW1_SWEEP_CACHE") or Path.home() / ".cache/lsw1/sweep")
# score_fn.py and every tools/ module it imports (tests.test_sweep checks the closure).
# score_fn.py hardcodes its compiler flags, so configure.py and config.yml are not inputs:
# keying on them dropped the whole cache on most merges.
SCORE_TOOLS = ("score_fn.py", "prodg_cc.py", "verify_fn.py", "verify_unit.py", "hillclimb.py",
               "variant_search.py", "order_search.py", "sprint_bench.py", "sprint_metrics.py", "opensaga.py",
               "prodg_branch_relocs.py", "strip_unused.py")
SCORE_CONFIG: tuple[str, ...] = ()
CACHED_VERDICTS = {"MATCH", "MISMATCH", "COMPILE-ERROR"}
USE_CACHE = True
_TOOLS_DIGEST: list[str] = []


def target_object(source: str) -> Path:
    """The split retail object verify_fn.py compares SOURCE's functions with."""
    rel = Path(source)
    return ROOT / "build/GL5E4F/obj" / (rel.relative_to("src") if rel.is_relative_to("src") else rel).with_suffix(".o")


def score_key(name: str, source: str, body: str) -> str | None:
    """Digest of a score's inputs: the scoring tools, the unit source and its preprocessed headers
    (unit_context), the unit's target object, the function's target asm and the candidate body;
    None without them."""
    if not _TOOLS_DIGEST:
        h = hashlib.sha256()
        for tool in SCORE_TOOLS:
            h.update((Path(__file__).resolve().parent / tool).read_bytes())
        for flags in SCORE_CONFIG:  # per-unit compiler and flags
            h.update((Path(__file__).resolve().parent.parent / flags).read_bytes())
        _TOOLS_DIGEST.append(h.hexdigest())
    asm = pack_asm.ensure_asm(name, OUT, make=False)  # keyed on the current target asm
    ctx = unit_context(source) if asm.is_file() else None
    obj = target_object(source)
    if ctx is None or not obj.is_file():
        return None
    h = hashlib.sha256(_TOOLS_DIGEST[0].encode())
    for part in (name, file_digest(ROOT / source), file_digest(ctx), file_digest(obj), file_digest(asm), body):
        h.update(len(part).to_bytes(8, "little") + part.encode())
    return h.hexdigest()


def score(name: str, source: str, body: str, apply: bool = False) -> dict:
    """score_fn.py's verdict for BODY. Repeat checks reuse the first result: in-run
    (repairs and variants often converge on the same text) and across runs and checkouts
    through CACHE_ROOT/scores (a rerun after a sweep.py change rescores only changed drafts)."""
    if apply:
        return _score(name, source, body, True)
    key = (name, source, body)
    if key not in _SCORES:
        digest = score_key(name, source, body) if USE_CACHE else None
        path = CACHE_ROOT / "scores" / digest[:2] / f"{digest}.json" if digest else None
        try:
            _SCORES[key] = json.loads(path.read_text()) if path else None
        except (OSError, ValueError):
            _SCORES[key] = None
        if _SCORES[key] is None:
            _SCORES[key] = _score(name, source, body)
            if path and _SCORES[key].get("verdict") in CACHED_VERDICTS:
                write_atomic(path, json.dumps(_SCORES[key]))
    return strip_tmp(dict(_SCORES[key]))  # verdicts cached before strip_tmp keep the name


def write_atomic(path: Path, text: str) -> None:
    try:
        path.parent.mkdir(parents=True, exist_ok=True)
        tmp = path.with_name(f"{path.name}.{os.getpid()}.{threading.get_ident()}")
        tmp.write_text(text)
        tmp.replace(path)
    except OSError:
        pass


def _score(name: str, source: str, body: str, apply: bool = False) -> dict:
    with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False) as f:
        f.write(body)
        path = f.name
    try:
        cmd = [sys.executable, str(ROOT / "tools" / "score_fn.py"), name, source, "--body", path, "--no-queue"]
        cmd += ["--apply"] if apply else ["--json"]
        r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return {"verdict": "TIMEOUT"}
    finally:
        Path(path).unlink(missing_ok=True)
    if apply:
        return {"verdict": "MATCH" if r.returncode == 0 else "NOT-APPLIED", "log": r.stdout[-300:]}
    if r.returncode == 2 and not r.stdout.strip().startswith("{"):
        return strip_tmp({"verdict": "COMPILE-ERROR", "error": r.stderr})
    try:
        rep = json.loads(r.stdout)
    except ValueError:
        return strip_tmp({"verdict": "ERROR", "error": (r.stdout + r.stderr)[-400:]})
    return {"verdict": rep.get("verdict"), "exact": rep.get("exact_insns"), "target": rep.get("target_insns")}


TMP_DIR = re.compile(r"(?:\b[A-Za-z]:)?[^\s:'\"`]*?score_fn_[A-Za-z0-9_]+[/\\]")


def strip_tmp(res: dict) -> dict:
    """RES with score_fn.py's random temporary directory dropped from its error text, so a
    COMPILE-ERROR row reads the same in every run (`Z:/tmp/score_fn_x1y2/u.c:5: ...` -> `u.c:5: ...`)."""
    if isinstance(res.get("error"), str):
        res["error"] = TMP_DIR.sub("", res["error"])
    return res


RANK = {"MATCH": 3, "MISMATCH": 2}


def better(a: dict, b: dict) -> bool:
    """Whether score A beats score B (MATCH, then more exact instructions)."""
    return (RANK.get(a["verdict"], 0), a.get("exact") or 0) > (RANK.get(b["verdict"], 0), b.get("exact") or 0)


def improves(new: dict, old: dict) -> bool:
    """Whether a repaired draft's score replaces OLD. Repairs accumulate, so a draft that
    still fails to compile is kept in its further-repaired form (its error is the live one);
    repair_draft retries the forms such a blind acceptance replaced."""
    return better(new, old) or new["verdict"] == old["verdict"] == "COMPILE-ERROR"


SCORE_FIELDS = ("verdict", "exact", "target", "error")


def repair_steps(name: str, asm_text: str, unit_text: str) -> list[tuple[str, object]]:
    """The repairs repair_draft applies after stack_arrays, in order, as
    (row flag, rewrite(body, res) -> body)."""
    steps = [(f.__name__, lambda b, r, f=f: f(b, asm_text)) for f in sweep_repairs.stage("frame")]
    steps += [(f.__name__, lambda b, r, f=f: f(b)) for f in sweep_repairs.stage("scored")]
    steps += [("padded", lambda b, r: pad_signature(b, name)),
              ("prototypes", lambda b, r: fix_prototypes(b, name, asm_text, unit_text)),
              ("own_prototype", lambda b, r: adopt_own_prototype(b, name, unit_text)
               if r["verdict"] == "COMPILE-ERROR" else b)]
    return steps


def run_steps(name: str, source: str, steps, start: int, res: dict | None, body: str,
              unit_text: str, asm_text: str, checkpoints: list | None) -> tuple[dict, str]:
    """Apply STEPS[START:] to BODY (scored RES), then the Mac demo's
    prototypes. With CHECKPOINTS, a failing draft keeps a failing repair (`improves`) and
    each form it replaced is recorded as (next step, res, body); without, a repair is kept
    only when it scores better."""
    keep = improves if checkpoints is not None else better
    pre = body
    for i in range(start, len(steps)):
        flag, rewrite = steps[i]
        if flag == "prototypes":
            pre = body
        alt = rewrite(body, res)
        if alt == body:
            continue
        alt_res, alt = score_draft(name, source, alt)
        if keep(alt_res, res):
            if checkpoints is not None and not better(alt_res, res):
                checkpoints.append((i + 1, res, body))
            flags = {k: v for k, v in res.items() if k not in SCORE_FIELDS}
            res, body = {**alt_res, **flags, flag: True}, alt
    for mac_body in (fix_prototypes(pre, name, asm_text, unit_text, True),
                     mac_protos.pad_params(body, name)):  # the Mac demo's types: strictly better only
        if mac_body not in (pre, body):
            mac_res, mac_body = score_draft(name, source, mac_body)
            if better(mac_res, res):
                flags = {k: v for k, v in res.items() if k not in SCORE_FIELDS}
                res, body = {**mac_res, **flags, "mac": True}, mac_body
    return res, body


def repair_draft(name: str, source: str, body: str, asm_text: str, unit_text: str) -> tuple[dict, str]:
    """(score, body) of m2c draft BODY after the sweep's repairs (repair_steps). Repairs
    accumulate on a draft that fails to compile; each form such a failing repair replaced
    (and the draft before stack_arrays) is then repaired again keeping only improvements,
    and the best result wins: a repair accepted blind can never hide a draft that a later
    repair alone would have made compile, nor one it would have compiled better."""
    steps = repair_steps(name, asm_text, unit_text)
    stacked = stack_arrays(body)
    checkpoints: list = [(0, None, body)] if stacked != body else []
    res, out = score_draft(name, source, stacked)
    res, out = run_steps(name, source, steps, 0, res, out, unit_text, asm_text, checkpoints)
    for start, cp_res, cp_body in checkpoints:
        if cp_res is None:
            cp_res, cp_body = score_draft(name, source, cp_body)
        alt_res, alt = run_steps(name, source, steps, start, cp_res, cp_body, unit_text, asm_text, None)
        if better(alt_res, res):
            res, out = alt_res, alt
    return res, out


def sweep_one(t: dict) -> dict:
    name, source = t["name"], t["source"]
    row = {"name": name, "unit": t["unit"], "source": source, "size": t["size"]}
    best = None
    ctx = unit_context(source)
    unit_text = (ctx or ROOT / source).read_text(errors="replace")
    drafts = []
    for context in [None] + ([ctx] if ctx else []):
        plain = draft(name, context)
        drafts.append((context, plain))
        sized = draft(name, context, sized=True)
        if sized != plain:  # scored only where the width casts change the draft
            drafts.append((context, sized))
    for context, (status, body) in drafts:
        if status != "ok":
            res = {"verdict": status.upper()}
        else:
            res, body = repair_draft(name, source, body, (OUT / name / "asm.s").read_text(), unit_text)
            res["context"] = context is not None
            res, body = try_variants(name, source, res, body)
            res, body = final_repairs(name, source, res, body,
                                      (OUT / name / "asm.s").read_text(), unit_text)
        if best is None or better(res, best[0]):
            best = (res, body)
        if res["verdict"] == "MATCH":
            break
    res, body = best
    if res["verdict"] in ("MATCH", "MISMATCH"):
        (OUT / name / "sweep_draft.c").write_text(body)
    elif res["verdict"] == "COMPILE-ERROR" and body:
        (OUT / name / "sweep_error.c").write_text(body)  # the failing draft, for repair work
    err = res.pop("error", "").strip()
    if res["verdict"] in ("COMPILE-ERROR", "ERROR") and err:
        msgs = [ln for ln in err.splitlines() if re.search(r":\d+: ", ln)
                and not re.search(r"\(Each undeclared|for each function it appears in|previous declaration|"
                                  r"this is the first entry", ln)]
        res["error"] = (msgs[0] if msgs else err.splitlines()[-1])[:200]
    return {**row, **res}


def final_repairs(name: str, source: str, res: dict, body: str,
                  asm: str, unit: str) -> tuple[dict, str]:
    """Independent alternatives to the completed pipeline; always retain its result.

    A locally better greedy repair can block a later improvement. Final repairs each
    start from the same completed draft and cannot lower its score, even indirectly.
    """
    base = body
    for rewrite in sweep_repairs.stage("final"):
        alternatives = rewrite(base, name, asm, unit)
        if isinstance(alternatives, str):
            alternatives = [alternatives]
        for alt in alternatives:
            if alt == base:
                continue
            candidate, alt = score_draft(name, source, alt)
            candidate, alt = try_variants(name, source, candidate, alt)
            if better(candidate, res):
                candidate[rewrite.__name__] = True
                candidate["context"] = res.get("context")
                res, body = candidate, alt
    return res, body


def try_variants(name: str, source: str, res: dict, body: str) -> tuple[dict, str]:
    """Score each `variant`/`variants` repair of a draft at least 80% exact; keep the best."""
    if res["verdict"] != "MISMATCH" or (res.get("exact") or 0) < 0.8 * (res.get("target") or 1):
        return res, body
    seen = {body}
    alts = [(v.__name__, v(body)) for v in sweep_repairs.stage("variant")]
    alts += [(v.__name__, a) for v in sweep_repairs.stage("variants") for a in v(body)]
    for vname, alt in alts:
        if alt in seen:
            continue
        seen.add(alt)
        alt_res, alt = score_draft(name, source, alt)
        if better(alt_res, res):
            res, body = {**alt_res, "context": res.get("context"), "rewrite": vname}, alt
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
            if (new := alias_decl(body, sym, name) or drop_decl(body, sym, name)):
                body = new
                break
        else:
            break
    else:
        res = score(name, source, body)
    return res, body


def kept_rows(previous: list[dict], unit: str, todo: list[dict]) -> list[dict]:
    """Rows of an earlier results.json that a sweep of the unit prefix UNIT does not replace."""
    names = {t["name"] for t in todo}
    return [r for r in previous if not r["unit"].startswith(unit) and r["name"] not in names
            and not r.get("applied")]


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
    ap.add_argument("--no-cache", action="store_true",
                    help="rerun m2c and rescore every body instead of reusing ~/.cache/lsw1/sweep")
    ap.add_argument("--retry", metavar="VERDICT", action="append",
                    help="resweep only the targets the last results.json gave VERDICT, or the named "
                         "target (repeatable, e.g. COMPILE-ERROR), and merge the new rows into it")
    args = ap.parse_args(argv)
    global USE_CACHE
    USE_CACHE = not args.no_cache
    if not (draft_fn.m2c_dir() / "m2c.py").is_file():
        print("error: m2c not fetched; run bash tools/fetch_m2c.sh", file=sys.stderr)
        return 2
    if not any((ROOT / "build/GL5E4F/asm").rglob("*.s")):
        print("error: no generated target assembly; run bash build.sh to completion before sweeping",
              file=sys.stderr)
        return 2
    todo = targets(args)
    previous = []
    if args.retry:
        previous = json.loads((OUT / "results.json").read_text())
        names = {r["name"] for r in previous if r["verdict"] in args.retry or r["name"] in args.retry}
        names |= set(args.retry)  # a named target the last sweep did not cover
        todo = [t for t in todo if t["name"] in names]
        previous = [r for r in previous if r["name"] not in {t["name"] for t in todo}
                    and not r.get("applied")]
    elif args.unit and (OUT / "results.json").is_file():
        # a unit-scoped sweep refreshes that unit's rows and keeps the rest of the ranking
        previous = kept_rows(json.loads((OUT / "results.json").read_text()), args.unit, todo)
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
            ok = score(row["name"], row["source"], body, apply=True)["verdict"] == "MATCH"
            if not ok:  # an earlier match in the same unit can now conflict with its externs
                res, body = score_draft(row["name"], row["source"], body)
                ok = res["verdict"] == "MATCH" and score(
                    row["name"], row["source"], body, apply=True)["verdict"] == "MATCH"
                if ok:
                    (OUT / row["name"] / "sweep_draft.c").write_text(body)
            if ok:
                applied.append(row["name"])
                row["applied"] = True
    if previous:
        print(f"swept {len(rows)} targets; {len(previous)} rows kept from the last results.json")
    rows += previous
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
    if not args.unit and not args.retry:  # a full sweep: record what it swept for kickoff.py
        head = subprocess.run(["git", "rev-parse", "HEAD"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
        (OUT / "meta.json").write_text(json.dumps({"head": head, "time": int(time.time()), "max_size": args.max_size,
                                                   "verdicts": counts}, indent=1) + "\n")
    if counts.get("NO-ASM"):
        print("error: incomplete sweep (NO-ASM); run bash build.sh, then rerun the affected units",
              file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main())
