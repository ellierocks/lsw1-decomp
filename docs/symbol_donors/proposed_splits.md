# Proposed Source-file Clusters

Based on 3816 text symbols, 15 named modules.
Call-graph propagation assigned 2259/3816 functions.

---
## Proposed splits.txt entries

Format: `module/path.c:0xADDRESS`

### ai/AIScript

| Address | Size | Functions |
|---------|------|-----------|
| 0x80011864 | 0x164 | 1 funcs: fn_80011864 |
| 0x8001260C | 0x1CC | 1 funcs: fn_8001260C |
| 0x80069C40 | 0x10CC | 2 funcs: fn_80069C40, fn_8006A6C8 |
| 0x80079824 | 0x1CC | 2 funcs: fn_80079824, fn_800798EC |
| 0x80079AC4 | 0x94 | 1 funcs: fn_80079AC4 |
| 0x80079C48 | 0x544 | 8 funcs: fn_80079C48, fn_80079D48, fn_80079E28, fn_80079EBC, fn_80079F68 ... +3 more |
| 0x8007A1E0 | 0xA8 | 1 funcs: fn_8007A1E0 |
| 0x8007BFD8 | 0x208C | 6 funcs: AIScriptParseConditions, fn_8007C534, fn_8007CC34, AIScriptXRefScript, AIScriptL |
| 0x8007E3CC | 0x8EC | 4 funcs: AIScriptLoadScriptTxt, AIScriptResolveReferences, fn_8007E97C, fn_8007EBD0 |
| 0x8007EFA0 | 0x3B4 | 2 funcs: fn_8007EFA0, fn_8007F130 |
| 0x80095038 | 0x98 | 1 funcs: fn_80095038 |
| 0x80096824 | 0x1124 | 2 funcs: fn_80096824, fn_8009758C |
| 0x800ACCC8 | 0x574 | 1 funcs: fn_800ACCC8 |

### game/Credits

| Address | Size | Functions |
|---------|------|-----------|
| 0x8010BDC0 | 0x7B0 | 1 funcs: CreditBufferAlloc |
| 0x8010CB48 | 0x158 | 1 funcs: fn_8010CB48 |

### game/Player

| Address | Size | Functions |
|---------|------|-----------|
| 0x80012514 | 0xF8 | 1 funcs: fn_80012514 |
| 0x8004B10C | 0xBC | 2 funcs: fn_8004B10C, fn_8004B16C |
| 0x800C6900 | 0x1EC | 1 funcs: fn_800C6900 |
| 0x800D149C | 0x4E0 | 2 funcs: fn_800D149C, Player_UpdateState |
| 0x800D2E44 | 0x210 | 1 funcs: fn_800D2E44 |
| 0x80104D38 | 0x6C | 1 funcs: fn_80104D38 |
| 0x80104E24 | 0x90 | 1 funcs: fn_80104E24 |

### game/Podrace

| Address | Size | Functions |
|---------|------|-----------|
| 0x80119B00 | 0x5EC | 1 funcs: fn_80119B00 |
| 0x8012C884 | 0x110 | 1 funcs: fn_8012C884 |
| 0x8014D1F8 | 0x3A8 | 2 funcs: fn_8014D1F8, PodraceCreateMine |
| 0x8014E468 | 0x754 | 3 funcs: PodraceUpdateStartLights, fn_8014E734, fn_8014E9B8 |
| 0x8014ECEC | 0x102C | 7 funcs: fn_8014ECEC, PodraceLoadTimeTrialSettings, PodraceCreateCollapsingMush, fn_8014F |
| 0x8015015C | 0x6C | 1 funcs: fn_8015015C |
| 0x8015022C | 0x16C | 1 funcs: PodraceLoadLapSettings |
| 0x80150428 | 0x3A4 | 3 funcs: fn_80150428, fn_80150518, fn_8015057C |
| 0x80150938 | 0x21C | 4 funcs: fn_80150938, fn_80150978, fn_801509C8, fn_80150A64 |
| 0x80150B9C | 0x1E4 | 3 funcs: fn_80150B9C, fn_80150C3C, fn_80150D64 |
| 0x8015100C | 0xDC | 2 funcs: fn_8015100C, fn_801510BC |
| 0x80151510 | 0x214 | 1 funcs: fn_80151510 |
| 0x801601AC | 0x74 | 1 funcs: fn_801601AC |
| 0x8016189C | 0x13C | 1 funcs: fn_8016189C |
| 0x801623BC | 0x158 | 1 funcs: fn_801623BC |
| 0x80162618 | 0x3C | 1 funcs: fn_80162618 |
| 0x80164170 | 0x174 | 1 funcs: fn_80164170 |
| 0x80164524 | 0x2F8 | 3 funcs: fn_80164524, fn_801645E0, fn_8016470C |
| 0x80164BCC | 0x3A4 | 3 funcs: fn_80164BCC, fn_80164DA0, fn_80164E94 |
| 0x80165FB4 | 0x98 | 1 funcs: fn_80165FB4 |
| ... | | +15 more clusters |

