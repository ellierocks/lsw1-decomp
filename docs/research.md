# Decomp research

Use research to answer a concrete matching question: a prototype, field offset,
constant, callee identity or object boundary. Start with the target assembly
and current `config/GL5E4F/symbols.txt`; donor names and generated queues are
leads that need target evidence.

## Symbol recovery

```sh
python3 tools/ls1_match_plan.py nuanim
python3 tools/ls1_lookup_symbol.py 0x8001E76C
python3 tools/ls1_rename.py --suggest fn_8001E76C
python3 tools/ls1_rename.py --check fn_8001E76C ProposedName
```

After confirming identity, apply with `ls1_rename.py <old> <new>` and run
`bash build.sh`. Naming is separate from C matching progress. Prefer a direct
LSW1 Mac name with matching anchor/body evidence to a descriptive guess.
CrashWOC is the primary source-level Nu2 reference; verify its algorithm/types
against LSW1 because it uses a different compiler. LSW2 Mac is supporting
evidence for shared engine functions. Struct offsets and control flow must
agree with any proposed name.

[symbol_donors/README.md](symbol_donors/README.md) owns artifact provenance,
staleness and the corrected animation mappings. Check
`symbol_donors/rejected_renames.tsv` before proposing names. Rejections are per
address; do not infer identity from a stale name printed in a queue or task pack.

## Donor mining

Run only the stage needed for the selected target, or the full pipeline when
refreshing evidence deliberately:

```sh
python3 tools/binary_mining_pipeline.py --list
python3 tools/binary_mining_pipeline.py mac-anchor-queue
python3 tools/binary_mining_pipeline.py gc-data-xrefs
python3 tools/binary_mining_pipeline.py
```

### Outputs

| Stage | Purpose | Primary outputs |
| --- | --- | --- |
| `cross-lineage` | Extract PS2 retail executables, compare PS2/GC/Mac string evidence, and test USA 1.1 vs USA 2.0 GH branch signals. | `build/cross_binary_lineage/lineage_report.md`, `build/cross_binary_lineage/*.tsv` |
| `mac-anchor-queue` | Read Mac debug symbols directly from Mach-O binaries and align unnamed GC functions between already-named anchors. | `docs/symbol_donors/mac_anchor_rename_queue.md`, `docs/symbol_donors/mac_anchor_rename_queue.tsv` |
| `pc-versions` | Mine PC demo/retail strings shared with GC and emit DOL-addressed string proposals. | `build/pc_analysis/pc_analysis.txt`, `build/pc_analysis/pc_symbol_proposals.txt` |
| `gc-data-xrefs` | Scan GC code references to data labels and rescore Mac data symbol candidates. | `docs/symbol_donors/gc_data_xrefs.tsv`, `docs/symbol_donors/mac_data_to_gc_verified.md` |
| `sdk-islands` | Map likely Dolphin SDK/MSL/runtime islands for later known-library matching and splits. | `docs/symbol_donors/sdk_islands.md`, `docs/symbol_donors/sdk_islands.tsv` |
| `sdk-anchor-queue` | Consume local SDK symbol-order files and emit SDK rename candidates from named anchor gaps. | `docs/symbol_donors/sdk_anchor_rename_queue.md`, `docs/symbol_donors/sdk_anchor_rename_queue.tsv` |

The pipeline also writes `build/binary_mining_index.md`, which lists stage
readiness and output status.


Review outputs before applying renames. A pipeline run may rewrite committed
reports; keep regeneration separate from source tuning. Missing donor binaries
mean some stages cannot run. Cross-platform strings support lineage, not direct
GC-vs-MIPS/x86 instruction matching.

### SDK and runtime

Use `sdk-islands` to locate candidate ranges, then review
[sdk_islands.md](symbol_donors/sdk_islands.md) and
[CrashWOC SDK split families](symbol_donors/crashwoc_sdk_split_map.md).
For ordered SDK symbols from a map or debug build:

```sh
python3 tools/sdk_symbol_source_import.py /path/to/framework.map --out-dir docs/symbol_donors/sdk_symbol_sources/imported
python3 tools/binary_mining_pipeline.py sdk-anchor-queue
```

Review the queue against actual GC instructions. CrashWOC shares the platform
and engine family, but its compiler differs. Confirm SDK/object boundaries
before isolating a range from game code.

