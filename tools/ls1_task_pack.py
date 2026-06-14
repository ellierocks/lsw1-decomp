#!/usr/bin/env python3
"""Generate a self-contained task folder for decompiling one function.

Output (tasks/<FunctionName>/):
  prompt.md          — LLM task description
  context.md         — Struct definitions, signatures, types
  asm.s              — Extracted assembly for the function
  related_symbols.md — Callers, callees, Mac hints, data refs
  verify.sh          — Verification instructions (with objdiff screenshot)
  status.md          — Status: TODO / IN_PROGRESS / MATCHED / BLOCKED

In batch mode, functions are sorted by priority score so the easiest
wins (small + named + Nu2 + struct hints + few callees) appear first.

Generates tasks/index.md with a scored table of all tasks.

Usage:
    python tools/ls1_task_pack.py NuAnimKeyLerp
    python tools/ls1_task_pack.py fn_8001E76C
    python tools/ls1_task_pack.py 0x8001E76C
    python tools/ls1_task_pack.py nuanim --top 3
    python tools/ls1_task_pack.py nuanim --all
    python tools/ls1_task_pack.py --out-dir work NuAnimKeyLerp
"""
import argparse
import csv
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM_FILE = ROOT / "build/GL5E4F/asm/auto_01_800034A0_text.s"
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"
CALLGRAPH = ROOT / "docs/symbol_donors/call_graph.tsv"
MAC_TSVS = [
    ROOT / "docs/symbol_donors/mac_lsw1_demo_symbols.tsv",
    ROOT / "docs/symbol_donors/mac_lsw2_symbols.tsv",
]

TEXT_START = 0x800034A0

# Module → header mapping
MODULE_HEADERS = {
    "nufile":       ("nucore/nufile.h",),
    "numem":        ("nucore/numem.h",),
    "error":        ("nucore/numem.h",),
    "numath":       ("numath/numath.h",),
    "nuanim":       ("nu3dx/nuanim.h", "nu3dx/nu3dx.h"),
    "render":       ("nu3dx/nu3dx.h",),
    "scene":        ("nu3dx/nu3dx.h",),
    "nusound":      ("nusound/nusound.h",),
    "gamelib":      ("nu.h",),
    "gamecode":     ("nu.h",),
}

MODULES = {
    "nufile":  (0x800034A0, 0x80006000),
    "numem":   (0x80006F74, 0x80007468),
    "error":   (0x80007468, 0x80008000),
    "numath":  (0x80008000, 0x80012000),
    "nuanim":  (0x80016A00, 0x80024000),
    "render":  (0x80024000, 0x8005C000),
    "scene":   (0x8005C000, 0x80090000),
    "nusound": (0x80090000, 0x800B0000),
    "gamelib": (0x800B0000, 0x80100000),
    "gamecode":(0x80100000, 0x8018CB00),
}

NU2_PREFIXES = ("Nu", "Action_", "Condition_")


# ── helpers ──────────────────────────────────────────────────────────

def load_symtab():
    funcs = []
    with open(SYMTAB) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            name = line.split("=")[0].strip()
            rest = line.split("=", 1)[1].strip()
            if ";" in rest: rest = rest.split(";")[0].strip()
            loc = rest.rsplit(":", 1)[-1] if ":" in rest else rest
            try: addr = int(loc, 16)
            except ValueError: continue
            size_str = line.split("size:")[1].split(";")[0].strip() if "size:" in line else ""
            sec = line.split(".")[1].split(":")[0] if "." in line else ""
            try: size = int(size_str, 16) if size_str else 0
            except ValueError: size = 0
            funcs.append((addr, name, size, sec))
    funcs.sort()
    return funcs


def resolve_target(raw: str):
    """Return (name, addr, size, section) or None."""
    funcs = load_symtab()
    if raw.startswith("0x"):
        addr = int(raw, 16)
        for a, n, s, sec in funcs:
            if a == addr:
                return n, a, s, sec
        return None
    for a, n, s, sec in funcs:
        if n == raw:
            return n, a, s, sec
    return None


