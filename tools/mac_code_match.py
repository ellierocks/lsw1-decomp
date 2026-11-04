#!/usr/bin/env python3
"""Name unnamed GC functions from the Mac LSW1 demo's *code*, not just its symbol order.

    python3 tools/mac_code_match.py                  # match, write docs/symbol_donors/mac_code_match.{tsv,md}
    python3 tools/mac_code_match.py --validate       # hold named GC functions out, report precision per tier
    python3 tools/mac_code_match.py --emit-renames work/mac_code_renames.txt   # HIGH names for batch_rename.py

The Mac demo (orig/mac/mac-debug-symbols/LEGO Star Wars Demo, gitignored) is Metrowerks
CodeWarrior PowerPC with C++-mangled symbols, so its codegen never transfers to ProDG, but both
builds come from the same source with the same struct layout.  `mac_code_features.py` extracts
what survives the compiler change from each function: callees and callers (by name, through the
already-named anchors), integer immediates, struct-field load/store offsets, float literal
values, string and named-global references.  A candidate pair scores the IDF-weighted overlap
of those facts per kind, minus a small penalty for what each side has that the other lacks, a
bonus when the Mac function sits between the Mac images of the GC function's neighbouring
anchors, and a size-plausibility factor.  Matching is mutual-best with a margin over the
runner-up, and iterates: each accepted name becomes an anchor for the next round.

Tiers: HIGH is accepted by the strict pass alone (its anchors are original names or other HIGH
names); MEDIUM extends that with looser thresholds; LOW is the best remaining candidate, not
accepted.  `--validate` measures each tier's precision by hiding a fifth of the named GC
functions that also exist in the Mac demo and checking whether the matcher recovers their names.
"""
from __future__ import annotations

import argparse
import bisect
import heapq
import math
import random
import re
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
import mac_code_features as mcf  # noqa: E402
from rejected_renames import load_rejected  # noqa: E402

MAC_BIN = ROOT / "orig" / "mac" / "mac-debug-symbols" / "LEGO Star Wars Demo"
DOL = ROOT / "orig" / "GL5E4F" / "sys" / "main.dol"
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
OUT_TSV = ROOT / "docs" / "symbol_donors" / "mac_code_match.tsv"
OUT_MD = ROOT / "docs" / "symbol_donors" / "mac_code_match.md"

AUTO = re.compile(r"^(fn|lbl|jumptable|zz|gap|ptr|sub|loc)_[0-9A-Fa-f]{8}$")
SYM_RE = re.compile(r"^(\S+) = \.(\w+):0x([0-9A-Fa-f]+); // (.*)$")
BAD_NAMES = {"start", "main", "atexit", "malloc", "free", "init", "Init", "Update", "Draw", "Load"}

# Feature kinds and how much each is worth.  Tuned on the hold-out (see --validate).
KINDS = "crsdfiomt"  # callee caller string dsym float imm field shift trailer
MULT = {"c": 3.0, "r": 2.0, "s": 3.0, "d": 3.0, "f": 1.5, "i": 0.8, "o": 0.8, "m": 0.4}
PEN = {"c": 0.30, "r": 0.10, "s": 0.15, "d": 0.15, "f": 0.10, "i": 0.04, "o": 0.04, "m": 0.02}
MAX_POSTING = 600
TOPK = 40
SIZE_SLOPE, SIZE_FREE = 8.0, 0.8
GAP_MAX, GAP_BONUS, GAP_RANK_BONUS, GAP_EDGE_BONUS = 40, 4.0, 6.0, 3.0


@dataclass
class Fn:
    name: str          # GC symbol or Mac base name
    raw: str
    addr: int
    size: int
    facts: mcf.Facts | None = None
    callees: list[int] = None  # indices into the same list; externs are negative: -(1+ext index)
    callers: set[int] = None


