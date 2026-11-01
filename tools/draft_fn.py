#!/usr/bin/env python3
"""Write an m2c first draft for one function, ready for score_fn.py and variant_search.py.

    python3 tools/draft_fn.py NuAnimCurve2SetApplyToJointTransLoc
    python3 tools/draft_fn.py NAME --score          # also score the draft once

The draft goes to `work/NAME/draft_m2c.c` (ignored by git). m2c runs from the pinned checkout
(`tools/fetch_m2c.sh`, default ~/.cache/lsw1/m2c; `M2C` overrides it) with the PPC GCC target.
Its output is a starting point only and usually does not compile until typed: parameter types are `void*`, locals are `var_*`/`temp_*`, and
struct accesses are raw offsets. Replace them with the shared headers, then iterate with
`score_fn.py` and `variant_search.py`. Nothing here counts as a match until
`verify_fn.py --build --strict-symbols` passes on the unit source.
"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TARGET = "ppc-gcc-c"


def m2c_dir() -> Path:
    return Path(os.environ.get("M2C") or Path(os.environ.get("LSW1_CACHE", Path.home() / ".cache/lsw1")) / "m2c")


def clean(text: str, name: str) -> str:
    """Make m2c output compile as C: no `?` types, no extern comments, a types.h include."""
    text = re.sub(r"^extern \? (\w+);", r"extern char \1[];", text, flags=re.M)
    text = re.sub(r"^\? (\w+)\(", r"void \1(", text, flags=re.M)
    text = re.sub(r"\s*/\* extern \*/", "", text)
    text = re.sub(r"\(\?(\s*\*?\w*)\)", r"(void\1)", text)
    return f'#include "types.h"\n\n{text.strip()}\n'


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="function name in symbols.txt")
    ap.add_argument("--score", action="store_true", help="score the draft once with score_fn.py")
    ap.add_argument("--out-dir", type=Path, default=ROOT / "work")
    args = ap.parse_args(argv)
    task = args.out_dir / args.name
    asm = task / "asm.s"
    if not asm.is_file():
        subprocess.run([sys.executable, str(ROOT / "tools" / "ls1_task_pack.py"), args.name,
                        "--out-dir", str(args.out_dir)], cwd=ROOT, capture_output=True)
    if not asm.is_file() or not asm.read_text().strip():
        print(f"error: no assembly for {args.name}; run bash build.sh, then tools/ls1_task_pack.py", file=sys.stderr)
        return 2
    m2c = m2c_dir()
    if not (m2c / "m2c.py").is_file():
        print("error: m2c not fetched; run bash tools/fetch_m2c.sh", file=sys.stderr)
        return 2
    r = subprocess.run([sys.executable, str(m2c / "m2c.py"), "-t", TARGET, str(asm.resolve())],
                       capture_output=True, text=True)
    if r.returncode or not r.stdout.strip():
        print(f"error: m2c failed\n{(r.stdout + r.stderr).strip()[-800:]}", file=sys.stderr)
        return 2
    out = task / "draft_m2c.c"
    out.write_text(clean(r.stdout, args.name))
    print(f"wrote {out.relative_to(ROOT) if out.is_relative_to(ROOT) else out}")
    if args.score:
        s = subprocess.run([sys.executable, str(ROOT / "tools" / "score_fn.py"), args.name, str(out),
                            "-I", "src"], cwd=ROOT, capture_output=True, text=True)
        first = (s.stdout or s.stderr).strip().splitlines()
        if s.returncode == 2:
            print("draft does not compile yet (expected: m2c leaves `void *` members); give the "
                  "arguments real struct types, then run score_fn.py")
        else:
            print(first[0] if first else "no output")
    return 0


if __name__ == "__main__":
    sys.exit(main())
