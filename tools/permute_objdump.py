#!/usr/bin/env python3
"""Strict-verifier disassembly for decomp-permuter's `objdump_command`.

decomp-permuter scores a candidate by diffing objdump text of the candidate and
target objects. Its relocation handling compares symbol names, so a candidate's
`.rodata+0x8` never equals retail's `lbl_8040F8A0` even when the strict verifier
proves the bytes identical, and the unit's other functions would be diffed too.
`permute_setup.py` points the permuter here instead:

- For `target.o` this prints the target function, one instruction per line
  (dtk disassembly, function-relative labels, relocations inline).
- For a candidate it runs the same comparison as `score_fn.py` (verify_fn
  against the DTK target and the retail DOL). A row the verifier accepts
  (identical masked word, same/aliased or byte-verified reference) is printed
  with the target's text; every other row keeps the candidate's own text. Unless
  the verdict is a strict `MATCH`, a final `verify_fn <verdict>` line is added, so
  the permuter reaches score 0 exactly when the strict verifier matches.

    python3 tools/permute_objdump.py --dir work/permute/NAME OBJECT
"""

from __future__ import annotations

import argparse
import contextlib
import io
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import verify_fn as vf  # noqa: E402

SENTINEL = "verify_fn"


def verify_args(name: str, obj: Path | None, compiled_symbol: str | None):
    return argparse.Namespace(name=name, unit=None, object=str(obj) if obj else None,
                              compiled_symbol=compiled_symbol, target_object=None, build=False,
                              allow_stale=True, dol=None, no_dol=False, quiet=False)


def insn_text(dis, func, index: int) -> str:
    if dis is not None:
        return " ".join(dis[index].split())
    return f".word 0x{func.insns[index].word:08x}"


def accepted(rep, row, target, mine) -> bool:
    """True when the verifier accepted this aligned row without reservation."""
    ti, mi, kind, _detail = row
    if ti is None or mi is None or kind not in ("ok", "mapped"):
        return False
    if kind == "ok":
        return True
    mref = mine.insns[mi].ref
    prefix = mref.describe(False) + " -> "
    statuses = [v for k, v in rep.mappings if k.startswith(prefix)]
    return bool(statuses) and all(v.startswith("verified") for v in statuses)


def lines(rows: list[str]) -> str:
    out = ["", "00000000 <func>:"]
    for i, text in enumerate(rows):
        mnemonic, _, args = text.partition(" ")
        out.append(f"{i * 4:8x}:\t00 00 00 00 \t{mnemonic} {args}".rstrip())
    return "\n".join(out) + "\n"


def target_rows(name: str) -> list[str]:
    _rep, target, _mine, tdis, _mdis, *_ = vf.verify(_self_args(name))
    return [insn_text(tdis, target, i) for i in range(len(target.insns))]


def _self_args(name: str):
    """Compare the target with itself; only the target listing is used."""
    entry = vf.SymbolTable().function(name)
    tpath = vf.find_target_object(name, entry.addr, entry.size, vf.lookup_unit(entry.addr))
    return verify_args(name, tpath, None)


def candidate_rows(name: str, obj: Path, compiled_symbol: str | None) -> tuple[list[str], str]:
    rep, target, mine, tdis, mdis, *_ = vf.verify(verify_args(name, obj, compiled_symbol))
    rows = []
    for row in rep.rows:
        ti, mi = row[0], row[1]
        if accepted(rep, row, target, mine):
            rows.append(insn_text(tdis, target, ti))
        elif mi is not None:
            rows.append(insn_text(mdis, mine, mi))
    return rows, rep.verdict


def dump(meta: dict, obj: Path, is_target: bool) -> str:
    if is_target:
        return lines(target_rows(meta["name"]))
    rows, verdict = candidate_rows(meta["name"], obj, meta.get("compiled_symbol"))
    if verdict != "MATCH":
        rows.append(f"{SENTINEL} {verdict.replace(' ', '_')}")
    return lines(rows)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dir", type=Path, required=True, help="permuter directory from permute_setup.py")
    ap.add_argument("object", type=Path)
    args = ap.parse_args(argv)
    meta = json.loads((args.dir / "permute.json").read_text())
    is_target = args.object.resolve() == (args.dir / "target.o").resolve()
    try:
        with contextlib.redirect_stderr(io.StringIO()):
            text = dump(meta, args.object.resolve(), is_target)
    except (vf.ToolError, ValueError, OSError) as e:
        print(f"permute_objdump: {e}", file=sys.stderr)
        return 2
    sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
