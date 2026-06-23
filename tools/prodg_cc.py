#!/usr/bin/env python3
"""Compile one C file with the SN Systems ProDG (GCC) toolchain.

LSW1 GC was built with ProDG, not Metrowerks: its codegen (high-register
allocation, `lis/ori` constants, record-form `mr.` null checks) is reproduced
exactly by ProDG's `cc1`, and by no available MWCC version. ProDG ships as the
classic split GCC pipeline rather than a single compile-to-object binary, so
this wrapper drives the four stages and presents a single mwcc-like interface to
the ninja `prodg` rule:

    CPP  (preprocess, + -M depfile)  ->  cc1 (.i -> GNU .s)
      ->  powerpc-eabi-as (.s -> .o) ->  dtk elf fixup (.o)

Preprocessor flags (-I/-D/-U/-include) are routed to CPP; codegen flags
(-O*, -f*, -m*, -g) to cc1. Everything else is ignored so mwcc-style flags can
be passed harmlessly.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

# Routing prefixes
CPP_FLAG_PREFIXES = ("-I", "-D", "-U", "-include", "-nostdinc", "-i")
CC1_FLAG_PREFIXES = ("-O", "-f", "-m", "-g", "-W", "-w", "-std", "-ansi", "-pedantic")


def split_flags(flags: list[str]) -> tuple[list[str], list[str], str, str]:
    """Walk the compiler args, routing to (cpp, cc1) and extracting source/-o.

    Handles both `-I dir` (two tokens) and `-Idir` (one token) forms. Unknown
    flags are dropped so mwcc-only flags can be passed harmlessly.
    """
    cpp: list[str] = []
    cc1: list[str] = []
    source = ""
    output = ""
    i = 0
    while i < len(flags):
        f = flags[i]
        if f == "-o" and i + 1 < len(flags):
            output = flags[i + 1]
            i += 2
            continue
        if f == "-c":
            i += 1
            continue
        if f in ("-I", "-D", "-U", "-include") and i + 1 < len(flags):
            cpp += [f, flags[i + 1]]
            i += 2
            continue
        if f.startswith(CPP_FLAG_PREFIXES):
            cpp.append(f)
        elif f.startswith(CC1_FLAG_PREFIXES):
            cc1.append(f)
        elif not f.startswith("-"):
            source = f  # the input file
        # else: silently ignore (e.g. mwcc-only flags)
        i += 1
    return cpp, cc1, source, output


def run(cmd: list[str], what: str) -> None:
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0 or (proc.stderr and "rror" in proc.stderr):
        sys.stderr.write(f"prodg_cc: {what} failed\n")
        sys.stderr.write(" ".join(str(c) for c in cmd) + "\n")
        sys.stderr.write(proc.stdout + proc.stderr)
        if proc.returncode != 0:
            sys.exit(proc.returncode)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--compiler", required=True, type=Path, help="ProDG version dir (CPP.exe, cc1.exe)")
    ap.add_argument("--as-bin", required=True, type=Path, help="powerpc-eabi-as")
    ap.add_argument("--dtk", required=True, type=Path)
    ap.add_argument("--wibo", type=Path, help="wibo/wine wrapper for the .exe stages")
    ap.add_argument("--asflags", default="-mgekko")
    ap.add_argument("--dep", type=Path, help="depfile to write (Make format)")
    args, passthrough = ap.parse_known_args()

    cpp_flags, cc1_flags, source_str, output_str = split_flags(passthrough)
    if not source_str:
        sys.exit("prodg_cc: no input source file found in args")
    if not output_str:
        sys.exit("prodg_cc: no -o output given")
    args.source = Path(source_str)
    out = Path(output_str)
    wrap = [str(args.wibo)] if args.wibo else []
    cpp = wrap + [str(args.compiler / "CPP.exe")]
    cc1 = wrap + [str(args.compiler / "cc1.exe")]
    out.parent.mkdir(parents=True, exist_ok=True)
    base = out.with_suffix("")
    pre = base.with_suffix(".i")
    asm = base.with_suffix(".prodg.s")

    # 1. Dependencies (best-effort; ninja tolerates a missing depfile target line)
    if args.dep:
        dep = subprocess.run(cpp + ["-M"] + cpp_flags + [str(args.source)],
                             capture_output=True, text=True)
        # Rewrite the target to the real object path for ninja.
        body = dep.stdout.split(":", 1)[1] if ":" in dep.stdout else dep.stdout
        args.dep.parent.mkdir(parents=True, exist_ok=True)
        args.dep.write_text(f"{out}:{body}")

    # 2. Preprocess -> .i
    run(cpp + cpp_flags + [str(args.source), str(pre)], "preprocess (CPP)")
    # 3. Compile -> GNU .s
    run(cc1 + ["-quiet"] + cc1_flags + [str(pre), "-o", str(asm)], "compile (cc1)")
    # 4. Assemble -> .o
    run([str(args.as_bin)] + args.asflags.split() + ["-o", str(out), str(asm)], "assemble")
    # 5. dtk fixup (normalize ELF for the dtk link/diff flow)
    run([str(args.dtk), "elf", "fixup", str(out), str(out)], "elf fixup")


if __name__ == "__main__":
    main()
