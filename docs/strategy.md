# Matching strategy

The plan from here to 100% code, then data. [`config/plan.toml`](../config/plan.toml)
holds the stages and items in order; `python3 tools/plan.py` prints the current stage,
each item's bytes left (from `build/GL5E4F/report.json`), claims and the next item.
[C matching](code_matching_workflow.md) decides how a candidate is proved. The previous
phase plan (SDK import, whole-binary splits, draft-and-climb) is in
[strategy history](history/strategy_2026-11-10.md).

## Rule

Work the plan's next unclaimed item: `python3 tools/kickoff.py` names it and
`kickoff.py --start --item ID` claims it. Go off plan only when Ellie asks, when a tooling
fix unlocks more plan bytes than the item itself, or when Dolphin runtime evidence is
asked for or needed to resolve an ambiguity. State an off-plan choice and its reason in
the PR; tools record off-plan attempts as such.

## Where it starts (revision 1, 2026-11-11, main 7db800a)

Code 354,044 / 1,591,228 bytes (22.25%), 2,375 / 4,192 functions. Data 127,502 /
2,617,734 bytes (4.9%). The SDK donor queue is exhausted; what is left is game and Nu2
code compiled with ProDG GCC 2.95, plus HVQM4, the debugger driver and libsn.

## What the order rests on

