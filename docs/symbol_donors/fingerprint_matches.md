# Function Fingerprint Matching Results

Matched 205 unnamed GC functions to Mac Action_/Condition_ functions
using callee-set overlap.

**Method:** For each unnamed GC function, compute the set of named functions it calls.
Compare against each Mac Action_/Condition_ function's callee set. Score by string
overlap of callee names (after stripping Action_/Condition_ prefix).

---
## Best Matches (score >= 1)

| GC Func | Size | Mac Match | Score | GC Callee Overlap | Mac Callees |
|---------|------|-----------|-------|-------------------|-------------|
| fn_80068F14 | 0x8FC | `Action_CircleOpponent` | 4.0 | Action_FaceOpponent, Action_RetreatFromOpponent, Action_Circ | Action_CircleOpponent, Action_MoveAwayFromPlayer, Action_Ret |
| fn_80069C40 | 0xA88 | `Action_Idle` | 3.0 | Action_FollowOpponent, Action_Idle, Action_MoveAwayFromOppon | Action_SetForceBack, Action_FollowOpponent, Action_SetForceL |
| fn_8008CF0C | 0xDD4 | `Action_BlockPath` | 3.0 | Action_GoToLocator, Action_PathConnectionMaxLength, Action_B | Action_BlockPath, Action_PathConnectionMaxLength, Action_GoT |
| fn_8016BEC0 | 0x274 | `Action_CnxController` | 3.0 | Action_GoToLevelPath, Action_ObjectController, Action_CnxCon | Action_GoToLevelPath, Action_ObjectController, Action_CnxCon |
| fn_8016B3E8 | 0x70 | `Condition_Side` | 2.5 | Condition_EitherPlayerIsInit, Condition_CheckFlag, Condition | Condition_CheckFlag, Condition_EitherPlayerIs, Condition_Eit |
| fn_80176448 | 0xF0 | `Action_BigJumpToLocator` | 2.5 | Action_BigJump, Action_BigJumpToLocator~BigJump, Action_BigJ | ReadHGX_v1, Action_BigJump, Action_BigJumpToLocator, Action_ |
| fn_8006C748 | 0x298 | `Action_SnapToPosition` | 2.0 | Action_SnapToOrigin, Action_SnapToPosition | Action_SnapToOrigin, ReadNUX_v1, Action_SnapToSockPosition,  |
| fn_80077C34 | 0x120 | `Action_DontPush` | 2.0 | Action_RequiresLOS, Action_CanSeeBehind | Action_AddToSet, MidSpecialMove, Action_SetNearestSpline, Ac |
| fn_80077E9C | 0x224 | `Action_BlockPath` | 2.0 | Action_PathConnectionMaxLength, Action_BlockPath | Action_BlockPath, Action_PathConnectionMaxLength, Action_GoT |
| fn_8007818C | 0x22C | `Action_GoToLocator` | 2.0 | Action_GoToLocator, Action_MoveAwayFromNode | NuFloor, Action_GoToLocator, Action_FollowPath, NuEquiv, Act |
| fn_8008DCE0 | 0x7B4 | `Action_BlockPath` | 2.0 | Action_GoToLocator, Action_BlockPath | Action_BlockPath, Action_PathConnectionMaxLength, Action_GoT |
| fn_8008E650 | 0x474 | `Action_CircleOpponent` | 2.0 | Action_RetreatFromOpponent, Action_FaceOpponent | Action_CircleOpponent, Action_MoveAwayFromPlayer, Action_Ret |
| fn_800AD4C4 | 0x28FC | `Action_Snipe` | 2.0 | Action_SetPathCnxFlag, Action_CameraCut | Action_Snipe, Action_SetPathCnxFlag, Action_CameraCut |
| fn_800C88E0 | 0x404 | `Action_BigJump` | 2.0 | Action_BigJump, Action_SetDoomedEscapeLocator | Draw_FORMATFAILED, Action_SnapToPosition, Action_BigJump, Ac |
| fn_800E4DF4 | 0x660 | `Action_GoToOrigin` | 2.0 | Action_GoToLocator, Action_SetLocator | Action_GoToLocator, Action_SetLocator, Action_ApplyGravity,  |
| fn_800E5C14 | 0x5F4 | `Action_Idle` | 2.0 | Action_FollowOpponent, Action_MoveAwayFromOpponent | Action_SetForceBack, Action_FollowOpponent, Action_SetForceL |
| fn_800EA630 | 0x364 | `Action_CircleOpponent` | 2.0 | Action_RetreatFromNearestOpponent, Action_RetreatFromOpponen | Action_CircleOpponent, Action_MoveAwayFromPlayer, Action_Ret |
| fn_800EA994 | 0x39C | `Action_Idle` | 2.0 | Action_FollowOpponent, Action_MoveAwayFromOpponent | Action_SetForceBack, Action_FollowOpponent, Action_SetForceL |
| fn_8012228C | 0x141C | `Action_CreateCreatures` | 2.0 | Action_CreateCreatures, Action_CreateSockCreatures | UpdateNewMiniKits, DrawStatusSuperKit, UpdateStatusPacket, M |
| fn_8016B458 | 0x110 | `Action_PlaceOnSock` | 2.0 | Action_PlaceOnSock, Action_CreateCreatures | DrawCutBorders, Action_PlaceOnSock, Action_SetZeroAccelerati |
| fn_8016B5E8 | 0x298 | `Action_EngageOpponent` | 2.0 | Action_PlaceOnSock, Action_EngageObject | Action_EngageOpponent, LoadLights, Action_PlaceOnSock, Actio |
| fn_8016BB98 | 0x98 | `Action_SnapToSockPosition` | 2.0 | Action_EngageOpponent, Action_SnapToSockPosition | Action_EngageOpponent, ReadNUX_v1, Action_SnapToSockPosition |
| fn_8016BC30 | 0x8C | `Action_SnapToSockPosition` | 2.0 | Action_EngageOpponent, Action_SnapToSockPosition | Action_EngageOpponent, ReadNUX_v1, Action_SnapToSockPosition |
| fn_8016EC24 | 0x114 | `Action_CnxController` | 2.0 | Action_ObjectController, Action_CnxController | Action_GoToLevelPath, Action_ObjectController, Action_CnxCon |
| fn_8016EE88 | 0x300 | `Action_RaceOpponent` | 2.0 | Action_RaceOpponent, Action_ZamMovement | Action_RaceOpponent, NuInitHardware, Action_ZamMovement, Act |
| fn_80173404 | 0x1B8 | `Action_CnxController` | 2.0 | Action_GoToLevelPath, Action_ObjectController | Action_GoToLevelPath, Action_ObjectController, Action_CnxCon |
| fn_80176298 | 0xC0 | `Action_BigJump` | 2.0 | Action_SetDoomedEscapeLocator, Action_SnapToPosition | Draw_FORMATFAILED, Action_SnapToPosition, Action_BigJump, Ac |
| fn_80176358 | 0xF0 | `Action_BigJump` | 2.0 | Action_BigJump, Action_SetDoomedEscapeLocator | Draw_FORMATFAILED, Action_SnapToPosition, Action_BigJump, Ac |
| fn_80176538 | 0xAC | `Action_SnapToLocator` | 2.0 | Action_SnapToLocator, Action_BigJumpToLocator | MenuUpdateLoad, ReadHGX_v1, Action_SetDoomedEscapeLocator, A |
| fn_800778F0 | 0x1E4 | `Condition_InTriggerArea` | 1.5 | Condition_EitherPlayerInTriggerArea~InTriggerArea, Condition | Condition_PlayerRange, Condition_InTriggerArea |
| fn_800B6078 | 0xB44 | `Action_BigJumpToLocator` | 1.5 | Action_BigJumpToLocator~BigJump, Action_BigJumpToLocator | ReadHGX_v1, Action_BigJump, Action_BigJumpToLocator, Action_ |
| fn_8012184C | 0x85C | `Condition_LocatedCollectable` | 1.5 | Condition_PlayerOnDoorTrigger, Condition_PlayerOnDoorTrigger | Condition_EitherPlayerOnDoorTrigger, Condition_IsSetAlive, C |
| fn_80123E98 | 0x5A0 | `Action_BigJumpToLocator` | 1.5 | Action_BigJumpToLocator~BigJump, Action_BigJumpToLocator | ReadHGX_v1, Action_BigJump, Action_BigJumpToLocator, Action_ |
| fn_8012AD98 | 0xF84 | `Action_SetLocator` | 1.5 | Action_SetLocator_Level~SetLocator, Action_SnapToLocator | Action_SetTableLocator, Action_GoToLocator, Action_SetCounte |
| fn_8015F408 | 0x19C | `Action_AddPart` | 1.5 | Action_AddPart~AddPartDebris, Action_AddPart | Action_AddPartDebris, Shadow_Map_System_release_objects, Sha |
| fn_8016CDE0 | 0x808 | `Action_AddPart` | 1.5 | Action_AddPart~AddPartDebris, Action_AddPart | Action_AddPartDebris, Shadow_Map_System_release_objects, Sha |
| fn_8016DB18 | 0x7C | `Condition_BeenToLevelInit` | 1.5 | Condition_BeenToLevelInit~BeenToLevel, Condition_BeenToLevel | Condition_BeenToLevel, Condition_MessageInit |
| fn_8016DCDC | 0x98 | `Condition_LocatedCollectable` | 1.5 | Condition_PlayerOnDoorTrigger, Condition_PlayerOnDoorTrigger | Condition_EitherPlayerOnDoorTrigger, Condition_IsSetAlive, C |
| fn_80176688 | 0x2A0 | `Action_PlaceAheadOfPlayer` | 1.5 | Action_SetLocator_Level~SetLocator, Action_PlaceAheadOfPlaye | LC_backr_bottom, Action_SetLocator, Action_SetCounterLocator |
| fn_80176ECC | 0x110 | `Action_BigJumpToLocator` | 1.5 | Action_BigJumpToLocator~BigJump, Action_BigJumpToLocator | ReadHGX_v1, Action_BigJump, Action_BigJumpToLocator, Action_ |
| fn_80007F00 | 0x58 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | Action_SetVisibility, Action_ZamMovement, TerrShapeSideStep, |
| fn_800081CC | 0xD8 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | Action_SetVisibility, Action_ZamMovement, TerrShapeSideStep, |
| fn_80009E94 | 0x3D4 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | Action_SetVisibility, Action_ZamMovement, TerrShapeSideStep, |
| fn_8000F9FC | 0x31C | `Action_SetHearDistance` | 1.0 | Action_SetHearDistance | Action_SetHearDistance, Action_SetMoveRadius, Action_JudderG |
| fn_8000FF20 | 0xD8 | `Action_Idle` | 1.0 | Action_Idle | Action_SetForceBack, Action_FollowOpponent, Action_SetForceL |
| fn_80011864 | 0x164 | `Action_GoToNode` | 1.0 | Action_GoToLocator | Action_GoToLocator, Action_GoToOrigin, Action_ZamMovement, A |
| fn_800119C8 | 0x4FC | `Action_GoToNode` | 1.0 | Action_GoToNode | Action_GoToLocator, Action_GoToOrigin, Action_ZamMovement, A |
| fn_80011EC4 | 0x55C | `Action_RaceOpponent` | 1.0 | NuInitHardware | Action_RaceOpponent, NuInitHardware, Action_ZamMovement, Act |
| fn_80012420 | 0xF4 | `Action_RaceOpponent` | 1.0 | NuInitHardware | Action_RaceOpponent, NuInitHardware, Action_ZamMovement, Act |
| fn_8001260C | 0x1CC | `Action_Idle` | 1.0 | Action_FollowOpponent | Action_SetForceBack, Action_FollowOpponent, Action_SetForceL |

