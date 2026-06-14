#!/usr/bin/env python3
"""Compare LSW1 binaries across PS2, GC, and Mac debug builds.

The useful direct byte comparison is within one CPU family only.  For the
GC-vs-PS2 branch question this script compares executable string evidence and
debug/source residue, then uses Mac debug symbols as named lineage donors.
"""

from __future__ import annotations

import csv
import re
import struct
import subprocess
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "build" / "cross_binary_lineage"
DOCS_DIR = ROOT / "docs" / "symbol_donors"

GC_US = ROOT / "orig" / "GL5E4F" / "sys" / "main.dol"
GC_PAL = ROOT / "build" / "region" / "GL5P4F" / "main.dol"
PS2_PROTO = ROOT / "orig" / "ps2-prototype" / "extracted" / "SLUS_999.99"

PS2_IMAGES = [
    (
        "ps2_eu_demo",
        ROOT / "orig" / "ps2-retail" / "eu demo" / "LEGO Star Wars - The Video Game (Europe) (Demo).bin",
    ),
    (
        "ps2_eu_1_0",
        ROOT / "orig" / "ps2-retail" / "eu 1.0" / "LEGO Star Wars - The Video Game (Europe) (En,Fr,De,Es,It,Nl,Da).bin",
    ),
    (
        "ps2_usa_1_1",
        ROOT / "orig" / "ps2-retail" / "usa 1.1" / "LEGO Star Wars - The Video Game (USA) (v1.01).bin",
    ),
    (
        "ps2_usa_2_0_gh",
        ROOT / "orig" / "ps2-retail" / "usa 2.0 gh" / "LEGO Star Wars - The Video Game (USA) (v2.00).iso",
    ),
]

MAC_BINS = [
    ("mac_lsw1_demo", ROOT / "orig" / "mac-debug-symbols" / "LEGO Star Wars Demo", "ppc"),
    ("mac_lsw2", ROOT / "orig" / "mac-debug-symbols" / "LEGO Star Wars II", "ppc"),
    ("mac_batman", ROOT / "orig" / "mac-debug-symbols" / "LEGO Batman", "i386"),
    ("mac_saga", ROOT / "orig" / "mac-debug-symbols" / "LEGO Star Wars Saga", "i386"),
    ("mac_indy", ROOT / "orig" / "mac-debug-symbols" / "LEGOIndianaJones", "i386"),
]

KEY_RE = re.compile(
    r"(Nu[A-Z]|instNu|AIPath|AISys|AIScript|Camera|SCRIPT|TRIGGER|"
    r"LEGO Options|Go To|Debug|debug|assert|ASSERT|\.c$|\.cpp$|gameapi|nu2api)"
)
SOURCE_RE = re.compile(r"(^|[/\\])([A-Za-z0-9_./\\-]+\.(c|cpp|h))$", re.IGNORECASE)
AUTO_NAMES = ("fn_", "lbl_", "jumptable_")


@dataclass(frozen=True)
class IsoFile:
    name: str
    lba: int
    size: int


@dataclass
class BinaryInfo:
    key: str
    label: str
    platform: str
    path: Path
    size: int
    strings: set[str]
    key_strings: set[str]
    source_paths: set[str]
    text_symbols: set[str]


def read_sector(image: Path, mode: str, lba: int) -> bytes:
    if mode == "iso2048":
        image_offset = lba * 2048
        payload_offset = 0
        sector_size = 2048
    elif mode == "mode2_2352":
        image_offset = lba * 2352
        payload_offset = 24
        sector_size = 2352
    else:
        raise ValueError(mode)
    with image.open("rb") as f:
        f.seek(image_offset)
        sector = f.read(sector_size)
    return sector[payload_offset:payload_offset + 2048]


def detect_iso_mode(image: Path) -> str:
    with image.open("rb") as f:
        f.seek(16 * 2048)
        if f.read(6)[1:6] == b"CD001":
            return "iso2048"
        f.seek(16 * 2352 + 24)
        if f.read(6)[1:6] == b"CD001":
            return "mode2_2352"
    raise RuntimeError(f"Could not detect ISO sector layout for {image}")