- **Near misses are residue, not a stage.** The 56 functions at 90% or better (14,668
  bytes) share no fixable cause: 55 have equal frames and none differ in relocations.
  They differ by one to three instructions of scheduling or register order, and about
  a dozen are recorded plateaus of earlier sprints and permuter runs
  ([batch history](phase3_batches.md#sprints-of-2026-11-0708-handoff)). They are worked
  inside their item, with permuter bursts.
- **Mechanical repairs have stopped converting.** #117 improved 130 drafts by 5,018
  exact instructions and matched nothing. The two largest blocker classes are source
  shape, which no regex repair can recover:
  `reloc:symbol+offset` (185 functions, 147 KB) is an untyped global table (in
  `fn_80075C78`, retail indexes 32 entries of 0x6C bytes at `lbl_803D8280`; the draft
  folds `+0x40` into the symbol), and `frame:smaller-same-gprs` (210 functions, 231 KB)
  is a missing local or inlined helper (in `fn_80066498`, retail's frame is 16 bytes
  bigger with nothing stored there). A type or helper written once fixes every user.
- **Callee prototypes block callers.** Recorded plateaus wait on callee signatures
  (`fn_8014BEB0`, `fn_8009EC80`), and the sweep's prototype repairs work only when a
  prototype exists. 33 unmatched functions have ten or more unmatched callers each
  (`fn_8006DCD4` 84, `NuFsqrt` 72, `fn_8001384C` 39). So the plan runs bottom-up
  through the call graph: engine, then shared game layers, then the game units.
- **Evidence is uneven.** JP PC 1.0 pairs cover 525 targets (458 KB), mostly in
  `levels`, `obstacle` and `config`; OpenSaga name leads cover 196 (168 KB), mostly AI
  and Nu2 ([PC builds](research.md#pc-builds), [OpenSaga](symbol_donors/opensaga_matching.md)).
- **Yield is measured, not assumed.** The one bench is 29 functions in 0.6 active hours
  on small `levels` functions; unit sprints have closed about 24 functions each. The
  average function left is about 680 bytes against about 150 in those sprints, so expect
  fewer functions per hour. Every item run is benched (`kickoff.py --start --item`), and
  `plan.py` reports bytes per active hour per stage so the order can be corrected.

## Stages

Bytes and functions are as of revision 1; `plan.py` prints the live figures. A stage is
done when each function of each item is matched in the report or parked with a reason in
`plan.toml`.

| Stage | Targets | Code bytes | Functions | Depends on |
|---|---|---:|---:|---|
| 1 Engine core | prototype hubs, then `nucore`, `numath`, `rtl`, `nusound3`, `nungc`, `nufmv` | 77,624 + hubs | 171 | source-unit scaffolding for `rnc`, `nuheap`, `nugraph_2`, `NGCStreaming2`, `HVQM4PlayerEx` ([gap 8](workflow_gaps.md#8-functions-without-a-source-unit-are-not-swept)) |
| 2 Nu3d engine | `auto/nu3d/*`, `nu3dx/nuanim` | 152,100 | 272 | stage 1 prototypes; vector, matrix and material types; `nuportal_gen` scaffold |
| 3 Objects, AI, camera | `apiobject`, `aisys`, `aiscript`, `gameai`, `gameaiscript`, `socksys` | 189,208 | 264 | `include/game` object and AI types ([game structs](game_structs.md)); `aiscript` scaffold |
| 4 Gamelib and GUI | `audio`, `apimenu`, `apisave`, `saveload`, `gcutscn`, `terrain`, `debris` | 202,700 | 273 | stages 1–3 types |
| 5a Smaller game units | `main`, `pickups`, `turret`, `rtlif`, `credits`, `cut`, `chris`, `creature`, `podrace` | 149,292 | 225 | stages 1–4 |
| 5b Levels | `auto/game/levels`, eight address slices | 163,176 | 252 | stages 1–4 |
| 5c Obstacle | `auto/game/obstacle`, eight address slices | 164,328 | 176 | stages 1–4; `ForceCode` and `ApplyGravity` are hubs |
| 5d Config and force | both units have no source file yet | 91,484 | 64 | scaffolding; JP PC pairs cover 63 KB |
| 6 No known route | HVQM4, SDK leftovers, debugger driver, libsn | 47,272 | 120 | see below |
| 7 Data | everything not promoted with its code | 2,490,232 | – | unit promotion; `.rodata` ownership |

Large units are sliced into items of about 20 KB so one child can finish one. Items
overlap only where a hub function also belongs to its unit's item.

## How an item is worked

One item per thread, one PR, its own worktree, branch `t3/plan-<item>`.

1. `kickoff.py --start --item ID` claims the item, records the baseline and opens the
   bench run. `next_target.py`, `hillclimb.py`, `sweep_permute.py` and the near-miss lane
   default to the item's functions.
2. Type before climbing: give the item's global tables and shared structs real types
   (confirmed against target accesses), write missing inline helpers, and confirm the
   prototypes of its callees. Then rerun the sweep on the item's units.
3. Climb what remains with `score_fn.py`, JP PC bodies, OpenSaga evidence, Ghidra and
   permuter bursts of 180 s. Park a function that plateaus with its best score, the
   remaining mismatch and the next hypothesis.
4. Promote a unit whose code is complete when its data permits; otherwise record what
   data it still needs, for stage 7.
5. `sprint_finish.py` closes the bench. The PR states matched bytes and functions from
   `matching_progress.py`.

## No known route yet

- **HVQM4 decoder** (`dolphin/hvqm4/hvqm4dec`, 18,592 bytes): a route exists. The
  parked candidate strictly verifies 18 of 43 functions
  ([HVQM4 evidence](sdk_import.md#hvqm4-decoder-parked)); resume it.
- **SDK leftovers** (`Pad`, `dvdfs`, `.init` stubs, 3,160 bytes): functions the
  dolsdk2004 donors did not match; per-function version drift against retail.
- **Debugger driver** (`8018A10C..8018AB8C`, 7,236 bytes, blocked): needs a hand-written
  `DebuggerDriver.c`. A donor compiled for the same SDK revision would unlock it.
- **libsn and the unsplit runtime ranges** (18,284 bytes, blocked): no upstream source
  and no split ([SN runtime](sn_runtime.md#not-imported)). Unlocked by an SN runtime
  source or another ProDG game's decompilation of the same objects, then splits.
- **Data** (stage 7): most comes with promoting units. The rest needs data splits per
  section ([build configuration](build_config.md)); there is no data-matching lane yet.

## Revising the plan

`plan.py` says when the plan needs revision: an unmatched report function no item
covers, an item naming a unit or function the report lacks, a stage left with only
claimed, parked or blocked work, or a finished item run that matched nothing. Revise
`plan.toml` (bump `revision`), this file and the PR together. Never reorder to chase a
score; reorder on evidence such as measured yield, a new donor or a type that unlocks
another stage.

## Rules that do not change

- The strict verifier, pinned compilers and the retail DOL are the oracle. Never weaken
  them, change pins, or move a split to improve a score.
- VERIFIED is not PROMOTED. Promotion needs unit sections, relocations and linked retail
  ranges with no regression. Renames are not matching progress.
- Donor code and generated C are evidence; a function counts only after it matches
  strictly with this game's compiler.
- Raw experiment logs and proprietary artifacts stay under the ignored `work/`.