def extract_asm(addr, size, known_name=None):
    """Extract assembly for the function at addr from the monolithic ASM file."""
    candidates = [f"fn_{addr:08X}"]
    if known_name and any(known_name.startswith(p) for p in NU2_PREFIXES):
        candidates.insert(0, known_name)
    lines = ASM_FILE.read_text().splitlines()
    for candidate in candidates:
        out = []
        capture = False
        for line in lines:
            if f".fn {candidate}" in line:
                capture = True
            if capture:
                out.append(line)
            if capture and f".endfn {candidate}" in line:
                break
        if out:
            return "\n".join(out) + "\n"
    return None


def callers_of(addr):
    """Return list of (caller_name, caller_addr) that call addr."""
    results = []
    with open(CALLGRAPH) as f:
        for line in f:
            if line.startswith("#"): continue
            parts = line.strip().split("\t")
            if len(parts) < 4: continue
            cn = parts[0].strip()
            ca = parts[1].strip()
            callee_addr = parts[2].strip()
            if int(callee_addr, 16) == addr:
                results.append((cn, ca))
    return results


def callees_of(addr):
    """Return list of (callee_name, callee_addr) called by addr."""
    results = []
    raw_name = f"fn_{addr:08X}"
    with open(CALLGRAPH) as f:
        for line in f:
            if line.startswith("#"): continue
            parts = line.strip().split("\t")
            if len(parts) < 4: continue
            cn = parts[0].strip()
            if cn == raw_name:
                callee_name = parts[3].strip()
                callee_addr = parts[2].strip()
                results.append((callee_name, callee_addr))
    return results


def find_mac_hits(query):
    """Return list of (source, name, demangled) from Mac TSVs."""
    results = []
    for tsv in MAC_TSVS:
        if not tsv.exists(): continue
        source = tsv.stem.replace("mac_", "").replace("_symbols", "")
        with open(tsv) as f:
            for row in csv.reader(f, delimiter="\t"):
                if len(row) < 8: continue
                if row[4].strip() != "text": continue
                if row[7].strip().endswith(".eh"): continue
                clean = row[7].strip()
                if query in clean:
                    results.append((source, clean, row[8].strip() if len(row) > 8 else ""))
    return results


def extract_struct_defs(name, addr):
    """Return struct definitions relevant to this function."""
    module = None
    for mod, (lo, hi) in MODULES.items():
        if lo <= addr < hi:
            module = mod
            break

    wanted_structs = set()
    mac_hits = find_mac_hits(name) + find_mac_hits(f"fn_{addr:08X}")
    for _, clean, dem in mac_hits:
        for s in re.findall(r'(nu[a-z]+_s|NU[A-Z]+_s)', clean):
            wanted_structs.add(s)
        for s in re.findall(r'(nu[a-z]+_s|NU[A-Z]+_s)', dem):
            wanted_structs.add(s)

    struct_headers = {"nuanimkey_s", "nuanimcurve_s", "nuanimdata_s",
                       "nuanimdata2_s", "nuanimcurve2_s", "nuanimcurvedata_s",
                       "nuanimtime_s", "nuanimcurveset_s", "nuanimdatachunk_s"}
    if module == "nuanim":
        wanted_structs.update(struct_headers)
    if module == "nufile":
        wanted_structs.update({"nudathdr_s", "nudatinfo_s", "nudfnode_s",
                                "nudatfile_s", "numemfile_s", "fileinfo_s", "filebuff_s",
                                "nuiffhdr_s", "BlockInfo"})

    header_files = MODULE_HEADERS.get(module, [])
    common_headers = ["types.h"]
    all_headers = common_headers + list(header_files)

    defs = []
    for hdr in all_headers:
        path = ROOT / "src" / hdr
        if not path.exists(): continue
        content = path.read_text()
        defs.append(f"// From {hdr}\n{content.strip()}\n")

    return "\n".join(defs), module


def find_data_refs(asm_text):
    """Find data references (rodata/data/sdata globals) in the asm."""
    refs = set()
    for m in re.finditer(r'lbl_([0-9A-Fa-f]+)', asm_text):
        refs.add(f"0x{m.group(1)}")
    for m in re.finditer(r'fn_([0-9A-Fa-f]+)', asm_text):
        refs.add(f"fn_0x{m.group(1)}")
    return sorted(refs)


