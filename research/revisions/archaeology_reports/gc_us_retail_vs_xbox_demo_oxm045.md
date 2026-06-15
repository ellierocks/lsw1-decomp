# Archaeology: `gc_us_retail` vs `xbox_demo_oxm045`

**retail GL5E4F 2005-04**  →  **demo  2005-06**

## Executable

| | `gc_us_retail` | `xbox_demo_oxm045` |
|-|---------|---------|
| Size | 2,085,664 bytes | 2,125,824 bytes |
| Delta | | +40,160 bytes |

## String Overview

| Category | `gc_us_retail` | `xbox_demo_oxm045` | Only in A | Only in B |
|----------|---------|---------|-----------|-----------|
| all | 4683 | 3689 | 1841 | 847 |
| source_paths | 98 | 19 | 98 | 19 |
| nu_debug | 95 | 8 | 88 | 1 |
| assert_error | 138 | 52 | 114 | 28 |

## Source File Changes

**Added source paths:**
- `>M\projects\gameapi\gamelib\terrain.c`
- `Fps2dma.c`
- `Ga>nu3d\xbox\nutex.c`
- `Hnu3d\xbox\nugobj.c`
- `\projects\gameapi\edtools\edui.c`
- `\projects\gameapi\gui\gamemenu\apisave.c`
- `\projects\nu2api\nusound3\xbox\nusound.cpp`
- ``Bnu3d\xbox\nudgmngr.c`
- `nu3d\nuanim.c`
- `nu3d\nutexanm.c`
- `nu3d\xbox\nucamera.c`
- `nu3d\xbox\nufontx.cpp`
- `nu3d\xbox\nuhgobj.c`
- `nu3d\xbox\numtl.c`
- `nu3d\xbox\nurndr.c`
- `nu3d\xbox\nuscene.c`
- `nu3d\xbox\nuxscn1.cpp`
- `xbox\glutils.c`
- `xbox\listman.c`

**Removed source paths:**
- `../gameapi.legogc/ai/aisys/ngc_obj/../aiscript.c`
- `../gameapi.legogc/ai/aisys/ngc_obj/../aisys.c`
- `../gameapi.legogc/ai/aisys/ngc_obj/../edaipath.c`
- `../gameapi.legogc/camera/socksys/ngc_obj/../socksys.c`
- `../gameapi.legogc/characters/apiobject/ngc_obj/../apiobject.c`
- `../gameapi.legogc/effects/objects/ngc_obj/../part.c`
- `../gameapi.legogc/gamelib/ngc_obj/../audio.c`
- `../gameapi.legogc/gamelib/ngc_obj/../debris.c`
- `../gameapi.legogc/gamelib/ngc_obj/../edbits.c`
- `../gameapi.legogc/gamelib/ngc_obj/../edgra.c`
- `../gameapi.legogc/gamelib/ngc_obj/../terrain.c`
- `../gameapi.legogc/gui/gamemenu/ngc_obj/../apimenu.c`
- `../gameapi.legogc/gui/gamemenu/ngc_obj/../apisave.c`
- `../gameapi.legogc/rtl/ngc_obj/../rtl.c`
- `../nu2api.legogc/gamelib/ngc/ngc_obj/../randtab.c`
- `../nu2api.legogc/gamelib/ngc/ngc_obj/../saveload.c`
- `../nu2api.legogc/gamelib/ngc/ngc_obj/../specterr.c`
- `../nu2api.legogc/gamelib/ngc_obj/../gcutscn.c`
- `../nu2api.legogc/gamelib/ngc_obj/../glutils.c`
- `../nu2api.legogc/gamelib/ngc_obj/../listman.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../ngc_err.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../ngc_mtl.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../ngc_video.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nucamera.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nudlistngc.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nufont_ngc.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nugobj.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nugscn.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nuhgobj.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../numtl.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nuqfnt_ngc.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nurndr.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nuscene.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nustream.c`
- `../nu2api.legogc/nu3d/ngc/ngc_obj/../nutex.c`
- `../nu2api.legogc/nu3d/ngc_obj/../NuHGobjEval.c`
- `../nu2api.legogc/nu3d/ngc_obj/../Nuanimbuffer.c`
- `../nu2api.legogc/nu3d/ngc_obj/../nuanim.c`
- `../nu2api.legogc/nu3d/ngc_obj/../nuanimv3.c`
- `../nu2api.legogc/nu3d/ngc_obj/../nudlist.c`

## Nu Engine Debug String Changes

**Added:**
- `NULL row buffer`

**Removed:**
- `NuAllocHigh : request for 0 bytes of memory denied`
- `NuAllocHighInit : Already initialised`
- `NuAlloced:%d`
- `NuAnimCurve2CalcVal: should have already evaluated NUANIMKEYTYPE_BOOLEAN`
- `NuAnimCurve2CalcVal: should have already evaluated NUANIMKEYTYPE_NONE`
- `NuAnimDataLoadBuff: Is in new format, expecting old format.`
- `NuAnimDataLoadBuff: Is in old format, expecting new format.`
- `NuAnimLoad : Failed to open animation file %s!`
- `NuDatFileLoadBuffer "%s" - Unpacking`
- `NuDatFileLoadBuffer "%s"- LSN Buffer Overrun - falling back to HFS`
- `NuDatFileLoadBuffer - LSN Reading Packed file %s (%d->%d)`
- `NuDatFileLoadBuffer - LSN Reading file %s (%d)`
- `NuDatFileLoadBuffer - not 64 byte aligned`
- `NuDatFileLoadBuffer - recoverable read error, retrying`
- `NuDatFileLoadBuffer - recoverable seek error, retrying`
- `NuDatFileOpen - File %s not found in datfile`
- `NuDatFileOpen - No free dat handles`
- `NuDatFileOpen - opened %s`
- `NuDatFileOpen - recoverable seek error - retrying`
- `NuDatOpenEx - recoverable read error, retrying`
- `NuDatOpenEx - recoverable seek error, retrying`
- `NuDebugMsg - %s(%d) :`
- `NuError - %s Line %d : `
- `NuError trapped - %s`
- `NuFParResume - failed to reopen file!`
- `NuFileBeginBlkRead : Block header mismatch!`
- `NuFileBeginBlkWrite : writing block... (id %c%c%c%c)`
- `NuFileClose - close failed, retrying`
- `NuFileCreatePath not implemented.....`
- `NuFileEndBlkWrite : ...close block (id %c%c%c%c, size %d)`
- `NuFileLoadBuffer "%s" to %08x size %08x`
- `NuFileLoadBuffer - recoverable read failure - retrying`
- `NuFileLoadBuffer - recoverable seek failure - retrying`
- `NuFileOpen - size seek failed - retrying`
- `NuFileOpen - size seek restore failed - retrying`
- `NuFilePos - seek failed %d`
- `NuFilePos - seek failed %d - retrying`
- `NuFileReadDir not implemented.....`
- `NuFileSetAddress:- Need to expand MAX_FILE_ADDRESS`
- `NuFileWriteAddress:- Need to expand MAX_FILE_ADDRESS`
- `NuFileendBlockRead : read past end of block`
- `NuFmvNgc_Alloc: Buffer not big enough`
- `NuGCutLocatorSysFixUp: cannot fixup locator <%s> in cutscene`
- `NuGCutRigidSysFixUp: cannot fixup rigid object <%s> in cutscene`
- `NuGCutTriggerSysFixUp: Unable to find trigger <%s>`
- `NuGHGReadEx - buffer overflow by %d bytes`
- `NuGScnBufferAllocAligned failed, scene buffer overrun. `
- `NuGobjCalcDims : Lock VB failed!`
- `NuHeap: Out of memory`
- `NuHexStringToI: Invalid character`
- `NuInitHardware : Insufficient buffer space`
- `NuMemAlloc : Failed to alloc %d bytes!`
- `NuMtlSetCurrentRenderPlane out of range`
- `NuPPLoadBuffer1 - recoverable seek failure, retrying`
- `NuPPLoadBuffer2 - recoverable read failure, retrying`
- `NuPPLoadBuffer3 - recoverable seek failure, retrying`
- `NuPPLoadBuffer4 - recoverable read failure, retrying (err%d)`
- `NuPPLoadBuffer5 - recoverable seek failure, retrying`
- `NuPPLoadBuffer6 - recoverable read failure, retrying`
- `NuRndrItem : Unknown render item type!`

## Assert / Error Message Changes

**Added:**
- `- This application cannot run using the active version of the Microsoft .NET Runtime`
- `- unable to initialize heap`
- `- unable to open console device`
- `Audio\_CutScenes\5_1_Xbox\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\5_1_Xbox\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\5_1_Xbox\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\5_1_Xbox\Ep1_FailedNegOutro_R`
- `CCIR601 sampling not implemented yet`
- `Cannot quantize more than %d color components`
- `Cannot quantize to fewer than %d colors`
- `Cannot quantize to more than %d colors`
- `Cannot transcode due to multiple use of quantization table %d`
- `Failed to create temporary file %s`
- `Fractional sampling not implemented yet`
- `GetDiskFreeSpaceEx failed with %08x`
- `Not implemented yet`
- `Out of Memory`
- `Please insert a memory card (PS2) with at least %iKB free into MEMORY CARD slot 1 or you will be unable to Save.`
- `Read failed on temporary file`
- `Read from EMS failed`
- `Read from XMS failed`
- `Seek failed on temporary file`
- `Unable to re-open movie cache file, status=%08x`
- `Write failed on temporary file --- out of disk space?`
- `Write to EMS failed`
- `Write to XMS failed`
- `corrupted the program's internal state.  The program cannot safely`
- `internal state.  The program cannot safely continue execution and must`

