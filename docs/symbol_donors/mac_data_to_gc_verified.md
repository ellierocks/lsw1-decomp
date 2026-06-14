# Mac Data -> GC lbl_* Verified Candidates (with GC xref evidence)

Each GC lbl_* object was scanned for code references. The functions
that reference it provide subsystem context. Mac candidates are scored
by size match + section match + xref function subsystem agreement.

**Confidence:**
- **HIGH** (>=15): exact size + section + xref functions match Mac subsystem
- **MEDIUM** (8-14): exact/close size + some xref context
- **LOW** (4-7): size hint + weak xref context
- **REJECT** (<4): size collision, no useful xrefs, or conflicting

---

## HIGH Confidence

| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | Score | Xref Funcs | Xref Types | Evidence |
|------------|--------|-----------|-------|--------|-------|------------|------------|----------|
| `NuTimeBar_DefaultColours` | ~64 | lbl_801B0F08 | 0x40 | .data | 27 | fn_800262BC, fn_800644E4, fn_8005F254, fn_8000F65C | addr | size+sec; xsub:Nu2; word:Nu; word:Nu |
| `DoorExitCameraSplineName` | ~32 | lbl_80313268 | 0x20 | .bss | 26 | NuCameraGetViewMtx, fn_80043328, fn_8006B328, NuAnimCurveSet | addr | size+sec; xsub:Camera; word:Camera; word:Camera |
| `GameMenuLevel` | ~4 | lbl_801D68E0 | 0x4 | .data | 25 | fn_8009E620, fn_8011D78C, fn_800BA87C, fn_8010CCA0 | addr | size+sec; xsub:Menu; word:Menu; word:Menu |
| `LevInstAnim` | ~16 | lbl_803145D0 | 0x10 | .bss | 22 | fn_8001F804, NuAnimKeyLerp, fn_8001EFA4, NuAnimKeyRead | addr | size+sec; xsub:Anim; word:Anim; word:Anim |
| `kFirstTimeKey` | ~4 | lbl_80190A3C | 0x4 | .rodata | 18 | NuAnimKeyBlend | addr | size+sec; xsub:Input; word:Key |
| `NuFade_Enabled` | ~8 | lbl_80190BF8 | 0x8 | .rodata | 18 | NuAnimData2Apply | addr | size+sec; xsub:Nu2; word:Nu |
| `NuRndrWaterLevel` | ~4 | lbl_80190C4C | 0x4 | .rodata | 18 | NuAnimData2CalcMatrix | addr | size+sec; xsub:Nu2; word:Nu |
| `Actions_AnakinA` | ~80 | lbl_801A006C | 0x50 | .rodata | 18 | Action_MoveAwayFromOpponent, fn_800984DC, Action_FacePlayer, | addr | size+sec; xsub:Script; xfuncs:10 |
| `TexAnimList` | ~96 | lbl_801B15D8 | 0x60 | .data | 18 | NuAnimCurveSetEval | addr | size+sec; xsub:Anim; word:Anim |
| `NuRndrDebRange` | ~12 | lbl_80313250 | 0xC | .bss | 18 | NuAnimCurveSetEval, fn_800265C4 | addr | size+sec; xsub:Nu2; word:Nu |
| `NuRndrWaterTint` | ~4 | lbl_80337924 | 0x4 | .bss | 17 | NuAnimCurve2SetApplyBlendToJoint2 | addr | size+sec; xsub:Nu2; word:Nu |
| `global_Light2Position` | ~12 | lbl_803E04C4 | 0xC | .bss | 17 | fn_800A4E68, Action_CreateSplineCreatures, fn_8012C1C8, Acti | addr | size+sec; word:Position; word:Position; xfuncs:13 |
| `cfgtab_Script` | ~32 | lbl_801D63C4 | 0x20 | .data | 15 | fn_80135CBC, Action_CycleCharacter | addr | size+sec; xsub:Script |

---
## MEDIUM Confidence

| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | Score | Xref Funcs | Xref Types | Evidence |
|------------|--------|-----------|-------|--------|-------|------------|------------|----------|
| `Area_RetakeTheedPalaceLevelList` | ~24 | lbl_801A000C | 0x18 | .rodata | 14 | fn_8008C768, fn_80070690, fn_800D28A0, fn_800AD4C4 | addr | size+sec; xfuncs:9 |
| `Area_InvasionOfNabooModelList_FreePlay` | ~16 | lbl_801B0650 | 0x10 | .data | 14 | fn_8011D78C, NuMemAlloc, NuMemFree | addr | size+sec; word:Free; xfuncs:3 |
| `BoulderRadius` | ~4 | lbl_801D63A4 | 0x4 | .data | 14 | Action_BoulderSection, fn_800BA74C, fn_800CC284, fn_800BA87C | addr | size+sec; word:Boulder; xfuncs:4 |
| `pBackBufferViewport` | ~24 | lbl_80313298 | 0x18 | .bss | 14 | fn_8001D7EC, fn_800455A4, fn_800CA018, fn_8002406C | addr | size+sec; xfuncs:6 |
| `LevForceBlownUp` | ~48 | lbl_80340404 | 0x30 | .bss | 14 | fn_8002DE44, fn_80037A4C, fn_8002C178, fn_8002C090 | addr | size+sec; xfuncs:6 |
| `STAPAnim` | ~192 | lbl_80191200 | 0xC0 | .rodata | 13 | fn_8002452C, fn_800247FC, fn_80024704, fn_80024854 | addr | size+sec; xfuncs:4 |
| `ZTV13CMemoryBuffer` | ~56 | lbl_80191930 | 0x38 | .rodata | 13 | fn_80029EC8, fn_800282E8, fn_80028444, fn_800286B4 | addr | size+sec; xfuncs:6 |
| `Area_GunshipCavalryLevelList` | ~8 | lbl_80193B70 | 0x8 | .rodata | 13 | fn_80041A74, fn_80041914, fn_800417CC | addr | size+sec; xfuncs:3 |
| `Area_JediBattleLevelList` | ~12 | lbl_801A0024 | 0xC | .rodata | 13 | fn_8006BBA8, fn_800AFDC0, fn_8006B328 | addr | size+sec; xfuncs:3 |
| `global_Light0Color` | ~16 | lbl_801A2F4C | 0x10 | .rodata | 13 | fn_800C6FB8, fn_800C78EC, fn_800C749C | addr | size+sec; xfuncs:3 |
| `JarJarBinksAnim` | ~768 | lbl_801D53C8 | 0x300 | .data | 13 | fn_800AC340, fn_800AC384, fn_800AAF44, fn_80149420 | addr | size+sec; xfuncs:12 |
| `CutSceneConfigList` | ~300 | lbl_801D6648 | 0x12C | .data | 13 | fn_800CE080, fn_800C3EEC, fn_800CE11C | addr | size+sec; xfuncs:3 |
| `Area_EscapeFromNabooLevelList` | ~20 | lbl_801E15E8 | 0x14 | .data | 13 | fn_8013DF98, fn_8013CA3C, fn_8013E4DC, fn_8013CBAC | addr | size+sec; xfuncs:9 |
| `CutCamMtx` | ~64 | lbl_802D5D54 | 0x40 | .bss | 13 | fn_8009D23C, fn_8009D000, fn_8009D320, fn_8009D0AC | addr | size+sec; xfuncs:5 |
| `ParticleChunkRenderStack` | ~20 | lbl_802E0B9C | 0x14 | .bss | 13 | fn_800CE408, fn_800C4758, fn_800CE43C | addr | size+sec; xfuncs:3 |
| `NuRndrHackWaterLevel` | ~8 | lbl_802E0BBC | 0x8 | .bss | 13 | fn_800CEF44, fn_800C62A0, fn_800C6430, fn_800CEEF0 | addr | size+sec; xfuncs:5 |
| `LevForce` | ~128 | lbl_802F46B0 | 0x80 | .bss | 13 | fn_80165938, fn_80165C70, fn_801666E0, fn_801667B4 | addr | size+sec; xfuncs:9 |
| `ZN16CKeyboardDevice89sKeyQueueE` | ~40 | lbl_802F4A50 | 0x28 | .bss | 13 | fn_8016BE88, fn_8016BA70, fn_8016BCBC, fn_8016BAFC | addr | size+sec; xfuncs:9 |
| `ObjectAnim` | ~256 | lbl_80312D50 | 0x100 | .bss | 13 | fn_80014EC0, fn_8001619C, fn_80014274 | addr | size+sec; xfuncs:3 |
| `tinfo` | ~144000 | lbl_803145E0 | 0x23280 | .bss | 13 | fn_8002DE44, NuAnimCurve2SetApplyToJointTransLoc, NuAnimCurv | addr | size+sec; xfuncs:4 |
| `sGlobalMouse` | ~4 | lbl_803404D4 | 0x4 | .bss | 13 | fn_800356FC, fn_8002DE44, fn_80035470, fn_80035A24 | addr | size+sec; xfuncs:10 |
| `g_revertSoundInfo` | ~51200 | lbl_803B6E04 | 0xC800 | .bss | 13 | fn_8006BA54, fn_8006BBA8, fn_8006C748, fn_8006DDC4 | addr | size+sec; xfuncs:12 |
| `GlobalSfxBits` | ~100 | lbl_803D45B8 | 0x64 | .bss | 13 | fn_8006DBE8, fn_8006C748, fn_8006DA58, fn_8006DBB8 | addr | size+sec; xfuncs:6 |
| `gDontFinish` | ~1 | lbl_803E0298 | 0x1 | .bss | 13 | fn_800A53DC, Menu_Unknown4_Enter, fn_80147E3C, fn_800C8E38 | addr | size+sec; xfuncs:53 |
| `ZTV21CDirect3DVertexShader` | ~76 | lbl_80190074 | 0x4C | .rodata | 12 | NuAnimCurveDestroy, NuAnimDataCreate, NuAnimDataLoad, NuAnim | addr | size+sec; xfuncs:8 |
| `ViewCam` | ~40 | lbl_801907FC | 0x28 | .rodata | 12 | fn_80037FEC | addr | size+sec |
| `DogFightSpeedList` | ~72 | lbl_801918E8 | 0x48 | .rodata | 12 | fn_80028B88, fn_8002A3A8, fn_80029EC8, fn_800282E8 | addr | size+sec; xfuncs:8 |
| `gensorttab` | ~52 | lbl_8019416C | 0x34 | .rodata | 12 | fn_80045F70, fn_800457D4, fn_80045E40 | addr | size+sec; xfuncs:3 |
| `gunshipb_pulse` | ~180 | lbl_801D6774 | 0xB4 | .data | 12 | fn_800CE144, fn_800C42A0, fn_800C3FB8 | addr | size+sec; xfuncs:3 |
| `unicodename` | ~68 | lbl_801F5A28 | 0x44 | .data | 12 | fn_80165C70, fn_80165974, fn_801662C0, fn_80165918 | addr | size+sec; xfuncs:4 |
| `ZTV15CDirect3DDevice` | ~500 | lbl_801F6748 | 0x1F4 | .data | 12 | fn_8016C1BC, fn_8016C468, fn_8016CDE0 | addr | size+sec; xfuncs:3 |
| `file_info` | ~1320 | lbl_80200080 | 0x528 | .bss | 12 | NuDatFileLoadBuffer, NuFileClose, NuFileLoadBuffer, NuFileSe | addr | size+sec; xfuncs:10 |
| `txt` | ~1024 | lbl_802055AC | 0x400 | .bss | 12 | fn_8000965C, fn_80009858, fn_80009748, fn_80009564 | addr | size+sec; xfuncs:4 |
| `boulder_oldpos` | ~36 | lbl_80220CB0 | 0x24 | .bss | 12 | fn_80030E3C, fn_80032958, fn_80032CB4 | addr | size+sec; xfuncs:3 |
| `SubShelfPos` | ~88 | lbl_802E15F4 | 0x58 | .bss | 12 | fn_800E1850, fn_800E1D0C, fn_800E1564 | addr | size+sec; xfuncs:3 |
| `repsfxtab` | ~896 | lbl_802E18F4 | 0x380 | .bss | 12 | fn_800E6338, fn_800E6394, fn_800E346C | addr | size+sec; xfuncs:3 |
| `sfxcomplist` | ~1536 | lbl_802E1D1C | 0x600 | .bss | 12 | fn_8010439C, fn_801031E0, fn_8010438C | addr | size+sec; xfuncs:3 |
| `sAppBundleRef` | ~80 | lbl_802F3B50 | 0x50 | .bss | 12 | fn_8015E744, fn_80162AA8, fn_8015E95C, fn_80160BF4 | addr | size+sec; xfuncs:4 |
| `UpdateGroupTab` | ~192 | lbl_802F45F0 | 0xC0 | .bss | 12 | fn_80164FB0, fn_80164AFC, fn_8016470C, fn_801645E0 | addr | size+sec; xfuncs:20 |
| `CurrentSFXTAB` | ~104 | lbl_802F49E8 | 0x68 | .bss | 12 | fn_8016B364, fn_8016B084, fn_8016B568, fn_8016B044 | addr | size+sec; xfuncs:6 |
| `vtx$11527` | ~144 | lbl_802F4CD0 | 0x90 | .bss | 12 | fn_801724BC, fn_80172408, fn_801725BC | addr | size+sec; xfuncs:3 |
| `datfiles` | ~640 | lbl_80312250 | 0x280 | .bss | 12 | fn_80006010, NuDatFileLoadBuffer, fn_80005E80, fn_80005FF4 | addr | size+sec; xfuncs:8 |
| `fstackmem` | ~4096 | lbl_80347074 | 0x1000 | .bss | 12 | fn_80043050, fn_80043328, fn_80043128 | addr | size+sec; xfuncs:3 |
| `g_groups` | ~800 | lbl_803C3604 | 0x320 | .bss | 12 | fn_8006E37C, fn_8006C54C, fn_8006E394, fn_8006B328 | addr | size+sec; xfuncs:4 |
| `configdata` | ~512 | lbl_803C3924 | 0x200 | .bss | 12 | fn_8006C54C, fn_8006E394, fn_8006B328 | addr | size+sec; xfuncs:3 |
| `vader_c` | ~152 | lbl_803D5680 | 0x98 | .bss | 12 | fn_80074DDC, fn_80075760, fn_80109550, fn_800746F8 | addr | size+sec; xfuncs:11 |
| `kamino_e` | ~120 | lbl_80402404 | 0x78 | .bss | 12 | fn_8014A040, fn_80148CC4, fn_801474EC | addr | size+sec; xfuncs:3 |
| `HintItems` | ~9216 | lbl_80404BB0 | 0x2400 | .bss | 12 | fn_8014ABAC, fn_801474EC, fn_8014BBE0, fn_8014B978 | addr | size+sec; xfuncs:5 |
| `ExtraCurPos` | ~84 | lbl_80406FCC | 0x54 | .bss | 12 | fn_801486FC, fn_8014ABAC, fn_801474EC | addr | size+sec; xfuncs:3 |
| `AnakinsPodAnim` | ~256 | lbl_8019167C | 0x100 | .rodata | 11 | fn_80027FAC | addr | size+sec |
| `Area_MosEspaPodRaceLevelList` | ~28 | lbl_801917B4 | 0x1C | .rodata | 11 | fn_80028138 | addr | size+sec |
| `SebulbasPodAnim` | ~128 | lbl_80192290 | 0x80 | .rodata | 11 | fn_80033F30, fn_80032CB4 | addr | size+sec |
| `ZTV13CMouseDevice8` | ~140 | lbl_80194330 | 0x8C | .rodata | 11 | fn_80045E40 | addr | size+sec |
| `RoyalGuardAnim` | ~1024 | lbl_801B0F8C | 0x400 | .data | 11 | fn_800316D4 | addr | size+sec |
| `Area_MosEspaPodRaceModelList_Story` | ~48 | lbl_801B1C74 | 0x30 | .data | 11 | fn_8002E53C | addr | size+sec |
| `sEnableSwap` | ~60 | lbl_801E1614 | 0x3C | .data | 11 | fn_8013D9FC | addr | size+sec |
| `SaucerAnim` | ~384 | lbl_801E3278 | 0x180 | .data | 11 | fn_8015F5A4 | addr | size+sec |
| `lightfan` | ~108 | lbl_80337860 | 0x6C | .bss | 11 | NuAnimCurve2SetApplyToJointTransLoc, fn_80022DE8 | addr | size+sec |
| `GameMenu` | ~2240 | lbl_8040191C | 0x8C0 | .bss | 11 | fn_8014728C | addr | size+sec |
| `mvt` | ~112 | lbl_801913EC | 0x70 | .rodata | 10 | fn_80024DC8 | addr | size+sec |
| `jedib_offsets` | ~36 | lbl_8019145C | 0x24 | .rodata | 10 | fn_80025A70 | addr | size+sec |
| `SplTab` | ~108 | lbl_801914A0 | 0x6C | .rodata | 10 | fn_80027378, fn_80027D54 | addr | size+sec |
| `ZTV9CDirect3D` | ~88 | lbl_80191C4C | 0x58 | .rodata | 10 | fn_8002B978 | addr | size+sec |
| `MENUFLASH1B` | ~3 | lbl_80191D7C | 0x3 | .rodata | 10 | fn_8002C460 | addr | size+sec |
| `PntScaleTab$6887` | ~100 | lbl_801930E4 | 0x64 | .rodata | 10 | fn_8003A244 | addr | size+sec |
| `CInfoTab` | ~44 | lbl_80194728 | 0x2C | .rodata | 10 | fn_8004901C | addr | size+sec |
| `ang` | ~1028 | lbl_801B0AC8 | 0x404 | .data | 10 | fn_8000A9F0, fn_8000ABDC | addr | size+sec |
| `MusicPairCutRestore` | ~144 | lbl_801B143C | 0x90 | .data | 10 | fn_800374C8 | addr | size+sec |
| `BOLTSPACESHIPR` | ~1 | lbl_801D5358 | 0x1 | .data | 10 | fn_800A5DBC | addr | size+sec |
| `lego_aiconditiondefs` | ~1104 | lbl_801E0D08 | 0x450 | .data | 10 | fn_8012A5D0 | addr | size+sec |
| `lego_aiactiondefs` | ~1168 | lbl_801E1158 | 0x490 | .data | 10 | fn_8012A5D0 | addr | size+sec |
| `GCDATA_DEFAULT` | ~208 | lbl_801E1650 | 0xD0 | .data | 10 | fn_800DCB80 | addr | size+sec |
| `file_buff` | ~4128 | lbl_802005A8 | 0x1020 | .bss | 10 | NuFileRead | addr | size+sec |
| `fpars` | ~4068 | lbl_802015C8 | 0xFE4 | .bss | 10 | fn_80006B14 | addr | size+sec |
| `nufile_blkinfo` | ~12288 | lbl_802025AC | 0x3000 | .bss | 10 | fn_80006B14 | addr | size+sec |
| `PS2_SCRATCH_BASE` | ~16384 | lbl_802211E0 | 0x4000 | .bss | 10 | fn_8003117C | addr | size+sec |
| `sfx_filename` | ~51264 | lbl_802B15E4 | 0xC840 | .bss | 10 | fn_8006BBA8, fn_8006C748 | addr | size+sec |
| `argstext` | ~2048 | lbl_802D4EF7 | 0x800 | .bss | 10 | fn_8009D23C | addr | size+sec |
| `StatusPacket` | ~148 | lbl_802E0BE4 | 0x94 | .bss | 10 | fn_800C6900, fn_800C67B8 | addr | size+sec |
| `mapminishadowmtx` | ~1408 | lbl_802E1074 | 0x580 | .bss | 10 | fn_800E1850, fn_800E1564 | addr | size+sec |
| `streaks` | ~29696 | lbl_802E2320 | 0x7400 | .bss | 10 | fn_8010B140 | addr | size+sec |
| `Transfer` | ~112 | lbl_802F4850 | 0x70 | .bss | 10 | fn_80166118, fn_8016604C | addr | size+sec |
| `ntalsysbuff` | ~768 | lbl_80312E50 | 0x300 | .bss | 10 | fn_80015BD0 | addr | size+sec |
| `atoz0to9icon` | ~432 | lbl_80402254 | 0x1B0 | .bss | 10 | fn_801473E8, fn_801474EC | addr | size+sec |
| `KashyyykB_mine_specials` | ~72 | lbl_8040710C | 0x48 | .bss | 10 | fn_8014C8B0, fn_8014C728 | addr | size+sec |
| `PodRace_snipers` | ~320 | lbl_804071B4 | 0x140 | .bss | 10 | fn_8014D0A8, fn_8014D1F8 | addr | size+sec |
| `ZGV__ZZNK10Metrowerks23__cdeque_deleter_commo` | ~3 | lbl_804088B0 | 0x3 | .sdata | 10 | fn_80151510 | addr | size+sec |

