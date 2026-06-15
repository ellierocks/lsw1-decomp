#!/usr/bin/env python3
"""
Generate reverse-engineering leads from all available build executables.

Searches for:
  - Nu subsystem function name patterns
  - Source file paths → module coverage
  - Debug / assertion strings → function name candidates
  - Script commands (from Xbox demo .scp files)
  - Dispatch table signatures
  - Naming opportunities (functions named in strings but not in symbols.txt)

Outputs:
  research/revisions/leads/rename_candidates.md
  research/revisions/leads/symbol_recovery_leads.md
  research/revisions/leads/subsystem_candidates.md

Usage:
    python3 tools/revisions/leads.py
"""
from __future__ import annotations

import csv
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILDS_TSV = ROOT / "research" / "revisions" / "builds.tsv"
SYMBOLS_TXT = ROOT / "config" / "GL5E4F" / "symbols.txt"
LEADS_DIR = ROOT / "research" / "revisions" / "leads"

MIN_STRING_LEN = 6

# Pattern for "FuncName : error message" style debug strings
FUNC_MSG_RE = re.compile(
    r"^([A-Z][A-Za-z0-9_]+(?:::[A-Za-z0-9_]+)?)\s*[:\-]\s*.{6,}$"
)
# Nu function names
NU_FUNC_RE = re.compile(r"^(Nu[A-Z][A-Za-z0-9_]+)")
# Source file paths
SOURCE_RE = re.compile(r"[A-Za-z0-9_./\\-]+\.(c|cpp|h)$", re.IGNORECASE)
# Script command pattern from .scp files
SCP_CMD_RE = re.compile(r"^\s*([A-Za-z_][A-Za-z0-9_]+)\s*\(", re.MULTILINE)
# Script function definition
SCP_DEF_RE = re.compile(r"^\s*function\s+([A-Za-z_][A-Za-z0-9_]+)", re.MULTILINE)


