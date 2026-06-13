# Engine Analysis

This document covers the Nu2 engine architecture, library boundaries, string cross-referencing, AI script parser details, and external reference projects.

## Library Boundaries

A first-pass map of likely game, runtime, and SDK regions for `GL5E4F`. The analysis is based on lightweight checks: `strings`, generated symbols, and section layout. Treat ranges as working hypotheses until they are validated against a successful split/link after the relocation blocker is isolated.

### High-Level Text Layout

| Range | Confidence | Notes |
| --- | --- | --- |
| `.init:0x80003100-0x8000348C` | High | Startup/init section. Entry symbol is `__start` at `0x80003100`. |
| `.text:0x800034A0-0x80150F04` | Medium | Mostly game/engine code based on address order and game strings before the SDK string cluster. |
| `.text:0x80150F04-0x8015BF48` | Low | Problematic analyzer region. `dtk` over-extends control flow from `0x80151AFC`; keep this region isolated until manually reviewed. |
| `.text:0x8015BF48-0x8015BFC0` | Medium | Small bridge/thunk region immediately before recognized low-level PPC helpers. |
| `.text:0x8015BFC0-0x8018CAFC` | High | Dolphin SDK / runtime tail region. Confirmed by SDK release strings and named symbols. |

The last generated text function is `fn_8018C51C` with size `0x5E0`, ending at `0x8018CAFC`. `.rodata` begins at `0x8018CB00`.

### Confirmed SDK Symbols

| Symbol | Address | Notes |
| --- | --- | --- |
| `__start` | `.init:0x80003100` | DOL entry. Renamed from generated `fn_80003100`. |
| `PPCMfhid2` | `.text:0x8015BFC0` | Low-level PPC helper. Likely start of SDK/runtime block. |
| `PPCMthid2` | `.text:0x8015BFC8` | Low-level PPC helper. |
| `PPCMtwpar` | `.text:0x8015BFD0` | Low-level PPC helper. |
| `OSRegisterVersion` | `.text:0x8015CAA8` | OS version registration. |
| `OSRegisterResetFunction` | `.text:0x80160F14` | OS reset callback registration. |
| `__GXInitRevisionBits` | `.text:0x8017CA2C` | GX init code anchor. |
| `GXInit` | `.text:0x8017CBD0` | GX init. |
| `__GXInitGX` | `.text:0x8017D1D0` | GX init internals. |
| `GXInitFifoBase` | `.text:0x8017DC3C` | GX FIFO setup. |
| `GXSetCPUFifo` | `.text:0x8017DD24` | GX FIFO setup. |
| `GXSetGPFifo` | `.text:0x8017DE4C` | GX FIFO setup. |
| `__GXFifoInit` | `.text:0x8017E008` | GX FIFO setup. |
| `GXSetMisc` | `.text:0x8017F5F4` | GX misc state. |
| `__GXPEInit` | `.text:0x8017FDB0` | GX pixel engine init. |
| `GXInitTexCacheRegion` | `.text:0x80181A34` | GX texture cache setup. |
| `GXInitTlutRegion` | `.text:0x80181B28` | GX TLUT setup. |
| `__GXSetTmemConfig` | `.text:0x80181DEC` | GX texture memory setup. |
| `__GXSetIndirectMask` | `.text:0x80182628` | GX indirect state. |
| `__GXFlushTextureState` | `.text:0x80182658` | GX texture state flush. |

### SDK String Evidence

`strings -a orig/GL5E4F/sys/main.dol` shows these Dolphin SDK release markers:

| Library | Release marker |
| --- | --- |
| OS | `May 21 2004 09:28:09 (0x2301)` |
| EXI | `Apr  5 2004 04:14:14 (0x2301)` |
| SI | `Apr  5 2004 04:14:16 (0x2301)` |
| DVD | `Apr  5 2004 04:14:51 (0x2301)` |
| VI | `Apr  7 2004 04:13:59 (0x2301)` |
| PAD | `Apr  5 2004 04:14:49 (0x2301)` |
| AI | `Apr  5 2004 04:15:02 (0x2301)` |
| AR | `Apr  5 2004 04:15:03 (0x2301)` |
| ARQ | `Apr  5 2004 04:15:04 (0x2301)` |
| AX | `Apr  5 2004 04:15:05 (0x2301)` |
| DSP | `Apr  5 2004 04:15:32 (0x2301)` |
| CARD | `Apr  5 2004 04:15:35 (0x2301)` |
| GX | `Apr  5 2004 04:14:28 (0x2301)` |

