# Call Graph Rename Suggestions

Extracted 30596 call sites from 3816 functions.
490 unnamed functions have named call context.

---
## Suggestions (by named callee count descending)

| Function | Size | Named Callers | Named Callees | Suggested Context |
|----------|------|---------------|---------------|-------------------|
| fn_80151510 | 0x214 | MemZero, Menu_AddOption | PodraceLoadSplineSettings, PodraceLoadTimeTrialSettings, __G | Menu(2) |
| fn_800040E0 | 0x2B0 | NuFileOpen, NuFileClose, NuFileLoadBuffer | NuFileBeginBlkRead, NuDatFileLoadBuffer | Nu2(5) |
| fn_80003AEC | 0x148 | NuFileOpen, NuFilePos | NuFileLoadBuffer, NuFileClose | Nu2(4) |
| fn_8006A6C8 | 0x644 |  | AIScriptXRefScript, AIScriptResolveReferences, AIScriptParse | AIScript(4) |
| fn_800A76AC | 0x1700 |  | MemZero, PPCMtwpar, Menu_AddOption, UI_DrawText | Menu(1) |
| fn_800B7870 | 0x700 |  | Menu_Unknown4_HandleSelection, Menu_Unknown4_BuildOptions, M | Menu(4) |
| fn_800ED648 | 0x19B8 | Menu_ResetAndReturn, Menu_FullReset | MemZero, Menu_OpenFromRegistry | Menu(3) |
| fn_8010CD84 | 0x6564 | GXInit, CreditBufferAlloc, Menu_InitDefinitions, __GXFifoIni |  | Menu(1) |
| fn_8014F058 | 0x218 | PodraceLoadTimeTrialSettings, MemZero | PodraceLoadSplineSettings, PodraceInitBoost |  |
| fn_80162814 | 0x228 | Menu_Unknown14_HandleSelection, MemZero | PodraceLoadLapSettings, MemZero | Menu(1) |
| fn_8018C51C | 0x5E0 | Player_UpdateState, __GXInitGX, GXInit | Menu_Unknown3_HandleSelection | Player(1), Menu(1) |
| fn_80006398 | 0x498 | Menu_ResetAndReturn | NuMemAlloc, NuMemFree | Nu2(2), Menu(1) |
| fn_800077E8 | 0x118 | NuDatOpenEx, NuMemAlloc, NuMemFree |  | Nu2(3) |
| fn_8001752C | 0x974 | NuDatFileOpen, NuDatOpenEx, NuAnimCurve2CalcVal |  | Nu2(3) |
| fn_80017EA0 | 0x6B8 | NuAnimCurve2CalcVal, NuDatFileLoadBuffer | NuSpecialFind | Nu2(3) |
| fn_8001A2D8 | 0xC88 | NuFileLoadBuffer, NuDatOpenEx, NuFileClose |  | Nu2(3) |
| fn_80069C40 | 0xA88 |  | AIScriptResolveReferences, AIScriptLoadScriptTxt, Menu_Navig | AIScript(2), Menu(1) |
| fn_8007373C | 0xC38 | Menu_OpenFromRegistry | Menu_Open, Menu_OpenFromRegistry | Menu(3) |
| fn_80078B5C | 0x1AC | AIScriptXRefScript | Menu_RegisterDescriptors, Menu_ResetStack | Menu(2), AIScript(1) |
| fn_8009D5D0 | 0x408 | Menu_ResetAndReturn | NuAnimCurve2CalcVal, __GXSetTmemConfig | Menu(1), Nu2(1) |
| fn_800AD4C4 | 0x28FC |  | Menu_Unknown4_BuildOptions, Menu_Unknown4_HandleSelection, M | Menu(3) |
| fn_800B6D94 | 0xADC |  | DebugMenu_BuildOptions, Menu_Unknown1_HandleSelection, Menu_ | Menu(2) |
| fn_800DD1C8 | 0x1F08 | NuFilePos, NuFileClose, NuFileOpen |  | Nu2(3) |
| fn_801177C0 | 0xCB8 | Menu_Unknown3_HandleSelection | Menu_SetupDisplay, Menu_AddOption | Menu(3) |
| fn_8013FCA0 | 0x71C | __GXInitGX, Menu_Unknown12_BuildOptions, Menu_FullReset |  | Menu(2) |
| fn_8015057C | 0x250 |  | MemZero, Menu_AddOption, PodraceUpdateMine | Menu(1) |
| fn_80162D74 | 0x1BC | PodraceInitBoost | PodraceUpdateMine, MemZero |  |
| fn_8016E6A8 | 0x32C | Menu_FullReset | CreditBufferAlloc, PodraceLoadLapSettings | Menu(1) |
| fn_80186D18 | 0xA2C | GXInit | GXSetGPFifo, GXSetCPUFifo |  |
| fn_80004BA4 | 0x154 | NuDatFileLoadBuffer | NuDatFileOpen | Nu2(2) |
| fn_80005F5C | 0x98 | NuDatFileLoadBufferLsn, NuDatFileLoadBuffer |  | Nu2(2) |
| fn_80007900 | 0x22C | NuMemAlloc, NuFileClose |  | Nu2(2) |
| fn_800081CC | 0xD8 | NuPPLoadBuffer | NuPPLoadBuffer | Nu2(2) |
| fn_80009E94 | 0x3D4 | NuDatFileLoadBuffer | NuPPLoadBuffer | Nu2(2) |
| fn_8000C1B8 | 0x47C | NuFileLoadBuffer | NuAnimCurve2CalcVal | Nu2(2) |
| fn_8000D4DC | 0x398 | NuPPLoadBuffer, NuMemFree |  | Nu2(2) |
| fn_800151CC | 0xA04 | NuFileEndBlockRead | NuAnimCurve2CalcVal | Nu2(2) |
| fn_800187F4 | 0x27C | NuAnimCurve2CalcVal, NuDatFileLoadBuffer |  | Nu2(2) |
| fn_80019248 | 0x3EC | NuDatFileLoadBufferLsn, NuDatFileLoadBuffer |  | Nu2(2) |
| fn_8001B22C | 0x5BC | NuFilePos, NuFileClose |  | Nu2(2) |
| fn_8001B7E8 | 0x540 | NuDatFileOpen, NuFileOpen |  | Nu2(2) |
| fn_8001BD70 | 0x124 | NuFileOpen, NuDatFileLoadBuffer |  | Nu2(2) |
| fn_8001C20C | 0x848 | NuFileOpen, NuDatFileLoadBuffer |  | Nu2(2) |
| fn_80072410 | 0x900 | DebugMenu_HandleSelection, Menu_OpenSpecial |  | Menu(1) |
| fn_8007C534 | 0x370 | AIScriptParseConditions | AIScriptXRefScript | AIScript(2) |
| fn_8007CC34 | 0x220 |  | AIScriptXRefScript, AIScriptLoadScp | AIScript(2) |
| fn_8007D550 | 0xB14 | AIScriptXRefScript, AIScriptLoadScp |  | AIScript(2) |
| fn_8007E97C | 0x254 | AIScriptResolveReferences | AIScriptLoadScp | AIScript(2) |
| fn_8007F354 | 0x554 |  | AIScriptLoadScp, Menu_Unknown3_BuildOptions | AIScript(1), Menu(1) |
| fn_8009AFE0 | 0x12C4 | Menu_FullReset | Menu_ResetAndReturn | Menu(2) |
| fn_8009FB78 | 0x728 | Menu_ResetAndReturn | Menu_FullReset | Menu(2) |
| fn_800A41D8 | 0x3E4 |  | Menu_ResetAndReturn, Menu_FullReset | Menu(2) |
| fn_800B203C | 0x3D8 |  | Menu_Unknown3_BuildOptions, PauseMenu_HandleSelection | Menu(1) |
| fn_800B4DF0 | 0x9EC |  | Menu_Unknown9_Enter, Menu_FullReset | Menu(2) |
| fn_800DA478 | 0x1968 | NuDatFileLoadBuffer, Menu_FullReset |  | Nu2(1), Menu(1) |
| fn_800DCB80 | 0x444 | NuFileLoadBuffer, Menu_FullReset |  | Nu2(1), Menu(1) |
| fn_800F5C34 | 0xCA4 | GXSetGPFifo, GXSetCPUFifo |  |  |
| fn_800F68D8 | 0x3C0 |  | NuFileLoadBuffer, NuFilePos | Nu2(2) |
| fn_80105500 | 0x1B0 | Menu_FullReset | DebugMenu_HandleEntry2 | Menu(1) |
| fn_80106398 | 0x494 | GXSetGPFifo, __GXFifoInit |  |  |
| fn_80107DA4 | 0x7F8 | NuFileOpen, Menu_FullReset |  | Nu2(1), Menu(1) |
| fn_8012D23C | 0x1E4 | PauseMenu_HandleSelection | Menu_OpenFromRegistry | Menu(1) |
| fn_80137D6C | 0x34 | Menu_InitDefinitions | Menu_AddOption | Menu(2) |
| fn_8014C20C | 0x51C | PodraceUpdateStartLights, MemZero |  |  |
| fn_8014CAE8 | 0x4A4 | MemZero | PodraceCreateMine |  |
| fn_801503A0 | 0x78 | PodraceLoadLapSettings, MemZero |  |  |
| fn_801509C8 | 0x9C | PodraceUpdateMine | PodraceCreateCollapsingMush |  |
| fn_8015C194 | 0x4E0 | UI_DrawText | UI_DrawText |  |
| fn_8015CF8C | 0x230 |  | MemZero, UI_DrawText |  |
| fn_80160E90 | 0x70 |  | MemZero, Menu_AddOption | Menu(1) |
| fn_80161160 | 0x200 | MemZero | MemZero |  |
| fn_80161FB4 | 0x84 | MemZero | MemZero |  |
| fn_80162354 | 0x64 | MemZero | PodraceUpdateMine |  |
| fn_80162F30 | 0x140 | MemZero | MemZero |  |
| fn_80163554 | 0x104 | OSRegisterVersion | MemZero |  |
| fn_80163984 | 0xF4 |  | MemZero, Menu_AddOption | Menu(1) |
| fn_80163A78 | 0x25C |  | MemZero, Menu_AddOption | Menu(1) |
| fn_80163E60 | 0x24C | Menu_Unknown14_BuildOptions | Menu_AddOption | Menu(2) |
| fn_80164418 | 0x10C | Menu_AddOption | Menu_AddOption | Menu(2) |
| fn_80164AFC | 0xD0 | Menu_AddOption, Menu_Unknown14_Close |  | Menu(2) |
| fn_80166A70 | 0x298 | MemZero | MemZero |  |
| fn_801696B0 | 0x658 | Menu_AddOption, PodraceInitBoost |  | Menu(1) |
| fn_8016A2B0 | 0x27C | Menu_AddOption | DebugMenu_HandleEntry4 | Menu(1) |
| fn_8016E5D0 | 0xD8 |  | CreditBufferAlloc, PodraceUpdateStartLights |  |
| fn_8017B5D8 | 0xB58 | Menu_NavigateForward, GXInit |  | Menu(1) |
| fn_8017C1A0 | 0x324 | Menu_FullReset | __GXInitRevisionBits | Menu(1) |
| fn_8017C89C | 0x190 |  | GXInit, __GXInitRevisionBits |  |
| fn_8017E130 | 0x100 |  | __GXInitGX, GXInit |  |
| fn_801854C4 | 0x538 | __GXInitGX, Menu_Unknown4_HandleSelection |  | Menu(1) |
| fn_80187EEC | 0x430 | __GXInitGX | __GXSetTmemConfig |  |
| fn_80188C74 | 0x510 | GXInit | GXInit |  |
| fn_800034A0 | 0x150 | NuFileClose |  | Nu2(1) |
| fn_8000556C | 0x80 | NuDatOpenEx |  | Nu2(1) |
| fn_800055EC | 0xF4 | NuDatOpenEx |  | Nu2(1) |
| fn_800056E0 | 0x84 | NuDatOpenEx |  | Nu2(1) |
| fn_800059A0 | 0xAC |  | NuFileEndBlockRead | Nu2(1) |
| fn_80005A4C | 0xD0 |  | NuFileEndBlockRead | Nu2(1) |
| fn_80005E80 | 0xDC | NuFileLoadBuffer |  | Nu2(1) |
| fn_800061D8 | 0x1C0 |  | NuFileEndBlockRead | Nu2(1) |
| fn_80006E1C | 0xB4 |  | NuMemAlloc | Nu2(1) |

---
## Stats

| Metric | Value |
|--------|-------|
| Total functions | 3816 |
| Named | 119 |
| Unnamed | 3697 |
| Unnamed with named context | 490 |
| bl instructions | 30596 |
