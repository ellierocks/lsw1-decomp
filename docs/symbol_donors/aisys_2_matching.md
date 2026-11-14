# `aisys-2` matching record, 2026-11-13

Baseline `7de48da`; branch `t3/plan-aisys-2`; bench run
`t3-b621a640-0c411265a75bb957-1794637829`.
One unit: `src/auto/ai/aisys/aisys.c`. The 21-function, 19,960-byte queue yields
**12 new strict VERIFIED functions, 4,936 bytes; none PROMOTED**. Five functions
are parked at the candidate floor and four larger functions remain untouched.
None of the twelve passed strict verification at the recorded baseline. No symbols
were renamed and no compiler pins, split boundaries or oracle rules changed.

This stage-3 look-ahead was requested out of order: nuportal is held by PR #172,
nurndr-nugscn is running in another session, and the recent nuanim bench produced
zero new strict matches. The preceding aisys run yielded 25 matches at about
28.8 KB/h. Those three other plan items were not worked. The plan edit only adds
a note to aisys-2; it does not change the revision or ordering.

## Verified functions

Every row passed `sprint_finish.py` against the built objects, including bytes,
relocations, strict symbol references and retail referenced data.

| Function | GC address | Bytes |
|---|---|---:|
| Condition_YawToOpponent | `0x80084650` | 176 |
| Action_SetState | `0x80084C88` | 156 |
| Action_SetMoveRadius | `0x800854D4` | 340 |
| Action_SetIgnoreAntinodes | `0x800858D8` | 228 |
| Action_FacePlayer | `0x80084D40` | 360 |
| fn_80087F98 | `0x80087F98` | 572 |
| fn_800868B4 | `0x800868B4` | 336 |
| Action_ResetToOrigin | `0x800857FC` | 80 |
| Action_FollowPath | `0x80085628` | 208 |
| Condition_EitherPlayerInTriggerArea | `0x800842A4` | 216 |
| fn_800881D4 | `0x800881D4` | 1,016 |
| fn_800885CC | `0x800885CC` | 1,248 |

