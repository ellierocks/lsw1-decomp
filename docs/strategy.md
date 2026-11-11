# Matching strategy

This is the project plan for agent-driven matching. It decides **what to work
on**. [C matching](code_matching_workflow.md) decides **how a candidate is
proved**. Read the current phase below before choosing a task.

## Why the plan changed

By October 2026 the project had matched 23 of 4,147 functions (0.13% of code),
after most of its effort had gone into symbol naming. Correctness was good: the
ProDG wiring, the strict verifier and the promotion checks all hold. The
bottleneck was throughput. One hand-tuned function per bounded ledger, with
each attempt going through ninja, cannot cover 4,147 functions. The plan
reorders the work by yield:

| Lane | Code share | Nature |
|---|---|---|
| Dolphin SDK import | about 12–15% | Integration: matched donor C already exists |
| `libsn` / libc | about 1% | newlib and libgcc DImode imported from upstream ([SN runtime](sn_runtime.md)); libsn and the debugger driver stay assembly |
| Small leaf functions (agent + permuter) | many functions | High throughput once tooling exists |
| Nu2 engine with CrashWOC/LSW2 donors | large | Algorithms carry over; code generation must be rematched |
| Game logic | the rest | Slowest; depends on shared types |

The matching metric (`tools/matching_progress.py`) is the measure of progress.
Naming only counts when it unblocks a matching target.

## Facts the plan rests on