def parse_dir_records(data: bytes) -> list[IsoFile]:
    records: list[IsoFile] = []
    pos = 0
    while pos < len(data):
        length = data[pos]
        if length == 0:
            pos = ((pos // 2048) + 1) * 2048
            continue
        rec = data[pos:pos + length]
        if len(rec) < 34:
            break
        lba = struct.unpack_from("<I", rec, 2)[0]
        size = struct.unpack_from("<I", rec, 10)[0]
        flags = rec[25]
        name_len = rec[32]
        raw_name = rec[33:33 + name_len]
        name = raw_name.decode("ascii", errors="replace")
        if name not in ("\x00", "\x01"):
            name = name.split(";")[0]
            if flags & 0x02:
                name += "/"
            records.append(IsoFile(name, lba, size))
        pos += length
        if pos & 1:
            pos += 1
    return records


def read_iso_file(image: Path, mode: str, entry: IsoFile) -> bytes:
    chunks: list[bytes] = []
    sectors = (entry.size + 2047) // 2048
    for i in range(sectors):
        chunks.append(read_sector(image, mode, entry.lba + i))
    return b"".join(chunks)[:entry.size]


def iso_root_entries(image: Path) -> tuple[str, list[IsoFile]]:
    mode = detect_iso_mode(image)
    pvd = read_sector(image, mode, 16)
    if pvd[1:6] != b"CD001":
        raise RuntimeError(f"Bad PVD in {image}")
    root = pvd[156:190]
    root_entry = IsoFile("/", struct.unpack_from("<I", root, 2)[0], struct.unpack_from("<I", root, 10)[0])
    root_data = read_iso_file(image, mode, root_entry)
    return mode, parse_dir_records(root_data)


def extract_ps2_executable(key: str, image: Path) -> Path:
    out_path = OUT_DIR / "extracted" / key
    out_path.mkdir(parents=True, exist_ok=True)
    if key == "ps2_prototype":
        return PS2_PROTO

    mode, entries = iso_root_entries(image)
    table = {e.name.upper().rstrip("/"): e for e in entries}
    system = table.get("SYSTEM.CNF")
    if system is None:
        raise RuntimeError(f"SYSTEM.CNF not found in {image}")
    system_text = read_iso_file(image, mode, system).decode("ascii", errors="replace")
    match = re.search(r"cdrom0:\\([^;\\\r\n]+)", system_text, re.IGNORECASE)
    if not match:
        raise RuntimeError(f"Could not parse boot path in {image}")
    exe_name = match.group(1).upper()
    exe = table.get(exe_name)
    if exe is None:
        raise RuntimeError(f"{exe_name} not found in {image}")
    output = out_path / exe_name
    if not output.exists() or output.stat().st_size != exe.size:
        output.write_bytes(read_iso_file(image, mode, exe))
    return output


def run_strings(path: Path, min_len: int = 5) -> set[str]:
    result = subprocess.run(
        ["strings", "-n", str(min_len), str(path)],
        check=True,
        capture_output=True,
        text=True,
        errors="replace",
    )
    return {line.strip() for line in result.stdout.splitlines() if line.strip()}


def clean_symbol(name: str) -> str:
    if name.startswith("_"):
        name = name[1:]
    m = re.match(r"_?Z(\d+)([A-Za-z_]\w*)", name)
    if m:
        length = int(m.group(1))
        body = m.group(2)
        if len(body) >= length:
            return body[:length]
    return name


def macho_symbols(path: Path, arch: str) -> set[str]:
    if not path.exists():
        return set()
    cmd = ["llvm-nm", "-a"]
    if arch:
        cmd.extend(["-arch", arch])
    cmd.append(str(path))
    result = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
    names: set[str] = set()
    for line in result.stdout.splitlines():
        parts = line.split(None, 2)
        if len(parts) < 3:
            continue
        if parts[1] not in ("T", "t"):
            continue
        name = clean_symbol(parts[2])
        if name and not name.startswith(AUTO_NAMES):
            names.add(name)
    return names


def gc_named_symbols() -> set[str]:
    symbols = set()
    sym_path = ROOT / "config" / "GL5E4F" / "symbols.txt"
    if not sym_path.exists():
        return symbols
    for line in sym_path.read_text(errors="replace").splitlines():
        if "=" not in line:
            continue
        name = line.split("=", 1)[0].strip()
        if name and not name.startswith(AUTO_NAMES):
            symbols.add(name)
    return symbols


def source_paths(strings: set[str]) -> set[str]:
    return {s for s in strings if SOURCE_RE.search(s) or s.startswith("../") or "nu2api/" in s or "gameapi/" in s}


def byte_positional_match(a: bytes, b: bytes) -> tuple[int, int, float]:
    total = min(len(a), len(b))
    if total == 0:
        return 0, 0, 0.0
    matched = sum(1 for x, y in zip(a[:total], b[:total]) if x == y)
    return matched, total, matched / total


def jaccard(a: set[str], b: set[str]) -> float:
    if not a and not b:
        return 1.0
    if not a or not b:
        return 0.0
    return len(a & b) / len(a | b)


def coverage(a: set[str], b: set[str]) -> float:
    if not a:
        return 0.0
    return len(a & b) / len(a)


def build_infos() -> list[BinaryInfo]:
    infos: list[BinaryInfo] = []
    targets: list[tuple[str, str, str, Path]] = [
        ("gc_us", "GC USA GL5E4F", "gc", GC_US),
        ("gc_pal", "GC PAL GL5P4F", "gc", GC_PAL),
        ("ps2_prototype", "PS2 Prototype", "ps2", PS2_PROTO),
    ]
    for key, image in PS2_IMAGES:
        targets.append((key, key.replace("_", " ").upper(), "ps2", extract_ps2_executable(key, image)))
    for key, path, arch in MAC_BINS:
        targets.append((key, key.replace("_", " ").upper(), f"mac-{arch}", path))

    for key, label, platform, path in targets:
        if not path.exists():
            continue
        strings = run_strings(path)
        symbols = macho_symbols(path, platform.removeprefix("mac-")) if platform.startswith("mac-") else set()
        infos.append(
            BinaryInfo(
                key=key,
                label=label,
                platform=platform,
                path=path,
                size=path.stat().st_size,
                strings=strings,
                key_strings={s for s in strings if KEY_RE.search(s)},
                source_paths=source_paths(strings),
                text_symbols=symbols,
            )
        )
    return infos


def write_tsv(path: Path, rows: list[dict[str, object]], fields: list[str]) -> None:
    with path.open("w", newline="") as f:
        writer = csv.DictWriter(f, delimiter="\t", fieldnames=fields)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def main() -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    infos = build_infos()
    by_key = {info.key: info for info in infos}

    summary_rows = []
    for info in infos:
        summary_rows.append(
            {
                "key": info.key,
                "label": info.label,
                "platform": info.platform,
                "size": info.size,
                "strings": len(info.strings),
                "key_strings": len(info.key_strings),
                "source_paths": len(info.source_paths),
                "text_symbols": len(info.text_symbols),
                "path": info.path.relative_to(ROOT),
            }
        )

    string_rows = []
    byte_rows = []
    for i, left in enumerate(infos):
        for right in infos[i + 1:]:
            shared = left.strings & right.strings
            shared_key = left.key_strings & right.key_strings
            string_rows.append(
                {
                    "left": left.key,
                    "right": right.key,
                    "left_strings_in_right": f"{coverage(left.strings, right.strings):.4f}",
                    "right_strings_in_left": f"{coverage(right.strings, left.strings):.4f}",
                    "jaccard": f"{jaccard(left.strings, right.strings):.4f}",
                    "shared_strings": len(shared),
                    "shared_key_strings": len(shared_key),
                    "shared_source_paths": len(left.source_paths & right.source_paths),
                }
            )
            if left.platform == right.platform == "ps2":
                lb = left.path.read_bytes()
                rb = right.path.read_bytes()
                matched, total, ratio = byte_positional_match(lb, rb)
                byte_rows.append(
                    {
                        "left": left.key,
                        "right": right.key,
                        "matched_bytes": matched,
                        "compared_bytes": total,
                        "positional_match": f"{ratio:.4f}",
                        "left_size": len(lb),
                        "right_size": len(rb),
                    }
                )

    gc_named = gc_named_symbols()
    symbol_rows = []
    for info in infos:
        if not info.text_symbols:
            continue
        symbol_rows.append(
            {
                "mac_binary": info.key,
                "text_symbols": len(info.text_symbols),
                "already_named_in_gc": len(info.text_symbols & gc_named),
                "shared_with_lsw1_demo_symbols": (
                    len(info.text_symbols & by_key["mac_lsw1_demo"].text_symbols)
                    if "mac_lsw1_demo" in by_key and info.key != "mac_lsw1_demo"
                    else ""
                ),
            }
        )

    branch_rows = []
    branch_examples: dict[tuple[str, str], list[str]] = {}
    if "ps2_usa_1_1" in by_key and "ps2_usa_2_0_gh" in by_key:
        launch_unique = by_key["ps2_usa_1_1"].strings - by_key["ps2_usa_2_0_gh"].strings
        gh_unique = by_key["ps2_usa_2_0_gh"].strings - by_key["ps2_usa_1_1"].strings
        launch_unique_key = by_key["ps2_usa_1_1"].key_strings - by_key["ps2_usa_2_0_gh"].key_strings
        gh_unique_key = by_key["ps2_usa_2_0_gh"].key_strings - by_key["ps2_usa_1_1"].key_strings
        for probe_key in ("gc_us", "gc_pal", "mac_lsw1_demo", "ps2_eu_demo", "ps2_eu_1_0", "ps2_prototype"):
            probe = by_key.get(probe_key)
            if probe is None:
                continue
            launch_hits = probe.strings & launch_unique
            gh_hits = probe.strings & gh_unique
            launch_key_hits = probe.key_strings & launch_unique_key
            gh_key_hits = probe.key_strings & gh_unique_key
            branch_rows.append(
                {
                    "probe": probe_key,
                    "usa_1_1_unique_hits": len(launch_hits),
                    "gh_unique_hits": len(gh_hits),
                    "usa_1_1_unique_key_hits": len(launch_key_hits),
                    "gh_unique_key_hits": len(gh_key_hits),
                }
            )
            launch_examples = sorted(s for s in launch_hits if KEY_RE.search(s))
            gh_examples = sorted(s for s in gh_hits if KEY_RE.search(s))
            if not launch_examples:
                launch_examples = sorted(launch_hits)
            if not gh_examples:
                gh_examples = sorted(gh_hits)
            branch_examples[(probe_key, "usa_1_1")] = launch_examples[:40]
            branch_examples[(probe_key, "gh")] = gh_examples[:40]

    write_tsv(
        OUT_DIR / "binary_summary.tsv",
        summary_rows,
        ["key", "label", "platform", "size", "strings", "key_strings", "source_paths", "text_symbols", "path"],
    )
    write_tsv(
        OUT_DIR / "string_similarity.tsv",
        string_rows,
        [
            "left",
            "right",
            "left_strings_in_right",
            "right_strings_in_left",
            "jaccard",
            "shared_strings",
            "shared_key_strings",
            "shared_source_paths",
        ],
    )
    write_tsv(
        OUT_DIR / "ps2_positional_byte_match.tsv",
        byte_rows,
        ["left", "right", "matched_bytes", "compared_bytes", "positional_match", "left_size", "right_size"],
    )
    write_tsv(
        OUT_DIR / "mac_symbol_overlap.tsv",
        symbol_rows,
        ["mac_binary", "text_symbols", "already_named_in_gc", "shared_with_lsw1_demo_symbols"],
    )
    write_tsv(
        OUT_DIR / "usa_1_1_vs_gh_branch_strings.tsv",
        branch_rows,
        ["probe", "usa_1_1_unique_hits", "gh_unique_hits", "usa_1_1_unique_key_hits", "gh_unique_key_hits"],
    )

    focus_pairs = [
        ("gc_us", "ps2_usa_1_1"),
        ("gc_us", "ps2_usa_2_0_gh"),
        ("gc_pal", "ps2_usa_1_1"),
        ("gc_pal", "ps2_usa_2_0_gh"),
        ("mac_lsw1_demo", "ps2_usa_1_1"),
        ("mac_lsw1_demo", "ps2_usa_2_0_gh"),
    ]
    report = [
        "# LSW1 Cross-Binary Lineage",
        "",
        "Direct byte match is reported only for PS2 executable pairs. GC and Mac are PowerPC, while PS2 is MIPS, so cross-platform closeness uses executable strings, key diagnostics, source paths, and Mac symbol overlap.",
        "",
        "## Binaries",
    ]
    for row in summary_rows:
        report.append(
            f"- {row['key']}: {row['platform']}, {row['size']} bytes, "
            f"{row['strings']} strings, {row['key_strings']} key strings, "
            f"{row['source_paths']} source paths, {row['text_symbols']} text symbols"
        )

    report.extend(["", "## Focus: GC/Mac Against PS2 USA 1.1 vs USA 2.0 GH"])
    string_lookup = {(r["left"], r["right"]): r for r in string_rows}
    string_lookup.update({(r["right"], r["left"]): r for r in string_rows})
    for left, right in focus_pairs:
        row = string_lookup.get((left, right))
        if not row:
            continue
        report.append(
            f"- {left} vs {right}: shared={row['shared_strings']}, "
            f"key={row['shared_key_strings']}, source_paths={row['shared_source_paths']}, "
            f"jaccard={row['jaccard']}"
        )

    report.extend(["", "## PS2 Positional Byte Match"])
    for row in byte_rows:
        report.append(
            f"- {row['left']} -> {row['right']}: {float(row['positional_match']) * 100:.2f}% "
            f"({row['matched_bytes']}/{row['compared_bytes']})"
        )

    report.extend(["", "## Mac Debug Symbol Overlap"])
    for row in symbol_rows:
        report.append(
            f"- {row['mac_binary']}: {row['text_symbols']} text symbols, "
            f"{row['already_named_in_gc']} already named in GC symbols"
        )

    if branch_rows:
        report.extend(["", "## USA 1.1 vs GH Branch-Diagnostic Strings"])
        report.append("Counts below use strings that appear in one PS2 USA executable but not the other.")
        for row in branch_rows:
            report.append(
                f"- {row['probe']}: USA 1.1 unique hits={row['usa_1_1_unique_hits']} "
                f"(key={row['usa_1_1_unique_key_hits']}), GH unique hits={row['gh_unique_hits']} "
                f"(key={row['gh_unique_key_hits']})"
            )
        for probe_key in ("gc_us", "mac_lsw1_demo"):
            left = branch_examples.get((probe_key, "usa_1_1"), [])
            right = branch_examples.get((probe_key, "gh"), [])
            report.extend(["", f"### {probe_key} branch examples"])
            report.append("USA 1.1-only:")
            report.extend(f"- {s}" for s in left[:20])
            report.append("GH-only:")
            report.extend(f"- {s}" for s in right[:20])

    if "gc_us" in by_key and "ps2_usa_1_1" in by_key and "ps2_usa_2_0_gh" in by_key:
        gc = by_key["gc_us"]
        us11 = by_key["ps2_usa_1_1"]
        gh = by_key["ps2_usa_2_0_gh"]
        us11_score = (len(gc.key_strings & us11.key_strings), len(gc.source_paths & us11.source_paths), jaccard(gc.strings, us11.strings))
        gh_score = (len(gc.key_strings & gh.key_strings), len(gc.source_paths & gh.source_paths), jaccard(gc.strings, gh.strings))
        report.extend(["", "## Preliminary Read"])
        branch_gc = next((r for r in branch_rows if r["probe"] == "gc_us"), None)
        if branch_gc and branch_gc["usa_1_1_unique_key_hits"] > branch_gc["gh_unique_key_hits"]:
            verdict = "GC USA leans closer to PS2 USA 1.1 on branch-diagnostic executable strings."
        elif branch_gc and branch_gc["gh_unique_key_hits"] > branch_gc["usa_1_1_unique_key_hits"]:
            verdict = "GC USA leans closer to PS2 USA 2.0 GH on branch-diagnostic executable strings."
        elif branch_gc and branch_gc["usa_1_1_unique_hits"] > branch_gc["gh_unique_hits"]:
            verdict = "GC USA weakly leans closer to PS2 USA 1.1 on non-key branch-diagnostic executable strings."
        elif branch_gc and branch_gc["gh_unique_hits"] > branch_gc["usa_1_1_unique_hits"]:
            verdict = "GC USA weakly leans closer to PS2 USA 2.0 GH on non-key branch-diagnostic executable strings."
        elif gh_score > us11_score:
            verdict = "GC USA leans closer to PS2 USA 2.0 GH on executable string/source evidence."
        elif us11_score > gh_score:
            verdict = "GC USA leans closer to PS2 USA 1.1 on executable string/source evidence."
        else:
            verdict = "GC USA is effectively tied between PS2 USA 1.1 and USA 2.0 GH on this evidence."
        report.append(verdict)
        report.append(f"- USA 1.1 score tuple: key/shared source/jaccard = {us11_score[0]}/{us11_score[1]}/{us11_score[2]:.4f}")
        report.append(f"- USA 2.0 GH score tuple: key/shared source/jaccard = {gh_score[0]}/{gh_score[1]}/{gh_score[2]:.4f}")

    (OUT_DIR / "lineage_report.md").write_text("\n".join(report) + "\n")

    print(f"Wrote {OUT_DIR.relative_to(ROOT) / 'lineage_report.md'}")
    print(f"Wrote {OUT_DIR.relative_to(ROOT) / 'string_similarity.tsv'}")
    print(f"Wrote {OUT_DIR.relative_to(ROOT) / 'ps2_positional_byte_match.tsv'}")


if __name__ == "__main__":
    main()
