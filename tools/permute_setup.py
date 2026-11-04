#!/usr/bin/env python3
"""Write a decomp-permuter directory for one function, scored by the strict verifier.

    python3 tools/permute_setup.py fn_8000A64C            # writes work/permute/fn_8000A64C/
    python3 tools/permute_setup.py fn_8000A64C --run -- -j 8 --stop-on-zero   # reuses the directory

The directory holds:

    base.c         the current best candidate, preprocessed with ProDG's CPP: the newest
                   best hillclimb ledger snapshot of the unit source, else the unit
                   source itself (`--source` overrides)
    target.o       the retail DTK split object that defines the function
    compile.sh     cc1 -> as -> dtk fixup on the preprocessed input (about 3x faster than
                   score_fn.py --compile-only, which it falls back to when the objects
                   for base.c differ)
    settings.toml  func_name, compiler_type and objdump_command
    permute.json   function, unit and provenance for permute_objdump.py

The permuter's own objdump scorer compares relocation symbol names and every
function in the object, so it cannot agree with verify_fn. `objdump_command`
therefore runs `permute_objdump.py`, which renders rows the strict verifier
accepted with the target's text and adds a sentinel line unless the verdict is a
strict MATCH: score 0 means `score_fn.py` reports MATCH. Permuter output is still
only a candidate. Port it into the unit source and accept it with
`verify_fn.py NAME --build --strict-symbols`.

Focus the search. Random whole-function mutation rarely lands the one-slot
schedule fixes a near miss needs, so mark the region that is wrong in the
candidate (`--source`, or a copy of the unit source):

    /* permute: begin */ ... /* permute: end */      only randomize these lines
    /* permute: lineswap */ ... /* permute: end */   only permute these lines' order

They become PERM_RANDOMIZE(...) and PERM_LINESWAP(...) in base.c; keep each
lineswap line a complete statement. For pure statement order inside one block,
`order_search.py --climb` is faster than the permuter.

Setup warns when the base differs structurally (instruction count or under 90%
exact), which the permuter seldom fixes, and when the function still carries
m2c names (temp_r3, var_f31), which waste its budget on noise.

`bash tools/bootstrap.sh` (or tools/fetch_permuter.sh) fetches the pinned
permuter into the shared cache; DECOMP_PERMUTER overrides the checkout.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import score_fn  # noqa: E402
import verify_fn as vf  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
SHIM = ROOT / "tools" / "permuter_shim"
FOCUS = re.compile(r"/\*\s*permute:\s*(begin|lineswap|end)\s*\*/")
M2C_NAME = re.compile(r"\b(?:temp|var)_[rf]\d+\b")
FOCUS_MACRO = {"begin": "PERM_RANDOMIZE(", "lineswap": "PERM_LINESWAP(", "end": ")"}


def permuter_checkout() -> Path:
    if os.environ.get("DECOMP_PERMUTER"):
        return Path(os.environ["DECOMP_PERMUTER"])
    cache = Path(os.environ.get("LSW1_CACHE", Path.home() / ".cache" / "lsw1"))
    return cache / "decomp-permuter"


def ledger_best(name: str, unit: str, compiled_symbol: str | None, source_rel: str,
                ledger: Path) -> tuple[str, str] | None:
    """(text, provenance) of the best scored attempt that snapshotted the unit source."""
    identity = {"function": name, "unit": unit, "compiled_symbol": compiled_symbol}
    key = hashlib.sha256(json.dumps(identity, sort_keys=True).encode()).hexdigest()[:20]
    best = None
    for path in sorted((ledger / key).glob("[0-9]*.json")):
        rec = json.loads(path.read_text())
        snap = rec.get("sources", {}).get(source_rel)
        if rec.get("score") is None or snap is None:
            continue
        if best is None or rec["score"] < best[0]:
            best = (rec["score"], snap["text"], f"{path.relative_to(ROOT)} (score {rec['score']})")
    return (best[1], best[2]) if best else None


def preprocess(source: Path, out: Path, include_dir: Path) -> str:
    """ProDG CPP with the pinned flags; '' on success, else the error text."""
    build = ROOT / "build"
    cpp = [str(build / "tools" / "wibo"), str(build / "compilers" / score_fn.PRODG_VERSION / "CPP.exe"), "-P"]
    flags = [f for f in score_fn.PRODG_FLAGS if not f.startswith("-O")]
    return score_fn.run(cpp + flags + ["-I", str(include_dir), str(source), str(out)])


def focus(text: str, keep: bool) -> tuple[str, int]:
    """Turn permute focus markers into PERM macros (keep) or drop them; returns (text, regions)."""
    depth = regions = 0
    for m in FOCUS.finditer(text):
        if m.group(1) == "end":
            depth -= 1
            if depth < 0:
                raise vf.ToolError("/* permute: end */ without begin or lineswap")
        else:
            depth += 1
            regions += 1
    if depth:
        raise vf.ToolError("unclosed /* permute: begin|lineswap */ marker")
    return FOCUS.sub(lambda m: FOCUS_MACRO[m.group(1)] if keep else "", text), regions


def m2c_names(text: str, symbol: str) -> list[str]:
    """m2c temporaries (temp_r3, var_f31) still left in SYMBOL's definition."""
    from variant_search import VariantError, find_function
    try:
        fn = find_function(text, symbol)
    except VariantError:
        return []
    return sorted(set(M2C_NAME.findall(fn.body)))