---
## LOW Confidence

| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | Score | Xref Funcs | Evidence |
|------------|--------|-----------|-------|--------|-------|------------|----------|
| `BobaFettBoyAnim` | ~512 | lbl_80193B78 | 0x1F0 | .rodata | 7 | fn_80042E6C, fn_80042EE0 | size:near (512 vs 0x1F0) |
| `DroidekaAnim` | ~576 | lbl_801E2B40 | 0x230 | .data | 7 | fn_8015DA0C, fn_8015DC04, fn_8015DAA4 | size:near (576 vs 0x230) |
| `GameCam` | ~380 | lbl_80220FBC | 0x1FC | .bss | 7 | fn_80033EA4, fn_8003117C, fn_800339C0 | size:near (380 vs 0x1FC) |
| `GamePad` | ~5888 | lbl_802F4D60 | 0x1680 | .bss | 7 | fn_80172A90, fn_801729C8, fn_80172938 | size:near (5888 vs 0x1680) |
| `SfxInfo` | ~16024 | lbl_803D0724 | 0x3E94 | .bss | 7 | fn_8006BBA8, fn_8006C748, fn_8006C65C | size:near (16024 vs 0x3E94) |
| `KaaduAnim` | ~320 | lbl_80190684 | 0x150 | .rodata | 6 | fn_8001C110 | size:near (320 vs 0x150) |
| `LevelConfigList` | ~488 | lbl_801A5B4C | 0x1E0 | .rodata | 6 | fn_800E2944 | size:near (488 vs 0x1E0) |
| `ZTV16CJoystickDevice8` | ~168 | lbl_801B18DC | 0xA4 | .data | 6 | fn_801486FC, fn_8014ABAC, fn_8002BFC0 | size:near (168 vs 0xA4) |
| `TerSurface` | ~240 | lbl_801D2CE4 | 0xEC | .data | 6 | fn_800BA74C, fn_800A2100, fn_80114AE8 | size:near (240 vs 0xEC) |
| `CloneAnim` | ~960 | lbl_801E1C70 | 0x3C4 | .data | 6 | fn_80150428 | size:near (960 vs 0x3C4) |
| `CharacterConfigList` | ~1296 | lbl_801E2530 | 0x590 | .data | 6 | fn_8015CAD4 | size:near (1296 vs 0x590) |
| `DarthSidiousAnim` | ~2240 | lbl_801E3E18 | 0x900 | .data | 6 | fn_80161160 | size:near (2240 vs 0x900) |
| `Explosion` | ~176 | lbl_80207D20 | 0xA0 | .bss | 6 | fn_8000A500, fn_8000A40C, fn_8000A530 | size:near (176 vs 0xA0) |
| `sResult$6286` | ~2 | lbl_802B0D90 | 0x6 | .bss | 6 | fn_800466E4, fn_80046264, fn_80046500 | size:near (2 vs 0x6) |
| `localMatrixIdentity` | ~64 | lbl_802E0B38 | 0x3C | .bss | 6 | fn_800C3054, fn_800C2F20, fn_800C2D14 | size:near (64 vs 0x3C) |
| `sCaptureName` | ~256 | lbl_802F4AB8 | 0xF0 | .bss | 6 | fn_8016C468, fn_8016BEC0, fn_8016CDE0 | size:near (256 vs 0xF0) |
| `memfiles` | ~400 | lbl_803120C0 | 0x188 | .bss | 6 | fn_80005D34, fn_80005D98, fn_80005A4C | size:near (400 vs 0x188) |
| `GameMessage` | ~9984 | lbl_803407FC | 0x2800 | .bss | 6 | fn_80038058, fn_80037E1C | size:near (9984 vs 0x2800) |
| `ZTV16CDirect3DSurface` | ~140 | lbl_80191A40 | 0x84 | .rodata | 5 | fn_80029EC8 | size:near (140 vs 0x84) |
| `host_device` | ~3236 | lbl_801B3C8C | 0xCA8 | .data | 5 | fn_800369C0 | size:near (3236 vs 0xCA8) |
| `ObjTab` | ~2296 | lbl_801D5AA4 | 0x8F4 | .data | 5 | fn_801474EC | size:near (2296 vs 0x8F4) |
| `txt_DUTCH` | ~468 | lbl_801E2D70 | 0x1B4 | .data | 5 | fn_8015E224 | size:near (468 vs 0x1B4) |
| `Default_ADDPART` | ~156 | lbl_801F5B40 | 0x98 | .data | 5 | fn_80167178 | size:near (156 vs 0x98) |
| `padnames_uk` | ~224 | lbl_801F6CA8 | 0xD8 | .data | 5 | fn_8017482C | size:near (224 vs 0xD8) |

