#!/usr/bin/env python3
"""Import one dolsdk2004 unit into the build: source, headers, split, config, names, proof.

Handles the common shape, a unit whose retail footprint is a single .text range plus
optional file-scope .bss/.sbss statics (built with external_bss). Units that also own
initialised data (.data, .rodata, .sdata, .sdata2) are reported as manual work.

    python3 tools/sdk_import_unit.py ax/AXAux           # plan only, writes nothing
    python3 tools/sdk_import_unit.py ax/AXAux --apply   # import, rebuild and prove

--apply does what a hand import does:
  1. copy the donor source (and donor-local headers) under src/dolphin/
  2. add the .text split to splits.txt and an SDKObject to configure.py, both in address order
  3. build the split target object, pair its relocations with the donor's to find the
     donor names for unnamed callees, statics and functions, and rename them in symbols.txt
     (names listed in docs/symbol_donors/rejected_renames.tsv are never applied)
  4. rebuild, then run verify_unit.py --functions and the linked-image comparison
and prints the docs/sdk_import.md table row. Review `git diff` afterwards; if a step
fails the tree is left as-is for inspection. The retail DOL and the oracle are never
edited, and a symbol rename alone is not progress.
"""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import sdk_probe  # noqa: E402
import verify_fn as vf  # noqa: E402
from rejected_renames import load_rejected  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
SPLITS = ROOT / "config/GL5E4F/splits.txt"
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
CONFIGURE = ROOT / "configure.py"
BSS_SECTIONS = {".bss", ".sbss"}
UNNAMED = re.compile(r"^(?:fn|lbl|jumptable)_[0-9A-F]{8}$")
LOCAL_SUFFIX = re.compile(r"^(.+)_([0-9A-F]{8})$")
INCLUDE = re.compile(r'^\s*#\s*include\s+([<"])([^>"]+)[>"]', re.M)
STT_SECTION = 3


class ImportError_(Exception):
    pass


# ------------------------------------------------------------------ text edits

def split_blocks(text: str) -> list[tuple[str, int, int | None]]:
    """(header line, first line index, .text start or None) for every unit block."""
    lines = text.splitlines()
    blocks = []
    i = 0
    while i < len(lines):
        line = lines[i]
        if line and not line[0].isspace() and line.endswith(":") and line != "Sections:":
            start = None
            j = i + 1
            while j < len(lines) and lines[j].strip():
                m = re.match(r"\s*\.text\s+start:0x([0-9A-Fa-f]+)", lines[j])
                if m:
                    start = int(m[1], 16)
                j += 1
            blocks.append((line, i, start))
            i = j
        else:
            i += 1
    return blocks


def insert_split_block(text: str, unit: str, start: int, end: int) -> str:
    """Insert `dolphin/<unit>.c` with its .text range after the block with the next lower start."""
    name = f"dolphin/{unit}.c:"
    if any(h == name for h, _, _ in split_blocks(text)):
        raise ImportError_(f"splits.txt already has {name}")
    lines = text.splitlines()
    before = [(s, i) for _, i, s in split_blocks(text) if s is not None and s < start]
    if not before:
        raise ImportError_("no earlier split block to insert after")
    anchor_line = max(before)[1]
    j = anchor_line + 1
    while j < len(lines) and lines[j].strip():
        j += 1
    block = [name, f"\t.text       start:0x{start:08X} end:0x{end:08X}"]
    if j == len(lines):
        lines += [""] + block
    else:
        lines[j + 1:j + 1] = block + [""]
    return "\n".join(lines) + "\n"


def _balanced(text: str, open_at: int) -> int:
    depth = 0
    for k in range(open_at, len(text)):
        if text[k] == "(":
            depth += 1
        elif text[k] == ")":
            depth -= 1
            if depth == 0:
                return k
    raise ImportError_("unbalanced parentheses in configure.py")


