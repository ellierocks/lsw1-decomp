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
hillclimb.py), exact instructions and the first difference. `--diff` adds the
side-by-side TARGET/MINE listing (mismatched rows marked `!!`). `--body FILE`
scores FILE in place of NAME's definition without editing SOURCE (`--apply`
writes it back on a strict MATCH), so one unit source can host many trials.
When SOURCE has no definition of NAME yet, the body is appended instead, so a
new function never needs a placeholder or a cleanup pass after a plateau:

    python3 tools/score_fn.py fn_X src/auto/u.c --body /tmp/try.c --diff --apply

During a timed sprint (`work/sprint_timing.json` active) the result line ends
with the measured active seconds (idle gaps over 15 min are not counted). With a sprint
baseline, any scored function is added to the saved queue if absent, unless
`--no-queue` is passed for an automatic scan. A mismatch at 95% exact or better also prints
NEAR MISS: those are finished with targeted changes, not parked on attempt count.

`--json` prints
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


MW_VERSION = "1.2.5n"
MW_FLAGS = SDK_FLAGS


def missing_toolchain(compiler: str) -> list[Path]:
    needed = [BUILD / "tools" / "wibo", BUILD / "tools" / "dtk_patched"]
    if compiler == "prodg":
        needed += [BUILD / "compilers" / PRODG_VERSION / "cc1.exe", BUILD / "binutils" / "powerpc-eabi-as"]
    else:
        needed += [BUILD / "compilers" / "GC" / MW_VERSION / "mwcceppc.exe"]
    return [p for p in needed if not p.exists()]


def run(cmd: list[str]) -> str:
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if r.returncode == 0:
        return ""
    out = (r.stdout + r.stderr).strip()
    # A unit's warnings can bury the error past the tail kept here; report errors first.
    errors = [ln for ln in out.splitlines() if ": warning:" not in ln and "In function" not in ln]
    return ("\n".join(errors) or out)[-1500:] or "failed"


def compile_source(source: Path, out: Path, compiler: str, extra: list[str],
                   diagnostics: Path | None = None, cc1_native: Path | None = None) -> str:
    if compiler == "prodg":
        diagnostic_args = ["--diagnostics-dir", str(diagnostics)] if diagnostics else []
        if cc1_native:
            diagnostic_args += ["--cc1-native", str(cc1_native)]
        return run([sys.executable, "tools/prodg_cc.py",
                    "--compiler", str(BUILD / "compilers" / PRODG_VERSION),
                    "--as-bin", str(BUILD / "binutils" / "powerpc-eabi-as"),
                    "--dtk", str(BUILD / "tools" / "dtk_patched"),
                    "--wibo", str(BUILD / "tools" / "wibo"),
                    *diagnostic_args, *PRODG_FLAGS, *extra, "-c", str(source), "-o", str(out)])
    raw = out.with_suffix(".raw.o")
    inc = []
    for f in extra:
        inc.append("-i" if f == "-I" else f.replace("-I", "-i", 1) if f.startswith("-I") else f)
    err = run([str(BUILD / "tools" / "wibo"), str(BUILD / "compilers" / "GC" / MW_VERSION / "mwcceppc.exe"),
               *MW_FLAGS, "-I-", *inc, "-c", str(source), "-o", str(raw)])
    return err or run([str(BUILD / "tools" / "dtk_patched"), "elf", "fixup", str(raw), str(out)])


def timer_suffix(timing: Path = ROOT / "work" / "sprint_timing.json") -> str:
    """Measured active seconds of a running sprint timer, or nothing when none is running.

    A score run is an activity event: the gap since the previous event counts toward active
    time only up to the idle cap (15 min; $LSW1_IDLE_CAP or `sprint_finish.py --idle-cap`).
    """
    from sprint_metrics import active_seconds, record_activity
    state = record_activity(timing)
    if state is None:
        return ""
    return f"  [active {active_seconds(state):.0f}s]"


def note_queue(name: str, baseline: Path = ROOT / "work" / "sprint_baseline.json") -> None:
    """Append a scored function to the saved sprint queue (no-op without a sprint baseline)."""
    from sprint_metrics import queue_add
    if queue_add(baseline, [name]):
        print(f"queue: added {name} to the sprint queue", file=sys.stderr)


