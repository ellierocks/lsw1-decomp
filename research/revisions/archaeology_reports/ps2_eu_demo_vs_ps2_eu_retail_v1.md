# Archaeology: `ps2_eu_demo` vs `ps2_eu_retail_v1`

**demo  2005**  →  **retail 1.0 2005-04**

## Executable

| | `ps2_eu_demo` | `ps2_eu_retail_v1` |
|-|---------|---------|
| Size | 3,173,684 bytes | 3,170,488 bytes |
| Delta | | -3,196 bytes |

## String Overview

| Category | `ps2_eu_demo` | `ps2_eu_retail_v1` | Only in A | Only in B |
|----------|---------|---------|-----------|-----------|
| all | 4735 | 4716 | 224 | 205 |
| source_paths | 90 | 90 | 78 | 78 |
| nu_debug | 120 | 120 | 0 | 0 |
| assert_error | 125 | 125 | 0 | 0 |

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