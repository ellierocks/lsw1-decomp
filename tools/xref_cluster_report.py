#!/usr/bin/env python3
"""Summarize named-string xref clusters for symbol discovery."""

from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
XREFS = ROOT / "build" / "xrefs" / "string_xrefs.txt"
OUT_DIR = ROOT / "build" / "xrefs"


@dataclass(frozen=True)
class Xref:
    string: str
    string_addr: str
    function: str
    function_addr: str
    at_addr: str


def parse_xrefs(path: Path) -> list[Xref]:
    rows: list[Xref] = []
    for line in path.read_text().splitlines():
        if not line.startswith("str_"):
            continue
        fields = line.split()
        if len(fields) < 7 or fields[4] != "at":
            continue
        rows.append(
            Xref(
                string=fields[0],
                string_addr=fields[1],
                function=fields[2],
                function_addr=fields[3],
                at_addr=fields[5],
            )
        )
    return rows


def grouped_by_function(rows: list[Xref], prefixes: tuple[str, ...]) -> dict[str, list[Xref]]:
    grouped: dict[str, list[Xref]] = defaultdict(list)
    for row in rows:
        if row.string.startswith(prefixes):
            grouped[f"{row.function} {row.function_addr}"].append(row)
    return dict(sorted(grouped.items(), key=lambda item: item[0].split()[-1]))


def write_function_report(path: Path, title: str, grouped: dict[str, list[Xref]]) -> None:
    lines = [title, "=" * len(title), ""]
    lines.append(f"Functions: {len(grouped)}")
    lines.append(f"Xrefs: {sum(len(rows) for rows in grouped.values())}")
    lines.append("")

    for function, rows in grouped.items():
        unique_strings = sorted({row.string for row in rows})
        lines.append(f"{function}")
        lines.append(f"  xrefs: {len(rows)}")
        for name in unique_strings:
            count = sum(1 for row in rows if row.string == name)
            suffix = f" x{count}" if count > 1 else ""
            lines.append(f"  - {name}{suffix}")
        lines.append("")

    path.write_text("\n".join(lines).rstrip() + "\n")


def write_summary(path: Path, rows: list[Xref]) -> None:
    script_parser_prefixes = (
        "str_ScriptToken",
        "str_ScriptSection",
        "str_ScriptSource",
        "str_ScriptState",
        "str_ScriptExt",
        "str_ScriptPath",
        "str_ScriptCmd",
    )
    buckets = {
        "Nu2 file/dat/pp/debug/special": (
            "str_NuFile",
            "str_NuDat",
            "str_NuPP",
            "str_NuDebug",
            "str_NuSpecial",
        ),
        "AI script parser/source": script_parser_prefixes,
        "Runtime script handlers": ("str_ScriptParam", "str_ScriptFlag", "str_ScriptDir", "str_ScriptScope", "str_ScriptTarget", "str_ScriptCategory", "str_ScriptBehavior", "str_ScriptAction", "str_ScriptSide"),
        "Shop frontend": ("str_Shop",),
        "Podrace": ("str_Podrace",),
        "AI allocators": ("str_AIScriptBufferAlloc", "str_AISysBufferAlloc"),
        "Other allocators": ("str_CreditBufferAlloc",),
    }

    lines = ["String Xref Cluster Summary", "===========================", ""]
    for label, prefixes in buckets.items():
        grouped = grouped_by_function(rows, prefixes)
        unnamed = sum(1 for function in grouped if function.startswith("fn_"))
        lines.append(
            f"{label}: {sum(len(v) for v in grouped.values())} xrefs, "
            f"{len(grouped)} functions, {unnamed} unnamed"
        )
        for function, function_rows in sorted(
            grouped.items(), key=lambda item: (-len(item[1]), item[0].split()[-1])
        )[:12]:
            unique = sorted({row.string for row in function_rows})
            preview = ", ".join(unique[:4])
            if len(unique) > 4:
                preview += ", ..."
            lines.append(f"  {function}: {len(function_rows)} xrefs ({preview})")
        lines.append("")

    path.write_text("\n".join(lines).rstrip() + "\n")


def main() -> None:
    if not XREFS.exists():
        raise SystemExit(f"missing xref input: {XREFS}")

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    rows = parse_xrefs(XREFS)
    write_summary(OUT_DIR / "xref_cluster_report.txt", rows)
    write_function_report(
        OUT_DIR / "script_handler_xrefs.txt",
        "Script Handler String Xrefs",
        grouped_by_function(
            rows,
            (
                "str_ScriptParam",
                "str_ScriptFlag",
                "str_ScriptDir",
                "str_ScriptScope",
                "str_ScriptTarget",
                "str_ScriptCategory",
                "str_ScriptBehavior",
                "str_ScriptAction",
                "str_ScriptSide",
            ),
        ),
    )
    write_function_report(
        OUT_DIR / "script_parser_xrefs.txt",
        "AI Script Parser String Xrefs",
        grouped_by_function(
            rows,
            (
                "str_ScriptToken",
                "str_ScriptSection",
                "str_ScriptSource",
                "str_ScriptState",
                "str_ScriptExt",
                "str_ScriptPath",
                "str_ScriptCmd",
            ),
        ),
    )
    write_function_report(
        OUT_DIR / "shop_xrefs.txt",
        "Shop String Xrefs",
        grouped_by_function(rows, ("str_Shop",)),
    )
    write_function_report(
        OUT_DIR / "podrace_xrefs.txt",
        "Podrace String Xrefs",
        grouped_by_function(rows, ("str_Podrace",)),
    )
    write_function_report(
        OUT_DIR / "allocator_xrefs.txt",
        "Allocator String Xrefs",
        grouped_by_function(rows, ("str_AIScriptBufferAlloc", "str_AISysBufferAlloc", "str_CreditBufferAlloc")),
    )
    print(f"Wrote cluster reports to {OUT_DIR}")


if __name__ == "__main__":
    main()
