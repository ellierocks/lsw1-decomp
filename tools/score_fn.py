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
new function never needs a placeholder or a cleanup pass after a plateau. Top-level
`extern` declarations and `struct NAME { ... };` definitions in the body that SOURCE already
declares are dropped (SOURCE's version wins), so a draft that carries m2c's guessed prototypes
does not fail on a conflicting redeclaration:

    python3 tools/score_fn.py fn_X src/auto/u.c --body /tmp/try.c --diff --apply

During a timed sprint (`work/sprint_timing.json` active) the result line ends
with the measured active seconds (idle gaps over 15 min are not counted). With a sprint
baseline, any scored function is added to the saved queue if absent, unless
`--no-queue` is passed for an automatic scan. A mismatch at 95% exact or better also prints
NEAR MISS with a pointer to common near-miss causes.

`--json` prints
the full verifier report with the score added. Exit codes follow verify_fn.py:
0 match, 1 mismatch, 2 error, 3 code match with unverified references.

Compilers:
    prodg  ProDG 3.9.3 with configure.py's cflags_prodg (game code, default)
    sn     ProDG 3.9.3 with configure.py's cflags_sn_libc (SN runtime, src/sn; the default
           for sources under src/sn/)
    sdk    MWCC GC/1.2.5n with dolsdk2004 release flags (Dolphin SDK units)
