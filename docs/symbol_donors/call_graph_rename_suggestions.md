# Call Graph Rename Suggestions

Extracted 30596 call sites from 4135 functions.
764 unnamed functions have named call context.

---
## Suggestions (by named callee count descending)

| Function | Size | Named Callers | Named Callees | Suggested Context |
|----------|------|---------------|---------------|-------------------|
| fn_80068F14 | 0x8FC |  | Action_FaceOpponent, Action_SetViewDistance, Action_MoveAway | AI_Actions(10) |
| fn_80069C40 | 0xA88 | Action_OpenDoor | Action_FollowOpponent, AIScriptLoadScriptTxt, Menu_Navigate, | AI_Actions(7), AIScript(2), Menu(1) |
| fn_8008CF0C | 0xDD4 |  | Action_SetMoveRadius, Action_SetMinViewHeight, Condition_Opp | AI_Actions(6), AI_Conditions(4) |
| fn_800AD4C4 | 0x28FC |  | Action_AttackOpponent, Action_SetInvulnerable, Action_Engage | AI_Actions(7), Menu(3) |
| fn_8010CD84 | 0x6564 | GXInit, Condition_HitPointsInit, Condition_IsOnScreenInit, C |  | AI_Conditions(6), Menu(1) |
| fn_80151510 | 0x214 | Menu_AddOption, MemZero | Menu_AddOption, PodraceLoadSplineSettings, PodraceUpdateMine | Menu(2) |
| fn_8006A6C8 | 0x644 | Action_CloseDoor | AIScriptParseConditions, ScriptKeyword_STATE, AIScriptXRefSc | AIScript(4), Script(2), AI_Actions(1) |
| fn_80072410 | 0x900 | Menu_OpenSpecial, DebugMenu_HandleSelection, Action_Activate |  | AI_Actions(5), Menu(1) |
| fn_800ED648 | 0x19B8 | Menu_FullReset, Menu_ResetAndReturn | NuAnimKeyWrite, NuAnimKeyBlend, MemZero, Menu_OpenFromRegist | Menu(3), Nu2(3) |
| fn_8018C51C | 0x5E0 | Action_SetNearestSpline, GXInit, Player_UpdateState, Action_ | Menu_Unknown3_HandleSelection | AI_Actions(3), Player(1), Menu(1) |
| fn_80019A54 | 0x4B4 | NuDatFileOpenSize, NuAnimData2FixPtrs | NuAnimCurveSetLoad, NuAnimData2Apply, NuAnimCurve2SetApplyBl | Nu2(6) |
| fn_8008DCE0 | 0x7B4 | PodraceCreateCollapsingMush | Action_FaceOpponent, Condition_OpponentToLocatorXZ, Action_S | AI_Actions(4), AI_Conditions(1) |
| fn_800B7870 | 0x700 | Action_SetLocator, Action_GoToOrigin | Menu_Unknown4_HandleSelection, Menu_Main_HandleSelection, Me | Menu(4), AI_Actions(2) |
| fn_8012AD98 | 0xF84 | Action_SetSide | Condition_LocatedCollectable, Action_SetLocator_Level, Actio | AI_Actions(5), AI_Conditions(1) |
| fn_800119C8 | 0x4FC | NuAnimDataRead, NuAnimDataFindVersion | Action_MoveAwayFromPlayer, Action_GoToNode, Action_SetMinVie | AI_Actions(3), Nu2(2) |
| fn_8001F548 | 0x2BC |  | Action_FaceOpponent, NuAnimData2FixPtrs, NuAnimKeyWrite, Act | Nu2(3), AI_Actions(2) |
| fn_8009D5D0 | 0x408 | Menu_ResetAndReturn | NuAnimData2CalcTime, NuAnimCurve2CalcVal, NuAnimCurve2CalcVa | Nu2(3), Menu(1) |
| fn_800B4DF0 | 0x9EC | Action_GoToLocator, Action_MoveAwayFromNode, Action_BlockPat | Menu_Unknown9_Enter, Menu_FullReset | AI_Actions(3), Menu(2) |
| fn_800DA478 | 0x1968 | NuDatFileLoadBuffer, Menu_FullReset | NuAnimCurve2SetApplyBlendToJoint2TransLoc, NuAnimData2FixPtr | Nu2(4), Menu(1) |
| fn_8012228C | 0x141C | DebugMenu_HandleEntry1, Action_SetInvulnerable, Action_DontP | Action_CreateCreatures, Action_CreateSockCreatures | AI_Actions(4) |
| fn_80169088 | 0x2E8 | Menu_AddOption | Action_Activate, Condition_ToNodeInit, Condition_CutScenePla | AI_Conditions(3), Menu(1), AI_Actions(1) |
| fn_8016AEDC | 0x168 | Menu_AddOption | Action_SetAIOverrideControl, Action_UseForce, Action_SetScri | AI_Actions(4), Menu(1) |
| fn_8016B084 | 0x2E0 |  | Action_CreateSplineCreatures, Condition_LocatorOnScreenInit, | AI_Conditions(4), AI_Actions(1) |
| fn_8016BEC0 | 0x274 |  | Action_GoToLevelPath, MemZero, Action_UseForce, Action_CnxCo | AI_Actions(4) |
| fn_8016EE88 | 0x300 |  | Action_RaceOpponent, Action_MakeExplosion, Action_ZamMovemen | AI_Actions(5) |
| fn_80006398 | 0x498 | Menu_ResetAndReturn | NuMemAlloc, NuMemFree, NuDatFileFindTree | Nu2(3), Menu(1) |
| fn_8000C1B8 | 0x47C | NuHeap, NuFileLoadBuffer | NuAnimCurve2CalcVal, NuAnimCurveCalcVal2 | Nu2(4) |
| fn_800151CC | 0xA04 | NuFileEndBlockRead, NuAnimCurve2CalcValEx | NuAnimData2CalcTime, NuAnimCurve2CalcVal | Nu2(4) |
| fn_80077E9C | 0x224 |  | Action_BlockPath, Action_PathConnectionMaxLength, Condition_ | AI_Actions(3), AI_Conditions(1) |
| fn_8007818C | 0x22C |  | Action_GoToLocator, Menu_Navigate, Action_MoveAwayFromNode,  | AI_Actions(2), Menu(1), AI_Conditions(1) |
| fn_800A76AC | 0x1700 |  | Menu_AddOption, PPCMtwpar, MemZero, UI_DrawText | Menu(1) |
| fn_800B6078 | 0xB44 | Action_MoveAwayFromNode | Action_EngageOpponent, Action_CreateSplineCreatures, Action_ | AI_Actions(4) |
| fn_800B6D94 | 0xADC | Action_GoToLocator | Menu_Unknown9_BuildOptions, DebugMenu_BuildOptions, Menu_Unk | Menu(2), AI_Actions(1) |
| fn_800D197C | 0xF24 |  | NuAnimCurve2SetApplyToMatrix, NuAnimCurve2SetApplyBlendToJoi | Nu2(2), AI_Actions(2) |
| fn_800DD1C8 | 0x1F08 | NuFilePos, NuFileClose, NuFileOpen, NuFileSeek |  | Nu2(4) |
| fn_800E8B1C | 0x1998 |  | Action_SetMoveRadius, Condition_OpponentInTriggerArea, MemZe | AI_Actions(2), AI_Conditions(1) |
| fn_800F5C34 | 0xCA4 | GXSetGPFifo, GXSetCPUFifo | Action_RaceOpponent, Action_CameraCut | AI_Actions(2) |
| fn_800F99D4 | 0x96C | Condition_PlayerUsingForceInit, Condition_ForceCompleteInit, |  | AI_Conditions(4) |
| fn_8010A318 | 0x878 | Action_CantDie, Action_SetDontMove, Action_DontSetStoppedFla |  | AI_Actions(3), Menu(1) |
| fn_8014F058 | 0x218 | PodraceLoadTimeTrialSettings, MemZero | PodraceInitBoost, PodraceLoadSplineSettings |  |
| fn_8015EC44 | 0x188 | MemZero | Action_AttackButtonMash, Action_SetTaggable, Action_SetDoorF | AI_Actions(3) |
| fn_8015EF84 | 0x484 |  | Action_SetFormationCommander, Action_OpenDoor, MemZero, Acti | AI_Actions(3) |
| fn_80162814 | 0x228 | Menu_Unknown14_HandleSelection, MemZero | MemZero, PodraceLoadLapSettings | Menu(1) |
| fn_8016B3E8 | 0x70 |  | Condition_EitherPlayerOnObject, Condition_EitherPlayerIsInit | AI_Conditions(4) |
| fn_8016C468 | 0x4B0 | Menu_AddOption | Action_SetHint, Action_SetTaggable, Action_SetSide | AI_Actions(3), Menu(1) |
| fn_80176688 | 0x2A0 |  | Action_SetInvulnerable, Action_PlaceAheadOfPlayer, Action_En | AI_Actions(4) |
| fn_800077E8 | 0x118 | NuDatOpenEx, NuMemAlloc, NuMemFree |  | Nu2(3) |
| fn_80011EC4 | 0x55C | NuAnimDataRead, NuAnimDataFindVersion | NuInitHardware | Nu2(3) |
| fn_80017EA0 | 0x6B8 | NuDatFileLoadBuffer, NuAnimCurve2CalcVal | NuSpecialFind | Nu2(3) |
| fn_8001DC4C | 0xA0 |  | NuAnimCurveSetApplyBlendToJoint2, NuAnimData2Fixup, NuAnimCu | Nu2(3) |
| fn_8001E430 | 0x33C | NuFileSeek | NuAnimKeyRead, NuAnimKeyLerp | Nu2(3) |
| fn_8001F804 | 0x2B4 |  | NuAnimCurveSetLoad, Action_FaceOpponent, NuAnimData2FixPtrs | Nu2(2), AI_Actions(1) |
| fn_80027FAC | 0x18C |  | NuAnimCurveSetLoad, NuAnimData2Relocate, NuAnimData2Fixup | Nu2(3) |
| fn_8004901C | 0x980 | NuAnimCurveSetApplyBlendToJoint2, NuAnimCurveSetLoad, NuSoun |  | Nu2(3) |
| fn_800622F0 | 0x848 |  | Action_SetFullPathSearch, Action_IgnoreWallSplines, Action_P | AI_Actions(3) |
| fn_8006C748 | 0x298 | Menu_InitDefinitions | Action_SnapToPosition, Action_SnapToOrigin | AI_Actions(2), Menu(1) |
| fn_800712E4 | 0x384 | Menu_NavigateForward, Action_LockDoor, Action_UnLockDoor |  | AI_Actions(2), Menu(1) |
| fn_8007373C | 0xC38 | Menu_OpenFromRegistry | Menu_OpenFromRegistry, Menu_Open | Menu(3) |
| fn_800775B4 | 0x1BC |  | Condition_IAm, Action_CheckWallSplines, Action_SetState | AI_Actions(2), AI_Conditions(1) |
| fn_800778F0 | 0x1E4 |  | Condition_PlayerInTriggerArea, Condition_OpponentInTriggerAr | AI_Conditions(3) |
| fn_80077AD4 | 0x160 | Menu_OpenFromRegistry | Action_PathConnectionObstacle, Action_ClearInterrupt | AI_Actions(2), Menu(1) |
| fn_800783B8 | 0x2A0 |  | Menu_Navigate, Condition_GotLocator, Condition_LocatorRangeI | AI_Conditions(2), Menu(1) |
| fn_80078B5C | 0x1AC | AIScriptXRefScript | Menu_ResetStack, Menu_RegisterDescriptors | Menu(2), AIScript(1) |
| fn_8007C534 | 0x370 | AIScriptParseConditions | AIScriptXRefScript, ScriptKeyword_STATE | AIScript(2), Script(1) |
| fn_8007CC34 | 0x220 | ScriptKeyword_PARAM | AIScriptXRefScript, AIScriptLoadScp | AIScript(2), Script(1) |
| fn_80083958 | 0xE4 | Action_SetInterrupt | Condition_Random, Condition_LocatorRangeXZ | AI_Conditions(2), AI_Actions(1) |
| fn_8008E650 | 0x474 |  | Action_FaceOpponent, Action_Idle, Action_RetreatFromOpponent | AI_Actions(3) |
| fn_8009AFE0 | 0x12C4 | Menu_FullReset, Action_GoToDoorTrigger | Menu_ResetAndReturn | Menu(2), AI_Actions(1) |
| fn_8009FB78 | 0x728 | Menu_ResetAndReturn, Action_PathConnectionMaxLength | Menu_FullReset | Menu(2), AI_Actions(1) |
| fn_800B9164 | 0x544 | Action_GoToNodeRandom | Condition_Player2InTriggerArea, Condition_GotTriggerArea | AI_Conditions(2), AI_Actions(1) |
| fn_800C8014 | 0x8CC | Menu_FullReset | Action_CreateCreatures, Action_EngageObject | AI_Actions(2), Menu(1) |
| fn_800D0964 | 0x810 | Menu_ResetAndReturn | NuAnimData2CalcMatrix, NuAnimCurve2SetApplyBlendToJoint2Tran | Nu2(2), Menu(1) |
| fn_800F68D8 | 0x3C0 |  | Action_GoToLocator, NuFileLoadBuffer, NuFilePos | Nu2(2), AI_Actions(1) |
| fn_80106CD8 | 0x210 | Action_SetHitPoints, GXSetCPUFifo, GXInitFifoPtrs |  | AI_Actions(1) |
| fn_801171A0 | 0x620 | SceneSelect_HandleSelection, Action_CnxController | Action_GoToOriginalPath | AI_Actions(2) |
| fn_801177C0 | 0xCB8 | Menu_Unknown3_HandleSelection | Menu_AddOption, Menu_SetupDisplay | Menu(3) |
| fn_80135CBC | 0x70 | Action_CameraCut, Condition_IAm_Level | Condition_CategoryIs | AI_Conditions(2), AI_Actions(1) |
| fn_8013FCA0 | 0x71C | Menu_FullReset, __GXInitGX, Menu_Unknown12_BuildOptions |  | Menu(2) |
| fn_8015057C | 0x250 |  | PodraceUpdateMine, Menu_AddOption, MemZero | Menu(1) |
| fn_80161160 | 0x200 | MemZero | MemZero, Action_CnxController | AI_Actions(1) |
| fn_80162354 | 0x64 | MemZero | Action_DontPush_Level, PodraceUpdateMine | AI_Actions(1) |
| fn_80162D74 | 0x1BC | PodraceInitBoost | PodraceUpdateMine, MemZero |  |
| fn_8016CDE0 | 0x808 |  | Action_SetZeroAcceleration, Action_SetJumping, Action_AddPar | AI_Actions(3) |
| fn_8016E6A8 | 0x32C | Menu_FullReset | CreditBufferAlloc, PodraceLoadLapSettings | Menu(1) |
| fn_8016E9D4 | 0x140 |  | Action_GoToOriginalPath, Action_DeflectPlayersPart, PodraceU | AI_Actions(2) |
| fn_8016EB14 | 0x110 |  | Action_GoToOriginalPath, Action_ObjectController, PodraceLoa | AI_Actions(2) |
| fn_8016EC24 | 0x114 |  | PodraceLoadSplineSettings, Action_CnxController, Action_Obje | AI_Actions(2) |
| fn_8017616C | 0xA8 |  | Action_SnapToSockPosition, Action_CreateCreatures, Action_Sn | AI_Actions(3) |
| fn_8017B5D8 | 0xB58 | Menu_NavigateForward, GXInit | GXCPInterruptHandler | Menu(1) |
| fn_80186D18 | 0xA2C | GXInit | GXSetCPUFifo, GXSetGPFifo |  |
| fn_800034A0 | 0x150 | NuFileClose | NuAnimCurveSetApplyBlendToJoint2 | Nu2(2) |
| fn_800059A0 | 0xAC |  | NuFileEndBlockRead, NuAnimDataFindVersion | Nu2(2) |
| fn_80005A4C | 0xD0 |  | NuAnimDataRead, NuFileEndBlockRead | Nu2(2) |
| fn_80005E80 | 0xDC | NuFileLoadBuffer | NuDatFileFindTree | Nu2(2) |
| fn_80007900 | 0x22C | NuMemAlloc, NuFileClose |  | Nu2(2) |
| fn_800081CC | 0xD8 | NuPPLoadBuffer | NuPPLoadBuffer | Nu2(2) |
| fn_80008E78 | 0x330 | NuHeap, NuFileOpen |  | Nu2(2) |
| fn_80009E94 | 0x3D4 | NuDatFileLoadBuffer | NuPPLoadBuffer | Nu2(2) |
| fn_8000B5F4 | 0x2D8 | NuFileOpen, NuFileRead |  | Nu2(2) |
| fn_8000D4DC | 0x398 | NuPPLoadBuffer, NuMemFree |  | Nu2(2) |

---
## Stats

| Metric | Value |
|--------|-------|
| Total functions | 4135 |
| Named | 557 |
| Unnamed | 3578 |
| Unnamed with named context | 764 |
| bl instructions | 30596 |
