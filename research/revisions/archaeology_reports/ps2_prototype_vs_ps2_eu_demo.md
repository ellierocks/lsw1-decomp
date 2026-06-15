# Archaeology: `ps2_prototype` vs `ps2_eu_demo`

**prototype  2005-01-10**  →  **demo  2005**

## Executable

| | `ps2_prototype` | `ps2_eu_demo` |
|-|---------|---------|
| Size | 2,941,736 bytes | 3,173,684 bytes |
| Delta | | +231,948 bytes |

## String Overview

| Category | `ps2_prototype` | `ps2_eu_demo` | Only in A | Only in B |
|----------|---------|---------|-----------|-----------|
| all | 4049 | 4735 | 404 | 1090 |
| source_paths | 87 | 90 | 76 | 79 |
| nu_debug | 115 | 120 | 1 | 6 |
| assert_error | 116 | 125 | 5 | 14 |

## Source File Changes

**Added source paths:**
- `../gameapi.legops2/ai/aisys/aiscript.c`
- `../gameapi.legops2/ai/aisys/aisys.c`
- `../gameapi.legops2/ai/aisys/edaipath.c`
- `../gameapi.legops2/camera/socksys/socksys.c`
- `../gameapi.legops2/characters/apiobject/apiobject.c`
- `../gameapi.legops2/edtools/edui.c`
- `../gameapi.legops2/gamelib/audio.c`
- `../gameapi.legops2/gamelib/debris.c`
- `../gameapi.legops2/gamelib/edbits.c`
- `../gameapi.legops2/gamelib/edgra.c`
- `../gameapi.legops2/gamelib/edprelit.c`
- `../gameapi.legops2/gamelib/terrain.c`
- `../gameapi.legops2/gui/gamemenu/apimenu.c`
- `../gameapi.legops2/gui/gamemenu/apisave.c`
- `../gameapi.legops2/rtl/rtl.c`
- `../nu2api.legops2/gamelib/gcutscn.c`
- `../nu2api.legops2/gamelib/glutils.c`
- `../nu2api.legops2/gamelib/listman.c`
- `../nu2api.legops2/gamelib/ps2/specterr.c`
- `../nu2api.legops2/gamelib/script.c`
- `../nu2api.legops2/gamelib/trigger.c`
- `../nu2api.legops2/nu3d/nuanim.c`
- `../nu2api.legops2/nu3d/nudlist.c`
- `../nu2api.legops2/nu3d/nuspecial.c`
- `../nu2api.legops2/nu3d/nuspline.c`
- `../nu2api.legops2/nu3d/nutexanm.c`
- `../nu2api.legops2/nu3d/nutimebar.c`
- `../nu2api.legops2/nu3d/ps2/bitconv.c`
- `../nu2api.legops2/nu3d/ps2/nucamera.c`
- `../nu2api.legops2/nu3d/ps2/nudlistPS2.c`
- `../nu2api.legops2/nu3d/ps2/nufont.c`
- `../nu2api.legops2/nu3d/ps2/nufpatch.c`
- `../nu2api.legops2/nu3d/ps2/nugobj.c`
- `../nu2api.legops2/nu3d/ps2/nugscn.c`
- `../nu2api.legops2/nu3d/ps2/nuhgobj.c`
- `../nu2api.legops2/nu3d/ps2/numtl.c`
- `../nu2api.legops2/nu3d/ps2/nunrmmap.c`
- `../nu2api.legops2/nu3d/ps2/nuportal.c`
- `../nu2api.legops2/nu3d/ps2/nuprelit.c`
- `../nu2api.legops2/nu3d/ps2/nurndr.c`

**Removed source paths:**
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
- `../nu2api/nu3d/ps2/nurndr2d.c`

## Nu Engine Debug String Changes

**Added:**
- `NULL instance`
- `NUSND3G.IRX`
- `NuDatFileLoadBuffer "%s" - Unpacking`
- `NuDatFileOpen - No free dat handles`
- `NuHeap: Out of memory`
- `NuSound compatible streaming requires a NuSound data stream`

**Removed:**
- `NUSND3F.IRX`

## Assert / Error Message Changes

**Added:**
- `AddCreature: unable to allocator gamepad`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\PL2_PS2\Ep1_FailedNegOutro_R`
- `CannotDropIn`
- `Checksum failed`
- `Display list buffer overflow.`
- `External memory alloc of %d bytes failed at: %s (%d).`
- `Failed to Overwrite`
- `Failed to alloc IOP mem`
- `NuHeap: Out of memory`
- `Please insert a memory card (PS2) with at least %iKB free into MEMORY CARD slot 1 or you will be unable to Save.`
- `cannotbigjump`

**Removed:**
- `Audio\_CutScenes\Ep1_FailedNegIntro_L`
- `Audio\_CutScenes\Ep1_FailedNegIntro_R`
- `Audio\_CutScenes\Ep1_FailedNegOutro_L`
- `Audio\_CutScenes\Ep1_FailedNegOutro_R`
- `Please insert a memory card (PS2) with at least %iKB free or you will be unable to Save.`