### Problem Region

`dtk dol info` and the initial split reported:

```text
Control flow from 1:0x80151AFC hit known function 1:0x801510E8
Overlapping functions 1:0x80151AFC-1:0x8015BF48 -> 1:0x80152038
```

Current config skips CFA for:

```yaml
skip_cfa_ranges:
- start: .text:0x80151AFC
  end: .text:0x8015BF48
```

The generated symbols in this region are mostly labels, followed by short functions at `0x8015BF48` through `0x8015BF98`, then recognized PPC helpers at `0x8015BFC0`.

## Nu2 Engine Notes

Likely Nu2 / Traveller's Tales engine subsystems in `GL5E4F`. The analysis is based on strings and current symbol anchors only. Do not rename symbols solely from this file; use it to guide later code review and split work.

### Reference Context

The LSW3 Wii decomp reference lists later Nu2 object groups such as:

- `nufile.master/nufile_Lump.cpp`
- `numath.master/numath_Lump.cpp`
- `legoapi.master/AIState.cpp`
- `legoapi.master/AverageAttack.cpp`
- `gameframework.master/gameframework_Lump.cpp`

Our target is older and GameCube-specific, but the same broad custom engine lineage is visible in strings. In particular, this DOL contains paths under:

- `../nu2api.legogc/...`
- `../gameapi.legogc/...`

### File And Datpack

Likely overlap with later `nufile.master`.

Evidence strings:

- `Too Many Devices - increase NUFILE_MAXDEVICES`
- `NuFileOpen - size seek failed - retrying`
- `NuFileClose - close failed, retrying`
- `NuFilePos - seek failed %d`
- `NuFileLoadBuffer "%s" to %08x size %08x`
- `NuFileBeginBlkRead : Block header mismatch!`
- `NuFileBeginBlkWrite : writing block... (id %c%c%c%c)`
- `NuFileEndBlkWrite : ...close block (id %c%c%c%c, size %d)`
- `NuFileWriteAddress:- Need to expand MAX_FILE_ADDRESS`
- `NuFileSetAddress:- Need to expand MAX_FILE_ADDRESS`
- `NuDatFileLoadBuffer - LSN Reading Packed file %s (%d->%d)`
- `NuDatFileLoadBuffer - LSN Reading file %s (%d)`
- `NuDatFileLoadBuffer "%s" - Unpacking`
- `NuDatFileOpen - opened %s`
- `NuDatFileOpen - File %s not found in datfile`
- `NuDatFileOpen - No free dat handles`
- `NuDatOpenEx - recoverable read error, retrying`
- `Cannot resume a fileparser that was not created with NuFParCreate`
- `NuFParResume - failed to reopen file!`

### Memory And Diagnostics

- `NuMemAlloc : Failed to alloc %d bytes!`
- `NuAllocHighInit : Already initialised`
- `NuAllocHigh : request for 0 bytes of memory denied`
- `NuHeap: Out of memory`
- `NuError - %s Line %d :`
- `NuWarning - %s Line %d :`
- `NuDebugMsg - %s(%d) :`
- `NuError trapped - %s`
- `MEMBLKTAIL01`

### String Tables

- `NuHexStringToI: Invalid character`
- `NuStringTableLoad() allocated tables %dk`
- `NuStringTableLoad() - Error: %s not a UNICODE file.`
- `NuStringTableLoad() allocated strings %dk`
- `NuStringTableSaveCharacterList: ERROR - over 512 characters used`

### Packed/Platform Loading

The `NuPP` prefix likely relates to packed/platform file loading.

- `NuPPLoadBuffer1 - recoverable seek failure, retrying`
- `NuPPLoadBuffer2 - recoverable read failure, retrying`
- `NuPPLoadBuffer3 - recoverable seek failure, retrying`
- `NuPPLoadBuffer4 - recoverable read failure, retrying (err%d)`
- `NuPPLoadBuffer5 - recoverable seek failure, retrying`
- `NuPPLoadBuffer6 - recoverable read failure, retrying`
- `alignment granularity exceeds FILEPAK_MAX_ALIGN`

### Math / Vectors

- `Missing NuVecClipTestPoint (VU CODE)`

### 3D, Scene, Materials, Texture, Render

Evidence paths and strings:

