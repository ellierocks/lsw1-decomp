# Symbol Recovery Leads

Source file paths and subsystem debug strings found in each build.
Cross-platform string matches help link GC addresses to named functions.

## Source Path Coverage by Build

| Build | Source Paths | Nu Debug Strings |
|-------|-------------|-----------------|
| `gc_uk_retail` | 112 | 75 |
| `gc_us_retail` | 113 | 75 |
| `mac_demo_dbg` | 74 | 58 |
| `pc_us_demo` | 122 | 75 |
| `pc_us_retail` | 119 | 73 |
| `ps2_eu_demo` | 94 | 84 |
| `ps2_eu_retail_v1` | 94 | 84 |
| `ps2_prototype` | 91 | 80 |
| `ps2_us_retail_gh` | 95 | 85 |
| `ps2_us_retail_v1` | 94 | 84 |
| `xbox_demo_oxm045` | 19 | 0 |
| `xbox_retail` | 19 | 0 |

## Platform-Unique Source Paths

Source paths found in non-GC builds but not in the GC DOL — may indicate platform-specific modules.

### `gc_uk_retail` (1 unique)
- `hngc_obj/../config.c`

### `mac_demo_dbg` (74 unique)
- `@|D-.H`
- `@|E1.H`
- `NUAPI.cpp`
- `NUAPIEffects.cpp`
- `NUAPIShader.cpp`
- `NuConsole.cpp`
- `XboxStuff.cpp`
- `aiscript.c`
- `aisys.c`
- `apimenu.c`
- `apiobject.c`
- `apisave.c`
- `audio.c`
- `bgproc.c`
- `chris.c`
- `config.c`
- `creature.c`
- `credits.c`
- `d3dApiCalls.cpp`
- `d3dCore.cpp`
- `d3dInput.cpp`
- `datsys.cpp`
- `debris.c`
- `debris_pc.cpp`
- `force.c`
- `game_obj.c`
- `gameai.c`
- `gameaiscript.c`
- `gcutscn.c`
- `glutils.c`

### `pc_us_demo` (122 unique)
- `../drm_xml_wrapper/inc\drm_data_access_log.h`
- `..\numath\pc\VuMath.h`
- `./inc\drm_logging.h`
- `./obj/encrypted_decrypt_vmtree.cpp`
- `./obj/encrypted_drm_data_access.cpp`
- `./obj\encrypted_drm_eval_sort.cpp`
- `./obj\encrypted_drm_user_data.cpp`
- `./src/drm_alloc_block.cpp`
- `./src/hook_tables.cpp`
- `.\NUAPI.cpp`
- `.\NUAPIEffects.cpp`
- `.\NuConsole.cpp`
- `.\XboxStuff.cpp`
- `.\ai\aisys\aiscript.c`
- `.\bgproc.c`
- `.\camera\socksys\socksys.c`
- `.\characters\apiobject\apiobject.c`
- `.\chris.c`
- `.\config.c`
- `.\creature.c`
- `.\cut.c`
- `.\d3dApiCalls.cpp`
- `.\d3dAudio.cpp`
- `.\d3dCalls.cpp`
- `.\d3dCore.cpp`
- `.\d3dInput.cpp`
- `.\edtools\edui.c`
- `.\effects\objects\part.c`
- `.\force.c`
- `.\gameai.c`

### `pc_us_retail` (119 unique)
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
- `.\cut.c`
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

### `ps2_eu_demo` (94 unique)
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

### `ps2_eu_retail_v1` (94 unique)
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

### `ps2_prototype` (91 unique)
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

### `ps2_us_retail_gh` (95 unique)
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

### `ps2_us_retail_v1` (94 unique)
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

### `xbox_demo_oxm045` (19 unique)
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

### `xbox_retail` (19 unique)
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

## Xbox Demo Script Commands

Script function names and commands from the Xbox demo `.scp` files.
These may correspond to game script dispatch table entries.

Total unique command/function names: **0**

| Script File | Commands |
|-------------|----------|
| `Scripts/Attack.scp` |  |
| `Scripts/CommanderPatrol.scp` |  |
| `Scripts/DisguisedClone.scp` |  |
| `Scripts/DoggedAttack.scp` |  |
| `Scripts/Droid.scp` |  |
| `Scripts/FreePlay.scp` |  |
| `Scripts/FreePlayDefend.scp` |  |
| `Scripts/GenericDefend.scp` |  |
| `Scripts/Jedi.scp` |  |
| `Scripts/NewSniper.scp` |  |
| `Scripts/NonJedi.scp` |  |
| `Scripts/Sniper.scp` |  |
| `Scripts/Storm.scp` |  |
| `Scripts/Wander.scp` |  |
| `Scripts/backups/Copy of NonJedi.scp` |  |
| `Scripts/backups/NonJedi.scp` |  |
| `Scripts/backups/OLDCopy of NonJedi.scp` |  |
| `Scripts/backups/Sniper.scp` |  |
| `Scripts/block.scp` |  |
| `Scripts/children.scp` |  |
| `Scripts/default.scp` |  |
| `Scripts/defend.scp` |  |
| `Scripts/patrol.scp` |  |
| `Scripts/test.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_Outro/AI/patrol.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/Copy of Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/Deactivated.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/TC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/VentDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/doorAttack.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/Copy of Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/Deactivated.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/TC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/doorAttack.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_1/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/Deactivated.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/TC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/VentDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/doorAttack.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_2/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/Deactivated.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/TC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/VentDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/doorAttack.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_3/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/Deactivated.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/TC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/VentDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/doorAttack.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI Backups/AI_4/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/Copy of Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/Copy of level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/Deactivated.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/Dynamic PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/LEVJedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/LEVTC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/Level_Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/NOT USED Dynamic PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/NOTUSEDLevel_Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/TC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/VentDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/doorAttack.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_a/AI/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI Backups/AI/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI Backups/AI/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI Backups/AI/lazydroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI Backups/AI/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI Backups/AI_1/lazydroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI Backups/AI_1/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/Dynamic PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/LEVJedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/LEVTC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/PKDroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/lazydroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_b/AI/level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI/Level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI/droideka.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI/mtt_droid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI/pkdroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_1/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_1/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_1/Level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_1/droideka.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_1/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_1/mtt_droid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_1/pkdroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_2/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_2/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_2/Level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_2/droideka.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_2/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_2/mtt_droid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI Backups/AI_2/pkdroid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/Freeplay.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/Jedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/LEVJedi.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/LEVTC14.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/LEVpatrol.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/Level.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/droideka.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/level1.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/mtt_droid.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/patrol.scp` |  |
| `levels/episode_i/chapter_01/Negotiations_c/AI/pkdroid.scp` |  |