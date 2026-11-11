#!/usr/bin/env python3
"""Rank what blocks the automatic sweep, by functions and bytes per class.

    python3 tools/sweep_report.py                       # Markdown report on stdout
    python3 tools/sweep_report.py --write               # regenerate docs/sweep_blockers.md
    python3 tools/sweep_report.py --json before.json    # per-function classes, for --compare
    python3 tools/sweep_report.py --compare before.json # class movement since before.json
    python3 tools/sweep_report.py --class reloc:symbol+offset --limit 20

Reads the last full sweep (`work/sweep/results.json` and each `NAME/sweep_draft.c`, or
`--sweep-dir`) and never writes into it. results.json keeps only a verdict and an exact
count, so every MISMATCH draft is rescored with `score_fn.py --json` for its per-instruction
differences. Rescores are cached in `~/.cache/lsw1/sweep/report` (under `LSW1_SWEEP_CACHE`
when set) with sweep.py's score-key ingredients: the scoring tools and build config, the
unit source and its preprocessed headers, the function's target assembly and the draft.

Each function gets a list of classes:

    verdict:V             the sweep verdict
    band:B                closeness band of a MISMATCH (exact / target instructions)
    error:TEXT            first compile error, normalised (no file:line, quoted names -> X, digits -> N)
    shape:S               MISMATCH diff shape (mutually exclusive, see SHAPES)
    frame:F               stack frame and saved-GPR comparison (first stwu r1 / stmw differences)
    reloc:R               relocation causes (symbol+offset folded into the address, one-sided)
    draft:D               features of the sweep draft text
    mnemonic:B            mnemonic-order similarity band (aligned same-opcode rows / target)
    unswept:U             ready targets the sweep did not score

Classes overlap, so a class's bytes are an upper bound on what fixing its cause unlocks.
`--compare` measures a fix: functions and bytes entering and leaving each class, including
functions that left the listing (matched or no longer ready) and changed verdicts.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))

DOC = ROOT / "docs" / "sweep_blockers.md"
REPORT_VERSION = "1"  # bump when the cached rescore payload changes shape
CLOSENESS_BANDS = (">=90", "80-90", "50-80", "<50")
MNEMONIC_BANDS = (">=95", "90-95", "80-90", "50-80", "<50")
SHAPES = (
    ("reloc-data-only", "reloc/data only"),
    ("frame-offsets", "same shape, only stack/frame offsets differ"),
    ("registers", "same shape, only registers differ"),
    ("immediates", "same shape, only immediates differ"),
    ("same-length", "same length with opcode/order differences"),
    ("length-3pct", "length within 3%"),
    ("length-10pct", "length within 10%"),
    ("mine-longer", "mine longer >10%"),
    ("mine-shorter", "mine shorter >10%"),
)
FRAMES = (
    ("equal", "frame equal or no frame difference"),
    ("smaller-same-gprs", "frame smaller, saved GPRs same"),
    ("bigger-same-gprs", "frame bigger, saved GPRs same"),
    ("fewer-gprs", "frame differs, mine saves fewer GPRs"),
    ("more-gprs", "frame differs, mine saves more GPRs"),
)
RELOCS = (
    ("symbol+offset", "mine relocates against symbol+offset, target against the bare symbol"),
    ("only-mine", "only mine has a relocation"),
    ("only-target", "only target has a relocation"),
)
DRAFT_FEATURES = (
    ("casted-deref", "raw casted deref `*(T *)((s8 *)p + off)`",
     re.compile(r"\*\s*\(\s*[\w ]+\*+\s*\)\s*\(\s*\(\s*[\w ]+\*\s*\)")),
    ("void-param", "`void *` parameter", None),
    ("goto", "`goto`", re.compile(r"\bgoto\b")),
    ("sp-local", "`sp`-named stack locals", re.compile(r"\bsp[0-9A-Fa-f]+\b")),
    ("switch", "`switch`", re.compile(r"\bswitch\s*\(")),
)
LABELS = {**{f"shape:{k}": v for k, v in SHAPES}, **{f"frame:{k}": v for k, v in FRAMES},
          **{f"reloc:{k}": v for k, v in RELOCS}, **{f"draft:{k}": v for k, v, _ in DRAFT_FEATURES},
          "unswept:no-source": "ready target whose unit has no source file",
          "unswept:not-in-results": "ready target with a source but no results.json row",
          "rescore:failed": "MISMATCH draft that no longer rescores"}
# Classes that describe a function rather than name a cause; left out of the ranking.
NEUTRAL = {"frame:equal"}


# --- classification (pure, unit tested) -------------------------------------------------

def band(closeness: float) -> str:
    return (">=90" if closeness >= 0.9 else "80-90" if closeness >= 0.8
            else "50-80" if closeness >= 0.5 else "<50")


def mnemonic_band(ratio: float) -> str:
    return (">=95" if ratio >= 0.95 else "90-95" if ratio >= 0.9 else "80-90" if ratio >= 0.8
            else "50-80" if ratio >= 0.5 else "<50")


def normalise_error(error: str) -> str:
    """First compile error line without its location, quoted names as X and digits as N."""
    line = (error or "").strip().splitlines()[0] if (error or "").strip() else "(no error text)"
    line = re.sub(r"^\S*?:\d+:(?:\d+:)?\s*", "", line)
    line = re.sub(r"^(?:error|Error):\s*", "", line)
    line = re.sub(r"`[^`']*'|'[^']*'|\"[^\"]*\"", "X", line)
    line = re.sub(r"\d+", "N", line)
    return re.sub(r"\s+", " ", line).strip()


_WORDS = re.compile(r"([0-9A-Fa-f]{8}) vs ([0-9A-Fa-f]{8})")


def words(detail: str) -> tuple[int, int] | None:
    """(target, mine) instruction words of an opcode/operand difference (verify_fn order)."""
    m = _WORDS.match(detail or "")
    return (int(m[1], 16), int(m[2], 16)) if m else None


def s16(x: int) -> int:
    x &= 0xFFFF
    return x - 0x10000 if x & 0x8000 else x


def is_stwu_r1(w: int) -> bool:
    return w >> 26 == 37 and (w >> 21) & 31 == 1 and (w >> 16) & 31 == 1


def is_stmw_r1(w: int) -> bool:
    return w >> 26 == 47 and (w >> 16) & 31 == 1


def operand_kind(detail: str) -> str:
    """What an operand difference touches: frame, stackoff, imm, reg, branch, reg+imm or other."""
    pair = words(detail)
    if not pair:
        return "other"
    t, m = pair
    op, x = t >> 26, t ^ m
    if op in (16, 18):
        return "branch"
    if is_stwu_r1(t):
        return "frame"
    if op in (31, 19, 59, 63, 4):
        return "reg"
    if x & 0xFFFF == 0:
        return "reg"
    if x & 0x03FF0000 == 0:
        return "stackoff" if (t >> 16) & 31 == 1 else "imm"
    return "reg+imm"


def shape(rep: dict) -> str:
    kinds: dict[str, int] = defaultdict(int)
    operands: set[str] = set()
    for d in rep.get("differences", []):
        kinds[d["kind"]] += 1
        if d["kind"] == "operand":
            operands.add(operand_kind(d.get("detail", "")))
    t, m = rep.get("target_insns", 0), rep.get("mine_insns", 0)
    dl = m - t
    if not kinds["length"] and not kinds["opcode"]:
        if not kinds["operand"]:
            return "reloc-data-only"
        if operands & {"frame", "stackoff"}:
            return "frame-offsets"
        if operands <= {"reg", "branch"}:
            return "registers"
        return "immediates"
    if dl == 0 and kinds["length"] <= 2:
        return "same-length"
    if abs(dl) <= max(2, t * 0.03):
        return "length-3pct"
    if abs(dl) <= t * 0.10:
        return "length-10pct"
    return "mine-longer" if dl > 0 else "mine-shorter"


def frame(rep: dict) -> str:
    """Compare the first differing `stwu r1` (frame size) and `stmw` (first saved GPR)."""
    size_delta = gpr_delta = None
    for d in rep.get("differences", []):
        if d["kind"] != "operand":
            continue
        pair = words(d.get("detail", ""))
        if not pair:
            continue
        t, m = pair
        if size_delta is None and is_stwu_r1(t):
            size_delta = -s16(m) - -s16(t)  # > 0: mine's frame is bigger
        if gpr_delta is None and is_stmw_r1(t):
            gpr_delta = ((t >> 21) & 31) - ((m >> 21) & 31)  # > 0: mine saves more GPRs
    if not size_delta:
        return "equal"
    if gpr_delta:
        return "more-gprs" if gpr_delta > 0 else "fewer-gprs"
    return "bigger-same-gprs" if size_delta > 0 else "smaller-same-gprs"


_OFFSET = re.compile(r"[+-]0x[0-9A-Fa-f]+$")
_REFERENCES = re.compile(r"references (\S+) (\S+) \(0x[0-9A-Fa-f]+\) but target references (\S+) (\S+) \(")
_ADDEND = re.compile(r"addend: target (\S+) (\S+) vs mine (\S+) (\S+)$")


def symbol_offset(detail: str) -> bool:
    """Mine relocates against symbol+offset where the target relocates against a bare symbol."""
    m = _REFERENCES.match(detail or "")
    if m:
        mine, target = m[2], m[4]
    else:
        m = _ADDEND.match(detail or "")
        if not m:
            return False
        target, mine = m[2], m[4]
    return bool(_OFFSET.search(mine)) and not _OFFSET.search(target)


def reloc_classes(rep: dict) -> list[str]:
    found = set()
    for d in rep.get("differences", []):
        if d["kind"] != "reloc":
            continue
        detail = d.get("detail", "")
        if symbol_offset(detail):
            found.add("symbol+offset")
        elif detail == "only mine has a relocation":
            found.add("only-mine")
        elif detail == "only target has a relocation":
            found.add("only-target")
    return [f"reloc:{k}" for k, _ in RELOCS if k in found]


def draft_classes(text: str, name: str) -> list[str]:
    found = []
    for key, _label, pattern in DRAFT_FEATURES:
        if pattern is None:  # void * parameter of NAME's own definition
            sig = re.search(rf"\b{re.escape(name)}\s*\(([^)]*)\)\s*\{{", text)
            hit = bool(sig and re.search(r"\bvoid\s*\*", sig.group(1)))
        else:
            hit = bool(pattern.search(text))
        if hit:
            found.append(f"draft:{key}")
    return found


def classify(row: dict, rep: dict | None = None, draft: str | None = None) -> list[str]:
    """Classes of one results.json row (REP: its rescore, DRAFT: its sweep_draft.c text)."""
    verdict = row["verdict"]
    classes = [f"verdict:{verdict}"]
    if verdict == "COMPILE-ERROR":
        classes.append("error:" + normalise_error(row.get("error", "")))
    elif verdict == "UNHANDLED":
        classes.append("unswept:unhandled")
    if verdict != "MISMATCH":
        return classes
    classes.append("band:" + band(row.get("closeness") or 0.0))
    if rep is None or "differences" not in rep:
        classes.append("rescore:failed")
    else:
        classes.append("shape:" + shape(rep))
        classes.append("frame:" + frame(rep))
        classes += reloc_classes(rep)
        target = rep.get("target_insns") or 0
        if target:
            classes.append("mnemonic:" + mnemonic_band(rep.get("mnemonic_similarity", 0) / target))
    if draft is not None:
        classes += draft_classes(draft, row["name"])
    return classes


# --- rescoring --------------------------------------------------------------------------

class Rescorer:
    """score_fn.py --json for sweep drafts, cached on sweep.py's score-key ingredients."""

    def __init__(self, sweep_dir: Path, jobs: int, use_cache: bool = True):
        import sweep  # read-only use of its key ingredients; nothing here writes sweep state
        self.sweep = sweep
        self.sweep_dir = sweep_dir
        self.jobs = jobs
        self.use_cache = use_cache
        self.cache = sweep.CACHE_ROOT / "report"
        self._contexts: dict[str, str | None] = {}
        self._tools: str | None = None
        self._symbols: dict[str, tuple[int, int]] | None = None

    def tools_digest(self) -> str:
        if self._tools is None:
            h = hashlib.sha256(REPORT_VERSION.encode())
            for rel in [f"tools/{t}" for t in self.sweep.SCORE_TOOLS] + list(self.sweep.SCORE_CONFIG):
                h.update((ROOT / rel).read_bytes())
            self._tools = h.hexdigest()
        return self._tools

    def context_digest(self, source: str) -> str | None:
        """Digest of SOURCE preprocessed as sweep.unit_context does (computed, not written)."""
        if source not in self._contexts:
            try:
                r = subprocess.run(self.sweep.CPP + [source], cwd=ROOT, capture_output=True, text=True)
                ok = r.returncode == 0
            except OSError:
                ok = False
            self._contexts[source] = hashlib.sha256(r.stdout.encode()).hexdigest() if ok else None
        return self._contexts[source]

    def target_asm(self, name: str) -> str | None:
        """NAME's assembly in the current build (what pack_asm.refresh would write)."""
        tp = self.sweep.pack_asm._task_pack()
        if self._symbols is None:
            self._symbols = {}
            for addr, sym, size, _ in tp.load_symtab():
                self._symbols.setdefault(sym, (addr, size))
        sym = self._symbols.get(name)
        if sym is None:
            return None
        text, _source = tp.locate_asm(sym[0], sym[1], name)
        return text

    def key(self, name: str, source: str, body: str) -> str | None:
        asm = self.target_asm(name)
        ctx = self.context_digest(source)
        if asm is None or ctx is None:
            return None
        h = hashlib.sha256(self.tools_digest().encode())
        for part in (name, self.sweep.file_digest(ROOT / source), ctx, asm, body):
            h.update(len(part).to_bytes(8, "little") + part.encode())
        return h.hexdigest()

    def one(self, row: dict) -> dict | None:
        draft = self.sweep_dir / row["name"] / "sweep_draft.c"
        try:
            body = draft.read_text()
        except OSError:
            return None
        digest = self.key(row["name"], row["source"], body) if self.use_cache else None
        path = self.cache / digest[:2] / f"{digest}.json" if digest else None
        if path:
            try:
                return json.loads(path.read_text())
            except (OSError, ValueError):
                pass
        try:
            r = subprocess.run([sys.executable, str(ROOT / "tools" / "score_fn.py"), row["name"], row["source"],
                                "--body", str(draft), "--no-queue", "--json"],
                               cwd=ROOT, capture_output=True, text=True, timeout=300)
            rep = json.loads(r.stdout)
        except (subprocess.TimeoutExpired, ValueError):
            return None
        rep = {k: rep.get(k) for k in ("verdict", "target_insns", "mine_insns", "exact_insns",
                                       "mnemonic_similarity", "differences")}
        if path:
            self.sweep.write_atomic(path, json.dumps(rep))
        return rep

    def all(self, rows: list[dict]) -> dict[str, dict | None]:
        todo = [r for r in rows if r["verdict"] == "MISMATCH"]
        for r in todo:  # warm the per-source and symbol caches before threading
            self.context_digest(r["source"])
        if todo:
            self.target_asm(todo[0]["name"])
        with ThreadPoolExecutor(self.jobs) as pool:
            return dict(zip((r["name"] for r in todo), pool.map(self.one, todo)))


