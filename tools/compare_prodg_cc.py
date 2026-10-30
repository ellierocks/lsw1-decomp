#!/usr/bin/env python3
"""Compare ProDG compiler variants on the game units, one hypothesis at a time.

Compiles each ProDG source with the pinned wrapper (baseline) and with each
variant, then strictly verifies every function the object defines against
retail with the same verifier as `score_fn.py`. A variant changes exactly what
its spec names, so compiler build, preprocessing defines, cc1 flags and alias
behavior are separate runs.

    python3 tools/compare_prodg_cc.py --out /tmp/cmp \\
        --variant 'v179:cc1=build/sn-gcc/v179/cc1' \\
        --variant 'defines:defines=ngccc'

Spec: NAME[:key=value;key=value]. Keys: cc1 (native cc1 path), defines=ngccc
(driver defines), flag (repeatable cc1 flag, e.g. flag=-mfast-cast), cpp
(repeatable extra -D/-I preprocessing flag). Prints a table; --json keeps the
full per-function results. Raw objects stay under --out (keep them local).
"""

from __future__ import annotations

import argparse
import contextlib
import hashlib
import io
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import verify_fn as vf  # noqa: E402
from hillclimb import score  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
PRODG = BUILD / "compilers" / "ProDG/3.9.3"
BASE_FLAGS = ["-O2", "-I", "src", "-I", "include", "-I", "build/GL5E4F/include",
              "-DBUILD_VERSION=0", "-DVERSION_GL5E4F", "-DNDEBUG=1"]
DEFAULT_SOURCES = ["src/nucore/nufile.c", "src/numath/nurand.c", "src/numath/nuvec.c",
                   "src/numath/numtx.c"]


def parse_variant(text: str) -> dict:
    name, _, rest = text.partition(":")
    spec = {"name": name, "cc1": None, "defines": None, "flag": [], "cpp": []}
    for item in filter(None, rest.split(";")):
        key, _, value = item.partition("=")
        if key in ("flag", "cpp"):
            spec[key].append(value)
        elif key in ("cc1", "defines"):
            spec[key] = value
        else:
            raise SystemExit(f"unknown variant key {key!r} in {text!r}")
    if spec["defines"] not in (None, "ngccc"):
        raise SystemExit(f"defines must be ngccc in {text!r}")
    return spec


def compile_unit(spec: dict, source: Path, out: Path) -> str:
    cmd = [sys.executable, "tools/prodg_cc.py", "--compiler", str(PRODG),
           "--as-bin", str(BUILD / "binutils" / "powerpc-eabi-as"),
           "--dtk", str(BUILD / "tools" / "dtk_patched"),
           "--wibo", str(BUILD / "tools" / "wibo")]
    if spec["cc1"]:
        cmd += ["--cc1-native", str(Path(spec["cc1"]).resolve())]
    if spec["defines"]:
        cmd += ["--ngccc-defines"]
    cmd += [*BASE_FLAGS, *spec["cpp"], *spec["flag"], "-c", str(source), "-o", str(out)]
    out.parent.mkdir(parents=True, exist_ok=True)
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    return "" if r.returncode == 0 else (r.stdout + r.stderr).strip()[-800:] or "failed"


def functions(obj: Path) -> list[str]:
    nm = BUILD / "binutils" / "powerpc-eabi-nm"
    out = subprocess.run([str(nm), "--defined-only", str(obj)], capture_output=True, text=True).stdout
    return [line.split()[2] for line in out.splitlines() if len(line.split()) == 3 and line.split()[1] == "T"]


def verify(obj: Path, name: str) -> dict:
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf), contextlib.redirect_stderr(io.StringIO()):
        vf.main([name, "--object", str(obj), "--strict-symbols", "--json"])
    rep = json.loads(buf.getvalue())
    if rep.get("verdict") == "ERROR":
        return {"verdict": "ERROR", "error": rep.get("error")}
    return {"verdict": rep["verdict"], "score": score(rep), "exact": rep["exact_insns"],
            "target": rep["target_insns"], "mine": rep["mine_insns"]}


def sha(path: Path) -> str:
    """Hash the object without .comment, which names the compiler build and always differs."""
    stripped = path.with_suffix(".nocomment.o")
    subprocess.run([str(BUILD / "binutils" / "powerpc-eabi-objcopy"), "-R", ".comment",
                    str(path), str(stripped)], check=True)
    return hashlib.sha256(stripped.read_bytes()).hexdigest()[:12]


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, required=True, help="new directory for variant objects")
    ap.add_argument("--variant", action="append", default=[], type=parse_variant)
    ap.add_argument("--source", action="append", type=Path, help="C source (default: ProDG units)")
    ap.add_argument("--json", type=Path, help="write full results here")
    args = ap.parse_args(argv)
    args.out.mkdir(parents=True, exist_ok=False)
    sources = args.source or [Path(s) for s in DEFAULT_SOURCES]
    variants = [{"name": "baseline", "cc1": None, "defines": None, "flag": [], "cpp": []}, *args.variant]
    results: dict = {}
    for spec in variants:
        for source in sources:
            obj = (args.out / spec["name"] / source.with_suffix(".o")).resolve()
            err = compile_unit(spec, source, obj)
            entry = {"error": err} if err else {"object": sha(obj), "functions": {}}
            if not err:
                for fn in functions(obj):
                    entry["functions"][fn] = verify(obj, fn)
            results.setdefault(str(source), {})[spec["name"]] = entry
    for source, per in results.items():
        base = per["baseline"]
        print(f"\n{source}")
        for name, entry in per.items():
            if "error" in entry:
                print(f"  {name:<14} COMPILE ERROR {entry['error'].splitlines()[-1] if entry['error'] else ''}")
                continue
            same = "identical" if entry["object"] == base.get("object") else "differs"
            fns = entry["functions"]
            matched = sum(1 for f in fns.values() if f["verdict"].startswith("MATCH"))
            print(f"  {name:<14} object {entry['object']} ({same} from baseline, ignoring .comment)  strict MATCH {matched}/{len(fns)}")
            if name == "baseline" or "functions" not in base:
                continue
            for fn, res in fns.items():
                ref = base["functions"].get(fn)
                if ref != res:
                    print(f"      {fn}: {ref and ref.get('verdict')} {ref and ref.get('exact')}/"
                          f"{ref and ref.get('target')} -> {res['verdict']} {res.get('exact')}/{res.get('target')}")
    if args.json:
        args.json.write_text(json.dumps(results, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
