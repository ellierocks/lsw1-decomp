# `aisys-1` matching record, 2026-11-13

Baseline `591ab1a`; bench run
`t3-d590fc6c-5d871e0c18916104-1794598243` (closed by `sprint_finish.py`).
All 32 queued targets were attempted: **25 new strict VERIFIED functions,
16,432 bytes; zero promoted; zero parked; seven unfinished handoffs**.
At kickoff none of these targets was strictly verified. No symbols were renamed.

## Strict matches

The baseline column is the first scored draft (exact instructions), not a
claim of pre-existing C coverage. Best for every row is the complete byte and
relocation match, including strict symbol references and retail data.
Bodies and consumed partial views are in `src/auto/ai/aisys/aisys.c`.

| Function | GC address | Bytes | First draft | Best |
|---|---|---:|---:|---:|
| AIScriptLoadScriptTxt | `0x8007E3CC` | 388 | 90/97 | 97/97 |
| Action_Idle | `0x8007F8A8` | 884 | 202/221 | 221/221 |
| Action_FollowOpponent | `0x8007FC1C` | 416 | 94/104 | 104/104 |
| Action_MoveAwayFromOpponent | `0x8007FDBC` | 480 | 120/120 | 120/120 |
| Action_FollowPlayer | `0x8007FF9C` | 432 | 100/108 | 108/108 |
| Action_CirclePlayer | `0x8008014C` | 640 | 135/160 | 160/160 |
| Action_CircleOpponent | `0x800803CC` | 624 | 143/156 | 156/156 |
| Action_MoveAwayFromPlayer | `0x8008063C` | 452 | 113/113 | 113/113 |
| Action_RetreatFromOpponent | `0x80080800` | 428 | 104/107 | 107/107 |
| Action_RetreatFromNearestOpponent | `0x800809AC` | 420 | 94/105 | 105/105 |
| Action_FaceOpponent | `0x80080B50` | 1,304 | 307/326 | 326/326 |
| Action_SetViewDistance | `0x80081068` | 420 | ERROR | 105/105 |
| Action_SetMinViewHeight | `0x8008120C` | 420 | ERROR | 105/105 |
| Action_SetMaxViewHeight | `0x800813B0` | 420 | ERROR | 105/105 |
| Action_SetHearDistance | `0x80081554` | 420 | ERROR | 105/105 |
| Action_GoToNode | `0x800816F8` | 684 | 171/171 | 171/171 |
| Action_GoToNodeRandom | `0x800819A4` | 472 | 92/118 | 118/118 |
| Action_GoToOrigin | `0x80081B7C` | 644 | 161/161 | 161/161 |
| Action_SetLocator | `0x80081E00` | 732 | 183/183 | 183/183 |
| Action_GoToLocator | `0x800820DC` | 2,192 | 477/548 | 548/548 |
| Action_MoveAwayFromNode | `0x8008296C` | 684 | 163/171 | 171/171 |
| Action_OverrideAnimation | `0x80082C18` | 512 | 127/128 | 128/128 |
| Action_BlockPath | `0x80082E18` | 888 | 208/222 | 222/222 |
| Action_PathConnectionMaxLength | `0x80083190` | 588 | 96/147 | 147/147 |
| Action_SetInterrupt | `0x800833DC` | 888 | 183/222 | 222/222 |

## Unfinished handoffs

None meets the 25-distinct-candidate floor; these are handoffs with structural
hypotheses, not plateaus or short parks. Counts include search codegen hashes.
Local status notes retain the draft paths and first differing blocks.

| Function | GC address | Best exact | Distinct candidates | Local draft |
|---|---|---:|---:|---|
| AIScriptResolveReferences | `0x8007E550` | 246/267 | 3 | `work/AIScriptResolveReferences/list.c` |
| fn_8007E97C | `0x8007E97C` | 118/149 | 3 | `work/fn_8007E97C/list.c` |
| fn_8007EBD0 | `0x8007EBD0` | 51/58 | 5 | `work/fn_8007EBD0/helper.c` |
| AIScriptProcessorInit | `0x8007ECB8` | 112/186 | 2 | `work/AIScriptProcessorInit/structured.c` |
| fn_8007EFA0 | `0x8007EFA0` | 90/100 | 4 | `work/fn_8007EFA0/body.c` |
| fn_8007F130 | `0x8007F130` | 119/137 | 8 | `work/variant_search/fn_8007F130/best.c` |
| AIScriptProcess | `0x8007F354` | 297/341 | 3 | `work/AIScriptProcess/loop.c` |

### `AIScriptResolveReferences`