**Removed:**
- `!!!!!!!!!!!!!!!!!!! opening file %s failed!!!!!!!!!!!!!!!!!!!!1`
- `AI Save FAILED`
- `ASSERT trapped - %s`
- `AddCreature: unable to allocator gamepad`
- `Alloc failed %d requested at : %s (%d)`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegOutro_R`
- `Buffer Overflow`
- `Buffer overflow - please save at least %i bytes.`
- `CUT_FindCharacters: cannot fixup cutscene character <%s>`
- `Cannot have prelight sets on characters (you fool).`
- `Cannot render a display scene twice in a frame.`
- `Cannot resume a fileparser that was not created with NuFParCreate`
- `Cannot resume a fileparser that was not created with NuFParCreate - therefore, cannot suspend it`
- `Checksum failed`
- `Display list buffer overflow.`
- `Error: Failed to open directory %s`
- `Error: Failed to open file %s`
- `Error: Failed to read %d bytes from file`
- `External memory alloc of %d bytes failed at: %s (%d).`
- `Failed to Create`
- `Failed to Save`
- `Failed to create movie mtl %s`
- `Failed to open scene file %s!`
- `Func not implemented`
- `Function not implemented.`
- `KaminoEReset: cannot find door %s`
- `KaminoEReset: cannot find turret %s`
- `NGCAUDIO -> Failed to acquire voice, sound will be ignored`
- `Not implemented.`
- `NuAnimLoad : Failed to open animation file %s!`
- `NuFParResume - failed to reopen file!`
- `NuFileClose - close failed, retrying`
- `NuFileCreatePath not implemented.....`
- `NuFileOpen - size seek failed - retrying`
- `NuFileOpen - size seek restore failed - retrying`
- `NuFilePos - seek failed %d`
- `NuFilePos - seek failed %d - retrying`