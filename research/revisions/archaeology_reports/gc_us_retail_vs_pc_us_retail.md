# Archaeology: `gc_us_retail` vs `pc_us_retail`

**retail GL5E4F 2005-04**  →  **retail  2005-04**

## Executable

| | `gc_us_retail` | `pc_us_retail` |
|-|---------|---------|
| Size | 2,085,664 bytes | 11,362,304 bytes |
| Delta | | +9,276,640 bytes |

## String Overview

| Category | `gc_us_retail` | `pc_us_retail` | Only in A | Only in B |
|----------|---------|---------|-----------|-----------|
| all | 4683 | 14148 | 1598 | 11063 |
| source_paths | 98 | 115 | 98 | 115 |
| nu_debug | 95 | 141 | 40 | 86 |
| assert_error | 138 | 577 | 82 | 521 |

## Source File Changes

**Added source paths:**
- `..\numath\pc\VuMath.h`
- `./src/hook_tables.cpp`
- `.\ai\aisys\aiscript.c`
- `.\ai\aisys\aisys.c`
- `.\bgprocxbx.c`
- `.\camera\socksys\socksys.c`
- `.\characters\apiobject\apiobject.c`
- `.\config.c`
- `.\creature.c`
- `.\credits.c`
- `.\edtools\edui.c`
- `.\effects\objects\part.c`
- `.\force.c`
- `.\gameai.c`
- `.\gameaiscript.c`
- `.\gamelib\audio.c`
- `.\gamelib\pc\debris_pc.cpp`
- `.\gamelib\terrain.c`
- `.\gcutscn.c`
- `.\glutils.c`
- `.\gui\gamemenu\apisave.c`
- `.\levels.c`
- `.\listman.c`
- `.\nuanim.c`
- `.\nudir.c`
- `.\nufile.c`
- `.\nufilepak.c`
- `.\nufpar.c`
- `.\nuheap.c`
- `.\numem.c`
- `.\nuplane.c`
- `.\nurndr.cpp`
- `.\nuspecial.c`
- `.\nuspline.c`
- `.\nustring.c`
- `.\nutexanm.c`
- `.\nutimebar.c`
- `.\obstacle.c`
- `.\pc\Bink\dsplay_bink.cpp`
- `.\pc\datsys.cpp`

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
- `NUD3DWARNING: %s, DirectDescription: %s, %s`
- `NUDEBUG: %s`
- `NUERROR: %s`
- `NULL != ((CNodeExpression *)pArrayExpr)->m_pLeft`
- `NULL != *ppTailAssignmentInfo`
- `NULL != m_pSimpleDecl`
- `NULL != m_pTailSetCall`
- `NULL != pAnnotation`
- `NULL != pArrayExpr->m_pCar && D3DXShader::D3DXNT_VALUE == pArrayExpr->m_pCar->m_Type && NULL == pArrayExpr->m_pCdr`
- `NULL != pAssignment->m_pCode`
- `NULL != pAssignment->m_pConstantTable`
- `NULL != pBlockAssignment->m_pCode`
- `NULL != pExpression->m_pLeft && NULL != pExpression->m_pRight`
- `NULL != pExpression->m_pLeft->m_pCar && D3DXShader::D3DXNT_VALUE == pExpression->m_pLeft->m_pCar->m_Type`
- `NULL != pFunction`
- `NULL != pHandle->m_pValueList && NULL != pHandle->m_pValueList->m_pCar && D3DXShader::D3DXNT_VALUE == pHandle->m_pValueList->m_pCar->m_Type`
- `NULL != pLValues[iState].m_szName`
- `NULL != pMatrix`
- `NULL != pNode->m_pCar && D3DXShader::D3DXNT_VALUE == pNode->m_pCar->m_Type`
- `NULL != pParameter`
- `NULL != pPass`
- `NULL != pPreProcessor`
- `NULL != pRValue`
- `NULL != pRef->m_pFunction`
- `NULL != pRef->m_pPixelShader`
- `NULL != pRef->m_pVertexShader`
- `NULL != pTechnique`
- `NULL != pTechnique->m_ppSamplerStateBlocks[PS]`
- `NULL != pTechnique->m_ppSamplerStateBlocks[VS]`
- `NULL != pTechnique->m_ppShaderStateBlocks[PS]`
- `NULL != pTechnique->m_ppShaderStateBlocks[VS]`
- `NULL != pTrueDevice`
- `NULL != pValueList->m_pCar && D3DXShader::D3DXNT_VALUE == pValueList->m_pCar->m_Type`
- `NULL != pVector`
- `NULL != pcRegisters`
- `NULL != pf && NULL != pValueList->m_pCar && D3DXShader::D3DXNT_VALUE == pValueList->m_pCar->m_Type`
- `NULL != piRegistersMax`
- `NULL != piRegistersMin`
- `NULL != ppStateManager`
- `NULL != ppTailAssignmentInfo`
- `NULL != ppdwFunction`
- `NULL != szId`
- `NULL == m_pTrueDevice`
- `NULL == pAssignment->m_pArguments`
- `NULL == pAssignment->m_pCode`
- `NULL == pValueList`
- `NULL row buffer`
- `NUMTL_FX_WATER_REFRACTION water case not handled`
- `NUMTL_FX_WATER_SPECULAR water case not handled`
- `NUMTL_FX_WATER_UV_MAPPED_ONLY water case not handled`
- `NUWARNING: %s`
- `NuFileCloseDir - close failed!`
- `NuFileOpenDir - failed to open dir %s`
- `NuGeomUnlockVB : Failed to lock vertex buffer!`
- `NuRndrBurstObjAdd without NuRndrBurstObjBegin`
- `NuRndrBurstObjEndNoClip : No free geom item slots!`
- `NuRndrBurstObjEndNoClip : No free matrix slots!`
- `NuRndrGobj : No free geom item slots!`
- `NuRndrGobj : No free matrix slots!`
- `NuRndrGobjSkin : No free geom item slots!`

