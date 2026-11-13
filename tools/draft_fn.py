#!/usr/bin/env python3
"""Write an m2c first draft for one function, ready for score_fn.py and variant_search.py.

    python3 tools/draft_fn.py NuAnimCurve2SetApplyToJointTransLoc
    python3 tools/draft_fn.py NAME --score          # also score the draft once
    python3 tools/draft_fn.py NAME --no-ghidra      # skip work/NAME/ghidra.c
    python3 tools/draft_fn.py A B C --no-ghidra --print   # draft a wave; print drafts and asm

Globals the target addresses with `@ha`/`@l` instead of `@sda21` are declared as arrays
and read as `sym[0]`: ProDG puts scalar externs of 8 bytes or less in small data, so the
scalar declaration m2c writes can never match those accesses.

The draft goes to `work/NAME/draft_m2c.c` (ignored by git). m2c reads the function's unit
source, preprocessed, as `--context` (as `sweep.py` does), so declared prototypes, parameter
types and struct fields carry into the draft; `draft_m2c_plain.c` is the context-free draft for
comparison, and `--no-context` makes it the only one. m2c runs from the pinned checkout
(`tools/fetch_m2c.sh`, default ~/.cache/lsw1/m2c; `M2C` overrides it) with the PPC GCC target.
Its output is a starting point only: locals are `var_*`/`temp_*`, and anything the unit does
not declare stays `void *` with raw offsets. Type those from the target assembly, then iterate with
`score_fn.py` and `variant_search.py`. Nothing here counts as a match until
`verify_fn.py --build --strict-symbols` passes on the unit source.

When the Ghidra project exists (`tools/ghidra_decompile.py setup`, run by bootstrap), it also
writes `work/NAME/ghidra.c` (named globals, loops, switch recovery) unless that file is
already there; `--ghidra` regenerates it. It is a reference, not a draft.

When the function has a Japanese PC pair (docs/research.md#pc-builds), it also writes
`work/NAME/jp_x86.s` (annotated x86 body, tools/pc_gc_body.py) if missing and, once
`tools/ghidra_decompile.py setup-pc` has run, `work/NAME/jp_ghidra.c`. Both are references
for call order, branches, globals and constants; PC offsets and codegen never transfer.
"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import ghidra_decompile  # noqa: E402
import m2c_helpers as repairs
import pack_asm  # noqa: E402
import pc_gc_body  # noqa: E402
import verify_fn as vf  # noqa: E402
TARGET = "ppc-gcc-c"


def m2c_dir() -> Path:
    return Path(os.environ.get("M2C") or Path(os.environ.get("LSW1_CACHE", Path.home() / ".cache/lsw1")) / "m2c")


def absolute_symbols(asm: str) -> set[str]:
    """Symbols the target reaches through @ha/@l, i.e. outside small data."""
    return set(re.findall(r"\b(\w+)@(?:ha|l)\b", asm))


def arrayize(text: str, symbols: set[str]) -> str:
    """Declare scalar externs of absolutely-addressed symbols as arrays and index them."""
    for sym in sorted(symbols):
        decl = re.compile(rf"^extern ([^;()\[\]]*?)\b{re.escape(sym)};", re.M)
        if not decl.search(text):
            continue
        text = decl.sub(rf"extern \1{sym}[];", text)
        head, sep, body = text.partition(f"{sym}[];")
        # One pass: `&sym` becomes the array itself, a bare read becomes `sym[0]`.
        body = re.sub(rf"(&?)(?<!\w){re.escape(sym)}\b(?!\s*\[)",
                      lambda m: (f"((s8 *){sym})" if m.group(1) and
                                 (re.search(r"[+\-]\s*$", body[:m.start()]) or
                                  re.match(r"\s*[+\-]", body[m.end():]))
                                 else sym) if m.group(1) else f"{sym}[0]", body)
        text = head + sep + body
    return text


def drop_cr_saves(text: str, asm: str) -> str:
    """Remove the stack slot of a CR save (`mfcr rX; stw rX, N(r1)`).

    The patched m2c (tools/patches/m2c-crfields.patch) models mfcr's GPR as 0, so the slot
    would read `spN = 0;`. The compiler emits this save itself whenever cr2-cr4 are used, so
    it has no C source. Only slots with no other use in the draft are dropped.
    """
    mfcr_regs: set[str] = set()
    for line in asm.splitlines():
        if m := re.search(r"\bmfcr (r\d+)\s*$", line):
            mfcr_regs.add(m.group(1))
        elif m := re.search(r"\bstw (r\d+), (0x[0-9a-f]+|\d+)\(r1\)\s*$", line):
            if m.group(1) in mfcr_regs:
                slot = f"sp{int(m.group(2), 0):X}"
                if len(re.findall(rf"\b{slot}\b", text)) == 2 and \
                        re.search(rf"^\s+s32 {slot};\n", text, flags=re.M):
                    text = re.sub(rf"^\s+s32 {slot};\n", "", text, flags=re.M)
                    text = re.sub(rf"^\s+{slot} = 0;\n", "", text, flags=re.M)
        elif m := re.search(r"\b(?:mr|li|lwz|addi|add|rlwinm|extrwi)\s+(r\d+),", line):
            mfcr_regs.discard(m.group(1))
    return text


def clean(text: str, name: str, asm: str = "") -> str:
    """Make m2c output compile as C: no `?` types, no extern comments, a types.h include."""
    text = drop_cr_saves(text, asm)
    text = repairs.repair_indirect_prototypes(text, asm)
    text = re.sub(r"^extern \? (\w+);", r"extern char \1[];", text, flags=re.M)
    text = re.sub(r"^\? (\w+)\(", r"void \1(", text, flags=re.M)
    text = re.sub(r"\s*/\* extern \*/", "", text)
    text = re.sub(r"\(\?(\s*\*?\w*)\)", r"(void\1)", text)
    text = arrayize(text, absolute_symbols(asm))
    if any(fn in text for fn in repairs.PS_CALLS):
        text = re.sub(r"^(?=\S)[^\n]*\b(?:" + "|".join(repairs.PS_CALLS) + r")\([^\n]*\);\n",
                      "", text, flags=re.M)
        text = repairs.fix_ps_calls(text)
        text = '#include "numath/nuvec_ps.h"\n' + text
    return f'#include "types.h"\n\n{text.strip()}\n'


CPP = ["cpp", "-P", "-nostdinc", "-Isrc", "-Iinclude", "-Ibuild/GL5E4F/include", "-D__attribute__(x)="]


def unit_source(name: str) -> Path | None:
    """The C source of NAME's split unit, or None."""
    m = re.search(rf"^{re.escape(name)} = \.text:0x([0-9A-Fa-f]+);", vf.SYMBOLS.read_text(), re.M) \
        if vf.SYMBOLS.is_file() else None
    unit = vf.lookup_unit(int(m.group(1), 16)) if m else None
    source = vf.source_for_unit(unit) if unit else None
    return source if source is not None and source.is_file() else None


