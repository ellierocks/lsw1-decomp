#!/usr/bin/env python3
"""
Generate the revision graph from verified evidence.

Reads:
  research/revisions/builds.tsv
  research/revisions/similarity_matrix.tsv
  research/revisions/evidence.tsv  (manually curated, optional)

Outputs:
  research/revisions/revision_graph.md
  research/revisions/revision_graph.dot

Graph rules:
  - Edges only where we have measured evidence or verified observations.
  - Directional arrows imply temporal or development order only when evidence supports it.
  - Clusters group builds by verified payload identity.
  - Unknown relationships are left disconnected.

Usage:
    python3 tools/revisions/graph.py
"""
from __future__ import annotations

import csv
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILDS_TSV = ROOT / "research" / "revisions" / "builds.tsv"
MATRIX_TSV = ROOT / "research" / "revisions" / "similarity_matrix.tsv"
EVIDENCE_TSV = ROOT / "research" / "revisions" / "evidence.tsv"
GRAPH_MD = ROOT / "research" / "revisions" / "revision_graph.md"
GRAPH_DOT = ROOT / "research" / "revisions" / "revision_graph.dot"


# Manually encoded verified relationships from the research document.
# Each entry: (from_id, to_id, rel_type, confidence, evidence, score_pct)
# rel_type: ancestor_candidate / same_payload / major_divergence / unknown
VERIFIED_EDGES = [
    # PS2 family — temporal direction inferred from dates + prototype label
    ("ps2_prototype", "ps2_eu_demo", "precedes", "medium",
     "Positional byte match 19.2% (shared ~565K/2.94MB bytes). "
     "Prototype predates demo by date.", "19.2"),
    ("ps2_prototype", "ps2_eu_retail_v1", "precedes", "medium",
     "Positional byte match 19.3% (567K/2.94MB).", "19.3"),
    ("ps2_prototype", "ps2_us_retail_v1", "precedes", "medium",
     "Positional byte match 19.3% (567K/2.94MB).", "19.3"),
    ("ps2_prototype", "ps2_us_retail_gh", "precedes", "medium",
     "Positional byte match 19.0% (559K/2.94MB).", "19.0"),
    # EU Demo → Retail family: closely related but not identical
    ("ps2_eu_demo", "ps2_eu_retail_v1", "closely_related", "high",
     "Positional byte match 31.3% (993K/3.17MB). "
     "Similar size and string count; same SLED/SLES family.", "31.3"),
    ("ps2_eu_demo", "ps2_us_retail_v1", "closely_related", "high",
     "Positional byte match 31.0% (982K/3.17MB).", "31.0"),
    # EU 1.0 vs US 1.1: closest known retail pair
    ("ps2_eu_retail_v1", "ps2_us_retail_v1", "closely_related", "high",
     "Positional byte match 43.0% (1.36MB/3.17MB). "
     "Highest similarity in PS2 family; likely same codebase, region differences only.", "43.0"),
    # GH differs significantly from all v1.x
    ("ps2_us_retail_v1", "ps2_us_retail_gh", "diverged", "high",
     "Positional byte match 27.3% (866K/3.17MB). "
     "Significant executable differences; cause undetermined.", "27.3"),
    ("ps2_eu_retail_v1", "ps2_us_retail_gh", "diverged", "high",
     "Positional byte match 27.4% (868K/3.17MB).", "27.4"),
    # Verified demo payload clusters
    ("ps2_us_kiosk_demo", "ps2_eu_demo", "unknown", "none",
     "No byte comparison yet; separate demo families (US vs EU).", ""),
    # Xbox
    ("xbox_demo_oxm045", "xbox_retail", "precedes", "low",
     "Demo predates retail by date. No byte comparison yet; different file layout "
     "(demo is loose files, retail is XISO-packed).", ""),
    # PC: date ordering only
    ("pc_us_demo", "pc_us_retail", "precedes", "medium",
     "Exe dates: demo 2005-01-13, retail 2005-03-15. "
     "Both x86 PE; string comparison pending.", ""),
    # Cross-platform: not comparable by bytes; string evidence only
    ("gc_us_retail", "ps2_us_retail_v1", "same_game_codebase", "high",
     "String Jaccard 0.41 (4059 shared strings). "
     "Same Nu2 engine; different CPU arch (PPC vs MIPS).", ""),
    ("gc_us_retail", "gc_uk_retail", "closely_related", "high",
     "Both GL5xF GC builds; string overlap expected; byte comparison pending.", ""),
]