def load_builds() -> list[dict]:
    with open(BUILDS_TSV, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def get_strings(path: Path) -> list[str]:
    r = subprocess.run(
        ["strings", "-n", str(MIN_STRING_LEN), str(path)],
        capture_output=True, text=True,
    )
    return r.stdout.splitlines()


def load_gc_symbols() -> set[str]:
    """Load all named symbols from GC symbols.txt."""
    named = set()
    with open(SYMBOLS_TXT) as f:
        for line in f:
            m = re.match(r"^([A-Za-z_][A-Za-z0-9_:]+)\s*=", line)
            if m and not m.group(1).startswith(("fn_", "lbl_", "jumptable_", "str_")):
                named.add(m.group(1))
    return named


def extract_script_commands(xbox_demo_dir: Path) -> dict[str, list[str]]:
    """Extract script commands and function names from Xbox demo .scp files."""
    commands: dict[str, list[str]] = {}
    for scp in xbox_demo_dir.rglob("*.scp"):
        text = scp.read_text(encoding="utf-8", errors="replace")
        cmds = SCP_CMD_RE.findall(text)
        defs = SCP_DEF_RE.findall(text)
        rel = str(scp.relative_to(xbox_demo_dir))
        commands[rel] = sorted(set(cmds + defs))
    return commands


def write_rename_candidates(builds: list[dict], gc_named: set[str]) -> str:
    """Find strings of the form 'NuFuncName : ...' in executables not yet in symbols.txt."""
    seen: dict[str, list[str]] = {}  # func_name -> [build_ids]

    for b in builds:
        if b.get("available", "").strip() != "yes":
            continue
        exe_raw = b.get("exe_path", "").strip()
        if not exe_raw:
            continue
        exe = ROOT / exe_raw
        if not exe.exists():
            continue
        for s in get_strings(exe):
            m = NU_FUNC_RE.match(s)
            if m:
                name = m.group(1)
                if name not in gc_named:
                    seen.setdefault(name, []).append(b["build_id"])
            m2 = FUNC_MSG_RE.match(s)
            if m2:
                name = m2.group(1)
                if name not in gc_named and len(name) > 5:
                    seen.setdefault(name, []).append(b["build_id"])

    lines = [
        "# Rename Candidates",
        "",
        "Function names found in executable debug strings across builds but not yet",
        "present in `config/GL5E4F/symbols.txt`.",
        "",
        "| Function Name | Found In Builds | Notes |",
        "|---------------|-----------------|-------|",
    ]
    for name in sorted(seen):
        builds_str = ", ".join(sorted(set(seen[name])))
        lines.append(f"| `{name}` | {builds_str} | |")

    return "\n".join(lines)


def write_symbol_leads(builds: list[dict]) -> str:
    """Summarize source file coverage by build and flag interesting residue."""
    lines = [
        "# Symbol Recovery Leads",
        "",
        "Source file paths and subsystem debug strings found in each build.",
        "Cross-platform string matches help link GC addresses to named functions.",
        "",
    ]

    all_src: dict[str, set[str]] = {}  # build_id -> source paths
    all_nu: dict[str, set[str]] = {}   # build_id -> Nu debug strings

    for b in builds:
        if b.get("available", "").strip() != "yes":
            continue
        exe_raw = b.get("exe_path", "").strip()
        if not exe_raw:
            continue
        exe = ROOT / exe_raw
        if not exe.exists():
            continue
        strings = get_strings(exe)
        src = {s for s in strings if SOURCE_RE.search(s)}
        nu = {s for s in strings if NU_FUNC_RE.search(s)}
        all_src[b["build_id"]] = src
        all_nu[b["build_id"]] = nu

    # Build summary table
    lines += [
        "## Source Path Coverage by Build",
        "",
        "| Build | Source Paths | Nu Debug Strings |",
        "|-------|-------------|-----------------|",
    ]
    for bid in sorted(all_src):
        lines.append(f"| `{bid}` | {len(all_src[bid])} | {len(all_nu.get(bid, set()))} |")

    # Unique source paths per build (not in GC)
    gc_src = all_src.get("gc_us_retail", set())
    lines += [
        "",
        "## Platform-Unique Source Paths",
        "",
        "Source paths found in non-GC builds but not in the GC DOL — "
        "may indicate platform-specific modules.",
        "",
    ]
    for bid, srcs in sorted(all_src.items()):
        if bid == "gc_us_retail":
            continue
        unique = srcs - gc_src
        if unique:
            lines.append(f"### `{bid}` ({len(unique)} unique)")
            for s in sorted(unique)[:30]:
                lines.append(f"- `{s}`")
            lines.append("")

    # Script command summary from Xbox demo
    xbox_dir = ROOT / "orig" / "xbox" / "demo"
    if xbox_dir.exists():
        lines += [
            "## Xbox Demo Script Commands",
            "",
            "Script function names and commands from the Xbox demo `.scp` files.",
            "These may correspond to game script dispatch table entries.",
            "",
        ]
        cmds = extract_script_commands(xbox_dir)
        all_cmds: set[str] = set()
        for script_cmds in cmds.values():
            all_cmds.update(script_cmds)
        lines.append(f"Total unique command/function names: **{len(all_cmds)}**")
        lines.append("")
        lines.append("| Script File | Commands |")
        lines.append("|-------------|----------|")
        for scp_path, script_cmds in sorted(cmds.items()):
            lines.append(f"| `{scp_path}` | {', '.join(f'`{c}`' for c in script_cmds[:10])}{'...' if len(script_cmds)>10 else ''} |")

    return "\n".join(lines)


def write_subsystem_candidates(builds: list[dict]) -> str:
    """Identify potential subsystem names from string patterns."""
    subsystems: dict[str, set[str]] = {}  # subsystem prefix -> build_ids

    prefix_re = re.compile(r"^([A-Z][a-z]{2,}[A-Z][a-zA-Z0-9]+)(?:\s|_|:)")

    for b in builds:
        if b.get("available", "").strip() != "yes":
            continue
        exe_raw = b.get("exe_path", "").strip()
        if not exe_raw:
            continue
        exe = ROOT / exe_raw
        if not exe.exists():
            continue
        for s in get_strings(exe):
            m = prefix_re.match(s)
            if m:
                subsystems.setdefault(m.group(1), set()).add(b["build_id"])

    lines = [
        "# Subsystem Candidates",
        "",
        "Repeated string prefixes suggesting subsystem boundaries.",
        "Sorted by frequency of occurrence across builds.",
        "",
        "| Prefix | Build Count | Builds |",
        "|--------|-------------|--------|",
    ]
    ranked = sorted(subsystems.items(), key=lambda x: -len(x[1]))
    for prefix, bids in ranked[:80]:
        lines.append(
            f"| `{prefix}` | {len(bids)} | {', '.join(sorted(bids))[:80]} |"
        )

    return "\n".join(lines)


def main():
    builds = load_builds()
    gc_named = load_gc_symbols() if SYMBOLS_TXT.exists() else set()
    LEADS_DIR.mkdir(parents=True, exist_ok=True)

    print("  generating rename_candidates.md ...")
    rc = write_rename_candidates(builds, gc_named)
    (LEADS_DIR / "rename_candidates.md").write_text(rc, encoding="utf-8")

    print("  generating symbol_recovery_leads.md ...")
    sl = write_symbol_leads(builds)
    (LEADS_DIR / "symbol_recovery_leads.md").write_text(sl, encoding="utf-8")

    print("  generating subsystem_candidates.md ...")
    sc = write_subsystem_candidates(builds)
    (LEADS_DIR / "subsystem_candidates.md").write_text(sc, encoding="utf-8")

    print("Done.")


if __name__ == "__main__":
    main()
