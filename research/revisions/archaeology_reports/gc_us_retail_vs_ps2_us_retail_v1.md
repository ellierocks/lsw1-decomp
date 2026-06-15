# Archaeology: `gc_us_retail` vs `ps2_us_retail_v1`

**retail GL5E4F 2005-04**  →  **retail 1.01 2005-04**

## Executable

| | `gc_us_retail` | `ps2_us_retail_v1` |
|-|---------|---------|
| Size | 2,085,664 bytes | 3,170,744 bytes |
| Delta | | +1,085,080 bytes |

## String Overview

| Category | `gc_us_retail` | `ps2_us_retail_v1` | Only in A | Only in B |
|----------|---------|---------|-----------|-----------|
| all | 4683 | 4730 | 1225 | 1272 |
| source_paths | 98 | 90 | 98 | 90 |
| nu_debug | 95 | 120 | 14 | 39 |
| assert_error | 138 | 125 | 67 | 54 |

## Source File Changes

**Added source paths:**
- `../gameapi/ai/aisys/aiscript.c`
- `../gameapi/ai/aisys/aisys.c`
- `../gameapi/ai/aisys/edaipath.c`
- `../gameapi/camera/socksys/socksys.c`
- `../gameapi/characters/apiobject/apiobject.c`
- `../gameapi/edtools/edui.c`
- `../gameapi/gamelib/audio.c`
- `../gameapi/gamelib/debris.c`
- `../gameapi/gamelib/edbits.c`
- `../gameapi/gamelib/edgra.c`
- `../gameapi/gamelib/edprelit.c`
- `../gameapi/gamelib/terrain.c`
- `../gameapi/gui/gamemenu/apimenu.c`
- `../gameapi/gui/gamemenu/apisave.c`
- `../gameapi/rtl/rtl.c`
- `../nu2api/gamelib/gcutscn.c`
- `../nu2api/gamelib/glutils.c`
- `../nu2api/gamelib/listman.c`
- `../nu2api/gamelib/ps2/specterr.c`
- `../nu2api/gamelib/script.c`
- `../nu2api/gamelib/trigger.c`
- `../nu2api/nu3d/nuanim.c`
- `../nu2api/nu3d/nudlist.c`
- `../nu2api/nu3d/nuspecial.c`
- `../nu2api/nu3d/nuspline.c`
- `../nu2api/nu3d/nutexanm.c`
- `../nu2api/nu3d/nutimebar.c`
- `../nu2api/nu3d/ps2/bitconv.c`
- `../nu2api/nu3d/ps2/nucamera.c`
- `../nu2api/nu3d/ps2/nudlistPS2.c`
- `../nu2api/nu3d/ps2/nufont.c`
- `../nu2api/nu3d/ps2/nufpatch.c`
- `../nu2api/nu3d/ps2/nugobj.c`
- `../nu2api/nu3d/ps2/nugscn.c`
- `../nu2api/nu3d/ps2/nuhgobj.c`
- `../nu2api/nu3d/ps2/numtl.c`
- `../nu2api/nu3d/ps2/nunrmmap.c`
- `../nu2api/nu3d/ps2/nuportal.c`
- `../nu2api/nu3d/ps2/nuprelit.c`
- `../nu2api/nu3d/ps2/nurndr.c`

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
- `NUCOMMAND_PLAYCUTSCENEINST %s backward`
- `NUCOMMAND_PLAYCUTSCENEINST %s forward`
- `NUMTL_ALPHAADD`
- `NUMTL_ALPHADARKEN`
- `NUMTL_ALPHALIGHTEN`
- `NUMTL_ALPHAMOD`
- `NUMTL_ALPHAMODTXT`
- `NUMTL_ALPHAOFF`
- `NUMTL_ALPHASCALE`
- `NUMTL_ALPHASUB`
- `NURNDRSTREAM_ATTR_FX_MTX`
- `NURNDRSTREAM_ATTR_RT_LIGHTS`
- `NURNDRSTREAM_ATTR_SPEC_LIGHTS`
- `NURNDRSTREAM_ATTR_TRACK_MTX`
- `NUSND3G.IRX`
- `NuAssert - %s(%d) : assert(%s) failed!`
- `NuError - %s(%d) : `
- `NuFileCloseDir - close failed!`
- `NuFileOpenDir - failed to open dir %s`
- `NuFileReadDir - error reading directory (%d)`
- `NuFmvMtlCreate - failed to create texture`
- `NuFmvMtlCreate - failed to initialise MPEG system`
- `NuFmvMtlCreate - failed to reserve sufficient texture memory`
- `NuFmvPlay - unable to reserve %d bytes of VRAM`
- `NuFntWrite Could not allocate buffer`
- `NuMcCloseDir - invalid handle`
- `NuMcOpenDir - failed to open dir %s`
- `NuMcReadDir - invalid handle`
- `NuMtlInsert - Renderplane out of range`
- `NuNormalMapClose called without Initialisation`
- `NuRndrBurstObjAdd without NuRndrBurstObjBegin`
- `NuSound compatible streaming requires a NuSound data stream`
- `NuSoundStream - opening %s`
- `NuSpecialGetOriginRadius: New system does not work with this yet, see Alistair.`
- `NuTexLoadBitmap:Bad BitCount <%d> on loading bitmap <%s>`
- `NuTexLoadBitmap:Bad BitCount <%d> on loading memory bitmap`
- `NuTexRead does not accept file extensions - these are implied by the platform.`
- `NuWarning - %s(%d) : `
- `null.ico`

