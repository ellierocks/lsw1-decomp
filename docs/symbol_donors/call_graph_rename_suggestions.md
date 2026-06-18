# Call Graph Rename Suggestions

Extracted 30596 call sites from 4135 functions.
804 unnamed functions have named call context.

---
## Suggestions (by named callee count descending)

| Function | Size | Named Callers | Named Callees | Suggested Context |
|----------|------|---------------|---------------|-------------------|
| fn_80151510 | 0x214 | ConfigureZipUps, Menu_AddOption, ConfigureDoorTriggers, MemZ | PodraceUpdateMine, PodraceLoadLapSettings, PodraceLoadTimeTr | Menu(2) |
| fn_80068F14 | 0x8FC |  | Action_GoToNodeRandom, Action_FaceOpponent, Action_BlockPath | AI_Actions(10) |
| fn_80069C40 | 0xA88 | Action_OpenDoor | AIScriptResolveReferences, Menu_Navigate, Action_CirclePlaye | AI_Actions(7), AIScript(2), Menu(1) |
| fn_8008CF0C | 0xDD4 |  | Action_BlockPath, Action_SetMaxViewHeight, Action_GoToLocato | AI_Actions(7), AI_Conditions(3) |
| fn_800AD4C4 | 0x28FC |  | Action_SetPathCnxFlag, Menu_Unknown4_BuildOptions, Menu_Unkn | AI_Actions(7), Menu(3) |
| fn_8010CD84 | 0x6564 | Menu_InitDefinitions, Condition_HitPointsInit, Condition_IsA |  | AI_Conditions(6), Menu(1) |
| fn_8006A6C8 | 0x644 | Action_CloseDoor | AIScriptXRefScript, AIScriptResolveReferences, AIScriptLoadS | AIScript(4), Script(2), AI_Actions(1) |
| fn_80072410 | 0x900 | Action_SetDoorFinished, Action_SetObstacleChainPhase, DebugM |  | AI_Actions(5), Menu(1) |
| fn_800ED648 | 0x19B8 | Menu_ResetAndReturn, Menu_FullReset | NuAnimKeyWrite, NuAnimKeyBlend, Menu_OpenFromRegistry, NuAni | Menu(3), Nu2(3) |
| fn_8012AD98 | 0xF84 | Action_SetSide | Action_GoToDoorTrigger, Action_SnapToLocator, Action_SetLoca | AI_Actions(5), AI_Conditions(1) |
| fn_8018C51C | 0x5E0 | Action_SetRandomSpline, __GXInitGX, Player_UpdateState, Acti | Menu_Unknown3_HandleSelection | AI_Actions(3), Player(1), Menu(1) |
| fn_80019A54 | 0x4B4 | NuDatFileOpenSize, NuAnimData2FixPtrs | NuAnimData2Apply, NuAnimCurve2SetApplyBlendToJoint2TransLoc, | Nu2(6) |
| fn_8008DCE0 | 0x7B4 | PodraceCreateCollapsingMush | Action_FaceOpponent, Action_BlockPath, Action_GoToLocator, A | AI_Actions(4), AI_Conditions(1) |
| fn_800B7870 | 0x700 | Action_SetLocator, Action_GoToOrigin | Menu_Unknown1_HandleSelection, Menu_Unknown4_HandleSelection | Menu(4), AI_Actions(2) |
| fn_800DA478 | 0x1968 | ConfigureDoorTriggers, Menu_FullReset, NuDatFileLoadBuffer | NuAnimData2FixPtrs, NuAnimCurve2SetApplyBlendToJoint2TransLo | Nu2(4), Menu(1) |
| fn_801171A0 | 0x620 | ConfigureDoorTriggers, ConfigureBlupUps, Action_CnxControlle | Action_GoToOriginalPath, ConfigureBlupUps | AI_Actions(2) |
| fn_800119C8 | 0x4FC | NuAnimDataRead, NuAnimDataFindVersion | Action_MoveAwayFromPlayer, Action_GoToNode, Action_SetMinVie | AI_Actions(3), Nu2(2) |
| fn_8001F548 | 0x2BC |  | Action_FaceOpponent, Action_SetMaxViewHeight, NuAnimData2Loa | Nu2(3), AI_Actions(2) |
| fn_8003BA5C | 0xCF8 | NuAnimCurveSetApplyBlendToJoint2 | instNuGCutCamSysUpdate, instNuCGutRigidSysCreate, NuInitHard | Nu2(2), Camera(1) |
| fn_8009D5D0 | 0x408 | Menu_ResetAndReturn | NuAnimCurve2CalcValEx, NuAnimCurve2CalcVal, __GXSetTmemConfi | Nu2(3), Menu(1) |
| fn_800B4DF0 | 0x9EC | Action_BlockPath, Action_MoveAwayFromNode, Action_GoToLocato | Menu_Unknown9_Enter, Menu_FullReset | AI_Actions(3), Menu(2) |
| fn_800DD1C8 | 0x1F08 | NuFileSeek, NuFilePos, NuFileOpen, NuFileClose | NuFileGetEndianSwap | Nu2(5) |
| fn_8012228C | 0x141C | Action_DontPush_Level, Action_SetInvulnerable, DebugMenu_Han | Action_CreateCreatures, Action_CreateSockCreatures | AI_Actions(4) |
| fn_80169088 | 0x2E8 | Menu_AddOption | Condition_ToNodeInit, Action_Activate, Condition_CutScenePla | AI_Conditions(3), Menu(1), AI_Actions(1) |
| fn_8016AEDC | 0x168 | Menu_AddOption | Action_SetScriptState, Action_UseForce, Action_SetPathCnxFla | AI_Actions(4), Menu(1) |
| fn_8016B084 | 0x2E0 |  | Condition_LocatorOnScreenInit, Action_CreateSplineCreatures, | AI_Conditions(4), AI_Actions(1) |
| fn_8016BEC0 | 0x274 |  | Action_GoToLevelPath, MemZero, Action_CnxController, Action_ | AI_Actions(4) |
| fn_8016EE88 | 0x300 |  | Action_MakeExplosion, Action_ZamMovement, Action_SetDoorFini | AI_Actions(5) |
| fn_80006398 | 0x498 | Menu_ResetAndReturn | NuDatFileFindTree, NuMemAlloc, NuMemFree | Nu2(3), Menu(1) |
| fn_8000C1B8 | 0x47C | NuHeap, NuFileLoadBuffer | NuAnimCurve2CalcVal, NuAnimCurveCalcVal2 | Nu2(4) |
| fn_800151CC | 0xA04 | NuAnimCurve2CalcValEx, NuFileEndBlockRead | NuAnimCurve2CalcVal, NuAnimData2CalcTime | Nu2(4) |
| fn_8004901C | 0x980 | NuAnimCurveSetApplyBlendToJoint2, NuAnimCurveSetLoad, NuSoun | NuFmvNgc_Alloc | Nu2(4) |
| fn_80077E9C | 0x224 |  | Action_BlockPath, Action_PathConnectionMaxLength, Action_Set | AI_Actions(3), AI_Conditions(1) |
| fn_8007818C | 0x22C |  | Menu_Navigate, Action_MoveAwayFromNode, Condition_EitherPlay | AI_Actions(2), Menu(1), AI_Conditions(1) |
| fn_800A76AC | 0x1700 |  | UI_DrawText, PPCMtwpar, Menu_AddOption, MemZero | Menu(1) |
| fn_800B6078 | 0xB44 | Action_MoveAwayFromNode | Action_CreateSplineCreatures, Action_BigJumpToLocator, Actio | AI_Actions(4) |
| fn_800B6D94 | 0xADC | Action_GoToLocator | Menu_Unknown1_HandleSelection, DebugMenu_BuildOptions, Menu_ | Menu(2), AI_Actions(1) |
| fn_800D197C | 0xF24 |  | Action_SetAreaFlag, Action_CannotBeSeen, NuAnimCurve2SetAppl | AI_Actions(2), Nu2(2) |
| fn_800E8B1C | 0x1998 |  | Action_SetMoveRadius, Action_FollowPath, Condition_OpponentI | AI_Actions(2), AI_Conditions(1) |
| fn_800F5C34 | 0xCA4 | GXSetCPUFifo, GXSetGPFifo | Action_RaceOpponent, Action_CameraCut | AI_Actions(2) |
| fn_800F99D4 | 0x96C | Condition_EitherPlayerUsingForceInit, Condition_ForceBeingUs |  | AI_Conditions(4) |
| fn_8010A318 | 0x878 | Menu_Unknown3_HandleSelection, Action_SetDontMove, Action_Ca |  | AI_Actions(3), Menu(1) |
| fn_80114AE8 | 0x2504 | ConfigureBlupUps, ConfigureRippleEffects, Action_CameraCut,  |  | AI_Actions(2) |
| fn_8014F058 | 0x218 | MemZero, PodraceLoadTimeTrialSettings | PodraceInitBoost, PodraceLoadSplineSettings |  |
| fn_8015EC44 | 0x188 | MemZero | Action_SetDoorFinished, Action_SetTaggable, Action_AttackBut | AI_Actions(3) |
| fn_8015EF84 | 0x484 |  | Action_SetRandomSpline, Action_SetFormationCommander, Action | AI_Actions(3) |
| fn_80162814 | 0x228 | MemZero, Menu_Unknown14_HandleSelection | MemZero, PodraceLoadLapSettings | Menu(1) |
| fn_8016B3E8 | 0x70 |  | Condition_CheckFlag, Condition_EitherPlayerOnObject, Conditi | AI_Conditions(4) |
| fn_8016C468 | 0x4B0 | Menu_AddOption | Action_SetSide, Action_SetHint, Action_SetTaggable | AI_Actions(3), Menu(1) |
| fn_80176688 | 0x2A0 |  | Action_EngageOpponent, Action_SetInvulnerable, Action_PlaceA | AI_Actions(4) |
| fn_800077E8 | 0x118 | NuDatOpenEx, NuMemAlloc, NuMemFree |  | Nu2(3) |
| fn_80011EC4 | 0x55C | NuAnimDataRead, NuAnimDataFindVersion | NuInitHardware | Nu2(3) |
| fn_80017EA0 | 0x6B8 | NuAnimCurve2CalcVal, NuDatFileLoadBuffer | NuSpecialFind | Nu2(3) |
| fn_8001DC4C | 0xA0 |  | NuAnimCurveSetApplyBlendToJoint2, NuAnimData2Fixup, NuAnimCu | Nu2(3) |
| fn_8001E430 | 0x33C | NuFileSeek | NuAnimKeyRead, NuAnimKeyLerp | Nu2(3) |
| fn_8001F804 | 0x2B4 |  | Action_FaceOpponent, NuAnimData2FixPtrs, NuAnimCurveSetLoad | Nu2(2), AI_Actions(1) |
| fn_80027FAC | 0x18C |  | NuAnimData2Relocate, NuAnimData2Fixup, NuAnimCurveSetLoad | Nu2(3) |
| fn_80029EC8 | 0x4E0 |  | NuGHGReadEx, NuAnimCurve2SetApplyBlendToJoint2, NuAnimCurve2 | Nu2(3) |
| fn_80042730 | 0x6F0 |  | instNuGCutDebrisLocatorUpdate, NuGCutLocatorSysFixUp, NuSoun | Nu2(2) |
| fn_800622F0 | 0x848 |  | Action_PathConnectionObstacle, Action_SetFullPathSearch, Act | AI_Actions(3) |
| fn_8006C748 | 0x298 | Menu_InitDefinitions | Action_SnapToOrigin, Action_SnapToPosition | AI_Actions(2), Menu(1) |
| fn_800712E4 | 0x384 | Action_UnLockDoor, Action_LockDoor, Menu_NavigateForward |  | AI_Actions(2), Menu(1) |
| fn_8007373C | 0xC38 | Menu_OpenFromRegistry | Menu_OpenFromRegistry, Menu_Open | Menu(3) |
| fn_800775B4 | 0x1BC |  | Action_SetState, Action_CheckWallSplines, Condition_IAm | AI_Actions(2), AI_Conditions(1) |
| fn_800778F0 | 0x1E4 |  | Condition_OpponentInTriggerArea, Condition_EitherPlayerInTri | AI_Conditions(3) |
| fn_80077AD4 | 0x160 | Menu_OpenFromRegistry | Action_PathConnectionObstacle, Action_ClearInterrupt | AI_Actions(2), Menu(1) |
| fn_800783B8 | 0x2A0 |  | Menu_Navigate, Condition_GotLocator, Condition_LocatorRangeI | AI_Conditions(2), Menu(1) |
| fn_80078B5C | 0x1AC | AIScriptXRefScript | Menu_RegisterDescriptors, Menu_ResetStack | Menu(2), AIScript(1) |
| fn_8007C534 | 0x370 | AIScriptParseConditions | AIScriptXRefScript, ScriptKeyword_STATE | AIScript(2), Script(1) |
| fn_8007CC34 | 0x220 | ScriptKeyword_PARAM | AIScriptLoadScp, AIScriptXRefScript | AIScript(2), Script(1) |
| fn_80083958 | 0xE4 | Action_SetInterrupt | Condition_LocatorRangeXZ, Condition_Random | AI_Conditions(2), AI_Actions(1) |
| fn_8008E650 | 0x474 |  | Action_FaceOpponent, Action_RetreatFromOpponent, Action_Idle | AI_Actions(3) |
| fn_8009AFE0 | 0x12C4 | Action_GoToDoorTrigger, Menu_FullReset | Menu_ResetAndReturn | Menu(2), AI_Actions(1) |
| fn_8009FB78 | 0x728 | Menu_ResetAndReturn, Action_PathConnectionMaxLength | Menu_FullReset | Menu(2), AI_Actions(1) |
| fn_800B9164 | 0x544 | Action_GoToNodeRandom | Condition_GotTriggerArea, Condition_Player2InTriggerArea | AI_Conditions(2), AI_Actions(1) |
| fn_800C8014 | 0x8CC | Menu_FullReset | Action_EngageObject, Action_CreateCreatures | AI_Actions(2), Menu(1) |
| fn_800D0964 | 0x810 | Menu_ResetAndReturn | NuAnimCurve2SetApplyBlendToJoint2TransLoc, NuAnimData2CalcMa | Nu2(2), Menu(1) |
| fn_800D4810 | 0x240 | Condition_CutSceneStartedInit, Menu_Unknown1_HandleSelection |  | AI_Conditions(1), Menu(1), Nu2(1) |
| fn_800D9BB8 | 0x8C0 | NuFileGetEndianSwap | NuAnimKeyRead, NuAnimCurve2SetApplyToJointTransLoc | Nu2(3) |
| fn_800DC454 | 0x72C | ConfigureBlupUps, NuFileRead, NuFileLoadBuffer |  | Nu2(2) |
| fn_800E5C14 | 0x5F4 | ConfigureBlupUps | Action_FollowOpponent, Action_MoveAwayFromOpponent | AI_Actions(2) |
| fn_800F68D8 | 0x3C0 |  | NuFilePos, NuFileLoadBuffer, Action_GoToLocator | Nu2(2), AI_Actions(1) |
| fn_80106CD8 | 0x210 | Action_SetHitPoints, GXInitFifoPtrs, GXSetCPUFifo |  | AI_Actions(1) |
| fn_801132E8 | 0x158C | CUT_FindCharacters, GXCPInterruptHandler, Action_CnxControll |  | AI_Actions(1) |
| fn_80119B00 | 0x5EC |  | ConfigureZipUps, PodraceInitBoost, NuGCutTriggerSysFixUp | Nu2(1) |
| fn_80135CBC | 0x70 | Condition_IAm_Level, Action_CameraCut | Condition_CategoryIs | AI_Conditions(2), AI_Actions(1) |
| fn_8013FCA0 | 0x71C | Menu_FullReset, __GXInitGX, Menu_Unknown12_BuildOptions |  | Menu(2) |
| fn_8015057C | 0x250 |  | Menu_AddOption, PodraceUpdateMine, MemZero | Menu(1) |
| fn_80161160 | 0x200 | MemZero | MemZero, Action_CnxController | AI_Actions(1) |
| fn_80162354 | 0x64 | MemZero | Action_DontPush_Level, PodraceUpdateMine | AI_Actions(1) |
| fn_80162D74 | 0x1BC | PodraceInitBoost | PodraceUpdateMine, MemZero |  |
| fn_8016CDE0 | 0x808 |  | Action_SetZeroAcceleration, Action_SetJumping, Action_AddPar | AI_Actions(3) |
| fn_8016E6A8 | 0x32C | Menu_FullReset | PodraceLoadLapSettings, CreditBufferAlloc | Menu(1) |
| fn_8016E9D4 | 0x140 |  | Action_DeflectPlayersPart, PodraceUpdateStartLights, Action_ | AI_Actions(2) |
| fn_8016EB14 | 0x110 |  | PodraceLoadSplineSettings, Action_ObjectController, Action_G | AI_Actions(2) |
| fn_8016EC24 | 0x114 |  | Action_CnxController, PodraceLoadSplineSettings, Action_Obje | AI_Actions(2) |
| fn_8017616C | 0xA8 |  | Action_CreateCreatures, Action_SnapToSockPosition, Action_Sn | AI_Actions(3) |
| fn_80176358 | 0xF0 |  | Action_SetDoomedEscapeLocator, AIPathCnxControllerCreate, Ac | AI_Actions(2) |
| fn_8017B5D8 | 0xB58 | Menu_NavigateForward, GXInit | GXCPInterruptHandler | Menu(1) |
| fn_8017C89C | 0x190 |  | ConfigureBlupUps, __GXInitRevisionBits, GXInit |  |

---
## Stats

| Metric | Value |
|--------|-------|
| Total functions | 4135 |
| Named | 571 |
| Unnamed | 3564 |
| Unnamed with named context | 804 |
| bl instructions | 30596 |