# --- data assembly ----------------------------------------------------------------------

def ready_targets() -> list[dict]:
    """The listing sweep.main sweeps (next_target.py through sweep.targets)."""
    import sweep
    return sweep.targets(argparse.Namespace(max_size="0x100000", unit=None))


def sweep_revision(sweep_dir: Path) -> str:
    """Short sha of the sweep checkout's commit when results.json was written (its HEAD may have
    moved on since)."""
    stamp = int((sweep_dir / "results.json").stat().st_mtime)
    r = subprocess.run(["git", "-C", str(sweep_dir), "log", "-1", "--first-parent", f"--before=@{stamp}",
                        "--format=%h", "HEAD"], capture_output=True, text=True)
    return r.stdout.strip() if r.returncode == 0 and r.stdout.strip() else "unknown"


def build(sweep_dir: Path, jobs: int, use_cache: bool = True, ready: list[dict] | None = None) -> dict:
    rows = json.loads((sweep_dir / "results.json").read_text())
    reps = Rescorer(sweep_dir, jobs, use_cache).all(rows)
    functions = []
    for row in rows:
        draft = None
        if row["verdict"] == "MISMATCH":
            try:
                draft = (sweep_dir / row["name"] / "sweep_draft.c").read_text()
            except OSError:
                pass
        functions.append({"name": row["name"], "unit": row.get("unit", ""), "size": row["size"],
                          "verdict": row["verdict"], "closeness": round(row.get("closeness") or 0.0, 4),
                          "error": row.get("error", ""),
                          "classes": classify(row, reps.get(row["name"]), draft)})
    seen = {f["name"] for f in functions}
    for t in ready if ready is not None else ready_targets():
        if t["name"] in seen:
            continue
        cls = "unswept:no-source" if not t.get("source") else "unswept:not-in-results"
        functions.append({"name": t["name"], "unit": t.get("unit", ""), "size": t["size"],
                          "verdict": "NOT-SWEPT", "closeness": 0.0, "classes": [f"verdict:NOT-SWEPT", cls]})
        seen.add(t["name"])
    functions.sort(key=lambda f: f["name"])
    return {"sweep": sweep_revision(sweep_dir), "rows": len(rows), "functions": functions}