GC struct/reference/condition loops improved 165 to 246 exact instructions.
The first differing block preserves the script in r19 rather than target r21;
list addresses form in a saved register then copy to r3, while the target forms
r3 first. Flag bit 30 is extracted earlier into r9 rather than later into r0.
Passing the existing states list to aiFindStateList removed a duplicate live
list and an extra instruction; assignment inside the call gave identical code.
Next: recover a typed list view or original inlining/lifetime boundary, then
check flag-argument evaluation order. Donor AIScriptFindRefScripts corroborates
saved-next/removal/callback flow; modern offsets and flags do not transfer.
JP 0x4E1A60 is an inferred callee pair with the same flow, different layout.

### `fn_8007E97C`

The nested script/state/condition callbacks and Base lookup/resolve are
recovered. The first differing list-address block uses addi into a saved
register followed by mr r3; the target uses addi r0 and two copies. The draft
has four fewer instructions and interchanges r29/r30. A direct list helper
removed a duplicate live range; assignment-in-call compiled identically.
Seven search variants had no errors or improvement. Next: recover the list
helper ABI/inlining boundary, possibly a macro wrapper, without register padding.
No exact-name donor hit; AIScriptInitConditions corroborates traversal but its
param_cond extension and third callback argument are rejected. JP 0x4E1C20
supports callbacks/Base/resolve with different offsets.

### `fn_8007EBD0`

Inline aiFindStateRaw plus shared aiSetState is the best draft. The first
lookup block preserves script+0x10 rather than the target's script base r29,
which recomputes +0x10 in the loop; the draft adds one mr. Struct-member and
assignment variants compile identically. Calling external fn_8008386C does
not inline (13/58). Sixteen search variants had no errors and only four
original codegen hashes. Next: recover the lookup's original inline boundary
or a surviving use of the script base. No dummy register padding. No exact-name
donor hit; AIScriptSetStateByName supports list/name and state-set stores,
without modern reset/logging fields. No PC pair in the task pack.

### `AIScriptProcessorInit`

The nine-argument ABI, typed processor, script/default search and state
initialization are recovered. The GC mask is a sign-extended 64-bit expression
using __ashldi3, not the donor's native 32-bit test. The first differing entry
block saves scriptName in r27 instead of target r31 and processor in r28
instead of r26; helper-return blocks also differ in placement. The draft's
pointer-walk copy differs from the target's indexed four-float copy.
Next: explicit copy-loop bounds and delayed defaults-pointer lifetime, then
script/state search helper boundaries. Reject newer donor processor resets.
No PC pair in the task pack.

### `fn_8007EFA0`

The condition callback+4, value+8, control flags+0xc, signed compare/index
bytes +0xc/+0xd, and params/data/descriptor +0x10/+0x14/+0x18 are recovered.
The first meaningful successful-condition branch in the target moves the
entry into r3 before andi and returns directly; the draft moves it after
the branch. The bottom loop also differs in mr/cmpwi placement. Result-local,
nullable-result and break forms scored 89, 86 and 88, below best 90.
Next: result lifetime across OR/AND exits, bottom-loop shape, or an original
inline list-next wrapper. No exact-name donor hit; AIScriptProcessStateConditions
supports callback intent, not its extra comparisons/complex parameters/finish
callback. No PC pair in the task pack.

### `fn_8007F130`

The best full-source snapshot uses a do-while variant of typed.c. The two
reference loops use script+0x40/state+0x1c, stack+0x28 and depth+0x38.
The first differing found-reference block needs distinct found/reference
lifetimes and the target's duplicated pre-call argument setup while sharing
the depth increment and call tail. stack.c scored 114; shared-tail.c scored
101 because a goto initialization overmerged the first found block.
Next: retain two stack-base temporaries and separate found-reference lifetimes
without losing the shared tail. One experimental rewrite omitted an initialize
label; it was corrected immediately. No exact-name donor hit; reviewed reference
processing callees, rejecting modern complex stacks/offsets. No PC pair supplied.

### `AIScriptProcess`

The structured rewrite improved 61 to 289 to 297 exact instructions. It
corrects pointer-array stepping to 4 bytes, retains nextState/action values
and fixes the callback prototype. Initial stack processing matches; the first
differing action-load block loads via r0 then copies to r31 instead of the
target's direct r31 load. The target guards the initial state once then uses
an inline advance operation returning a boolean; the draft checks action and
flags differently and hoists zero constants into saved registers.
Next: inline advanceAction returning bool, preserve its extra null check,
reuse one timer zero while retaining the previous-timer predicate, and recover
packet-reset widths/store order. Do not add a descriptor callback argument:
r9 only retains scratch at the indirect call. Donor modern resets, parameter
stacks and logging are rejected. JP 0x4E2230 supports stack stride/calls/state
progression but uses flag bit 0 rather than GC bit 31 and out-of-line setters.