# ── object-neighborhood helpers ──────────────────────────────────────

def find_neighbors(name, addr, context=2):
    """Return list of (addr, name, size, sec) for functions around addr.

    Includes `context` entries before and after in address-sorted order.
    """
    funcs = load_symtab()
    idx = None
    for i, (a, n, _, _) in enumerate(funcs):
        if a == addr and n == name:
            idx = i
            break
    if idx is None:
        return None
    start = max(0, idx - context)
    end = min(len(funcs), idx + context + 1)
    return funcs[start:end]


def extract_asm_for_cluster(cluster):
    """Extract combined ASM for all functions in the cluster."""
    parts = []
    for addr, name, size, _ in cluster:
        asm = extract_asm(addr, size, name)
        if asm:
            parts.append(f"// {name} @ 0x{addr:08X}\n{asm}")
    return "\n\n".join(parts)


# ── priority scoring ─────────────────────────────────────────────────

def priority_score(name, addr, size, mac_hits, callee_list, asm_text):
    """Return a score (higher = better / easier to match first)."""
    score = 0

    # size — smaller is easier
    if size <= 0x100:
        score += 3
    elif size <= 0x400:
        score += 2
    else:
        score += 1

    # named — known name means we've already done symbol recovery
    if not name.startswith("fn_"):
        score += 2

    # Nu2 engine prefix — likely a well-structured engine function
    if any(name.startswith(p) for p in NU2_PREFIXES):
        score += 1

    # struct hints from Mac signatures
    if mac_hits:
        has_struct = False
        for _, clean, dem in mac_hits:
            if re.search(r'[a-z]+_s', clean + dem):
                has_struct = True
                break
        if has_struct:
            score += 2
        else:
            score += 1  # has Mac evidence even without structs

    # few callees — leaf-ish functions are easier
    n_callees = len(callee_list)
    if n_callees == 0:
        score += 3
    elif n_callees <= 2:
        score += 2
    elif n_callees <= 5:
        score += 1

    # control flow complexity — flag switch/jump tables
    if asm_text:
        # bctr = jump table / switch, blr only = leaf
        has_bctr = "bctr" in asm_text
        has_blr = "blr" in asm_text
        branch_count = len(re.findall(r'\bb\s', asm_text))
        inst_count = max(len(asm_text.splitlines()), 1)
        branch_ratio = branch_count / inst_count
        # No jump table = easier
        if not has_bctr:
            score += 2
        # Mostly straight-line = easier
        if branch_ratio < 0.1:
            score += 1
        # Leaf function (only blr at end) = easier
        if has_blr and branch_count <= 1:
            score += 1

    return score


def priority_label(score):
    if score >= 12:
        return "★ easy"
    elif score >= 9:
        return "◎ medium"
    elif score >= 6:
        return "◆ hard"
    return "▲ complex"


# ── file generators ──────────────────────────────────────────────────

def gen_prompt(name, addr, size, sec, module, mac_hits, struct_defs):
    mac_line = f"  - Mac evidence: {mac_hits[0][1]}" if mac_hits else ""
    dem_line = f"    Demangled: {mac_hits[0][2]}" if mac_hits and mac_hits[0][2] else ""
    struct_types = set()
    for _, clean, dem in mac_hits:
        for s in re.findall(r'(nu[a-z]+_s|NU[A-Z]+_s)', clean + dem):
            struct_types.add(s)

    struct_lines = ""
    if struct_types:
        struct_lines = "\n".join(f"  - `{s}`" for s in sorted(struct_types))
        struct_lines = f"\n### Known struct types\n{struct_lines}\n"

    return f"""# Decompile {name}

## Task

Write a matching C function for `{name}` in the LEGO Star Wars: The Video Game
(GameCube) decompilation project.

- **Symbol:** `{name}`
- **Address:** `0x{addr:08X}`
- **Size:** `0x{size:X}` ({size} bytes)
- **Section:** `{sec}`
- **Source module:** `{module or "unknown"}`

## Constraints

- Match the original Metrowerks CodeWarrior for GameCube (PPC 750CL) compiler output exactly.
- Use the provided struct definitions from `context.md`.
- Name all called functions exactly as given (keep `fn_ADDR` if unnamed).
- The stack frame and prologue/epilogue must match exactly.
- Preserve instruction selection — the compiler was not optimizing and
  was fairly literal with C control flow.
- Register assignment in the original binary was determined by the
  Metrowerks calling convention (r3–r10 for parameters, r3 for return,
  r13–r31 are callee-saved).
- Use `f32` for `float`, `s32` for `int` / `signed int`, `u32` for
  `unsigned int`, `s16` for `short`, `u16` for `unsigned short`, `s8`
  for `char` (signed), `u8` for `unsigned char`.
- Do **not** add comments explaining what the code does.
- Match control flow exactly — every branch, every loop.
{struct_lines}
{mac_line}
{dem_line}
"""  # noqa: E501