# --- aggregation and report -------------------------------------------------------------

class Tally:
    def __init__(self):
        self.fns = self.bytes = self.near = 0
        self.members: list[dict] = []

    def add(self, f: dict) -> None:
        self.fns += 1
        self.bytes += f["size"]
        if f["verdict"] == "MISMATCH" and f["closeness"] >= 0.8:
            self.near += f["size"]
        self.members.append(f)

    def example(self) -> str:
        """The largest member nearest a match (ties by name): the one worth opening first."""
        if not self.members:
            return "-"
        best = max(self.members, key=lambda f: (f["size"] * (f["closeness"] or 0), f["size"], f["name"]))
        return best["name"]


def tally(functions: list[dict]) -> dict[str, Tally]:
    out: dict[str, Tally] = defaultdict(Tally)
    for f in sorted(functions, key=lambda f: f["name"]):
        for c in dict.fromkeys(f["classes"]):
            out[c].add(f)
    return out


def label(cls: str) -> str:
    if cls in LABELS:
        return LABELS[cls]
    prefix, _, rest = cls.partition(":")
    return rest if prefix == "error" else cls


def _num(n: int) -> str:
    return f"{n:,}"


def table(head: list[str], rows: list[list[str]]) -> list[str]:
    right = [i for i, h in enumerate(head) if h in ("functions", "bytes", "bytes >=80%", "share")]
    sep = ["---:" if i in right else "---" for i in range(len(head))]
    return ["| " + " | ".join(head) + " |", "| " + " | ".join(sep) + " |",
            *("| " + " | ".join(r) + " |" for r in rows)]