Body-only drafts use the retained full source
`work/aisys-with-experimental-helpers.c`; unused experimental helpers are
excluded from the shipped source. `fn_8007F130/best.c` is a full-source snapshot.
Raw drafts, task packs, assembly, donor text and experiment logs remain local.

## Donor and PC evidence limits

All 32 target task-pack `donor_evidence.md` files and corresponding donor leads
were reviewed at OpenSaga pin `7bb2402aa902d08f0979892ca23ee80e809e5ee1`.
`hillclimb.py try --donor-note` records were made once per target/revision
(retrospectively after assembly-first scoring). No donor source was copied.
The [owning AI evidence](opensaga_ai_script_evidence.md#aisys-1-target-derived-layout-and-abi-evidence)
records consumed GC layouts and rejected donor transfers.

The Japanese PC 1.0 pairs supplied by task packs were inspected:

- Loader `0x4E1930` supports name stride 32/file iteration, but lacks GC's
  final repeated resolution pass.
- Resolve `0x4E1A60`, initialization `0x4E1C20`, and processing `0x4E2230`
  corroborate flow only; see their handoffs for layout and flag differences.
- GoToLocator `0x4E4970` supports parser/move/rotate/add calls, but has a
  zero-radius special case and object byte `0x27d`; GC uses squared
  `(radius + global)` and byte `0x285`.
- OverrideAnimation `0x4E4F20` begins in the preceding MoveAwayFromNode
  extent; reject it as a whole-body boundary oracle. Only the later
  from/All/FindAnimIX sequence corroborates parsing.
- **Reject PathConnectionMaxLength `0x4E52E0` as the proposed pair**: its
  from/to/bothways/FALSE parser has no length or float operations and resembles
  BlockPath. The pairing TSV is unchanged pending stronger evidence.
- SetInterrupt `0x4E5680` supports priority/id/state/time ordering, with a
  PC out-of-line numeric reader versus the GC inline reader.
- Other targets have no PC pair listed in their task packs.

Ellie questions: none. Unknown gameplay meanings remain unnamed. No runtime
session was driven, and no runtime ambiguity was used to justify a match.

## Verification and measurements

Acceptance commands:

```sh
python3 tools/sprint_finish.py --unit src/auto/ai/aisys/aisys.c --matched-from-diff --deferred AIScriptResolveReferences fn_8007E97C fn_8007EBD0 AIScriptProcessorInit fn_8007EFA0 fn_8007F130 AIScriptProcess
bash build.sh
python3 tools/matching_progress.py
```

Finish strictly verified all 25 against the built objects. For any table entry:
`python3 tools/verify_fn.py NAME --build --strict-symbols --quiet` reproduces
acceptance. All 92 previously defined functions in this unit also passed that
command after the shared partial-view extension. The whole DOL remains
byte-identical, SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`.
Full-unit sections, rodata and linked ranges were not promoted.

The report changes from 2,856 to 2,859 matched functions and 483,584 to
487,052 matched code bytes (+3,468). Strict-only coverage changes from 191 to
213 functions and 66,816 to 79,780 bytes (+12,964). These separate measures
sum to the new strict total; 16,432 is not a promotion claim.

Checks: the 705-test unit suite passed (two skipped), including 91 sweep tests;
all 64 retail-required verifier tests, compileall, configuration, symbol-map
generation, shell syntax, documentation links and diff whitespace validation
passed. Final incremental build:
1.04 s (finish build 1.4 s); median scoring latency 0.31 s.

The closed bench records 399 scoring attempts and 26 compile errors (6.5%),
including intentionally broken baseline repair replays and one invalid replay
against changed declarations. Initial sweep errors were four pointer reciprocal
multiplications plus a malformed interpreter callback. Other encountered errors
included a missing default-token declaration, a C89 declaration after a statement,
and an omitted experimental goto label; each was corrected. FaceOpponent search
had three failing single rewrites, with their combinations pruned; its exact
diagnostics were not retained. EBD0, F130 and E97C searches had no errors.
No search crossed the 20% error warning threshold. No permuter runs or PLATEAU
notices occurred; there was no post-notice grinding.

The pointer-byte-difference sweep repair reduced identical-draft compile errors
4/4 to 0/4 and full scoped sweep errors 5/32 to 1/32; it produced zero automatic
matches. See [workflow gap 11](../workflow_gaps.md#11-search-attempts-mostly-fail-to-compile)
and the fixed-source cohorts in `config/workflow_metrics.toml`. The remaining
malformed callback draft is unfixed tool friction. Unsupported score flags were
replaced with the documented positional-source interface; donor ledger records
initially missing input were rerun with `--candidate`. Neither remains blocked.