**Removed:**
- `NuAllocHigh : request for 0 bytes of memory denied`
- `NuAllocHighInit : Already initialised`
- `NuDatOpenEx - recoverable read error, retrying`
- `NuDatOpenEx - recoverable seek error, retrying`
- `NuDebugMsg - %s(%d) :`
- `NuError - %s Line %d : `
- `NuError trapped - %s`
- `NuFParResume - failed to reopen file!`
- `NuFileBeginBlkWrite : writing block... (id %c%c%c%c)`
- `NuFileCreatePath not implemented.....`
- `NuFileEndBlkWrite : ...close block (id %c%c%c%c, size %d)`
- `NuFileReadDir not implemented.....`
- `NuFileSetAddress:- Need to expand MAX_FILE_ADDRESS`
- `NuFileWriteAddress:- Need to expand MAX_FILE_ADDRESS`
- `NuFmvNgc_Alloc: Buffer not big enough`
- `NuGobjCalcDims : Lock VB failed!`
- `NuInitHardware : Insufficient buffer space`
- `NuMemAlloc : Failed to alloc %d bytes!`
- `NuScene no longer supported.`
- `NuSoundInitV called`
- `NuSoundInitV: Multiple initialisations`
- `NuSoundSetRequestTable - tablesize x 16 exceeds max samples passed to NuSoundInitV`
- `NuSoundSetRequestTable called`
- `NuSpecialFindByPlatformID wont work with new format`
- `NuStringTableLoad() - Error: %s not a UNICODE file.`
- `NuStringTableLoad() allocated strings %dk`
- `NuStringTableLoad() allocated tables %dk`
- `NuStringTableSaveCharacterList: ERROR - over 512 characters used`
- `NuTexRead cant find file %s.`
- `NuWarning - %s Line %d : `
- `instNuGCutSceneFind: Failed to find <%s>`
- `instNuGCutSceneSetRepeat: <%d> exceeds max num of repeats <32>`
- `nugraph: Cannot add point, less than 2 existing!`
- `nugraph: Cannot add point, one here already.`
- `nugraph: Cannot add point, too many already`
- `nugraphGenerateLookupTable: tx<x1`
- `nugraphGenerateLookupTable: value > 1.0`
- `nugraphGenerateLookupTable: x2<tx`
- `nugraphGetYatX(): Couldn't find point %.02f in graph`
- `numeminit is empty and someone just used it`

