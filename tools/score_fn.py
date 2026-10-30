#!/usr/bin/env python3
"""Fast compile-and-score for one function: the inner loop of agent search.

Compiles a single C file straight to a temporary object (no configure, no
ninja, no ledger) and scores one function with the strict verifier. Use it
for many cheap attempts; use `hillclimb.py try` to record a candidate worth
keeping and `verify_fn.py --build --strict-symbols` for acceptance.

    python3 tools/score_fn.py NuVecScale src/numath/nuvec.c
    python3 tools/score_fn.py fn_80181374 scratch.c --compiled-symbol GXInitTexObj --compiler sdk \\
        -I ../dolsdk2004/include
    python3 tools/score_fn.py NuVecScale scratch.c --compile-only -o out.o   # permuter compile step

Output is one line: verdict, score (lower is better, same order as
hillclimb.py), exact instructions and the first difference. `--json` prints
the full verifier report with the score added. Exit codes follow verify_fn.py:
0 match, 1 mismatch, 2 error, 3 code match with unverified references.

Compilers:
    prodg  ProDG 3.9.3 with configure.py's cflags_prodg (game code, default)
    sdk    MWCC GC/1.2.5n with dolsdk2004 release flags (Dolphin SDK units)
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
from hillclimb import score  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
PRODG_VERSION = "ProDG/3.9.3"
PRODG_FLAGS = ["-O2", "-I", "src", "-I", "include", "-I", "build/GL5E4F/include",
               "-DBUILD_VERSION=0", "-DVERSION_GL5E4F", "-DNDEBUG=1"]
SDK_FLAGS = ["-nodefaults", "-proc", "gekko", "-fp", "hard", "-Cpp_exceptions", "off",
             "-enum", "int", "-char", "unsigned", "-warn", "pragmas", "-requireprotos",
             "-pragma", "cats off", "-O4,p", "-inline", "auto", "-D__GEKKO__",
             "-DSDK_REVISION=1", "-DRELEASE"]


def run(cmd: list[str]) -> str:
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    return "" if r.returncode == 0 else (r.stdout + r.stderr).strip()[-1500:] or "failed"


def compile_source(source: Path, out: Path, compiler: str, extra: list[str]) -> str:
    if compiler == "prodg":
        return run([sys.executable, "tools/prodg_cc.py",
                    "--compiler", str(BUILD / "compilers" / PRODG_VERSION),
                    "--as-bin", str(BUILD / "binutils" / "powerpc-eabi-as"),
                    "--dtk", str(BUILD / "tools" / "dtk_patched"),
                    "--wibo", str(BUILD / "tools" / "wibo"),
                    *PRODG_FLAGS, *extra, "-c", str(source), "-o", str(out)])
    raw = out.with_suffix(".raw.o")
    inc = []
    for f in extra:
        inc.append("-i" if f == "-I" else f.replace("-I", "-i", 1) if f.startswith("-I") else f)
    err = run([str(BUILD / "tools" / "wibo"), str(BUILD / "compilers" / "GC" / "1.2.5n" / "mwcceppc.exe"),
               *SDK_FLAGS, "-I-", *inc, "-c", str(source), "-o", str(raw)])
    return err or run([str(BUILD / "tools" / "dtk_patched"), "elf", "fixup", str(raw), str(out)])


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="retail function name in symbols.txt")
    ap.add_argument("source", type=Path, help="C file to compile")
    ap.add_argument("--compiled-symbol", help="symbol emitted by the source if it differs")
    ap.add_argument("--compiler", choices=("prodg", "sdk"), default="prodg")
    ap.add_argument("-I", dest="includes", action="append", default=[], help="extra include dir")
    ap.add_argument("-D", dest="defines", action="append", default=[], help="extra define")
    ap.add_argument("--compile-only", action="store_true", help="only compile (needs -o)")
    ap.add_argument("-o", type=Path, help="keep the object here")
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args(argv)
    extra = [x for d in args.includes for x in ("-I", d)] + [f"-D{d}" for d in args.defines]
    if args.compile_only and not args.o:
        ap.error("--compile-only needs -o")
    with tempfile.TemporaryDirectory(prefix="score_fn_") as tmp:
        obj = args.o.resolve() if args.o else Path(tmp) / "candidate.o"
        err = compile_source(args.source.resolve(), obj, args.compiler, extra)
        if err:
            print(f"error: compile failed\n{err}", file=sys.stderr)
            return vf.EXIT_ERROR
        if args.compile_only:
            return 0
        buf = io.StringIO()
        vargs = [args.name, "--object", str(obj), "--strict-symbols", "--json"]
        if args.compiled_symbol:
            vargs += ["--compiled-symbol", args.compiled_symbol]
        with contextlib.redirect_stdout(buf), contextlib.redirect_stderr(io.StringIO()):
            code = vf.main(vargs)
    rep = json.loads(buf.getvalue())
    if rep.get("verdict") == "ERROR":
        print(f"error: {rep.get('error')}", file=sys.stderr)
        return code
    rep["score"] = score(rep)
    if args.json:
        print(json.dumps(rep, indent=2))
        return code
    first = rep["differences"][0] if rep["differences"] else None
    detail = f"  first: {first['kind']} @{first.get('index_t')}: {first.get('detail', '')}" if first else ""
    print(f"{rep['verdict']}  score={rep['score']}  exact={rep['exact_insns']}/{rep['target_insns']}"
          f"  mine_insns={rep['mine_insns']}{detail}")
    return code


if __name__ == "__main__":
    sys.exit(main())