def score_compile_script(name: str) -> str:
    score = shlex.quote(str(ROOT / "tools" / "score_fn.py"))
    return ("#!/usr/bin/env bash\n"
            "# decomp-permuter compile step: compile.sh IN.c -o OUT.o\n"
            f'exec python3 {score} {shlex.quote(name)} "$1" --compile-only -o "$3"\n')


def compile_script(name: str) -> str:
    """Direct cc1 -> as -> dtk fixup on already-preprocessed input (base.c is CPP output)."""
    build = ROOT / "build"
    cc1 = [build / "tools" / "wibo", build / "compilers" / score_fn.PRODG_VERSION / "cc1.exe"]
    opt = [f for f in score_fn.PRODG_FLAGS if f.startswith("-O")]
    q = lambda parts: " ".join(shlex.quote(str(x)) for x in parts)  # noqa: E731
    return ("#!/usr/bin/env bash\n"
            f"# decomp-permuter compile step for {name}: compile.sh IN.c -o OUT.o\n"
            "# Same stages as tools/prodg_cc.py minus CPP (the input is preprocessed).\n"
            "set -e\n"
            'tmp=$(mktemp -d); trap \'rm -rf "$tmp"\' EXIT\n'
            f"cd {shlex.quote(str(ROOT))}\n"
            f'{q(cc1)} -quiet {q(opt)} "$1" -o "$tmp/c.s"\n'
            f'{q([build / "binutils" / "powerpc-eabi-as", "-mgekko"])} -o "$3" "$tmp/c.s"\n'
            f'{q([build / "tools" / "dtk_patched", "elf", "fixup"])} "$3" "$3"\n')


def fast_compile_agrees(out: Path, source: Path) -> bool:
    """True when compile.sh and score_fn.py --compile-only give identical objects for SOURCE."""
    with tempfile.TemporaryDirectory(prefix="permute_cmp_") as tmp:
        a, b = Path(tmp) / "fast.o", Path(tmp) / "score.o"
        fast = subprocess.run(["bash", str(out / "compile.sh"), str(source), "-o", str(a)],
                              capture_output=True, text=True)
        err = score_fn.compile_source(source, b, "prodg", [])
        return fast.returncode == 0 and not err and a.read_bytes() == b.read_bytes()


def settings_toml(func_name: str, directory: Path) -> str:
    dumper = f"python3 {ROOT / 'tools' / 'permute_objdump.py'} --dir {directory}"
    return (f'func_name = "{func_name}"\n'
            'compiler_type = "gcc"\n'
            f"objdump_command = {json.dumps(dumper)}\n")