def unit_context(name: str, task: Path) -> Path | None:
    """NAME's unit source preprocessed for m2c's `--context` (as sweep.py does), or None.

    Without it m2c guesses every type: float returns and parameters came back as `void *`, struct
    fields as raw offsets, and function-pointer callees as unparseable declarations."""
    source = unit_source(name)
    if source is None:
        return None
    try:
        r = subprocess.run(CPP + [str(source.relative_to(ROOT))], cwd=ROOT, capture_output=True, text=True)
    except OSError:
        return None
    if r.returncode:
        return None
    out = task / "m2c_context.i"
    out.write_text(r.stdout)
    return out


def run_m2c(prepared: Path, context: Path | None) -> str:
    cmd = [sys.executable, str(m2c_dir() / "m2c.py"), "-t", TARGET, "--valid-syntax"]
    if context is not None:
        cmd += ["--context", str(context)]
    r = subprocess.run(cmd + [str(prepared.resolve())], capture_output=True, text=True)
    return "" if r.returncode or not r.stdout.strip() else r.stdout


def compact_asm(asm: str) -> str:
    """Assembly without the address/offset/bytes comment column."""
    return "\n".join(re.sub(r"^/\* [0-9A-F]+ [0-9A-F]+\s+(?:[0-9A-F]{2} ){4}\*/\s*", "\t", ln)
                     for ln in asm.splitlines() if ln.strip() and not ln.startswith((".fn", ".endfn")))


def write_jp_references(name: str, task: Path, args) -> None:
    """jp_x86.s and (with the PC Ghidra project) jp_ghidra.c for a JP-paired function; quiet otherwise."""
    shown = lambda p: p.relative_to(ROOT) if p.is_relative_to(ROOT) else p  # noqa: E731
    if not (task / "jp_x86.s").is_file():
        path, status = pc_gc_body.write_task_body(name, task)
        if path:
            print(f"wrote {shown(path)} ({status})")
        elif "no JP PC pair" not in status:
            print(f"note: {status}", file=sys.stderr)
    jp_ghidra = task / "jp_ghidra.c"
    if (task / "jp_x86.s").is_file() and not args.no_ghidra and ghidra_decompile.pc_ready() \
            and (args.ghidra or not jp_ghidra.is_file()):
        g = subprocess.run([sys.executable, str(ROOT / "tools" / "ghidra_decompile.py"), "decompile-pc", name,
                            "--out-dir", str(task)], cwd=ROOT, capture_output=True, text=True)
        produced = task / f"{name}.c"
        if g.returncode == 0 and produced.is_file():
            produced.replace(jp_ghidra)
            print(f"wrote {shown(jp_ghidra)}")
        else:
            print(f"ghidra (pc): {(g.stdout + g.stderr).strip()[-300:] or 'no output'}", file=sys.stderr)


