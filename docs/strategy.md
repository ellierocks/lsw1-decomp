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

- the HVQM4 decoder (`8018432C..8018A10C`), which has no donor source;
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

Throughput is the measure. Run Phase 3 as [sprints](sprint_workflow.md): one
thread, one unit, an explicit queue, measured active time and score-based limits per function, and one
PR that merges as soon as CI passes. Parallel work means separate sprint threads on
separate units, not workers inside a thread. Grow the shared `nu2` and game headers
inside sprints: confirm a field when a queued function accesses it. Results and
failure modes from the earlier multi-unit batches are in
[batch history](phase3_batches.md).

**Biggest progress first.** Every Phase 3 round starts with the unattended sweep,
`python3 tools/sweep.py --apply` (about two minutes, no model or agent). It drafts every ready
target with m2c, strict-scores each draft and writes the matches into their units.
m2c reads the preprocessed unit source as `--context`, so typed headers type the drafts;
each target keeps the better of its typed and untyped drafts, and drafts at 80% exact or
better also try the [store-in-arms and const `.rodata` rewrites](code_matching_workflow.md#measure-and-iterate).
Each draft is also scored with its signature padded to the argument registers m2c
numbered (`arg2` alone becomes `s32 pad0, s32 pad1, ... arg2`), keeping the better.
Drafts are then repaired and rescored: m2c's guessed callee prototypes give way to the
unit's or the repo's (call arguments trimmed or padded to match), a callee the target calls
after `crclr`/`crset cr1eq` is declared variadic or unprototyped, and undeclared stack slots
(`spC` beside `&sp8`) fold into an array. That cut failing drafts from 340 to 184. The pinned m2c carries
`tools/patches/m2c-crfields.patch` (applied by `tools/fetch_m2c.sh`, idempotent): per-field
`crN` bits, `cror`/`crand`/... folding, `mfcr` + `rlwinm` bit extraction, `rotlw`, and the
GCC 2.95 carry idioms (`subfic/adde` = `x == 0`, `addic/subfe` = `-(y == 0)`, `subfic/subfe/neg`
= `x >u K`). That took UNHANDLED targets from 359 functions (453 KB) to 38 (32 KB) and drafted
about 257 KB of new code. The sweep also strips the varargs `stfd` prologue, declares
unset-register and `saved_reg_*` reads as locals, adopts the unit's own prototype for the
target and fixes callee arity from the unit's declarations. What stays UNHANDLED is recorded
hand-work: paired-single code beyond the vector add/sub block (`ps_mul`/`madd`/`sum`/`merge`
in `NuVecMagSqr`, `NuVecCross`, `NuVecMtxTransform`, `NuMtxMul`, `fn_8000EFE0`, and the
`psq_l`/`psq_st` dumps in the runtime region past 0x801503A0), supervisor code (`mtmsr`,
`mfspr HID`, BATs) and 64-bit carry pairs. The game's 8-instruction `psq_l`/`ps_add|ps_sub`/
`psq_st` block (24 functions, mostly `chris`/`podrace`) is source-level inline asm:
`src/numath/nuvec_ps.h` holds it as `NuVecAddPS`/`NuVecSubPS`, `fold_ps` turns each block into
a call m2c can read and `fix_ps_calls` keeps the arguments to `(dst, a, b)`. That drafts all
24 functions; `NuVecAdd`/`NuVecSub` match from it, and the rest still need typing. Many new drafts still fail to compile on `void *` member access, irregular
switches and stack-address typing; those need the shared headers. The
first sweep matched 183 functions (13,608 verified bytes) in under ten minutes of wall
time, more than any agent sprint so far. Check its output with `bash build.sh` and
`verify_fn.py --strict-symbols` on each applied function. Everything that did not match is
ranked in `work/sweep/results.json` by size × exact-instruction ratio; that ranking is the
sprint queue. Large near-misses (90% exact or better) come before small fresh targets,
because the draft is already most of the way there. Rerun the sweep after header or
tool changes that could turn more drafts into matches. Then run `python3 tools/sweep_permute.py --apply`.
It permutes the top near-misses (90% exact or better, largest first) with the strict-scored
permuter under a time limit per function and applies any strict match. Agents take what is
left.

For each queued function:

1. **Draft.** Combine m2c (PPC) output, the Ghidra output (`work/NAME/ghidra.c` from `draft_fn.py`), CrashWOC/Nu2 donor
   C and context from shared headers.
2. **Agent search.** Iterate with `score_fn.py`, recording candidates worth
   keeping through `hillclimb.py try`. When the C behind a mismatching instruction
   shape is unclear, look it up in RE4's matched ProDG code and LSW1's own matches with
   the [pattern library](code_matching_workflow.md#pattern-library).
3. **Permuter.** Run decomp-permuter on functions that are close (about 95%
   exact or better) with `tools/permute_setup.py NAME --run`, focused on the differing
   region. Use `order_search.py --climb` instead when only the statement order differs.
4. **Accept.** Use `verify_fn.py --build --strict-symbols`. Promote whole TUs
   when every function matches. One sprint produces one PR.

Order the queue by size, callee count, type coverage and donor similarity.
Small leaf functions come first. The
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
evidence before expanding shared types. This supports the existing one-unit
sprints and strict promotion gates; asset coverage is not matching progress.

## Choosing a task

1. Choose by expected verified code bytes per unit of effort, not phase number.
   The SDK donor queue is exhausted. Prioritize game/Nu2 matching in existing
   units; do remaining Phase 2 boundary and data work when it directly unblocks
   a promising unit. Keep libsn, debugger, HVQM4 and the remaining [SN runtime](sn_runtime.md#not-imported) misses deferred until a
   concrete donor or implementation lead changes their expected yield.
2. Run the sweep first (see Phase 3), then take the sprint queue from the top of
   `work/sweep/results.json`, grouped by unit. Fetch, inspect `git worktree list`, and run
   `python3 tools/next_target.py --by-unit`. Exclude active local worktrees as
   well as unmerged sprint branches and open PRs. Treat the ranking as a
   shortlist: prefer related function families with reusable code and types,
   and enough ready bytes for several waves. Use few functions left only as a
   tie-breaker when there is a concrete completion advantage. Inspect owned data
   sections before assuming code completion permits promotion. Do not pay the
   full sprint setup/finish cost for a tiny queue merely because
   it is nearly complete. The selector ranks first-wave bytes, not predicted
   speed; inspect code shapes before choosing. Run one
   [sprint](sprint_workflow.md) in its own worktree branch, keeping the attempt
   budget so difficult targets do not consume the easy gains. Preserve the queue across
   context checkpoints; finish accounting must include every selected target.
3. Record a plateau in the task's `status.md` and the PR with its best score, its
   remaining mismatch and one concrete next hypothesis. Then move on.
4. When the plan changes, update this file in the same PR. Sprint results stay in
   the PR.
