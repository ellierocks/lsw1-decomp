# Module Split Plan

Based on call-graph clustering (244 entries in `proposed_splits.txt`) and Crash: Wrath of Cortex conventions (`docs/symbol_donors/crashwoc_nu2_comparison.md`).

## Section Boundaries (from `symbols.txt`)

```
GL5E4F/sections:
  .init      start:0x80003100 end:0x800034A0
  .text      start:0x800034A0 end:0x8018CB9C
  .rodata    start:0x8018CB9C end:0x801B0640
  .data      start:0x801B0640 end:0x801FD080
  .bss       start:0x801FD080 end:0x80407A40
  .sdata     start:0x80407A40 end:0x8040A2C0
  .sbss      start:0x8040A2C0 end:0x8040B300
  .sdata2    start:0x8040B300 end:0x8040BD40
```

## Proposed Modules (crashwoc naming convention)

| Module | Source file | Functions | Confidence |
|--------|-------------|-----------|------------|
| `gamecode/ai.c` | ai/AIScript.c | AIScriptParse, Action_*, Condition_* | HIGH (3 clusters) |
| `gamecode/podrace.c` | game/Podrace.c | PodraceLoad*, PodraceUpdate* | HIGH (11 clusters, named funcs) |
| `gamelib/menusystem.c` | menu/MenuSystem.c | Menu_*, PauseMenu_*, DebugMenu_* | MEDIUM (24 clusters) |
| `system/gs/gx.c` | gx/GX.c | GXInit, GXSet* | MEDIUM (6 clusters) |
| `runtime/ppc.c` | runtime/PPC.c | PPCMtwpar, PPCMfhid2 | HIGH (single unique addr) |
| `runtime/memory.c` | runtime/Memory.c | MemZero, alloc/free helpers | LOW (11 clusters, mostly fn_*) |
| `nucore/nufile.c` | nu2/NuFile.c | NuFileOpen/Close/Pos, NuFileLoad* | CATCH-ALL (170 clusters, spans text) |
| `nucore/numem.c` | nu2/NuMemory.c | NuMemAlloc/Free | LOW (8 clusters) |
| `nucore/nudata.c` | nu2/NuData.c | NuDatFileOpen/LoadBuffer | LOW (7 clusters) |
| `nu3dx/nuanim.c` | nu2/NuAnimation.c | NuAnimCurve2CalcVal | HIGH (single func) |
| `gamelib/ui.c` | ui/UI.c | UI_DrawText | HIGH (single func) |

## Format (crashwoc-style, if we refactor)

```yaml
GL5E4F/modules:
  gamecode/ai.c:
    .text:      start:0x80079C48 end:0x8007E3CC
  gamecode/podrace.c:
    .text:      start:0x8014E468 end:0x80150B9C
    .text:      start:0x80164524 end:0x80164BCC
    .text:      start:0x80182790 end:0x80184F18
  gamelib/menusystem.c:
    .text:      start:0x8006ECF0 end:0x80071AEC
    .text:      start:0x80076CF0 end:0x80079434
    .text:      start:0x8015C194 end:0x80160F98
```

## Status
- Current: 9-section-only split (no modules)
- Proposed_splits.txt covers ~5.9% of `.text` functions
- Data/rodata/BSS not yet assigned — needs map-file or Ghidra analysis
- All `.text` not assigned to a specific module defaults to `nucore/nufile.c` (catch-all)

## Action Plan
1. Add HIGH-confidence modules first: `runtime/ppc.c`, `gamelib/debugmenu.c`, `gamelib/ui.c`, `nu3dx/nuanim.c`
2. Add MEDIUM modules: `gamecode/podrace.c`, `system/gs/gx.c`, `gamelib/menusystem.c`, `gamecode/ai.c`
3. Add LOW/catch-all: `nucore/nufile.c`, `runtime/memory.c`, `nucore/numem.c`, `nucore/nudata.c`
4. Assign rodata/data/bss ranges after Ghidra analysis of data cross-references
