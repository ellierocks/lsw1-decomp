"""Symbol-table decisions for tools/sdk_import_unit.py: name conflicts and .bss/.sbss granularity.

Pure functions over symbols.txt data so the importer's rules can be tested without a build.

Name conflicts: the retail address already has a different SDK name than the donor's. A
proposal is applied only on code evidence (same size, identical words outside relocations),
never for an address/name pair in docs/symbol_donors/rejected_renames.tsv, and only when the
donor name is free or being vacated by another accepted proposal.

Granularity: DTK splits a u64 into two 4-byte labels and merges neighbouring arrays into one
symbol, because it only sees what the code touches. The donor object gives the real extents.
A merge or split is planned only when the donor objects tile the retail symbols exactly.
"""

from __future__ import annotations

import re
from collections.abc import Callable
from dataclasses import dataclass, field

import verify_fn as vf

UNNAMED = re.compile(r"^(?:fn|lbl|jumptable)_[0-9A-F]{8}$")
IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
SYMBOL_LINE = re.compile(r"^(\S+)(\s*=\s*)(\.\w+):0x([0-9A-Fa-f]+);(.*)$")


# --------------------------------------------------------------- name conflicts

@dataclass
class Proposal:
    addr: int
    size: int
    retail: str
    donor: str
    evidence: str                 # how the donor and retail code were compared
    identical: bool               # same size and identical words outside relocations
    matches: Callable[[str], bool] | None = field(default=None, repr=False, compare=False)
    status: str = "ok"            # ok | rejected | blocked | unproven
    detail: str = ""
    demote: str | None = None     # placeholder for the wrongly named function that holds `donor`

    def describe(self) -> str:
        head = f"retail {self.retail} at {self.addr:08X} vs donor {self.donor}, "
        if self.identical:
            head += f"same size ({self.size:#x}), code identical"
        else:
            head += f"{self.evidence}"
        tail = {"ok": "", "rejected": " [rejected]", "blocked": " [blocked]", "unproven": " [unproven]"}[self.status]
        return head + tail + (f": {self.detail}" if self.detail else "")


def resolve_proposals(props: list[Proposal], symbols: vf.SymbolTable, rejected: dict[int, set[str]],
                      text_range: tuple[int, int] | None = None,
                      owner: Callable[[int], str | None] | None = None) -> None:
    """Set each proposal's status. Chains and swaps are fine: a donor name held by a retail
    symbol that another accepted proposal renames away counts as free. `owner(addr)` names the
    split unit an address already belongs to; a function there is a different, real SDK
    function that merely shares the name (two statics called OnReset), so it is not demoted."""
    for p in props:
        if not p.identical:
            p.status, p.detail = "unproven", p.detail or "code differs, not proposed"
        elif p.donor in rejected.get(p.addr, set()):
            p.status, p.detail = "rejected", f"{p.donor} is rejected for 0x{p.addr:08X} in rejected_renames.tsv"
        elif not IDENT.fullmatch(p.donor):
            p.status, p.detail = "blocked", f"{p.donor} is not a valid identifier"
    live = [p for p in props if p.status == "ok"]
    while True:
        moving = {p.retail for p in live}
        seen: dict[str, Proposal] = {}
        dropped = False
        for p in live:
            p.demote, p.detail = None, ""
            holder = symbols.entries.get(p.donor)
            reason = ""
            if p.donor in seen:
                reason = f"{p.donor} is also proposed for {seen[p.donor].retail}"
            elif holder is not None and holder.addr == p.addr:
                reason = f"{p.donor} already names this address"
            elif holder is not None and p.donor not in moving:
                p.demote, reason = demotion(p, holder, symbols, text_range, owner)
                if p.demote:
                    p.detail = (f"{p.donor} now names 0x{holder.addr:08X} ({holder.size:#x} bytes, different "
                                f"code); that symbol becomes {p.demote}")
            if reason:
                p.status, p.detail = "blocked", reason
                dropped = True
            else:
                seen[p.donor] = p
        if not dropped:
            return
        live = [p for p in live if p.status == "ok"]


def placeholder(holder: vf.SymEntry) -> str:
    return f"{'fn' if holder.is_function else 'lbl'}_{holder.addr:08X}"


def demotion(p: Proposal, holder: vf.SymEntry, symbols: vf.SymbolTable,
             text_range: tuple[int, int] | None,
             owner: Callable[[int], str | None] | None = None) -> tuple[str | None, str]:
    """(placeholder, "") when the holder is a function outside the unit whose code is not the
    donor's, i.e. a wrong name on a game function; otherwise (None, reason it blocks)."""
    base = f"{p.donor} is already used at 0x{holder.addr:08X} (size {holder.size:#x})"
    if not holder.is_function or holder.section not in vf.CODE_SECTIONS:
        return None, base
    if text_range and text_range[0] <= holder.addr < text_range[1]:
        return None, base + ", inside this unit, and no accepted proposal renames it"
    unit = owner(holder.addr) if owner else None
    if unit:
        return None, base + f", inside split {unit}: a different function with the same name"
    if holder.size == p.size and p.matches is not None and p.matches(p.donor):
        return None, base + " with identical code; refusing to guess between two copies"
    if placeholder(holder) in symbols.entries:
        return None, base + f"; {placeholder(holder)} already exists"
    return placeholder(holder), ""