- **Two compilers.** Game and Nu2 code use ProDG 3.9.3 (SN GCC 2.95). The
  Dolphin SDK is Nintendo's prebuilt CodeWarrior **GC/1.2.5n** output. The SDK
  banners in the DOL date from April/May 2004 (`0x2301`).
  [doldecomp/dolsdk2004](https://github.com/doldecomp/dolsdk2004) reconstructs
  the 20 April 2004 SDK. Unmodified dolsdk2004 sources compiled with its release
  flags give strict matches here, for example `OSSetAlarm`, `GXInitTexObj` and
  `SIBusy`'s code.
- **Reference implementation.** [RE4's decomp](https://github.com/adonis-singh/re4)
  uses the same ProDG 3.9.3 + MWCC 1.2.5n/dolsdk2004 mix. Copy its solutions
  before inventing new ones: `DolphinLib` flags and per-file overrides,
  `strip_unused.py` for linker-stripped functions, and the CRI/`libsn` handling.
- **The SN linker dead-strips unreferenced functions.** An SDK object whose
  donor source has extra functions needs RE4-style post-build stripping before
  it can be promoted.
- **TU anchors.** The DOL contains 91 `__FILE__` paths, such as
  `../nu2api.legogc/nu3d/ngc_obj/../nuanim.c`. Their xrefs mark real
  translation-unit boundaries.
- **The library region** runs from about `0x80150C3C` to the end of `.text`
  (OS, DVD, VI, PAD, SI, EXI, AI/AR/AX/DSP, CARD, GX, HVQM4, `libsn`).

## Phases

### Phase 0: foundations (done in the strategy PR)

- `tools/sdk_probe.py`: compile dolsdk2004 units, anchor them to retail by name,
  unique code bytes and layout, and classify each function with the strict verifier.
- `tools/score_fn.py`: compile-and-score a single function without ninja, at
  about 0.15 s per attempt (ProDG) or 0.5 s (MWCC). This is the inner loop for
  agents and the permuter.
- `verify_fn.py` accepts MWCC's halfword-placed `EMB_SDA21` relocations.
- Wrong SDK names in game code were removed. See `rejected_renames.tsv`.

### Phase 1: SDK import (complete)

Eighty-nine SDK units are promoted, and the whole image still matches retail. The per-import
figures are archived in [strategy progress](history/strategy_progress.md); the current totals are
in the README status block and [SDK import](sdk_import.md).

The donor queue is exhausted. Each remaining `sdk_probe.py --pending` row is
recorded in [SDK import](sdk_import.md) as not linked in retail or as a false
anchor: CARDErase, CARDProgram, SISteeringAuto, axfx, mtxstack, OSTimer and
odemustubs. These ranges stay assembly fallback and are deferred lanes, not
blockers:

- the HVQM4 decoder (`8018432C..8018A10C`), now split with a NonMatching
  candidate adapted from CC0 MSSB code and retail analysis; 18/43 functions
  (5,440 bytes) strictly verify, but no bytes are promoted. See the
  [parked HVQM4 evidence](sdk_import.md#hvqm4-decoder-parked);
- the debugger driver (`8018A10C..8018AB8C`), which needs a hand-written
  `DebuggerDriver.c`;
- `libsn` (`dummy`, `FSasync`, `sndvd`), which has no upstream source. The newlib and libgcc
  objects are imported; the remaining runtime misses are in [SN runtime](sn_runtime.md#not-imported).

SISamplingRate promotes its two functions, initialized data and four-byte
`SamplingRate` `.sbss`. BSS is verified by layout (see
[BSS layout policy](build_config.md#bss-layout-policy)); 24 SDK units own their
`.bss`/`.sbss`, and `ax/AXSPB` and `gx/GXDisplayList` stay on the
`--external-bss` fallback with recorded reasons.

1. Vendor the needed dolsdk2004 headers and sources under `src/dolphin/` and
   `include/dolphin/`. Record the upstream commit in `src/dolphin/README.md`.
2. Run `sdk_probe.py --pending --tsv work/sdk_pending.tsv` (or `--all`) for the import
   queue and `sdk_import_unit.py UNIT --apply` for units without initialised data
   ([quick import](sdk_import.md#importing-a-unit-quickly)). Use the `CODE_SAME` and
   `UNVERIFIED` rows to name SDK functions and data in `symbols.txt` from
   verified code identity. These names follow from matching code, which is the
   one case where naming is the matching task.
3. Add SDK splits library by library, in retail link order, using a
   `DolphinLib` entry per archive. Port RE4's `strip_unused.py` handling for
   donor functions the linker removed.
4. Promote each unit through the normal promotion checks. Fix per-file
   version drift (`MISMATCH` rows) by comparing with the retail code; the
   OS library was rebuilt in May 2004.

### Phase 2: whole-binary splits

1. Add `tools/tu_boundaries.py`. It combines the `__FILE__` string xrefs,
   alignment padding, Mac demo/lineage neighbours and SDK archive order into a
   proposed `splits.txt` for `.text` and data. **Started:** the `.text` pass
   exists and its proposal relinks byte-identically. Alignment padding carries
   no signal. `tools/rodata_splits.py` adds `.rodata` ranges for the 58 units whose
   literal references are unambiguous. Anonymous TUs, the other data sections and
   `.rodata` for TUs cut into several units remain; see
   [proposed TU boundaries](build_config.md#proposed-game-tu-boundaries).
2. Add a provisional unit per TU with assembly fallback. A split change must
   not change the relinked DOL. Mark boundaries provisional and revise them on
   evidence, never to raise a score. **Done:** 78 `auto/` `.text` units are
   committed. Phase 3 does not wait for the remaining boundary and data work.
   Revise a boundary when it blocks promoting a TU.

### Phase 3: agent draft-and-climb for game code

Use the [matching tools](sprint_workflow.md) to collect verified matches and improve
the workflow as gaps appear. Branch-based unit detection helps avoid overlapping
work, but does not cover arbitrary cross-unit changes. Confirm shared `nu2` and game
header fields against consumers. Results and
failure modes from the earlier multi-unit batches are in
[batch history](phase3_batches.md).

**Automatic progress.** `python3 tools/sweep.py --apply` drafts every ready target with m2c
(typed by the unit's preprocessed source), strict-scores each draft and writes matches into
their units. Each draft is then rewritten by registered repairs (prototypes, padding, sized
derefs, paired-single and carry idioms, and more in `tools/sweep_repairs/`); a repair is kept
only when it scores better. What stays UNHANDLED is recorded hand-work. Check applied matches
with `bash build.sh` and `verify_fn.py --strict-symbols`. Everything else is ranked in
`work/sweep/results.json` by size × exact-instruction ratio. Similarity helps prioritize but
does not predict how hard the remaining mismatch is. `sweep_permute.py --apply` searches the
>=90% near misses. `python3 tools/kickoff.py` shows the current figures and whether the sweep
is stale; rerun affected targets after header or tool changes. Dated figures, the repair list
and the remaining compile-error classes are in the [sweep history](phase3_batches.md#sweep-history).

Available approaches include m2c and Ghidra drafts, donor evidence, shared types,
manual iteration with `score_fn.py`, and automated search. The
[pattern library](code_matching_workflow.md#pattern-library) supplies matched ProDG
examples. `permute_setup.py NAME --run` prepares permuter search;
`order_search.py --climb` explores statement order. Choose their order and effort
based on the remaining mismatch, and record useful candidates with `hillclimb.py try`.
Accept functions with `verify_fn.py --build --strict-symbols`; whole-TU promotion
also requires section, relocation and linked-range checks.

Size, callees, type coverage and donor similarity help estimate useful targets.
The
[OpenSaga lane](symbol_donors/opensaga_matching.md) queues parser/helpers
before AI and animation and requires strict matches before expansion. Build
shared `nu2` struct headers once, from donor layouts confirmed against LSW1
field accesses. The initial OpenSaga parser/helper lane has promoted 1,460 bytes
(12 functions, 10 newly matched); [results](symbol_donors/opensaga_matching_results.md)
record the corrected parser layout and the next small-target queue. The
[donor audit](symbol_donors/opensaga_audit.md) extends that queue to small
string/math helpers, AI conditions, accessors and cutscene helpers, with
explicit exclusions for changed TCS behavior.

## Rules that do not change

- The strict verifier, pinned compilers and the retail DOL are the oracle.
  Never weaken them, and never move a split to improve a score.
- VERIFIED is not PROMOTED. Promotion needs unit sections, relocations and
  linked retail ranges with no regression.
- Donor code is evidence. A donor function counts only after it matches
  strictly with this game's compiler.
- Raw experiment logs, probe TSVs and proprietary artifacts stay under the
  ignored `work/` directory.

## Asset evidence for blocked targets

Use the [asset inventory and script index](asset_mining.md#available-tooling) when
a selected handler or loader needs argument, count, stride or format evidence.
Start with original LSW1 inputs and retain their hashes and source locations.
Lexical command hits are leads; connect them through GC parser/dispatch/access
evidence before expanding shared types. This supports matching and strict promotion gates; asset coverage is not matching progress.

## Choosing a task

1. Choose by expected verified code bytes per unit of effort, not phase number.
   The SDK donor queue is exhausted. Prioritize game/Nu2 matching in existing
   units; do remaining Phase 2 boundary and data work when it directly unblocks
   a promising unit. Resume the parked HVQM4 candidate from its strict-verification
   handoff when pursuing SDK work; libsn, debugger and the remaining
   [SN runtime](sn_runtime.md#not-imported) misses need concrete leads.
2. `python3 tools/kickoff.py` prints the shortlist (sweep lanes and `next_target.py --by-unit`
   units) with sibling-worktree state. Inspect active worktrees, branches and PRs before
   editing overlapping targets; branch-name ownership detection covers only its documented
   naming convention. Related families
   can reuse types and tooling fixes. Inspect owned data before assuming code matches
   permit promotion. When using a sprint queue, finish accounting includes every
   selected target.
3. When parking a target, retain its best score, remaining mismatch and a useful next
   hypothesis in the task's `status.md` and PR.
4. When the plan changes, update this file in the same PR. Sprint results stay in
   the PR.