def insert_sdk_object(text: str, splits_text: str, unit: str, external_bss: bool) -> str:
    """Add SDKObject(unit) to its DolphinLib (or create the lib), in retail address order."""
    lib = unit.split("/")[0]
    starts = {h[:-1]: s for h, _, s in split_blocks(splits_text) if s is not None}
    mine = starts.get(f"dolphin/{unit}.c")
    if mine is None:
        raise ImportError_("insert the split before configure.py")
    entry = f'SDKObject("{unit}", Matching' + (", external_bss=True)" if external_bss else ")")
    libs = [(m, _balanced(text, m.start() + len("DolphinLib")))
            for m in re.finditer(r'DolphinLib\("(\w+)", \[', text)]
    for m, close in libs:
        if m.group(1) != lib:
            continue
        body = text[m.start():close + 1]
        objs = list(re.finditer(r'SDKObject\("([\w/]+)"[^)]*\)', body))
        after = None
        for o in objs:
            other = starts.get(f"dolphin/{o.group(1)}.c")
            if other is not None and other > mine:
                after = o
                break
        if after is not None:
            at = m.start() + after.start()
            return text[:at] + entry + ", " + text[at:] if "\n" not in body else \
                text[:at] + entry + ",\n" + " " * _indent(text, at) + text[at:]
        last = objs[-1]
        at = m.start() + last.end()
        return text[:at] + ", " + entry + text[at:] if "\n" not in body else \
            text[:at] + ",\n" + " " * _indent(text, at) + entry + text[at:]
    line = f'    DolphinLib("{lib}", [{entry}]),\n'
    first_start = []
    for m, close in libs:
        found = [starts[f"dolphin/{o.group(1)}.c"] for o in
                 re.finditer(r'SDKObject\("([\w/]+)"', text[m.start():close])
                 if f"dolphin/{o.group(1)}.c" in starts]
        first_start.append((min(found) if found else 0, m, close))
    later = [t for t in first_start if t[0] > mine]
    if later:
        at = text.rfind("\n", 0, min(later, key=lambda t: t[0])[1].start()) + 1
    elif first_start:
        close = max(first_start, key=lambda t: t[0])[2]
        at = text.index("\n", close) + 1
    else:
        raise ImportError_("no DolphinLib in configure.py to insert beside")
    return text[:at] + line + text[at:]


def _indent(text: str, at: int) -> int:
    line = text[text.rfind("\n", 0, at) + 1:at]
    return len(line) - len(line.lstrip())


def add_include_flag(text: str, lib: str) -> str:
    flag = f'"-i src/dolphin/{lib}"'
    if flag in text:
        return text
    last = list(re.finditer(r'"-i src/dolphin/\w+",', text))
    if not last:
        raise ImportError_("no -i src/dolphin/<lib> flag to extend in configure.py")
    at = last[-1].end()
    return text[:at] + f" {flag}," + text[at:]


def rename_symbols(text: str, renames: dict[str, str]) -> str:
    out = []
    for line in text.splitlines():
        m = re.match(r"^(\S+)(\s*=.*)$", line)
        if m and m[1] in renames:
            line = renames[m[1]] + m[2]
        out.append(line)
    return "\n".join(out) + "\n"


def donor_includes(source: str) -> list[tuple[str, str]]:
    return [(m[1], m[2]) for m in INCLUDE.finditer(source)]


# ------------------------------------------------------------- relocation pairing

def logical(name: str, symbols: vf.SymbolTable) -> str:
    """Undo DTK's NAME_<address> renaming of local symbols."""
    m = LOCAL_SUFFIX.match(name)
    if m and name not in symbols.entries:
        e = symbols.entries.get(m[1])
        if e and e.addr == int(m[2], 16):
            return m[1]
    return name


def function_refs(elf, sym) -> list[tuple[int, int, str, int]]:
    """(offset in function, type, symbol name, addend) for each relocation inside sym."""
    refs = []
    size = elf.symbol_extent(sym)
    for rel in sorted(elf.relocs.get(sym.shndx, []), key=lambda r: r.offset):
        if not sym.value <= rel.offset < sym.value + size:
            continue
        target = elf.symbols[rel.sym] if rel.sym < len(elf.symbols) else None
        name, addend = "", rel.addend
        if target is not None:
            if target.name.startswith("."):
                pass
            elif target.sym_type == STT_SECTION:
                named = [s for s in elf.symbols if s.shndx == target.shndx and s.name
                         and s.sym_type != STT_SECTION and s.value == rel.addend]
                if len(named) == 1:
                    name, addend = named[0].name, 0
            else:
                name = target.name
        # MWCC puts 16-bit relocations on the halfword, DTK on the word start.
        refs.append(((rel.offset - sym.value) & ~3, rel.rtype, name, addend))
    return refs