### Other binary sources

| Source under `orig/` | Practical use | Limit |
|---|---|---|
| `mac/` LSW1 demo | Closest debug-symbol names, prototypes and symbol order | Anchor/body evidence still required |
| `mac/` LSW2 | Shared Nu2 names and types | Different game/revision |
| `GL5P4F/` PAL GC | Regional deltas and string confirmation | Addresses differ from US |
| `lsw2-ps2-prototype/` | Nu2/game source paths, asserts and subsystem names | MIPS and newer lineage |
| `tcs-wii-prototype/DATA/` | Shared diagnostics, script/data grammar | Later engine, not direct function identity |
| `pc/usa/retail/`, `pc/usa/demo/` | Source breadcrumbs and string-label proposals | x86, stripped, platform-specific code |

The PS2 source paths distinguish `nu2api/nucore`, `nu2api/nu3d`,
`gameapi/ai/aisys`, `gameapi/camera/socksys`, and `gameapi/gui/gamemenu`.
TCS's loose `Levels/levels.txt`, `Levels/areas.txt` and `Scripts/SCRIPT.TXT`
can clarify record grammar. Confirm that grammar in LSW1 before typing its data.

Detailed cross-build evidence is retained under
[research/revisions](../research/revisions/revision_graph.md). Concrete donor
symbols, body matches and correction evidence remain under `symbol_donors/`.

## String references

```sh
python3 tools/string_xrefs.py
```

Outputs include `build/xrefs/string_xrefs.txt` and
`build/xrefs/nufile_string_xrefs.txt`. The scanner recognizes common PowerPC
address-materialization patterns (`lis` plus `addi`, load/store displacement
or `ori`). It can miss indirect references. Confirm the actual instruction
sequence and containing function before using a string as rename evidence.

## Ghidra

Optional control-flow reference; the matching build does not require it.
Put `analyzeHeadless` on `PATH` or set `GHIDRA_INSTALL_DIR` to the Ghidra
installation. The helper defaults to the retail DOL:

```sh
python3 tools/ghidra_decompile.py import-symbols
python3 tools/ghidra_decompile.py decompile Shop_UpdateSubMenu
python3 tools/ghidra_decompile.py decompile 0x801486FC
```

Projects live under `ghidra_out/project/`; exports under
`ghidra_out/decomp/<Function>.c`. Re-import after renames. Use exports to
understand branches and pointer roles, then write C against target assembly
and verify with ProDG. Ghidra output is not matching source to commit.

## RAM dumps

`tools/ramdump.py` reads raw Dolphin MEM1: 24 MiB (`0x1800000`), big-endian,
with address `0x80000000` at file offset zero. Capture a stable state using
Dolphin's memory debugger and save dumps under ignored `dumps/`. Register
labels in `dumps/manifest.txt` to compare states such as hub and shop.

```sh
python3 tools/ramdump.py read hub 0x80407B40 --type f32 --count 4
python3 tools/ramdump.py read hub 0x801D5338 --type ptr
python3 tools/ramdump.py sym hub NuRandSeed --type u32
python3 tools/ramdump.py whatis 0x8000C720
python3 tools/ramdump.py dump hub 0x80407B40 --len 0x80
python3 tools/ramdump.py strings hub --range 0x80400000 0x80410000
python3 tools/ramdump.py diff title level1 --section sdata
python3 tools/ramdump.py diff hub shop --type f32 --range 0x80400000 0x80420000
```

Use controlled state changes to test field offsets, enum values, live pointers
and globals. A dump is evidence for that execution; verify access widths and
layout in assembly before generalizing. [Menu state](menu_system.md) retains
watch addresses and their known field mappings.

## Subsystem evidence

- [Nu2 reference](nu2_engine_reference.md): donor layout sketches, SDK markers,
  file/datpack call relationships and the recovered camera state boundary.
- [Menus and shop](menu_system.md): descriptor/runtime layouts, callback stacks,
  category selection and transition state.
- [Scripts](scripts.md): parser anchors and AI/level dispatch indices.
- [Static call exports](recomp_exports/call_graph_summary.md) and
  [reset-path inspection](recomp_exports/menu_fullreset_startup_path.md):
  direct callsite evidence. Static callsite order does not prove runtime order;
  indirect calls and unresolved targets are absent from these exports.
