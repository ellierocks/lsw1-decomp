# nungc-nufmv matching evidence

Measured from `0599d74` on 2026-11-11 with the pinned ProDG 3.9.3 compiler.
The item remains open; partial units retain retail fallback objects.

| Measure | Before | After | Change |
| --- | ---: | ---: | ---: |
| Global objdiff matched code bytes | 406,596 | 408,700 | +2,104 |
| Global matched functions | 2,601 | 2,613 | +12 |
| Item unmatched code bytes | 15,772 | 13,668 | -2,104 |
| Item unmatched functions | 31 | 19 | -12 |
| Complete source units | 149 | 151 | +2 |
| Whole linked image differing bytes | 0 | 0 | 0 |

Newly promoted code is **904 bytes / six functions**. The other **1,200 bytes /
six functions** strictly match and count in objdiff, but their units are not
complete and remain `NonMatching`. No symbols, split boundaries, compiler pins
or verifier rules changed.

## Promoted units

| Unit | Functions and code bytes | Owned sections |
| --- | --- | --- |
| `auto/nufmv/ngc/HVQM4PlayerEx` | `fn_80049AFC` (8), `fn_80049B04` (212), `fn_80049BD8` (12), `fn_80049BE4` (120), `fn_80049C5C` (356) | `.text` 708; `.rodata` 255 |
| `auto/gamelib/glutils` | `fn_80040A88` (196) | `.text` 196; `.rodata` 500 |

Both complete-section and relocation proofs pass, as do all six strict function
checks. The linked gate passes for all promoted units, and the entire rebuilt
DOL has zero differing bytes:

```sh
python3 tools/verify_unit.py auto/nufmv/ngc/HVQM4PlayerEx --build --functions
python3 tools/verify_unit.py auto/gamelib/glutils --build --functions
bash build.sh
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
```

HVQM4PlayerEx was an unverified draft with no configured source at baseline.
Its GCC prologues and `nu2api.legogc` panic path establish ProDG provenance.
The first four functions are Nu2 glue. In `fn_80049C5C`, a decoder subobject
pointer and loads ordered horizontal sampling, width, height, vertical sampling
reproduce the retail register allocation. Including `VuMath.h` after the glue
reproduces the header's pool after the first float constant.

For glutils, GC's material layout comes from `nu3dx/nu3dx.h`; the chained diffuse
stores must run from component 2 to 0. Its trailing pool contains header copies
and stripped functions from subsequent TUs within the existing split. These
unreferenced items were classified with `rodata_layout.py` and retained without
moving boundaries. Full section and linked proofs cover the entire pool.

## New strict matches in partial units

| Unit | Function | Instructions | Code bytes |
| --- | --- | ---: | ---: |
| `auto/nungc/ngcapi` | `clipRoomAgainstFrustrum` | 71/71 | 284 |
| `auto/nungc/ngcapi` | `fn_80037D3C` | 27/27 | 108 |
| `auto/nufmv/ngc/nufmvmtl` | `fn_800483D0` | 50/50 | 200 |
| `auto/nufmv/ngc/nufmvmtl` | `fn_8004856C` | 32/32 | 128 |
| `auto/nufmv/ngc/nufmvmtl` | `fn_80048998` | 91/91 | 364 |
| `auto/nufmv/ngc/nufmvmtl` | `NuFmvNgc_Alloc` | 29/29 | 116 |

Acceptance used `python3 tools/verify_fn.py NAME --unit UNIT --build
--strict-symbols`. All 35 retained strict matches across ngcapi, ps2dma and
nufmvmtl were rechecked; there are no objdiff regressions in any unit.
`fn_80048348` remains a baseline match: splitting its initial zero stores
preserves it with the now-volatile movie state. It is not new progress.

GC-supported layout facts in the local source views:

- Rooms have an index pointer at +0 and a signed 16-bit count at +0xc.
  `fn_80037D3C` uses signed remainder by eight, rather than the donor's mask.
- Portal scenes use spheres +0x188, boxes +0x18c and display +0x1a0;
  display override bits are +0x88. Frustum planes are +0x80 with signed
  16-bit count +0x9c. The clipping function retains a 0x30-byte unused local
  area and the explicit zero switch arm required by retail code generation.
- Voice-mix reset writes indices 0, 2, 20, 18 in that order and passes both
  the voice and mix pointer to `AXSetVoiceMix`. The state machine passes the
  decoded frame's pixel pointer as the first of four video-copy arguments.

## Donor review