---
## REJECT

| GC Object | GC Sz | Mac Candidate | Score | Evidence / Reason |
|-----------|-------|---------------|-------|-------------------|
| lbl_801A5338 | 0x4 | `GameMenuLevel` | 24 | Mac already matched to better GC (lbl_801D68E0) |
| lbl_801B1638 | 0x40 | `NuTimeBar_DefaultColours` | 23 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801B1778 | 0x40 | `NuTimeBar_DefaultColours` | 23 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801A005C | 0x4 | `GameMenuLevel` | 20 | Mac already matched to better GC (lbl_801D68E0) |
| lbl_801A04F0 | 0x4 | `GameMenuLevel` | 20 | Mac already matched to better GC (lbl_801D68E0) |
| lbl_801A53C8 | 0x4 | `GameMenuLevel` | 20 | Mac already matched to better GC (lbl_801D68E0) |
| lbl_801B1738 | 0x40 | `NuTimeBar_DefaultColours` | 19 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801D2DD0 | 0x4 | `GameMenuLevel` | 19 | Mac already matched to better GC (lbl_801D68E0) |
| lbl_80190C30 | 0x8 | `NuFade_Enabled` | 18 | Mac already matched to better GC (lbl_80190BF8) |
| lbl_80190C38 | 0x8 | `NuFade_Enabled` | 18 | Mac already matched to better GC (lbl_80190BF8) |
| lbl_80190C50 | 0x4 | `NuRndrWaterLevel` | 18 | Mac already matched to better GC (lbl_80190C4C) |
| lbl_80190C54 | 0x4 | `NuRndrWaterLevel` | 18 | Mac already matched to better GC (lbl_80190C4C) |
| lbl_80190C58 | 0x4 | `NuRndrWaterLevel` | 18 | Mac already matched to better GC (lbl_80190C4C) |
| lbl_801A0108 | 0x4 | `GameMenuLevel` | 18 | Mac already matched to better GC (lbl_801D68E0) |
| lbl_801B17B8 | 0x40 | `NuTimeBar_DefaultColours` | 18 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801B1878 | 0x40 | `NuTimeBar_DefaultColours` | 18 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_8031325C | 0xC | `NuRndrDebRange` | 18 | Mac already matched to better GC (lbl_80313250) |
| lbl_801B1678 | 0x40 | `NuTimeBar_DefaultColours` | 17 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801B16B8 | 0x40 | `NuTimeBar_DefaultColours` | 17 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801B16F8 | 0x40 | `NuTimeBar_DefaultColours` | 17 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801B17F8 | 0x40 | `NuTimeBar_DefaultColours` | 17 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801B1838 | 0x40 | `NuTimeBar_DefaultColours` | 17 | Mac already matched to better GC (lbl_801B0F08) |
| lbl_801A0008 | 0x4 | `kFirstTimeKey` | 14 | Mac already matched to better GC (lbl_80190A3C) |
| lbl_801A0048 | 0x4 | `kFirstTimeKey` | 14 | Mac already matched to better GC (lbl_80190A3C) |
| lbl_801A010C | 0x4 | `kFirstTimeKey` | 14 | Mac already matched to better GC (lbl_80190A3C) |
| lbl_802C4EDC | 0xC | `global_Light2Position` | 14 | Mac already matched to better GC (lbl_803E04C4) |
| lbl_803E0108 | 0x10 | `LevInstAnim` | 14 | Mac already matched to better GC (lbl_803145D0) |
| lbl_8019217C | 0x4 | `kFirstTimeKey` | 13 | Mac already matched to better GC (lbl_80190A3C) |
| lbl_801943BC | 0x38 | `ZTV13CMemoryBuffer` | 13 | Mac already matched to better GC (lbl_80191930) |
| lbl_801A0030 | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801A0ECC | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801A5680 | 0x10 | `global_Light0Color` | 13 | Mac already matched to better GC (lbl_801A2F4C) |
| lbl_801B0ECC | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801B0ED8 | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801B0EE4 | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801B0F88 | 0x4 | `kFirstTimeKey` | 13 | Mac already matched to better GC (lbl_80190A3C) |
| lbl_801B14D8 | 0x10 | `global_Light0Color` | 13 | Mac already matched to better GC (lbl_801A2F4C) |
| lbl_801D5964 | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801D597C | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801D5988 | 0xC | `Area_JediBattleLevelList` | 13 | Mac already matched to better GC (lbl_801A0024) |
| lbl_801D5AA0 | 0x4 | `kFirstTimeKey` | 13 | Mac already matched to better GC (lbl_80190A3C) |
| lbl_801D68E4 | 0x8 | `Area_GunshipCavalryLevelList` | 13 | Mac already matched to better GC (lbl_80193B70) |
| lbl_801E19C0 | 0x8 | `Area_GunshipCavalryLevelList` | 13 | Mac already matched to better GC (lbl_80193B70) |
| lbl_801F5A6C | 0x14 | `Area_EscapeFromNabooLevelList` | 13 | Mac already matched to better GC (lbl_801E15E8) |
| lbl_801F5A80 | 0xC0 | `STAPAnim` | 13 | Mac already matched to better GC (lbl_80191200) |
| lbl_80207DC0 | 0xC | `global_Light2Position` | 13 | Mac already matched to better GC (lbl_803E04C4) |
| lbl_802B0C20 | 0x20 | `DoorExitCameraSplineName` | 13 | Mac already matched to better GC (lbl_80313268) |
| lbl_802B10C0 | 0x20 | `DoorExitCameraSplineName` | 13 | Mac already matched to better GC (lbl_80313268) |
| lbl_802D5D28 | 0x20 | `DoorExitCameraSplineName` | 13 | Mac already matched to better GC (lbl_80313268) |
| lbl_802D5D48 | 0xC | `global_Light2Position` | 13 | Mac already matched to better GC (lbl_803E04C4) |
| lbl_802E0BB0 | 0xC | `global_Light2Position` | 13 | Mac already matched to better GC (lbl_803E04C4) |
| lbl_802E0C80 | 0x14 | `ParticleChunkRenderStack` | 13 | Mac already matched to better GC (lbl_802E0B9C) |
| lbl_802F48C0 | 0x20 | `DoorExitCameraSplineName` | 13 | Mac already matched to better GC (lbl_80313268) |
| lbl_802F4900 | 0x30 | `LevForceBlownUp` | 13 | Mac already matched to better GC (lbl_80340404) |
| lbl_802F4958 | 0x20 | `DoorExitCameraSplineName` | 13 | Mac already matched to better GC (lbl_80313268) |
| lbl_802F4C00 | 0x10 | `LevInstAnim` | 13 | Mac already matched to better GC (lbl_803145D0) |
| lbl_802F4C10 | 0x40 | `CutCamMtx` | 13 | Mac already matched to better GC (lbl_802D5D54) |
| lbl_802F4C50 | 0x80 | `LevForce` | 13 | Mac already matched to better GC (lbl_802F46B0) |
| lbl_80340464 | 0x30 | `LevForceBlownUp` | 13 | Mac already matched to better GC (lbl_80340404) |
| lbl_80340494 | 0x40 | `CutCamMtx` | 13 | Mac already matched to better GC (lbl_802D5D54) |

---
## GC Data With Xrefs But No Viable Mac Candidate

