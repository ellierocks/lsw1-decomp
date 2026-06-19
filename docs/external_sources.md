# External Source Analysis

This document summarizes findings from the extra knowledge bases under `orig/` beyond the target US GameCube release: PAL GameCube, LSW2 PS2 prototype, TCS Wii prototype, and PC versions.

## Overview

| Source | Path | Strings | Shared with US GC |
| --- | --- | --- | --- |
| US GameCube (target) | `orig/GL5E4F/sys/main.dol` | 7,785 | — |
| PAL GameCube | `orig/GL5P4F/` | 7,757 | 7,292 |
| LSW2 PS2 Prototype | `orig/lsw2-ps2-prototype/` | 7,491 | 2,534 |
| TCS Wii Prototype | `orig/tcs-wii-prototype/` | 11,321 | 2,604 |
| PC Retail | `orig/pc/usa/retail/` | 40,703 | 3,656 |
| PC Demo | `orig/pc/usa/demo/` | 37,132 | 2,765 |

## Tools

Cross-version analysis is handled by the binary mining pipeline:

```sh
python tools/binary_mining_pipeline.py cross-lineage   # PAL GC, PS2, Xbox string similarity
python tools/binary_mining_pipeline.py pc-versions     # PC retail/demo shared strings
```

Generated outputs:
- `build/cross_binary_lineage/lineage_report.md`
- `build/cross_binary_lineage/string_similarity.tsv`
- `build/pc_analysis/pc_analysis.txt`
- `build/pc_analysis/pc_symbol_proposals.txt`

## PAL GameCube Retail

The PAL `GL5P4F` DOL is very close to the US `GL5E4F` DOL:

- `.text` is 0x60 bytes larger than US.
- `.rodata`, `.data`, `.sdata`, and `.sdata2` are shifted by 0x60 to 0x80 bytes in address space after the text delta.
- The only notable PAL-only key string is `hngc_obj/../config.c`.

Use PAL primarily for regional delta checks and confirming target strings are stable across retail GameCube builds. **Do not copy PAL addresses into US symbols.**

## LSW2 PS2 Prototype

The LSW2 PS2 prototype is a strong near-lineage reference — newer than LSW1, but preserving a large set of Nu2/game source paths and debug/assert strings.

### Useful Source-Path Evidence

- `../nu2api/nucore/nufile.c`, `numem.c`
- `../nu2api/nu3d/nuanim.c`, `nugscn.c`, `nurndr.c`
- `../nu2api/nufmv/ps2/shellmpeg/shellmpeg.c`
- `../nu2api/nups2/ps2ctl.c`
- `../nu2api/nusound3/ps2/nustream.c`
- `../gameapi/gamelib/audio.c`, `terrain.c`
- `../gameapi/gui/gamemenu/apisave.c`

### Symbol Work Completed

The LSW2 prototype overlap confirmed 5 direct string-label renames:

- `str_NuDatFileOpen_NoFreeHandles`
- `str_NuDatOpenEx_ReadErrorRetrying`
- `str_NuDatOpenEx_SeekErrorRetrying`
- `str_ScriptCmd_DERIVEFROMSCRIPT`
- `str_instNuGCutSceneFind_FailedToFind`

## TCS Wii Prototype

Extracted with Dolphin into `DATA` and `UPDATE` partitions. The useful partition is `orig/tcs-wii-prototype/DATA/sys/main.dol` and `orig/tcs-wii-prototype/DATA/files/`.

**Disc metadata:** Game ID `RLGP64`, internal name `LEGO Star Wars`, version `1.00`.

The TCS DOL is much larger than LSW1/LSW2 executables and includes Wii SDK, NW4R Home Button, and later Nu2 code. It is later lineage, but useful for shared-string confirmation and data naming.

### Useful Shared Code Strings

- `NuDebugMsg - %s(%d) :`, `NuError trapped - %s`
- `NuFileReadDir not implemented.....`, `NuFileCreatePath not implemented.....`
- `NuSpecialFindByPlatformID wont work with new format`
- `NuDatFileOpen - No free dat handles`
- `NuDatOpenEx - recoverable read error, retrying`
- `DERIVEFROMSCRIPT`, `instNuGCutSceneFind: Failed to find <%s>`

### Useful TCS-Only Strings

- `DynamicCameraCut`, `EndCameraCut`, `NOTRIGGER`
- `NuDatFileOpen - File %s is zero length`
- `Loading into an occupied stringbank! Call NuStringTableUnload() first.`
- `NuGCutSceneLoadAddr - streaming cutscenes not supported`
- `NuGCutSceneLoadAddr - only supports new stlye cutscenes`
- `NuMemFileOpen failed for script.txt`

### TCS DATA Files

The loose DATA tree is valuable for content and script structure:

- `Levels/levels.txt` documents level records with `dir`, `file`, `test_level`, `intro_level`, `cutscene_level`, `outro_level`, `status_level`, `newgame_level`, `loadgame_level`.
- `Levels/areas.txt` documents area records with flags such as `single_buffer`, `minikit`, `test_area`, `vehicle_area`, `bonus_area`, `hub_area`, `override_things_scene`, `nopickupgravity`, `timetrial_time`, `redbrick_cheat`, `ending_area`, `no_gold_brick`, `no_completion_points`.
- `Levels/missions.txt` lists bounty hunter mission declarations.
- `Scripts/SCRIPT.TXT` lists AI script names such as `Attack`, `Jedi`, `MissionTarget`, `patrol`, `Snake`.

### TCS Symbol Work Completed

