#!/usr/bin/env python3
"""
Match Mac LSW1 data/BSS symbols to GC lbl_* objects.

Approach: group by section category + size bucket, then for each GC object
list plausible Mac candidates with contextual evidence. No brute-force O(N*M).

Output: docs/symbol_donors/mac_data_to_gc_candidates.md
"""

import re
import subprocess
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
MAC_LSW1 = ROOT / "orig/mac/mac-debug-symbols/LEGO Star Wars Demo"
OUT_FILE = ROOT / "docs/symbol_donors/mac_data_to_gc_candidates.md"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s+//\s+(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")
ALIGN_RE = re.compile(r"\balign:(\d+)")
TYPE_RE = re.compile(r"\bdata:(\w+)")

CAT_MAP = {
    ".data": "data", ".bss": "bss",
    ".sdata": "sdata", ".sbss": "sbss",
    ".sdata2": "sdata2",
}
GC_CAT_ORDER = ["data", "sdata", "bss", "sbss", "sdata2"]


def classify(name: str) -> str:
    c = name.lstrip("_")
    if c.startswith("gNu") or c.startswith("Nu"): return "Nu2"
    if c.startswith("gAI") or "AISYS" in c: return "AI"
    if "Pad" in c or "Key" in c or "Mouse" in c: return "Input"
    if "Cam" in c or "View" in c: return "Camera"
    if "Menu" in c or "Gui" in c: return "Menu"
    if "Anim" in c or "Bone" in c: return "Anim"
    if "Sound" in c or "Sfx" in c or "Audio" in c: return "Audio"
    if "Part" in c or "Bolt" in c or "Blast" in c: return "FX"
    if "Force" in c or "Light" in c: return "FX"
    if "File" in c or "Stream" in c: return "FileIO"
    if "Script" in c or "Action" in c or "Cond" in c: return "Script"
    if "Heap" in c or "Alloc" in c or "Mem" in c: return "Memory"
    if "Save" in c or "Load" in c or "Config" in c: return "Config"
    if "Scene" in c or "World" in c or "Level" in c: return "Scene"
    if "Collis" in c or "Phys" in c: return "Physics"
    if "Font" in c or "Text" in c or "Draw" in c: return "Render"
    if "String" in c or "Debug" in c or "Print" in c: return "Debug"
    if c.startswith("g") and len(c) > 1 and c[1].isupper(): return "Global"
    return "Other"