The accepted changes recover vectors, pointer tables, bitfields and inline parser/list
boundaries from GC assembly. FollowPath's final scheduling fix was the early return
for absent path pointers; it did not require a special speed local. For the ellipse
handler, angle reuse, x loads before neighboring y stores and a void NuVecNorm result
resolved the remaining differences. For the rectangle handler, five explicit cases
and trig results evaluated before reloading AI pointers recovered the target flow.
The [owning evidence](opensaga_ai_script_evidence.md#aisys-2-target-derived-layout-and-abi-evidence)
records the consumed layouts and ABI facts.

## Remaining functions

These five satisfy the 25-distinct-compiled-candidate floor. Counts include search
hashes; repeated identical code is counted once. Scores below refer to retained
assembly-informed drafts, not accepted source.

| Function | Best retained exact | Distinct candidates | Next hypothesis |
|---|---:|---:|---|
| fn_80083754 | 64/70 | 25 | Typed-list inline boundary and name/list lifetimes; six register/order differences remain. |
| fn_800875F8 | 127/213 | 76 | Node/matrix pointer lifetimes and connection-vector copies; frame size is recovered. |
| fn_80088AAC | 87/139 | 25 | Stack table-base lifetime, ephemeral node cursor and collection-loop shape. |
| fn_80088CD8 | 202/273 | 37 | Pair-loop bounds and timer propagation using typed AI fields. |
| fn_8008911C | 150/201 | 37 | Pair-loop bounds, mask temporaries and initial timer-loop lifetime. |

The latter two now have four-byte pointer-table steps, 12-byte local vectors,
contiguous 64-bit masks and a two-bit AI field. Their drafts remain local and
unapplied. For fn_800875F8, automatic order search's numerical 143/213 candidate
moved a position copy after dependent vector additions; reject that candidate.
An earlier ellipse order candidate similarly used an angle before assignment;
it was rejected before recovering the legal strict match. Search scores do not
supersede target semantics.

Four larger functions had only the initial sweep and task-pack review; no manual
compiled candidate is claimed: fn_80085A50 (435/921 sweep), fn_80086A04 (sweep
compile error), AISysProcessCharacter (194/403), and AISysCharacterTestPathCnx
(344/771). Next steps are inline buffer allocator/layout recovery for the loaders,
processor flow for AISysProcessCharacter, and the GC symmetric byte bitset plus
0x58-byte path-node view for TestPathCnx. These nine functions total 15,024 bytes.

## Donor and PC limits

OpenSaga pin `7bb2402aa902d08f0979892ca23ee80e809e5ee1` was reviewed through
local aisysall.cpp and target task packs. All 17 manually worked targets have a
`hillclimb.py try --donor-note` record, including explicit no-name/no-PC limits.
No donor source was copied.

- Named conditions/actions corroborate parsing, state lookup and movement calls.
  Reject modern SetState message flags, ResetToOrigin hooks/null guards,
  FollowPath completion/min/max-time extensions, and FacePlayer's extended parser.
  MoveRadius's GC path guards and inline parser remain GC-specific.
- Circle/ellipse/rectangle donor geometry supports arithmetic and call-order leads.
  Reject modern post-operation object flags and path-info layouts. Keep the three
  GC address names; the geometry alone does not establish gameplay identity.
- The unnamed list, pointer-table and pair-processing functions have no exact-name
  hits. Their offsets, widths and mask directions come from GC accesses.
- None of the 17 manually worked targets has a PC pair in its task pack.
  For the untouched loader fn_80086A04, JP `0x004D2390` is a shared ai2 filename
  string lead; its 95 calls versus GC's 114 and mismatched aligned callees prevent
  treating the initial call alignment as identity proof.
- AISysProcessCharacter's JP `0x004DA810` begins with the Initialise flow but
  agrees on only 14/29 aligned calls. TestPathCnx's inferred `0x004D8EF0` is
  unconfirmed; the modern donor checksum differs from GC's symmetric byte bitset.
  These remain leads for the untouched functions, not accepted layout evidence.

Ellie question: Do you recognize situations where characters steer around circular,
elliptical or rectangular invisible AI boundaries? Examples would help name the
three unnamed geometry handlers. Their gameplay names remained parked in this
matching pass. The [subsequent naming investigation](aisys_shape_handlers.md)
resolves their antinode identities and records Ellie's answers. No runtime
session was driven and no gameplay assumption was used to justify a match.

## Workflow repair and verification

The pointer-array sweep repair is [gap 27](../workflow_gaps.md#27-pointer-array-cursors-retained-byte-displacements).
A controlled replay gives 97 → 104/201 and 143 → 144/273 exact instructions:
combined **240 → 248**, compile errors **0 → 0**, strict matches **0 → 0**.
The affected sweep was rerun with `--apply`; it found no strict matches. The three
regression tests pass and disabling the repair fails the captured case. This is
draft improvement, with no measured matching-throughput gain. Named cohorts and
comparison live in config/workflow_metrics.toml.

The initial unit sweep found no matches (18 mismatches, three compile errors).
This checkout's next_target.py does not accept `--plan`; the current claim plus
`--limit 40 --max-size 0x10000` enumerated the same 21 targets.

The finish gate built once and strictly verified all twelve new functions. Report:
2,869 → 2,875 matched functions; 493,720 → 494,968 matched code bytes (+1,248).
Strict-only coverage: 225 → 231 functions; 86,048 → 89,736 bytes (+3,688).
Together the two byte deltas equal 4,936. Full-unit sections, relocations and linked
ranges were not promoted. The DOL stays byte-identical with SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`.

The closed bench records 2,406 active seconds (about 40 minutes), including tool
work; this is approximately 7.4 KB/h for this pass, not a controlled comparison
with aisys-1. Raw task packs, assembly, donor text, drafts and experiment logs stay
local. The retail DOL is ignored by Git.

Validation: `bash build.sh`, `matching_progress.py`, all 129 source-defined unit
functions with `verify_fn.py --strict-symbols`, 733 portable tests (two skipped),
64 retail-required verifier tests, compileall, configuration, symbol-map generation,
shell syntax, 62 local documentation links and `git diff --check` all pass.
No complete-unit promotion gate was run.