| GC Object | GC Sz | GC Sec | Xref Funcs | Xref Types |
|-----------|-------|--------|------------|------------|
| lbl_80152700 | 0x0 | .text | fn_8015057C, fn_80151510 | addr |
| lbl_80152718 | 0x0 | .text | fn_80151510 | addr |
| lbl_80152720 | 0x0 | .text | fn_80151510 | addr |
| lbl_80152728 | 0x0 | .text | fn_80151510 | addr |
| lbl_80152730 | 0x0 | .text | fn_80151510 | addr |
| lbl_8015BF34 | 0x0 | .text | fn_8015057C | addr |
| lbl_8019017C | 0x2B | .rodata | NuAnimDataLoad | addr |
| lbl_8019063C | 0x46 | .rodata | fn_8001C110 | addr |
| lbl_801909C4 | 0x1D | .rodata | fn_8001DCEC | addr |
| lbl_80190A08 | 0x1A | .rodata | fn_8001E430 | addr |
| lbl_80190AD4 | 0x2A | .rodata | fn_8001FD98 | addr |
| lbl_80190B00 | 0x12 | .rodata | fn_8001FD98 | addr |
| lbl_801912C4 | 0x2D | .rodata | fn_80024D58, fn_80024A34 | addr |
| lbl_801912F4 | 0x26 | .rodata | fn_80024A34 | addr |
| lbl_801913BC | 0x2D | .rodata | fn_80024DC8, fn_80024D58 | addr |
| lbl_80191480 | 0x1E | .rodata | fn_80025A70 | addr |
| lbl_80191510 | 0x25 | .rodata | fn_80027378, fn_80027D54 | addr |
| lbl_8019177C | 0xE | .rodata | fn_80028138 | addr |
| lbl_801917D8 | 0x19 | .rodata | fn_80027FAC | addr |
| lbl_80191990 | 0x16 | .rodata | fn_80028B88 | addr |
| lbl_801919E4 | 0x1A | .rodata | fn_80029EC8 | addr |
| lbl_80191A00 | 0x16 | .rodata | fn_80029EC8 | addr |
| lbl_80191A18 | 0x26 | .rodata | fn_80029EC8 | addr |
| lbl_80191B04 | 0x26 | .rodata | fn_8002A948 | addr |
| lbl_80191B2C | 0x33 | .rodata | fn_8002A948 | addr |
| lbl_80191B6C | 0x16 | .rodata | fn_8002A948 | addr |
| lbl_80191D80 | 0x32 | .rodata | fn_8002C460 | addr |
| lbl_80191DB4 | 0x1A | .rodata | fn_8002C460 | addr |
| lbl_80192310 | 0x1E | .rodata | fn_80032CB4 | addr |
| lbl_801923B4 | 0x12 | .rodata | fn_80033F30 | addr |
| lbl_8019253C | 0x31 | .rodata | fn_800350B8, fn_80035470, fn_80037228 | addr |
| lbl_80192664 | 0x6 | .rodata | fn_80037188 | addr |
| lbl_80192B44 | 0xF | .rodata | fn_80038058, fn_80037E1C | addr |
| lbl_80192CA8 | 0x2B | .rodata | fn_80038628, fn_80038A98 | addr |
| lbl_80192D4C | 0x2B | .rodata | fn_80038A98 | addr |
| lbl_80192E0C | 0x2B | .rodata | fn_80039DB4 | addr |
| lbl_80192E38 | 0x2D | .rodata | fn_80039DB4 | addr |
| lbl_8019308C | 0x2E | .rodata | fn_8003DA04, fn_8003C924, fn_8003B5F8, fn_8003FC1C, fn_8003B874 | addr |
| lbl_801930BC | 0x27 | .rodata | fn_8003A244 | addr |
| lbl_801931F4 | 0x2 | .rodata | fn_8003B5F8 | addr |
| lbl_801931F8 | 0x29 | .rodata | fn_8003B5F8 | addr |
| lbl_80193248 | 0x1E | .rodata | fn_8003B5F8 | addr |
| lbl_801932BC | 0x3E | .rodata | fn_80040974, fn_8003BA5C, fn_8003B874 | addr |
| lbl_801932FC | 0x45 | .rodata | fn_80040974, fn_8003BA5C, fn_8003B874 | addr |
| lbl_80193344 | 0x2A | .rodata | fn_80040974, fn_8003BA5C, fn_8003B874 | addr |
| lbl_80193908 | 0x17 | .rodata | fn_80040E48 | addr |
| lbl_80193920 | 0x17 | .rodata | fn_80040E48 | addr |
| lbl_80193938 | 0x1B | .rodata | fn_80040E48 | addr |
| lbl_80193954 | 0x15 | .rodata | fn_80040E48 | addr |
| lbl_8019396C | 0x19 | .rodata | fn_80040E48 | addr |
| lbl_801939C4 | 0x17 | .rodata | fn_80040E48 | addr |
| lbl_801939DC | 0x19 | .rodata | fn_80040E48 | addr |
| lbl_801939F8 | 0x1D | .rodata | fn_80040E48 | addr |
| lbl_80193A18 | 0x15 | .rodata | fn_80040E48 | addr |
| lbl_80193A30 | 0x19 | .rodata | fn_80040E48 | addr |
| lbl_80193A4C | 0x15 | .rodata | fn_80040E48 | addr |
| lbl_80193A64 | 0x19 | .rodata | fn_80040E48 | addr |
| lbl_80193A80 | 0x19 | .rodata | fn_80040E48 | addr |
| lbl_80193A9C | 0x15 | .rodata | fn_80040E48 | addr |
| lbl_80193AB4 | 0x15 | .rodata | fn_80040E48 | addr |
| lbl_80193ACC | 0x16 | .rodata | fn_80040E48 | addr |
| lbl_80193AE4 | 0x1A | .rodata | fn_80040E48 | addr |
| lbl_80193B00 | 0x17 | .rodata | fn_80040E48 | addr |
| lbl_80193F2C | 0x33 | .rodata | fn_80044400, fn_80044668, fn_8004425C, fn_80044C78, fn_80045004 | addr |
| lbl_80193F9C | 0x1E | .rodata | fn_8004425C | addr |
| lbl_80193FBC | 0x53 | .rodata | fn_8004425C | addr |
| lbl_80194010 | 0x53 | .rodata | fn_8004425C | addr |
| lbl_80194064 | 0x19 | .rodata | fn_8004425C | addr |
| lbl_80194080 | 0xF | .rodata | fn_80044400, fn_800450C0 | addr |
| lbl_80194090 | 0x17 | .rodata | fn_80044400 | addr |
| lbl_801940B4 | 0xF | .rodata | fn_80044668 | addr |
| lbl_801940C4 | 0x15 | .rodata | fn_80044C78 | addr |
| lbl_801941C0 | 0x12 | .rodata | fn_80045F70 | addr |
| lbl_801941FC | 0x2D | .rodata | fn_80045F70 | addr |
| lbl_8019422C | 0x1A | .rodata | fn_800457D4 | addr |
| lbl_80194248 | 0x1D | .rodata | fn_800457D4 | addr |
| lbl_80194268 | 0x2B | .rodata | fn_800457D4 | addr |
| lbl_801942F4 | 0x3B | .rodata | fn_80045E40 | addr |
| lbl_801943F4 | 0x2 | .rodata | fn_80046264 | addr |
| lbl_80194408 | 0x1A | .rodata | fn_80046500 | addr |
| lbl_80194424 | 0x1A | .rodata | fn_800466E4 | addr |
| lbl_80194440 | 0xE | .rodata | fn_800466E4 | addr |
| lbl_80194470 | 0x1E | .rodata | fn_80047078 | addr |
| lbl_801944E4 | 0x39 | .rodata | fn_800476DC, fn_80047978 | addr |
| lbl_80194520 | 0x2 | .rodata | fn_800476DC | addr |
| lbl_80194540 | 0xE | .rodata | fn_80047978 | addr |
| lbl_80194624 | 0x31 | .rodata | fn_80049A14, fn_8004999C, fn_8004876C, fn_8004901C | addr |
| lbl_80194658 | 0x27 | .rodata | fn_8004876C, fn_8004999C | addr |
| lbl_80194680 | 0x1A | .rodata | fn_8004876C | addr |
| lbl_801946C4 | 0x1E | .rodata | fn_80049A14 | addr |
| lbl_80194754 | 0x16 | .rodata | fn_8004901C | addr |
| lbl_80194944 | 0x19 | .rodata | fn_8004901C | addr |
| lbl_80194AC8 | 0x19 | .rodata | fn_8004901C | addr |
| lbl_80194AE4 | 0x1F | .rodata | fn_8004901C | addr |
| lbl_80194B5C | 0x36 | .rodata | fn_80049C5C | addr |
| lbl_80194B94 | 0x27 | .rodata | fn_80049C5C | addr |
| lbl_80194BBC | 0x27 | .rodata | fn_80049C5C | addr |
| lbl_80194BE4 | 0x27 | .rodata | fn_80049C5C | addr |
| lbl_80194C0C | 0x1A | .rodata | fn_8004AF88 | addr |
| lbl_80194C94 | 0x2F | .rodata | fn_8005A338, fn_8004C118, fn_8004C538 | addr |
| lbl_80194CC4 | 0xF | .rodata | fn_8004C118 | addr |
| lbl_80194CDC | 0x12 | .rodata | fn_8004C118 | addr |
| lbl_80194DA8 | 0x3F | .rodata | fn_8004C538 | addr |
| lbl_80194E64 | 0x52 | .rodata | fn_8004C538 | addr |
| lbl_80195548 | 0x2E | .rodata | fn_8005CB34 | addr |
| lbl_80195578 | 0x3D | .rodata | fn_8005CB34 | addr |
| lbl_801A04A4 | 0x12 | .rodata | fn_800B3774 | addr |
| lbl_801A04B8 | 0xD | .rodata | fn_800B3774, Menu_Unknown4_HandleSelection, fn_800B02F0 | addr |
| lbl_801A04C8 | 0xD | .rodata | fn_800B3774 | addr |
| lbl_801A1064 | 0x19 | .rodata | fn_800DFC04 | addr |
| lbl_801A1098 | 0x19 | .rodata | fn_800D28A0, Menu_NavigateForward | addr |
| lbl_801A1630 | 0x6 | .rodata | fn_800BA0EC | addr |
| lbl_801A1638 | 0x6 | .rodata | fn_800BA0EC | addr |
| lbl_801A1640 | 0x6 | .rodata | fn_800BA0EC | addr |
| lbl_801A1648 | 0x6 | .rodata | fn_800BA0EC | addr |
| lbl_801A168C | 0x6 | .rodata | fn_800B4DF0 | addr |
| lbl_801A16A4 | 0x6 | .rodata | fn_800E1D0C, fn_800B4DF0 | addr |
| lbl_801A16C8 | 0xA | .rodata | fn_800B4DF0 | addr |
| lbl_801A181C | 0xD | .rodata | fn_800B6D94 | addr |
| lbl_801A186C | 0xA | .rodata | fn_800B6D94 | addr |
| lbl_801A1884 | 0xA | .rodata | fn_800B6D94 | addr |
| lbl_801A1898 | 0x16 | .rodata | fn_800B6D94 | addr |
| lbl_801A1AC0 | 0x6 | .rodata | fn_800BB5A8 | addr |
| lbl_801A1AF8 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B08 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B18 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B28 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B38 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B48 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B58 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B68 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B78 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B88 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1B98 | 0xD | .rodata | fn_800BB950 | addr |
| lbl_801A1BA8 | 0xA | .rodata | fn_800CE7F4, fn_800BB950 | addr |
| lbl_801A1BF8 | 0xA | .rodata | fn_800BC048 | addr |
| lbl_801A1C10 | 0xA | .rodata | fn_800BC048 | addr |
| lbl_801A1C1C | 0xA | .rodata | fn_800BC048 | addr |
| lbl_801A1C5C | 0xE | .rodata | fn_800BC1A8 | addr |
| lbl_801A1C6C | 0xE | .rodata | fn_800BC1A8 | addr |
| lbl_801A1D04 | 0xF | .rodata | fn_800BC678, fn_800BCED8 | addr |
| lbl_801A1D70 | 0xE | .rodata | fn_800CD0DC | addr |
| lbl_801A1D80 | 0xD | .rodata | fn_800CD0DC | addr |
| lbl_801A1E68 | 0xF | .rodata | fn_800CD234 | addr |
| lbl_801A1EAC | 0x6 | .rodata | fn_800CD2B4 | addr |
| lbl_801A1EB4 | 0x6 | .rodata | fn_800CD2B4 | addr |
| lbl_801A1EC4 | 0xF | .rodata | fn_800BD1F4, fn_800BD594 | addr |
| lbl_801A1EE4 | 0x16 | .rodata | fn_800BD1F4, fn_800BD594 | addr |
| lbl_801A1F00 | 0xE | .rodata | fn_800CD364 | addr |
| lbl_801A1F10 | 0xE | .rodata | fn_800CD364 | addr |
| lbl_801A1F2C | 0xA | .rodata | fn_800BD7BC | addr |
| lbl_801A1F38 | 0xA | .rodata | fn_800BD7BC | addr |
| lbl_801A1F44 | 0xA | .rodata | fn_800BD7BC | addr |
| lbl_801A1F50 | 0x6 | .rodata | fn_800BD7BC | addr |
| lbl_801A1F58 | 0x6 | .rodata | fn_800BD7BC | addr |
| lbl_801A1F74 | 0x12 | .rodata | fn_800BD8E8 | addr |
| lbl_801A1F88 | 0xF | .rodata | fn_800BD8E8 | addr |
| lbl_801A1F98 | 0xF | .rodata | fn_800BD8E8 | addr |
| lbl_801A1FC0 | 0xE | .rodata | fn_800BD8E8 | addr |
| lbl_801A1FD0 | 0xD | .rodata | fn_800BD8E8 | addr |
| lbl_801A1FE8 | 0xA | .rodata | fn_800BDA3C | addr |
| lbl_801A1FF4 | 0xA | .rodata | fn_800BDA3C | addr |
| lbl_801A2000 | 0x12 | .rodata | fn_800BDA3C | addr |
| lbl_801A20AC | 0x6 | .rodata | fn_800BE118 | addr |
| lbl_801A20C0 | 0xE | .rodata | fn_800BE118 | addr |
| lbl_801A20D0 | 0xE | .rodata | fn_800BE118 | addr |
| lbl_801A20E0 | 0xE | .rodata | fn_800BE118 | addr |
| lbl_801A21B4 | 0xA | .rodata | fn_800BE7E0 | addr |
| lbl_801A21C0 | 0xA | .rodata | fn_800BE7E0 | addr |
| lbl_801A21CC | 0xA | .rodata | fn_800BE7E0 | addr |
| lbl_801A21D8 | 0xA | .rodata | fn_800BE7E0 | addr |
| lbl_801A21F0 | 0xA | .rodata | fn_800BE7E0 | addr |
| lbl_801A2208 | 0x15 | .rodata | fn_800BE8E8 | addr |
| lbl_801A2284 | 0xE | .rodata | fn_800BEE34 | addr |
| lbl_801A2294 | 0xD | .rodata | fn_800BEE34 | addr |
| lbl_801A22A4 | 0xF | .rodata | fn_800BEE34 | addr |
| lbl_801A22B4 | 0xE | .rodata | fn_800BEE34 | addr |
| lbl_801A22D0 | 0xA | .rodata | fn_800BEE34 | addr |
| lbl_801A22DC | 0x6 | .rodata | fn_800BEE34 | addr |
| lbl_801A2308 | 0xD | .rodata | fn_800BEE34 | addr |
| lbl_801A2318 | 0xE | .rodata | fn_800BF2CC, fn_800BF160 | addr |
| lbl_801A2328 | 0xE | .rodata | fn_800BF2CC, fn_800BF160 | addr |
| lbl_801A2338 | 0xA | .rodata | fn_800BF160 | addr |
| lbl_801A2354 | 0x16 | .rodata | fn_800BF2CC | addr |
| lbl_801A2384 | 0x12 | .rodata | fn_800BF2CC | addr |
| lbl_801A23CC | 0x12 | .rodata | fn_800BFDFC | addr |
| lbl_801A23E0 | 0x12 | .rodata | fn_800BFDFC | addr |
| lbl_801A23F4 | 0x12 | .rodata | fn_800BFDFC | addr |
| lbl_801A2408 | 0x12 | .rodata | fn_800BFDFC | addr |
| lbl_801A241C | 0x12 | .rodata | fn_800BFDFC | addr |
| lbl_801A2430 | 0x12 | .rodata | fn_800BFDFC | addr |
| lbl_801A2470 | 0xD | .rodata | fn_800CD928 | addr |
| lbl_801A2514 | 0xE | .rodata | fn_800C0164 | addr |
| lbl_801A2524 | 0xD | .rodata | fn_800C0164 | addr |
| lbl_801A2534 | 0xF | .rodata | fn_800C0164 | addr |
| lbl_801A2558 | 0xE | .rodata | fn_800C04C4 | addr |
| lbl_801A2568 | 0x16 | .rodata | fn_800C04C4 | addr |
| lbl_801A2614 | 0x12 | .rodata | fn_800C12D0 | addr |
| lbl_801A2654 | 0xA | .rodata | fn_800C1400 | addr |
| lbl_801A2660 | 0x15 | .rodata | fn_800C1400 | addr |
| lbl_801A269C | 0x6 | .rodata | fn_800CDDC4 | addr |
| lbl_801A26A4 | 0x6 | .rodata | fn_800CDDC4 | addr |
| lbl_801A26E0 | 0x15 | .rodata | fn_800C169C | addr |
| lbl_801A26F8 | 0xD | .rodata | fn_800C169C | addr |
| lbl_801A2754 | 0xA | .rodata | fn_800C1F54 | addr |
| lbl_801A2760 | 0xA | .rodata | fn_800C1F54 | addr |
| lbl_801A276C | 0xA | .rodata | fn_800C1F54 | addr |
| lbl_801A2778 | 0xA | .rodata | fn_800C1F54 | addr |
| lbl_801A2790 | 0xD | .rodata | fn_800C239C | addr |
| lbl_801A27A0 | 0xD | .rodata | fn_800C239C | addr |
| lbl_801A27B0 | 0xD | .rodata | fn_800C239C | addr |
| lbl_801A27C0 | 0xE | .rodata | fn_800C239C | addr |
| lbl_801A27D0 | 0x6 | .rodata | fn_800C239C | addr |
| lbl_801A27D8 | 0xE | .rodata | fn_800C239C | addr |
| lbl_801A27E8 | 0xF | .rodata | fn_800C239C | addr |
| lbl_801A27F8 | 0xE | .rodata | fn_800C239C | addr |
| lbl_801A2808 | 0xF | .rodata | fn_800C239C | addr |
| lbl_801A2818 | 0xF | .rodata | fn_800C239C | addr |
| lbl_801A2990 | 0xD | .rodata | fn_800C3B54 | addr |
| lbl_801A29B8 | 0xD | .rodata | fn_800C3EEC | addr |
| lbl_801A2B00 | 0xD | .rodata | fn_800CE2F8 | addr |
| lbl_801A2B10 | 0xD | .rodata | fn_800CE2F8 | addr |
| lbl_801A2B20 | 0xE | .rodata | fn_800C4758 | addr |
| lbl_801A2B30 | 0xA | .rodata | fn_800C4758 | addr |
| lbl_801A2B48 | 0xA | .rodata | fn_800C4978 | addr |
| lbl_801A2BA8 | 0xD | .rodata | fn_800CE948 | addr |
| lbl_801A2BB8 | 0xD | .rodata | fn_800CE980 | addr |
| lbl_801A2BDC | 0x6 | .rodata | fn_800C5174 | addr |
| lbl_801A2BE2 | 0x6 | .rodata | fn_800C5174 | addr |
| lbl_801A2C08 | 0x6 | .rodata | fn_800CEA9C | addr |
| lbl_801A2C2C | 0xA | .rodata | fn_800C5498 | addr |
| lbl_801A2CB4 | 0xF | .rodata | Action_BoulderSection | addr |
| lbl_801A2CC4 | 0xD | .rodata | Action_BoulderSection | addr |
| lbl_801A2CE0 | 0xA | .rodata | fn_800CEC90 | addr |
| lbl_801A2CEC | 0xD | .rodata | fn_800CEC90 | addr |
| lbl_801A2CFC | 0xE | .rodata | fn_800CEC90 | addr |
| lbl_801A2D0C | 0xE | .rodata | fn_800CEC90 | addr |
| lbl_801A2D1C | 0xE | .rodata | fn_800CEC90 | addr |
| lbl_801A2D3C | 0xA | .rodata | fn_800CEE40, fn_800C67B8 | addr |
| lbl_801A2D48 | 0xE | .rodata | fn_800CEE40, fn_800C62A0 | addr |
| lbl_801A2D80 | 0xE | .rodata | fn_800C62A0 | addr |
| lbl_801A2DBC | 0xE | .rodata | fn_800C6430 | addr |
| lbl_801A2DCC | 0xE | .rodata | fn_800C6430 | addr |
| lbl_801A2DE8 | 0xF | .rodata | fn_800CEFAC | addr |
| lbl_801A2F3C | 0x6 | .rodata | fn_800C6FB8 | addr |
| lbl_801A2F80 | 0xA | .rodata | fn_800C72D4 | addr |
| lbl_801A2F8C | 0x15 | .rodata | fn_800C749C | addr |
| lbl_801A2FA8 | 0x6 | .rodata | fn_800C78EC, fn_800C757C | addr |
| lbl_801A2FB0 | 0xE | .rodata | fn_800C757C | addr |
| lbl_801A3024 | 0xA | .rodata | fn_800C7DB4 | addr |
| lbl_801A303C | 0xA | .rodata | fn_800C7DB4 | addr |
| lbl_801A3048 | 0xD | .rodata | fn_800C7DB4 | addr |
| lbl_801A3058 | 0xD | .rodata | fn_800C7DB4 | addr |
| lbl_801A3068 | 0xD | .rodata | fn_800C7DB4 | addr |
| lbl_801A309C | 0x6 | .rodata | fn_800C88E0 | addr |
| lbl_801A4C58 | 0xD | .rodata | fn_800C9AF4 | addr |
| lbl_801A4C88 | 0x12 | .rodata | fn_800CA018 | addr |
| lbl_801A4C9C | 0xF | .rodata | fn_800CA018 | addr |
| lbl_801A4CC0 | 0xF | .rodata | fn_800CA018 | addr |
| lbl_801A4CE4 | 0x12 | .rodata | fn_800CA018 | addr |
| lbl_801A4D18 | 0x6 | .rodata | fn_800CA4AC | addr |
| lbl_801A4D20 | 0x6 | .rodata | fn_800CAEC0, fn_800CA4AC | addr |
| lbl_801A4D4C | 0x2 | .rodata | fn_800CAC5C | addr |
| lbl_801A4D58 | 0xF | .rodata | fn_800CAEC0 | addr |
| lbl_801A4D68 | 0xE | .rodata | fn_800CAEC0 | addr |
| lbl_801A4D78 | 0xE | .rodata | fn_800CAEC0 | addr |
| lbl_801A4D88 | 0xF | .rodata | fn_800CAEC0 | addr |
| lbl_801A4D98 | 0xF | .rodata | fn_800CAEC0 | addr |
| lbl_801A4DA8 | 0xD | .rodata | fn_800CAEC0 | addr |
| lbl_801A4DB8 | 0xF | .rodata | fn_800CAEC0 | addr |
| lbl_801A4DC8 | 0xE | .rodata | fn_800CAEC0 | addr |
| lbl_801A4DD8 | 0xE | .rodata | fn_800CAEC0 | addr |
| lbl_801A4DE8 | 0xF | .rodata | fn_800CAEC0 | addr |
| lbl_801A4DF8 | 0xF | .rodata | fn_800CAEC0 | addr |
| lbl_801A4E08 | 0xD | .rodata | fn_800CAEC0 | addr |
| lbl_801A4E18 | 0xE | .rodata | fn_800CAEC0 | addr |
| lbl_801A4E68 | 0x19 | .rodata | fn_800CAEC0 | addr |
| lbl_801A4E88 | 0xD | .rodata | fn_800CAEC0 | addr |
| lbl_801A5028 | 0xA | .rodata | fn_800D5488 | addr |
| lbl_801A5170 | 0xD | .rodata | fn_800D4810, fn_800D407C | addr |
| lbl_801A51A0 | 0xD | .rodata | fn_800D407C | addr |
| lbl_801A51F4 | 0xD | .rodata | fn_800D4810 | addr |
| lbl_801A55FC | 0xE | .rodata | fn_800DA478 | addr |
| lbl_801A5644 | 0x12 | .rodata | fn_800DA478 | addr |
| lbl_801A5658 | 0xD | .rodata | fn_800DA478 | addr |
| lbl_801A5784 | 0x2 | .rodata | fn_800DD1C8 | addr |
| lbl_801A57B4 | 0x12 | .rodata | fn_800DD1C8 | addr |
| lbl_801A57C8 | 0x2 | .rodata | fn_800DD1C8 | addr |
| lbl_801A57CC | 0x6 | .rodata | fn_800DD1C8 | addr |
| lbl_801A59C8 | 0x6 | .rodata | fn_800E2220 | addr |
| lbl_801A59EC | 0x6 | .rodata | fn_800E2398 | addr |
| lbl_801A5AB0 | 0x12 | .rodata | fn_800E2944 | addr |
| lbl_801A5AEC | 0x12 | .rodata | fn_800E2944 | addr |
| lbl_801A5B00 | 0x12 | .rodata | fn_800E2944 | addr |
| lbl_801A5D2C | 0x6 | .rodata | fn_800E2BA4 | addr |
| lbl_801A5DF8 | 0xE | .rodata | fn_800E356C, fn_800E3A74 | addr |
| lbl_801A5E08 | 0xA | .rodata | fn_800E356C | addr |
| lbl_801A5E2C | 0xE | .rodata | fn_800E356C, fn_800E3A74 | addr |
| lbl_801A5E54 | 0xA | .rodata | fn_800E6208 | addr |
| lbl_801A5E80 | 0xA | .rodata | fn_800E5C14, fn_800E6208 | addr |
| lbl_801A5E8C | 0xA | .rodata | fn_800E6208 | addr |
| lbl_801A5E98 | 0xA | .rodata | fn_800E6208 | addr |
| lbl_801A5EA4 | 0xA | .rodata | fn_800E6208 | addr |
| lbl_801A5EB0 | 0xD | .rodata | fn_800E36B4 | addr |
| lbl_801A5EC0 | 0xA | .rodata | fn_800E36B4 | addr |
| lbl_801A5ECC | 0xE | .rodata | fn_800E36B4, fn_800E3DC4 | addr |
| lbl_801A5EE8 | 0xA | .rodata | fn_800E36B4 | addr |
| lbl_801A5EF4 | 0xA | .rodata | fn_800E36B4 | addr |
| lbl_801A5F00 | 0xA | .rodata | fn_800E36B4 | addr |
| lbl_801A5F0C | 0xA | .rodata | fn_800E36B4 | addr |
| lbl_801A5F38 | 0xA | .rodata | fn_800E3824 | addr |
| lbl_801A5F44 | 0xA | .rodata | fn_800E3824 | addr |
| lbl_801A5F98 | 0xA | .rodata | fn_800E3A74 | addr |
| lbl_801A5FB0 | 0xF | .rodata | fn_800E3A74 | addr |
| lbl_801A5FD0 | 0xF | .rodata | fn_800E3A74 | addr |
| lbl_801A5FE0 | 0xF | .rodata | fn_800E3A74 | addr |
| lbl_801A5FF0 | 0x12 | .rodata | fn_800E3A74 | addr |
| lbl_801A6014 | 0xA | .rodata | fn_800E3A74 | addr |
| lbl_801A6020 | 0xA | .rodata | fn_800E3A74 | addr |
| lbl_801A6038 | 0xA | .rodata | fn_800E3A74 | addr |
| lbl_801A6044 | 0xF | .rodata | fn_800E3A74 | addr |
| lbl_801A6054 | 0xE | .rodata | fn_800E3A74 | addr |
| lbl_801A6064 | 0xD | .rodata | fn_800E3A74 | addr |
| lbl_801A608C | 0xA | .rodata | fn_800E3A74 | addr |
| lbl_801A60A4 | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A60B8 | 0xD | .rodata | fn_800E3DC4 | addr |
| lbl_801A60FC | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A611C | 0xE | .rodata | fn_800E3DC4 | addr |
| lbl_801A6194 | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6218 | 0xF | .rodata | fn_800E3DC4 | addr |
| lbl_801A6228 | 0xD | .rodata | fn_800E3DC4 | addr |
| lbl_801A6238 | 0xD | .rodata | fn_800E3DC4 | addr |
| lbl_801A6278 | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A62A4 | 0xD | .rodata | fn_800E3DC4 | addr |
| lbl_801A6338 | 0xE | .rodata | fn_800E3DC4 | addr |
| lbl_801A6354 | 0xA | .rodata | fn_800E3DC4 | addr |
| lbl_801A636C | 0xF | .rodata | fn_800E3DC4 | addr |
| lbl_801A63A4 | 0xF | .rodata | fn_800CC0F8, Action_BoulderSection, fn_800E3DC4, fn_800C5B9C | addr |
| lbl_801A63C4 | 0x16 | .rodata | fn_800E3DC4, fn_800C267C | addr |
| lbl_801A63DC | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6434 | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A645C | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A64AC | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A64E8 | 0xD | .rodata | fn_800E3DC4 | addr |
| lbl_801A64F8 | 0x15 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6520 | 0xF | .rodata | fn_800E3DC4 | addr |
| lbl_801A6530 | 0xE | .rodata | fn_800E3DC4 | addr |
| lbl_801A6540 | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6554 | 0x15 | .rodata | fn_800E3DC4 | addr |
| lbl_801A656C | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A65D4 | 0xF | .rodata | fn_800E3DC4 | addr |
| lbl_801A65F8 | 0x15 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6610 | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6628 | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6640 | 0x17 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6658 | 0x15 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6670 | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A66B0 | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A66F0 | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A6718 | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801A672C | 0x15 | .rodata | fn_800E3DC4 | addr |
| lbl_801A676C | 0x16 | .rodata | fn_800E3DC4 | addr |
| lbl_801A67E4 | 0x12 | .rodata | fn_800E3DC4 | addr |
| lbl_801B0680 | 0x224 | .data | fn_80144A88, fn_8000A530 | addr |
| lbl_801B08A4 | 0x224 | .data | fn_8000A530 | addr |
| lbl_801B1DB8 | 0xEC0 | .data | fn_800369C0 | addr |
| lbl_801B2C78 | 0x1014 | .data | fn_800369C0 | addr |
| lbl_801B4AE4 | 0x94C | .data | fn_800369C0 | addr |
| lbl_801B5430 | 0x4BC | .data | fn_800369C0 | addr |
| lbl_801B5E14 | 0x13C8 | .data | fn_800369C0 | addr |
| lbl_801B71DC | 0x1A90 | .data | fn_800369C0 | addr |
| lbl_801D2DD4 | 0x2564 | .data | fn_800EBF2C, fn_800BA87C | addr |
| lbl_801D5359 | 0x6F | .data | fn_800A5DBC | addr |
| lbl_801D5994 | 0x2 | .data | fn_800D3C18, fn_800B87BC, Menu_Unknown5_BuildOptions, fn_800F13BC, fn_8014728C | addr |
| lbl_801D68EC | 0x5154 | .data | fn_801171A0, fn_80137270 | addr |
| lbl_801E1AD5 | 0x5D | .data | fn_801507E0 | addr |
| lbl_801E1B32 | 0x13E | .data | fn_801508CC | addr |
| lbl_801E2F48 | 0x2DC | .data | fn_8015E95C, fn_8015E618 | addr |
| lbl_801E33F8 | 0x990 | .data | fn_8015F5A4 | addr |
| lbl_801F5CC8 | 0x37 | .data | fn_80167AAC | addr |
| lbl_801F5D8C | 0x264 | .data | fn_80168000 | addr |
| lbl_801F6038 | 0xA | .data | fn_801680CC | addr |
| lbl_801F6D80 | 0x1A40 | .data | fn_80172D10 | addr |
| lbl_80207DCC | 0x2024 | .bss | fn_80009C00 | addr |
| lbl_802158A4 | 0x2784 | .bss | fn_80027404 | addr |
| lbl_80293BE0 | 0x59C0 | .bss | fn_80042730 | addr |
| lbl_802B1504 | 0x61 | .bss | fn_8005A834, fn_80059300, fn_8005592C | addr |
| lbl_802C4EE8 | 0x1000F | .bss | fn_800984DC, fn_80098190, fn_8009758C, fn_80098B70 | addr |
| lbl_802D5D94 | 0x4184 | .bss | fn_8009D41C, fn_8009D000, fn_8009D320, fn_8009D0AC, fn_8009D160 | addr |
| lbl_802E0CD8 | 0x28C | .bss | fn_800DC454, fn_800DCB80 | addr |
| lbl_802EAC60 | 0x8820 | .bss | fn_8015057C, fn_80150518, fn_801507CC, fn_80150428, fn_80151510 | addr |
| lbl_802F3684 | 0x45C | .bss | fn_80151510 | addr |
| lbl_802F3BF8 | 0x9F8 | .bss | fn_80163070, fn_80162814, fn_80163554, fn_801623BC, fn_80162654 | addr |
| lbl_8033792C | 0x8BE | .bss | fn_80024F68 | addr |
| lbl_80340588 | 0x274 | .bss | fn_80037DE8, fn_80037DB8, fn_80037DA8, fn_80037C20, fn_80037D3C | addr |
| lbl_803D5718 | 0x29AE | .bss | fn_80075760 | addr |
| lbl_803E03AC | 0x15 | .bss | fn_800E1850 | addr |
| lbl_803E05F0 | 0x7A9C | .bss | fn_800F13BC, fn_800A2100, fn_800F1ECC | addr |
| lbl_8040275C | 0x18E4 | .bss | fn_80149420, fn_8014ABAC, fn_801474EC, fn_8014BBE0, fn_8014B978 | addr |

---
## Stats

| Metric | Value |
|--------|-------|
| GC lbl_* objects | 11768 |
| With any xref | 1414 |
| Absolute lis+addi/load/store xrefs | 3392 |
| SDA (r2) xrefs | 1 |
| SDA2 (r13) xrefs | 0 |
| Total xrefs | 3393 |
| Mac symbols with candidate | 1015 |
| HIGH | 13 |
| MEDIUM | 87 |
| LOW | 24 |
| REJECT | 891 |
