# Binary Mining Workflow

The binary-mining pass is the front door for extracting symbol and lineage
evidence from every donor binary currently in `orig/`.

Run the full refresh:

```sh
python tools/binary_mining_pipeline.py
```

Run individual stages:

```sh
python tools/binary_mining_pipeline.py --list
python tools/binary_mining_pipeline.py cross-lineage
python tools/binary_mining_pipeline.py mac-anchor-queue
python tools/binary_mining_pipeline.py pc-versions
python tools/binary_mining_pipeline.py gc-data-xrefs
```

## Stages

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

## Rename Flow

1. Refresh mining artifacts:

   ```sh
   python tools/binary_mining_pipeline.py
   ```

2. Review high-confidence Mac anchor candidates:

   ```sh
   sed -n '1,140p' docs/symbol_donors/mac_anchor_rename_queue.md
   ```

3. Apply candidates one at a time with the normal verifier:

   ```sh
   python tools/ls1_rename.py fn_801357A0 Condition_GlynTestInit
   ```

4. Re-run the pipeline after a rename batch. More anchor gaps become usable as
   `symbols.txt` gains names.

## Evidence Priority

Use this order when evidence conflicts:

1. Mac LSW1 Demo debug symbols with exact GC anchor gaps.
2. Mac LSW1 Demo symbol order plus close function size.
3. LSW2 Mac PPC for shared Nu2 engine functions.
4. PC/PS2/Xbox strings and source paths for subsystem/table names.
5. Later Traveller's Tales Mac debug binaries for broad Nu2 confirmation.

## SDK/Runtime Flow

1. Refresh the SDK map:

   ```sh
   python tools/binary_mining_pipeline.py sdk-islands
   ```

2. Review `docs/symbol_donors/sdk_islands.md`. Start with HIGH `sdk/GX`
   ranges, especially `0x8017CA2C-0x80182708`.
3. Review `docs/symbol_donors/crashwoc_sdk_split_map.md` for the CrashWOC
   source-tree families that line up with those islands.
4. Add a local CrashWOC or Dolphin SDK symbol-order source under
   `docs/symbol_donors/sdk_symbol_sources/`. CrashWOC is the best-case donor
   because it matches the same platform, engine family, and compiler. Map
   files from decomp projects and debug-build symbol tables work directly. If
   you start from a raw map, normalize it first with:

   ```sh
   python tools/sdk_symbol_source_import.py /path/to/framework.map --out-dir docs/symbol_donors/sdk_symbol_sources/imported
   ```
5. Refresh SDK rename candidates:

   ```sh
   python tools/binary_mining_pipeline.py sdk-anchor-queue
   ```

   Review `docs/symbol_donors/sdk_anchor_rename_queue.md` before applying
   anything.
6. Split confirmed SDK/runtime objects before attempting game/Nu2 C matching in
   the same tail range.

## Notes

- Cross-platform byte matching is intentionally not used for GC-vs-PS2 code
  closeness because GC/Mac are PowerPC while PS2 is MIPS.
- PS2 positional byte matching is still emitted for PS2 executable pairs only.
- The Mac anchor queue is conservative: high-confidence entries require exact
  named-anchor gaps from the LSW1 Demo donor plus enough size/order evidence.
