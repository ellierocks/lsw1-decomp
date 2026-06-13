# Filtered Call Graph Rename Candidates

Total unnamed with named context: 914
HIGH (score≥7): 58 candidates
MEDIUM (score 4-6): 752 candidates
LOW (score<4): 104 candidates

Score = dispatch_callers*3 + dispatch_callees*2 + named_caller + named_callee

## HIGH Confidence

| Score | Function | Ctx | DispCallers | DispCallees | NamedCallers | NamedCallees | Named Callers | Named Callees |
|-------|----------|-----|-------------|-------------|--------------|--------------|---------------|---------------|
| 25 | fn_80151510 | Menu, Podrace | 1 | 6 | 2 | 8 | MemZero, Menu_AddOption | MemZero, Menu_AddOption, PodraceCreateMine, PodraceLoadLapSettings, PodraceLoadSplineSettings |
| 18 | fn_800040E0 | Nu | 3 | 2 | 3 | 2 | NuFileClose, NuFileLoadBuffer, NuFileOpen | NuDatFileLoadBuffer, NuFileBeginBlkRead |
| 14 | fn_80003AEC | Nu | 2 | 2 | 2 | 2 | NuFileOpen, NuFilePos | NuFileClose, NuFileLoadBuffer |
| 12 | fn_800ED648 | Menu | 2 | 1 | 2 | 2 | Menu_FullReset, Menu_ResetAndReturn | MemZero, Menu_OpenFromRegistry |
| 12 | fn_800DD1C8 | Nu | 3 | 0 | 3 | 0 | NuFileClose, NuFileOpen, NuFilePos |  |
| 12 | fn_800B7870 | Menu | 0 | 4 | 0 | 4 |  | Menu_Main_HandleSelection, Menu_Unknown1_HandleSelection, Menu_Unknown4_BuildOptions, Menu_Unknown4_HandleSelection |
| 12 | fn_8006A6C8 | AIScript | 0 | 4 | 0 | 4 |  | AIScriptLoadScp, AIScriptParseConditions, AIScriptResolveReferences, AIScriptXRefScript |
| 12 | fn_8001A2D8 | Nu | 3 | 0 | 3 | 0 | NuDatOpenEx, NuFileClose, NuFileLoadBuffer |  |
| 12 | fn_8001752C | Nu | 3 | 0 | 3 | 0 | NuAnimCurve2CalcVal, NuDatFileOpen, NuDatOpenEx |  |
| 12 | fn_800077E8 | Nu | 3 | 0 | 3 | 0 | NuDatOpenEx, NuMemAlloc, NuMemFree |  |
| 11 | fn_8014F058 | Podrace | 1 | 2 | 2 | 2 | MemZero, PodraceLoadTimeTrialSettings | PodraceInitBoost, PodraceLoadSplineSettings |
| 11 | fn_80017EA0 | Nu | 2 | 1 | 2 | 1 | NuAnimCurve2CalcVal, NuDatFileLoadBuffer | NuSpecialFind |
| 10 | fn_801177C0 | Menu | 1 | 2 | 1 | 2 | Menu_Unknown3_HandleSelection | Menu_AddOption, Menu_SetupDisplay |
| 10 | fn_80078B5C | AIScript, Menu | 1 | 2 | 1 | 2 | AIScriptXRefScript | Menu_RegisterDescriptors, Menu_ResetStack |
| 10 | fn_8007373C | Menu | 1 | 2 | 1 | 2 | Menu_OpenFromRegistry | Menu_Open, Menu_OpenFromRegistry |
| 10 | fn_80006398 | Menu, Nu | 1 | 2 | 1 | 2 | Menu_ResetAndReturn | NuMemAlloc, NuMemFree |
| 9 | fn_80162814 | Menu, Podrace | 1 | 1 | 2 | 2 | MemZero, Menu_Unknown14_HandleSelection | MemZero, PodraceLoadLapSettings |
| 9 | fn_8013FCA0 | Menu | 2 | 0 | 3 | 0 | Menu_FullReset, Menu_Unknown12_BuildOptions, __GXInitGX |  |
| 9 | fn_800B6D94 | DebugMenu, Menu | 0 | 3 | 0 | 3 |  | DebugMenu_BuildOptions, Menu_Unknown1_HandleSelection, Menu_Unknown9_BuildOptions |
| 9 | fn_800AD4C4 | Menu | 0 | 3 | 0 | 3 |  | Menu_Unknown1_HandleSelection, Menu_Unknown4_BuildOptions, Menu_Unknown4_HandleSelection |
| 9 | fn_80069C40 | AIScript, Menu | 0 | 3 | 0 | 3 |  | AIScriptLoadScriptTxt, AIScriptResolveReferences, Menu_Navigate |
| 8 | fn_8016E6A8 | Menu, Podrace | 1 | 1 | 1 | 2 | Menu_FullReset | CreditBufferAlloc, PodraceLoadLapSettings |
| 8 | fn_801696B0 | Menu, Podrace | 2 | 0 | 2 | 0 | Menu_AddOption, PodraceInitBoost |  |
| 8 | fn_80164AFC | Menu | 2 | 0 | 2 | 0 | Menu_AddOption, Menu_Unknown14_Close |  |
| 8 | fn_80162D74 | Podrace | 1 | 1 | 1 | 2 | PodraceInitBoost | MemZero, PodraceUpdateMine |
| 8 | fn_80107DA4 | Menu, Nu | 2 | 0 | 2 | 0 | Menu_FullReset, NuFileOpen |  |
| 8 | fn_800DCB80 | Menu, Nu | 2 | 0 | 2 | 0 | Menu_FullReset, NuFileLoadBuffer |  |
| 8 | fn_800DA478 | Menu, Nu | 2 | 0 | 2 | 0 | Menu_FullReset, NuDatFileLoadBuffer |  |
| 8 | fn_8009D5D0 | Menu, Nu | 1 | 1 | 1 | 2 | Menu_ResetAndReturn | NuAnimCurve2CalcVal, __GXSetTmemConfig |
| 8 | fn_8007D550 | AIScript | 2 | 0 | 2 | 0 | AIScriptLoadScp, AIScriptXRefScript |  |
| 8 | fn_80072410 | DebugMenu, Menu | 2 | 0 | 2 | 0 | DebugMenu_HandleSelection, Menu_OpenSpecial |  |
| 8 | fn_8001C20C | Nu | 2 | 0 | 2 | 0 | NuDatFileLoadBuffer, NuFileOpen |  |
| 8 | fn_8001BD70 | Nu | 2 | 0 | 2 | 0 | NuDatFileLoadBuffer, NuFileOpen |  |
| 8 | fn_8001B7E8 | Nu | 2 | 0 | 2 | 0 | NuDatFileOpen, NuFileOpen |  |
| 8 | fn_8001B22C | Nu | 2 | 0 | 2 | 0 | NuFileClose, NuFilePos |  |
| 8 | fn_80019248 | Nu | 2 | 0 | 2 | 0 | NuDatFileLoadBuffer, NuDatFileLoadBufferLsn |  |
| 8 | fn_800187F4 | Nu | 2 | 0 | 2 | 0 | NuAnimCurve2CalcVal, NuDatFileLoadBuffer |  |
| 8 | fn_8000D4DC | Nu | 2 | 0 | 2 | 0 | NuMemFree, NuPPLoadBuffer |  |
| 8 | fn_80007900 | Nu | 2 | 0 | 2 | 0 | NuFileClose, NuMemAlloc |  |
| 8 | fn_80005F5C | Nu | 2 | 0 | 2 | 0 | NuDatFileLoadBuffer, NuDatFileLoadBufferLsn |  |
| 7 | fn_8016A2B0 | DebugMenu, Menu | 1 | 1 | 1 | 1 | Menu_AddOption | DebugMenu_HandleEntry4 |
| 7 | fn_80164418 | Menu | 1 | 1 | 1 | 1 | Menu_AddOption | Menu_AddOption |
| 7 | fn_80163E60 | Menu | 1 | 1 | 1 | 1 | Menu_Unknown14_BuildOptions | Menu_AddOption |
| 7 | fn_801509C8 | Podrace | 1 | 1 | 1 | 1 | PodraceUpdateMine | PodraceCreateCollapsingMush |
| 7 | fn_8015057C | Menu, Podrace | 0 | 2 | 0 | 3 |  | MemZero, Menu_AddOption, PodraceUpdateMine |
| 7 | fn_80137D6C | Menu | 1 | 1 | 1 | 1 | Menu_InitDefinitions | Menu_AddOption |
| 7 | fn_8012D23C | Menu, PauseMenu | 1 | 1 | 1 | 1 | PauseMenu_HandleSelection | Menu_OpenFromRegistry |
| 7 | fn_8010CD84 | Menu | 1 | 0 | 4 | 0 | CreditBufferAlloc, GXInit, Menu_InitDefinitions, __GXFifoInit |  |
| 7 | fn_80105500 | DebugMenu, Menu | 1 | 1 | 1 | 1 | Menu_FullReset | DebugMenu_HandleEntry2 |
| 7 | fn_8009FB78 | Menu | 1 | 1 | 1 | 1 | Menu_ResetAndReturn | Menu_FullReset |
| 7 | fn_8009AFE0 | Menu | 1 | 1 | 1 | 1 | Menu_FullReset | Menu_ResetAndReturn |
| 7 | fn_8007E97C | AIScript | 1 | 1 | 1 | 1 | AIScriptResolveReferences | AIScriptLoadScp |
| 7 | fn_8007C534 | AIScript | 1 | 1 | 1 | 1 | AIScriptParseConditions | AIScriptXRefScript |
| 7 | fn_800151CC | Nu | 1 | 1 | 1 | 1 | NuFileEndBlockRead | NuAnimCurve2CalcVal |
| 7 | fn_8000C1B8 | Nu | 1 | 1 | 1 | 1 | NuFileLoadBuffer | NuAnimCurve2CalcVal |
| 7 | fn_80009E94 | Nu | 1 | 1 | 1 | 1 | NuDatFileLoadBuffer | NuPPLoadBuffer |
| 7 | fn_800081CC | Nu | 1 | 1 | 1 | 1 | NuPPLoadBuffer | NuPPLoadBuffer |
| 7 | fn_80004BA4 | Nu | 1 | 1 | 1 | 1 | NuDatFileLoadBuffer | NuDatFileOpen |

