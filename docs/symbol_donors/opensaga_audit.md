# OpenSaga donor audit and remaining queue

Audited on 2026-10-30 against `opensagadev/saga` at
[`7bb2402aa902d08f0979892ca23ee80e809e5ee1`](https://github.com/opensagadev/saga/tree/7bb2402aa902d08f0979892ca23ee80e809e5ee1),
the same commit already pinned by the [matching lane](opensaga_matching.md).
LSW1 discovery used `496ae7f` and the current GC symbols. No donor pin update,
source copying or execution of donor scripts was needed.

## Scope and interpretation

The repository-wide discovery pass inventoried 1,046 files, screened all 921
source/header files under `src/`, and joined the complete committed matching
report's demangled function names to named GC `.text` symbols. It found 484
distinct shared names across 486 donor source-owner rows, including 25 cutscene
names. These are leads, not verified equivalents. The pass inspected selected
bodies against GC assembly; it did not establish semantic equivalence for every
function. Unnamed GC bodies and differently named equivalents are outside this
join. Raw inventories remain local; source paths and target addresses below are
the durable queue. Check current symbols and worktree ownership before claiming
one target.

Saga's Android x86 binary retained function symbols and C++ names. Compiler
`_GLOBAL__sub_I_` names also supply original source basename clues, but its
[source-structure documentation](https://github.com/opensagadev/saga/blob/7bb2402aa902d08f0979892ca23ee80e809e5ee1/doc/source-structure.md)
reports no DWARF or `STT_FILE` symbols. Its types and TU reconstruction still
need independent GC evidence. Our Mac donors already supply some naming
coverage; Saga adds reconstructed bodies, calls, tables and test scenarios.

The pinned [matching report](https://github.com/opensagadev/saga/blob/7bb2402aa902d08f0979892ca23ee80e809e5ee1/matching.json)
labels the README's 87.56% as **fuzzy** similarity. It separately reports
15.017515% matched code bytes, 7,260/13,459 matched functions and 3.722281%
matched data. These are its tool's measures, not our strict symbol, section,
relocation and linked-retail-range acceptance gates. A 100% donor function can
also have much less behavior than its GC namesake.

## NuStrCpy return contract

GC `NuStrCpy` (`0x80007B6C`, `0x3C` bytes) initializes `r9` to zero, increments
it at `0x80007B84` for each byte copied, including the terminating NUL, and
returns it with `mr r3,r9` at `0x80007BA0`. A null source writes NUL to the
destination and returns zero. A non-null empty string returns one. This is a
32-bit integer count, not a destination pointer.

Saga's `src/nu2api/nucore/nucore_includes.c:5299` instead returns the length
excluding NUL. Its loop and return expression cannot simply be transplanted.
The declaration in [`nufpar.c`](../../src/nucore/nufpar.c) is corrected to `s32`;
all three parser call sites discard the result. This corrects the earlier
[matching-results explanation](opensaga_matching_results.md#what-transferred-and-what-did-not),
not an observed runtime failure.

After correcting the declaration, `bash build.sh` passes, all six parser
functions strictly match, and the complete unit section/relocation proof passes:

```sh
python3 tools/verify_unit.py nucore/nufpar --build --functions
```

For an individual caller, the equivalent strict command is:

```sh
python3 tools/verify_fn.py NuFParCreate --unit nucore/nufpar --build --strict-symbols
```

No new function or data bytes are promoted by this correction. The existing
1,072-byte parser range retains its match; the wider parser/helper lane's
historical promotion remains documented in [results](opensaga_matching_results.md).

## Remaining matching queue

Finish the existing [parser/helper queue](opensaga_matching_results.md#next-queue-and-limits)
before expanding. The strict-match expansion gate has passed, but each new
layout and body still needs its own proof. Paths below are relative to the
pinned Saga tree; the shared names alone establish no identity.

| Order | GC leads | Donor source | Next step |
| --- | --- | --- | --- |
| 1 | `NuStrLen` `0x80007BFC`, `NuStrCat` `0x80007B2C`, `NuStrCmp` `0x80007D48`, `NuToUpper` `0x80008074`, `NuToLower` `0x800080A0` | `src/nu2api/nucore/nucore_includes.c` | Small adjacent helpers; establish signedness, termination and return contracts from GC, especially before using their results in larger candidates. |
| 2 | `NuAngAdd` `0x8000B00C`, `NuAngSub` `0x8000B024`, `NuPower2` `0x8000B03C` | `src/nu2api/numath/numath_includes.c` | Inspected add wraps to signed 16-bit; power-of-two code starts at 128 above 127, otherwise 1, then doubles. These semantics correspond to GC. Score before claiming a match; existing vector/matrix candidates need status checks before further work. |
| 3 | `Condition_Message` `0x801373BC`; then selected small conditions and init handlers | `src/gameapi/ai/aisys/gameaiscript.cpp` | GC returns zero for a null fifth argument, otherwise loads its float at `+0x28`, corresponding to donor message semantics. Confirm only this accessed field; do not import a complete AI object layout. |
| 4 | `NuCameraGetMtx` `0x80023D80` and adjacent getters; `NuSpecialGetInstAnim` `0x80013580`; `NuSplineFind` `0x800138D4` | `src/nu2api/nu3d/nu3d_includes.cpp` | Isolate global/field evidence. GC special helper follows `special+8` then `+0xB4`, else `special+4` then `+0x44`, with null handling. Getter and spline names remain discovery leads. |
| 5 | `NuGCutSceneSysInit` `0x8003F018`, `instNuGCutSceneSetEndCallback` `0x8003F31C`; then lifecycle helpers | `src/legoapi/cutscenes/gcutscn.cpp` | Underused lane with 25 shared names. Start with init/setters before camera, locator and rigid update bodies. No strict candidate was attempted in the audit. |
| 6 | `NuDatFileOpenSize` `0x80005504`, `NuDatFileFindTree` `0x80005F5C`; later `NuPPLoadBuffer` `0x800082A4`, `NuPPUnpack` `0x800085B0` | `src/nu2api/nucore/nucore_includes.c` | Resume the existing archive lane with size/find helpers. Confirm archive version, packing, endian and device behavior before larger loaders. |
| 7 | `FullReflect` `0x8005A860`, `GenDebMomAdjFromPos` `0x80064CF4` and adjacent debris callbacks; terrain/lighting/ripples | `src/legoapi/render/core/gameliball.cpp` and related render files | Algorithm and test-scenario evidence; larger structures and TCS changes require bounded per-function comparisons. |
| 8 | `BuyShopItem` `0x80147E3C`; world, gizmo, character and combat families | `src/legoapi/menus/screens/shop.cpp` and corresponding domains | Later gameplay behavior can differ substantially. Independently identify the GC body before using donor structures or control flow. |

The existing AI PARAM draft (69/78) and legacy animation draft (43/105) remain
unverified historical attempts, with their next hypotheses in the linked lane
notes. This audit did not remeasure them or call untouched targets plateaus.
For each selected target, score with ProDG, record improvements with
`hillclimb.py try`, then use strict function and whole-unit promotion checks.

## Observed differences and transfer limits

- **Force-object condition:** Saga `gameaiscript.cpp:2066` returns zero for
  `Condition_NumForceObjects`; GC `0x801371EC` has a `0x84`-byte body calling
  `fn_8012741C` with flags and converting a count to float. Donor 100% similarity
  for its nine-byte body does not establish the GC behavior.
- **Hints:** Saga `gameaiscript.cpp:2609` implements `Action_SetHint` as a no-op
  returning one. GC `0x8013AAB0` has a `0x90`-byte body parsing `ID` parameters
  and calling `fn_800D875C`. Keep the GC implementation authoritative.
- **Formation state:** GC
  `Action_BreakFormation` (`0x80139B48`) follows packet `+0xF0` and clears a bit
  in group `+0x50`. Saga `gameaiscript.cpp:2587` assigns zero to
  `is_in_formation`, which is itself a **one-bit field** in
  `src/gameapi/ai/aisys/aisys.h:317`. This supports packed formation-state
  semantics; derive the GC bit position and layout from GC.
- **Stale names:** the current `NuWaterRender` name at `0x800069F4` conflicts
  with the parser/interpreter evidence already recorded in the
  [parser queue](opensaga_matching_results.md#next-queue-and-limits). A same-named five-byte Android
  stub is not evidence for water rendering. No rename is applied here.
- **Existing exclusions:** GC parser pool/stack layouts and animation mask-byte
  order differ; the parser has three `0x54C` slots with `0x1000` buffers. New
  curve formats do not establish GC layouts. Retain the lane's opaque fields.

Android/JNI/Java/touch, GLES, OpenSL and host adapters are not direct GC import
candidates. Modern Nu2/C++ infrastructure also needs age/identity evidence.
Some platform files retain legacy names, so inspect behavior rather than using
filenames as proof. Dolphin SDK work continues through its existing lane.

## Other evidence worth consulting

Saga's `tests/terrain/terrain_collision_test.cpp` covers hit types, scaling,
rotation, zero normals and preserved state. `tests/animation/pak_loader_test.cpp`
exercises 32-bit pointer relocation and nested pointers; GC format compatibility
must be established first. Combat, ripple and lighting tests provide additional
behavior scenarios, not GC validation.

`scripts/checks/check_host_boundary.py` rejects host dependencies and behavioral
host-build forks in shared code. The restructure tooling separates original
relocations, GOT order and TU placement. These are workflow ideas; no Saga
scripts or compiler flags have been adopted. Our existing strict verifiers and
retail range checks remain the acceptance authority.