- `../nu2api.legogc/nu3d/ngc_obj/../NuHGobjEval.c`
- `../nu2api.legogc/nu3d/ngc_obj/../Nuanimbuffer.c`
- `ReadNuIFFGobjSet : Object already defined!`
- `ReadNuIFFGeomVtx : Lock VB failed!`
- `NuScene no longer supported.`
- `NuGobjCalcDims : Lock VB failed!`
- `NuMtlSetCurrentRenderPlane out of range`
- `NuTexRead cant find file %s.`
- `NuRndrItem : Unknown render item type!`
- `NuGScnBufferAllocAligned failed, scene buffer overrun.`
- `NuGHGReadEx - buffer overflow by %d bytes`
- `Not enough space in material for NGCMTL_MTLPACKET structure`
- `NuInitHardware : Insufficient buffer space`

Likely module names: `nu3d`, `NuHGobjEval`, `Nuanimbuffer`, `NuMtl`, `NuTex`, `NuRndr`, `NuGScn`, `NuGHG`.

### Animation

- `NuAnimLoad : Failed to open animation file %s!`
- `NuAnimDataLoadBuff: Is in new format, expecting old format.`
- `NuAnimDataLoadBuff: Is in old format, expecting new format.`
- `NuAnimCurve2CalcVal: should have already evaluated NUANIMKEYTYPE_NONE`
- `NuAnimCurve2CalcVal: should have already evaluated NUANIMKEYTYPE_BOOLEAN`
- `Function 'NuHGobjEvalAnim' No Longer supported!`
- `Function 'NuHGobjEvalAnimBlend2Root_buff' No Longer supported!`
- `Function 'NuHGobjEvalAnim2Rootx' No Longer supported!`

### Cutscene

- `Cutscene %s buffer overrun by %d bytes`
- `instNuGCutSceneSetRepeat: <%d> exceeds max num of repeats <32>`
- `instNuGCutSceneFind: Failed to find <%s>`
- `instNuGCutCamSysUpdate: internal error`
- `NuGCutLocatorSysFixUp: cannot fixup locator <%s> in cutscene`
- `instNuGCutDebrisLocatorUpdate: Can no longer see debris effect %s`
- `NuGCutRigidSysFixUp: cannot fixup rigid object <%s>`
- `instNuCGutRigidSysCreate: cannot reference rigid object <%s>, object was not fixed up`
- `NuGCutTriggerSysFixUp: Unable to find trigger <%s>`

Likely module names: `NuGCut`, `instNuGCut`.

### Save/Card

Game-facing card/save code layered above Dolphin CARD, using `NGCCARD_STATE_*` enums:

- `NGCCARD_STATE_NULL`, `NGCCARD_STATE_MOUNT`, `NGCCARD_STATE_WAITMOUNT`, `NGCCARD_STATE_CHECK`, `NGCCARD_STATE_WAITCHECK`, `NGCCARD_STATE_OPENFILE`, `NGCCARD_STATE_READFILE`, `NGCCARD_STATE_WAITREADFILE`, `NGCCARD_STATE_CREATE`, `NGCCARD_STATE_WAITCREATE`, `NGCCARD_STATE_READY`, `NGCCARD_STATE_LOAD`, `NGCCARD_STATE_SAVE`, `NGCCARD_STATE_WAITSAVE`, `NGCCARD_STATE_SAVESTATUS`, `NGCCARD_STATE_WAITSAVESTATUS`, `NGCCARD_STATE_FORMAT`, `NGCCARD_STATE_WAITFORMAT`, `NGCCARD_STATE_DELETE`, `NGCCARD_STATE_WAITDELETE`, `NGCCARD_STATE_FATALERROR`, `NGCCARD_STATE_NOCARD`, `NGCCARD_STATE_BROKEN`, `NGCCARD_STATE_IOERROR`, `NGCCARD_STATE_WRONGDEVICE`, `NGCCARD_STATE_ENCODING`

### Sound, Streaming, FMV, ARAM