def setup(name: str, out: Path, source: Path | None, compiled_symbol: str | None,
          ledger: Path, force: bool) -> dict:
    symbols = vf.SymbolTable()
    entry = symbols.function(name)
    unit = vf.lookup_unit(entry.addr)
    if not unit:
        raise vf.ToolError(f"no split unit covers 0x{entry.addr:08X}")
    target = vf.find_target_object(name, entry.addr, entry.size, unit)
    unit_source = vf.source_for_unit(unit)
    if source is None and (unit_source is None or unit_source.suffix != ".c"):
        raise vf.ToolError(f"unit {unit} has no C source; pass --source")
    if out.exists() and any(out.iterdir()) and not force:
        raise vf.ToolError(f"{out} already exists; pass --force to replace it")
    provenance = None
    text = None
    if source is not None:
        source = source.resolve()
        provenance = str(source.relative_to(ROOT) if source.is_relative_to(ROOT) else source)
    else:
        source = unit_source
        best = ledger_best(name, unit, compiled_symbol, str(unit_source.relative_to(ROOT)), ledger)
        if best:
            text, provenance = best
        else:
            provenance = str(unit_source.relative_to(ROOT))
    if out.exists():
        shutil.rmtree(out)
    out.mkdir(parents=True)
    if text is None:
        text = source.read_text()
    marked, regions = focus(text, keep=True)
    with tempfile.TemporaryDirectory(prefix="permute_setup_") as tmp:
        src = Path(tmp) / source.name
        src.write_text(marked)
        err = preprocess(src, out / "base.c", source.parent)
        if not err and regions:
            # base.c with PERM macros does not compile; score and check against a plain copy.
            src.write_text(focus(text, keep=False)[0])
            err = preprocess(src, out / "base_plain.c", source.parent)
    if err:
        raise vf.ToolError(f"preprocessing {provenance} failed:\n{err}")
    shutil.copyfile(target, out / "target.o")
    (out / "compile.sh").write_text(compile_script(name))
    (out / "compile.sh").chmod(0o755)
    plain = out / ("base_plain.c" if regions else "base.c")
    fast = fast_compile_agrees(out, plain)
    if not fast:
        (out / "compile.sh").write_text(score_compile_script(name))
    (out / "settings.toml").write_text(settings_toml(compiled_symbol or name, out.resolve()))
    meta = {"name": name, "compiled_symbol": compiled_symbol, "unit": unit,
            "target_object": str(target.relative_to(ROOT)), "base_source": provenance,
            "focus_regions": regions, "fast_compile": fast,
            "m2c_names": m2c_names(text, compiled_symbol or name)}
    (out / "permute.json").write_text(json.dumps(meta, indent=2) + "\n")
    return meta


def base_score(out: Path) -> dict:
    """Compile the base and score it the way the permuter will (sanity check)."""
    base = out / "base_plain.c" if (out / "base_plain.c").is_file() else out / "base.c"
    with tempfile.TemporaryDirectory(prefix="permute_base_") as tmp:
        obj = Path(tmp) / "base.o"
        r = subprocess.run(["bash", str(out / "compile.sh"), str(base), "-o", str(obj)],
                           capture_output=True, text=True)
    if r.returncode:
        raise vf.ToolError(f"{base.name} does not compile:\n{(r.stdout + r.stderr)[-1500:]}")
    meta = json.loads((out / "permute.json").read_text())
    args = [meta["name"], str(base), "--json", "--no-queue"]
    if meta.get("compiled_symbol"):
        args += ["--compiled-symbol", meta["compiled_symbol"]]
    r = subprocess.run([sys.executable, str(ROOT / "tools" / "score_fn.py"), *args],
                       cwd=ROOT, capture_output=True, text=True)
    try:
        return json.loads(r.stdout)
    except json.JSONDecodeError:
        raise vf.ToolError(f"scoring {base.name} failed:\n{(r.stderr or r.stdout).strip()[-1500:]}")