def gen_context(name, addr, struct_defs_text, module):
    return f"""# Context for {name}

## Module
`{module or "unknown"}`

## Types
{struct_defs_text}

## Compilation
- Compiler: Metrowerks CodeWarrior for GameCube (mwcceppc.exe)
- CPU: PPC 750CL (Gekko)
- Calling convention: PPC EABI (r3–r10 params, r3 return,
  r13–r31 callee-saved, stack frames aligned to 8 bytes)
- ABI: No full IEEE denormals; `frsqrte` for reciprocal sqrt
"""


def gen_related_symbols(name, addr, callers, callees, mac_hits, data_refs):
    lines = [f"# Related symbols for {name}\n"]

    if callers:
        lines.append(f"## Callers ({len(callers)})\n")
        for cn, ca in sorted(callers, key=lambda x: x[1]):
            lines.append(f"- `{cn}` @ 0x{int(ca, 16):08X}")
        lines.append("")

    if callees:
        lines.append(f"## Callees ({len(callees)})\n")
        for cn, ca in sorted(callees, key=lambda x: x[1]):
            lines.append(f"- `{cn}` @ 0x{int(ca, 16):08X}")
        lines.append("")

    if mac_hits:
        lines.append(f"## Mac symbol hits ({len(mac_hits)})\n")
        seen = set()
        for src, clean, dem in mac_hits:
            if clean in seen: continue
            seen.add(clean)
            line = f"- [{src}] `{clean}`"
            if dem:
                line += f"\n  → `{dem}`"
            lines.append(line)
        lines.append("")

    if data_refs:
        lines.append(f"## Data references\n")
        for ref in data_refs:
            lines.append(f"- `{ref}`")
        lines.append("")

    return "\n".join(lines)


def gen_verify(name, addr, size, module):
    obj_path = f"build/GL5E4F/obj/{module or 'auto'}/{name}.o"
    return f"""#!/usr/bin/env bash
# Verification for {name} (0x{addr:08X})
#
# Steps:
#   1. Write matching C in the appropriate src/<module>/{name}.c
#   2. Add the object to configure.py under the {module or '???'} module
#   3. Reconfigure and build:
#
#        python3 configure.py && ninja
#
#   4. Check the objdiff report:
#
#        ninja build/GL5E4F/report.json
#        objdiff-cli report generate --config objdiff.json
#
#      Open build/GL5E4F/report.html in a browser for the visual diff,
#      or run objdiff-cli directly:
#
#        objdiff-cli diff \\
#          --base build/GL5E4F/obj/auto_01_800034A0_text.o \\
#          --target {obj_path} \\
#          --symbol {name} \\
#          --output /tmp/{name}_diff.png
#
#   5. Screenshot the objdiff UI showing 100% match and save to:
#      tasks/{name}/match_screenshot.png
#
# Troubleshooting:
#   - Stack mismatch? Check prologue stwu / epilogue addi sp
#   - Instruction mismatch? Verify register assignment matches
#   - Size mismatch? Check for extra padding or missing code
#
echo "Run: ninja build/GL5E4F/report.json"
"""


def gen_status(name, status="TODO"):
    return f"""# Status: {status}

Update this file as you work through the task.

Valid statuses:
  - TODO        — not started
  - IN_PROGRESS — actively working
  - MATCHED     — C compiles and matches 100%
  - BLOCKED     — blocked on a dependency or question

## Notes
"""
# ── object-neighborhood generators ────────────────────────────────────