def proposal_renames(props: list[Proposal]) -> dict[str, str]:
    """old name -> new name for every accepted proposal and the demotions it needs."""
    out: dict[str, str] = {}
    for p in props:
        if p.status != "ok":
            continue
        out[p.retail] = p.donor
        if p.demote:
            out[p.donor] = p.demote
    return out


def rewrite_identifiers(text: str, renames: dict[str, str]) -> str:
    """Simultaneous whole-word replacement, so chains and swaps cannot clobber each other."""
    if not renames:
        return text
    pattern = re.compile(r"\b(" + "|".join(re.escape(k) for k in sorted(renames, key=len, reverse=True)) + r")\b")
    return pattern.sub(lambda m: renames[m.group(1)], text)


# ---------------------------------------------------------------- granularity

@dataclass
class DonorObject:
    name: str
    section: str
    offset: int
    size: int


@dataclass
class Layout:
    replace: dict[str, list[tuple[str, int, int]]] = field(default_factory=dict)
    touched: set[str] = field(default_factory=set)   # every old and new name an edit mentions
    merged: set[str] = field(default_factory=set)    # objects built from several labels
    manual: list[str] = field(default_factory=list)


def _table(symbols: vf.SymbolTable, section: str) -> list[tuple[int, int, str]]:
    return sorted((e.addr, e.size, n) for n, e in symbols.entries.items()
                  if e.section == section and e.size and n not in symbols.duplicates)


def donor_objects(elf: vf.Elf) -> dict[str, list[DonorObject]]:
    """The donor's file-scope .bss/.sbss objects by section, with their offsets in the section."""
    out: dict[str, list[DonorObject]] = {}
    for sec in elf.sections:
        if sec.name not in (".bss", ".sbss"):
            continue
        out[sec.name] = sorted((DonorObject(s.name, sec.name, s.value, s.size) for s in elf.symbols
                                if s.shndx == sec.index and s.sym_type == 1 and s.name and s.size),
                               key=lambda o: o.offset)
    return out


def describe_objects(objs: list[DonorObject]) -> str:
    return ", ".join(f"{o.name}@{o.offset:#x}+{o.size:#x}" for o in objs)


def plan_layout(section: str, objs: list[DonorObject], refs: list[tuple[str, int, str, int]],
                symbols: vf.SymbolTable, referenced: Callable[[str], bool]) -> Layout:
    """Plan merges of unnamed labels and splits of unnamed covering symbols for one section.

    refs are (donor object, donor addend, retail symbol, retail addend) from paired relocations.
    Each fixes the object's retail address; they must agree on one constant bias between the
    donor's .bss/.sbss offsets and the retail addresses, as strip_unused.py requires.
    `referenced(name)` says whether sources under src/ already use a retail symbol, in which
    case it is left alone.
    """
    out = Layout()
    by_name = {o.name: o for o in objs}
    evidence: dict[str, set[int]] = {}
    for dname, dadd, rname, radd in refs:
        e = symbols.entries.get(rname)
        if dname in by_name and e is not None and e.section == section:
            evidence.setdefault(dname, set()).add(e.addr + radd - dadd)
    biases = {a - by_name[n].offset for n, addrs in evidence.items() for a in addrs}
    if not biases:
        return out
    if len(biases) > 1:
        out.manual.append(f"{section} layout: paired relocations place the donor objects at inconsistent "
                          f"biases {', '.join(f'{b:#x}' for b in sorted(biases))}; donor objects "
                          + describe_objects(objs))
        return out
    base = biases.pop()
    table = _table(symbols, section)
    taken: set[str] = set()
    covered: dict[str, list[DonorObject]] = {}
    for o in sorted(objs, key=lambda o: o.offset):
        addr = base + o.offset
        hits = [t for t in table if t[0] < addr + o.size and t[0] + t[1] > addr]
        if len(hits) == 1 and hits[0][0] == addr and hits[0][1] == o.size:
            continue
        if len(hits) == 1 and hits[0][0] <= addr and hits[0][0] + hits[0][1] >= addr + o.size:
            covered.setdefault(hits[0][2], []).append(o)
        elif len(hits) >= 2 and hits[0][0] == addr and sum(t[1] for t in hits) == o.size \
                and all(a + s == b for (a, s, _), (b, _, _) in zip(hits, hits[1:])):
            names = [t[2] for t in hits]
            if o.name not in evidence:
                out.manual.append(f"{o.name} ({section} {addr:08X}..{addr + o.size:08X}) could merge "
                                  f"{', '.join(names)} but no relocation names it")
            elif not all(UNNAMED.match(n) for n in names):
                out.manual.append(f"{o.name} would merge named symbols {', '.join(names)}")
            elif any(referenced(n) for n in names[1:]):
                out.manual.append(f"{o.name} would merge {', '.join(names)}, but src/ uses {names[1]}")
            else:
                out.replace[names[0]] = [(o.name, addr, o.size)]
                out.replace.update({n: [] for n in names[1:]})
                out.touched.update(names + [o.name])
                out.merged.add(o.name)
                taken.add(o.name)
        elif not hits:
            out.manual.append(f"{o.name} ({section} {addr:08X}+{o.size:#x}) has no symbol in symbols.txt")
        else:
            out.manual.append(f"{o.name} ({section} {addr:08X}+{o.size:#x}) overlaps "
                              + ", ".join(f"{n}@{a:08X}+{s:#x}" for a, s, n in hits)
                              + " without tiling them")
    for cname, inside in covered.items():
        a, size, _ = next(t for t in table if t[2] == cname)
        inside.sort(key=lambda o: o.offset)
        cursor, tiles = a, True
        for o in inside:
            tiles = tiles and base + o.offset == cursor
            cursor = base + o.offset + o.size
        tiles = tiles and cursor == a + size
        if tiles and UNNAMED.match(cname) and not referenced(cname):
            out.replace[cname] = [(o.name, base + o.offset, o.size) for o in inside]
            out.touched.update([cname] + [o.name for o in inside])
        else:
            why = "they do not tile it" if not tiles else \
                (f"{cname} is named" if not UNNAMED.match(cname) else f"src/ uses {cname}")
            out.manual.append(f"{section} symbol {cname} ({a:08X}+{size:#x}) covers donor objects "
                              + describe_objects(inside) + f" (retail {a:08X}.. = base {base:#x}); {why}")
    return out