def warnings(rep: dict, meta: dict) -> list[str]:
    """Reasons a permuter run on this base is likely to waste its budget."""
    out = []
    target, mine, exact = rep.get("target_insns", 0), rep.get("mine_insns", 0), rep.get("exact_insns", 0)
    if rep.get("verdict") != "MATCH" and target and (mine != target or exact / target < 0.9):
        out.append(f"structural diff ({exact}/{target} exact, {mine} vs {target} instructions): the "
                   "permuter seldom fixes frame size, instruction count or missing code; fix the C first")
    if meta.get("m2c_names"):
        names = ", ".join(meta["m2c_names"][:6])
        out.append(f"m2c names remain ({names}): rename/fold temporaries and drop redundant casts "
                   "before permuting so mutations act on real structure")
    if not meta.get("focus_regions") and rep.get("verdict") != "MATCH":
        out.append("no /* permute: begin|lineswap */ focus: whole-function randomization; mark the "
                   "differing region to narrow the search")
    if not meta.get("fast_compile", True):
        out.append("fast compile.sh disagreed with score_fn.py on the base; using the slower score_fn wrapper")
    return out


def run_permuter(out: Path, extra: list[str]) -> int:
    checkout = permuter_checkout()
    if not (checkout / "permuter.py").is_file():
        print(f"error: decomp-permuter not found at {checkout}; run bash tools/fetch_permuter.sh",
              file=sys.stderr)
        return 2
    env = dict(os.environ)
    env["PYTHONPATH"] = os.pathsep.join(filter(None, [str(SHIM), env.get("PYTHONPATH")]))
    return subprocess.call([sys.executable, str(checkout / "permuter.py"), str(out), *extra], env=env)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="retail function name in symbols.txt")
    ap.add_argument("--source", type=Path, help="candidate C file (default: best ledger snapshot or unit source)")
    ap.add_argument("--compiled-symbol", help="symbol emitted by the source if it differs")
    ap.add_argument("--out", type=Path, help="directory (default work/permute/NAME)")
    ap.add_argument("--ledger", type=Path, default=ROOT / "work" / "hillclimb")
    ap.add_argument("--force", action="store_true", help="replace an existing directory")
    ap.add_argument("--run", action="store_true", help="start the permuter; arguments after -- pass through")
    args, extra = ap.parse_known_args(argv)
    if extra and extra[0] == "--":
        extra = extra[1:]
    if extra and not args.run:
        ap.error(f"unrecognized arguments: {' '.join(extra)}")
    out = (args.out or ROOT / "work" / "permute" / args.name).resolve()
    missing = score_fn.missing_toolchain("prodg")
    if missing:
        print("error: toolchain not bootstrapped; run bash tools/bootstrap.sh", file=sys.stderr)
        return 2
    rel = out.relative_to(ROOT) if out.is_relative_to(ROOT) else out
    reuse = args.run and not args.force and (out / "settings.toml").is_file()
    if not reuse:
        try:
            meta = setup(args.name, out, args.source, args.compiled_symbol, args.ledger.resolve(), args.force)
            rep = base_score(out)
        except (vf.ToolError, ValueError, OSError) as e:
            print(f"error: {e}", file=sys.stderr)
            return 2
        focus_note = f", {meta['focus_regions']} focus region(s)" if meta["focus_regions"] else ""
        print(f"wrote {rel} (base from {meta['base_source']}, target {meta['target_object']}{focus_note})")
        print(f"base: {rep['verdict']}  score={rep.get('score')}  "
              f"exact={rep.get('exact_insns')}/{rep.get('target_insns')}  mine_insns={rep.get('mine_insns')}")
        for w in warnings(rep, meta):
            print(f"warning: {w}")
    if args.run:
        return run_permuter(out, extra)
    print(f"next: python3 tools/permute_setup.py {args.name} --run -- -j 8 --stop-on-zero")
    return 0


if __name__ == "__main__":
    sys.exit(main())
