#!/usr/bin/env python3
"""Write a decomp-permuter directory for one function, scored by the strict verifier.

    python3 tools/permute_setup.py fn_8000A64C            # writes work/permute/fn_8000A64C/
    python3 tools/permute_setup.py fn_8000A64C --run -- -j 8 --stop-on-zero   # reuses the directory

The directory holds:

    base.c         the current best candidate, preprocessed with ProDG's CPP: the newest
                   best hillclimb ledger snapshot of the unit source, else the unit
                   source itself (`--source` overrides)
    target.o       the retail DTK split object that defines the function
    compile.sh     score_fn.py NAME "$1" --compile-only -o "$3" (ProDG, pinned flags)
    settings.toml  func_name, compiler_type and objdump_command
    permute.json   function, unit and provenance for permute_objdump.py

The permuter's own objdump scorer compares relocation symbol names and every
function in the object, so it cannot agree with verify_fn. `objdump_command`
therefore runs `permute_objdump.py`, which renders rows the strict verifier
accepted with the target's text and adds a sentinel line unless the verdict is a
strict MATCH: score 0 means `score_fn.py` reports MATCH. Permuter output is still
only a candidate. Port it into the unit source and accept it with
`verify_fn.py NAME --build --strict-symbols`.

`bash tools/bootstrap.sh` (or tools/fetch_permuter.sh) fetches the pinned
permuter into the shared cache; DECOMP_PERMUTER overrides the checkout.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
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


def compile_script(name: str) -> str:
    score = shlex.quote(str(ROOT / "tools" / "score_fn.py"))
    return ("#!/usr/bin/env bash\n"
            "# decomp-permuter compile step: compile.sh IN.c -o OUT.o\n"
            f'exec python3 {score} {shlex.quote(name)} "$1" --compile-only -o "$3"\n')


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
    with tempfile.TemporaryDirectory(prefix="permute_setup_") as tmp:
        src = source
        if text is not None:
            src = Path(tmp) / source.name
            src.write_text(text)
        err = preprocess(src, out / "base.c", source.parent)
    if err:
        raise vf.ToolError(f"preprocessing {provenance} failed:\n{err}")
    shutil.copyfile(target, out / "target.o")
    (out / "compile.sh").write_text(compile_script(name))
    (out / "compile.sh").chmod(0o755)
    (out / "settings.toml").write_text(settings_toml(compiled_symbol or name, out.resolve()))
    meta = {"name": name, "compiled_symbol": compiled_symbol, "unit": unit,
            "target_object": str(target.relative_to(ROOT)), "base_source": provenance}
    (out / "permute.json").write_text(json.dumps(meta, indent=2) + "\n")
    return meta


def base_score(out: Path) -> str:
    """Compile base.c and score it the way the permuter will (sanity check)."""
    with tempfile.TemporaryDirectory(prefix="permute_base_") as tmp:
        obj = Path(tmp) / "base.o"
        r = subprocess.run(["bash", str(out / "compile.sh"), str(out / "base.c"), "-o", str(obj)],
                           capture_output=True, text=True)
    if r.returncode:
        raise vf.ToolError(f"base.c does not compile:\n{(r.stdout + r.stderr)[-1500:]}")
    meta = json.loads((out / "permute.json").read_text())
    args = [meta["name"], str(out / "base.c")]
    if meta.get("compiled_symbol"):
        args += ["--compiled-symbol", meta["compiled_symbol"]]
    r = subprocess.run([sys.executable, str(ROOT / "tools" / "score_fn.py"), *args],
                       cwd=ROOT, capture_output=True, text=True)
    return (r.stdout or r.stderr).strip()


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
            base = base_score(out)
        except (vf.ToolError, ValueError, OSError) as e:
            print(f"error: {e}", file=sys.stderr)
            return 2
        print(f"wrote {rel} (base from {meta['base_source']}, target {meta['target_object']})")
        print(f"base: {base}")
    if args.run:
        return run_permuter(out, extra)
    print(f"next: python3 tools/permute_setup.py {args.name} --run -- -j 8 --stop-on-zero")
    return 0


if __name__ == "__main__":
    sys.exit(main())