### gx/GX

| Address | Size | Functions |
|---------|------|-----------|
| 0x801470B8 | 0x1D4 | 1 funcs: fn_801470B8 |
| 0x8017CA2C | 0x10DC | 3 funcs: __GXInitRevisionBits, GXInit, __GXInitGX |
| 0x8017DC3C | 0x6C | 1 funcs: GXInitFifoBase |
| 0x8017DD24 | 0x2A0 | 2 funcs: GXSetCPUFifo, GXSetGPFifo |
| 0x8017E008 | 0x4C | 1 funcs: __GXFifoInit |
| 0x8017E130 | 0x100 | 1 funcs: fn_8017E130 |
| 0x8017E240 | 0x10E8 | 11 funcs: fn_8017E240, fn_8017E4AC, fn_8017E734, fn_8017E7F0, fn_8017E914 ... +6 more |
| 0x8017F338 | 0x350 | 3 funcs: fn_8017F338, fn_8017F5B8, GXSetMisc |
| 0x8017FDB0 | 0x74 | 1 funcs: __GXPEInit |
| 0x80180BEC | 0x1C | 1 funcs: fn_80180BEC |
| 0x80181A34 | 0x174 | 3 funcs: GXInitTexCacheRegion, GXInitTlutRegion, fn_80181B60 |
| 0x80181C70 | 0x53C | 3 funcs: fn_80181C70, __GXSetTmemConfig, fn_80182140 |
| 0x80182324 | 0x230 | 2 funcs: fn_80182324, fn_80182468 |
| 0x80182628 | 0xE0 | 3 funcs: __GXSetIndirectMask, __GXFlushTextureState, fn_8018267C |
| 0x8018274C | 0x44 | 1 funcs: fn_8018274C |
| 0x80183070 | 0x54 | 1 funcs: fn_80183070 |
| 0x80187D3C | 0x5E0 | 2 funcs: fn_80187D3C, fn_80187EEC |
| 0x80189E94 | 0x278 | 2 funcs: fn_80189E94, fn_80189FFC |

### menu/DebugMenu

| Address | Size | Functions |
|---------|------|-----------|
| 0x80084128 | 0x34 | 1 funcs: fn_80084128 |
| 0x800AB85C | 0x6C | 1 funcs: fn_800AB85C |
| 0x800AFEEC | 0x5C | 2 funcs: DebugMenu_ToggleOption1, DebugMenu_ToggleOption2 |
| 0x800B14B8 | 0x6BC | 2 funcs: DebugMenu_HandleSelection, DebugMenu_BuildOptions |
| 0x800CC7D0 | 0x60 | 1 funcs: DebugMenu_HandleEntry1 |
| 0x80103D18 | 0xA8 | 1 funcs: fn_80103D18 |
| 0x80105500 | 0x1B0 | 1 funcs: fn_80105500 |
| 0x80105DEC | 0x5AC | 3 funcs: fn_80105DEC, fn_80106128, fn_8010624C |
| 0x80107740 | 0xE8 | 1 funcs: DebugMenu_HandleEntry2 |
| 0x8010902C | 0xFC | 1 funcs: DebugMenu_HandleEntry3 |
| 0x80141C44 | 0x60 | 1 funcs: DebugMenu_HandleEntry4 |

### menu/MenuSystem