def gen_candidate_c(cluster):
    """Generate a candidate.c template with stubs for all functions in the cluster."""
    lines = [
        '#include "types.h"',
        '// Add other required includes as needed',
        '',
        f'// Object-neighborhood matching cluster — {len(cluster)} functions',
        '// Fill in these stubs and compile them together as a single .o file',
        '// for more reliable register-allocation matching.',
        '',
    ]
    for addr, name, size, _ in cluster:
        if not name.startswith("fn_") and any(name.startswith(p) for p in NU2_PREFIXES):
            known = True
        else:
            known = False
        if known:
            lines.append(f'// {name} @ 0x{addr:08X} (0x{size:X} bytes, known name)')
            lines.append(f's32 {name}(void) {{')
            lines.append(f'    // TODO: implement')
            lines.append(f'    return 0;')
            lines.append(f'}}')
        else:
            lines.append(f'// TODO: {name} @ 0x{addr:08X} (0x{size:X} bytes)')
            lines.append(f's32 {name}(void) {{')
            lines.append(f'    // TODO: implement')
            lines.append(f'    return 0;')
            lines.append(f'}}')
        lines.append('')
    return '\n'.join(lines)


def gen_object_context(name, addr, cluster, module, struct_defs_text):
    mac_hits = find_mac_hits(name)
    if not mac_hits:
        mac_hits = find_mac_hits(f"fn_{addr:08X}")
    mac_lines = ""
    if mac_hits:
        seen = set()
        items = []
        for src, clean, dem in mac_hits:
            if clean in seen:
                continue
            seen.add(clean)
            items.append(f"  - [{src}] `{clean}`" + (f"\n    → `{dem}`" if dem else ""))
        mac_lines = "\n" + "\n".join(items)

    lines = [
        f'# Object-Neighborhood Context for {name}',
        '',
        '## Approach',
        '',
        'In object-neighborhood matching, a group of adjacent functions is',
        'compiled together as a single compilation unit (.c → .o). This',
        'gives the compiler more freedom for register allocation and produces',
        'output that closely matches the original binary, where these functions',
        'were likely part of the same source file.',
        '',
        f'## Cluster ({len(cluster)} functions)',
        '',
    ]
    for i, (a, n, s, sec) in enumerate(cluster):
        marker = ' ← **target**' if n == name and a == addr else ''
        lines.append(f'{i+1}. `{n}` @ `0x{a:08X}` (0x{s:X} bytes){marker}')
    lines.append('')
    lines.append(f'**Module:** `{module or "unknown"}`')
    lines.append('')
    if mac_lines:
        lines.append(f'**Mac evidence for target:**{mac_lines}')
        lines.append('')
    lines.append('## Types')
    lines.append('')
    lines.append(struct_defs_text)
    return '\n'.join(lines)


def gen_combined_related_symbols(cluster):
    """Generate cross-references for the entire cluster."""
    all_callers = {}
    all_callees = {}
    all_mac = {}
    all_data_refs = set()
    for addr, name, size, _ in cluster:
        for cn, ca in callers_of(addr):
            all_callers.setdefault(ca, []).append((cn, ca))
        for cn, ca in callees_of(addr):
            all_callees.setdefault(ca, []).append((cn, ca))
        mh = find_mac_hits(name)
        if not mh:
            mh = find_mac_hits(f"fn_{addr:08X}")
        for src, clean, dem in mh:
            all_mac[clean] = (src, clean, dem)
        asm_text = extract_asm(addr, size, name)
        if asm_text:
            all_data_refs.update(find_data_refs(asm_text))

    lines = [f'# Related Symbols for Cluster\n']

    if all_callers:
        lines.append(f'## Callers ({sum(len(v) for v in all_callers.values())})\n')
        for ca, items in sorted(all_callers.items()):
            names = [cn for cn, _ in items]
            lines.append(f"- 0x{int(ca, 16):08X}: {', '.join(sorted(names))}")
        lines.append('')

    if all_callees:
        lines.append(f'## Callees ({sum(len(v) for v in all_callees.values())})\n')
        for ca, items in sorted(all_callees.items()):
            names = [cn for cn, _ in items]
            lines.append(f"- 0x{int(ca, 16):08X}: {', '.join(sorted(names))}")
        lines.append('')

    if all_mac:
        lines.append(f'## Mac symbol hits ({len(all_mac)})\n')
        for clean in sorted(all_mac):
            src, _, dem = all_mac[clean]
            line = f"- [{src}] `{clean}`"
            if dem:
                line += f"\n  → `{dem}`"
            lines.append(line)
        lines.append('')

    if all_data_refs:
        lines.append(f'## Data references\n')
        for ref in sorted(all_data_refs):
            lines.append(f"- `{ref}`")
        lines.append('')

    return '\n'.join(lines)