---
## All Matches

- fn_80068F14 → `Action_CircleOpponent` (score=4.0, overlap=Action_FaceOpponent; Action_RetreatFromOpponent; Action_CircleOpponent; Action_MoveAwayFromPlayer)
- fn_80069C40 → `Action_Idle` (score=3.0, overlap=Action_FollowOpponent; Action_Idle; Action_MoveAwayFromOpponent)
- fn_8008CF0C → `Action_BlockPath` (score=3.0, overlap=Action_GoToLocator; Action_PathConnectionMaxLength; Action_BlockPath)
- fn_8016BEC0 → `Action_CnxController` (score=3.0, overlap=Action_GoToLevelPath; Action_ObjectController; Action_CnxController)
- fn_8016B3E8 → `Condition_Side` (score=2.5, overlap=Condition_EitherPlayerIsInit; Condition_CheckFlag; Condition_EitherPlayerIsInit~EitherPlayerIs)
- fn_80176448 → `Action_BigJumpToLocator` (score=2.5, overlap=Action_BigJump; Action_BigJumpToLocator~BigJump; Action_BigJumpToLocator)
- fn_8006C748 → `Action_SnapToPosition` (score=2.0, overlap=Action_SnapToOrigin; Action_SnapToPosition)
- fn_80077C34 → `Action_DontPush` (score=2.0, overlap=Action_RequiresLOS; Action_CanSeeBehind)
- fn_80077E9C → `Action_BlockPath` (score=2.0, overlap=Action_PathConnectionMaxLength; Action_BlockPath)
- fn_8007818C → `Action_GoToLocator` (score=2.0, overlap=Action_GoToLocator; Action_MoveAwayFromNode)
- fn_8008DCE0 → `Action_BlockPath` (score=2.0, overlap=Action_GoToLocator; Action_BlockPath)
- fn_8008E650 → `Action_CircleOpponent` (score=2.0, overlap=Action_RetreatFromOpponent; Action_FaceOpponent)
- fn_800AD4C4 → `Action_Snipe` (score=2.0, overlap=Action_SetPathCnxFlag; Action_CameraCut)
- fn_800C88E0 → `Action_BigJump` (score=2.0, overlap=Action_BigJump; Action_SetDoomedEscapeLocator)
- fn_800E4DF4 → `Action_GoToOrigin` (score=2.0, overlap=Action_GoToLocator; Action_SetLocator)
- fn_800E5C14 → `Action_Idle` (score=2.0, overlap=Action_FollowOpponent; Action_MoveAwayFromOpponent)
- fn_800EA630 → `Action_CircleOpponent` (score=2.0, overlap=Action_RetreatFromNearestOpponent; Action_RetreatFromOpponent)
- fn_800EA994 → `Action_Idle` (score=2.0, overlap=Action_FollowOpponent; Action_MoveAwayFromOpponent)
- fn_8012228C → `Action_CreateCreatures` (score=2.0, overlap=Action_CreateCreatures; Action_CreateSockCreatures)
- fn_8016B458 → `Action_PlaceOnSock` (score=2.0, overlap=Action_PlaceOnSock; Action_CreateCreatures)
- fn_8016B5E8 → `Action_EngageOpponent` (score=2.0, overlap=Action_PlaceOnSock; Action_EngageObject)
- fn_8016BB98 → `Action_SnapToSockPosition` (score=2.0, overlap=Action_EngageOpponent; Action_SnapToSockPosition)
- fn_8016BC30 → `Action_SnapToSockPosition` (score=2.0, overlap=Action_EngageOpponent; Action_SnapToSockPosition)
- fn_8016EC24 → `Action_CnxController` (score=2.0, overlap=Action_ObjectController; Action_CnxController)
- fn_8016EE88 → `Action_RaceOpponent` (score=2.0, overlap=Action_RaceOpponent; Action_ZamMovement)
- fn_80173404 → `Action_CnxController` (score=2.0, overlap=Action_GoToLevelPath; Action_ObjectController)
- fn_80176298 → `Action_BigJump` (score=2.0, overlap=Action_SetDoomedEscapeLocator; Action_SnapToPosition)
- fn_80176358 → `Action_BigJump` (score=2.0, overlap=Action_BigJump; Action_SetDoomedEscapeLocator)
- fn_80176538 → `Action_SnapToLocator` (score=2.0, overlap=Action_SnapToLocator; Action_BigJumpToLocator)
- fn_800778F0 → `Condition_InTriggerArea` (score=1.5, overlap=Condition_EitherPlayerInTriggerArea~InTriggerArea; Condition_PlayerInTriggerArea~InTriggerArea; Condition_OpponentInTriggerArea~InTriggerArea)
- fn_800B6078 → `Action_BigJumpToLocator` (score=1.5, overlap=Action_BigJumpToLocator~BigJump; Action_BigJumpToLocator)
- fn_8012184C → `Condition_LocatedCollectable` (score=1.5, overlap=Condition_PlayerOnDoorTrigger; Condition_PlayerOnDoorTrigger~EitherPlayerOnDoorTrigger)
- fn_80123E98 → `Action_BigJumpToLocator` (score=1.5, overlap=Action_BigJumpToLocator~BigJump; Action_BigJumpToLocator)
- fn_8012AD98 → `Action_SetLocator` (score=1.5, overlap=Action_SetLocator_Level~SetLocator; Action_SnapToLocator)
- fn_8015F408 → `Action_AddPart` (score=1.5, overlap=Action_AddPart~AddPartDebris; Action_AddPart)
- fn_8016CDE0 → `Action_AddPart` (score=1.5, overlap=Action_AddPart~AddPartDebris; Action_AddPart)
- fn_8016DB18 → `Condition_BeenToLevelInit` (score=1.5, overlap=Condition_BeenToLevelInit~BeenToLevel; Condition_BeenToLevel)
- fn_8016DCDC → `Condition_LocatedCollectable` (score=1.5, overlap=Condition_PlayerOnDoorTrigger; Condition_PlayerOnDoorTrigger~EitherPlayerOnDoorTrigger)
- fn_80176688 → `Action_PlaceAheadOfPlayer` (score=1.5, overlap=Action_SetLocator_Level~SetLocator; Action_PlaceAheadOfPlayer)
- fn_80176ECC → `Action_BigJumpToLocator` (score=1.5, overlap=Action_BigJumpToLocator~BigJump; Action_BigJumpToLocator)
- fn_80007F00 → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_800081CC → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_80009E94 → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_8000F9FC → `Action_SetHearDistance` (score=1.0, overlap=Action_SetHearDistance)
- fn_8000FF20 → `Action_Idle` (score=1.0, overlap=Action_Idle)
- fn_80011864 → `Action_GoToNode` (score=1.0, overlap=Action_GoToLocator)
- fn_800119C8 → `Action_GoToNode` (score=1.0, overlap=Action_GoToNode)
- fn_80011EC4 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_80012420 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_8001260C → `Action_Idle` (score=1.0, overlap=Action_FollowOpponent)
- fn_80019634 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_800197B8 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_80019A54 → `Action_TakeDamage` (score=1.0, overlap=NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_8001F548 → `Action_FaceOpponent` (score=1.0, overlap=Action_FaceOpponent)
- fn_8001F804 → `Action_FaceOpponent` (score=1.0, overlap=Action_FaceOpponent)
- fn_80025C38 → `Action_TakeDamage` (score=1.0, overlap=NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_800262BC → `Action_DontSetStoppedFlag` (score=1.0, overlap=instNuGCutCamSysUpdate)
- fn_80027834 → `Action_TakeDamage` (score=1.0, overlap=NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_8002933C → `Action_TakeDamage` (score=1.0, overlap=NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_8002BB60 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_8002EC3C → `Action_DontSetStoppedFlag` (score=1.0, overlap=instNuGCutCamSysUpdate)
- fn_80036858 → `Action_DontSetStoppedFlag` (score=1.0, overlap=instNuGCutCamSysUpdate)
- fn_80037364 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_80038210 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_80038628 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_80039FAC → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_8003A134 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_8003B874 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_8003BA5C → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_8003C754 → `Action_DontSetStoppedFlag` (score=1.0, overlap=instNuGCutCamSysUpdate)
- fn_8003D4F0 → `Action_SetScriptState` (score=1.0, overlap=instNuGCutDebrisLocatorUpdate)
- fn_8003DD80 → `Action_SetScriptState` (score=1.0, overlap=instNuGCutDebrisLocatorUpdate)
- fn_8003E328 → `Action_DontSetStoppedFlag` (score=1.0, overlap=instNuGCutCamSysUpdate)
- fn_80042730 → `Action_SetScriptState` (score=1.0, overlap=instNuGCutDebrisLocatorUpdate)
- fn_8004D4D0 → `Action_CirclePlayer` (score=1.0, overlap=Action_MoveAwayFromPlayer)
- fn_80053D70 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_800622F0 → `Action_FacePlayer` (score=1.0, overlap=Action_IgnoreWallSplines)
- fn_800775B4 → `Condition_InterruptID` (score=1.0, overlap=Action_SetState)
- fn_80077770 → `Condition_OpponentBelow` (score=1.0, overlap=Condition_OriginRange)
- fn_80077AD4 → `Action_SetInterrupt` (score=1.0, overlap=Action_ClearInterrupt)
- fn_80077D54 → `Action_NoTerrain` (score=1.0, overlap=Action_NoLosCheck)
- fn_800780C0 → `Action_BlockPath` (score=1.0, overlap=Action_BlockPath)
- fn_800783B8 → `Condition_LocatorRangeXZ` (score=1.0, overlap=Condition_GotLocator)
- fn_80078EE4 → `Action_GoToNode` (score=1.0, overlap=Action_GoToOrigin)
- fn_8007922C → `Action_SetViewDistance` (score=1.0, overlap=Action_SetMinViewHeight)
- fn_80079304 → `Action_FaceOpponent` (score=1.0, overlap=Action_FaceOpponent)
- fn_800793C0 → `Action_FaceOpponent` (score=1.0, overlap=Action_FaceOpponent)
- fn_80079434 → `Action_FaceOpponent` (score=1.0, overlap=Action_FaceOpponent)
- fn_800794E8 → `Action_CirclePlayer` (score=1.0, overlap=Action_RetreatFromNearestOpponent)
- fn_8007F354 → `Action_Idle` (score=1.0, overlap=Action_Idle)
- fn_80083958 → `Condition_LocatorRange` (score=1.0, overlap=Condition_LocatorRangeXZ)
- fn_80086A04 → `Action_GoToNode` (score=1.0, overlap=Action_GoToLocator)
- fn_8008F45C → `Action_GoToNode` (score=1.0, overlap=Action_GoToOrigin)
- fn_800A4E68 → `Action_RaceOpponent` (score=1.0, overlap=NuInitHardware)
- fn_800AAD10 → `Action_SetState` (score=1.0, overlap=Action_CreateCreatures)
- fn_800B3980 → `Action_CanOpenDoors` (score=1.0, overlap=Action_CanShootOffScreen)
- fn_800B9164 → `Condition_LevelNodeRange` (score=1.0, overlap=Condition_GotTriggerArea)
- fn_800BA74C → `Action_SetViewDistance` (score=1.0, overlap=Action_SetMaxViewHeight)
- fn_800BA87C → `Action_Idle` (score=1.0, overlap=Action_Idle)
- fn_800BC4FC → `Action_SetState` (score=1.0, overlap=Action_CreateCreatures)
- fn_800C5B9C → `Action_BoulderSection` (score=1.0, overlap=Action_BoulderSection)
- fn_800C757C → `Action_DontPush` (score=1.0, overlap=Action_PressJumpButton)
- fn_800C7DB4 → `Action_AttackOpponent` (score=1.0, overlap=Action_EngageOpponent)
- fn_800C8014 → `Action_SetState` (score=1.0, overlap=Action_CreateCreatures)
- fn_800CAC5C → `Action_SetScriptState` (score=1.0, overlap=Action_SetAIOverrideControl)
- fn_800CB8F0 → `Action_MoveAwayFromNode` (score=1.0, overlap=Action_OverrideAnimation)
- fn_800D0964 → `Action_TakeDamage` (score=1.0, overlap=NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_800D197C → `Action_SetFlag` (score=1.0, overlap=Action_SetAreaFlag)
- fn_800DA478 → `Action_TakeDamage` (score=1.0, overlap=NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_800DF360 → `Action_FaceOpponent` (score=1.0, overlap=Action_NoTerrain)
- fn_800DFC04 → `Action_GoToNode` (score=1.0, overlap=Action_GoToLocator)
- fn_800E0790 → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_800E8B1C → `Action_GoToLocator` (score=1.0, overlap=Action_FollowPath)
- fn_800EA4B4 → `Action_FaceOpponent` (score=1.0, overlap=Action_FaceOpponent)
- fn_800F0E34 → `Action_CirclePlayer` (score=1.0, overlap=Action_RetreatFromNearestOpponent)
- fn_800F3A84 → `Action_BigJump` (score=1.0, overlap=Action_SnapToPosition)
- fn_800F4E8C → `Action_SetLocator` (score=1.0, overlap=Action_SnapToLocator)
- fn_800F5504 → `Condition_LocatedCollectable` (score=1.0, overlap=Condition_LocatedCollectable)
- fn_800F5C34 → `Action_Snipe` (score=1.0, overlap=Action_CameraCut)
- fn_800F68D8 → `Action_GoToNode` (score=1.0, overlap=Action_GoToLocator)
- fn_8010980C → `Action_TakeDamage` (score=1.0, overlap=NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_801171A0 → `Action_GoToLevelPath` (score=1.0, overlap=Action_GoToOriginalPath)
- fn_8011FCCC → `Action_AddPartDebris` (score=1.0, overlap=Action_SetHoverPhase)
- fn_80124438 → `Action_SetBoss` (score=1.0, overlap=Action_SetSide)
- fn_8012C1C8 → `Action_GoToDoorTrigger` (score=1.0, overlap=Action_GoToDoorTrigger)
- fn_8012D23C → `Condition_LocatedCollectable` (score=1.0, overlap=Condition_LocatedCollectable)
- fn_80135CBC → `Condition_IAm` (score=1.0, overlap=Condition_CategoryIs)
- fn_80136D84 → `Action_Idle` (score=1.0, overlap=Action_Idle)
- fn_8013D550 → `Action_FaceCamera` (score=1.0, overlap=Action_FaceCharacter)
- fn_8013EF38 → `Action_SetJumping` (score=1.0, overlap=Action_Kill)
- fn_80143ED8 → `Action_OpenDoor` (score=1.0, overlap=Action_DoorSetStayOpen)
- fn_80145914 → `Condition_FurthestPlayerDistanceAlongSock` (score=1.0, overlap=Condition_IsSplineSet)
- fn_80146B8C → `Action_Snipe` (score=1.0, overlap=Action_CameraCut)
- fn_80146D5C → `Condition_AIOverrideControl` (score=1.0, overlap=Condition_IsOnScreenInit)
- fn_8015E224 → `Action_CreateCreatures` (score=1.0, overlap=Action_CreateSockCreatures)
- fn_8015EC44 → `Action_SetTaggable` (score=1.0, overlap=Action_SetTaggable)
- fn_8015EE80 → `Action_RaceOpponent` (score=1.0, overlap=Action_JudderGameCamera)
- fn_8015EF84 → `Action_SetRandomSpline` (score=1.0, overlap=Action_SetRandomSpline)
- fn_801601AC → `Condition_Context` (score=1.0, overlap=Condition_AngleFromPlayerToMe)
- fn_80161160 → `Action_CnxController` (score=1.0, overlap=Action_CnxController)
- fn_801672A8 → `Action_ApplyGravity` (score=1.0, overlap=Action_IgnoreShoveSystem)
- fn_801680CC → `Action_RaceOpponent` (score=1.0, overlap=Action_JudderGameCamera)
- fn_80168DC8 → `Action_AttackButtonMash` (score=1.0, overlap=Action_CanHitForceObjects)
- fn_80168E0C → `Action_SetDefensive` (score=1.0, overlap=Action_Respawnable)
- fn_80168F9C → `Action_SetSide` (score=1.0, overlap=Action_UpdateSockPos)
- fn_80169088 → `Condition_FromNode` (score=1.0, overlap=Condition_ToNodeInit)
- fn_80169D08 → `Action_SetDontMove` (score=1.0, overlap=Action_CantDie)
- fn_80169DE4 → `Action_SetLastSafePathPos` (score=1.0, overlap=Action_SetLastSafePathPos)
- fn_80169EB4 → `Action_SetJumping` (score=1.0, overlap=Action_Kill)
- fn_80169F88 → `Action_SetSide` (score=1.0, overlap=Action_SetRespawnLocator)
- fn_8016A044 → `Action_SetDefensive` (score=1.0, overlap=Action_Respawnable)
- fn_8016AEDC → `Action_Kill` (score=1.0, overlap=Action_SetScriptState)
- fn_8016B044 → `Action_CreateCreatures` (score=1.0, overlap=Action_CreateSockCreatures)
- fn_8016B084 → `Condition_Blocking` (score=1.0, overlap=Condition_BeenHit)
- fn_8016B880 → `Action_FollowSpline` (score=1.0, overlap=Action_FollowSpline)
- fn_8016B940 → `Action_AttackOpponent` (score=1.0, overlap=Action_EngageOpponent)
- fn_8016B9E4 → `Action_AttackOpponent` (score=1.0, overlap=Action_EngageOpponent)
- fn_8016BA70 → `Action_EngageObject` (score=1.0, overlap=Action_EngageObject)
- fn_8016BAFC → `Action_EngageObject` (score=1.0, overlap=Action_EngageObject)
- fn_8016BCBC → `Action_SnapToOrigin` (score=1.0, overlap=Action_SnapToOrigin)
- fn_8016C134 → `Action_Idle` (score=1.0, overlap=Action_SetForceBack)
- fn_8016C178 → `Action_UseForce` (score=1.0, overlap=Action_DeflectPlayersPart)
- fn_8016C468 → `Action_SetBoss` (score=1.0, overlap=Action_SetSide)
- fn_8016C918 → `Action_CameraShake` (score=1.0, overlap=Action_PlayCutScene)
- fn_8016D5E8 → `Action_Respawnable` (score=1.0, overlap=Action_SetBoss)
- fn_8016D97C → `Condition_CutSceneFinished` (score=1.0, overlap=Condition_CutScenePlayingInit)
- fn_8016DC40 → `Condition_Context` (score=1.0, overlap=Condition_AngleFromPlayerToMe)
- fn_8016DD74 → `Condition_Blocking` (score=1.0, overlap=Condition_BeenHit)
- fn_8016E504 → `Action_Snipe` (score=1.0, overlap=Action_CameraCut)
- fn_8016E9D4 → `Action_UseForce` (score=1.0, overlap=Action_DeflectPlayersPart)
- fn_8016EB14 → `Action_CnxController` (score=1.0, overlap=Action_ObjectController)
- fn_8016ED38 → `Condition_AngleFromPlayerToMe` (score=1.0, overlap=Condition_NumBaddies)
- fn_8016F188 → `Action_AddPartDebris` (score=1.0, overlap=Action_SetMaxMovementRange)
- fn_8016FB68 → `Action_SetState` (score=1.0, overlap=Action_CreateCreatures)
- fn_8016FBE4 → `Action_FollowSock` (score=1.0, overlap=Action_FollowSock)
- fn_8016FC28 → `Action_PlaceOnSock` (score=1.0, overlap=Action_PlaceOnSock)
- fn_8016FCE0 → `Action_PlaceOnSock` (score=1.0, overlap=Action_PlaceOnSock)
- fn_8016FDC8 → `Action_FollowSpline` (score=1.0, overlap=Action_FollowSpline)
- fn_8016FEBC → `Action_SetInvulnerable` (score=1.0, overlap=Action_SetInvulnerable)
- fn_8016FFF8 → `Action_SetState` (score=1.0, overlap=Action_CreateCreatures)
- fn_801704C8 → `Action_SnapToPosition` (score=1.0, overlap=Action_SnapToSockPosition)
- fn_8017050C → `Action_SnapToOrigin` (score=1.0, overlap=Action_SnapToOrigin)
- fn_801705FC → `Action_BigJump` (score=1.0, overlap=Action_SnapToPosition)
- fn_801735BC → `Action_CnxController` (score=1.0, overlap=Action_CnxController)
- fn_80173684 → `Action_CnxController` (score=1.0, overlap=Action_CnxController)
- fn_801736DC → `Action_RaceOpponent` (score=1.0, overlap=Action_RaceOpponent)
- fn_80173708 → `Action_BigJump` (score=1.0, overlap=Action_BigJump)
- fn_80173B74 → `Action_CreateCreatures` (score=1.0, overlap=Action_CreateSockCreatures)
- fn_80176054 → `Action_SetState` (score=1.0, overlap=Action_CreateCreatures)
- fn_8017616C → `Action_SetState` (score=1.0, overlap=Action_CreateCreatures)
- fn_80176928 → `Action_SnapToPosition` (score=1.0, overlap=Action_SnapToSockPosition)
- fn_80176A38 → `Action_SnapToPosition` (score=1.0, overlap=Action_SnapToSockPosition)
- fn_80176C5C → `Action_BigJump` (score=1.0, overlap=Action_SnapToPosition)
- fn_80176D90 → `Action_BigJump` (score=1.0, overlap=Action_SetDoomedEscapeLocator)
- fn_80029EC8 → `Action_TakeDamage` (score=0.5, overlap=NuAnimCurve2SetApplyBlendToJoint2~NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_80084058 → `Condition_LevelNodeRangeInit` (score=0.5, overlap=Condition_NodeRange~LevelNodeRange)
- fn_800B96A8 → `Action_SetRandomSpline` (score=0.5, overlap=Condition_Random~SetRandomSpline)
- fn_800BD100 → `Action_GoToOrigin` (score=0.5, overlap=Action_SetLocator_Level~SetLocator)
- fn_800EC1C8 → `Action_TakeDamage` (score=0.5, overlap=NuAnimCurve2SetApplyBlendToJoint2~NuAnimCurve2SetApplyBlendToJoint2TransLoc)
- fn_800FAB34 → `Action_GoToOrigin` (score=0.5, overlap=Action_SetLocator_Level~SetLocator)
- fn_8015CB3C → `Condition_ForceBeingUsed` (score=0.5, overlap=Condition_ForceComplete~ForceCompleteInit)
- fn_80162354 → `Action_DontPush` (score=0.5, overlap=Action_DontPush_Level~DontPush)
- fn_8016DAAC → `Condition_Message` (score=0.5, overlap=Condition_AnimationFinished~AnimationFinishedInit)
- fn_80175F7C → `Action_GoToOrigin` (score=0.5, overlap=Action_SetLocator_Level~SetLocator)
- fn_80176214 → `Action_GoToOrigin` (score=0.5, overlap=Action_SetLocator_Level~SetLocator)

---
## Stats

| Metric | Value |
|--------|-------|
| GC unnamed functions | 3565 |
| GC named functions | 570 |
| GC unnamed with named callees | 3565 |
| Mac Action_/Condition_ functions | 540 |
| Raw matches | 855 |
| Best matches | 205 |