| Address | Size | Functions |
|---------|------|-----------|
| 0x8000BF70 | 0x64 | 1 funcs: fn_8000BF70 |
| 0x80038058 | 0x1B8 | 1 funcs: fn_80038058 |
| 0x80043050 | 0x2D8 | 2 funcs: fn_80043050, fn_80043128 |
| 0x8004BC1C | 0x94 | 1 funcs: fn_8004BC1C |
| 0x80053860 | 0x510 | 1 funcs: fn_80053860 |
| 0x80066294 | 0xF8 | 2 funcs: fn_80066294, fn_80066328 |
| 0x800663D8 | 0x18 | 1 funcs: fn_800663D8 |
| 0x80069ACC | 0x174 | 1 funcs: fn_80069ACC |
| 0x8006DF1C | 0x34 | 1 funcs: fn_8006DF1C |
| 0x8006E3E0 | 0x760 | 1 funcs: fn_8006E3E0 |
| 0x8006EB98 | 0x2C | 1 funcs: fn_8006EB98 |
| 0x8006ECF0 | 0x68C | 4 funcs: fn_8006ECF0, fn_8006F0BC, fn_8006F1D8, fn_8006F28C |
| 0x80070C0C | 0xAA4 | 4 funcs: fn_80070C0C, fn_80071128, fn_800712E4, fn_80071668 |
| 0x800718FC | 0x60 | 1 funcs: fn_800718FC |
| 0x80071AEC | 0x2A4 | 5 funcs: fn_80071AEC, fn_80071B78, fn_80071C04, fn_80071CC8, fn_80071D08 |
| 0x80071DA0 | 0x6C | 1 funcs: fn_80071DA0 |
| 0x80071E9C | 0x18C | 1 funcs: fn_80071E9C |
| 0x80075B44 | 0x134 | 1 funcs: fn_80075B44 |
| 0x80075D00 | 0x494 | 2 funcs: Menu_Open, Menu_CloseCurrent |
| 0x800762B0 | 0x8E4 | 2 funcs: Menu_OpenFromRegistry, fn_800769B4 |
| ... | | +128 more clusters |

### menu/PauseMenu

| Address | Size | Functions |
|---------|------|-----------|
| 0x8006B1E0 | 0x148 | 2 funcs: fn_8006B1E0, PauseMenu_HandleEntry1 |
| 0x80083EFC | 0x50 | 1 funcs: fn_80083EFC |
| 0x800AC444 | 0xF0 | 1 funcs: PauseMenu_HandleEntry2 |
| 0x800B2414 | 0x2A8 | 2 funcs: PauseMenu_HandleSelection, PauseMenu_BuildOptions |

### nu2/NuAnimation

| Address | Size | Functions |
|---------|------|-----------|
| 0x80016A98 | 0xA94 | 3 funcs: fn_80016A98, fn_80016C84, NuAnimCurve2CalcVal |
| 0x80019F08 | 0x28 | 1 funcs: fn_80019F08 |
| 0x80027C58 | 0xA8 | 1 funcs: fn_80027C58 |

### nu2/NuData

| Address | Size | Functions |
|---------|------|-----------|
| 0x800044A8 | 0x105C | 5 funcs: NuDatFileLoadBufferLsn, NuDatFileLoadBuffer, fn_80004BA4, NuDatFileOpen, NuDatOp |
| 0x8000556C | 0x370 | 6 funcs: fn_8000556C, fn_800055EC, fn_800056E0, fn_80005764, fn_8000584C ... +1 more |
| 0x80005F5C | 0xDC | 3 funcs: fn_80005F5C, fn_80005FF4, fn_80006010 |
| 0x800061D8 | 0x1C0 | 1 funcs: fn_800061D8 |
| 0x80006ED0 | 0xA4 | 2 funcs: fn_80006ED0, fn_80006F44 |
| 0x800091A8 | 0x148 | 1 funcs: fn_800091A8 |
| 0x80009324 | 0x1A0 | 2 funcs: fn_80009324, fn_80009408 |
| 0x8000965C | 0xEC | 1 funcs: fn_8000965C |
| 0x80009A28 | 0x46C | 3 funcs: fn_80009A28, fn_80009C00, fn_80009D40 |
| 0x8000A64C | 0x114 | 4 funcs: fn_8000A64C, fn_8000A714, fn_8000A724, fn_8000A754 |
| 0x8000AE18 | 0xAC | 1 funcs: fn_8000AE18 |
| 0x8000D010 | 0x124 | 1 funcs: fn_8000D010 |
| 0x8000D95C | 0x244 | 1 funcs: fn_8000D95C |
| 0x8000DD40 | 0x90 | 1 funcs: fn_8000DD40 |
| 0x8000E918 | 0xA4 | 1 funcs: fn_8000E918 |
| 0x8000EB24 | 0x20 | 1 funcs: fn_8000EB24 |
| 0x8000F65C | 0x1E4 | 1 funcs: fn_8000F65C |
| 0x8000F9FC | 0x31C | 1 funcs: fn_8000F9FC |
| 0x8001459C | 0x110 | 1 funcs: fn_8001459C |
| 0x80015D60 | 0x70 | 1 funcs: fn_80015D60 |
| ... | | +16 more clusters |

