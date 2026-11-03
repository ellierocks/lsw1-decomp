# Mining assets as decompilation evidence

Use files outside `main.dol` to answer a selected function's questions about
arguments, record layout, format versions, pointer fixups or behavior. Asset
interpretation supports C reconstruction; it does not replace strict function,
unit, relocation and linked-range verification. Keep extracted assets, strings,
raw inventories and experiments local under ignored `work/` or `build/`.

## Availability and existing coverage

The checkout inspected for this investigation contained only the encrypted target
DOL under `orig/GL5E4F/sys/`; no disc images, loose game assets or donor binaries
were present. `research/revisions/builds.tsv` is a historical registry: its
`available=yes` entries are not current filesystem checks. Check each input before
using a recorded result or planning an extraction. Missing inputs are a limit,
not evidence that a format or command is absent from the game.

The older pipeline is mostly executable-oriented; the new inventory/index CLI is
described under [available tooling](#available-tooling):

- `tools/binary_mining_pipeline.py` checks its input paths and mines executable
  strings, symbols, cross-build relationships and SDK regions. It does not build
  an asset corpus.
- `tools/revisions/extract.py` extracts main executables, and can update the build
  registry; it is not an asset inventory command.
- `tools/revisions/formats/xiso.py` has `list_xiso` and directory traversal.
  `ps2_bin.py` has an ISO9660 directory parser used by executable extraction.
  These are potential starting points, not validated general asset extractors.
- `tools/revisions/formats/gc_iso.py` extracts only the DOL. GameCube filesystem
  table traversal is implemented separately by `tools/asset_mining.py`.
- `tools/extract_dispatch_tables.py` connects executable command strings to native
  handlers. It does not collect usages from shipped scripts. Its generated
  reports can contain stale names; resolve addresses against current symbols.

## Sources worth mining

The first three lanes are the best initial candidates. Later lanes should be
opened when a sprint needs their evidence, rather than as a broad cataloguing job.
File extensions and byte layouts must come from actual inputs and GC consumers;
function names below are leads in current symbols, not proof of file formats.

| Source | Concrete question it can answer | Connection to native code | First bounded experiment |
|---|---|---|---|
| Original LSW1 scripts and text configuration | Which arguments, scopes, defaults and state transitions does a command actually use? | Script keyword parser → stored argument → dispatch entry → action/condition handler | Collect every use of one ambiguous command, with surrounding state and source location; trace one argument through parser and handler. `.scp` and `script.txt` have loader evidence in [scripts](scripts.md). |
| Archive directory and packed payloads | How are names, offsets, sizes and packed/unpacked lengths represented? | `NuDatOpenEx`, `NuDatFileOpen`, `NuDatFileLoadBufferLsn`, `NuPPLoadBuffer` | Validate one directory entry and one payload against file bounds and the GC reader; establish uncompressed behavior before decoding packed data. Do not assume the donor archive layout or compression algorithm. |
| Animation and cutscene payloads | Which fields are counts, offsets, relative pointers, masks or format discriminants? | `NuAnimData2Fixup`, `NuAnimDataFixPtrs`, `NuGCutSceneFixUp`, camera/locator/rigid fixup helpers | Trace one offset through the GC fixup arithmetic, then check multiple records and a second file. Derive serialized fields separately from runtime structures. |
| Scene, hierarchical model and level data | What do locator names, spline indices, object references and record strides mean? | `NuGHGReadEx`, `NuSplineFind`, script locator actions and cutscene locator helpers | Join one script reference to a named scene record and its lookup routine; confirm count/stride and reference resolution from GC instructions. Identify actual container/chunk conventions first. |
| Controlled saves and configuration outputs | Which state persists, at what width, and with what checksum/version handling? | Save/menu code under `auto/gui/gamemenu/apisave.c` and `auto/gamelib/ngc/saveload.c` | Compare repeated baseline saves, then change one known game value and save again. Trace changed bytes through serialization; timestamps, checksums and platform wrappers can create unrelated changes. These are proposed experiments, not available samples. |
| Localization and string tables | How are identifiers, language selection, encoding and fallback represented? | Text parser, Unicode conversion and consumers found through strings/xrefs | Follow one visible string from an actual table to its lookup and caller. Distinguish asset IDs from pointers and formatting arguments. BOM recognition does not prove full Unicode semantics. |
| Audio banks, stream metadata and texture assets | Which IDs, size units, alignment and resource-lifetime rules do loaders expect? | `nusound3/ngc` units and texture readers such as `NuTexReadBitmapMMEx` | Inspect one resource header beside its GC reader. Prefer metadata over decoding media; do not infer platform-specific payload equivalence from matching names. |
| Debug ELF/map files and other-build collateral | Which functions, data owners or source boundaries can be recovered directly? | Existing Mac, CrashWOC and Nemo donor lanes | Check the registered collateral paths and existing reports before reimplementing symbol mining. Determine whether symbols/types actually exist; an embedded PDB path is not the PDB itself. |

## Evidence already supporting these lanes

- [Script handlers](game_structs.md#confirmed-fields) have strictly matched
  consumers that establish AI packet fields and a shared timer read/write pair.
  This is demonstrated code/type recovery, although systematic mining of actual
  script usages now has lexical indexing support, pending original input files.
- [Parser/file results](symbol_donors/opensaga_matching_results.md) prove GC
  numeric readers, parser creation and command-stack operations. They also show
  why donor pool sizes, slots and return contracts must not be imported blindly.
- [File call evidence](nu2_engine_reference.md#file-and-datpack-call-evidence)
  records the DAT-context branch and LSN/fallback read path. The declarations in
  `src/nucore/nufile.h` contain archive and block sketches; their presence does
  not establish a verified on-disc schema or bitfield encoding.
- [Animation evidence](symbol_donors/opensaga_animation_evidence.md) identifies
  legacy interpolation semantics and a GC/donor mask-byte-order difference.
  A later game's file cannot establish an LSW1 format by itself.
- [Donor audit](symbol_donors/opensaga_audit.md#observed-differences-and-transfer-limits)
  documents later handlers that became stubs while LSW1 retained behavior.
  Original LSW1 content is particularly useful for checking such differences.

## First implementation and sprint integration

Use a read-only filesystem inventory for an original LSW1 disc or loose
installation. Record build identity, source hash, container/member path, byte
range, length and extraction method. Separate observed extensions/magic from
inferred types. For archives, retain the outer-file and member provenance.
Validate bounds before reading or extracting, and never overwrite the input.
Hash logical file contents to find duplicates across demos/platforms; executable
similarity and disc dates do not establish asset identity.

Then generate a script command-usage index for one available original LSW1 corpus.
Preserve encoding and source byte/line locations; support comments, quoting and
state/scope context before treating text matches as parsed commands. Retain
unparsed constructs instead of silently dropping them. Join command names to
retail dispatch addresses, including init/runtime distinctions, and resolve
current symbols by address. Usage frequency helps select examples; it does not
prove runtime reachability or imply that an unused handler was unnecessary.

For each sprint target, retain a small evidence record:

1. Target address, concrete unknown and asset/build provenance.
2. Observation with member path and byte/line location.
3. GC instruction/call evidence connecting the observation to a field or argument.
4. Accepted facts, contradictory examples and remaining uncertainty.
5. Candidate change, exact verification result and promotion status.

A good first task is an ambiguous small script handler whose argument meaning
blocks a shared header. A good binary-format task is a small fixup helper whose
count/stride can be checked against two actual files. Stop research once it
answers the matching question; retain the existing one-unit sprint workflow.
Do not regenerate every historical report or rename symbols merely because an
asset name resembles a function. Controlled asset mutations or runtime probes
are follow-up experiments only after a baseline interpretation is established.

## Available tooling

`tools/asset_mining.py` implements the first inventory and lexical-index steps.
It needs no external packages and never extracts or modifies input files. Reports
must go to new paths under this checkout's ignored `work/` or `build/` directories;
it refuses existing outputs and outputs inside a loose input corpus.

```sh
python3 tools/asset_mining.py inventory /path/to/disc.iso --kind gc --build GL5E4F --output work/assets/gc.json
python3 tools/asset_mining.py inventory /path/to/installation --kind loose --build pc_demo --output work/assets/pc.json
python3 tools/asset_mining.py scripts work/assets/gc.json --output work/assets/scripts.json
python3 tools/asset_mining.py scripts work/assets/gc.json --suffix .scp --suffix .txt --dol orig/GL5E4F/sys/main.dol --output work/assets/linked-scripts.json
```

Inventory records absolute source paths, build labels, source/member SHA-256,
file ranges, observed suffixes and the first 16 bytes as hex. Equal member hashes
identify byte duplicates without implying shared native implementations. GameCube
FST parsing checks root/count, directory nesting/parents, names, duplicate paths
and file bounds. Loose inventory rejects symlinks. Disc hashing reads the whole
image once in addition to member hashing. Xbox/PS2 inventory and DAT member
unpacking remain future work; this command does not call the older extractors.

Script selection defaults to `.scp`; add suffixes explicitly for configuration
files after inspecting the inventory. The index verifies source/member hashes
before reading, limits each script to 16 MiB, recognizes UTF-8 and BOM-marked
UTF-16 LE/BE, and reports decoding failures rather than guessing an encoding.
It retains unknown token sequences, byte ranges, line/column and lexical brace
context. Quoted strings and `//`/`/* */` comments are excluded from command
matching. Unbalanced braces, unmatched text and unfinished comments generate
diagnostics. Brace labels are nearby lexical text, not proven language scopes;
there is no claim to support every LSW1 grammar or comment convention.

With `--dol`, the CLI requires the pinned GL5E4F retail SHA-1 and a `GL5E4F`
build label. It reads the six known keyword/AI/level dispatch tables through the
DOL's actual section mappings, retains same-name entries across tables, and
resolves exact handler addresses against the supplied current symbols. The third
word of condition entries is retained without assuming its meaning. DOL and
symbol-file hashes accompany the index. It performs no automatic renames.
Without a DOL, the index still retains lexical records but has no handler links.
Matching is exact-case, at any unquoted word position: **a candidate occurrence
is not a parsed command**, and its following tokens are not proven arguments.
Inspect the original source and parser before using it to type a native field.

Synthetic tests cover nested/empty disc members, invalid ranges/parents/paths,
changed inputs, symlinks, UTF-16 offsets, comments, quotes, unknown syntax and
cross-table command collisions. No actual assets or decrypted retail DOL were
available for local validation; run the tools on original files before drawing
format conclusions. No matching bytes are claimed by adding these tools.