def gen_object_verify(name, addr, cluster):
    module = None
    for mod, (lo, hi) in MODULES.items():
        if lo <= addr < hi:
            module = mod
            break
    cluster_names = [n for _, n, _, _ in cluster]
    obj_path = f"build/GL5E4F/obj/{module or 'auto'}/cluster_{name}.o"
    cluster_list = "\n#   ".join(cluster_names)
    return f"""#!/usr/bin/env bash
# Object-neighborhood verification for {name} (0x{addr:08X})
#
# This task uses object-neighborhood matching — all {len(cluster)} functions
# in the cluster are compiled together as a single .o file.
#
# Functions in cluster:
#   {cluster_list}
#
# Steps:
#   1. Fill in all function stubs in candidate.c
#      (you may also split into separate files under src/<module>/)
#   2. Add the object to configure.py:
#        Object('{module or '???'}', 'cluster_{name}.c', ...)
#   3. Reconfigure and build:
#        python3 configure.py && ninja
#   4. Check the objdiff report:
#        ninja build/GL5E4F/report.json
#        objdiff-cli report generate --config objdiff.json
#   5. Check each function in the cluster for 100% match.
#
echo "Run: ninja build/GL5E4F/report.json"
"""


# ── task index ───────────────────────────────────────────────────────

def gen_index(tasks, out_root):
    """Write tasks/index.md with a sorted table of all task packs."""
    if not tasks:
        task_dirs = sorted(out_root.iterdir()) if out_root.exists() else []
    else:
        task_dirs = tasks

    rows = []
    for td in task_dirs:
        if not td.is_dir():
            continue
        # Read status
        status_file = td / "status.md"
        if status_file.exists():
            m = re.search(r'^# Status:\s*(\S+)', status_file.read_text(), re.M)
            status = m.group(1) if m else "TODO"
        else:
            status = "TODO"

        # Read prompt for metadata
        prompt_file = td / "prompt.md"
        size_str = ""
        module = ""
        addr = ""
        if prompt_file.exists():
            text = prompt_file.read_text()
            m = re.search(r'\*\*Size:\*\*\s*`(0x[0-9A-Fa-f]+)`', text)
            if m: size_str = m.group(1)
            m = re.search(r'\*\*Source module:\*\*\s*`([^`]+)`', text)
            if m: module = m.group(1)
            m = re.search(r'\*\*Address:\*\*\s*`([^`]+)`', text)
            if m: addr = m.group(1)

        name = td.name
        # Score from asm analysis
        asm_file = td / "asm.s"
        if asm_file.exists():
            asm_text = asm_file.read_text()
            has_bctr = "bctr" in asm_text
            n_callees = asm_text.count("bl ")
            inst_count = len(asm_text.splitlines())
            branch_count = len(re.findall(r'\bb\s', asm_text))
        else:
            has_bctr = False
            n_callees = 0
            inst_count = 0
            branch_count = 0

        # Recompute a rough score from the files on disk
        score = 0
        if not name.startswith("fn_"):
            score += 2
        if any(name.startswith(p) for p in NU2_PREFIXES):
            score += 1
        if not has_bctr:
            score += 2

        # Read mac hits from related_symbols.md
        rs_file = td / "related_symbols.md"
        has_struct_mac = False
        if rs_file.exists():
            if re.search(r'nudathdr_s|nuanimkey_s|nuvec_s', rs_file.read_text()):
                has_struct_mac = True

        if has_struct_mac:
            score += 2

        if n_callees <= 2:
            score += 2
        elif n_callees <= 5:
            score += 1
        if inst_count > 0 and branch_count / inst_count < 0.1:
            score += 1
        if size_str:
            try:
                sz = int(size_str, 16)
                if sz <= 0x100: score += 3
                elif sz <= 0x400: score += 2
                else: score += 1
            except ValueError:
                pass

        label = priority_label(score)
        rows.append((score, name, module, size_str or "?", status, label))

    rows.sort(key=lambda x: (-x[0], x[1]))

    lines = [
        "# Task Index",
        "",
        "Priority scores: higher = easier to match first.",
        "",
        "| Score | Label | Task | Module | Size | Status |",
        "|-------|-------|------|--------|------|--------|",
    ]
    for score, name, module, size, status, label in rows:
        lines.append(f"| {score} | {label} | `{name}` | {module} | {size} | {status} |")
    lines.append("")

    return "\n".join(lines)