def draft_one(name: str, args) -> int:
    task = args.out_dir / name
    asm = pack_asm.ensure_asm(name, args.out_dir)
    if not asm.is_file() or not asm.read_text().strip():
        print(f"error: no assembly for {name}; run bash build.sh, then tools/ls1_task_pack.py", file=sys.stderr)
        return 2
    m2c = m2c_dir()
    if not (m2c / "m2c.py").is_file():
        print("error: m2c not fetched; run bash tools/fetch_m2c.sh", file=sys.stderr)
        return 2
    prepared = task / "asm_m2c.s"
    prepared.write_text(repairs.prepare_asm(asm.read_text()))
    asm_text = asm.read_text()
    context = None if args.no_context else unit_context(name, task)
    typed = run_m2c(prepared, context) if context else ""
    plain = run_m2c(prepared, None)
    if not typed and not plain:
        print(f"error: m2c failed for {name}", file=sys.stderr)
        return 2
    out = task / "draft_m2c.c"
    for text, path in ((typed or plain, out), (plain if typed else "", task / "draft_m2c_plain.c")):
        if text:
            path.write_text(clean(repairs.type_derefs(repairs.expand_macros(
                repairs.declare_unset(text, name))), name, asm_text))
            print(f"wrote {path.relative_to(ROOT) if path.is_relative_to(ROOT) else path}"
                  + (" (typed from the unit source)" if path == out and typed else ""))
        elif path.exists():
            path.unlink()
    ghidra_out = task / "ghidra.c"
    if not args.no_ghidra and (args.ghidra or not ghidra_out.is_file()):
        ghidra_decompile.auto_setup()
    want_ghidra = not args.no_ghidra and (args.ghidra or (not ghidra_out.is_file() and ghidra_decompile.project_ready()))
    if want_ghidra:
        g = subprocess.run([sys.executable, str(ROOT / "tools" / "ghidra_decompile.py"), "decompile", name,
                            "--out-dir", str(task)], cwd=ROOT, capture_output=True, text=True)
        produced = task / f"{name}.c"
        if g.returncode == 0 and produced.is_file():
            produced.replace(task / "ghidra.c")
            print(f"wrote {(task / 'ghidra.c').relative_to(ROOT) if task.is_relative_to(ROOT) else task / 'ghidra.c'}")
        else:
            print(f"ghidra: {(g.stdout + g.stderr).strip()[-400:] or 'no output'}", file=sys.stderr)
    elif not args.no_ghidra and not ghidra_out.is_file():
        print("note: no Ghidra reference; run python3 tools/ghidra_decompile.py setup once", file=sys.stderr)
    write_jp_references(name, task, args)
    if args.print:
        print(f"--- {name} draft ---\n{out.read_text().strip()}\n--- {name} asm ---\n{compact_asm(asm_text)}")
    if args.score:
        source = unit_source(name)
        cmd = [name, str(source), "--body", str(out)] if source else [name, str(out), "-I", "src"]
        s = subprocess.run([sys.executable, str(ROOT / "tools" / "score_fn.py"), *cmd],
                           cwd=ROOT, capture_output=True, text=True)
        first = (s.stdout or s.stderr).strip().splitlines()
        if s.returncode == 2:
            print("draft does not compile yet; give the remaining `void *` arguments and members real "
                  "types, then run score_fn.py")
        else:
            print(first[0] if first else "no output")
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("names", nargs="+", metavar="name", help="function names in symbols.txt")
    ap.add_argument("--score", action="store_true", help="score each draft once with score_fn.py")
    ap.add_argument("--ghidra", action="store_true",
                    help="regenerate work/NAME/ghidra.c even if it exists (default: write it when missing "
                         "and the Ghidra project exists)")
    ap.add_argument("--no-ghidra", action="store_true", help="skip the Ghidra reference")
    ap.add_argument("--print", action="store_true",
                    help="print each draft and its assembly (without the byte column)")
    ap.add_argument("--no-context", action="store_true",
                    help="run m2c without the unit source as --context (draft_m2c_plain.c is always "
                         "written next to the typed draft)")
    ap.add_argument("--out-dir", type=Path, default=ROOT / "work")
    args = ap.parse_args(argv)
    return max(draft_one(name, args) for name in args.names)

if __name__ == "__main__":
    sys.exit(main())