"""

from __future__ import annotations

import argparse
import contextlib
import io
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import prodg_cc  # noqa: E402
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

# configure.py's cflags_sn_libc (newlib/libgcc under src/sn): -fno-common, -G 1024 and the
# ngccc target defines change code generation, and the game include dirs would shadow
# newlib's headers. tests/test_score_fn_sn.py keeps this in step with configure.py.
SN_FLAGS = ["-O2", "-mfast-cast", "-fno-common", "-G", "8", "-I", "src/sn/include", "-DNDEBUG=1", "-DMB_CAPABLE",
            "-D__GNUC__=2", "-D__GNUC_MINOR__=95", "-DPPC", "-D__PPC__", "-D__PPC",
            "-D__CHAR_UNSIGNED__", "-D_BIG_ENDIAN", "-D__BIG_ENDIAN__", "-D__PTRDIFF_TYPE__=int",
            "-D__SIZE_TYPE__=unsigned", "-D_CALL_SYSV", "-D__SN__", "-DSN_TARGET_NGC",
            "-D__NOASMPP__", "-D_LANGUAGE_C", "-D__LANGUAGE_C", "-DLANGUAGE_C"]
SN_SOURCE_DIR = ROOT / "src" / "sn"
SN_UNIT_FLAGS: list[str] | None = None  # main(): the scored unit's build.ninja cflags


def default_compiler(source: Path) -> str:
    """`sn` for a source under src/sn/, else `prodg`."""
    return "sn" if source.resolve().is_relative_to(SN_SOURCE_DIR) else "prodg"


MW_VERSION = "1.2.5n"
MW_FLAGS = SDK_FLAGS


def missing_toolchain(compiler: str) -> list[Path]:
    needed = [BUILD / "tools" / "wibo", BUILD / "tools" / "dtk_patched"]
    if compiler in ("prodg", "sn"):
        needed += [BUILD / "compilers" / PRODG_VERSION / "cc1.exe", BUILD / "binutils" / "powerpc-eabi-as"]
    else:
        needed += [BUILD / "compilers" / "GC" / MW_VERSION / "mwcceppc.exe"]
    return [p for p in needed if not p.exists()]


def unit_prodg_flags(source: Path, ninja: Path | None = None) -> list[str] | None:
    """cflags of SOURCE's ProDG build edge in build.ninja, or None.

    SN runtime units add per-library flags to cflags_sn_libc (libsn's `-G 0`, libm's
    `-msafe-sda`), and `-G` alone moves every global between sda21 and lis/addi
    addressing, so `--compiler sn` scores with the unit's own flags when it has an edge.
    """
    import shlex
    ninja = ninja or ROOT / "build.ninja"
    try:
        rel = source.resolve().relative_to(ROOT).as_posix()
        lines = ninja.read_text().replace("$\n", "").splitlines()
    except (OSError, ValueError):
        return None
    for i, line in enumerate(lines):
        if not (line.startswith("build ") and ": prodg " in line and f" {rel} " in line + " "):
            continue
        for var in lines[i + 1:]:
            if not var.startswith("  "):
                break
            key, _, value = var.strip().partition(" = ")
            if key == "cflags":
                return shlex.split(value)
    return None


def unit_mw_settings(source: Path, ninja: Path | None = None) -> tuple[str, list[str]] | None:
    """(GC version, cflags) of SOURCE's mwcc build edge in build.ninja, or None.

    A unit can pin its own compiler (configure.py's per-object `mw_version`, e.g. HVQM4 on
    GC/1.2.5), and its cflags carry the include dirs and `-lang=c`, so scoring an SDK unit
    source with these settings reproduces the build instead of the generic SDK defaults.
    """
    import shlex
    ninja = ninja or ROOT / "build.ninja"
    try:
        rel = source.resolve().relative_to(ROOT).as_posix()
        lines = ninja.read_text().replace("$\n", "").splitlines()
    except (OSError, ValueError):
        return None
    for i, line in enumerate(lines):
        if not (line.startswith("build ") and ": mwcc " in line and f" {rel} " in line + " "):
            continue
        version = flags = None
        for var in lines[i + 1:]:
            if not var.startswith("  "):
                break
            key, _, value = var.strip().partition(" = ")
            if key == "mw_version":
                version = value.strip().removeprefix("GC/")
            elif key == "cflags":
                flags = shlex.split(value)
        if version and flags:
            return version, flags
    return None


def run(cmd: list[str]) -> str:
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if r.returncode == 0:
        return ""
    return error_tail((r.stdout + r.stderr).strip())


# The random temporary directory in compiler paths (`Z:/tmp/score_fn_x1y2/u.c:5:`).
TMP_DIR = re.compile(r"(?:\b[A-Za-z]:)?[^\s:'\"`]*?score_fn_[A-Za-z0-9_]+[/\\]")


def error_tail(out: str) -> str:
    # A unit's warnings can bury the error past the tail kept here; report errors first.
    # The temporary directory goes before the cut, which could otherwise split its name.
    out = TMP_DIR.sub("", out)
    errors = [ln for ln in out.splitlines() if ": warning:" not in ln and "In function" not in ln]
    text = "\n".join(errors) or out
    tail = text[-1500:]
    if len(text) > 1500 and "\n" in tail:  # whole lines only: no `cn.c:374` fragments
        tail = tail[tail.index("\n") + 1:]
    return tail or "failed"


def compile_source(source: Path, out: Path, compiler: str, extra: list[str],
                   diagnostics: Path | None = None, cc1_native: Path | None = None) -> str:
    if compiler in ("prodg", "sn"):
        diagnostic_args = ["--diagnostics-dir", str(diagnostics)] if diagnostics else []
        if cc1_native:
            diagnostic_args += ["--cc1-native", str(cc1_native)]
        try:  # in-process: one interpreter start less per score (prodg_cc is thread-safe)
            prodg_cc.compile_c([
                "--compiler", str(BUILD / "compilers" / PRODG_VERSION),
                "--as-bin", str(BUILD / "binutils" / "powerpc-eabi-as"),
                "--dtk", str(BUILD / "tools" / "dtk_patched"),
                "--wibo", str(BUILD / "tools" / "wibo"),
                *diagnostic_args, *(SN_UNIT_FLAGS or SN_FLAGS if compiler == "sn" else PRODG_FLAGS), *extra, "-c", str(source), "-o", str(out)], cwd=ROOT)
        except prodg_cc.CompileError as e:
            return error_tail(str(e).strip())
        return ""
    raw = out.with_suffix(".raw.o")
    inc = []
    for f in extra:
        inc.append("-i" if f == "-I" else f.replace("-I", "-i", 1) if f.startswith("-I") else f)
    err = run([str(BUILD / "tools" / "wibo"), str(BUILD / "compilers" / "GC" / MW_VERSION / "mwcceppc.exe"),
               *MW_FLAGS, *([] if "-I-" in MW_FLAGS else ["-I-"]), *inc, "-c", str(source), "-o", str(raw)])
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


_STRUCT_DEF = re.compile(r"^struct\s+(\w+)\s*\{(?:[^{}]|\{[^{}]*\})*\};[ \t]*\n?", re.M)
# `extern T (*name)(...)` names a function pointer; otherwise the first identifier followed by
# `[`, `;` or a parameter list (not `(*`) is the declared name.
_EXTERN = re.compile(r"^extern\b[^;{]*?(?:\(\s*\*\s*(\w+)\s*\)[^;{]*;|\b(\w+)\s*(?:;|(?:\[|\((?!\s*\*))[^;{]*;))"
                     r"[ \t]*\n?", re.M)


def _declares(text: str, ident: str) -> bool:
    """True when an unindented line of `text` declares or defines `ident` at file scope."""
    pat = re.compile(rf"^(?![ \t#/*]).*\b{re.escape(ident)}\s*[\[(;)]", re.M)
    return bool(pat.search(text))


def drop_redeclared(context: str, body: str) -> tuple[str, list[str]]:
    """Remove body-level externs and struct definitions that `context` already declares."""
    dropped: list[str] = []

    def struct(m):
        if re.search(rf"^struct\s+{m.group(1)}\s*\{{", context, re.M):
            dropped.append(f"struct {m.group(1)}")
            return ""
        return m.group(0)

    def extern(m):
        ident = m.group(1) or m.group(2)
        if _declares(context, ident):
            dropped.append(ident)
            return ""
        return m.group(0)

    body = _STRUCT_DEF.sub(struct, body)
    return _EXTERN.sub(extern, body), dropped


def bench_note(name: str, verdict: str, exact: int = 0, target: int = 0) -> None:
    """Record an interactive attempt for tools/sprint_bench.py (no-op without a started run)."""
    try:
        import sprint_bench
        sprint_bench.append("score", name=name, verdict=verdict, exact=exact, target=target)
    except (OSError, ValueError):
        pass


def function_extern(text: str, name: str, body: str) -> str:
    """Turn SOURCE's data-style `extern T name[];` into a declaration of the function `body` defines.

    Units reference callbacks they have not decompiled yet (parse tables, function-pointer
    arguments) as `extern s32 fn_X[];`; defining fn_X then fails as "redeclared as different
    kind of symbol". Either form takes the same `@ha/@l` address, so the codegen is unchanged.
    """
    decl = re.compile(rf"^extern\b[^;(){{}}=]*\b{re.escape(name)}\s*\[\s*\]\s*;", re.M)
    if not decl.search(text):
        return text
    sig = re.search(rf"^([^\n;{{}}#]*?\b{re.escape(name)})\s*\(([^;{{}}]*)\)\s*\{{", body, re.M)
    if not sig:
        return text
    head, params = sig.group(1).strip(), " ".join(sig.group(2).split())
    # A struct tag first named in a parameter list would get prototype scope; leave those
    # unprototyped (still compatible with the later definition).
    proto = f"{head}({params});" if not re.search(r"\b(struct|union|enum)\b", params) else f"{head}();"
    return decl.sub(lambda m: proto, text, count=1)


def splice_body(text: str, name: str, body: str) -> str:
    from variant_search import VariantError, find_function
    body = body.strip("\n")
    text = function_extern(text, name, body)
    try:
        fn = find_function(text, name)
        context = fn.before + fn.tail[1:]
    except VariantError:
        fn, context = None, text
    body, dropped = drop_redeclared(context, body)
    if dropped:
        print(f"note: dropped body declarations SOURCE already has: {', '.join(dropped)}", file=sys.stderr)
    body = body.strip("\n")
    if fn is not None:
        return fn.before + body + fn.tail[1:]
    return text.rstrip("\n") + "\n\n" + body + "\n"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="retail function name in symbols.txt")
    ap.add_argument("source", type=Path, help="C file to compile")
    ap.add_argument("--compiled-symbol", help="symbol emitted by the source if it differs")
    ap.add_argument("--compiler", choices=("prodg", "sn", "sdk"),
                    help="default: sdk when SOURCE has an mwcc edge in build.ninja (an SDK or "
                         "middleware unit, e.g. HVQM4), sn for a source under src/sn/, else prodg")
    ap.add_argument("-I", dest="includes", action="append", default=[], help="extra include dir")
    ap.add_argument("-D", dest="defines", action="append", default=[], help="extra define")
    ap.add_argument("--mw-version", metavar="VER",
                    help="with --compiler sdk: directory under build/compilers/GC (default: the unit's "
                         "build.ninja mw_version, else 1.2.5n)")
    ap.add_argument("--mw-flags", metavar="FLAGS",
                    help="with --compiler sdk: replace the flags (default: the unit's build.ninja cflags, "
                         "else the dolsdk2004 release flags; shell-quoted string)")
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
    global MW_VERSION, MW_FLAGS, SN_UNIT_FLAGS
    import shlex
    unit = unit_mw_settings(args.source) if args.compiler in (None, "sdk") else None
    if args.compiler is None:  # sweep.py and other callers pass no --compiler for SDK units
        args.compiler = "sdk" if unit else default_compiler(args.source)
    MW_VERSION = args.mw_version or (unit[0] if unit else "1.2.5n")
    MW_FLAGS = shlex.split(args.mw_flags) if args.mw_flags else (unit[1] if unit else SDK_FLAGS)
    SN_UNIT_FLAGS = unit_prodg_flags(args.source) if args.compiler == "sn" else None
    extra = [x for d in args.includes for x in ("-I", d)] + [f"-D{d}" for d in args.defines]
    if args.compile_only and not args.o:
        ap.error("--compile-only needs -o")
    if (args.diagnostics_dir or args.cc1_native) and args.compiler == "sdk":
        ap.error("--diagnostics-dir and --cc1-native require --compiler prodg or sn")
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
            if not args.no_queue and not args.compile_only:
                bench_note(args.name, "ERROR")
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
        bench_note(args.name, rep.get("verdict", ""), rep.get("exact_insns", 0), rep.get("target_insns", 0))
    if args.json:
        print(json.dumps(rep, indent=2))
        return code
    first = rep["differences"][0] if rep["differences"] else None
    detail = f"  first: {first['kind']} @{first.get('index_t')}: {first.get('detail', '')}" if first else ""
    print(f"{rep['verdict']}  score={rep['score']}  exact={rep['exact_insns']}/{rep['target_insns']}"
          f"  mine_insns={rep['mine_insns']}{detail}{timer_suffix()}")
    from sprint_metrics import near_miss
    if near_miss(rep["exact_insns"], rep["target_insns"]):
        print("NEAR MISS: common causes and search tools in "
              "docs/sprint_workflow.md#per-function-search")
    if listing and code != 0:
        print(listing)
    if args.apply and code == 0:
        args.source.write_text(replaced)
        print(f"applied to {args.source}")
    return code


if __name__ == "__main__":
    sys.exit(main())