class Side:
    """One binary's functions and their static token sets."""

    def __init__(self, fns: list[Fn], ext_names: dict[int, str]):
        self.fns = fns
        self.ext = ext_names
        self.by_addr = {f.addr: i for i, f in enumerate(fns)}
        for f in fns:
            f.callees, f.callers = [], set()
        for i, f in enumerate(fns):
            seen = []
            for t in [*f.facts.calls, *f.facts.tails]:
                j = self.by_addr.get(t)
                if j is not None and j != i:
                    seen.append(j)
                elif t in ext_names:
                    seen.append(-1 - t)  # extern, keyed by address
            f.callees = seen
        for i, f in enumerate(fns):
            for j in f.callees:
                if j >= 0:
                    fns[j].callers.add(i)

    def callee_name(self, j: int) -> str | None:
        return self.fns[j].name if j >= 0 else self.ext.get(-1 - j)

    def static_tokens(self, i: int) -> dict[str, float]:
        """Static (iteration-independent) tokens of function I with unit multiplicity."""
        f = self.fns[i].facts
        toks: dict[str, float] = {}
        for v in f.imms:
            if -1 <= v <= 0:
                continue
            toks[f"i{v}"] = 1.0
        for k, off in f.fields:
            toks[f"o{k}{off}"] = 1.0
        for v in f.floats:
            toks[f"f{v}"] = 1.0
        for s in f.strings:
            toks[f"s{s}"] = 1.0 + min(len(s), 24) / 12.0
        for d in f.dsyms:
            toks[f"d{d}"] = 1.0
        for sh in f.shifts:
            toks["m%d.%d.%d" % sh] = 1.0
        return toks


def load_gc() -> tuple[Side, mcf.Image, list[tuple[int, int, str]]]:
    img, r13, r2 = mcf.load_dol(DOL)
    funcs, data = [], []
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        m = SYM_RE.match(line)
        if not m:
            continue
        name, sec, addr, meta = m[1], m[2], int(m[3], 16), m[4]
        sm = re.search(r"size:0x([0-9A-Fa-f]+)", meta)
        size = int(sm[1], 16) if sm else 0
        if "type:function" in meta and sec in ("text", "init"):
            funcs.append([name, addr, size])
        elif sec in ("rodata", "data", "sdata", "sdata2", "bss", "sbss") and not AUTO.match(name) \
                and not name.startswith(("@", "lbl_", "jumptable_")):
            data.append((addr, size or 4, name))
    funcs.sort(key=lambda f: f[1])
    for k, f in enumerate(funcs):
        if not f[2]:
            f[2] = (funcs[k + 1][1] if k + 1 < len(funcs) else f[1] + 4) - f[1]
    data.sort()
    starts = [d[0] for d in data]

    def symname(a: int) -> str | None:
        k = bisect.bisect_right(starts, a) - 1
        if k >= 0 and data[k][0] <= a < data[k][0] + data[k][1]:
            return data[k][2]
        return None

    fns = []
    for name, addr, size in funcs:
        fns.append(Fn(name, name, addr, size, mcf.scan(img, addr, size, sda=(r13, r2), symname=symname)))
    return Side(fns, {}), img, data


def load_mac() -> Side:
    img = mcf.load_macho(MAC_BIN)
    nm = subprocess.run(["llvm-nm", "-n", "-a", str(MAC_BIN)], capture_output=True, text=True, errors="replace").stdout
    text = next(s for s in img.sections if s.name == "__text")
    syms, data = {}, []
    for line in nm.splitlines():
        p = line.split(None, 2)
        if len(p) < 3 or p[1] not in ("t", "T", "d", "D", "s", "S", "b", "B"):
            continue
        try:
            addr = int(p[0], 16)
        except ValueError:
            continue
        if p[1] in "tT":
            syms.setdefault(addr, p[2].strip())
        else:
            data.append((addr, p[2].strip()))
    data.sort()
    dstarts = [d[0] for d in data]
    dnames = [mcf.base_name(n) for _, n in data]
    # The nearest following symbol bounds a data symbol; cap at 4 KiB so tables do not swallow code.
    def symname(a: int) -> str | None:
        k = bisect.bisect_right(dstarts, a) - 1
        if k < 0 or dnames[k] is None:
            return None
        nxt = dstarts[k + 1] if k + 1 < len(dstarts) else dstarts[k] + 4
        return dnames[k] if a < min(nxt, dstarts[k] + 4096) else None

    addrs = sorted(syms)
    fns = []
    for k, a in enumerate(addrs):
        end = addrs[k + 1] if k + 1 < len(addrs) else text.addr + text.size
        raw = syms[a]
        fns.append(Fn(mcf.base_name(raw) or "", raw, a, end - a, mcf.scan(img, a, end - a, symname=symname)))
    # Extern stubs: __picsymbol_stub entries name the C library / Toolbox function behind each stub.
    ext = {}
    ind = subprocess.run(["llvm-objdump", "--macho", "--indirect-symbols", str(MAC_BIN)],
                         capture_output=True, text=True, errors="replace").stdout
    in_stub = False
    for line in ind.splitlines():
        if line.startswith("Indirect symbols for"):
            in_stub = "picsymbol_stub" in line
        elif in_stub and line.startswith("0x"):
            p = line.split()
            if len(p) >= 3:
                ext[int(p[0], 16)] = p[2].lstrip("_")
    return Side(fns, ext)


