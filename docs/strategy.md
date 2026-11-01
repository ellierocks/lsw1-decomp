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
| `libsn` / libc | about 1% | Needs SN's GPL GCC source build |
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
- `libsn`/libc, which needs SN's GCC source build (see RE4).

SISamplingRate promotes its two functions and initialized data; its four-byte
`SamplingRate` BSS remains assembly fallback because the complete-unit gate does
not verify BSS.

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
   no signal. Anonymous TUs and data sections remain; see
   [proposed TU boundaries](build_config.md#proposed-game-tu-boundaries).
2. Add a provisional unit per TU with assembly fallback. A split change must
   not change the relinked DOL. Mark boundaries provisional and revise them on
   evidence, never to raise a score. **Done:** 78 `auto/` `.text` units are
   committed. Phase 3 does not wait for the remaining boundary and data work.
   Revise a boundary when it blocks promoting a TU.

### Phase 3: agent draft-and-climb for game code

Throughput is the measure. Run Phase 3 in parallel batches: one worktree thread
per TU or leaf-function cluster, a capped number of attempts per function, and
a plateau note for each function left unmatched. Record matched functions per
agent-hour after each batch and choose the next lanes by that rate. Build the
shared `nu2` headers early, starting with the structs the most-called functions
touch, because every later draft reuses them.

For each queued function:

1. **Draft.** Combine m2c (PPC) output, the Ghidra output, CrashWOC/Nu2 donor
   C and context from shared headers.
2. **Agent search.** Iterate with `score_fn.py`, recording candidates worth
   keeping through `hillclimb.py try`.
3. **Permuter.** Run decomp-permuter on functions that are close (about 95%
   exact or better) with `tools/permute_setup.py NAME --run`.
4. **Accept.** Use `verify_fn.py --build --strict-symbols`. Promote whole TUs
   when every function matches, and batch several matched functions per PR.

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

#### Leaf batch 1

The first leaf batch (`t3/phase3-leaf-batch-1`) attempted 51 small functions across
11 `auto/` units plus `nuerror_ngc`, and strictly matched 50 (`verify_fn.py --build
--strict-symbols`). `fn_8000A64C` is parked at 49/50. The objdiff report counts 46 new
matches (+1,616 code bytes, 773 to 819 functions): four float-literal functions
(`fn_8000ED40`, `NuFdiv`, `fn_8000B234`, `fn_8000B2C4`) are strict matches whose literal
reloc reads `.rodata` rather than the `lbl_` symbol, so objdiff shows 98 to 99%. Only
`nuerror_ngc` was promoted; `nudlist_1` is complete but its TU is unresolved (see below).

Failure modes, in order of cost:

- **GCC store and load order.** The scheduler does not follow source order. A shared
  `s32` local for repeated constants (`-1`, `0`) and an exhaustive search over store
  permutations (at most 7! compiles at 0.15 s) solved `fn_800127D8`, `fn_8002BE28` and
  siblings. `tools/order_search.py` automates that search; run it before hand-tuning.
- **Branch shape.** `return a != b;` and `if (a != b) return 1; return 0;` compile
  differently (`bnelr` early return only in the second). Try both forms and the inverted
  sense before suspecting types.
- **Missing types.** Off-SDA data needs `extern T x[]` (array of unknown size) to get
  `lis`/`lfs` instead of an SDA21 access.
- **Handler thunks.** `fn_80009948` returns a function pointer that is called with the
  message; model it as `((void(*)(char*, ...))fn(file, line))(msg)`.
- **Out of scope for C.** Paired-single (`psq_l`/`ps_*`) functions are hand-written asm.

decomp-permuter is wired end to end. `bootstrap.sh` fetches the pinned checkout,
`tools/permute_setup.py NAME` writes the work directory and `--run` starts it; scoring
goes through the strict verifier, so score 0 means `score_fn.py` MATCH
([workflow](code_matching_workflow.md#decomp-permuter)). Its first run matched the
parked `fn_8000A64C` in about 2,000 iterations: wrapping the body after
`OSWaitSemaphore` in `do { ... } while (0)` makes GCC load `t->value` before the
`mode == 1` compare. The whole `nuthread` unit then matched and was promoted to
`nucore/ngc/nuthread.c` (0x8000A64C..0x8000A834, 8 functions).

`nudlist_1` and `nudlist_2` stay separate: `tu_boundaries.py` has no evidence that
they form one TU. The seven `nu3d/nudlist.c` path references start at `fn_80010120`.
`nudlist_1` (`NuMtxDet3`, `fn_8000E830`, `fn_8000E8A4`) has no rodata or path
reference, and the DP's `nudlist.c` start range spans 0x8000E7E4..0x8000EBE8. The
hand-split `numath/nu_asm.c` lies between the halves, so merging them would also
absorb it. The nearest evidence (Mac order: `NuMtxDet3` 1949, `NuVec4MtxTransformVU0`
2305) points at numath matrix code rather than `nudlist.c`.

#### Leaf batch 2

The second leaf batch (`t3/phase3-leaf-batch-2`, lane `auto/nucore/*` and `auto/numath/*`)
attempted 64 small functions across 10 `auto/` units and strictly matched 62
(`verify_fn.py --build --strict-symbols`). Parked: `NuFParGetWord` (71 insns, 13
attempts) and `fn_8000DC80` (matrix-identity stores, about 2,700 order evaluations;
`order_search.py` cannot cover 16 free stores). No unit was promoted: every touched
unit still has unmatched functions. Targets were mostly 4 to 100 byte leaves, so
bytes per function are low.

New failure modes seen:

- **Signedness of switch values.** `fn_80009408`/`fn_800094C4` need `u32 type` (the
  retail tree emits `cmplw`) and an explicit `case 0x12345679:` sharing the default.
- **Float bit tricks.** A pointer cast `*(f32*)&bits` allocates `f0`/`f1` the wrong
  way round; a `union { s32 i; f32 f; }` reproduces retail (`NuRandIntSeeded`).
- **Rodata constants.** A guessed float literal passes the instruction compare but
  fails the data compare; read the real value from `build/GL5E4F/main.elf` (`NuASin`).
- **Struct-copy size.** A global-to-pointer struct assignment is a 3-stage unrolled
  loop; size the struct from the tail copy (`fn_8000DBA0`, 0x40 bytes).

#### Shared-header batch

The first shared-header batch (`t3/phase3-nu2-headers`) confirmed `nugscn_s`,
`nuinstance_s`, `nuspecial_s`, `nugobj_s`, and the new `nudisplayscene_s`,
`nudisplayspecial_s` and `nuhspecial_s` against LSW1 accesses
([evidence](nu2_engine_reference.md#lsw1-confirmed-nu3dx-layouts)). Using those headers,
22 of the 24 functions in `auto/nu3d/nuspecial` strictly match. Two are parked, so the
unit stays NonMatching and is not promoted. The rate was about 9 to 10 matched functions
per agent-hour, including header confirmation.

- `NuSpecialFind` is parked at 100/101. Retail emits the display-path clear tail as
  stores at 8, 0, 4. The source order that produces that tail is cross-jumped into the
  identical legacy-path tail. Next: stop the cross-jump, or run the permuter.
- `fn_80012C44` is parked at 45/47. Retail uses `add` then `stbu r0,8(r9)`, and the
  draft uses `addi` then `stb`. Next: step a pre-incremented attribute pointer by 16.

Failure modes new to this batch:

- **Block order.** GCC 2.95 places basic blocks in source order. Put the shared
  "found" blocks ahead of their loops as `goto` targets to get the retail layout.
- **Cross-jumping.** GCC merges identical tails, so two branches cannot keep
  separate copies of the same store sequence.
- **Register allocation.** Separate count and index locals per branch, declared
  in retail's order, matched `NuSpecialFindMulti` after the shared-local versions
  failed.
- **Misnamed symbols.** Read the body before trusting a donor name.
  `NuSpecialFindMultiWC` and `NuSpecialGetInstAnim` hold other functions.

The next structs to confirm, by caller count, are the animation and instance-animation
structs, `numtl_s`, `nugeom_s`, game character and object structs, `nufile`, and the
rest of `nugscn_s`.

#### Leaf batch 3

The third leaf batch (`t3/phase3-leaf-batch-3`) covered `auto/nu3d`, `auto/nungc`,
`auto/nusound3` and `nufmvmtl`. It attempted 155 smallest-first functions with at most
one callee and strictly matched 154 (`verify_fn.py --build --strict-symbols`), 841 to
995 functions and +3,328 code bytes (173,200 to 176,528). Six more matched in
`nuspecial` but landed first via the headers batch, so they are not counted. No unit
was promoted: every touched unit still holds unmatched functions.

Composition: 38 empty `blr` stubs, about 50 constant-return, getter and setter
accessors, 32 zero-callee bodies of 20 to 64 bytes, and 39 one-callee wrappers. Active
agent time was about 20 minutes, roughly 450 functions per hour. The wall-clock span
in `work/batch3_timing.txt` is longer because the session sat paused. This rate is for
trivial leaves; do not extrapolate it to bodies with branches, structs or floats.

- `fn_80043040` is parked at 3/4. Retail is `li r0,1; stw r0,A444; stw r4,A448`; the
  draft stores `r4` first. Tried source swaps, a shared local, a `u32` argument,
  `volatile` on either global and chained assignment. Next: model both stores as
  fields of one struct or find a caller that shows an inlined macro.

Failure modes new to this batch, in order of cost:

- **Store order.** An exhaustive permutation search (a stand-in for
  `tools/order_search.py`) fixed every wrong-order case but `fn_80043040`.
- **Conditional call.** `bne` to the epilogue means the `bl` sits inside the `if`
  (`fn_80042F58`), not after it.
- **Array sizing.** `li rX, sym@sda21` needs an `extern T x[2]`; an unsized array
  gives `lis/addi`.
- **Local reuse.** One local for both halves of an if-chain gave `mr` plus `bnelr`;
  two locals matched.
- **Prototype state.** `crclr cr1eq` before `bl memset` means the callee is declared
  without a prototype (`extern void* memset();`).
- **Donors.** OpenSaga pin 7bb2402 has no body for any of these leaves; layouts came
  from GC assembly.

#### Leaf batch 4

Batch 4 (`t3/phase3-leaf-batch-4`) added 20 provisional `auto/` units and strictly matched
275 leaf functions (`verify_fn.py --build --strict-symbols`), moving the objdiff report
from 819 to 1,094 functions (+5,536 code bytes, 170,712 to 176,248). No unit was promoted
and the relinked DOL is unchanged. Four `auto/game/podrace` functions are parked
(`fn_801507CC`, `fn_80150D80`, `fn_80150DBC`, `fn_80150418`): several variants do not match,
and the next hypothesis is that the TU's compile flags or inlining differ from the rest.

Measured rate: the wall clock from first to last acceptance was 5 h 20 min, but the active
time (gaps over 15 minutes removed) was about 15 minutes, so the rate is 275 functions in
roughly 15 active minutes. That figure is not comparable to batch 1: nearly all targets were
callee-less accessors and setters that a template generator drafted from the disassembly,
with `score_fn.py` as the filter. Plan Phase 3 on the functions that remain, which have calls,
loops and branches, and expect a much lower rate.

Failure modes, in order of cost:

- **Families with a shared shape.** One template matches many siblings (the 76 guarded
  setters in `auto/game/cut`). Do the template once, then batch-score; do not hand-draft.
- **Store order and compare sense.** `fn_8011B378` matched only with stores ordered
  32, 40, 44. `cror so,eq,lt` + `bnslr` is `if (x <= limit) store`, not `x > limit`.
- **Pointer-as-integer globals.** cut.c declares `lbl_8040B250` as `s32` and indexes it
  with integer arithmetic; a struct pointer conflicts with sibling externs in the same unit.
  A local `volatile s32*` stops GCC merging `|=` and `^=` into one `ori`/`xori`.
- **Possible assembly in provisional TUs.** The podrace parks prompted a flag
  investigation; the private register/CR convention instead points to absorbed
  debugger assembly (see below).

#### Per-TU compile flags

The [podrace investigation](build_config.md#podrace-investigation-phase-3) tested
70 pinned ProDG flag trials without matching the four parked probes. No per-TU
flag override is confirmed. The tail after `0x80150398` has private r31 state,
r30 LR saves, CR0 results across calls and SN debugger strings; investigate its
assembly/runtime ownership next. The sweep found no other `auto/` unit with
that signature. Existing podrace matches remain unchanged; no bytes were promoted.
Use the [flag investigation gate](code_matching_workflow.md#per-tu-flag-investigations)
before installing an override, and keep the strict oracle and compiler pin.

#### Game struct headers

The first game-side batch confirms sparse `NUCHARACTERDATA_s`, `NUGAMEOBJECT_s`,
`NUAPIOBJECT_s`, `NUAIPACKET_s` and `NUAISCRIPTPROCESS_s` in `include/game/`.
[Layout evidence, access ranking and donor differences](game_structs.md) record
all confirmed fields. Twelve consumers strictly match: eleven new functions and
the existing `Condition_Timer` converted to the shared type. Zero parked, zero
promoted bytes; both units retain unfinished functions and remain NonMatching.
The DOL remains byte-identical to retail.

The measured active interval is approximately 4 minutes including confirmation,
search and integration (timestamps in `work/gameheaders_timing.txt`), about 165
new functions per active hour. This small sample includes simple AI accessors;
it is not a forecast for larger game functions. Next: the API-object position
and flag prefix and its relationship to creature objects, character-data extent,
then pickup and turret structs. Engine headers remain in the parallel lane.

## Rules that do not change

- The strict verifier, pinned compilers and the retail DOL are the oracle.
  Never weaken them, and never move a split to improve a score.
- VERIFIED is not PROMOTED. Promotion needs unit sections, relocations and
  linked retail ranges with no regression.
- Donor code is evidence. A donor function counts only after it matches
  strictly with this game's compiler.
- Raw experiment logs, probe TSVs and proprietary artifacts stay under the
  ignored `work/` directory.

## Choosing a task

1. Take the lowest unfinished phase that still has open work. Phase 1 can run
   in parallel with Phase 3 on functions inside units that already exist.
2. Claim one library, TU or function at a time in its own worktree branch so
   parallel agents do not collide.
3. Record a plateau in the task's `status.md` with its best score, its
   remaining mismatch and one concrete next hypothesis. Then move on.
4. When a phase step lands, update this file in the same PR.