## Assert / Error Message Changes

**Added:**
- `!This program cannot be run in DOS mode.`
- `%s failed to stop.`
- `'%s': %s cannot be declared 'const'`
- `'%s': %s cannot be declared 'extern'`
- `'%s': %s cannot be declared 'inline'`
- `'%s': %s cannot be declared 'shared'`
- `'%s': %s cannot be declared 'static'`
- `'%s': %s cannot be declared 'uniform out'`
- `'%s': %s cannot be declared 'uniform'`
- `'%s': %s cannot be declared 'volatile'`
- `'%s': %s cannot be target specific`
- `'%s': %s cannot have %ss`
- `'%s': %s cannot have annotations`
- `'%s': %s cannot have semantics`
- `'%s': cannot %sconvert from '%s' to '%s'`
- `'%s': extern %s cannot be declared 'static'`
- `'%s': functions cannot be declared 'extern'`
- `'%s': functions cannot be declared 'uniform'`
- `'%s': non-numeric uniform %s cannot have %ss`
- `'%s': output only %s cannot have %ss`
- `'%s': output paramaters cannot be declared 'const'`
- `'%s': uniform %s cannot be declared 'static'`
- `'%s': void function cannot have a semantic`
- `'%s': void functions cannot return a value`
- `- This application cannot run using the active version of the Microsoft .NET Runtime`
- `- unable to initialize heap`
- `- unable to open console device`
- `@internal error: InputRemap component matchup failed`
- `A NULL pointer was passed as a parameter`
- `A mapper file function failed because reading or writing the user or IHV settings file failed. & A run-time error occurred.`
- `A required object is not initialized or failed to initialize.`
- `A script routine written in AudioVBScript failed because a function outside of a script failed to complete. For example, a call to PlaySegment that fails to play because of low memory would return this error.`
- `A script routine written in AudioVBScript failed because an invalid operation occurred.  For example, adding the number 3 to a segment object would produce this error.  So would attempting to call a routine that doesn't exist.`
- `ActiveMovie cannot play MPEG movies on this processor.`
- `ActiveMovie cannot play this video stream because it falls outside the constrained standard.`
- `An attempt to add a filter with a duplicate name failed.`
- `An attempt to use this object failed because it first needs to be loaded.`
- `An attempt was made to modify parameters of an effect while it is playing.  Not all hardware devices support altering the parameters of an effect while it is playing. & Two pins of the same direction cannot be connected together.`
- `An audiopath failed to create because a requested buffer could not be created.`
- `An operation failed due to a certification failure.`

**Removed:**
- `AI Save FAILED`
- `ASSERT trapped - %s`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\GameCube\Ep1_FailedNegOutro_R`
- `Buffer Overflow`
- `Cannot have prelight sets on characters (you fool).`
- `Cannot render a display scene twice in a frame.`
- `Cannot resume a fileparser that was not created with NuFParCreate`
- `Cannot resume a fileparser that was not created with NuFParCreate - therefore, cannot suspend it`
- `Display list buffer overflow.`
- `Error: Failed to open directory %s`
- `Error: Failed to open file %s`
- `Error: Failed to read %d bytes from file`
- `Failed to Create`
- `Failed to create movie mtl %s`
- `Func not implemented`
- `Function not implemented.`
- `NGCAUDIO -> Failed to acquire voice, sound will be ignored`
- `Not implemented.`
- `NuFParResume - failed to reopen file!`
- `NuFileCreatePath not implemented.....`
- `NuFileReadDir not implemented.....`
- `NuGobjCalcDims : Lock VB failed!`
- `NuMemAlloc : Failed to alloc %d bytes!`
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