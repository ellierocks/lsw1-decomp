#!/usr/bin/env python3
"""Generate an evidence-only task pack for fn_80038628; never changes symbols."""

from __future__ import annotations

import csv
import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
CALLGRAPH = ROOT / "docs" / "recomp_exports" / "call_graph.tsv"
XREFS = ROOT / "docs" / "symbol_donors" / "gc_data_xrefs.tsv"
STRING_XREFS = ROOT / "docs" / "symbol_donors" / "gc_dol_string_renames.tsv"


def symbol_rows() -> list[tuple[int, str, str]]:
    rows = []
    pattern = re.compile(r"^(\S+)\s+=\s+\.text:0x([0-9A-Fa-f]+);\s*//\s*(.*)$")
    for line in SYMBOLS.read_text(encoding="utf-8").splitlines():
        match = pattern.match(line)
        if match:
            rows.append((int(match.group(2), 16), match.group(1), line))
    return sorted(rows)


def function_asm(name: str, address: int) -> str:
    candidates = (name, f"fn_{address:08X}")
    for path in sorted((ROOT / "build" / "GL5E4F" / "asm").glob("*.s")):
        text = path.read_text(encoding="utf-8")
        for candidate in candidates:
            start = text.find(f".fn {candidate},")
            if start < 0:
                continue
            end = text.find(f".endfn {candidate}", start)
            if end >= 0:
                return text[start:end + len(f".endfn {candidate}")] + "\n"
    raise RuntimeError(f"assembly for {name} not found")


def direct_edges(address: int) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    with CALLGRAPH.open(newline="", encoding="utf-8") as source:
        rows = list(csv.DictReader(source, delimiter="\t"))
    return ([row for row in rows if row["caller_address"] == f"0x{address:08X}"],
            [row for row in rows if row["callee_address"] == f"0x{address:08X}"])


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("target", nargs="?", default="fn_80038628")
    args = parser.parse_args()
    name = args.target
    symbols = symbol_rows()
    index = next(i for i, (_, symbol, _) in enumerate(symbols) if symbol == name)
    address = symbols[index][0]
    out = ROOT / "docs" / "recomp_exports" / "task_packs" / name
    asm = function_asm(name, address)
    callees, callers = direct_edges(address)
    nearby = symbols[max(0, index - 3):index + 4]
    xrefs = [line for line in XREFS.read_text(encoding="utf-8").splitlines()
             if f"\t{name}\t" in line] if XREFS.is_file() else []
    string_xrefs = [line for line in STRING_XREFS.read_text(encoding="utf-8").splitlines()
                    if f"\t{name}\t" in line] if STRING_XREFS.is_file() else []
    stack = re.search(r"stwu r1, (-0x[0-9A-Fa-f]+)\(r1\)", asm)
    registers = sorted({int(value) for value in re.findall(r"\br(\d+)\b", asm)})
    out.mkdir(parents=True, exist_ok=True)
    (out / "asm.s").write_text(asm, encoding="utf-8")

    (out / "direct_callees.tsv").write_text(
        "caller_address\tcaller_name\tcallee_address\tcallee_name\tcallsite_address\tevidence\n" +
        "".join("\t".join(row[key] for key in ("caller_address", "caller_name", "callee_address", "callee_name", "callsite_address", "evidence")) + "\n" for row in callees), encoding="utf-8")
    (out / "direct_callers.tsv").write_text(
        "caller_address\tcaller_name\tcallee_address\tcallee_name\tcallsite_address\tevidence\n" +
        "".join("\t".join(row[key] for key in ("caller_address", "caller_name", "callee_address", "callee_name", "callsite_address", "evidence")) + "\n" for row in callers), encoding="utf-8")
    (out / "nearby_symbols.txt").write_text("\n".join(line for _, _, line in nearby) + "\n", encoding="utf-8")
    (out / "data_xrefs.tsv").write_text("\n".join(xrefs) + ("\n" if xrefs else ""), encoding="utf-8")
    (out / "string_xrefs.tsv").write_text("\n".join(string_xrefs) + ("\n" if string_xrefs else ""), encoding="utf-8")

    platform = [row for row in callees if row["callee_name"].startswith(("OS", "GX", "PAD", "VI", "DVD", "AX", "AI", "AR", "Nu"))]
    notes = [
        f"# Evidence task pack: `{name}`", "",
        f"- Address: `0x{address:08X}`", f"- Size: `0x{symbols[index][2].split('size:0x')[1].split()[0] if 'size:0x' in symbols[index][2] else 'unknown'}` (from `symbols.txt`)",
        f"- Assembly source: `build/GL5E4F/asm/auto_01_8001752C_text.s`", "",
        "## Stack/register observations", "",
        f"- Prologue stack allocation: `{stack.group(0) if stack else 'not found'}`.",
        f"- General-purpose registers mentioned in extracted assembly: {', '.join(f'`r{reg}`' for reg in registers)}.",
        "- These are mechanical assembly observations, not an inferred ABI or signature.", "",
        "## Direct call evidence", "",
        f"- Direct callees: **{len(callees)}**; direct callers: **{len(callers)}**.",
        "- `direct_callees.tsv` and `direct_callers.tsv` are derived from `docs/recomp_exports/call_graph.tsv` using PPC `bl`/`bla` evidence.",
        "- The legacy `docs/symbol_donors/call_graph.tsv` is intentionally not used here.", "",
        "## Platform-boundary calls", "",
    ]
    notes.extend(f"- `{row['callee_name']}` at callsite `{row['callsite_address']}` ({row['evidence']})." for row in platform)
    notes += ["", "## Data/xref evidence", "", f"- Existing `gc_data_xrefs.tsv` rows: **{len(xrefs)}**; copied verbatim to `data_xrefs.tsv`.",
              f"- Existing `gc_dol_string_renames.tsv` rows: **{len(string_xrefs)}**; copied verbatim to `string_xrefs.tsv`.",
              "- No string interpretation is asserted by this pack.", "", "## Confidence notes", "",
              "- HIGH: direct caller/callee rows include a PPC `bl`/`bla` instruction and direct target from the recomp callgraph export.",
              "- MED: nearby symbols and existing data-xref table rows provide locality/reference evidence only.",
              "- LOW: any functional description or rename hypothesis below is unaccepted and requires independent review.", "",
              "## Suggested rename candidates — UNACCEPTED", ""]
    if name == "fn_80038628":
        notes += ["- `GamePlatformInit` — LOW; motivated only by the direct calls to OS/GX/PAD initialization-named functions.",
                  "- `GameStartupSetup` — LOW; same limited evidence.",
                  "- Keep the current name `fn_80038628`; this pack does not authorize a rename."]
    else:
        notes += ["- No rename proposal is warranted by this evidence-only pack."]
    notes += ["",
              "## Verification/build", "",
              "```sh", "python3 configure.py", "ninja build/GL5E4F/main.dol", "# Then compare the relevant object/function in objdiff.", "```", ""]
    (out / "context.md").write_text("\n".join(notes), encoding="utf-8")
    (out / "status.md").write_text("# Status\n\nInspection only. No rename or decompilation has been accepted.\n", encoding="utf-8")
    print(f"wrote {out}; {len(callees)} callees, {len(callers)} callers, {len(xrefs)} data xrefs, {len(string_xrefs)} string xrefs")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