def by_bytes(t: dict[str, Tally], classes) -> list[str]:
    return sorted(classes, key=lambda c: (-t[c].bytes, -t[c].fns, c))


def ranked(t: dict[str, Tally]) -> list[str]:
    # `draft:` text features are traits of most m2c drafts, not causes; they keep their own table.
    keys = [c for c in t if (c.startswith(("error:", "frame:", "reloc:")) or c == "unswept:no-source")
            and c not in NEUTRAL and t[c].fns]
    return by_bytes(t, keys)


def markdown(data: dict) -> str:
    fns = data["functions"]
    t = tally(fns)
    swept = [f for f in fns if f["verdict"] != "NOT-SWEPT"]
    total = sum(f["size"] for f in swept)
    mism = t["verdict:MISMATCH"]
    out = ["# Sweep blockers", "",
           "Generated by `python3 tools/sweep_report.py --write`; do not edit by hand. "
           "Generated from sweep of " f"{data['sweep']}, {data['rows']} rows.", "",
           "Classes overlap: a function counts in every class it belongs to, so a class's bytes are "
           "an upper bound on what fixing its cause can unlock. `bytes >=80%` counts MISMATCH "
           "functions at 80% exact instructions or better. List a class's members with "
           "`python3 tools/sweep_report.py --class CLASS`; measure a fix with `--json` before and "
           "`--compare` after.", ""]

    out += ["## Verdict mix", ""]
    verdicts = sorted({f["verdict"] for f in swept}, key=lambda v: (-t[f"verdict:{v}"].bytes, v))
    out += table(["verdict", "functions", "bytes", "share"],
                 [[v, _num(t[f"verdict:{v}"].fns), _num(t[f"verdict:{v}"].bytes),
                   f"{100 * t[f'verdict:{v}'].bytes / total:.1f}%" if total else "-"] for v in verdicts]
                 + [["total", _num(len(swept)), _num(total), "100.0%"]])
    out += ["", "MISMATCH closeness bands (exact / target instructions, %):", ""]
    out += table(["band", "functions", "bytes"],
                 [[b, _num(t[f"band:{b}"].fns), _num(t[f"band:{b}"].bytes)] for b in CLOSENESS_BANDS])
    if t["rescore:failed"].fns:
        out += ["", f"{t['rescore:failed'].fns} MISMATCH drafts did not rescore and have no diff classes "
                f"({_num(t['rescore:failed'].bytes)} B)."]

    out += ["", "## COMPILE-ERROR classes", "",
            "First error line, without file:line, quoted names as `X` and digits as `N`.", ""]
    errors = by_bytes(t, [c for c in t if c.startswith("error:")])
    out += table(["error", "functions", "bytes", "example"],
                 [[f"`{label(c)}`", _num(t[c].fns), _num(t[c].bytes), t[c].example()] for c in errors])

    out += ["", "## MISMATCH diff shapes", "",
            "Mutually exclusive, first matching row wins. \"Same shape\" means no opcode or length "
            "differences, only operand (and relocation) ones; lengths compare instruction counts.", ""]
    out += table(["shape", "functions", "bytes", "bytes >=80%", "example"],
                 [[label(f"shape:{k}"), _num(t[f"shape:{k}"].fns), _num(t[f"shape:{k}"].bytes),
                   _num(t[f"shape:{k}"].near), t[f"shape:{k}"].example() if t[f"shape:{k}"].fns else "-"]
                  for k, _ in SHAPES])

    out += ["", "## Cross-cutting causes", "",
            "Not exclusive. Frame rows compare the first differing `stwu r1` (frame size) and `stmw` "
            "(first saved GPR); `symbol+offset` means the draft folds a constant offset into the "
            "address where retail materialises the base and uses a displacement.", ""]

    def cause_rows(keys):
        return [[f"`{c}`", label(c), _num(t[c].fns), _num(t[c].bytes), _num(t[c].near),
                 t[c].example() if t[c].fns else "-"] for c in keys]
    head = ["class", "meaning", "functions", "bytes", "bytes >=80%", "example"]
    out += ["### Frame", ""] + table(head, cause_rows(by_bytes(t, [f"frame:{k}" for k, _ in FRAMES])))
    out += ["", "### Relocations", ""] + table(head, cause_rows(by_bytes(t, [f"reloc:{k}" for k, _ in RELOCS])))
    out += ["", "### Draft text (`sweep_draft.c`)", ""]
    out += table(head, cause_rows(by_bytes(t, [f"draft:{k}" for k, _, _ in DRAFT_FEATURES])))
    out += ["", "### Mnemonic-order similarity", "",
            "Aligned rows with the same opcode, over target instructions.", ""]
    out += table(["band", "functions", "bytes", "bytes >=80%"],
                 [[b, _num(t[f"mnemonic:{b}"].fns), _num(t[f"mnemonic:{b}"].bytes),
                   _num(t[f"mnemonic:{b}"].near)] for b in MNEMONIC_BANDS])

    out += ["", "## Not swept", "",
            "Ready targets (`next_target.py`, as `sweep.py` lists them) the sweep skips because their "
            "unit has no source file, plus drafts m2c could not express (UNHANDLED).", ""]
    unswept = by_bytes(t, [c for c in t if c.startswith("unswept:")])
    out += table(["class", "meaning", "functions", "bytes", "example"],
                 [[f"`{c}`", label(c) if c != "unswept:unhandled" else "m2c draft uses an unhandled macro",
                   _num(t[c].fns), _num(t[c].bytes), t[c].example()] for c in unswept])
    units: dict[str, list[int]] = defaultdict(lambda: [0, 0])
    for f in t["unswept:no-source"].members:
        units[f["unit"]][0] += 1
        units[f["unit"]][1] += f["size"]
    if units:
        top = sorted(units.items(), key=lambda kv: (-kv[1][1], kv[0]))[:10]
        out += ["", "Largest units without a source file:", ""]
        out += table(["unit", "functions", "bytes"], [[f"`{u}`", _num(n), _num(b)] for u, (n, b) in top])

    out += ["", "## Ranked by bytes", "",
            "COMPILE-ERROR classes, frame and relocation causes and unswept targets together, top 20.", ""]
    out += table(["class", "functions", "bytes", "bytes >=80%", "example"],
                 [[f"`{c}`" if not c.startswith("error:") else f"error: `{label(c)}`", _num(t[c].fns),
                   _num(t[c].bytes), "-" if c.startswith(("error:", "unswept:")) else _num(t[c].near), t[c].example()]
                  for c in ranked(t)[:20]])
    out += ["", "Classes overlap, so each row's bytes are an upper bound on what fixing that cause "
            "can unlock, not an additive share."]
    return "\n".join(out) + "\n"


