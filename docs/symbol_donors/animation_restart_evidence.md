# Animation symbol recovery: restart evidence

Scope: `nu3dx/anim` range `0x80016A00`–`0x80024000`, GL5E4F. This is symbol
recovery only. No C was added, no split or build wiring changed.

## Read this first

- **Compiler.** LSW1 GC is built with SN Systems ProDG 3.9.3 (GCC), not MWCC.
  Nothing here assumes CrashWOC codegen. CrashWOC and Mac sizes were used only
  as shape and ordering hints for naming, never as codegen evidence.
- **Naming is not matching.** `report.json` was identical before and after
  these renames: 2,032 matched code bytes of 1,569,532 and 23 of 4,147 matched
  functions. The renames change `config/GL5E4F/symbols.txt` only.
- **Prior names were unreliable.** The earlier "Mac anchor gap-fill" and CrashWOC
  size-coincidence renames (commit `030c1e7`) put several names on the wrong
  functions, some swapped. Call targets printed in generated asm use those same
  names, so a callee name is **not** evidence. Every rename below was derived
  from the function's own body plus independent signals.

## Evidence signals used

| Signal | How it was used |
|---|---|
| Argument registers at entry | Count and int/float split compared with the Mac demo mangled signature (`r3`–`r10`, `f1`–`f8`). |
| Wrapper shape | A small function that forwards its args plus extra zeroed stack args to a larger one matches a Mac "wrapper over `…TransLoc`/`…Root`" pair. |
| Size vs Mac demo | Within a few tens of bytes is consistent; used to rank, not to prove. |
| Relative order | Mac demo orders `NuHGobjEval…` then `NuAnimCurve2SetApply…`; GC order differs because ProDG places small functions differently, so order only supports clustering. |
| Struct strides and offsets | `nuanimdatachunk_s` is 0x14, `nuanimdata2_s` fields, `NUJOINTANIM_s`, `nuanimtime_s` accesses in the bodies. |
| Callers / callee sets | Who calls the function and which sibling it reaches. |
| `NuMemAlloc` `li r5,<__LINE__>` | Source-line values order allocation sites. |

## Renames applied (18 symbol changes)

Confidence: **High** = body, signature shape and a second signal agree.
**Medium** = signature/wrapper shape agrees but the body is large and was not
traced instruction by instruction.

### Corrected animation-curve apply cluster (the old names were shifted)

| Address | Old name | New name | Size | Evidence | Confidence |
|---|---|---|---|---|---|
| `0x80017EA0` | `NuAnimCurve2SetApplyToJoint` | `NuAnimCurve2SetApplyToJointTransLoc` | 0x6B8 | Large body; its only small wrapper `0x80018E64` forwards with extra zero stack args. Mac `…ToJointTransLoc` 0x129760 is the large sibling of `…ToJoint` 0x129e10. | Medium |
| `0x80018E64` | `fn_80018E64` | `NuAnimCurve2SetApplyToJoint` | 0x34 | Tail-calls `0x80017EA0` with the extra args zeroed; Mac `…ToJoint` is a 0x40 wrapper of `…TransLoc`. | High |
| `0x8001752C` | `NuAnimCurve2CalcValEx` | `NuAnimCurve2SetApplyBlendToJoint2TransLoc` | 0x974 | Largest function in the cluster; blend variant with two time/value sets; Mac 0x128d70 is ~0x990. Not a CalcVal: it writes joint translation/rotation, not a scalar. | Medium |
| `0x80018E98` | `fn_80018E98` | `NuAnimCurve2SetApplyBlendToJoint2` | 0x60 | Wrapper that forwards to `0x8001752C`; Mac 0x129700 is a 0x60 wrapper. | High |
| `0x80018558` | `NuAnimDataRead` | `NuAnimCurveSetApplyToMatrix` | 0x29C | Takes a curve set, time and matrix out-pointer; no file I/O. Mac 0x129e50 is 0x2B0. | Medium |
| `0x800187F4` | `NuAnimDataFindVersion` | `NuAnimCurve2SetApplyToMatrix` | 0x27C | Per-curve loop writing a matrix; no version compare. Mac 0x12a100. | Medium |

### NuHGobj evaluation family