# ---------------------------------------------------------------------------------------------

class Matcher:
    def __init__(self, gc: Side, mac: Side, hidden: frozenset[str] = frozenset()):
        self.gc, self.mac, self.hidden = gc, mac, hidden
        # GC names in force: auto and hidden functions are unnamed.
        self.gname: dict[int, str] = {}
        for i, f in enumerate(gc.fns):
            if not AUTO.match(f.name) and f.name not in hidden:
                self.gname[i] = f.name
        self.all_gc_names = {f.name for f in gc.fns}
        self.mac_by_name: dict[str, list[int]] = defaultdict(list)
        for j, f in enumerate(mac.fns):
            if f.name and f.name not in BAD_NAMES:
                self.mac_by_name[f.name].append(j)
        self.mac_static = [mac.static_tokens(j) for j in range(len(mac.fns))]
        self.gc_static = [gc.static_tokens(i) for i in range(len(gc.fns))]
        self.post_static: dict[str, list[int]] = defaultdict(list)
        for j, toks in enumerate(self.mac_static):
            for t in toks:
                self.post_static[t].append(j)
        self.nmac = len(mac.fns)
        self.assigned: dict[int, tuple[str, int, dict]] = {}  # gc idx -> (name, mac idx, info)
        self.used_mac: set[int] = set()
        self._anchor_index = None

    # --- dynamic (call-graph) tokens ------------------------------------------------------
    def anchor_names(self) -> set[str]:
        names = {n for n in self.gname.values() if n in self.mac_by_name or n in self.mac.ext.values()}
        names |= {a[0] for a in self.assigned.values()}
        return names

    def gc_dyn(self, i: int, anchors: set[str]) -> dict[str, float]:
        toks: dict[str, float] = {}
        f = self.gc.fns[i]
        for j in f.callees:
            n = self.cur_name(j)
            if n in anchors:
                toks[f"c{n}"] = 1.0
        for j in f.callers:
            n = self.cur_name(j)
            if n in anchors:
                toks[f"r{n}"] = 1.0
        return toks

    def cur_name(self, i: int) -> str | None:
        if i < 0:
            return None
        if i in self.assigned:
            return self.assigned[i][0]
        return self.gname.get(i)

    def mac_dyn(self, anchors: set[str]) -> list[dict[str, float]]:
        out = []
        for f in self.mac.fns:
            toks = {}
            for j in f.callees:
                n = self.mac.callee_name(j)
                if n in anchors:
                    toks[f"c{n}"] = 1.0
            for j in f.callers:
                n = self.mac.fns[j].name
                if n in anchors:
                    toks[f"r{n}"] = 1.0
            out.append(toks)
        return out

    def mac_of(self, i: int) -> int | None:
        """The Mac function GC function I is known to be (an anchor or an accepted match)."""
        if i in self.assigned:
            return self.assigned[i][1]
        n = self.gname.get(i)
        js = self.mac_by_name.get(n, ()) if n else ()
        return js[0] if len(js) == 1 else None

    def gap_candidates(self, i: int) -> dict[int, float]:
        """Mac functions lying between the Mac images of GC function I's neighbouring anchors.

        Both builds keep most of the source order, so an unnamed function between two anchors
        is among the Mac functions between theirs; an equal-sized gap pins it by rank."""
        if self._anchor_index is None:
            self._anchor_index = sorted((k, self.mac_of(k)) for k in range(len(self.gc.fns)) if self.mac_of(k) is not None)
        idx = self._anchor_index
        pos = bisect.bisect_left(idx, (i, -1))
        prev = idx[pos - 1] if pos > 0 else None
        nxt = idx[pos] if pos < len(idx) else None
        out: dict[int, float] = {}
        if prev and nxt and 0 < nxt[1] - prev[1] <= GAP_MAX:
            gap_mac, gap_gc = nxt[1] - prev[1] - 1, nxt[0] - prev[0] - 1
            for j in range(prev[1] + 1, nxt[1]):
                bonus = GAP_BONUS
                if gap_mac == gap_gc and j - prev[1] == i - prev[0]:
                    bonus += GAP_RANK_BONUS
                out[j] = bonus
        elif prev:
            for d in (1, 2):
                out[prev[1] + d] = GAP_EDGE_BONUS / d
        elif nxt:
            for d in (1, 2):
                out[nxt[1] - d] = GAP_EDGE_BONUS / d
        return out

    # --- scoring --------------------------------------------------------------------------
    def candidate_pool(self) -> set[int]:
        taken = {self.cur_name(i) for i in range(len(self.gc.fns))} - {None}
        taken |= {f.name for i, f in enumerate(self.gc.fns) if f.name not in self.hidden and not AUTO.match(f.name)}
        pool = set()
        for j, f in enumerate(self.mac.fns):
            if f.name and f.name not in BAD_NAMES and f.name not in taken and j not in self.used_mac:
                pool.add(j)
        return pool

    def score_all(self, targets: list[int], strict: bool) -> dict[int, list[tuple[float, int, dict]]]:
        anchors = self.anchor_names()
        mdyn = self.mac_dyn(anchors)
        post_dyn: dict[str, list[int]] = defaultdict(list)
        for j, toks in enumerate(mdyn):
            for t in toks:
                post_dyn[t].append(j)
        pool = self.candidate_pool()
        # IDF over all Mac functions.
        idf_memo: dict[str, float] = {}

        def weight(t: str, m: float) -> float:
            w = idf_memo.get(t)
            if w is None:
                df = len(self.post_static.get(t) or post_dyn.get(t) or ())
                w = idf_memo[t] = math.log((self.nmac + 1) / (df + 0.5)) if df else 0.0
            return w * m

        wm: list[dict[str, float]] = []
        mtot: list[dict[str, float]] = []
        for j in range(self.nmac):
            toks = {**self.mac_static[j], **mdyn[j]}
            wm.append(toks)
            tot: dict[str, float] = defaultdict(float)
            for t, mult in toks.items():
                tot[t[0]] += weight(t, mult)
            mtot.append(tot)
        mstrong = [sum(t.get(k, 0.0) for k in STRONG) for t in mtot]
        out: dict[int, list] = {}
        for i in targets:
            g = self.gc.fns[i]
            gtoks = {**self.gc_static[i], **self.gc_dyn(i, anchors)}
            gtot: dict[str, float] = defaultdict(float)
            for t, mult in gtoks.items():
                gtot[t[0]] += weight(t, mult)
            gap_bonus = self.gap_candidates(i)
            quick: dict[int, float] = defaultdict(float)
            hits: dict[int, list[str]] = defaultdict(list)
            for j in gap_bonus:  # a gap candidate with no shared token still gets scored
                if j in pool:
                    quick[j] += 0.0
            for t, mult in gtoks.items():
                plist = self.post_static.get(t) or post_dyn.get(t) or ()
                if len(plist) > MAX_POSTING:
                    continue
                w = weight(t, mult) * MULT[t[0]]
                for j in plist:
                    if j in pool:
                        quick[j] += w
                        hits[j].append(t)
            gstrong = sum(gtot.get(k, 0.0) for k in STRONG)
            cands = []
            # Fully score only the best few by raw overlap plus the gap bonus.
            for j in heapq.nlargest(TOPK, quick, key=lambda j: quick[j] + gap_bonus.get(j, 0.0)):
                sh: dict[str, float] = defaultdict(float)
                for t in hits[j]:
                    sh[t[0]] += weight(t, gtoks[t])
                score = 0.0
                for k, mu in MULT.items():
                    s = sh.get(k, 0.0)
                    score += mu * (s - PEN[k] * (gtot.get(k, 0.0) + mtot[j].get(k, 0.0) - 2 * s))
                # CodeWarrior vs ProDG size: median ratio 0.84, 10-90% within 0.55-1.4 (named anchors).
                ratio = (g.size or 4) / max(self.mac.fns[j].size, 4)
                score -= SIZE_SLOPE * max(0.0, abs(math.log(ratio) + 0.17) - SIZE_FREE)
                score += gap_bonus.get(j, 0.0)
                kinds = {k for k, v in sh.items() if v > 0}
                # Specificity: shared weight of the identifying kinds (callee/caller/string/global/float),
                # and what fraction of the larger side's identifying weight it covers.
                sw = sum(sh.get(k, 0.0) for k in STRONG)
                big = max(gstrong, mstrong[j], 1e-9)
                cands.append((score, j, {"kinds": kinds, "hits": hits[j], "shared": dict(sh), "sw": sw, "cov": sw / big}))
            cands.sort(key=lambda c: -c[0])
            out[i] = cands[:5]
        return out

    def run_pass(self, strict: bool, rounds: int = 10) -> int:
        th = STRICT if strict else LOOSE
        total = 0
        for rnd in range(1, rounds + 1):
            self._anchor_index = None
            unnamed = [i for i, f in enumerate(self.gc.fns)
                       if i not in self.gname and i not in self.assigned and f.size >= 16]
            res = self.score_all(unnamed, strict)
            # Mutual best: for each Mac candidate, which GC function wants it most.
            best_for_mac: dict[int, tuple[float, int]] = {}
            for i, cands in res.items():
                for sc, j, _ in cands:
                    if j not in best_for_mac or sc > best_for_mac[j][0]:
                        best_for_mac[j] = (sc, i)
            new = {}
            for i, cands in res.items():
                if not cands:
                    continue
                s1, j, info = cands[0]
                s2 = next((c[0] for c in cands[1:] if self.mac.fns[c[1]].name != self.mac.fns[j].name), 0.0)
                rival = max((sc for sc, g2 in [(best_for_mac[j][0], best_for_mac[j][1])] if g2 != i), default=0.0)
                if best_for_mac[j][1] != i:
                    continue
                strong = len(info["kinds"] & set(STRONG))
                ratio = (self.gc.fns[i].size or 4) / max(self.mac.fns[j].size, 4)
                if (th["ratio"][0] <= ratio <= th["ratio"][1] and s1 >= th["score"] and s1 - s2 >= th["margin"] and strong >= th["kinds"]
                        and info["sw"] >= th["sw"] and info["cov"] >= th["cov"] and s1 - s2 >= th["rel"] * s1):
                    new[i] = (self.mac.fns[j].name, j, {"score": s1, "margin": s1 - s2, "round": rnd,
                                                         "strict": strict, **info})
            # One Mac name per GC function: drop name collisions between this round's picks.
            cnt = defaultdict(int)
            for v in new.values():
                cnt[v[0]] += 1
            new = {i: v for i, v in new.items() if cnt[v[0]] == 1}
            if not new:
                break
            for i, v in new.items():
                self.assigned[i] = v
                self.used_mac.add(v[1])
            total += len(new)
        return total

    def run(self) -> None:
        self.run_pass(True)
        self.run_pass(False)

    def leftovers(self) -> dict[int, tuple[float, int, dict]]:
        unnamed = [i for i, f in enumerate(self.gc.fns) if i not in self.gname and i not in self.assigned and f.size >= 16]
        res = self.score_all(unnamed, False)
        return {i: c[0] for i, c in res.items() if c and c[0][0] > 0}