The HVQM4 donor is [ShulkMaster/mk-da](https://github.com/ShulkMaster/mk-da/tree/3d6f17751309eb5e7cb5b6162c21360cbb5a35bf),
commit `3d6f17751309eb5e7cb5b6162c21360cbb5a35bf`, **CC0-1.0** (LICENSE checked).
Reviewed `src/hvqm4/HVQM4PlayerEx.c`, `hvqm4play.c`, `HVQM4Bufa.c`,
`HVQM4Bufv.c`, and `include/hvqm4player.h`, `hvqm4buf.h`, `hvqm4alloc.h`.
It supplies logic, field and API evidence; GC instructions establish offsets
and acceptance. Its CodeWarrior code generation does not transfer to this
ProDG player. Provenance and licence are also recorded in the source header.

OpenSaga review follows the [donor workflow](symbol_donors/opensaga_matching.md)
at `7bb2402aa902d08f0979892ca23ee80e809e5ee1`. Reviewed conclusions were
recorded with `hillclimb.py try --donor-note`:

- `src/nu2api/nu3d/android/nuportal_android.c`: room clipping and visibility
  semantics transfer, but extra Android null/display/box paths do not.
- `src/nu2api/nucore/nucore_includes.c`: newer Android hardware initialization
  does not establish the GC initialization sequence.
- `src/nu2api/nu3d/glutils.c`, `CreateAlphaBlendTexture`: material creation
  semantics transfer; PC bitfields and its parameter ABI do not. GC has seven
  arguments and separate byte attributes.
- `objectsall.cpp`, `InitParts`: GC has 64 groups of stride 0xb0, eight signed
  16-bit slots at +0x10 and eight signed byte flags at +0x20; 32 emitters of
  stride 0x6c with index +0xc; particles use stride 0x1f4. Reject the donor's
  128 groups and 40 emitters.
- Searches for ps2dma/NuDma/NuScratch found no corresponding implementation,
  only scratch callers in `gameliball.cpp`. No OpenSaga HVQM4 implementation
  was found. Neither absence establishes that differently named code is absent.

Japanese PC task packs have no pair for clipping, room flags, NuInitHardware,
glutils, particles, the FMV allocator, or `fn_80039CB0`. Additional anchor
review found:

- GC `fn_80039DB4` / JP `0x004B34F0`: supported by both diagnostics, two
  diagnostic callees, 16-byte tag-length arithmetic and clearing the current
  packet. PC uses a different platform source path; no PC layout was imported.
- Inferred GC `fn_800395E8` / JP `0x004FAAA0`: rejected as implementation
  evidence. The PC body has three calls versus GC's 17 and lacks the GC float
  constants and branch structure.
- Inferred GC `fn_800483D0` / JP `0x004E5800`: rejected. The PC body destroys
  and clears object slots; it lacks GC's movie globals and four message cases.

## Remaining candidates and next work

Scores below are exact instructions, not percentages or acceptance. Uninstalled
bodies and raw logs remain under ignored `work/`. The large generated drafts
have not exhausted source reconstruction; this handoff does not park the whole
item merely because a sweep ended.

| Target(s) | Best evidence / limit | Next hypothesis |
| --- | --- | --- |
| `fn_80037E1C`, `fn_80038058`, `fn_80038210` | Generated drafts: 45/116, 49/110, 112/202 | Recover typed GC state and call prototypes before scheduling search. |
| `fn_80038538` | 24/27, 28 emitted; absolute bus-clock load materializes an extra address instruction. Direct, indexed and volatile address forms did not fix it. | Recover the original bus-clock macro and argument lifetimes; retain the u64 alarm ABI. |
| `fn_80038628`, `NuInitHardware` | Generated dispatcher is unhandled; hardware draft has a noninteger switch expression. | Type the GC initialization state/dispatcher first; Android donor is not a substitute. |
| `fn_800395E8` | 215/382 generated draft; proposed JP pair rejected above. | Reconstruct GC render-state helper prototypes and typed state bits. |
| `fn_80039BE4`, `fn_80039CB0`, `fn_8003A134` | Already strict MATCH at baseline (48/48, 30/30, 56/56); 536 bytes remain below 100% in objdiff solely on pool placement. No new progress claimed. | Complete missing functions and reproduce per-TU pools at current boundaries. |
| `fn_80039D74` | Aggregate-return draft 3/16; explicit output/input-pointer variant 9/16 regressed the existing caller to 34/56. Both rejected. Statement-order/union/u64-copy probes did not solve it. | Recover the original packet/cursor aggregate and template-copy helper while preserving hidden-return ABI in callers. |
| `fn_80039E78`, `fn_80039FAC` | Generated aggregate/pointer expressions do not compile; repaired E78 body only 20/77. | Model the 64-byte local DMA payload and update-address writes before permutation. |
| `fn_800485EC` | 94/96, 96 emitted; corrected signed 16-bit samples and two-argument buffer-free API. Focused 180-second permuter did not match. | Resolve the zero-index and conversion-constant scheduling around the sample loop. |
| `fn_8004876C`, `fn_80048B04`, `fn_8004901C` | Generated drafts: 97/139, 79/209, 356/608 | Replace integer player/buffer guesses with GC-confirmed API and ownership views. |
| `fn_80048E48` | Generated draft references an undeclared stack temporary. | Reconstruct the movie descriptor and out-parameter call shape from GC. |
| `fn_80071E9C` | Retained comparison source 92/99; focused 180-second search reached 95/99, 100 emitted, two unverified address-taken strings. Still MISMATCH. | Recover nested-loop induction scope; retail decrements the outer counter at the tail and branches on nonzero. Then resolve string pools with the full unit gate. |

No candidate requiring an unresolved runtime layout was promoted. No Dolphin
session was driven. Permuter runs were bounded to 180 seconds each.

## Validation

`bash build.sh`, `python3 tools/matching_progress.py`, both complete-unit proofs,
the linked comparison, portable compile/configure/map/shell checks and
`git diff --check` pass. `python3 -m unittest discover -s tests` passes **633
tests**, with two skipped. Required current-head GitHub CI remains a separate
gate; local success does not assert PR review or CI approval.