# ── main ─────────────────────────────────────────────────────────────

def generate_object_pack(name, addr, size, out_root, mac_hits, callee_list, asm,
                          status="TODO", sec="text", context=2):
    """Generate an object-neighborhood task pack (cluster-based matching).

    Instead of generating a task for a single function, this generates
    a pack for the function plus its N nearest address neighbors, so
    they can be compiled together as a single .o for correct register
    allocation.
    """
    if asm is None:
        asm = extract_asm(addr, size, name)
    if asm is None:
        print("SKIP (asm not found)")
        return

    cluster = find_neighbors(name, addr, context)
    if cluster is None:
        print("SKIP (could not find position in symbol table)")
        return

    task_dir = out_root / name
    task_dir.mkdir(parents=True, exist_ok=True)

    struct_text, module = extract_struct_defs(name, addr)
    combined_asm = extract_asm_for_cluster(cluster)

    (task_dir / "prompt.md").write_text(
        gen_prompt(name, addr, size, sec, module, mac_hits or [], struct_text))
    (task_dir / "object_context.md").write_text(
        gen_object_context(name, addr, cluster, module, struct_text))
    (task_dir / "asm.s").write_text(asm or "")
    (task_dir / "combined.s").write_text(combined_asm or "")
    (task_dir / "candidate.c").write_text(gen_candidate_c(cluster))
    (task_dir / "related_symbols.md").write_text(
        gen_combined_related_symbols(cluster))
    (task_dir / "verify.sh").write_text(
        gen_object_verify(name, addr, cluster))
    (task_dir / "verify.sh").chmod(0o755)
    (task_dir / "status.md").write_text(
        gen_status(name, status))

    print(f"OK [{len(cluster)} functions, ctx={context}]")