def splice_body(text: str, name: str, body: str) -> str:
    from variant_search import VariantError, find_function
    body = body.strip("\n")
    try:
        fn = find_function(text, name)
        return fn.before + body + fn.tail[1:]
    except VariantError:
        return text.rstrip("\n") + "\n\n" + body + "\n"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="retail function name in symbols.txt")
    ap.add_argument("source", type=Path, help="C file to compile")
    ap.add_argument("--compiled-symbol", help="symbol emitted by the source if it differs")
    ap.add_argument("--compiler", choices=("prodg", "sdk"), default="prodg")
    ap.add_argument("-I", dest="includes", action="append", default=[], help="extra include dir")
    ap.add_argument("-D", dest="defines", action="append", default=[], help="extra define")
    ap.add_argument("--mw-version", default="1.2.5n", metavar="VER",
                    help="with --compiler sdk: directory under build/compilers/GC (default 1.2.5n)")
    ap.add_argument("--mw-flags", metavar="FLAGS",
                    help="with --compiler sdk: replace the dolsdk2004 release flags (shell-quoted string)")
    ap.add_argument("--compile-only", action="store_true", help="only compile (needs -o)")
    ap.add_argument("-o", type=Path, help="keep the object here")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--no-queue", action="store_true",
                    help="score an automatic scan without adding to the selected sprint queue")
    ap.add_argument("--body", type=Path, metavar="FILE",
                    help="score FILE (a full definition of NAME, plus any new externs above it) in place of "
                         "NAME's definition in SOURCE; SOURCE itself is not edited unless --apply")
    ap.add_argument("--apply", action="store_true",
                    help="with --body: write the replacement into SOURCE when the result is a strict MATCH")
    ap.add_argument("--diff", action="store_true",
                    help="also print the verifier's side-by-side listing (mismatched rows plus context)")
    ap.add_argument("--diagnostics-dir", type=Path, help="new directory for ProDG RTL dumps and logs")
    ap.add_argument("--cc1-native", type=Path,
                    help="evaluate a source-built cc1 (tools/sn-gcc/build.sh); with GDBG=1/LADBG=1 and "
                         "a diagnostics build its allocation trace lands in compiler.log")
    args = ap.parse_args(argv)
    global MW_VERSION, MW_FLAGS
    MW_VERSION = args.mw_version
    import shlex
    MW_FLAGS = shlex.split(args.mw_flags) if args.mw_flags else SDK_FLAGS
    extra = [x for d in args.includes for x in ("-I", d)] + [f"-D{d}" for d in args.defines]
    if args.compile_only and not args.o:
        ap.error("--compile-only needs -o")
    if (args.diagnostics_dir or args.cc1_native) and args.compiler != "prodg":
        ap.error("--diagnostics-dir and --cc1-native require --compiler prodg")
    if args.apply and not args.body:
        ap.error("--apply needs --body")
    if args.body and not args.body.is_file():
        ap.error(f"--body file not found: {args.body}")
    missing = missing_toolchain(args.compiler)
    if missing:
        names = ", ".join(str(p) for p in missing)
        print(f"error: toolchain not bootstrapped (missing {names}).\n"
              "Run: bash tools/bootstrap.sh", file=sys.stderr)
        return vf.EXIT_ERROR
    with tempfile.TemporaryDirectory(prefix="score_fn_") as tmp:
        obj = args.o.resolve() if args.o else Path(tmp) / "candidate.o"
        source = args.source.resolve()
        replaced = None
        if args.body:
            replaced = splice_body(source.read_text(), args.compiled_symbol or args.name,
                                   args.body.read_text())
            source = Path(tmp) / source.name
            source.write_text(replaced)
        err = compile_source(source, obj, args.compiler, extra,
                             args.diagnostics_dir.resolve() if args.diagnostics_dir else None,
                             args.cc1_native.resolve() if args.cc1_native else None)
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
        listing = ""
        if args.diff and not args.json:
            lbuf = io.StringIO()
            with contextlib.redirect_stdout(lbuf), contextlib.redirect_stderr(io.StringIO()):
                vf.main([a for a in vargs if a != "--json"])
            # Keep the TARGET/MINE rows; the summary line below replaces the hex tail.
            lines = lbuf.getvalue().splitlines()
            keep = [ln for ln in lines if ln.startswith(("   ", "!!"))]
            listing = "\n".join(keep)
    rep = json.loads(buf.getvalue())
    if rep.get("verdict") == "ERROR":
        print(f"error: {rep.get('error')}", file=sys.stderr)
        return code
    rep["score"] = score(rep)
    if not args.no_queue:
        note_queue(args.name)
    if args.json:
        print(json.dumps(rep, indent=2))
        return code
    first = rep["differences"][0] if rep["differences"] else None
    detail = f"  first: {first['kind']} @{first.get('index_t')}: {first.get('detail', '')}" if first else ""
    print(f"{rep['verdict']}  score={rep['score']}  exact={rep['exact_insns']}/{rep['target_insns']}"
          f"  mine_insns={rep['mine_insns']}{detail}{timer_suffix()}")
    from sprint_metrics import near_miss
    if near_miss(rep["exact_insns"], rep["target_insns"]):
        print("NEAR MISS: do not park on attempt count; target each remaining diff, then "
              "variant_search.py once (docs/sprint_workflow.md#per-function-budget)")
    if listing and code != 0:
        print(listing)
    if args.apply and code == 0:
        args.source.write_text(replaced)
        print(f"applied to {args.source}")
    return code


if __name__ == "__main__":
    sys.exit(main())