# Verified payload clusters (build_ids sharing identical payloads)
PAYLOAD_CLUSTERS = [
    {
        "cluster_id": "ps2_us_kiosk_2005",
        "label": "PS2 US Kiosk Demo Q2-Q3 2005",
        "members": ["ps2_us_kiosk_demo"],
        "carriers": ["OPS2M 107", "Jampack Vol 12", "Kiosk Q2-Q3 2005"],
        "evidence": "Verified: same LEGO Star Wars data size across all three carriers.",
        "confidence": "high",
    },
    {
        "cluster_id": "ps2_eu_demo_family",
        "label": "PS2 EU Demo Family",
        "members": ["ps2_eu_demo", "ps2_eu_bonus9_demo"],
        "carriers": [
            "Bonus Demo 9 EU", "OPS2M French Christmas Special",
            "OPS2M Special Edition 20", "OPS2M 59", "OPS2M 62", "OPS2M 79",
        ],
        "evidence": "Verified: same LEGO Star Wars data size across all listed carriers. "
                    "Byte identity vs ps2_eu_demo not yet confirmed.",
        "confidence": "medium",
    },
]


def load_builds() -> dict[str, dict]:
    result = {}
    if BUILDS_TSV.exists():
        with open(BUILDS_TSV, newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f, delimiter="\t"):
                result[row["build_id"]] = row
    return result