STRONG = "crsdf"
STRICT = {"score": 20.0, "margin": 8.0, "kinds": 2, "sw": 15.0, "cov": 0.6, "rel": 0.35, "ratio": (0.4, 2.5)}
LOOSE = {"score": 8.0, "margin": 3.0, "kinds": 1, "sw": 3.0, "cov": 0.2, "rel": 0.2, "ratio": (0.0, 1e9)}


def tier(info: dict) -> str:
    return "HIGH" if info.get("strict") else "MEDIUM"


def evidence(m: Matcher, i: int, info: dict) -> str:
    byk = defaultdict(list)
    for t in info["hits"]:
        byk[t[0]].append(t[1:])
    parts = []
    for k, label in (("c", "callee"), ("r", "caller"), ("s", "str"), ("d", "global"), ("f", "float")):
        if byk.get(k):
            parts.append(f"{label}:" + ",".join(sorted(byk[k])[:4]) + (f"(+{len(byk[k]) - 4})" if len(byk[k]) > 4 else ""))
    for k, label in (("i", "imm"), ("o", "field"), ("m", "rot")):
        if byk.get(k):
            parts.append(f"{label}={len(byk[k])}")
    return "; ".join(parts)


def source_defined_names() -> set[str]:
    """Functions defined in src/: their names were fixed alongside matched C, so they are the
    most trustworthy ground truth for validation."""
    names: set[str] = set()
    for p in (ROOT / "src").rglob("*.c"):
        names |= set(re.findall(r"^\w[\w \*]*?\b(\w+)\([^;{]*\)\s*\{", p.read_text(errors="replace"), flags=re.M))
    return names