### nu2/NuFile

| Address | Size | Functions |
|---------|------|-----------|
| 0x800034A0 | 0x1008 | 8 funcs: fn_800034A0, NuFileOpen, NuFilePos, fn_80003AEC, NuFileClose ... +3 more |
| 0x800059A0 | 0x324 | 4 funcs: fn_800059A0, fn_80005A4C, NuFileEndBlockRead, fn_80005BC0 |
| 0x80005E80 | 0xDC | 1 funcs: fn_80005E80 |
| 0x80006398 | 0xA6C | 7 funcs: fn_80006398, fn_80006830, fn_800068F4, fn_800069F4, fn_80006B14 ... +2 more |
| 0x80008E78 | 0x330 | 1 funcs: fn_80008E78 |
| 0x800094C4 | 0x198 | 2 funcs: fn_800094C4, fn_80009564 |
| 0x80009748 | 0x110 | 1 funcs: fn_80009748 |
| 0x80009984 | 0x14 | 1 funcs: fn_80009984 |
| 0x80009E94 | 0x53C | 2 funcs: fn_80009E94, fn_8000A268 |
| 0x8000A40C | 0xF4 | 1 funcs: fn_8000A40C |
| 0x8000A530 | 0xD4 | 1 funcs: fn_8000A530 |
| 0x8000A788 | 0xAC | 1 funcs: fn_8000A788 |
| 0x8000A86C | 0x38 | 1 funcs: fn_8000A86C |
| 0x8000A8AC | 0x56C | 4 funcs: fn_8000A8AC, fn_8000A94C, fn_8000A9F0, fn_8000ABDC |
| 0x8000AEC4 | 0xF4 | 1 funcs: fn_8000AEC4 |
| 0x8000AFBC | 0x50 | 1 funcs: fn_8000AFBC |
| 0x8000B080 | 0xC30 | 11 funcs: fn_8000B080, fn_8000B144, fn_8000B20C, fn_8000B234, fn_8000B264 ... +6 more |
| 0x8000BD10 | 0xC0 | 1 funcs: fn_8000BD10 |
| 0x8000C1B8 | 0x47C | 1 funcs: fn_8000C1B8 |
| 0x8000C844 | 0x1D8 | 4 funcs: fn_8000C844, fn_8000C8BC, fn_8000C934, fn_8000C9AC |
| ... | | +470 more clusters |

### nu2/NuMemory

| Address | Size | Functions |
|---------|------|-----------|
| 0x80006E1C | 0xB4 | 1 funcs: fn_80006E1C |
| 0x80006F74 | 0x620 | 3 funcs: NuMemAlloc, NuMemFree, fn_80007468 |
| 0x800076E4 | 0x448 | 3 funcs: fn_800076E4, fn_800077E8, fn_80007900 |
| 0x80007B6C | 0x3C | 1 funcs: fn_80007B6C |
| 0x80007BFC | 0x28 | 1 funcs: fn_80007BFC |
| 0x8000A604 | 0x48 | 1 funcs: fn_8000A604 |
| 0x8000D4DC | 0x398 | 1 funcs: fn_8000D4DC |
| 0x8000E6B0 | 0xBC | 1 funcs: fn_8000E6B0 |
| 0x8000E830 | 0xE8 | 2 funcs: fn_8000E830, fn_8000E8A4 |
| 0x8000F2B8 | 0x1D0 | 1 funcs: fn_8000F2B8 |
| 0x80010120 | 0x156C | 5 funcs: fn_80010120, fn_80010C30, fn_80010EF8, fn_80011070, fn_80011214 |
| 0x80012420 | 0xF4 | 1 funcs: fn_80012420 |
| 0x80012980 | 0x9C | 1 funcs: fn_80012980 |
| 0x80013350 | 0x30 | 1 funcs: fn_80013350 |
| 0x80018DA8 | 0xBC | 1 funcs: fn_80018DA8 |
| 0x80018F78 | 0x2D0 | 1 funcs: fn_80018F78 |
| 0x80019634 | 0x184 | 1 funcs: fn_80019634 |
| 0x80019A54 | 0x4B4 | 1 funcs: fn_80019A54 |
| 0x80021784 | 0x1664 | 3 funcs: fn_80021784, fn_800225C4, fn_80022A88 |
| 0x80025C38 | 0x1B4 | 1 funcs: fn_80025C38 |
| ... | | +36 more clusters |