def main():
    parser = argparse.ArgumentParser(
        description="Generate decomp task pack for a function (or batch for a module)."
    )
    parser.add_argument("target", nargs="?", default="", help="Function name, fn_ADDR, hex address, or module name")
    parser.add_argument("--out-dir", default="tasks",
                        help="Output directory (default: tasks/)")
    parser.add_argument("--top", type=int, default=0,
                        help="For module targets, generate top N highest-scored (default: all)")
    parser.add_argument("--all", action="store_true",
                        help="For module targets, generate for all unnamed")
    parser.add_argument("--status", default="TODO",
                        choices=["TODO", "IN_PROGRESS", "MATCHED", "BLOCKED"],
                        help="Initial status for generated tasks (default: TODO)")
    parser.add_argument("--index", action="store_true",
                        help="Re-index existing task directories without regenerating")
    parser.add_argument("--object", "-O", action="store_true",
                        help="Generate object-neighborhood pack (cluster-based)")
    parser.add_argument("--context", type=int, default=2,
                        help="Number of adjacent functions on each side (default: 2)")
    args = parser.parse_args()

    target = args.target
    out_root = ROOT / args.out_dir

    # Re-index mode (no target needed)
    if args.index:
        if not out_root.exists():
            print(f"Error: {out_root} does not exist; nothing to index")
            return
        index_text = gen_index([], out_root)
        index_path = out_root / "index.md"
        index_path.write_text(index_text)
        print(f"Index written: {index_path}")
        return

    # Module batch mode
    if target.lower() in MODULES:
        mod_lo, mod_hi = MODULES[target.lower()]
        funcs = load_symtab()
        in_range = [f for f in funcs if mod_lo <= f[0] < mod_hi]
        unnamed = [(a, n, s) for a, n, s, _ in in_range if n.startswith("fn_") and s > 0]
        if not unnamed:
            print(f"No unnamed functions in {target}")
            return

        # Score and sort unnamed by priority
        scored = []
        for addr, name, size in unnamed:
            mac_hits = find_mac_hits(name)
            if not mac_hits:
                mac_hits = find_mac_hits(f"fn_{addr:08X}")
            callee_list = callees_of(addr)
            asm = extract_asm(addr, size)
            score = priority_score(name, addr, size, mac_hits, callee_list, asm or "")
            scored.append((score, addr, name, size, mac_hits, callee_list, asm))

        scored.sort(key=lambda x: (-x[0], x[2]))

        count = len(scored)
        if not args.all and args.top:
            count = min(args.top, count)

        batch_mode = True
        mode_str = "object-neighborhood " if args.object else ""
        print(f"Generating {count} {mode_str}tasks for {target} (sorted by priority)...")
        for item in scored[:count]:
            score, addr, name, size, mac_hits, callee_list, asm = item
            display_name = name
            label = priority_label(score)
            print(f"  [{score:2d} {label}] {display_name} @ 0x{addr:08X}...", end=" ", flush=True)
            if args.object:
                generate_object_pack(name, addr, size, out_root, mac_hits, callee_list,
                                     asm, args.status, context=args.context)
            else:
                generate_pack(name, addr, size, out_root, mac_hits, callee_list, asm, args.status)
                print("OK")

        # Write index
        index_text = gen_index([], out_root)
        if index_text:
            (out_root / "index.md").write_text(index_text)
        print(f"\nDone — {count} task packs written to {out_root}/ ({out_root}/index.md updated)")
        return

    # Single function mode
    if args.index:
        return  # already handled

    resolved = resolve_target(target)
    if not resolved:
        print(f"Error: '{target}' not found in symbols.txt")
        sys.exit(1)

    name, addr, size, sec = resolved
    mac_hits = find_mac_hits(name)
    if not mac_hits:
        mac_hits = find_mac_hits(f"fn_{addr:08X}")
    callee_list = callees_of(addr)
    asm = extract_asm(addr, size, name)

    if args.object:
        generate_object_pack(name, addr, size, out_root, mac_hits, callee_list,
                             asm, args.status, sec, context=args.context)
    else:
        generate_pack(name, addr, size, out_root, mac_hits, callee_list, asm, args.status, sec)

    # Write index
    index_text = gen_index([], out_root)
    if index_text:
        (out_root / "index.md").write_text(index_text)

    print(f"Done — wrote task pack to {out_root}/{name}/")


def generate_pack(name, addr, size, out_root, mac_hits=None, callee_list=None, asm=None, status="TODO", sec="text"):
    display_name = name

    print(f"{display_name} @ 0x{addr:08X}...", end=" ", flush=True)

    if asm is None:
        asm = extract_asm(addr, size, display_name)
    if asm is None:
        print("SKIP (asm not found)")
        return

    task_dir = out_root / display_name
    task_dir.mkdir(parents=True, exist_ok=True)

    if mac_hits is None:
        mac_hits = find_mac_hits(display_name)
        if not mac_hits:
            mac_hits = find_mac_hits(f"fn_{addr:08X}")

    if callee_list is None:
        callee_list = callees_of(addr)

    struct_text, module = extract_struct_defs(display_name, addr)
    callers = callers_of(addr)
    data_refs = find_data_refs(asm)

    (task_dir / "prompt.md").write_text(
        gen_prompt(display_name, addr, size, sec, module, mac_hits, struct_text))
    (task_dir / "context.md").write_text(
        gen_context(display_name, addr, struct_text, module))
    (task_dir / "asm.s").write_text(asm)
    (task_dir / "related_symbols.md").write_text(
        gen_related_symbols(display_name, addr, callers, callee_list, mac_hits, data_refs))
    (task_dir / "verify.sh").write_text(
        gen_verify(display_name, addr, size, module))
    (task_dir / "verify.sh").chmod(0o755)
    (task_dir / "status.md").write_text(
        gen_status(display_name, status))


if __name__ == "__main__":
    main()
