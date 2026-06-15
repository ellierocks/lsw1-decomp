# Archaeology: `pc_us_demo` vs `pc_us_retail`

**demo  2005**  →  **retail  2005-04**

## Executable

| | `pc_us_demo` | `pc_us_retail` |
|-|---------|---------|
| Size | 10,248,192 bytes | 11,362,304 bytes |
| Delta | | +1,114,112 bytes |

## String Overview

| Category | `pc_us_demo` | `pc_us_retail` | Only in A | Only in B |
|----------|---------|---------|-----------|-----------|
| all | 12712 | 14148 | 2303 | 3739 |
| source_paths | 120 | 115 | 35 | 30 |
| nu_debug | 143 | 141 | 7 | 5 |
| assert_error | 577 | 577 | 24 | 24 |

## Source File Changes

**Added source paths:**
- `.\ai\aisys\aisys.c`
- `.\bgprocxbx.c`
- `.\credits.c`
- `.\gamelib\pc\debris_pc.cpp`
- `.\gui\gamemenu\apisave.c`
- `.\nuheap.c`
- `.\numem.c`
- `.\pc\datsys.cpp`
- `.\pc\nusound.cpp`
- `.\rtlif.c`
- `<.\nuanim.c`
- `=333?..\numath\pc\VuMath.h`
- `=c:\projects\nu2api.legopc\nuwin32\d3dinput.cpp`
- `?.\gui\gamemenu\apimenu.c`
- `?.\pc\nucamera.c`
- `?.\turret.c`
- `?C.\main.c`
- `G.\chris.c`
- `L?..\numath\pc\VuMath.h`
- `c:\projects\nu2api.legopc\nuwin32\d3dapicalls.cpp`
- `c:\projects\nu2api.legopc\nuwin32\d3dcalls.cpp`
- `c:\projects\nu2api.legopc\nuwin32\d3dcore.cpp`
- `c:\projects\nu2api.legopc\nuwin32\nuapi.cpp`
- `c:\projects\nu2api.legopc\nuwin32\nuapieffects.cpp`
- `c:\projects\nu2api.legopc\nuwin32\nuapishader.cpp`
- `c:\projects\nu2api.legopc\nuwin32\nuconsole.cpp`
- `c:\projects\nu2api.legopc\nuwin32\nuwavloader.cpp`
- `c:\projects\nu2api.legopc\nuwin32\pcapi.cpp`
- `c:\projects\nu2api.legopc\nuwin32\xboxstuff.cpp`
- `~?.\podrace.c`

**Removed source paths:**
- `../drm_xml_wrapper/inc\drm_data_access_log.h`
- `./inc\drm_logging.h`
- `./obj/encrypted_decrypt_vmtree.cpp`
- `./obj/encrypted_drm_data_access.cpp`
- `./obj\encrypted_drm_eval_sort.cpp`
- `./obj\encrypted_drm_user_data.cpp`
- `./src/drm_alloc_block.cpp`
- `.\NUAPI.cpp`
- `.\NUAPIEffects.cpp`
- `.\NuConsole.cpp`
- `.\XboxStuff.cpp`
- `.\bgproc.c`
- `.\chris.c`
- `.\d3dApiCalls.cpp`
- `.\d3dAudio.cpp`
- `.\d3dCalls.cpp`
- `.\d3dCore.cpp`
- `.\d3dInput.cpp`
- `.\main.c`
- `.\nudlist.c`
- `.\pc\nudlistpc.c`
- `.\pc\nuerror.cpp`
- `.\pcapi.cpp`
- `.\turret.c`
- `:..\numath\pc\VuMath.h`
- `;E..\numath\pc\VuMath.h`
- `;G.\pc\nusound.cpp`
- `=..\numath\pc\VuMath.h`
- `>.\rtlif.c`
- `?.\credits.c`
- `?.\numem.c`
- `?.\thrusters.c`
- `B..\numath\pc\VuMath.h`
- `aD.\ai\aisys\aisys.c`
- `pA..\numath\pc\VuMath.h`

## Nu Engine Debug String Changes

**Added:**
- `NULL instance`
- `NuAlloced:%d`
- `NuDatFileLoadBuffer "%s" - Unpacking`
- `NuDatFileOpen - No free dat handles`
- `NuHeap: Out of memory`

**Removed:**
- `NuError trapped - %s`
- `NuMtlAddRndrItem: Exceeded maximum number of water items in render queue!`
- `NuSoundPauseStream will not work for other cases`
- `NuSoundResumeStream will not work for other cases`
- `NuSoundSetStreamPitch will not work for other cases`
- `NuSoundSetStreamVolume will not work for other cases`
- `NudxFw_GetBackBufferCopyTID function is broken`

## Assert / Error Message Changes

**Added:**
- `AddCreature: unable to allocator gamepad`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegOutro_R`
- `CannotDropIn`
- `Checksum failed`
- `Error: Failed to receive current User-SID.`
- `External memory alloc of %d bytes failed at: %s (%d).`
- `Failed to Overwrite`
- `Failed to create duplicate buffer`
- `Failed to create shader <fxtype><%i>`
- `L<Window Creation Failed!`
- `NuHeap: Out of memory`
- `Please insert a memory card (PS2) with at least %iKB free into MEMORY CARD slot 1 or you will be unable to Save.`
- `Sorry, but your machine is unable to run this game.`
- `USER DATA ERROR (Patch_700_TO_701): Failed to get OLD user home path.`
- `Unable to open datfile %s`
- `cannotbigjump`
- `failed to get ASPI function addresses`
- `failed to load WNASPI32.DLL`
- `failed to send command to ASPI device`
- `sound failed to load: %s`
- `streaming failed: %s`

**Removed:**
- `ASSERT trapped - %s`
- `Audio\_CutScenes\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\Ep1_FailedNegOutro_R`
- `BeginScene failed`
- `Cannot render a display scene twice in a frame.`
- `CreateDepthStencilSurface failed`
- `CreateRenderTarget failed`
- `D3DXCreateEffectFromFileA: File failed to open`
- `Failed create user data directory.`
- `Please insert a memory card (PS2) with at least %iKB free or you will be unable to Save.`
- `SetDepthStencilSurface failed`
- `SetRenderTarget failed`
- `SetViewport failed`
- `Sound ID<%i> failed to load. No sound data path specified`
- `Sound ID<%i> failed to load. No sound filename specified`
- `Window Creation Failed!`
- `failed to allocate memory for section headers`
- `failed to allocate memory for sections`
- `failed to locate import directory`
- `failed to retrieve exports`
- `failed to retrieve imports`
- `memextern failed, contact someone`