### runtime/Memory

| Address | Size | Functions |
|---------|------|-----------|
| 0x800A2498 | 0x604 | 1 funcs: fn_800A2498 |
| 0x800A2D48 | 0x24C | 1 funcs: fn_800A2D48 |
| 0x8014BEB0 | 0x13C | 1 funcs: fn_8014BEB0 |
| 0x801503A0 | 0x78 | 1 funcs: fn_801503A0 |
| 0x80155D54 | 0x0 | 1 funcs: MemZero |
| 0x8015CA40 | 0x54 | 1 funcs: fn_8015CA40 |
| 0x8015CE70 | 0x34C | 2 funcs: fn_8015CE70, fn_8015CF8C |
| 0x8015DA0C | 0x1F8 | 2 funcs: fn_8015DA0C, fn_8015DAA4 |
| 0x8015DF44 | 0x8 | 1 funcs: fn_8015DF44 |
| 0x8015E224 | 0x2A8 | 1 funcs: fn_8015E224 |
| 0x8015E550 | 0x48 | 1 funcs: fn_8015E550 |
| 0x8015EDCC | 0xA8 | 3 funcs: fn_8015EDCC, fn_8015EDF4, fn_8015EE08 |
| 0x8015EEC0 | 0x858 | 4 funcs: fn_8015EEC0, fn_8015EF84, fn_8015F408, fn_8015F5A4 |
| 0x8015F8F0 | 0x324 | 1 funcs: fn_8015F8F0 |
| 0x8015FD54 | 0x13C | 1 funcs: fn_8015FD54 |
| 0x801604F8 | 0x4A4 | 4 funcs: fn_801604F8, fn_80160580, fn_80160608, fn_8016094C |
| 0x80160ADC | 0x184 | 3 funcs: fn_80160ADC, fn_80160BB8, fn_80160BF4 |
| 0x80160CE0 | 0x80 | 1 funcs: fn_80160CE0 |
| 0x80161118 | 0x280 | 3 funcs: fn_80161118, fn_80161160, fn_80161360 |
| 0x80161F48 | 0x3EC | 8 funcs: fn_80161F48, fn_80161FB4, fn_80162038, fn_801620E4, fn_80162154 ... +3 more |
| ... | | +28 more clusters |

### runtime/PPC

| Address | Size | Functions |
|---------|------|-----------|
| 0x8015BFC0 | 0x18 | 3 funcs: PPCMfhid2, PPCMthid2, PPCMtwpar |

### ui/UI

| Address | Size | Functions |
|---------|------|-----------|
| 0x8015AA00 | 0x0 | 1 funcs: UI_DrawText |
| 0x8015BF70 | 0x8 | 1 funcs: fn_8015BF70 |
| 0x8015BFD8 | 0x28 | 1 funcs: fn_8015BFD8 |
| 0x801619D8 | 0x3F4 | 3 funcs: fn_801619D8, fn_80161A34, fn_80161A90 |


---
## Summary

| Module | Clusters | Total Funcs |
|--------|----------|-------------|
| ai/AIScript | 13 | 32 |
| game/Credits | 2 | 2 |
| game/Player | 7 | 9 |
| game/Podrace | 35 | 70 |
| gx/GX | 18 | 42 |
| menu/DebugMenu | 11 | 15 |
| menu/MenuSystem | 148 | 271 |
| menu/PauseMenu | 4 | 6 |
| nu2/NuAnimation | 3 | 5 |
| nu2/NuData | 36 | 60 |
| nu2/NuFile | 490 | 1562 |
| nu2/NuMemory | 56 | 83 |
| runtime/Memory | 48 | 93 |
| runtime/PPC | 1 | 3 |
| ui/UI | 4 | 6 |
| _unknown | - | 1557 |
