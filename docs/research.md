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

For OpenSaga, start with the [matching lane](symbol_donors/opensaga_matching.md):
pinned provenance, parser/helper queue, GC layout evidence, and the strict-match
gate for expansion into AI, animation and archive code.

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
| `mac-code-match` (`python3 tools/mac_code_match.py`) | Match unnamed GC functions to Mac demo functions by code body: callers/callees by name, immediates, field (width, offset), float literals, strings and order inside anchor gaps, propagated until stable. Tiered HIGH/MEDIUM/LOW with evidence; `--emit-renames FILE` writes HIGH pairs for `batch_rename.py`. Hold-out on named GC functions: HIGH 96.1% (98/102), MEDIUM 86.0% (labels are noisy: some repo names are themselves wrong). Families of near-identical functions (`Action_*`, `NuFPar*`, `CutScene*`) are the remaining false positives, so check MEDIUM by hand. | `docs/symbol_donors/mac_code_match.tsv` |
| `mac-protos` (`python3 tools/mac_protos.py --build`) | Demangle the Mac text symbols into int/float/pointer parameter lists for `tools/sweep.py` (unknown struct pointers become `void *`; by-value structs, 64-bit integers and function pointers are dropped). | `docs/symbol_donors/mac_lsw1_demo_prototypes.tsv` |
| `pc-versions` | Mine strings that any present PC build shares with GC and emit DOL-addressed string proposals. | `build/pc_analysis/pc_analysis.txt`, `build/pc_analysis/pc_symbol_proposals.txt` |
| `pc-anchors` | Pair GC functions with readable Japanese PC 1.0 x86 functions by shared strings, call order, pointer tables, globals and layout; map assert source paths to functions ([PC builds](#pc-builds)). | `build/pc_analysis/gc_pc_anchors.tsv`, `gc_pc_globals.tsv`, `pc_source_files.tsv` |
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
| `mac/` LSW1 demo | Closest debug-symbol names, prototypes and symbol order; code bodies via `mac_code_match.py` (names) and `mac_protos.py` (sweep prototypes) | CodeWarrior codegen does not transfer; only call graph, immediates, field offsets, floats and strings do. Mangled names carry parameter types, not return types |
| `mac/` LSW2 | Shared Nu2 names and types | Different game/revision |
| `GL5P4F/` PAL GC | Regional deltas and string confirmation | Addresses differ from US |
| `lsw2-ps2-prototype/` | Nu2/game source paths, asserts and subsystem names | MIPS and newer lineage |
| `tcs-wii-prototype/DATA/` | Shared diagnostics, script/data grammar | Later engine, not direct function identity |
| `pc/jpn/retail/` Japanese 1.0 | GC↔x86 function pairs, readable bodies, source-file ownership ([PC builds](#pc-builds)) | MSVC x86, stripped; PC-only code paths |
| `pc/usa/retail/`, `pc/usa/demo/`, `pc/patch2/` | Source breadcrumbs and string-label proposals | x86, stripped; SecuROM-wrapped, so code needs a runtime dump |

The PS2 source paths distinguish `nu2api/nucore`, `nu2api/nu3d`,
`gameapi/ai/aisys`, `gameapi/camera/socksys`, and `gameapi/gui/gamemenu`.
TCS's loose `Levels/levels.txt`, `Levels/areas.txt` and `Scripts/SCRIPT.TXT`
can clarify record grammar. Confirm that grammar in LSW1 before typing its data.

### PC builds

`research/revisions/builds.tsv` registers four PC builds. Only the Japanese 1.0
disc (`pc_jp_retail`, May 2005) ships an exe without SecuROM, so it is the one
PC build whose code can be read on disk; US retail, the US demo and patch 2
(`pc_patch2`, January 2006) carry a `.securom` section and keep game `.text`
encrypted until startup. Their strings remain readable for `pc-versions`.

Its game code follows the GC function structure closely: `PadOutPause`
(`0x8009F0B8`) and JP `0x00456850` test the same 13 globals in the same order.
`pc-anchors` uses that to pair functions:

```sh
python3 tools/binary_mining_pipeline.py pc-anchors   # tools/pc_gc_anchors.py
```

It pairs a GC function with the x86 function that references the same strings
(each string occurring once per binary), then repeats these steps to a fixed point:

| Method | Rule |
|---|---|
| `callee` | Callees of a paired function where both ordered call lists align. |
| `table` | Function-pointer tables of equal length that agree at two or more paired slots. |
| `global` | The only unpaired user of a paired global on each side, with matching shape. A global pair is a GC data symbol and an x86 address used together by every pair that uses either. |
| `layout` | Equal gaps of one or two functions between address-adjacent pairs. |
| `shape-callee`, `shape-layout` | Unequal gaps: the one candidate whose size, call count, float constants and strings fit. |

A step pairs a function only if it is the sole candidate both ways, its
paired callees mostly agree, the size ratio is sane and their message strings
(not source paths) overlap. Function starts and calls come from the system
`objdump`; without it, starts fall back to a byte scan that invents
mid-instruction starts, and the `global` and shape steps weaken.

`python3 tools/pc_gc_anchors.py --evaluate` hides 20% of the string pairs
(with their strings) over 20 seeds and counts how the other steps re-derive
them: 441 of 444 (99.3%). The three misses are one GC function,
`fn_8003BA5C`, which inlines the x86 helper its string sits in. Held-out
functions carry strings, so non-string functions may pair less accurately.
Of seven inferred pairs read in full on both sides, one (from the `global` step)
was wrong before the string veto and the shape check on that step; the other six,
and the pair that replaced the wrong one, are correct. Twelve more, sampled across
the `global` and shape methods, have the same callees in the same order. Rejected rules: looser call-list alignment (below
95%), pairing callers of paired functions (83%), and float constants alone (6 of
11), which now only support the shape test.

Outputs, in `build/pc_analysis/`:

- `gc_pc_anchors.tsv`: about 980 pairs, with the method in the evidence column
  and the independent audit (below) in the `audit` column.
- `gc_pc_globals.tsv`: about 180 global pairs.
- `pc_functions.tsv`: every x86 function start with its extent, direct caller
  count and pair.
- `pc_source_files.tsv`: the x86 functions that reference each `.\dir\file.c`
  assert path and their paired GC functions.

**Audit.** `python3 tools/pc_gc_anchors.py --audit` checks each pair without
strings, votes or layout. Distinctive integer immediates, float constants (x86
loads and float-like immediates) and paired callees that x86 calls or tail-jumps
to count for a pair; paired GC callees x86 never reaches count against it. String
pairs calibrate it against mispairs that reuse the same GC functions:

| Set | supported | partial | unchecked | weak |
|---|---|---|---|---|
| string pairs (249) | 72% | 24% | 0% | 3% |
| neighbour mispairs (next x86 start) | 6% | 14% | 0% | 80% |
| random mispairs | 0% | 1% | 0% | 98% |

So "supported" is strong evidence and "weak" is a reason to read both bodies, not
proof: LTCG inlines small callees (`NuToUpper` inside x86 `NuStrICmp`), which
reads weak. About 230 pairs, mostly `callee` leaves, have nothing to compare.
Of the weak inferred pairs read, `fn_80064088` (callee step picked a forwarder)
and `__fini_cpp_exceptions` (shape-layout) were wrong; a `none` row in
`pc_jp_pairs.tsv` vetoes a pair. A random sample of eight inferred pairs read
three right, four plausible and one too small to check. Vetoing weak candidates
inside the shape steps changed no held-out result, so it was not kept.
Seeding correct string-helper pairs (`NuStrLen`, `NuToUpper`, `NuStrIStr`,
`fn_800076E4`) raised held-out shape-layout errors from 3 to 7 while the steps
already derived all four, so those rows were dropped.

Task packs include the target's pair in `donor_evidence.md` and mark inferred
pairs. In a worktree, which has no `orig/pc`, the exe is taken from the primary
checkout, and the table is generated on first use (about 25 s) or after
`pc_jp_pairs.tsv` changes. `python3 tools/next_target.py --jp-paired` lists
ready targets that have a pair, and its `jp` column shows the x86 address.

PC struct layouts differ from GC. `fn_800F10B4` and JP `0x00468340` run the
same logic on shifted offsets (`+0x287` against `+0x290`, `+0xed8` against
`+0xeec`), so take field order from x86, never offsets.

`python3 tools/pc_gc_splits.py` projects the x86 file extents (MSVC links each
object contiguously, so the functions between the first and last loader of an
assert path belong to that file) onto GC address order. It writes boundary leads
to `pc_split_leads.tsv`; see [build configuration](build_config.md#japanese-pc-file-extents).

`tools/ls1_task_pack.py` and `tools/draft_fn.py` write `work/NAME/jp_x86.s` for
a paired target (`python3 tools/pc_gc_body.py NAME` prints it). It is the x86 body
with paired names for calls, globals and strings, float constants, switch tables
with their case ranges, and a header that aligns the x86 and GC call lists, compares
globals and branch counts, flags inferred pairs and shows the audit verdict. An
unpaired x86 callee where GC calls an unnamed function is a pair lead for naming.
With `setup-pc` (below), `draft_fn.py` also writes `jp_ghidra.c`. Use them for
branch structure, switches, constants and unknown globals. MSVC codegen does not
transfer, and PC-specific code differs: for example, JP `0x0041CD00` (GC
`fn_8011EAA8`) always returns 0, so PC never allows pausing a cutscene. After
reading both bodies, record the pair in
[pc_jp_pairs.tsv](symbol_donors/pc_jp_pairs.tsv), which seeds later runs, and
any global pairs in [pc_jp_globals.tsv](symbol_donors/pc_jp_globals.tsv).

Measured and not used for drafting:

- Argument counts from x86 call sites (`add esp` cleanup, pushes, `ret N`) agree
  with repo prototypes for 58% of 373 paired callees (push counts: 80%; strictest
  subset 84% of 127). LTCG passes arguments in registers and defers cleanup.
- Call order: the 371 matched functions with pairs show 0 reorders among 364
  shared calls, and GC assembly already fixes the order for m2c.
- Tail calls as pairing votes lowered `--evaluate` to 422 of 426; they feed the
  audit and the `jp_x86.s` call list only.

Treat pairs as donor evidence under the usual naming rules; renames are not
matching progress.

Detailed cross-build evidence is retained under
[research/revisions](../research/revisions/revision_graph.md). Concrete donor
symbols, body matches and correction evidence remain under `symbol_donors/`.

## Asset evidence

[Asset mining](asset_mining.md) ranks scripts, archives, animation/cutscene data,
scene references and controlled saves as evidence for specific native-code
questions. It records current input limitations, existing extractor coverage and
the proposed inventory → script-usage → handler workflow. Check actual file
presence; the historical revision registry is not a live availability report.

## String references

```sh
python3 tools/string_xrefs.py
```

Outputs include `build/xrefs/string_xrefs.txt` and
`build/xrefs/nufile_string_xrefs.txt`. The scanner recognizes common PowerPC
address-materialization patterns (`lis` plus `addi`, load/store displacement
or `ori`). It can miss indirect references. Confirm the actual instruction
sequence and containing function before using a string as rename evidence.

## Ghidra and REA

Both are optional control-flow references; the matching build needs neither. Their
output is never matching source, and it stays local because it is derived from the retail
binary. Write C against the target assembly and verify it with ProDG.

**Setup, once per checkout (about 4 s; 46 s for the first import).** `tools/bootstrap.sh` runs it when Ghidra 12.1 is
on PATH, in `GHIDRA_INSTALL_DIR` or in `/opt/ghidra` (`LSW1_NO_GHIDRA=1` skips it):

```sh
python3 tools/ghidra_decompile.py setup     # symbolized ELF + headless import + analysis
python3 tools/ghidra_decompile.py status    # prints the ELF path to open in REA
```

Ghidra cannot load a DOL with names, so `setup` first writes `main.elf`. It carries the
retail segments as named sections, every `symbols.txt` name with size and type, and
`_SDA_BASE_`/`_SDA2_BASE_` (r13 `0x8040FA40`, r2 `0x8041FA40`, read from `__start`).
A pre-analysis script sets r13 and r2, so small-data accesses resolve to `lbl_` globals.
State lives in `<primary checkout>/build/ghidra/<checkout>-<hash>/`, because Ghidra rejects
project paths that contain a `.`-prefixed element such as `.claude/worktrees`.
The first full import also publishes its analysed project to `build/ghidra/seed/<key>/`
(keyed by the DOL and the Ghidra scripts). Later checkouts copy that project and re-apply
their own `symbols.txt` names; `setup --force` re-imports from scratch. A checkout that only ran
`build.sh` (such as a fresh T3 worktree) gets the same copy on its first `draft_fn.py`, which
seeds the project whenever a matching seed exists (about 45 s once). It never starts a
full import implicitly.

**Headless (batch).** Use it for the sprint queue and for questions about globals:

```sh
python3 tools/ghidra_decompile.py decompile FN1 FN2 0x801486FC   # one JVM, work/FN/ghidra.c
python3 tools/draft_fn.py NAME                                   # m2c draft + ghidra.c if missing
python3 tools/ghidra_decompile.py xrefs lbl_80407A5C 0x80200080   # READ/WRITE sites by function
python3 tools/ghidra_decompile.py sync                            # after symbols.txt changes
```

`decompile` re-applies names automatically when `symbols.txt` has changed since the last
sync. `draft_fn.py` writes `work/NAME/ghidra.c` whenever the project exists and the file
is missing; `--ghidra` regenerates it and `--no-ghidra` skips it. A Ghidra GUI holding the
project open locks it; close the GUI first.

**REA (interactive).** The `rea` MCP server uses the same Ghidra install. Open the ELF that
`status` prints with `open_binary` (`provider_id: "ghidra"`). The first query imports and
analyses it, in about a minute. Then use `procedure_pseudo_code`, `procedure_assembly`,
`procedure_callers`/`procedure_callees`, `analyze_function`, `search_strings` and
`read_bytes` (data tables), and call `close_binary` when you are done. Two limits apply:

- REA imports without the r13/r2 context, so it shows small-data accesses as
  `*(unaff_r13 + -0x7fe4)` (address = `0x8040FA40` + offset; r2 uses `0x8041FA40`), and its
  `xrefs` misses those globals. Use the headless `xrefs` for them.
- `set_address_name` and annotations last only for the session. Durable names go through
  `symbols.txt` with the evidence rules above; regenerate the ELF (`ghidra_decompile.py elf`)
  before reopening.

**Japanese PC exe (opt-in, about 2 minutes once).** Bootstrap does not run it:

```sh
python3 tools/ghidra_decompile.py setup-pc            # import + analyse the JP exe, apply paired names
python3 tools/ghidra_decompile.py decompile-pc NAME   # x86 pair of a GC function -> work/NAME/jp_ghidra.c
```

The project sits next to the GC one. Paired GC names and global labels are applied
to the x86 addresses, and names from inferred pairs carry a `_jpinferred` suffix.
`decompile-pc` re-applies names when the pair tables change and creates functions
Ghidra missed (pointer-only targets). After `setup-pc`, `draft_fn.py` writes
`jp_ghidra.c` next to `jp_x86.s`. PC struct offsets never transfer.

Neither tool decodes Gekko paired-single instructions (`psq_l`, `ps_*`), which appear
mostly in SDK math. Read those functions from the assembly.

## RAM dumps

`tools/ramdump.py` reads raw Dolphin MEM1: 24 MiB (`0x1800000`), big-endian,
with address `0x80000000` at file offset zero. Capture a stable state with
`tools/dolphin.py snapshot <label>` while the game runs, or save one with
Dolphin's memory debugger, under ignored `dumps/`. Register labels in
`dumps/manifest.txt` to compare states such as hub and shop. Use `live` in place
of a dump to read the running game; [Dolphin runtime evidence](dolphin_runtime.md)
covers live reads, asking the user for a game state, and Gecko call tracing.

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
