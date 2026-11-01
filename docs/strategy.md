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

#### Mid-size batch B

Mid-size batch B (`t3/phase3-mid-batch-b`) took game-code bodies of 0x80 to 0x10C bytes with
branches, loops, calls and floats from `auto/ai`, `auto/game`, `auto/gamelib`,
`auto/characters` and `auto/camera`. It strictly matched 47 functions
(`verify_fn.py --build --strict-symbols`, 7,288 bytes), moving the report from 1,322 to
1,365 functions (+43 in objdiff) and from 186,400 to 193,092 code bytes (12.30%).
After rebasing, unrelated main changes add another function and 256 bytes: the final
integrated report is 1,366 functions and 193,348 bytes. Batch deltas above exclude those.
No unit was promoted: every touched unit still holds unmatched bodies. The relinked DOL is
byte-identical (`sha1 95cca08a19224775d1a8d6cc64601fb7d0080981`).

| Unit | Matched | Notes |
|---|---:|---|
| `auto/ai/aisys/aisys` | 15 | wall-spline, terrain and LOS flag actions, trigger-area conditions |
| `auto/game/gameaiscript` | 14 | parameter-loop actions, angle and force-object conditions |
| `auto/gamelib/gcutscn` | 10 | cutscene fix-up, create and update helpers |
| `auto/characters/apiobject/apiobject` | 5 | name-search loops and pool reset |
| `auto/camera/socksys/socksys` | 3 | stochastic particle counts and random animation time |

Measurement: timestamps remain in `work/midB_timing.txt` and the three worker timing
files. Search spans were 231 s for the parent, 238 s for AI, 198 s for game-AI and
149 s for cutscenes: 816 summed active seconds, about 207 strict functions/hour and
32,153 bytes/hour, including failed searches and tool waits. Parent integration through
00:26:52 adds 41 s (about 197 functions/hour and 30,615 bytes/hour). Documentation and
later continuation/CI time are outside that search measurement. No gaps were trimmed.
**Do not extrapolate this to the binary.** Twenty-two matches share parameter-loop flag
families, targets top out at 0x10C, and no unit finished. This sample does not establish a
sustainable rate for unrelated larger bodies. Initial drafts used GC assembly and donors;
Ghidra references were audited afterward, rather than used for the initial drafts.

Four strict matches are below 100% in objdiff because literal relocations use section
symbols: CalculateCentre, both AngleFromPlayer conditions and NumForceObjects. The
strict verifier checked their retail constant bytes. Thus the report adds 43 functions
and 6,692 bytes, distinct from 47 strict matches and 7,288 bytes.
Permuter: 3 genuine nonzero-base plateau runs (`Condition_PlayerInTriggerArea`,
`Action_SetGravityHeight`, `instNuGCutSceneCalculateCentre`) plus `fn_8008F968`, and all four
reached score 0, so 4 of 4 hits in about 25 to 700 iterations. Additional `--run` calls
started at an already-matching seed; they are confirmations and excluded from the hit rate.
This four-case hit rate is too small to generalize; CalculateCentre was also solved
independently by order_search.

Parked:

- `fn_80095128` (`auto/camera/socksys`, 53 insns) is at 49/53. Retail copies `pool->masks`
  into `r26` through `addic. r0,r29,8; mr r26,r0`, and the draft allocates `addic. r26`
  directly, which also shifts two branch targets. Nine pointer-expression variants did not solve it. Next: inline a LOS helper with
  independent address expressions to separate the address-test and mask-base lifetimes.

Failure modes new to this batch, in order of cost:

- **Donor identity is not behavior.** Same-name OpenSaga bodies often describe different
  code: `instNuGCutSceneSetEndCallback` sets a callback upstream and copies a matrix in GC;
  `Condition_NumForceObjects` is a constant-zero stub upstream; `Action_SetGravityHeight`
  parses keywords GC lacks; flag fields are words at different offsets. Only loop shapes and
  guards transferred. GC assembly stays authoritative.