def validate(gc: Side, mac: Side, folds: int, seed: int) -> dict:
    probe = Matcher(gc, mac)
    anchors = [i for i, n in probe.gname.items() if n in probe.mac_by_name and gc.fns[i].size >= 16
               and len(probe.mac_by_name[n]) == 1]
    rng = random.Random(seed)
    rng.shuffle(anchors)
    stats = defaultdict(lambda: [0, 0])  # tier -> [proposed, correct]
    wrong = []
    in_src = source_defined_names()
    records = []
    for k in range(folds):
        held = anchors[k::folds]
        names = frozenset(gc.fns[i].name for i in held)
        m = Matcher(gc, mac, hidden=names)
        m.run()
        for i in held:
            if i in m.assigned:
                name, j, info = m.assigned[i]
                t = tier(info)
                stats[t][0] += 1
                src = gc.fns[i].name in in_src
                records.append({"name": gc.fns[i].name, "got": name, "ok": name == gc.fns[i].name, "src": src,
                                "strict": info["strict"], "round": info["round"], "score": info["score"],
                                "margin": info["margin"], "sw": info["sw"], "cov": info["cov"],
                                "kinds": len(info["kinds"] & set(STRONG)), "size": gc.fns[i].size})
                stats[t + "-src"][0] += src
                if name == gc.fns[i].name:
                    stats[t][1] += 1
                    stats[t + "-src"][1] += src
                else:
                    wrong.append((t, gc.fns[i].name, name, round(info["score"], 1), round(info["margin"], 1)))
        # fraction of held-out found at all
        stats["held"][0] += len(held)
        # background: assignments to genuinely unnamed functions (cannot be verified) by tier
        for i, (n, j, info) in m.assigned.items():
            if AUTO.match(gc.fns[i].name):
                stats["new-" + tier(info)][0] += 1
    return {"stats": dict(stats), "wrong": wrong, "n": len(anchors), "records": records}