@dataclass
class Pairing:
    renames: dict[str, str] = field(default_factory=dict)   # retail name -> donor name
    conflicts: list[str] = field(default_factory=list)
    notes: list[str] = field(default_factory=list)


def pair_refs(mine, target, anchors: dict[str, str], symbols: vf.SymbolTable,
              fn: str, out: Pairing) -> None:
    """Pair two functions' relocations; propose donor names for unnamed target symbols."""
    if [(o, t) for o, t, _, _ in mine] != [(o, t) for o, t, _, _ in target]:
        out.conflicts.append(f"{fn}: relocation layouts differ; names not paired")
        return
    for (_, _, mname, madd), (_, _, tname, tadd) in zip(mine, target):
        if not mname or not tname:
            continue
        tname = logical(tname, symbols)
        if mname == tname or anchors.get(mname) == tname:
            continue
        if madd != tadd:
            out.conflicts.append(f"{fn}: {mname}+{madd:#x} vs {tname}+{tadd:#x}: addends differ")
        elif UNNAMED.match(tname):
            if out.renames.setdefault(tname, mname) != mname:
                out.conflicts.append(f"{fn}: {tname} paired with both {out.renames[tname]} and {mname}")
        else:
            out.conflicts.append(f"{fn}: donor {mname} vs retail {tname}: both named")


def check_renames(pairing: Pairing, symbols: vf.SymbolTable, rejected: dict[int, set[str]]) -> dict[str, str]:
    """Drop proposals that clash with existing names or the rejected list."""
    accepted: dict[str, str] = {}
    seen: dict[str, str] = {}
    for old, new in pairing.renames.items():
        entry = symbols.entries.get(old)
        if entry is None:
            pairing.conflicts.append(f"{old} -> {new}: {old} is not in symbols.txt")
        elif new in symbols.entries or new in seen:
            pairing.conflicts.append(f"{old} -> {new}: name already used in symbols.txt")
        elif new in rejected.get(entry.addr, set()):
            pairing.conflicts.append(f"{old} -> {new}: rejected for 0x{entry.addr:08X}")
        elif not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", new):
            pairing.conflicts.append(f"{old} -> {new}: not a valid identifier")
        else:
            accepted[old] = new
            seen[new] = old
    return accepted


# ---------------------------------------------------------------------- planning

@dataclass
class Plan:
    unit: str
    lib: str
    start: int = 0
    end: int = 0
    anchors: dict[str, tuple[str, str]] = field(default_factory=dict)
    dead: list[str] = field(default_factory=list)
    retail_funcs: int = 0
    external_bss: bool = False
    manual_sections: dict[str, int] = field(default_factory=dict)
    headers: list[tuple[Path, Path]] = field(default_factory=list)
    new_include_dir: bool = False
    errors: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)


def configure_flag_dirs(text: str) -> set[str]:
    return set(re.findall(r'"-i src/dolphin/(\w+)"', text))


def plan_headers(plan: Plan, sdk: Path, source: str, flag_dirs: set[str]) -> None:
    roots = [ROOT / "include", ROOT / "include/dolphin", ROOT / "include/dolphin/libc"]
    local_dirs = [ROOT / "src/dolphin" / d for d in sorted(flag_dirs)]
    for kind, name in donor_includes(source):
        if any((r / name).is_file() for r in roots):
            continue
        if kind == '"' and any((d / name).is_file() for d in local_dirs):
            continue
        donors = sorted((sdk / "src").glob(f"*/{name}"))
        if kind == '"' and donors:
            plan.headers.append((donors[0], ROOT / "src/dolphin" / plan.lib / name))
        else:
            plan.errors.append(f"include {kind}{name}{'>' if kind == '<' else kind} not found in include/ or the donor")
    if plan.headers and plan.lib not in flag_dirs:
        plan.new_include_dir = True