def merge_halves(accesses: list[tuple[str, int, str, int]], symbols: vf.SymbolTable,
                 referenced: Callable[[str], bool], renames: dict[str, str] | None = None) -> Layout:
    """Merge an unnamed 4-byte label into the named 4-byte symbol before it.

    DTK splits an eight-byte static (OSTime, u64) into two 4-byte labels because only word
    accesses are visible. When the donor reaches `NAME+4` where the retail code reaches an
    unnamed label that starts exactly 4 bytes after NAME, the pair is one eight-byte object.
    accesses are (donor symbol, donor addend, retail symbol, retail addend). `renames` maps
    unnamed retail symbols to the donor names the pairing gave them, so the first half may still
    be a label in symbols.txt.
    """
    out = Layout()
    labels = {new: old for old, new in (renames or {}).items()}
    for dname, dadd, rname, radd in dict.fromkeys(accesses):
        head = dname if dname in symbols.entries else labels.get(dname, dname)
        first, second = symbols.entries.get(head), symbols.entries.get(rname)
        if first is None or second is None or not UNNAMED.match(rname) or radd != 0 or dadd != 4:
            continue
        if first.section != second.section or first.section not in (".bss", ".sbss"):
            continue
        if (first.size, second.size) != (4, 4) or second.addr != first.addr + 4 or first.addr % 8:
            continue
        if referenced(rname):
            out.manual.append(f"{dname}+4 is {rname}, which src/ uses; merge them by hand")
            continue
        out.replace[head] = [(dname, first.addr, 8)]
        out.replace[rname] = []
        out.touched.update([head, dname, rname])
        out.merged.add(dname)
    return out


# ------------------------------------------------------------- symbols.txt edits

def replace_objects(text: str, replace: dict[str, list[tuple[str, int, int]]], widen: set[str] = frozenset()) -> str:
    """Rewrite the lines named in `replace` as the listed (name, addr, size) objects; an empty
    list deletes the line. The first line's attributes are kept except `data:`/`align:` hints
    that no longer fit the new size."""
    out = []
    for line in text.splitlines():
        m = SYMBOL_LINE.match(line)
        if not m or m[1] not in replace:
            out.append(line)
            continue
        for name, addr, size in replace[m[1]]:
            attrs = re.sub(r"size:0x[0-9A-Fa-f]+", f"size:{size:#X}".replace("0X", "0x"), m[5], count=1)
            unit = re.search(r" data:(\d)byte", attrs)
            if unit and name in widen and size == 8 and addr % 8 == 0:
                attrs = attrs.replace(unit[0], " data:8byte")
            elif unit and size % int(unit[1]):
                attrs = attrs.replace(unit[0], "")
            align = re.search(r"align:(\d+)", attrs)
            if align and addr % int(align[1]):
                attrs = re.sub(r" align:\d+", "", attrs)
            out.append(f"{name}{m[2]}{m[3]}:0x{addr:08X};{attrs}")
    return "\n".join(out) + "\n"