| Address | Old name | New name | Size | Evidence | Confidence |
|---|---|---|---|---|---|
| `0x80018F78` | `NuAnimData2LoadBuffEx` | `NuHGobjEval` | 0x2D0 | Walks a hierarchy object with an `NUJOINTANIM_s` and matrix; Mac `NuHGobjEval` 0x1261f0 is 0x2F0. | Medium |
| `0x80019248` | `NuAnimData2FixPtrs` | `NuHGobjEvalAnim2Root` | 0x3EC | Large hierarchy walk taking `nuanimdata2_s`, time and callback pointers; Mac 0x126510 is ~0x3A0. | Medium |
| `0x80019634` | `NuQuatSlerp` | `NuHGobjEvalDwa2` | 0x184 | Previously called `CalcTime`/`CalcVal` and indexes a `short` array, which a quaternion slerp never does; Mac `NuHGobjEvalDwa2(int,short*,nuanimdata2_s*,float)` is 0x170. | Medium |
| `0x800197B8` | `NuAnimCurveSetApplyToMatrix` | `NuHGobjEvalDwaBlend2` | 0x29C | Blend variant of `Dwa2` with a second `short*` and two float weights; Mac 0x126a20 is 0x2C0. | Medium |
| `0x80019A54` | `fn_80019A54` | `NuHGobjEvalAnimBlend2Root` | 0x4B4 | Blend sibling of `…Anim2Root`; Mac 0x126d20. | Medium |
| `0x80019F08` | `fn_80019F08` | `NuHGobjEvalAnim2` | 0x28 | Wrapper forwarding to `…Anim2Root` with a null callback; Mac 0x1264e0 is a 0x30 wrapper. | High |
| `0x80019F30` | `fn_80019F30` | `NuHGobjEvalAnimBlend2` | 0x28 | Wrapper forwarding to `…AnimBlend2Root`; Mac 0x126ce0 is a 0x40 wrapper. | High |
| `0x80018EF8` | `fn_80018EF8` | `NuAnimDataChunkCreate` | 0x80 | `NuMemAlloc` of a 0x14-byte stride array (`nuanimdatachunk_s`) with a count argument; Mac `NuAnimDataChunkCreate(int)`. | High |

### Forced reverts to placeholders

These four held names that belong to other functions once the cluster above
is corrected (the real holders are listed in the table). They are reverted to
placeholders rather than guessed: they are large apply-style routines whose
role was not established.

| Address | Old name | New name | Size | Reason kept unnamed |
|---|---|---|---|---|
| `0x80021784` | `NuAnimCurve2SetApplyBlendToJoint2TransLoc` | `fn_80021784` | 0xE40 | The real `…BlendToJoint2TransLoc` is `0x8001752C`; this one is a different, larger apply routine. |
| `0x800225C4` | `NuAnimCurve2SetApplyToJointTransLoc` | `fn_800225C4` | 0x4C4 | Real `…ToJointTransLoc` is `0x80017EA0`. |
| `0x80022A88` | `NuAnimCurve2SetApplyToMatrix` | `fn_80022A88` | 0x360 | Real one is `0x800187F4`. |
| `0x8002303C` | `NuAnimCurve2SetApplyBlendToJoint2` | `fn_8002303C` | 0x46C | Real one is `0x80018E98` (a 0x60 wrapper). |

The count of non-`fn_` names rose from 1462 to 1464 net of those four reverts.

## Existing names corroborated, unchanged

`NuAnimInit`, `NuAnimDataCreate`, `NuAnimCurveSetCreate`, `NuAnimCurveCreate`,
`NuAnimCurveDestroy`, `NuAnimData2LoadBuff`, `NuAnimDataLoad`,
`NuAnimDataLoadBuff`, `NuAnimCurve2CalcVal`, `NuAnimData2CalcTime`,
`NuAnimCurveCalcVal2` agreed with their argument shapes, the Mac demo
signatures and (where present) embedded strings or `NuMemAlloc` line order.

## Rejected or ambiguous candidates (left unrenamed)

| Address | Current | Candidate(s) | Why not renamed |
|---|---|---|---|
| `0x8001AF60`, `70`, `7C`, `84` | `fn_` | Getter/setter names from tiny-stub matches | 8–12 byte functions on `lbl_80407C28`/`lbl_80407C30`; any donor with the same stub shape matches, so no discriminating evidence. |
| `0x8001AF8C` | `fn_8001AF8C` | none | Small `nuvec_s` helper; role unclear. |
| `0x8002163C` | `fn_8002163C` | none | Pointer-relocation helper; Mac equivalents differ in size. |
| `0x8001BFC4` | `fn_8001BFC4` | clone wrapper | Wrapper over `NuAnimData2Clone` through `lbl_80407C38`; name would only restate the callee, which is itself unverified. |
| `0x800234A8`, `0x80023628` | `fn_` | `NuCameraDestroy`, `NuCameraLookAt` (LOW from mac-anchor queue) | Bodies are not camera code. Rejected. |
| `0x8001610C`, `0x80016154` | `fn_` | `pftaOn`/`pftaOff` (gamecode, LOW) | Cross-module and body shape unrelated. Rejected. |
| around `0x80018E64` | n/a | unnamed narnia/LSW2 body matches | Donor functions are themselves unnamed; no name gained. |