def make_plan(unit: str, sdk: Path, tmp: Path) -> tuple[Plan, Path]:
    plan = Plan(unit, unit.split("/")[0])
    source = sdk / "src" / f"{unit}.c"
    if not source.is_file():
        raise ImportError_(f"no donor source at {source}")
    if sdk_probe.is_imported(unit) or (ROOT / "src/dolphin" / f"{unit}.c").exists():
        raise ImportError_(f"{unit} is already imported")
    obj = tmp / (unit.replace("/", "__") + ".o")
    err = sdk_probe.compile_unit(sdk, unit, obj)
    if err:
        raise ImportError_(f"donor does not compile:\n{err}")
    elf = vf.Elf(obj)
    symbols = vf.SymbolTable()
    dol = vf.load_dol(vf.DOL_PATH, [])
    funcs = [f for f in elf.function_symbols() if f.size]
    anchors = sdk_probe.anchor_unit(elf, funcs, symbols, dol)
    plan.anchors = anchors
    plan.dead = [f.name for f in funcs if f.name not in anchors]
    if not anchors:
        plan.errors.append("no donor function could be anchored to a retail address")
        return plan, obj

    retail = sorted({(symbols.entries[r].addr, r, symbols.entries[r].size) for r, _ in anchors.values()})
    plan.start = min(a for a, _, _ in retail)
    plan.end = max(a + s for a, _, s in retail)
    anchored = {r for _, r, _ in retail}
    inside = [n for n, e in symbols.entries.items() if e.is_function and e.section in vf.CODE_SECTIONS
              and e.size and plan.start <= e.addr < plan.end]
    plan.retail_funcs = len(inside)
    unmatched = sorted(set(inside) - anchored)
    if unmatched:
        plan.errors.append(f"{len(unmatched)} retail functions inside the range are not matched by the donor: "
                           + ", ".join(unmatched[:8]))
    for addr in (plan.start, plan.end - 1):
        other = vf.lookup_unit(addr)
        if other:
            plan.errors.append(f"0x{addr:08X} is already covered by split {other}")
    if plan.end - plan.start != sum(s for _, _, s in retail) + 0 and not unmatched:
        plan.warnings.append("range has gaps or padding between functions")

    for sec in elf.sections:
        if sec.flags & 2 and sec.size and not sec.is_code:
            if sec.name in BSS_SECTIONS:
                plan.external_bss = True
            else:
                plan.manual_sections[sec.name] = sec.size
    if plan.manual_sections:
        plan.errors.append("unit owns initialised data needing split ranges by hand: "
                           + ", ".join(f"{n} {s:#x}" for n, s in plan.manual_sections.items()))
    plan_headers(plan, sdk, source.read_text(errors="replace"), configure_flag_dirs(CONFIGURE.read_text()))
    return plan, obj


def print_plan(plan: Plan) -> None:
    print(f"unit            {plan.unit}")
    if plan.start:
        print(f".text           {plan.start:08X}..{plan.end:08X} ({plan.end - plan.start:#x} bytes, "
              f"{plan.retail_funcs} retail functions)")
    how: dict[str, int] = {}
    for _, h in plan.anchors.values():
        how[h.split(" ")[0]] = how.get(h.split(" ")[0], 0) + 1
    print(f"anchored        {len(plan.anchors)} ({', '.join(f'{k} {v}' for k, v in sorted(how.items()))})")
    print(f"dead-stripped   {', '.join(plan.dead) or 'none'}")
    print(f"external_bss    {plan.external_bss}")
    for src, dst in plan.headers:
        print(f"copy header     {src.name} -> {dst.relative_to(ROOT)}")
    if plan.new_include_dir:
        print(f"new -i flag     src/dolphin/{plan.lib}")
    for w in plan.warnings:
        print(f"warning: {w}")
    for e in plan.errors:
        print(f"error: {e}")


# ------------------------------------------------------------------------ apply

def run(cmd: list[str], what: str) -> str:
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        tail = "\n".join((r.stdout + r.stderr).strip().splitlines()[-30:])
        raise ImportError_(f"{what} failed (exit {r.returncode}):\n{tail}")
    return r.stdout


def compute_renames(plan: Plan, raw_obj: Path, target_obj: Path) -> tuple[dict[str, str], Pairing]:
    symbols = vf.SymbolTable()
    mine, target = vf.Elf(raw_obj), vf.Elf(target_obj)
    pairing = Pairing()
    mapping = {c: r for c, (r, _) in plan.anchors.items()}
    for compiled, retail in mapping.items():
        if retail != compiled and UNNAMED.match(retail):
            if pairing.renames.setdefault(retail, compiled) != compiled:
                pairing.conflicts.append(f"{retail} claimed by {pairing.renames[retail]} and {compiled}")
        elif retail != compiled:
            pairing.notes.append(f"function {compiled} stays {retail} in symbols.txt")
        msym, tsym = mine.find_function(compiled), target.find_function(retail)
        if msym is None or tsym is None:
            pairing.conflicts.append(f"{compiled}: not found in both objects")
            continue
        pair_refs(function_refs(mine, msym), function_refs(target, tsym), mapping, symbols, compiled, pairing)
    return check_renames(pairing, symbols, load_rejected()), pairing