- `str_NuDebugMsg_Format`

## PC Retail and Demo Analysis

Both executables are stripped PE32 Windows binaries, but they retain source file breadcrumbs, assert strings, subsystem messages, and data path strings useful for the GameCube target.

### Symbol Work Completed

56 GameCube string symbols were renamed from PC retail/demo overlap:

- **Nu file I/O:** `str_NuFileOpen_SizeSeekFailedRetrying`, `str_NuFileLoadBuffer_Trace`, `str_NuFileBeginBlkRead_BlockHeaderMismatch`, `str_NuDatFileLoadBuffer_LsnReadingPacked`, `str_NuDatFileOpen_FileNotFound`
- **Packed-file loading:** `str_NuPPLoadBuffer1_SeekFailureRetrying`, `str_NuPPLoadBuffer2_ReadFailureRetrying`, `str_NuPPLoadBuffer4_ReadFailureRetrying`
- **Animation asserts:** `str_NuAnimLoad_FailedOpenFile`, `str_NuAnimDataLoadBuff_NewFormatExpectedOld`, `str_NuAnimCurve2CalcVal_BooleanAlreadyEvaluated`
- **AI/script system:** `str_AIScriptBufferAlloc_BufferTooSmall`, `str_AISysBufferAlloc_LevelBufferTooSmall`, `str_AIPathNodeDistanceToPathNode_SpecialRouteProblem`, `str_ScriptCmd_REFERENCESCRIPT`, `str_ScriptCmd_PARAM`
- **Cutscene fix-up:** `str_NuGCutLocatorSysFixUp_CannotFixupLocator`, `str_NuGCutRigidSysFixUp_CannotFixupRigid`, `str_NuGCutTriggerSysFixUp_TriggerNotFound`, `str_instNuGCutCamSysUpdate_InternalError`, `str_instNuCGutRigidSysCreate_CannotReferenceRigid`

### Demo-Specific Findings

- Source breadcrumbs: `.\NUAPI.cpp`, `.\NUAPIEffects.cpp`, `.\NuConsole.cpp`, `.\XboxStuff.cpp`, `.\pc\nudlistpc.c`, `.\pc\nuerror.cpp`, `.\d3dAudio.cpp`
- PC shader source paths: `c:\projects\nu2api\nuwin32\Shaders\LEGOSkinBlend.fx`, `LEGOTC1.fx`, `SharedFunctions.fx`
- Extra PC sound/debug messages: `NuError trapped - %s`, `NuSoundPauseStream`, `NuSoundResumeStream`, `NuSoundSetStreamPitch`, `NuSoundSetStreamVolume`

### Retail-Specific Findings

- Source breadcrumbs: `.\ai\aisys\aisys.c`, `.\gamelib\pc\debris_pc.cpp`, `.\gui\gamemenu\apisave.c`, `.\pc\datsys.cpp`, `.\pc\nusound.cpp`, `.\rtlif.c`
- Runtime/game strings: `AI_ObiWan`, `Unable to open datfile %s`, `NuHeap: Out of memory`, `NuAlloced:%d`

## PS2 Prototype Deep Analysis

The PS2 prototype provides valuable subsystem structure and naming conventions.

### Subsystem Architecture

#### Nu2 Engine (nu2api/)
- **Core (nucore/):** `NuFile` (file I/O), `NuMem` (memory), `NuString` (string handling)
- **3D (nu3d/):** `NuAnim` (animation), `NuCamera`, `NuRndr` (rendering), `NuGScn` (scenes), `NuGHG` (game objects)
- **Cutscene (gamelib/):** `gcutscn.c` - `instNuGCutSceneFind`, `instNuGCutCamSysUpdate`, locator/rigid/trigger fixup
- **Sound (nusound3/):** `NuSoundSetRequestTable`, `NuSoundStream`
- **Math (numath/):** `NuGraph`, `NuPlane`, `NuTrig`

#### Game API (gameapi/)
- **AI (ai/aisys/):** `aiscript.c` (script system), `aisys.c` (AI core), `edaipath.c` (path editor)
- **Camera (camera/socksys/):** `socksys.c` - `CameraCut`, `ResetGameCamera`, `CameraShake`
- **Script (gamelib/):** `script.c` - `SETTRIGGERFLAG`, `RESETTRIGGER`, `PLAYCUTSCENEINST`, `CHAINCUTSCENEINST`, `REFERENCESCRIPT`, `ENDSCRIPT`, and more
- **Menu (gui/gamemenu/):** `apisave.c`, debug menu: `LEGO Options`, `Go To Scene`, `Go To Level`
- **Characters (characters/apiobject/):** `apiobject.c` - OverrideAnimation, xRefScript
- **Game library (gamelib/):** `audio.c`, `debris.c`, `edbits.c`, `edgra.c`, `edprelit.c`, `terrain.c`

### PS2 Symbol Work Completed

- `str_CameraCmd_CameraCut` - Camera cut command string
- `instNuGCutSceneFind` - Cutscene find function string
- `NuAnimLoad` - Animation load function string
- `NuAnimDataLoadBuff` - Animation data load function string

## Cautions

- **PAL GameCube** is the same game but a different region; use for confirmation and deltas, not address transfer.
- **LSW2 PS2** is close lineage, not the same game; use for names, source-tree layout, and subsystem evidence.
- **TCS Wii** is later lineage; use for shared-string confirmation, source/data naming, and script grammar, not direct function identity.
- **PC builds** are x86 and platform-specific. Use for names, source-tree layout, and subsystem evidence, not for function boundary assumptions.