# --- compare and listing ----------------------------------------------------------------

def compare(old: dict, new: dict) -> str:
    """Per-class movement between two --json files (functions and bytes in and out)."""
    o = {f["name"]: f for f in old["functions"]}
    n = {f["name"]: f for f in new["functions"]}
    gone = sorted(set(o) - set(n))
    for name in gone:  # left the listing: matched or defined since, so no longer a target
        n[name] = {**o[name], "verdict": "GONE", "closeness": 1.0, "classes": ["verdict:GONE"]}
    classes = sorted({c for f in [*o.values(), *n.values()] for c in f["classes"]})
    rows = []
    for c in classes:
        ins = [f for k, f in n.items() if c in f["classes"] and (k not in o or c not in o[k]["classes"])]
        outs = [f for k, f in o.items() if c in f["classes"] and c not in n[k]["classes"]]
        if not ins and not outs:
            continue
        before = sum(f["size"] for f in o.values() if c in f["classes"])
        after = sum(f["size"] for f in n.values() if c in f["classes"] and c != "verdict:GONE")
        rows.append((c, len(ins), sum(f["size"] for f in ins), len(outs), sum(f["size"] for f in outs),
                     before, after))
    rows.sort(key=lambda r: (-abs(r[6] - r[5]), r[0]))

    def matched(d):
        return [f for f in d.values() if f["verdict"] in ("MATCH", "GONE")]
    mo, mn = [f for f in o.values() if f["verdict"] == "MATCH"], matched(n)
    out = [f"compare: {old.get('sweep', '?')} ({len(o)} functions) -> {new.get('sweep', '?')} "
           f"({len(new['functions'])} functions)",
           f"matched: {len(mo)} -> {len(mn)} functions ({sum(f['size'] for f in mn) - sum(f['size'] for f in mo):+,} B); "
           f"{len(gone)} left the listing ({sum(o[k]['size'] for k in gone):,} B, counted as matched/defined)",
           f"new in listing: {len(set(n) - set(o))}",
           f"verdict changes: {sum(1 for k in o if k not in gone and o[k]['verdict'] != n[k]['verdict'])}", ""]
    out += [f"{'in fn':>6} {'in B':>9} {'out fn':>6} {'out B':>9} {'B before':>10} {'B after':>10}  class"]
    for c, ni, bi, no, bo, before, after in rows:
        out.append(f"{ni:6} {bi:9,} {no:6} {bo:9,} {before:10,} {after:10,}  {c}")
    return "\n".join(out) + "\n"


