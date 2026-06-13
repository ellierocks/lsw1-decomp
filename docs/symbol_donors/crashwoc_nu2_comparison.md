# Crash: Wrath of Cortex Decomp — Nu2 Convention Reference

**Repo:** https://github.com/denzi-gh/crashwoc-decomp
**Engine:** Nu2 (same as LSW1 GC)
**Platform:** GameCube (same)
**Status:** Mature — 0 `fn_` entries, 112-module splits, complete source tree

## Source Tree Layout

```
src/
  gamecode/    — ai, camera, creature, game, vehicle, etc.
  gamelib/     — nubridge, nuwind, terrain, ed*, gcutscn, glutils, debris
  nu3dx/       — nuanim, nucamera, nuobj, nurndr, nuscene, nutex, nuwater, ...
  numath/      — nuvec, numtx, nuquat, nutrig, nufloat, nurand, nuplane
  nucore/      — nufile, numem, nuerror, nufpar
  nusound/     — nusound
  system/      — gs/ (GX), gc/ (DSP), ss/ (SDK)
  runtime/     — libc, libm, libgcc, sn/
```

## Key Naming Conventions

| Pattern | crashwoc | LSW1 (current) | Recommendation |
|---------|----------|-----------------|----------------|
| Function naming | `PascalCaseNoUnderscores` (`InitCrates`, `FindAIType`) | `PascalCase_With_Underscores` (`Action_Idle`) | Keep existing for dispatch handlers; use PascalCaseNoUnderscores for new names |
| Engine API prefix | `Nu*` (`NuAnimDataRead`, `NuBridgeAlloc`) | `Nu*` (`NuFileOpen`, `NuMemAlloc`) | Already consistent — keep |
| Anonymous functions | **0 `fn_`** | 3,703 `fn_` | Rename to descriptive PascalCase |
| Anonymous data | `lbl_*` (7,533) | `lbl_*` (11,772) | Keep for scalars; name larger structs |
| Data tables | `CMD_FISHSWIM` (SCREAMING_SNAKE for arrays) | `gMenuTracker` (camelCase) | No strong preference |
| Static locals | `name.number` (`Vel.211`) | n/a yet | Match MWCC pattern |
| Split modules | 112 | 9 (sections only) | Target 112 splits |
| Include hierarchy | `nu.h` → `nu3dx.h`/`numath.h`/`nucore.h`/... | None yet | Mirror hierarchy |

## Nu2 API File Set (confirmed)

| Module | Files | Our Match? |
|--------|-------|------------|
| nucore | `nuerror.c`, `nufile.c`, `nufpar.c`, `numem.c` | match 19 LSW1 `Nu*` symbols |
| numath | `nuvec.c/h`, `numtx.c/h`, `nuquat.c/h`, `nutrig.c/h`, `nufloat.c/h`, `nurand.c/h`, `nuplane.c/h`, `nuvec4.c/h`, `nu_asm.c/h` | not yet split |
| nu3dx | `nuanim.c/h`, `nucamera.c/h`, `nuobj.c/h`, `nulight.c/h`, `numtl.c/h`, `nurndr.c/h`, `nuscene.c/h`, `nutex.c/h`, `nutexanm.c/h`, `nuvport.c/h`, `nuwater.c/h`, `nucvtskn.c/h`, `nuglass.c/h`, `nuhaze.c/h`, `nuhgobj.c/h` | partial |
| nusound | `nusound.c/h` | not yet found in LSW1 |

## AI System Differences

crashwoc uses baked-in `AIType`/`creatcmd` arrays in `ai.c`. LSW1 uses a script-parsing AI system (`AIScriptParseConditions`, `ScriptKeyword_*`). These are fundamentally different — crashwoc has no equivalents for `Action_*` or `Condition_*` dispatch tables.

## SDK Code (for future splits)

crashwoc splits SDK into: `OS*.c` (105 modules!), `GX*.c`, `AX*.c`, `CARD*.c`, `AR*.c`, `DSP*.c`, `VI*.c`, `Mtx.c`, `dBG.c`, etc. This is beyond current needs but worth referencing.

## Implications for LSW1

1. **Source tree:** Mirror crashwoc's `src/nu3dx/`, `src/numath/`, `src/nucore/`, `src/gamecode/`, `src/gamelib/` layout
2. **`fn_` elimination:** Crashwoc proves 100% function naming is achievable — keep drive to rename
3. **Split granularity:** Target 100+ modules like crashwoc
4. **Dispatch naming:** LSW1's `Action_*`/`Condition_*` are correct for our script-based AI (crashwoc has no equivalent)