**Removed:**
- `NuAlloced:%d`
- `NuDebugMsg - %s(%d) :`
- `NuError - %s Line %d : `
- `NuFileCreatePath not implemented.....`
- `NuFileReadDir not implemented.....`
- `NuFmvNgc_Alloc: Buffer not big enough`
- `NuRndrItem : Unknown render item type!`
- `NuScene no longer supported.`
- `NuSoundInitV called`
- `NuSoundInitV: Multiple initialisations`
- `NuSoundSetRequestTable called`
- `NuSpecialFindByPlatformID wont work with new format`
- `NuTexRead cant find file %s.`
- `NuWarning - %s Line %d : `

## Assert / Error Message Changes

**Added:**
- `"%" is not a .mib file and therefore cannot be played with NuSoundPlayInterleavedStereo`
- `Assert in "%s" at line %d`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegOutro_R`
- `Binding the IOP module failed`
- `Buffer overflow`
- `Cannot Draw 2D Prims using a 3D material`
- `Cannot allocate SuperBuffer!`
- `Cannot end avi, no avi file currently open`
- `Cannot open new avi, already writing an AVI file`
- `Cannot set renderplane %d after itself`
- `Cannot set renderplane %d before itself`
- `Cannot write to a NUSOUND file`
- `Command FAILED`
- `DeleteThread FAILED`
- `Failed to alloc IOP mem`
- `Failed to allocated IOP memory`
- `Failed to create thread`
- `Failed to open %s`
- `NuAssert - %s(%d) : assert(%s) failed!`
- `NuFileCloseDir - close failed!`
- `NuFileOpenDir - failed to open dir %s`
- `NuFmvMtlCreate - failed to create texture`
- `NuFmvMtlCreate - failed to initialise MPEG system`
- `NuFmvMtlCreate - failed to reserve sufficient texture memory`
- `NuFmvPlay - unable to reserve %d bytes of VRAM`
- `NuMcOpenDir - failed to open dir %s`
- `Please insert a memory card (PS2) with at least %iKB free into MEMORY CARD slot 1 or you will be unable to Save.`
- `Prelight editor allocation failed.`
- `Recovery buffer overflow`
- `Stream buffer overflow - crash soon`
- `StreamInit - failed to find %s in %s`
- `StreamInit - failed to open %s`
- `StreamOpen - failed to find %s in %s`
- `StreamOpen(nosound) - failed to find %s in %s`
- `StreamOpen(nosound) - failed to open %s`
- `TermainateThread FAILED`
- `Transfer to IOP Failed`

**Removed:**
- `Audio\_CutScenes\GameCube\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegOutro_R`
- `Cannot have prelight sets on characters (you fool).`
- `Error: Failed to open directory %s`
- `Error: Failed to open file %s`
- `Error: Failed to read %d bytes from file`
- `Failed to Create`
- `Failed to create movie mtl %s`
- `Failed to open scene file %s!`
- `Func not implemented`
- `Function not implemented.`
- `NGCAUDIO -> Failed to acquire voice, sound will be ignored`
- `Not implemented.`
- `NuFileCreatePath not implemented.....`
- `NuFileReadDir not implemented.....`
- `OSCheckActiveThreads: Failed !IsSuspended(thread->suspend) in %d`
- `OSCheckActiveThreads: Failed !__OSCheckDeadLock(thread) in %d`
- `OSCheckActiveThreads: Failed *(thread->stackEnd) == OS_THREAD_STACK_MAGIC in %d`
- `OSCheckActiveThreads: Failed 0 <= thread->suspend in %d`
- `OSCheckActiveThreads: Failed CheckThreadQueue(&RunQueue[prio]) in %d`
- `OSCheckActiveThreads: Failed CheckThreadQueue(&thread->queueJoin) in %d`
- `OSCheckActiveThreads: Failed CheckThreadQueue(thread->queue) in %d`
- `OSCheckActiveThreads: Failed IsMember(&RunQueue[thread->priority], thread) in %d`
- `OSCheckActiveThreads: Failed IsMember(thread->queue, thread) in %d`
- `OSCheckActiveThreads: Failed OS_PRIORITY_MIN <= thread->priority && thread->priority <= OS_PRIORITY_MAX+1 in %d`
- `OSCheckActiveThreads: Failed RunQueue[prio].head != NULL && RunQueue[prio].tail != NULL in %d`
- `OSCheckActiveThreads: Failed RunQueue[prio].head == NULL && RunQueue[prio].tail == NULL in %d`
- `OSCheckActiveThreads: Failed __OSActiveThreadQueue.head == NULL || __OSActiveThreadQueue.head->linkActive.prev == NULL in %d`
- `OSCheckActiveThreads: Failed __OSActiveThreadQueue.tail == NULL || __OSActiveThreadQueue.tail->linkActive.next == NULL in %d`
- `OSCheckActiveThreads: Failed __OSCheckMutexes(thread) in %d`
- `OSCheckActiveThreads: Failed thread->linkActive.next == NULL || thread == thread->linkActive.next->linkActive.prev in %d`
- `OSCheckActiveThreads: Failed thread->linkActive.prev == NULL || thread == thread->linkActive.prev->linkActive.next in %d`
- `OSCheckActiveThreads: Failed thread->priority == 32 in %d`
- `OSCheckActiveThreads: Failed thread->priority == __OSGetEffectivePriority(thread) in %d`
- `OSCheckActiveThreads: Failed thread->queue != NULL in %d`
- `OSCheckActiveThreads: Failed thread->queue == &RunQueue[thread->priority] in %d`
- `OSCheckActiveThreads: Failed thread->queue == NULL in %d`
- `OSCheckActiveThreads: Failed thread->queueMutex.head == NULL && thread->queueMutex.tail == NULL in %d`