## Suspect existing names not touched

Outside the apply/eval cluster, so left for a later phase. Do not rely on them.

- `NuAnimKeyLerp` (`0x8001E76C`): the body loops over an array of 16-byte
  plane records at `+0x80` with count at `+0x9C`, dots a four-float input
  against each, and returns 0/1/2 (outside/inside/straddling). This is a
  sphere-vs-planes clip test, not a key lerp. Its caller
  `NuPortalClipTest` (`0x8001FAEC`) fits that. Neither is renamed here.
- The `NuAnimKey*` group around it, `NuRndrWaterRippleUpdate`,
  `NuPortalClipTest`.
- `NuAnimData2Init`/`Clone`/`Apply`/`CalcMatrix`/`Fixup`/`Relocate`/
  `BuffEvaluate`: names plausible but not independently verified.
- The clip/camera cluster `0x8001D0D8`–`0x80020400`.
- Remaining `fn_` placeholders in the range.

## Housekeeping notes

- The stale `NuQuatSlerp` prototype was removed from `src/numath/numath.h`;
  nothing referenced it. No verified `NuQuatSlerp` exists in
  `config/GL5E4F/symbols.txt`. The Mac demo has
  `NuQuatSlerp(nuquat_s*, nuquat_s*, nuquat_s*, float)` at `0x160e10`
  (0x1C0 bytes, after `NuQuatLerp`/`NuQuatHermiteInt`), so the function should
  exist in retail, but no GC candidate was found: the only 0x184-sized
  functions are `NuHGobjEvalDwa2`, `fn_8001D0D8`, `NuAnimKeyBlend`,
  `fn_80024A34`, `AIScriptLoadScriptTxt`, `fn_800BB7CC` and `fn_8014165C`
  (none in numath, none quaternion-shaped), and the CrashWOC size sequence
  `NuQuatMul`/`NuQuatNormalise`/`NuQuatSlerp` does not recur in the numath range.
  Declare it again, with the demo's `nuquat_s` tag, when a body is recovered.
- Generated artifacts (`call_graph.tsv`, `call_graph_rename_suggestions.md`,
  the mac-anchor queues, `named_to_unnamed_callees.md`) still use the old
  names. They were intentionally not regenerated so earlier research is
  preserved. Treat names in them as pre-correction.

## Next C-matching queue

All are still unmatched. Order is by lowest risk first. Wrappers are tiny and
make good first scoring targets for `tools/verify_fn.py`; the large bodies
should wait until the types exist.

| Priority | Function | Size | Needs |
|---|---|---|---|
| 1 | `NuAnimCurve2SetApplyToJoint` | 0x34 | `nuanimcurve2_s`, `nuanimtime_s`, `NUJOINTDATA_s`, `NUJOINTANIM_s`, `nuvec_s`, `numtx_s` declarations and the `…TransLoc` prototype |
| 2 | `NuAnimCurve2SetApplyBlendToJoint2` | 0x60 | same, plus second `nuanimtime_s` set |
| 3 | `NuHGobjEvalAnim2` | 0x28 | `NUHGOBJ_s`, `nuanimdata2_s`, `NUJOINTANIM_s`, `…Anim2Root` prototype with callback pointer type |
| 4 | `NuHGobjEvalAnimBlend2` | 0x28 | same plus blend args |
| 5 | `NuAnimDataChunkCreate` | 0x80 | `nuanimdatachunk_s` (0x14) is in `nuanim.h`; needs `NuMemAlloc` prototype and the source line macro value |
| 6 | `NuHGobjEval` | 0x2D0 | full `NUHGOBJ_s` layout |
| later | the `…TransLoc`, `…Root`, `Dwa` bodies | 0x184–0x974 | all of the above, plus callback function-pointer types |

Matching these still has to be proven by `verify_fn.py` against the target
asm; none is claimed here.

## Verification

- `bash build.sh`: succeeds; report unchanged at 2,032 / 1,569,532 bytes and
  23 / 4,147 functions.
- `python3 -m compileall -q configure.py tools tests` and
  `python3 -m unittest discover -s tests`: pass (3 tests).
- `python3 tools/symbols_to_map.py --all --output artifacts/GL5E4F.map`:
  succeeds (4,106 functions, 1,423 named in the map).
- `config/GL5E4F/symbols.txt`: addresses identical to `main`, only the name
  column changed on 18 lines, no duplicate names.