def members(data: dict, cls: str, limit: int) -> str:
    known = sorted({c for f in data["functions"] for c in f["classes"]})
    if cls not in known:
        hits = [c for c in known if cls in c]
        if len(hits) != 1:
            raise KeyError(f"no class {cls!r}" + (f"; candidates: {', '.join(hits[:20])}" if hits else ""))
        cls = hits[0]
    fs = sorted((f for f in data["functions"] if cls in f["classes"]), key=lambda f: (f["size"], f["name"]))
    lines = [f"{cls}: {len(fs)} functions, {sum(f['size'] for f in fs):,} B"]
    for f in fs[:limit] if limit else fs:
        lines.append(f"  {f['name']:32} {f['size']:6} B  {f['unit']:40} {f['verdict']:13} {100 * f['closeness']:5.1f}%")
    return "\n".join(lines) + "\n"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--sweep-dir", type=Path, default=ROOT / "work" / "sweep",
                    help="sweep output to read (default work/sweep); never written")
    ap.add_argument("--write", action="store_true", help=f"write {DOC.relative_to(ROOT)}")
    ap.add_argument("--json", type=Path, metavar="PATH", help="write the per-function classification")
    ap.add_argument("--compare", type=Path, metavar="OLD.json", help="class movement since an older --json")
    ap.add_argument("--against", type=Path, metavar="NEW.json",
                    help="with --compare: compare against this --json instead of the current sweep")
    ap.add_argument("--class", dest="cls", metavar="NAME", help="list one class's functions, smallest first")
    ap.add_argument("--limit", type=int, default=0, help="with --class: at most N functions (0: all)")
    ap.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 4) - 2), help="parallel rescores")
    ap.add_argument("--no-cache", action="store_true", help="rescore every draft instead of using the cache")
    args = ap.parse_args(argv)

    if args.compare and args.against:
        print(compare(json.loads(args.compare.read_text()), json.loads(args.against.read_text())), end="")
        return 0
    if not (args.sweep_dir / "results.json").is_file():
        print(f"error: no {args.sweep_dir / 'results.json'}; run tools/sweep.py or pass --sweep-dir",
              file=sys.stderr)
        return 2
    data = build(args.sweep_dir.resolve(), args.jobs, not args.no_cache)
    if args.json:
        args.json.write_text(json.dumps(data, indent=1, sort_keys=True) + "\n")
        print(f"wrote {args.json}", file=sys.stderr)
    if args.compare:
        print(compare(json.loads(args.compare.read_text()), data), end="")
    elif args.cls:
        try:
            print(members(data, args.cls, args.limit), end="")
        except KeyError as e:
            print(f"error: {e.args[0]}", file=sys.stderr)
            return 2
    elif args.write:
        DOC.write_text(markdown(data))
        print(DOC.relative_to(ROOT))
    elif not args.json:
        print(markdown(data), end="")
    return 0


if __name__ == "__main__":
    sys.exit(main())