def load_matrix() -> list[dict]:
    if not MATRIX_TSV.exists():
        return []
    with open(MATRIX_TSV, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def load_evidence() -> list[dict]:
    if not EVIDENCE_TSV.exists():
        return []
    with open(EVIDENCE_TSV, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


PLATFORM_COLORS = {
    "gc": "#4e9a06",
    "ps2": "#3465a4",
    "xbox": "#006400",
    "pc": "#8f5902",
    "mac": "#75507b",
}

REL_STYLES = {
    "precedes": "dashed",
    "closely_related": "solid",
    "diverged": "bold",
    "same_game_codebase": "dotted",
    "unknown": "invis",
    "same_payload": "solid",
}

CONF_COLORS = {
    "high": "black",
    "medium": "#888888",
    "low": "#bbbbbb",
    "none": "#dddddd",
}


def write_dot(builds: dict, edges: list, clusters: list) -> str:
    lines = [
        "digraph lsw1_revisions {",
        '  graph [rankdir=LR fontname="Helvetica" splines=ortho];',
        '  node [shape=box fontname="Helvetica" fontsize=10];',
        '  edge [fontname="Helvetica" fontsize=8];',
        "",
    ]

    # Group by platform
    by_platform: dict[str, list[str]] = {}
    for bid, b in builds.items():
        plat = b.get("platform", "?")
        by_platform.setdefault(plat, []).append(bid)

    for plat, bids in sorted(by_platform.items()):
        color = PLATFORM_COLORS.get(plat, "#aaaaaa")
        lines.append(f'  subgraph cluster_{plat} {{')
        lines.append(f'    label="{plat.upper()}";')
        lines.append(f'    color="{color}";')
        lines.append(f'    style=rounded;')
        for bid in sorted(bids):
            b = builds[bid]
            avail = b.get("available", "no").strip()
            var = b.get("variant", "").strip()
            ver = b.get("version", "").strip()
            date = b.get("date", "").strip()
            label = f"{bid}\\n{var}"
            if ver:
                label += f" {ver}"
            if date:
                label += f"\\n{date}"
            style = "filled" if avail == "yes" else '""'
            fill = f'"{color}22"' if avail == "yes" else '"white"'
            lines.append(f'    "{bid}" [label="{label}" style={style} fillcolor={fill}];')
        lines.append("  }")
        lines.append("")

    # Edges
    for src, dst, rel, conf, evidence, score in edges:
        style = REL_STYLES.get(rel, "dashed")
        color = CONF_COLORS.get(conf, "#999999")
        label = rel
        if score:
            label += f"\\n{score}%"
        lines.append(
            f'  "{src}" -> "{dst}" [label="{label}" '
            f'style={style} color="{color}" fontcolor="{color}"];'
        )

    lines.append("}")
    return "\n".join(lines)


def write_md(builds: dict, edges: list, clusters: list, matrix: list) -> str:
    lines = [
        "# LSW1 Revision Graph",
        "",
        "Evidence-driven revision map. All edges carry explicit evidence and confidence levels.",
        "Disconnected nodes indicate builds with no measured relationship yet.",
        "",
        "---",
        "",
        "## Build Registry",
        "",
        "| Build ID | Platform | Region | Variant | Version | Date | Available |",
        "|----------|----------|--------|---------|---------|------|-----------|",
    ]
    for bid, b in sorted(builds.items()):
        lines.append(
            f"| `{bid}` | {b.get('platform','')} | {b.get('region','')} | "
            f"{b.get('variant','')} | {b.get('version','')} | {b.get('date','')} | "
            f"{'✓' if b.get('available','').strip()=='yes' else '–'} |"
        )

    lines += [
        "",
        "---",
        "",
        "## Verified Payload Clusters",
        "",
        "Builds confirmed to carry identical payloads (same LEGO Star Wars data).",
        "",
    ]
    for cl in clusters:
        lines.append(f"### {cl['label']}")
        lines.append(f"- **Members**: {', '.join(f'`{m}`' for m in cl['members'])}")
        lines.append(f"- **Carriers**: {', '.join(cl['carriers'])}")
        lines.append(f"- **Evidence**: {cl['evidence']}")
        lines.append(f"- **Confidence**: {cl['confidence']}")
        lines.append("")

    lines += [
        "---",
        "",
        "## Revision Relationships",
        "",
        "| From | To | Relationship | Confidence | Score | Evidence |",
        "|------|----|-------------|------------|-------|----------|",
    ]
    for src, dst, rel, conf, evidence, score in edges:
        if conf == "none":
            continue
        lines.append(
            f"| `{src}` | `{dst}` | {rel} | {conf} | "
            f"{'`' + score + '%`' if score else '–'} | {evidence[:100]}... |"
            if len(evidence) > 100 else
            f"| `{src}` | `{dst}` | {rel} | {conf} | "
            f"{'`' + score + '%`' if score else '–'} | {evidence} |"
        )

    lines += [
        "",
        "---",
        "",
        "## Similarity Matrix (measured)",
        "",
        "| Build A | Build B | Method | Score | Matched | Total |",
        "|---------|---------|--------|-------|---------|-------|",
    ]
    for row in sorted(matrix, key=lambda r: float(r.get("score", 0)), reverse=True)[:40]:
        lines.append(
            f"| `{row['build_a']}` | `{row['build_b']}` | {row['method']} | "
            f"`{float(row['score']):.3f}` | {row['matched']} | {row['total']} |"
        )

    lines += [
        "",
        "---",
        "",
        "## Open Questions",
        "",
        "- Are `ps2_eu_bonus9_demo` and `ps2_eu_demo` byte-identical?",
        "- Are `xbox_demo_oxm045` and `xbox_demo_oxm061` byte-identical?",
        "- Which build does `ps2_ops2m_93_demo` correspond to?",
        "- What is the relationship between `ps2_us_kiosk_demo` and `ps2_eu_demo`?",
        "- Does `mac_demo` share the same build date as `mac_demo_dbg`?",
        "- What changed between `ps2_us_retail_v1` and `ps2_us_retail_gh`?",
        "- What is the build date of the Xbox retail?",
        "",
    ]
    return "\n".join(lines)


def main():
    builds = load_builds()
    matrix = load_matrix()
    extra_evidence = load_evidence()

    edges = list(VERIFIED_EDGES)

    # Inject high-confidence pairs from similarity matrix
    for row in matrix:
        score = float(row.get("score", 0))
        method = row.get("method", "")
        bid_a, bid_b = row["build_a"], row["build_b"]
        # Flag near-identical same-arch pairs
        if method == "positional_byte" and score > 0.95:
            edges.append((
                bid_a, bid_b, "same_payload", "high",
                f"Positional byte match {score*100:.1f}% — near-identical executables.",
                f"{score*100:.1f}",
            ))
        elif method == "positional_byte" and score > 0.80:
            edges.append((
                bid_a, bid_b, "closely_related", "high",
                f"Positional byte match {score*100:.1f}%.",
                f"{score*100:.1f}",
            ))

    dot = write_dot(builds, edges, PAYLOAD_CLUSTERS)
    GRAPH_DOT.write_text(dot, encoding="utf-8")

    md = write_md(builds, edges, PAYLOAD_CLUSTERS, matrix)
    GRAPH_MD.write_text(md, encoding="utf-8")

    print(f"Wrote {GRAPH_MD}")
    print(f"Wrote {GRAPH_DOT}")
    print(f"  {len(builds)} builds, {len(edges)} edges, {len(matrix)} matrix rows")


if __name__ == "__main__":
    main()