## MEDIUM Confidence

| Score | Function | Ctx | DispCallers | DispCallees | NamedCallers | NamedCallees | Named Callers | Named Callees |
|-------|----------|-----|-------------|-------------|--------------|--------------|---------------|---------------|
| 6 | fn_8018C51C | Menu | 0 | 1 | 3 | 1 | GXInit, Player_UpdateState, __GXInitGX | Menu_Unknown3_HandleSelection |
| 6 | fn_800F68D8 | Nu | 0 | 2 | 0 | 2 |  | NuFileLoadBuffer, NuFilePos |
| 6 | fn_800B4DF0 | Menu | 0 | 2 | 0 | 2 |  | Menu_FullReset, Menu_Unknown9_Enter |
| 6 | fn_800B203C | Menu, PauseMenu | 0 | 2 | 0 | 2 |  | Menu_Unknown3_BuildOptions, PauseMenu_HandleSelection |
| 6 | fn_800A76AC | Menu | 0 | 1 | 0 | 4 |  | MemZero, Menu_AddOption, PPCMtwpar, UI_DrawText |
| 6 | fn_800A41D8 | Menu | 0 | 2 | 0 | 2 |  | Menu_FullReset, Menu_ResetAndReturn |
| 6 | fn_8007F354 | AIScript, Menu | 0 | 2 | 0 | 2 |  | AIScriptLoadScp, Menu_Unknown3_BuildOptions |
| 6 | fn_8007CC34 | AIScript | 0 | 2 | 0 | 2 |  | AIScriptLoadScp, AIScriptXRefScript |
| 5 | fn_801854C4 | Menu | 1 | 0 | 2 | 0 | Menu_Unknown4_HandleSelection, __GXInitGX |  |
| 5 | fn_8017C1A0 | Menu | 1 | 0 | 1 | 1 | Menu_FullReset | __GXInitRevisionBits |
| 5 | fn_8017B5D8 | Menu | 1 | 0 | 2 | 0 | GXInit, Menu_NavigateForward |  |
| 5 | fn_801503A0 | Podrace | 1 | 0 | 2 | 0 | MemZero, PodraceLoadLapSettings |  |
| 5 | fn_8014C20C | Podrace | 1 | 0 | 2 | 0 | MemZero, PodraceUpdateStartLights |  |
| 4 | fn_8054D4C4 | Nu | 1 | 0 | 1 | 0 | NuFileOpen |  |
| 4 | fn_8054D47C | Nu | 1 | 0 | 1 | 0 | NuFileOpen |  |
| 4 | fn_8054CD3C | Nu | 1 | 0 | 1 | 0 | NuFileOpen |  |
| 4 | fn_8054BC80 | Nu | 1 | 0 | 1 | 0 | NuFileClose |  |
| 4 | fn_8054B3C8 | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_80549ACC | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80549520 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_805492D4 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80547F98 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_80547F74 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_80547F44 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_805476C0 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_80542F28 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80542EC8 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80542D30 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_805429B8 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80542164 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80541754 | Nu | 1 | 0 | 1 | 0 | NuMemFree |  |
| 4 | fn_80541670 | Nu | 1 | 0 | 1 | 0 | NuMemFree |  |
| 4 | fn_80540864 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_8053FEA4 | Nu | 1 | 0 | 1 | 0 | NuMemFree |  |
| 4 | fn_8051EF48 | Nu | 1 | 0 | 1 | 0 | NuSpecialFind |  |
| 4 | fn_8051ED5C | Nu | 1 | 0 | 1 | 0 | NuSpecialFind |  |
| 4 | fn_803ECA1C | Menu | 1 | 0 | 1 | 0 | Menu_ResetStack |  |
| 4 | fn_803E5DAC | Menu | 1 | 0 | 1 | 0 | Menu_RegisterDescriptors |  |
| 4 | fn_803E3128 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_803E2C10 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_803E07C4 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_803DFAF8 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_803D8EC4 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_803D5A50 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_803D5990 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_80388DF8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80374E60 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8034EA94 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_803459E0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_80345038 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_8033F194 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_8033E97C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_8033DF08 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_BuildOptions |  |
| 4 | fn_8033BD54 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_8033BC58 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_8033BB68 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_8033BA78 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_8033B940 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_8033B850 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_8033AB9C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_8033AA2C | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_8033A9E4 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_8033A020 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_80336984 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown11_BuildOptions |  |
| 4 | fn_8031AF40 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80315268 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_BuildOptions |  |
| 4 | fn_8030CCE4 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_802F2EF0 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_802F2DA0 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_802BD5AC | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_8029D1C8 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_8029BDA0 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_80297594 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8029743C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80296CE4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_802967D0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80295AC8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80292420 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8028CCD0 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_802507C4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8024F420 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8024B5B0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8024A9EC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8024958C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80231810 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_8022D9CC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8021FA14 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_80219084 | Menu | 1 | 0 | 1 | 0 | Menu_Main_HandleSelection |  |
| 4 | fn_80217064 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_BuildOptions |  |
| 4 | fn_80210004 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_80209C6C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_801F2DD4 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_801F2A8C | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_801F2264 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_801D401C | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_801B3904 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80183698 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_80182D38 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80182790 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80179354 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_801792B8 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_801701E0 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_8016FEBC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown13_HandleSelection |  |
| 4 | fn_8016FB68 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown14_Enter |  |
| 4 | fn_8016F588 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateMine |  |
| 4 | fn_8016E5D0 | Podrace | 0 | 1 | 0 | 2 |  | CreditBufferAlloc, PodraceUpdateStartLights |
| 4 | fn_8016C468 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_8016AEDC | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_8016AA48 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_8016A604 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateCollapsingMush |  |
| 4 | fn_8016A1A4 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_80169DE4 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_80169370 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_80169088 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_80168C00 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_80168734 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateMine |  |
| 4 | fn_801677B8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown15_BuildOptions |  |
| 4 | fn_80163A78 | Menu | 0 | 1 | 0 | 2 |  | MemZero, Menu_AddOption |
| 4 | fn_80163984 | Menu | 0 | 1 | 0 | 2 |  | MemZero, Menu_AddOption |
| 4 | fn_80162354 | Podrace | 0 | 1 | 1 | 1 | MemZero | PodraceUpdateMine |
| 4 | fn_80160E90 | Menu | 0 | 1 | 0 | 2 |  | MemZero, Menu_AddOption |
| 4 | fn_8015DE1C | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_801507E0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80150518 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_80150428 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_8015015C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_8014FDC8 | Menu | 1 | 0 | 1 | 0 | Menu_AddOption |  |
| 4 | fn_8014FD18 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_8014EBBC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown17_HandleSelection |  |
| 4 | fn_8014E9B8 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_8014E734 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_8014CAE8 | Podrace | 0 | 1 | 1 | 1 | MemZero | PodraceCreateMine |
| 4 | fn_8014B884 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80148CC4 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_801486FC | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_801474EC | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_80146588 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8014626C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_80146018 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80145F18 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80145BCC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80143ED8 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_80142BA8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown13_BuildOptions |  |
| 4 | fn_80142380 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown13_BuildOptions |  |
| 4 | fn_80142168 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown13_BuildOptions |  |
| 4 | fn_80141D64 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_801403BC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_BuildOptions |  |
| 4 | fn_8012C884 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_8012228C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleEntry1 |  |
| 4 | fn_8011F17C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_8011D78C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_8011D4F8 | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_8011C4B8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown6_HandleSelection |  |
| 4 | fn_8011B690 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_HandleSelection |  |
| 4 | fn_8010A318 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_HandleSelection |  |
| 4 | fn_801087C4 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_8010859C | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_80107828 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleEntry2 |  |
| 4 | fn_80104D38 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_801048FC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80103D18 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleEntry3 |  |
| 4 | fn_800F01C4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800EF000 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800E1850 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_800DC454 | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_800D8B70 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_800D64B0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_800D6028 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_800D4CD0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_800D4810 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_800D3054 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_800D0964 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_800CF138 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_800CE1EC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_HandleSelection |  |
| 4 | fn_800CD55C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800CCBC0 | Menu | 1 | 0 | 1 | 0 | Menu_Main_HandleSelection |  |
| 4 | fn_800CC450 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800C9648 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800C8014 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800C1DA8 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_800BDA3C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_800B2E10 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_800AFDC0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown6_BuildOptions |  |
| 4 | fn_800ACCC8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800AC728 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown6_HandleSelection |  |
| 4 | fn_800AB85C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_800AB534 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_800AAC04 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_800A5DBC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800A1E64 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8009FAD8 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_8009E310 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_8009DCE8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8009D550 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8009D23C | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_8009CD4C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8009CAEC | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_8009C2A4 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_80098D48 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown6_HandleSelection |  |
| 4 | fn_80097948 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800962B4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80095BA0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8009587C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80095630 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80093654 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80093428 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80092F94 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80091FC0 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_800913BC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8009083C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_800902C0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8008FCD0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8008F8B4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8008DCE0 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateCollapsingMush |  |
| 4 | fn_80079AC4 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_800787B4 | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_80077AD4 | Menu | 1 | 0 | 1 | 0 | Menu_OpenFromRegistry |  |
| 4 | fn_80076DF0 | Menu | 1 | 0 | 1 | 0 | Menu_OpenSpecial |  |
| 4 | fn_800769B4 | Menu | 1 | 0 | 1 | 0 | Menu_OpenFromRegistry |  |
| 4 | fn_80072D10 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown6_HandleSelection |  |
| 4 | fn_800712E4 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_800701F8 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_HandleSelection |  |
| 4 | fn_8006F28C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_Close |  |
| 4 | fn_8006F1D8 | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006F0BC | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006ECF0 | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006EB98 | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006E3E0 | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006DF1C | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006DD70 | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006DD1C | Menu | 1 | 0 | 1 | 0 | Menu_Navigate |  |
| 4 | fn_8006C748 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_8006B328 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_HandleEntry1 |  |
| 4 | fn_8006B1E0 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_HandleEntry1 |  |
| 4 | fn_80066294 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8005BE2C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80059C44 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_80056E88 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_80053860 | Menu | 1 | 0 | 1 | 0 | Menu_OpenFromRegistry |  |
| 4 | fn_80041CE8 | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_8003A244 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_80038058 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_8001FBB0 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_8001E850 | Nu | 1 | 0 | 1 | 0 | NuFilePos |  |
| 4 | fn_8001D4B8 | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_8001BE94 | Nu | 1 | 0 | 1 | 0 | NuFileOpen |  |
| 4 | fn_800197B8 | Nu | 1 | 0 | 1 | 0 | NuFileBeginBlkRead |  |
| 4 | fn_80018F78 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80018EF8 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80018DA8 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80018C84 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80018B58 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80018AF4 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80018558 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_8001678C | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_80014EC0 | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_8001459C | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBufferLsn |  |
| 4 | fn_80013B28 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_800137D0 | Nu | 1 | 0 | 1 | 0 | NuFileOpen |  |
| 4 | fn_80013310 | Nu | 1 | 0 | 1 | 0 | NuSpecialFindMulti |  |
| 4 | fn_80013268 | Nu | 1 | 0 | 1 | 0 | NuSpecialFindMulti |  |
| 4 | fn_80013200 | Nu | 1 | 0 | 1 | 0 | NuSpecialFind |  |
| 4 | fn_80011214 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80011070 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80010EF8 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80010C30 | Nu | 1 | 0 | 1 | 0 | NuMemFree |  |
| 4 | fn_80010120 | Nu | 1 | 0 | 1 | 0 | NuMemFree |  |
| 4 | fn_8000F9FC | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_8000F65C | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_8000E6B0 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_8000DCE0 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_8000DC10 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_8000D95C | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_8000D874 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_8000CAA0 | Nu | 1 | 0 | 1 | 0 | NuFilePos |  |
| 4 | fn_8000C8BC | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_8000BF70 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_8000BD10 | Nu | 1 | 0 | 1 | 0 | NuFileClose |  |
| 4 | fn_8000B5F4 | Nu | 1 | 0 | 1 | 0 | NuFileOpen |  |
| 4 | fn_8000B518 | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_8000A64C | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_8000A268 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80009C00 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_8000965C | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80009408 | Nu | 1 | 0 | 1 | 0 | NuDatFileLoadBuffer |  |
| 4 | fn_80008E78 | Nu | 1 | 0 | 1 | 0 | NuFileOpen |  |
| 4 | fn_800085B0 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_800080D4 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_800076E4 | Nu | 1 | 0 | 1 | 0 | NuMemAlloc |  |
| 4 | fn_80007468 | Nu | 1 | 0 | 1 | 0 | NuMemFree |  |
| 4 | fn_80005E80 | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_800056E0 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_800055EC | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_8000556C | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_800034A0 | Nu | 1 | 0 | 1 | 0 | NuFileClose |  |
| 4 | fn_8000306C | Nu | 1 | 0 | 1 | 0 | NuFileClose |  |
| 4 | fn_80002D1C | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_800021C0 | Nu | 1 | 0 | 1 | 0 | NuFileClose |  |
| 4 | fn_80001CEC | Nu | 1 | 0 | 1 | 0 | NuFileLoadBuffer |  |
| 4 | fn_80001BCC | Nu | 1 | 0 | 1 | 0 | NuFileBeginBlkRead |  |
| 4 | fn_80001AD0 | Nu | 1 | 0 | 1 | 0 | NuFileBeginBlkRead |  |
| 4 | fn_80001228 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_80000C64 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_800000A0 | Nu | 1 | 0 | 1 | 0 | NuDatFileOpen |  |
| 4 | fn_7FFFFAA0 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_7FFFF8CC | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_7FFFF4DC | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_7FFFE90C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFFE8F0 | Nu | 1 | 0 | 1 | 0 | NuDatOpenEx |  |
| 4 | fn_7FFFE584 | Menu | 1 | 0 | 1 | 0 | Menu_CleanupAndReset |  |
| 4 | fn_7FFFE438 | Menu | 1 | 0 | 1 | 0 | Menu_CleanupAndReset |  |
| 4 | fn_7FFFD8BC | Nu | 1 | 0 | 1 | 0 | NuFileEndBlockRead |  |
| 4 | fn_7FFFD534 | Nu | 1 | 0 | 1 | 0 | NuFileEndBlockRead |  |
| 4 | fn_7FFFCD08 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_7FFF9A08 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_7FFF7A48 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_7FFF7688 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_7FFF734C | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_7FFF62C0 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_7FFF5F6C | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_7FFF5BC4 | Nu | 1 | 0 | 1 | 0 | NuPPLoadBuffer |  |
| 4 | fn_7FFEDCB4 | Nu | 1 | 0 | 1 | 0 | NuSpecialFind |  |
| 4 | fn_7FFEDAE0 | Nu | 1 | 0 | 1 | 0 | NuSpecialFind |  |
| 4 | fn_7FFED7E0 | Nu | 1 | 0 | 1 | 0 | NuSpecialFindMulti |  |
| 4 | fn_7FFED744 | Nu | 1 | 0 | 1 | 0 | NuSpecialFindMulti |  |
| 4 | fn_7FFED51C | Nu | 1 | 0 | 1 | 0 | NuSpecialFindMulti |  |
| 4 | fn_7FFE7E74 | Nu | 1 | 0 | 1 | 0 | NuAnimCurve2CalcVal |  |
| 4 | fn_7FFE65E0 | Nu | 1 | 0 | 1 | 0 | NuSpecialFindMulti |  |
| 4 | fn_7FFE6358 | Nu | 1 | 0 | 1 | 0 | NuSpecialFindMulti |  |
| 4 | fn_7FFE5E90 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFE1584 | Nu | 1 | 0 | 1 | 0 | NuAnimCurve2CalcVal |  |
| 4 | fn_7FFE0648 | Nu | 1 | 0 | 1 | 0 | NuAnimCurve2CalcVal |  |
| 4 | fn_7FFD9CA4 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FFD98F0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FFD9748 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_7FFD8DA4 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_7FFD89B0 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_7FFD8458 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_7FFD6144 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFD5C84 | Menu | 1 | 0 | 1 | 0 | Menu_Main_BuildOptions |  |
| 4 | fn_7FFD5158 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFD414C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFD4060 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFD3AA0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_BuildOptions |  |
| 4 | fn_7FFD3200 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_7FFD3130 | Menu | 1 | 0 | 1 | 0 | Menu_CleanupAndReset |  |
| 4 | fn_7FFD278C | Menu | 1 | 0 | 1 | 0 | Menu_CleanupAndReset |  |
| 4 | fn_7FFD1E40 | Menu | 1 | 0 | 1 | 0 | Menu_CleanupAndReset |  |
| 4 | fn_7FFD01A0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFCF4D8 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FFCBC74 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_7FFCBB28 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_7FFCBA1C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_7FFCB8D0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_7FFC8A0C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FFC8934 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FFC882C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FFC86D0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FFC84F0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FFC7E68 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FFC7E24 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_BuildOptions |  |
| 4 | fn_7FFC7994 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FFC6E34 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_BuildOptions |  |
| 4 | fn_7FFC6E04 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_BuildOptions |  |
| 4 | fn_7FFC6DD4 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_BuildOptions |  |
| 4 | fn_7FFC6934 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FFC68E0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FFC5F04 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5EC8 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5E8C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5E50 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5E14 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5D18 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5C28 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5BFC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_HandleSelection |  |
| 4 | fn_7FFC5B38 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5A3C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5A00 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5910 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5814 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC57D8 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC579C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5760 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC5724 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC56E8 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC56AC | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC54FC | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC54C0 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FFC4324 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown6_HandleSelection |  |
| 4 | fn_7FFC3F78 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFC3F3C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFC3A6C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_7FFC35B8 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_BuildOptions |  |
| 4 | fn_7FFC3588 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_BuildOptions |  |
| 4 | fn_7FFC3534 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_BuildOptions |  |
| 4 | fn_7FFC33E4 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_7FFC336C | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_BuildOptions |  |
| 4 | fn_7FFC32E8 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_BuildOptions |  |
| 4 | fn_7FFC3198 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_7FFC2F28 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_BuildOptions |  |
| 4 | fn_7FFC2EF8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_BuildOptions |  |
| 4 | fn_7FFC2DE4 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_HandleSelection |  |
| 4 | fn_7FFC2B30 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_HandleSelection |  |
| 4 | fn_7FFC0948 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown11_BuildOptions |  |
| 4 | fn_7FFC0678 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown10_BuildOptions |  |
| 4 | fn_7FFC0444 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_HandleSelection |  |
| 4 | fn_7FFC03B4 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_HandleSelection |  |
| 4 | fn_7FFBF490 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_7FFBEDB0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_HandleSelection |  |
| 4 | fn_7FFBEC60 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown11_OpenHandler |  |
| 4 | fn_7FFBE9CC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown10_HandleSelection |  |
| 4 | fn_7FFBDFCC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown11_OpenHandler |  |
| 4 | fn_7FFBA33C | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_7FFBA2AC | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_7FFAEDA4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFAC498 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FFA6B5C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FFA5994 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFA52D8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFA51E8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFA5008 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFA4FFC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_HandleSelection |  |
| 4 | fn_7FFA4EB8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_HandleSelection |  |
| 4 | fn_7FFA4D74 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFA3FD0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFA179C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FFA02E8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_7FF9F9B0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_7FF9F704 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_7FF9F4DC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown12_HandleSelection |  |
| 4 | fn_7FF9F3D4 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_7FF9EDEC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_7FF9D9AC | Menu | 1 | 0 | 1 | 0 | Menu_Main_HandleSelection |  |
| 4 | fn_7FF9D934 | Menu | 1 | 0 | 1 | 0 | Menu_Main_HandleSelection |  |
| 4 | fn_7FF9B360 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FF9B210 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FF9B14C | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FF9B0FC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FF9AE90 | Menu | 1 | 0 | 1 | 0 | Menu_Main_HandleSelection |  |
| 4 | fn_7FF9A418 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_HandleSelection |  |
| 4 | fn_7FF996A4 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF98B40 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown6_HandleSelection |  |
| 4 | fn_7FF95C60 | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_HandleSelection |  |
| 4 | fn_7FF935CC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_HandleSelection |  |
| 4 | fn_7FF775A8 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FF69B24 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_7FF69AB8 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_7FF68A44 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_7FF3BDBC | Menu | 1 | 0 | 1 | 0 | Menu_ResetFromGame |  |
| 4 | fn_7FF3579C | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_7FF31554 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF30E94 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF30894 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF305D0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF30528 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF30498 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF30420 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF301B0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF3006C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF2F1F4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF2F0E8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF2EFB0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF18C80 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FF0F818 | Menu | 1 | 0 | 1 | 0 | Menu_ResetFromGame |  |
| 4 | fn_7FF09300 | Menu | 1 | 0 | 1 | 0 | Menu_ResetAndReturn |  |
| 4 | fn_7FF032A0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FF03204 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FF02804 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_HandleSelection |  |
| 4 | fn_7FF000B8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF000AC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF000A0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF00094 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF00088 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF0007C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF00070 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF00064 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF00058 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF0004C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF00040 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FF00034 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_HandleSelection |  |
| 4 | fn_7FEE8E8C | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FEDC2AC | PauseMenu | 1 | 0 | 1 | 0 | PauseMenu_HandleEntry1 |  |
| 4 | fn_7FED06C4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEC97C0 | Menu | 1 | 0 | 1 | 0 | Menu_OpenFromRegistry |  |
| 4 | fn_7FEBBF88 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBBF40 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBBF04 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBBC6C | Menu | 1 | 0 | 1 | 0 | Menu_RegisterDescriptors |  |
| 4 | fn_7FEBBC40 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBB9A0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBB854 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBB6C4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBB608 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBB5B4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBB368 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBAB88 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBA8F4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEBA648 | Menu | 1 | 0 | 1 | 0 | Menu_OpenSpecial |  |
| 4 | fn_7FEB9E98 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB9C60 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB9C04 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB97CC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB9640 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB9600 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB9214 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB8FC8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB4C6C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB49CC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB3FA0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB32B0 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB29C8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB262C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FEB2200 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEB2194 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEB1D8C | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEB1D20 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEB1750 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEB16E4 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAF8B4 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEAF830 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEAF554 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEAF4E8 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEAF2E4 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEAF278 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEAEF5C | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEAEBF4 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEAEB70 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEAE9A8 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEAE924 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEAB64C | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB604 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB564 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB55C | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB4C0 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB1F0 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB19C | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB130 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB0C4 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAB058 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAADE8 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAADAC | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAAD58 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAAD04 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAACB0 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAAA34 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAA94C | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAA848 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAA7AC | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAA550 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEAA310 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA9F14 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA93F4 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA938C | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA8880 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA8430 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA8340 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA81C0 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA8114 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA7EC0 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA7EA4 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA7E78 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA7E0C | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA7D18 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA7B18 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA7AA8 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA77F4 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA7664 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA7574 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA7148 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA63C8 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA6314 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA6268 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA5F60 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA5D70 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA5D34 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA5CF8 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA5C58 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA5C04 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA5BC8 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA5B4C | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA59A8 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA5798 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA5738 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA44F4 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA4434 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA4380 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA41D0 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA3F28 | AIScript | 1 | 0 | 1 | 0 | AIScriptParseConditions |  |
| 4 | fn_7FEA3EF4 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA3E34 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA3D80 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA3D78 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA3C00 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA3A88 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA39BC | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA2ED0 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA2C50 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_7FEA2BD4 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_7FEA2AA8 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA2A94 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2A78 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2A10 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_7FEA29B0 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2958 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA28F0 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2898 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2848 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA27E4 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA26F8 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA26D8 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA2634 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2630 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA25CC | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2544 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA24DC | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA24A0 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2484 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA23B0 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2358 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA22F0 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2298 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2248 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA21E4 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA211C | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2064 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA2030 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA1FFC | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA1F74 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA1F20 | AIScript | 1 | 0 | 1 | 0 | AIScriptResolveReferences |  |
| 4 | fn_7FEA1924 | AIScript | 1 | 0 | 1 | 0 | AIScriptXRefScript |  |
| 4 | fn_7FEA1120 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScp |  |
| 4 | fn_7FEA0F88 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_7FEA0548 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_7FE9F6FC | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_7FE9E108 | AIScript | 1 | 0 | 1 | 0 | AIScriptLoadScriptTxt |  |
| 4 | fn_7FE83FEC | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FE83FB0 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FE83F5C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FE83F2C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FE760FC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE75F78 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE75F54 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE75F4C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE75E7C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE75CE4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE75BB4 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE74C60 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE74C3C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE7457C | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE74558 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE72700 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FE6E1C8 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateMine |  |
| 4 | fn_7FE55F0C | Podrace | 1 | 0 | 1 | 0 | PodraceCreateMine |  |
| 4 | fn_7FE4DA98 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FE44230 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE43888 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE3ABDC | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE381B8 | Menu | 1 | 0 | 1 | 0 | Menu_FullReset |  |
| 4 | fn_7FE21AEC | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FE21A68 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FE219E4 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FE21960 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FE218DC | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FE21858 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FE20820 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FE20670 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FE205EC | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FE20568 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FE1DD9C | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_7FE1DD00 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_7FE1DC64 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_7FE1DBA4 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_7FE18AC0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown1_BuildOptions |  |
| 4 | fn_7FE14A30 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FE14A14 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FE13A98 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown16_HandleSelection |  |
| 4 | fn_7FE1100C | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FE10FA0 | Menu | 1 | 0 | 1 | 0 | Menu_InitDefinitions |  |
| 4 | fn_7FE0FB9C | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_7FE0FB30 | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_7FE0FAAC | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_7FE0F77C | Menu | 1 | 0 | 1 | 0 | Menu_NavigateForward |  |
| 4 | fn_7FE0D1E8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0D0B8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0D034 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0CFF0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0CEC0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0CEA0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0CE54 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0CD70 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0CCF8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0CC54 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown4_BuildOptions |  |
| 4 | fn_7FE0C684 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_BuildOptions |  |
| 4 | fn_7FE0C654 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown5_BuildOptions |  |
| 4 | fn_7FE09D50 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FE09C20 | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FE09B6C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FE09B3C | DebugMenu | 1 | 0 | 1 | 0 | DebugMenu_BuildOptions |  |
| 4 | fn_7FE093D8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FE093A8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FE0930C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FE092DC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FE08F04 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FE08EBC | Menu | 1 | 0 | 1 | 0 | Menu_Unknown9_BuildOptions |  |
| 4 | fn_7FE078F0 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_BuildOptions |  |
| 4 | fn_7FE078A8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_BuildOptions |  |
| 4 | fn_7FE077E8 | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_BuildOptions |  |
| 4 | fn_7FE0777C | Menu | 1 | 0 | 1 | 0 | Menu_Unknown3_BuildOptions |  |
| 4 | fn_7FDEADB0 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateMine |  |
| 4 | fn_7FDD9A88 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FDCC394 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_7FDCC2EC | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_7FDCC25C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_7FDCC19C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateStartLights |  |
| 4 | fn_7FCFE338 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateCollapsingMush |  |
| 4 | fn_7FC65EC4 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateMine |  |
| 4 | fn_7FC63570 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateMine |  |
| 4 | fn_7FC5ED78 | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_7FC5ECAC | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_7FC5E984 | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_7FC5E8B8 | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_7FC5E3A4 | Podrace | 1 | 0 | 1 | 0 | PodraceCreateCollapsingMush |  |
| 4 | fn_7FC5DA5C | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_7FC5D1FC | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC5C97C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC4C8AC | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC4C81C | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC4C684 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC4C534 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC4C420 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC48A9C | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC47EAC | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC43368 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC432F0 | Podrace | 1 | 0 | 1 | 0 | PodraceUpdateMine |  |
| 4 | fn_7FC38D44 | Podrace | 1 | 0 | 1 | 0 | PodraceInitBoost |  |
| 4 | fn_7FC32868 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FC327E4 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FC32760 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FC326DC | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FC32658 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FC325D4 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadTimeTrialSettings |  |
| 4 | fn_7FC31650 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC315F0 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC3159C | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC31500 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC313EC | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC31368 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC312E4 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadSplineSettings |  |
| 4 | fn_7FC2EB18 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_7FC2EA7C | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_7FC2E9E0 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
| 4 | fn_7FC2E920 | Podrace | 1 | 0 | 1 | 0 | PodraceLoadLapSettings |  |