def apply(plan: Plan, sdk: Path, raw_obj: Path, verify: bool) -> list[str]:
    unit, lib = plan.unit, plan.lib
    dest = ROOT / "src/dolphin" / f"{unit}.c"
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(sdk / "src" / f"{unit}.c", dest)
    for src, dst in plan.headers:
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(src, dst)
    splits = insert_split_block(SPLITS.read_text(), unit, plan.start, plan.end)
    SPLITS.write_text(splits)
    config = CONFIGURE.read_text()
    if plan.new_include_dir:
        config = add_include_flag(config, lib)
    config = insert_sdk_object(config, splits, unit, plan.external_bss)
    CONFIGURE.write_text(config)

    target_obj = ROOT / "build/GL5E4F/obj" / f"dolphin/{unit}.o"
    run([sys.executable, "configure.py"], "configure.py")
    run(["ninja", str(target_obj.relative_to(ROOT))], "split target object")
    renames, pairing = compute_renames(plan, raw_obj, target_obj)
    log = [f"rename {old} -> {new}" for old, new in sorted(renames.items())]
    if renames:
        for old in renames:
            for path in list((ROOT / "src").rglob("*.[ch]")):
                if re.search(rf"\b{re.escape(old)}\b", path.read_text(errors="replace")):
                    log.append(f"warning: {old} is also used in {path.relative_to(ROOT)}")
        SYMBOLS.write_text(rename_symbols(SYMBOLS.read_text(), renames))
    log += [f"note: {n}" for n in pairing.notes] + [f"manual: {c}" for c in dict.fromkeys(pairing.conflicts)]
    if not verify:
        return log
    try:
        run(["ninja", "build/GL5E4F/main.dol"], "full build")
        out = run([sys.executable, "tools/verify_unit.py", f"dolphin/{unit}", "--build", "--functions"], "verify")
        log.append(out.strip().splitlines()[-1])
        run([sys.executable, "tools/dol_compare.py", "build/GL5E4F/main.dol", "orig/GL5E4F/sys/main.dol",
             "--linked-units"], "linked-image comparison")
    except ImportError_ as exc:
        # The renames and conflicts explain most failures; do not lose them with the error.
        raise ImportError_(f"{exc}\n\nimport log before the failure (files are left modified):\n" + "\n".join(log)) from exc
    log.append("linked image: zero differing bytes in linked units")
    return log


def doc_row(plan: Plan) -> str:
    return (f"| `{plan.unit}` | `{plan.start:08X}..{plan.end:08X}` | {plan.retail_funcs} | "
            f"{plan.end - plan.start:,} | 0 |")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("unit", help="donor unit relative to src/, e.g. ax/AXAux")
    ap.add_argument("--apply", action="store_true", help="write files, rebuild and verify")
    ap.add_argument("--no-verify", action="store_true", help="with --apply, stop after renaming")
    ap.add_argument("--sdk", type=Path, default=None, help="dolsdk2004 checkout (default: shared cache)")
    args = ap.parse_args(argv)
    sdk = (args.sdk or sdk_probe.default_sdk()).resolve()
    if not (sdk / "src").is_dir():
        print(f"error: no dolsdk2004 checkout at {sdk}; run tools/fetch_dolsdk.sh", file=sys.stderr)
        return 2
    try:
        with tempfile.TemporaryDirectory(prefix="sdk_import_") as d:
            plan, raw_obj = make_plan(args.unit, sdk, Path(d))
            print_plan(plan)
            if plan.errors:
                return 1
            if not args.apply:
                print("\nplan only; rerun with --apply to import")
                return 0
            log = apply(plan, sdk, raw_obj, not args.no_verify)
    except (ImportError_, vf.ToolError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    print()
    for line in log:
        print(line)
    print("\nsdk_import.md row:\n" + doc_row(plan))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