- `NuSoundInitV called`
- `NuSoundInitV: Multiple initialisations`
- `NuSoundSetRequestTable called`
- `NuSoundSetRequestTable - tablesize x 16 exceeds max samples passed to NuSoundInitV`
- `**NuSoundSetReverb`
- `../nu2api.legogc/nusound3/ngc/ngc_obj/../NGCSound.c`
- `NGCAUDIO -> sampleid <%i> is not initialized, sound will be ignored`
- `NGCAUDIO -> Failed to acquire voice, sound will be ignored`
- `NGCAUDIO -> No more sound instances, sound will be ignored`
- `../nu2api.legogc/nusound3/ngc/ngc_obj/../NGCStreaming.c`
- `../nu2api.legogc/nusound3/ngc/ngc_obj/../NGCStreaming2.c`
- `NuFmvNgc_Alloc: Buffer not big enough`
- `Transferred %u bytes from %x to ARAM at %x`
- `Transferred %u bytes to %x from ARAM at %x`

Likely module names: `nusound3`, `NGCSound`, `NGCStreaming`, `NGCStreaming2`, `NuFmvNgc`.

### Game API / Editors / Particles

Evidence path: `../gameapi.legogc/gamelib/ngc_obj/../edbits.c`

- `edbitsGameSound table has overflowed - increase ED_MAX_SOUNDS in edbits.h`
- `Anim Editor`, `Particle Type`, `Highlighted Particle Type`
- `Attached Particles`, `Attached Sounds`
- `Save Params`, `Save Objects`, `Save Bridges`, `Save Grass`
- `Current List: Level`, `Current List: Character`
- `Texture Select`, `Render Settings`, `Particle Positioner`

### AI / Lego API

Likely conceptual overlap with later `legoapi.master`.

- `AIScriptBufferAlloc: Buffer not big enough!!!!!!!!!!!!`
- `AIScriptBufferAlloc: No buffer!!!!!`
- `AISysBufferAlloc: Level buffer not big enough!!!!!!!!!!!!`
- `AISysBufferAlloc: No buffer!!!!!`
- `AIPathNodeDistanceToPathNode: Problem with special route <%s>. Check nodes %d and %d`
- `AIEDITOR_PATHS`, `AIEDITOR_ROUTES`, `AIEDITOR_AREAS`, `AIEDITOR_LOCATORS`, `AIEDITOR_CREATURES`, `AIEDITOR_ANTINODES`
- `aieditor_Register:Too many modules registered. Increase MAXAIEDITORMODULES.`
- `AI Path Editor: "%s"`
- `AI Save FAILED`
- `AIPathCnxControllerCreate: Increase number of controllers from %d`
- `AIOverrideControl`, `SetAIOverrideControl`
- `AI_ObiWan`

Likely module names: `AIScript`, `AISys`, `AIPath`, `AIEditor`, `AIState`.

### Candidate Prefixes

`NuFile`, `NuDat`, `NuFPar`, `NuMem`, `NuAlloc`, `NuStringTable`, `NuPP`, `NuVec`, `NuAnim`, `NuHGobj`, `NuGobj`, `NuMtl`, `NuTex`, `NuRndr`, `NuGScn`, `NuGHG`, `NuGCut`, `NuSound`, `NuFmvNgc`, `NGCStreaming`, `NGCSound`, `NGCCARD`, `AIScript`, `AISys`, `AIPath`, `AIEditor`, `edbits`

### Notes For Future Symbol Work

- Prefer adding names only when a string has a direct code reference or when a known function body is identified from another source.
- Keep LSW3 names as architecture hints, not proof of identity.
- Expect this older GameCube build to use `nu2api.legogc` and `gameapi.legogc` names where LSW3 uses newer Wii-side groupings.
- The presence of editor/debug strings may make string-reference analysis very productive while full DOL splitting is blocked at relocation application.

## String Xref Analysis

`tools/string_xrefs.py` scans `orig/GL5E4F/sys/main.dol` for PowerPC string address materialization patterns and maps each hit back to the containing function from `config/GL5E4F/symbols.txt`.

Run:
```sh
python3 tools/string_xrefs.py
```

Generated reports:
- `build/xrefs/string_xrefs.txt` - all named string references found by the scanner.
- `build/xrefs/nufile_string_xrefs.txt` - focused NuFile, NuDat, NuPP, NuMem, NuDebug, and NuSpecial references.

The scanner is evidence-oriented, not a disassembler. It catches common Metrowerks patterns such as `lis` followed by `addi`, load/store displacement, or `ori`. It may miss indirect pointers and should be paired with source-string lineage evidence before promoting symbols.

### Current Nu2 File Module Map

The first xref pass promoted these functions:

- `NuFileOpen` at `0x800035F0`
- `NuFilePos` at `0x800039BC`
- `NuFileClose` at `0x80003C34`
- `NuFileLoadBuffer` at `0x80003DBC`
- `NuFileBeginBlkRead` at `0x80004390`
- `NuDatFileLoadBufferLsn` at `0x800044A8`
- `NuDatFileLoadBuffer` at `0x80004630`
- `NuDatFileOpen` at `0x80004CF8`
- `NuDatOpenEx` at `0x80004F6C`
- `NuFileEndBlockRead` at `0x80005B1C`
- `NuMemAlloc` at `0x80006F74`
- `NuMemFree` at `0x8000726C`
- `NuPPLoadBuffer` at `0x800082A4`
- `NuSpecialFind` at `0x80012D00`
- `NuSpecialFindMulti` at `0x80012E94`

Control-flow evidence:
- `NuFileLoadBuffer` calls `NuDatFileLoadBuffer` only when the DAT context global is active.
- `NuDatFileLoadBuffer` first calls `NuDatFileLoadBufferLsn`; on success it returns that result directly.
- If the LSN helper fails, `NuDatFileLoadBuffer` calls `NuDatFileOpen`, performs seek/read retry loops, emits the `Unpacking` message, and returns the loaded byte count or `0`.

## AI Script Notes

The AI script parser lives around `0x8007BFD8..0x80085568` and is backed by the source-path string `../gameapi.legogc/ai/aisys/ngc_obj/../aiscript.c`.

### Parser-Side Names

- `AIScriptParseConditions` at `0x8007BFD8` - Parses `IF`, comparison operators, `param`, `goto`, and `and`. Builds condition nodes until `}`.
- `AIScriptXRefScript` at `0x8007CE54` - Handles `Script`, `Source`, `Global`, `Level`, `ReturnState`, and `CONDITIONS`. Emits the `xRefScript` diagnostic path when a source reference is missing.
- `AIScriptLoadScp` at `0x8007D248` - Accepts `.scp` names and builds script state/action/condition structures.
- `AIScriptLoadScriptTxt` at `0x8007E3CC` - Opens `%s\script.txt`, extracts script names, and calls `AIScriptLoadScp`.
- `AIScriptResolveReferences` at `0x8007E550` - Resolves `default`/`Base` style script references after load.

Generated reports:
- `build/xrefs/script_parser_xrefs.txt`
- `build/xrefs/script_handler_xrefs.txt`

### Remaining High-Value Targets

- `fn_8007CC34` references `Script`, `Source`, `Global`, and `Level`; likely a source/scope parsing helper.
- `fn_8007C534`, `fn_8007C8A4`, and `fn_8007CAFC` sit immediately before the named parser functions and should be reviewed together with allocator state.
- Runtime handlers at `0x8012D8F8..0x8013C414` are still mostly unnamed; use `build/xrefs/script_handler_xrefs.txt` and call-site context before promoting names.

## External References

### LegoCloneWarsWii

Repository: <https://github.com/ThePlayerRolo/LegoCloneWarsWii>

Decompilation of *Lego Star Wars III: The Clone Wars* for Wii (SC4E64 USA Rev 0 Debug, SC4P64 EU Rev 0 Debug). Same broad Traveller's Tales / Nu2 engine lineage, but a much newer engine generation.

Useful details from its `configure.py`:
- Linker version: `Wii/1.5`.
- Uses `-enc SJIS`, `-func_align 4`, and `-code_merging safe,aggressive`.
- Progress categories include `game`, `runtime`, `trk`, `sdk`, `msl`, and `nulib`.
- Runtime objects include `global_destructor_chain.c`, `__init_cpp_exceptions.cpp`, `Gecko_ExceptionPPC.cpp`, `__va_arg.c`, `GCN_mem_alloc.c`, `ptmf.c`, `runtime.c`, `__mem.c`, `NMWException.cpp`.
- Nu library placeholders: `nufile.master/nufile_Lump.cpp`, `numath.master/numath_Lump.cpp`
- Game-code placeholders: `legoapi.master/AIState.cpp`, `legoapi.master/AverageAttack.cpp`, `gameframework.master/gameframework_Lump.cpp`

Good reference for naming conventions around `Nu`, `legoapi`, and `gameframework` components. Useful for broad architecture and folder naming; less likely to provide direct matching functions for this older GameCube target.

**Do not copy code or symbols blindly** — platform, compiler version, engine version, and debug/release differences matter.