def build_rows(m: Matcher) -> list[dict]:
    rejected = load_rejected()
    rows = []
    for i, (name, j, info) in sorted(m.assigned.items(), key=lambda kv: m.gc.fns[kv[0]].addr):
        g, mf = m.gc.fns[i], m.mac.fns[j]
        rows.append({"confidence": tier(info), "score": round(info["score"], 1), "margin": round(info["margin"], 1),
                     "round": info["round"], "gc_old": g.name, "gc_addr": f"0x{g.addr:08X}", "gc_size": f"0x{g.size:X}",
                     "new_name": name, "mac_raw": mf.raw, "mac_addr": f"0x{mf.addr:X}", "mac_size": f"0x{mf.size:X}",
                     "rejected": g.addr in rejected and name in rejected[g.addr],
                     "evidence": evidence(m, i, info)})
    for i, (sc, j, info) in sorted(m.leftovers().items(), key=lambda kv: -kv[1][0]):
        if sc < 4.0:
            continue
        g, mf = m.gc.fns[i], m.mac.fns[j]
        rows.append({"confidence": "LOW", "score": round(sc, 1), "margin": 0.0, "round": 0, "gc_old": g.name,
                     "gc_addr": f"0x{g.addr:08X}", "gc_size": f"0x{g.size:X}", "new_name": mf.name,
                     "mac_raw": mf.raw, "mac_addr": f"0x{mf.addr:X}", "mac_size": f"0x{mf.size:X}",
                     "rejected": g.addr in rejected and mf.name in rejected[g.addr], "evidence": evidence(m, i, info)})
    return rows