def main():
    print("=" * 60, flush=True)
    print("Mac Data -> GC lbl_* Matcher (conservative)", flush=True)
    print("=" * 60, flush=True)
    ROOT.joinpath("docs/symbol_donors").mkdir(parents=True, exist_ok=True)

    # --- Load GC ---
    print("\n[1] GC objects...", flush=True)
    gc_all = []
    for line in GC_SYMBOLS.read_text().splitlines():
        m = SYMBOL_RE.match(line)
        if not m or not m.group("name").startswith("lbl_"):
            continue
        sec = m.group("section")
        cat = CAT_MAP.get(sec)
        if not cat:
            continue
        addr = int(m.group("addr"), 16)
        meta = m.group("meta")
        sm = SIZE_RE.search(meta)
        sz = int(sm.group(1), 16) if sm else 0
        am = ALIGN_RE.search(meta)
        al = int(am.group(1)) if am else 1
        tm = TYPE_RE.search(meta)
        dt = tm.group(1) if tm else "object"
        gc_all.append({
            'name': m.group("name"), 'sec': sec, 'cat': cat,
            'addr': addr, 'size': sz, 'align': al, 'dtype': dt,
        })
    print(f"  {len(gc_all)} data/bss objects", flush=True)

    # --- Load Mac ---
    print("\n[2] Mac LSW1 symbols...", flush=True)
    cmd = ["llvm-nm", "-a", str(MAC_LSW1)]
    res = subprocess.run(cmd, capture_output=True, text=True)
    raw = []
    for line in res.stdout.splitlines():
        parts = line.split()
        if len(parts) < 3:
            continue
        st = parts[1]
        if st not in ("D", "d", "B", "b", "S", "s"):
            continue
        try:
            addr = int(parts[0], 16)
        except ValueError:
            continue
        name = parts[2]
        if not name.startswith("_"):
            name = "_" + name
        cat = "data" if st in ("D", "d") else "bss" if st in ("B", "b") else "sdata"
        raw.append({'name': name, 'addr': addr, 'cat': cat, 'size': 0})

    raw.sort(key=lambda x: x['addr'])
    for i in range(len(raw) - 1):
        if raw[i]['cat'] == raw[i + 1]['cat']:
            gap = raw[i + 1]['addr'] - raw[i]['addr']
            if 0 < gap <= 0x100000:
                raw[i]['size'] = gap

    print(f"  {len(raw)} symbols", flush=True)

    # Group Mac by (cat, size) for efficient lookup
    mac_by_cat_size = defaultdict(list)
    for m in raw:
        mac_by_cat_size[(m['cat'], m['size'])].append(m)

    # Group GC by (cat, size)
    gc_by_cat_size = defaultdict(list)
    for g in gc_all:
        gc_by_cat_size[(g['cat'], g['size'])].append(g)

    # Build named-function set for "nearby" annotation
    gc_named = set()
    for line in GC_SYMBOLS.read_text().splitlines():
        m = SYMBOL_RE.match(line)
        if m and not m.group("name").startswith("lbl_") and not m.group("name").startswith("str_"):
            gc_named.add(m.group("name"))

    # Build nearby-named-func lookup for each GC object
    gc_nearby = {}
    for g in gc_all:
        near = [o['name'] for o in gc_all
                if o['name'] in gc_named and abs(o['addr'] - g['addr']) < 0x1000]
        gc_nearby[g['name']] = near[:6]

    # --- Match: only exact section + exact or close size ---
    print("\n[3] Matching (exact section + close size)...", flush=True)
    matched = []  # (gc_obj, mac_sym, score, evidence)

    # For each Mac symbol, find GC objects with same cat and matching size
    for mac in raw:
        mcat = mac['cat']
        msiz = mac['size']
        msub = classify(mac['name'])
        clean = mac['name'].lstrip("_")

        best_score = 0
        best_match = None

        for gc in gc_all:
            if gc['cat'] != mcat:
                continue
            if gc['size'] == 0 and msiz == 0:
                continue

            # Size comparison
            score = 0
            ev = []

            if msiz > 0 and gc['size'] > 0:
                if gc['size'] == msiz:
                    score = 10
                    ev.append("size:exact")
                else:
                    mx = max(msiz, gc['size'])
                    mn = min(msiz, gc['size'])
                    ratio = mx / mn if mn > 0 else 999
                    if ratio <= 1.1:
                        score = 7
                        ev.append("size:close")
                    elif ratio <= 1.5:
                        score = 4
                        ev.append("size:near")
                    elif ratio <= 2:
                        score = 2
                        ev.append("size:loose")
                    else:
                        continue  # size too different
            else:
                score = 1

            if score < best_score:
                continue

            # --- Alignment ---
            if gc['align'] > 1:
                if msiz > 0 and msiz % gc['align'] == 0:
                    score += 1

            # --- Data type hint ---
            if gc['dtype'] != "object":
                pass  # note but no score boost

            # --- Subsystem context ---
            if msub != "Other":
                score += 1
                ev.append(f"sub:{msub}")

            # --- Nearby named functions ---
            near = gc_nearby.get(gc['name'], [])
            if near:
                score += 1
                ev.append(f"near:{','.join(near[:2])}")

            # --- Name prefix alignment ---
            if clean[:8].lower() == gc['name'][4:12].lower():
                score += 3
                ev.append("name_hint")

            if score > best_score:
                best_score = score
                best_match = gc
                best_ev = ev
                best_mac = mac

        if best_match and best_score >= 0:
            matched.append({
                'mac': best_mac,
                'gc': best_match,
                'score': best_score,
                'ev': best_ev,
            })

    # Deduplicate per GC object (keep best Mac candidate)
    gc_to_best = {}
    for m in matched:
        k = m['gc']['name']
        if k not in gc_to_best or m['score'] > gc_to_best[k]['score']:
            gc_to_best[k] = m

    per_gc = sorted(gc_to_best.values(), key=lambda x: (-x['score'], x['gc']['addr']))

    high = [m for m in per_gc if m['score'] >= 10]
    med = [m for m in per_gc if 5 <= m['score'] < 10]
    low = [m for m in per_gc if m['score'] < 5]

    print(f"  Matched GC objects: {len(per_gc)}", flush=True)
    print(f"  HIGH: {len(high)}, MED: {len(med)}, LOW: {len(low)}", flush=True)

    # --- Write ---
    print(f"\n[4] Writing {OUT_FILE}...", flush=True)

    lines = [
        "# Mac LSW1 Data/BSS -> GC lbl_* Candidates",
        "",
        "Matched by exact section category + close size. No brute-force pairing.",
        "",
        "| Level | Score | Criteria |",
        "|-------|-------|----------|",
        "| HIGH | >= 10 | exact size + section + subsystem/nearby/name evidence |",
        "| MEDIUM | 5-9 | close size + section + some evidence |",
        "| LOW | < 5 | approximate match, needs investigation |",
        "",
        "---",
        "",
        "## HIGH Confidence",
        "",
        "| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | GC Align | GC Type | Sub | Evidence |",
        "|------------|--------|-----------|-------|--------|----------|---------|-----|----------|",
    ]

    for m in high:
        mac = m['mac']
        gc = m['gc']
        ev = "; ".join(m['ev'][:4])
        ms = f"~{mac['size']}" if mac['size'] else "?"
        lines.append(
            f"| `{mac['name'].lstrip('_')[:45]}` | {ms} | {gc['name']} | "
            f"0x{gc['size']:X} | {gc['cat']} | {gc['align']} | "
            f"{gc['dtype']} | {classify(mac['name'])} | {ev} |"
        )
    if not high:
        lines.append("| *(none)* | | | | | | | | |")

    lines.extend(["", "---", "## MEDIUM Confidence", "",
        "| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | Sub | Evidence |",
        "|------------|--------|-----------|-------|--------|-----|----------|",
    ])

    for m in med:
        mac = m['mac']
        gc = m['gc']
        ev = "; ".join(m['ev'][:4])
        ms = f"~{mac['size']}" if mac['size'] else "?"
        lines.append(
            f"| `{mac['name'].lstrip('_')[:45]}` | {ms} | {gc['name']} | "
            f"0x{gc['size']:X} | {gc['cat']} | {classify(mac['name'])} | {ev} |"
        )
    if not med:
        lines.append("| *(none)* | | | | | | |")

    lines.extend(["", "---", "## LOW Confidence (sample)", "",
        "| Mac Symbol | GC Object | GC Sz | GC Sec | Sub | Hint |",
        "|------------|-----------|-------|--------|-----|------|",
    ])

    for m in low[:100]:
        mac = m['mac']
        gc = m['gc']
        ev = "; ".join(m['ev'][:3])
        lines.append(
            f"| `{mac['name'].lstrip('_')[:45]}` | {gc['name']} | "
            f"0x{gc['size']:X} | {gc['cat']} | {classify(mac['name'])} | {ev} |"
        )

    lines.extend(["", "---", "## Unmatched GC Objects (no plausible Mac candidate)", "",
        "| GC Object | GC Sz | GC Sec | Nearby Named Funcs |",
        "|-----------|-------|--------|--------------------|",
    ])

    matched_names = {m['gc']['name'] for m in per_gc}
    for g in sorted(gc_all, key=lambda x: x['addr']):
        if g['name'] not in matched_names:
            near = gc_nearby.get(g['name'], [])
            nn = ", ".join(near[:4]) if near else "-"
            lines.append(f"| {g['name']} | 0x{g['size']:X} | {g['cat']} | {nn} |")

    lines.extend(["", "---", "## Stats", "",
        f"| Metric | Value |",
        "|--------|-------|",
        f"| GC data/bss objects | {len(gc_all)} |",
        f"| Mac data symbols | {len(raw)} |",
        f"| GC objects with a candidate | {len(per_gc)} |",
        f"| HIGH (score >= 10) | {len(high)} |",
        f"| MEDIUM (5-9) | {len(med)} |",
        f"| LOW (< 5) | {len(low)} |",
        f"| Unmatched GC objects | {len(gc_all) - len(per_gc)} |",
        "",
        "## Limitations",
        "",
        "1. Mac sizes are estimated from nm next-symbol gap (may include padding).",
        "2. No GC data xref scan — without knowing which functions access which",
        "   lbl_* objects, matching relies on size + section + name hints.",
        "3. A real matching tool would scan the DOL for lis+addi/lwz patterns",
        "   loading lbl_* addresses, then cross-reference with Mac access patterns.",
        "4. All matches below HIGH need manual verification before applying.",
    ])

    OUT_FILE.write_text("\n".join(lines) + "\n")
    print(f"\nDone. Review file ready.", flush=True)


if __name__ == "__main__":
    main()