- **Indexed loop, not `while (n > 0)`.** An indexed `for` lets ProDG lower to the target
  count-down and allocate packet and parameter registers correctly.
- **Local lifetime.** A hoisted flag local reproduces an early `clrlwi` and `r0` reuse; a
  float local loaded before a flags store moves the `lfs` ahead of it; caching a pointer
  removed a redundant `mr` that retail has. The permuter found three of these.
- **Writable extern floats.** Explicit extern float reads add alias reloads; literals
  reproduce constant reuse.
- **Operand order.** `scene[3] + field` versus `field + scene[3]` survives instruction
  selection.
- **Shared tool mutation.** Concurrent lanes hit transient `ETXTBSY` from wibo while
  `download_tool.py` replaced the executable. Errors inflated permuter error counts. Retry
  the identical candidate; never count it as a mismatch. Run lanes with the tool already
  downloaded.
- **Stale ledgers.** `hillclimb.py try` rejects further records on a VERIFIED ledger, so
  cosmetic renames after verification are covered only by a final direct strict check.

#### Game struct headers 2

The second game-header batch extends the API-object collision view, establishes
`NUCHARACTERDATA_s`'s GC array stride of `0x48`, and starts pickup and turret
records ([field evidence and access ranking](game_structs.md)). Four new
consumers strictly match: `fn_8009083C`, `fn_8013F1B0`, `fn_8013F1DC`,
and `fn_8014068C`. The existing `fn_800AA92C` remains strict after the
character-data extension. One consumer, `fn_8013C620`, is parked at 41/51;
the bounded permuter run did not match. No unit is complete or promoted.

The active interval recorded in `work/gameheaders2_timing.txt` is about
350 seconds, or about 41 new strict functions per active hour. This is a small
sample dominated by short functions and does not predict sustained throughput.
Next: match the pickup group initializer by reshaping its loop and float-load
lifetime; prove more character-data and turret-entry fields with consumers,
and establish the relationship between turret entries and the allocation.

#### Mid-size batch C

Batch C attempted 60 game-code targets and strictly verified 46 functions, totaling
7,996 bytes. The initial report was 1,366 functions and 193,348 code bytes; after
rebasing on main, the integrated report was 1,403 functions and 199,716 bytes
(+37 and +6,368 in objdiff). Those report deltas include any intervening main
changes and differ from the strict function total. No unit finished; whole-image
comparison reported zero differing bytes. All 46 functions were rechecked after the
rebase with `verify_fn.py --build --strict-symbols`.

The local `work/midC_timing.txt` records baseline start at 04:56:47 UTC and the
end of matching at 05:21:32 UTC on 2026-11-01. The 1,485-second interval includes
baseline and searches, yielding about 19,384 strict bytes per active hour; final
integration and CI are outside that interval. This is a short, family-heavy sample,
not a projection. Eight non-matching permuter seeds produced three zero-score hits
(37.5%); one was also solved independently. OpenSaga had no direct name hits for
these anonymous targets; GC assembly and retail data supplied the accepted facts.

Useful observations:

- An inline scene-search helper reproduced an otherwise missing pair of return-path
  instructions and fixed register allocation in both obstacle flag handlers.
- A float local declared before title selection moved a shared constant load ahead
  of the branch. In other prompt functions, loading the action local before the
  timer store reproduced retail scheduling.
- An explicit positive pool branch and separate free-list store ordering reproduced
  allocation and release. A chain assignment had the correct values but the wrong
  store order.
- The RTL byte-reversal family plateaued at 56/60 and 54/60 instructions despite
  576 order-search variants and a permuter run: register lifetimes remain the issue.
- Two quoted-name parser bodies reached 36/49; inlining the reader improved the
  result, but loop rotation and pointer allocation still differ. The related
  prompt renderer reached 43/47 after a 720-variant argument-order search without
  changing the remaining float scheduling.

The remaining plateau scores and next hypotheses are retained in the batch PR;
raw hillclimb and task-pack artifacts stay local. Revisit a function only with a
new source-shape hypothesis rather than repeating the same permutation space.

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