COLS = ["confidence", "score", "margin", "round", "gc_old", "gc_addr", "gc_size", "new_name", "mac_raw",
        "mac_addr", "mac_size", "evidence"]


def write_tsv(rows: list[dict]) -> None:
    with OUT_TSV.open("w") as f:
        f.write("\t".join(COLS) + "\n")
        for r in rows:
            if not r["rejected"]:
                f.write("\t".join(str(r[c]) for c in COLS) + "\n")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--validate", action="store_true", help="hold-out precision per tier")
    ap.add_argument("--dump", metavar="FILE", help="with --validate: write per-assignment records as JSON")
    ap.add_argument("--folds", type=int, default=5)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--emit-renames", metavar="FILE", help="write HIGH 'old new' pairs for batch_rename.py")
    ap.add_argument("--explain", nargs="+", metavar="GCNAME", help="hide these named functions and show their candidates")
    ap.add_argument("--no-write", action="store_true", help="do not write docs/symbol_donors/mac_code_match.*")
    args = ap.parse_args(argv)
    if not MAC_BIN.is_file():
        print(f"missing {MAC_BIN.relative_to(ROOT)} (gitignored; link it from the primary checkout)", file=sys.stderr)
        return 2
    gc, _, _ = load_gc()
    mac = load_mac()
    if args.validate:
        v = validate(gc, mac, args.folds, args.seed)
        if args.dump:
            import json
            Path(args.dump).write_text(json.dumps(v["records"]))
        print(f"hold-out over {v['n']} named GC functions that exist once in the Mac demo ({args.folds} folds)")
        for t in ("HIGH", "MEDIUM", "HIGH-src", "MEDIUM-src"):
            p, c = v["stats"].get(t, [0, 0])
            print(f"  {t:<10} proposed {p:4d} correct {c:4d} precision {c / p:.1%}" if p else f"  {t}: none")
        print("  new assignments (unverifiable): " + ", ".join(
            f"{k} {v['stats'][k][0]}" for k in ("new-HIGH", "new-MEDIUM") if k in v["stats"]))
        print(f"  recovered (any tier): {sum(v['stats'].get(t, [0, 0])[1] for t in ('HIGH', 'MEDIUM'))}/{v['n']}")
        for t, a, b, s, mg in v["wrong"]:
            print(f"  WRONG {t} {a} -> {b} score {s} margin {mg}")
        return 0
    if args.explain:
        names = frozenset(args.explain)
        m = Matcher(gc, mac, hidden=names)
        for nm in args.explain:
            i = next(k for k, f in enumerate(gc.fns) if f.name == nm)
            res = m.score_all([i], False)[i]
            print(f"== {nm} size 0x{gc.fns[i].size:X} ({len(gc.fns[i].facts.calls)} calls)")
            true = m.mac_by_name.get(nm, [])
            for sc, j, info in res:
                print(f"  {sc:7.1f} {mac.fns[j].name} size 0x{mac.fns[j].size:X}  {evidence(m, i, info)}")
            for j in true:
                print(f"  true: {mac.fns[j].name} size 0x{mac.fns[j].size:X} calls {len(mac.fns[j].facts.calls)}")
        return 0
    m = Matcher(gc, mac)
    m.run()
    rows = build_rows(m)
    n = defaultdict(int)
    for r in rows:
        n[r["confidence"]] += 1
    print({k: n[k] for k in ("HIGH", "MEDIUM", "LOW")})
    if not args.no_write:
        write_tsv(rows)
    if args.emit_renames:
        with open(args.emit_renames, "w") as f:
            for r in rows:
                if r["confidence"] == "HIGH" and not r["rejected"]:
                    f.write(f"{r['gc_old']} {r['new_name']}\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
