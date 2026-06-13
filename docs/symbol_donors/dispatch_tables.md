# GC Dispatch Table Recovery Report

Extracted 329 dispatch entries from 6 tables.
Cross-referenced with Mac prototype. Found 319 rename candidates.

---

## Rename Candidates (unnamed GC function → Mac name)

| Table | Idx | String | Cur Func | Proposed Name | Confidence |
|-------|-----|--------|----------|---------------|------------|
| AI_Actions_API | 0 | `Idle` | fn_8007F8A8 | `Action_Idle` | HIGH |
| AI_Actions_API | 1 | `SetState` | fn_80084C88 | `Action_SetState` | HIGH |
| AI_Actions_API | 2 | `ResetTimer` | fn_80084D24 | `Action_ResetTimer` | HIGH |
| AI_Actions_API | 3 | `RetreatFromNearestOpponent` | fn_800809AC | `Action_RetreatFromNearestOpponent` | HIGH |
| AI_Actions_API | 4 | `RetreatFromOpponent` | fn_80080800 | `Action_RetreatFromOpponent` | HIGH |
| AI_Actions_API | 5 | `MoveAwayFromPlayer` | fn_8008063C | `Action_MoveAwayFromPlayer` | HIGH |
| AI_Actions_API | 6 | `CircleOpponent` | fn_800803CC | `Action_CircleOpponent` | HIGH |
| AI_Actions_API | 7 | `CirclePlayer` | fn_8008014C | `Action_CirclePlayer` | HIGH |
| AI_Actions_API | 8 | `FollowPlayer` | fn_8007FF9C | `Action_FollowPlayer` | HIGH |
| AI_Actions_API | 9 | `MoveAwayFromOpponent` | fn_8007FDBC | `Action_MoveAwayFromOpponent` | HIGH |
| AI_Actions_API | 10 | `FollowOpponent` | fn_8007FC1C | `Action_FollowOpponent` | HIGH |
| AI_Actions_API | 11 | `FacePlayer` | fn_80084D40 | `Action_FacePlayer` | HIGH |
| AI_Actions_API | 12 | `FaceOpponent` | fn_80080B50 | `Action_FaceOpponent` | HIGH |
| AI_Actions_API | 13 | `IgnoreWallSplines` | fn_80084EA8 | `Action_IgnoreWallSplines` | HIGH |
| AI_Actions_API | 14 | `CheckWallSplines` | fn_80084F2C | `Action_CheckWallSplines` | HIGH |
| AI_Actions_API | 15 | `NoTerrain` | fn_80084FB0 | `Action_NoTerrain` | HIGH |
| AI_Actions_API | 16 | `FlatTerrain` | fn_800850D8 | `Action_FlatTerrain` | HIGH |
| AI_Actions_API | 17 | `ShadowTerrain` | fn_8008516C | `Action_ShadowTerrain` | HIGH |
| AI_Actions_API | 18 | `DontUseShadowTerrain` | fn_80085200 | `Action_DontUseShadowTerrain` | HIGH |
| AI_Actions_API | 19 | `DontPush` | fn_80085294 | `Action_DontPush` | HIGH |
| AI_Actions_API | 20 | `CanSeeBehind` | fn_80085328 | `Action_CanSeeBehind` | HIGH |
| AI_Actions_API | 21 | `RequiresLOS` | fn_800853BC | `Action_RequiresLOS` | HIGH |
| AI_Actions_API | 22 | `SetFullPathSearch` | fn_80085440 | `Action_SetFullPathSearch` | HIGH |
| AI_Actions_API | 23 | `SetViewDistance` | fn_80081068 | `Action_SetViewDistance` | HIGH |
| AI_Actions_API | 24 | `SetMaxViewHeight` | fn_800813B0 | `Action_SetMaxViewHeight` | HIGH |
| AI_Actions_API | 25 | `SetMinViewHeight` | fn_8008120C | `Action_SetMinViewHeight` | HIGH |
| AI_Actions_API | 26 | `SetHearDistance` | fn_80081554 | `Action_SetHearDistance` | HIGH |
| AI_Actions_API | 27 | `SetMoveRadius` | fn_800854D4 | `Action_SetMoveRadius` | HIGH |
| AI_Actions_API | 28 | `GoToNode` | fn_800816F8 | `Action_GoToNode` | HIGH |
| AI_Actions_API | 29 | `GoToNodeRandom` | fn_800819A4 | `Action_GoToNodeRandom` | HIGH |
| AI_Actions_API | 30 | `GoToOrigin` | fn_80081B7C | `Action_GoToOrigin` | HIGH |
| AI_Actions_API | 31 | `GoToLocator` | fn_800820DC | `Action_GoToLocator` | HIGH |
| AI_Actions_API | 32 | `SetLocator` | fn_80081E00 | `Action_SetLocator` | HIGH |
| AI_Actions_API | 33 | `FollowPath` | fn_80085628 | `Action_FollowPath` | HIGH |
| AI_Actions_API | 34 | `MoveAwayFromNode` | fn_8008296C | `Action_MoveAwayFromNode` | HIGH |
| AI_Actions_API | 35 | `OverrideAnimation` | fn_80082C18 | `Action_OverrideAnimation` | HIGH |
| AI_Actions_API | 36 | `BlockPath` | fn_80082E18 | `Action_BlockPath` | HIGH |
| AI_Actions_API | 37 | `PathConnectionObstacle` | fn_800856F8 | `Action_PathConnectionObstacle` | HIGH |
| AI_Actions_API | 38 | `PathConnectionMaxLength` | fn_80083190 | `Action_PathConnectionMaxLength` | HIGH |
| AI_Actions_API | 39 | `NoLosCheck` | fn_80085044 | `Action_NoLosCheck` | HIGH |
| AI_Actions_API | 40 | `ResetToOrigin` | fn_800857FC | `Action_ResetToOrigin` | HIGH |
| AI_Actions_API | 41 | `SetInterrupt` | fn_800833DC | `Action_SetInterrupt` | HIGH |
| AI_Actions_API | 42 | `ClearInterrupt` | fn_8008584C | `Action_ClearInterrupt` | HIGH |
| AI_Actions_API | 43 | `SetIgnoreAntinodes` | fn_800858D8 | `Action_SetIgnoreAntinodes` | HIGH |
| AI_Actions_API | 44 | `NoShadows` | fn_800859BC | `Action_NoShadows` | HIGH |
| AI_Conditions_API | 0 | `LocatorRange` | fn_80083AC8 | `Condition_LocatorRange` | HIGH |
| AI_Conditions_API | 1 | `LocatorRangeXZ` | fn_80083B2C | `Condition_LocatorRangeXZ` | HIGH |
| AI_Conditions_API | 2 | `Timer` | fn_80083A6C | `Condition_Timer` | HIGH |
| AI_Conditions_API | 3 | `Random` | fn_80083A74 | `Condition_Random` | HIGH |
| AI_Conditions_API | 4 | `GotLocator` | fn_80083B90 | `Condition_GotLocator` | HIGH |
| AI_Conditions_API | 5 | `CurrentLocatorIs` | fn_80083C38 | `Condition_CurrentLocatorIs` | HIGH |
| AI_Conditions_API | 6 | `InTriggerArea` | fn_80083CA0 | `Condition_InTriggerArea` | HIGH |
| AI_Conditions_API | 7 | `PlayerRange` | fn_80083D30 | `Condition_PlayerRange` | HIGH |
| AI_Conditions_API | 8 | `NearestPlayerRange` | fn_80083D90 | `Condition_NearestPlayerRange` | HIGH |
| AI_Conditions_API | 9 | `InLevelNode` | fn_80083EA0 | `Condition_InLevelNode` | HIGH |
| AI_Conditions_API | 10 | `PlayerInLevelNode` | fn_80083F4C | `Condition_PlayerInLevelNode` | HIGH |
| AI_Conditions_API | 11 | `EitherPlayerInLevelNode` | fn_80083FB4 | `Condition_EitherPlayerInLevelNode` | HIGH |
| AI_Conditions_API | 13 | `GotTriggerArea` | fn_800840FC | `Condition_GotTriggerArea` | HIGH |
| AI_Conditions_API | 14 | `PlayerInTriggerArea` | fn_8008415C | `Condition_PlayerInTriggerArea` | HIGH |
| AI_Conditions_API | 15 | `Player2InTriggerArea` | fn_80084200 | `Condition_Player2InTriggerArea` | HIGH |
| AI_Conditions_API | 16 | `EitherPlayerInTriggerArea` | fn_800842A4 | `Condition_EitherPlayerInTriggerArea` | HIGH |
| AI_Conditions_API | 17 | `BaddyInTriggerArea` | fn_800843B0 | `Condition_BaddyInTriggerArea` | HIGH |
| AI_Conditions_API | 18 | `GoodyInTriggerArea` | fn_80084430 | `Condition_GoodyInTriggerArea` | HIGH |
| AI_Conditions_API | 19 | `OpponentInTriggerArea` | fn_800844B0 | `Condition_OpponentInTriggerArea` | HIGH |
| AI_Conditions_API | 20 | `GotOpponent` | fn_8008454C | `Condition_GotOpponent` | HIGH |
| AI_Conditions_API | 21 | `OpponentIsAThreat` | fn_80084578 | `Condition_OpponentIsAThreat` | HIGH |
| AI_Conditions_API | 22 | `OpponentOnSamePath` | fn_800845B0 | `Condition_OpponentOnSamePath` | HIGH |
| AI_Conditions_API | 23 | `OpponentRange` | fn_80084600 | `Condition_OpponentRange` | HIGH |
| AI_Conditions_API | 24 | `NearestOpponentRange` | fn_80084628 | `Condition_NearestOpponentRange` | HIGH |
| AI_Conditions_API | 25 | `YawToOpponent` | fn_80084650 | `Condition_YawToOpponent` | HIGH |
| AI_Conditions_API | 26 | `OpponentBelow` | fn_80084700 | `Condition_OpponentBelow` | HIGH |
| AI_Conditions_API | 27 | `OriginRange` | fn_8008474C | `Condition_OriginRange` | HIGH |
| AI_Conditions_API | 28 | `OpponentToOrigin` | fn_800847B8 | `Condition_OpponentToOrigin` | HIGH |
| AI_Conditions_API | 29 | `PlayerToOrigin` | fn_80084824 | `Condition_PlayerToOrigin` | HIGH |
| AI_Conditions_API | 30 | `OpponentToLocator` | fn_800848C4 | `Condition_OpponentToLocator` | HIGH |
| AI_Conditions_API | 31 | `OpponentToLocatorXZ` | fn_80084928 | `Condition_OpponentToLocatorXZ` | HIGH |
| AI_Conditions_API | 32 | `OpponentToLocatorY` | fn_8008498C | `Condition_OpponentToLocatorY` | HIGH |
| AI_Conditions_API | 33 | `PlayerToLocator` | fn_80084A08 | `Condition_PlayerToLocator` | HIGH |
| AI_Conditions_API | 34 | `NearestPlayerToLocator` | fn_80084A74 | `Condition_NearestPlayerToLocator` | HIGH |
| AI_Conditions_API | 35 | `OnPath` | fn_80083BBC | `Condition_OnPath` | HIGH |
| AI_Conditions_API | 36 | `TimeOffPath` | fn_80083BE8 | `Condition_TimeOffPath` | HIGH |
| AI_Conditions_API | 37 | `PathBlocked` | fn_80084B3C | `Condition_PathBlocked` | HIGH |
| AI_Conditions_API | 38 | `InterruptID` | fn_80084B68 | `Condition_InterruptID` | HIGH |
| AI_Conditions_API | 39 | `IAm` | fn_80084BEC | `Condition_IAm` | HIGH |
| AI_Conditions_API | 40 | `StuckTime` | fn_80084C60 | `Condition_StuckTime` | HIGH |
| Level_Actions | 0 | `Activate` | fn_80137EAC | `Action_Activate` | HIGH |
| Level_Actions | 1 | `DeActivate` | fn_80137F28 | `Action_DeActivate` | HIGH |
| Level_Actions | 2 | `GoToLevelPath` | fn_8013417C | `Action_GoToLevelPath` | HIGH |
| Level_Actions | 3 | `SetPath` | fn_8013ABF0 | `Action_SetPath` | HIGH |
| Level_Actions | 4 | `GoToOriginalPath` | fn_801342CC | `Action_GoToOriginalPath` | HIGH |
| Level_Actions | 5 | `GoToCollectable` | fn_80139154 | `Action_GoToCollectable` | HIGH |
| Level_Actions | 6 | `DumpCollectables` | fn_8013923C | `Action_DumpCollectables` | HIGH |
| Level_Actions | 7 | `SnapToLocator` | fn_8012E5DC | `Action_SnapToLocator` | HIGH |
| Level_Actions | 8 | `SetLocator` | fn_8012E2F4 | `Action_SetLocator` | HIGH |
| Level_Actions | 9 | `SetCounterLocator` | fn_80139278 | `Action_SetCounterLocator` | HIGH |
| Level_Actions | 10 | `SetTableLocator` | fn_80139324 | `Action_SetTableLocator` | HIGH |
| Level_Actions | 11 | `CallDexter` | fn_80139388 | `Action_CallDexter` | HIGH |
| Level_Actions | 12 | `SnapToOrigin` | fn_8012F3AC | `Action_SnapToOrigin` | HIGH |
| Level_Actions | 13 | `BigJumpToLocator` | fn_8012E9AC | `Action_BigJumpToLocator` | HIGH |
| Level_Actions | 14 | `BigJump` | fn_8012EC80 | `Action_BigJump` | HIGH |
| Level_Actions | 15 | `SetDoomedEscapeLocator` | fn_8012EE28 | `Action_SetDoomedEscapeLocator` | HIGH |
| Level_Actions | 16 | `SnapToPosition` | fn_8012F090 | `Action_SnapToPosition` | HIGH |
| Level_Actions | 17 | `SnapToSockPosition` | fn_8012F5B8 | `Action_SnapToSockPosition` | HIGH |
| Level_Actions | 18 | `SetAnimation` | fn_8013ACA4 | `Action_SetAnimation` | HIGH |
| Level_Actions | 19 | `AnimTimeRandom` | fn_8013AD20 | `Action_AnimTimeRandom` | HIGH |
| Level_Actions | 20 | `LockDoor` | fn_8013AD6C | `Action_LockDoor` | HIGH |
| Level_Actions | 21 | `UnLockDoor` | fn_8013ADA8 | `Action_UnLockDoor` | HIGH |
| Level_Actions | 22 | `CloseDoor` | fn_8013ADE4 | `Action_CloseDoor` | HIGH |
| Level_Actions | 23 | `OpenDoor` | fn_8013AE20 | `Action_OpenDoor` | HIGH |
| Level_Actions | 24 | `DoorSetStayOpen` | fn_8013AEC8 | `Action_DoorSetStayOpen` | HIGH |
| Level_Actions | 25 | `SetDoorFinished` | fn_8013AF84 | `Action_SetDoorFinished` | HIGH |
| Level_Actions | 26 | `ActivateGadget` | fn_8013B048 | `Action_ActivateGadget` | HIGH |
| Level_Actions | 27 | `SetObstacleChainPhase` | fn_8013B0BC | `Action_SetObstacleChainPhase` | HIGH |
| Level_Actions | 28 | `EndLoopingAnimation` | fn_8013B18C | `Action_EndLoopingAnimation` | HIGH |
| Level_Actions | 29 | `CanOpenDoors` | fn_8013B1FC | `Action_CanOpenDoors` | HIGH |
| Level_Actions | 30 | `CanShootOffScreen` | fn_8013B2C0 | `Action_CanShootOffScreen` | HIGH |
| Level_Actions | 31 | `CanCollectCoins` | fn_8013B384 | `Action_CanCollectCoins` | HIGH |
| Level_Actions | 32 | `KeepWeaponOut` | fn_8013B448 | `Action_KeepWeaponOut` | HIGH |
| Level_Actions | 33 | `SnapWeaponOut` | fn_8013B520 | `Action_SnapWeaponOut` | HIGH |
| Level_Actions | 34 | `ResetContext` | fn_8013B680 | `Action_ResetContext` | HIGH |
| Level_Actions | 35 | `PrefersPlayers` | fn_8013B734 | `Action_PrefersPlayers` | HIGH |
| Level_Actions | 36 | `SetBoltsDontGetDeflectedBack` | fn_8013B7CC | `Action_SetBoltsDontGetDeflectedBack` | HIGH |
| Level_Actions | 37 | `AttackButtonMash` | fn_8013B864 | `Action_AttackButtonMash` | HIGH |
| Level_Actions | 38 | `CanShootObstructions` | fn_8013B8FC | `Action_CanShootObstructions` | HIGH |
| Level_Actions | 39 | `SetTaggable` | fn_8013BA2C | `Action_SetTaggable` | HIGH |
| Level_Actions | 40 | `CannotDropIn` | fn_8013BB74 | `Action_CannotDropIn` | HIGH |
| Level_Actions | 41 | `CanAttack` | fn_8013BC84 | `Action_CanAttack` | HIGH |
| Level_Actions | 42 | `TakeDamage` | fn_8013BD64 | `Action_TakeDamage` | HIGH |
| Level_Actions | 43 | `TagCharacter` | fn_8013BE98 | `Action_TagCharacter` | HIGH |
| Level_Actions | 44 | `CanHitForceObjects` | fn_8013B994 | `Action_CanHitForceObjects` | HIGH |
| Level_Actions | 45 | `SetSide` | fn_8012DC18 | `Action_SetSide` | HIGH |
| Level_Actions | 46 | `AttackOpponent` | fn_8012FB7C | `Action_AttackOpponent` | HIGH |
| Level_Actions | 47 | `SetDefensive` | fn_80137F84 | `Action_SetDefensive` | HIGH |
| Level_Actions | 48 | `SetShootOpponents` | fn_80138184 | `Action_SetShootOpponents` | HIGH |
| Level_Actions | 49 | `SetBoss` | fn_80138220 | `Action_SetBoss` | HIGH |
| Level_Actions | 50 | `SetJumping` | fn_801383C8 | `Action_SetJumping` | HIGH |
| Level_Actions | 51 | `UpdateSockPos` | fn_80138464 | `Action_UpdateSockPos` | HIGH |
| Level_Actions | 52 | `UseForce` | fn_80134574 | `Action_UseForce` | HIGH |
| Level_Actions | 53 | `DeflectPlayersPart` | fn_8013BF90 | `Action_DeflectPlayersPart` | HIGH |
| Level_Actions | 54 | `Snipe` | fn_801347F4 | `Action_Snipe` | HIGH |
| Level_Actions | 55 | `EngageOpponent` | fn_8012FD28 | `Action_EngageOpponent` | HIGH |
| Level_Actions | 56 | `EngageObject` | fn_801302FC | `Action_EngageObject` | HIGH |
| Level_Actions | 57 | `Kill` | fn_80138500 | `Action_Kill` | HIGH |
| Level_Actions | 58 | `SetScriptState` | fn_80138618 | `Action_SetScriptState` | HIGH |
| Level_Actions | 59 | `SetAIOverrideControl` | fn_80138738 | `Action_SetAIOverrideControl` | HIGH |
| Level_Actions | 60 | `SetLastSafePathPos` | fn_80138818 | `Action_SetLastSafePathPos` | HIGH |
| Level_Actions | 61 | `SetIgnoreAntinodes` | fn_80138910 | `Action_SetIgnoreAntinodes` | HIGH |
| Level_Actions | 62 | `SetDontMove` | fn_801389F0 | `Action_SetDontMove` | HIGH |
| Level_Actions | 63 | `CantDie` | fn_80138AD0 | `Action_CantDie` | HIGH |
| Level_Actions | 64 | `DontSetStoppedFlag` | fn_80138BB0 | `Action_DontSetStoppedFlag` | HIGH |
| Level_Actions | 65 | `ForceFreeze` | fn_80138C90 | `Action_ForceFreeze` | HIGH |
| Level_Actions | 66 | `SetRespawnLocator` | fn_801382BC | `Action_SetRespawnLocator` | HIGH |
| Level_Actions | 67 | `SetFlag` | fn_80138DA8 | `Action_SetFlag` | HIGH |
| Level_Actions | 68 | `SetLevelFlag` | fn_80138EAC | `Action_SetLevelFlag` | HIGH |
| Level_Actions | 69 | `SetAreaFlag` | fn_80138F84 | `Action_SetAreaFlag` | HIGH |
| Level_Actions | 70 | `SetPlayType` | fn_8013904C | `Action_SetPlayType` | HIGH |
| Level_Actions | 71 | `PlaceAheadOfPlayer` | fn_8012DF14 | `Action_PlaceAheadOfPlayer` | HIGH |
| Level_Actions | 72 | `PressSpecialButton` | fn_8013950C | `Action_PressSpecialButton` | HIGH |
| Level_Actions | 73 | `PressTagButton` | fn_80139550 | `Action_PressTagButton` | HIGH |
| Level_Actions | 74 | `PressActionButton` | fn_80139594 | `Action_PressActionButton` | HIGH |
| Level_Actions | 75 | `UseWeapon` | fn_801395E4 | `Action_UseWeapon` | HIGH |
| Level_Actions | 76 | `SetInvulnerable` | fn_801305D0 | `Action_SetInvulnerable` | HIGH |
| Level_Actions | 77 | `DontPush` | fn_80130820 | `Action_DontPush` | HIGH |
| Level_Actions | 78 | `PressJumpButton` | fn_80139620 | `Action_PressJumpButton` | HIGH |
| Level_Actions | 79 | `AddToSet` | fn_80139664 | `Action_AddToSet` | HIGH |
| Level_Actions | 80 | `GoToDoorTrigger` | fn_8012D8F8 | `Action_GoToDoorTrigger` | HIGH |
| Level_Actions | 81 | `ResetDoorTrigger` | fn_80137E10 | `Action_ResetDoorTrigger` | HIGH |
| Level_Actions | 82 | `SetNearestSpline` | fn_801397A4 | `Action_SetNearestSpline` | HIGH |
| Level_Actions | 83 | `SetRandomSpline` | fn_801398E0 | `Action_SetRandomSpline` | HIGH |
| Level_Actions | 84 | `FollowSpline` | fn_80130A0C | `Action_FollowSpline` | HIGH |
| Level_Actions | 85 | `SetControlSystem` | fn_80139710 | `Action_SetControlSystem` | HIGH |
| Level_Actions | 86 | `PlaceOnSock` | fn_80130C60 | `Action_PlaceOnSock` | HIGH |
| Level_Actions | 87 | `FollowSock` | fn_80131078 | `Action_FollowSock` | HIGH |
| Level_Actions | 88 | `SetZeroAcceleration` | fn_80139A44 | `Action_SetZeroAcceleration` | HIGH |
| Level_Actions | 89 | `BreakFormation` | fn_80139B48 | `Action_BreakFormation` | HIGH |
| Level_Actions | 90 | `FormationMove` | fn_80139B70 | `Action_FormationMove` | HIGH |
| Level_Actions | 91 | `CreateCreatures` | fn_801311F4 | `Action_CreateCreatures` | HIGH |
| Level_Actions | 92 | `CreateSockCreatures` | fn_801318D0 | `Action_CreateSockCreatures` | HIGH |
| Level_Actions | 93 | `CreateSplineCreatures` | fn_80132194 | `Action_CreateSplineCreatures` | HIGH |
| Level_Actions | 94 | `SetSpeed` | fn_80139D0C | `Action_SetSpeed` | HIGH |
| Level_Actions | 95 | `SetRunSpeed` | fn_801326F0 | `Action_SetRunSpeed` | HIGH |
| Level_Actions | 96 | `SetWalkSpeed` | fn_80139D84 | `Action_SetWalkSpeed` | HIGH |
| Level_Actions | 97 | `SetHitPoints` | fn_80139E14 | `Action_SetHitPoints` | HIGH |
| Level_Actions | 98 | `SetMessage` | fn_8013287C | `Action_SetMessage` | HIGH |
| Level_Actions | 99 | `AddPart` | fn_801329FC | `Action_AddPart` | HIGH |
| Level_Actions | 100 | `AddPartDebris` | fn_80132C40 | `Action_AddPartDebris` | HIGH |
| Level_Actions | 101 | `ActivateTurret` | fn_80139F30 | `Action_ActivateTurret` | HIGH |
| Level_Actions | 102 | `SetHoverPhase` | fn_8013A014 | `Action_SetHoverPhase` | HIGH |
| Level_Actions | 103 | `UseCurrentSpeed` | fn_8013A0F8 | `Action_UseCurrentSpeed` | HIGH |
| Level_Actions | 104 | `SetMaxMovementRange` | fn_80132E78 | `Action_SetMaxMovementRange` | HIGH |
| Level_Actions | 105 | `SetDefaultMovementRange` | fn_8013A264 | `Action_SetDefaultMovementRange` | HIGH |
| Level_Actions | 106 | `SetGravityHeight` | fn_8013A2C4 | `Action_SetGravityHeight` | HIGH |
| Level_Actions | 107 | `ApplyGravity` | fn_8013A35C | `Action_ApplyGravity` | HIGH |
| Level_Actions | 108 | `IgnoreShoveSystem` | fn_8013A3F8 | `Action_IgnoreShoveSystem` | HIGH |
| Level_Actions | 109 | `CannotBeSeen` | fn_8013A494 | `Action_CannotBeSeen` | HIGH |
| Level_Actions | 111 | `RaceOpponent` | fn_8013301C | `Action_RaceOpponent` | HIGH |
| Level_Actions | 112 | `ZamMovement` | fn_80133278 | `Action_ZamMovement` | HIGH |
| Level_Actions | 113 | `SetVisibility` | fn_8013A530 | `Action_SetVisibility` | HIGH |
| Level_Actions | 114 | `EnableSock` | fn_8013A608 | `Action_EnableSock` | HIGH |
| Level_Actions | 115 | `MakeExplosion` | fn_801334CC | `Action_MakeExplosion` | HIGH |
| Level_Actions | 116 | `JudderGameCamera` | fn_8013A6E0 | `Action_JudderGameCamera` | HIGH |
| Level_Actions | 117 | `CameraShake` | fn_8013A7E8 | `Action_CameraShake` | HIGH |
| Level_Actions | 118 | `ResetGameCamera` | fn_8013A900 | `Action_ResetGameCamera` | HIGH |
| Level_Actions | 119 | `PlayCutScene` | fn_8013A934 | `Action_PlayCutScene` | HIGH |
| Level_Actions | 120 | `SetLevelPath` | fn_8013A9BC | `Action_SetLevelPath` | HIGH |
| Level_Actions | 121 | `ImmuneToKillTerrain` | fn_80138020 | `Action_ImmuneToKillTerrain` | HIGH |
| Level_Actions | 122 | `Respawnable` | fn_801380BC | `Action_Respawnable` | HIGH |
| Level_Actions | 123 | `SetPathCnxFlag` | fn_80134A14 | `Action_SetPathCnxFlag` | HIGH |
| Level_Actions | 124 | `SetHint` | fn_8013AAB0 | `Action_SetHint` | HIGH |
| Level_Actions | 125 | `SetHintComplete` | fn_8013AB40 | `Action_SetHintComplete` | HIGH |
| Level_Actions | 126 | `CancelHint` | fn_8013ABCC | `Action_CancelHint` | HIGH |
| Level_Actions | 127 | `CycleCharacter` | fn_801336A0 | `Action_CycleCharacter` | HIGH |
| Level_Actions | 128 | `CnxController` | fn_80133AD8 | `Action_CnxController` | HIGH |
| Level_Actions | 129 | `ObjectController` | fn_80133EB0 | `Action_ObjectController` | HIGH |
| Level_Actions | 130 | `PlaySfx` | fn_80134C48 | `Action_PlaySfx` | HIGH |
| Level_Actions | 131 | `CameraCut` | fn_80134EA8 | `Action_CameraCut` | HIGH |
| Level_Actions | 132 | `SetCircleDirection` | fn_801393E0 | `Action_SetCircleDirection` | HIGH |
| Level_Actions | 133 | `DontRaycastLOS` | fn_8013C030 | `Action_DontRaycastLOS` | HIGH |
| Level_Actions | 134 | `SetForceBack` | fn_8013C0C4 | `Action_SetForceBack` | HIGH |
| Level_Actions | 135 | `SetForceLightningTarget` | fn_8013C1F4 | `Action_SetForceLightningTarget` | HIGH |
| Level_Actions | 136 | `FaceCamera` | fn_8013C2E8 | `Action_FaceCamera` | HIGH |
| Level_Actions | 137 | `FaceCharacter` | fn_8013C414 | `Action_FaceCharacter` | HIGH |
| Level_Actions | 138 | `FollowPlayer` | fn_8013C4F4 | `Action_FollowPlayer` | HIGH |
| Level_Actions | 139 | `SetFormationCommander` | fn_80139B98 | `Action_SetFormationCommander` | HIGH |
| Level_Actions | 140 | `RemoveThrownForceObjects` | fn_80139C90 | `Action_RemoveThrownForceObjects` | HIGH |
| Level_Actions | 144 | `BoulderSection` | fn_800C5D68 | `Action_BoulderSection` | HIGH |
| Level_Conditions | 0 | `Active` | fn_80135728 | `Condition_Active` | HIGH |
| Level_Conditions | 1 | `GotBlaster` | fn_80135754 | `Condition_GotBlaster` | HIGH |
| Level_Conditions | 2 | `GlynTest` | fn_80135798 | `Condition_GlynTest` | HIGH |
| Level_Conditions | 3 | `IsAlive` | fn_8013642C | `Condition_IsAlive` | HIGH |
| Level_Conditions | 4 | `IsOnScreen` | fn_801365AC | `Condition_IsOnScreen` | HIGH |
| Level_Conditions | 5 | `OnObject` | fn_801368F4 | `Condition_OnObject` | HIGH |
| Level_Conditions | 6 | `OnSameObjectAsPlayer` | fn_80136974 | `Condition_OnSameObjectAsPlayer` | HIGH |
| Level_Conditions | 7 | `PlayerOnObject` | fn_80136710 | `Condition_PlayerOnObject` | HIGH |
| Level_Conditions | 8 | `EitherPlayerOnObject` | fn_801367D8 | `Condition_EitherPlayerOnObject` | HIGH |
| Level_Conditions | 9 | `OnGround` | fn_80136634 | `Condition_OnGround` | HIGH |
| Level_Conditions | 10 | `BeenAlerted` | fn_80136678 | `Condition_BeenAlerted` | HIGH |
| Level_Conditions | 11 | `PlayerOnGround` | fn_80136604 | `Condition_PlayerOnGround` | HIGH |
| Level_Conditions | 12 | `LocatedCollectable` | fn_8012D56C | `Condition_LocatedCollectable` | HIGH |
| Level_Conditions | 13 | `CollectedCount` | fn_801369E0 | `Condition_CollectedCount` | HIGH |
| Level_Conditions | 14 | `SpawnCount` | fn_80136A40 | `Condition_SpawnCount` | HIGH |
| Level_Conditions | 15 | `BehindCamera` | fn_80136A84 | `Condition_BehindCamera` | HIGH |
| Level_Conditions | 16 | `LocatorOnScreen` | fn_80136B4C | `Condition_LocatorOnScreen` | HIGH |
| Level_Conditions | 17 | `Blocking` | fn_80136BA8 | `Condition_Blocking` | HIGH |
| Level_Conditions | 18 | `BeenHit` | fn_80136BFC | `Condition_BeenHit` | HIGH |
| Level_Conditions | 19 | `HoverPhase` | fn_80136C54 | `Condition_HoverPhase` | HIGH |
| Level_Conditions | 20 | `HitPoints` | fn_80136CEC | `Condition_HitPoints` | HIGH |
| Level_Conditions | 21 | `StuckTime` | fn_80136D58 | `Condition_StuckTime` | HIGH |
| Level_Conditions | 22 | `DoorLocked` | fn_801357CC | `Condition_DoorLocked` | HIGH |
| Level_Conditions | 23 | `DoorOpened` | fn_8013581C | `Condition_DoorOpened` | HIGH |
| Level_Conditions | 24 | `DoorOpenedByPlayer` | fn_80135848 | `Condition_DoorOpenedByPlayer` | HIGH |
| Level_Conditions | 25 | `DoorOpen` | fn_801358A8 | `Condition_DoorOpen` | HIGH |
| Level_Conditions | 26 | `ObstacleFinished` | fn_801358F8 | `Condition_ObstacleFinished` | HIGH |
| Level_Conditions | 27 | `ObstacleChainPhase` | fn_8013596C | `Condition_ObstacleChainPhase` | HIGH |
| Level_Conditions | 28 | `AnimationFinished` | fn_80137428 | `Condition_AnimationFinished` | HIGH |
| Level_Conditions | 29 | `GadgetActive` | fn_80135A08 | `Condition_GadgetActive` | HIGH |
| Level_Conditions | 30 | `TubeActive` | fn_80135A64 | `Condition_TubeActive` | HIGH |
| Level_Conditions | 31 | `PlayerUsingForce` | fn_80136240 | `Condition_PlayerUsingForce` | HIGH |
| Level_Conditions | 32 | `PlayerDeflectingPart` | fn_801362E8 | `Condition_PlayerDeflectingPart` | HIGH |
| Level_Conditions | 33 | `EitherPlayerUsingForce` | fn_8012D420 | `Condition_EitherPlayerUsingForce` | HIGH |
| Level_Conditions | 34 | `ForceBeingUsed` | fn_8013636C | `Condition_ForceBeingUsed` | HIGH |
| Level_Conditions | 35 | `UsingForce` | fn_80135AB8 | `Condition_UsingForce` | HIGH |
| Level_Conditions | 36 | `UsingForceLightning` | fn_80135B70 | `Condition_UsingForceLightning` | HIGH |
| Level_Conditions | 37 | `TurretAlive` | fn_80135BD8 | `Condition_TurretAlive` | HIGH |
| Level_Conditions | 38 | `ForceComplete` | fn_801363C0 | `Condition_ForceComplete` | HIGH |
| Level_Conditions | 39 | `IAm` | fn_80135C44 | `Condition_IAm` | HIGH |
| Level_Conditions | 40 | `IAmA` | fn_80135E8C | `Condition_IAmA` | HIGH |
| Level_Conditions | 41 | `CanFightLikeAJedi` | fn_80135ED0 | `Condition_CanFightLikeAJedi` | HIGH |
| Level_Conditions | 42 | `Side` | fn_80135FCC | `Condition_Side` | HIGH |
| Level_Conditions | 43 | `IAmAPartyCharacter` | fn_80135DDC | `Condition_IAmAPartyCharacter` | HIGH |
| Level_Conditions | 44 | `CategoryIs` | fn_80135D2C | `Condition_CategoryIs` | HIGH |
| Level_Conditions | 45 | `PlayerCategoryIs` | fn_80135D88 | `Condition_PlayerCategoryIs` | HIGH |
| Level_Conditions | 46 | `EitherPlayerIs` | fn_80136084 | `Condition_EitherPlayerIs` | HIGH |
| Level_Conditions | 47 | `CheckFlag` | fn_801360F8 | `Condition_CheckFlag` | HIGH |
| Level_Conditions | 48 | `CheckLevelFlag` | fn_80136184 | `Condition_CheckLevelFlag` | HIGH |
| Level_Conditions | 49 | `CheckAreaFlag` | fn_801361F4 | `Condition_CheckAreaFlag` | HIGH |
| Level_Conditions | 50 | `PlayerOnDoorTrigger` | fn_80136DA8 | `Condition_PlayerOnDoorTrigger` | HIGH |
| Level_Conditions | 51 | `EitherPlayerOnDoorTrigger` | fn_80136E04 | `Condition_EitherPlayerOnDoorTrigger` | HIGH |
| Level_Conditions | 52 | `IsSetAlive` | fn_80136EB8 | `Condition_IsSetAlive` | HIGH |
| Level_Conditions | 53 | `Context` | fn_80136EF0 | `Condition_Context` | HIGH |
| Level_Conditions | 54 | `Player2Active` | fn_80136F58 | `Condition_Player2Active` | HIGH |
| Level_Conditions | 55 | `AngleFromPlayerToMe` | fn_80136F7C | `Condition_AngleFromPlayerToMe` | HIGH |
| Level_Conditions | 56 | `AngleFromPlayer2ToMe` | fn_80137010 | `Condition_AngleFromPlayer2ToMe` | HIGH |
| Level_Conditions | 57 | `NumBaddies` | fn_801370AC | `Condition_NumBaddies` | HIGH |
| Level_Conditions | 58 | `NumForceObjects` | fn_801371EC | `Condition_NumForceObjects` | HIGH |
| Level_Conditions | 59 | `BeenToLevel` | fn_80137318 | `Condition_BeenToLevel` | HIGH |
| Level_Conditions | 60 | `Message` | fn_801373BC | `Condition_Message` | HIGH |
| Level_Conditions | 61 | `CutSceneStarted` | fn_80137518 | `Condition_CutSceneStarted` | HIGH |
| Level_Conditions | 62 | `CutSceneFinished` | fn_80137580 | `Condition_CutSceneFinished` | HIGH |
| Level_Conditions | 63 | `CutScenePlaying` | fn_801375D0 | `Condition_CutScenePlaying` | HIGH |
| Level_Conditions | 64 | `RigidAnimFrame` | fn_801374D8 | `Condition_RigidAnimFrame` | HIGH |
| Level_Conditions | 65 | `RaceLap` | fn_8013761C | `Condition_RaceLap` | HIGH |
| Level_Conditions | 66 | `SockDistanceToPlayer` | fn_80137650 | `Condition_SockDistanceToPlayer` | HIGH |
| Level_Conditions | 67 | `SockDistanceToOpponent` | fn_801376F4 | `Condition_SockDistanceToOpponent` | HIGH |
| Level_Conditions | 68 | `SockXDistanceToPlayer` | fn_8013779C | `Condition_SockXDistanceToPlayer` | HIGH |
| Level_Conditions | 69 | `PlayerDistanceAlongSock` | fn_8013786C | `Condition_PlayerDistanceAlongSock` | HIGH |
| Level_Conditions | 70 | `FurthestPlayerDistanceAlongSoc` | fn_8013788C | `Condition_FurthestPlayerDistanceAlongSoc` | HIGH |
| Level_Conditions | 71 | `FinishedSpline` | fn_8013791C | `Condition_FinishedSpline` | HIGH |
| Level_Conditions | 72 | `IsSplineSet` | fn_801378CC | `Condition_IsSplineSet` | HIGH |
| Level_Conditions | 73 | `CurrentHintId` | fn_80137984 | `Condition_CurrentHintId` | HIGH |
| Level_Conditions | 74 | `HintAvailable` | fn_801379F8 | `Condition_HintAvailable` | HIGH |
| Level_Conditions | 75 | `HintComplete` | fn_80137A3C | `Condition_HintComplete` | HIGH |
| Level_Conditions | 76 | `Freeplay` | fn_80137A80 | `Condition_Freeplay` | HIGH |
| Level_Conditions | 77 | `CameraYRot` | fn_80137AB4 | `Condition_CameraYRot` | HIGH |
| Level_Conditions | 78 | `UsingForceFreeze` | fn_80137AF8 | `Condition_UsingForceFreeze` | HIGH |
| Level_Conditions | 79 | `FromNode` | fn_80137BB8 | `Condition_FromNode` | HIGH |
| Level_Conditions | 80 | `ToNode` | fn_80137C5C | `Condition_ToNode` | HIGH |
| Level_Conditions | 81 | `AIOverrideControl` | fn_80136514 | `Condition_AIOverrideControl` | HIGH |
| Level_Conditions | 82 | `StuckDontJumpNow` | fn_80137CB8 | `Condition_StuckDontJumpNow` | HIGH |
| Level_Conditions | 83 | `BoltsDontGetDeflectedBack` | fn_80137CE4 | `Condition_BoltsDontGetDeflectedBack` | HIGH |
| Level_Conditions | 84 | `CheatProgress` | fn_80137D20 | `Condition_CheatProgress` | HIGH |
| Level_Conditions | 85 | `BigJumpComplete` | fn_80137D2C | `Condition_BigJumpComplete` | HIGH |
| Level_Conditions | 86 | `RespawnLocatorIs` | fn_80137DA0 | `Condition_RespawnLocatorIs` | HIGH |
| Level_Conditions | 87 | `InMiniCut` | fn_80137DD4 | `Condition_InMiniCut` | HIGH |
| Level_Conditions | 88 | `MaulShouldRunAway` | fn_8012D7E0 | `Condition_MaulShouldRunAway` | HIGH |
| Level_Conditions | 89 | `DropBackInTimer` | fn_80137E08 | `Condition_DropBackInTimer` | HIGH |
| AI_Conditions_API | 12 | `NodeRange` | fn_800840A8 | `Condition_LevelNodeRange (substr)` | MEDIUM |
| Script_Keyword_Parser2 | 0 | `STATE` | fn_8007C8A4 | `Action_SetState (substr)` | MEDIUM |
| Script_Keyword_Parser2 | 1 | `PARAM` | fn_8007CAFC | `Action_SetParam (substr)` | MEDIUM |

---
## All Dispatch Tables

### Script_Keyword_Parser @ 0x801BC040

| # | String | Func Addr | Func Name | Extra | Mac Candidates |
|---|--------|-----------|-----------|-------|----------------|
| 0 | `CONDITIONS` | 0x8007BFD8 | AIScriptParseConditions |  |  |
| 1 | `ACTIONS` | 0x8007C534 | fn_8007C534 |  |  |
| 2 | `REFERENCESCRIPT` | 0x8007CE54 | AIScriptXRefScript |  |  |
| 3 | `}` | 0x80083A4C | fn_80083A4C |  |  |
### Script_Keyword_Parser2 @ 0x801BC068

| # | String | Func Addr | Func Name | Extra | Mac Candidates |
|---|--------|-----------|-----------|-------|----------------|
| 0 | `STATE` | 0x8007C8A4 | fn_8007C8A4 |  | Action_SetState (substr) |
| 1 | `PARAM` | 0x8007CAFC | fn_8007CAFC |  | Condition_Param; Action_SetParam (substr) |
| 2 | `REFERENCESCRIPT` | 0x8007CE54 | AIScriptXRefScript |  |  |
| 3 | `DERIVEFROMSCRIPT` | 0x8007CC34 | fn_8007CC34 |  |  |
### AI_Conditions_API @ 0x801BC090

| # | String | Func Addr | Func Name | Extra | Mac Candidates |
|---|--------|-----------|-----------|-------|----------------|
| 0 | `LocatorRange` | 0x80083AC8 | fn_80083AC8 | fn_80083A94 | Condition_LocatorRange |
| 1 | `LocatorRangeXZ` | 0x80083B2C | fn_80083B2C | fn_80083A94 | Condition_LocatorRangeXZ |
| 2 | `Timer` | 0x80083A6C | fn_80083A6C |  | Condition_Timer; Action_ResetTimer (substr) |
| 3 | `Random` | 0x80083A74 | fn_80083A74 |  | Condition_Random; Action_AnimTimeRandom (substr) |
| 4 | `GotLocator` | 0x80083B90 | fn_80083B90 |  | Condition_GotLocator |
| 5 | `CurrentLocatorIs` | 0x80083C38 | fn_80083C38 | fn_80083C04 | Condition_CurrentLocatorIs |
| 6 | `InTriggerArea` | 0x80083CA0 | fn_80083CA0 | fn_80083C6C | Condition_InTriggerArea |
| 7 | `PlayerRange` | 0x80083D30 | fn_80083D30 |  | Condition_PlayerRange |
| 8 | `NearestPlayerRange` | 0x80083D90 | fn_80083D90 |  | Condition_NearestPlayerRange |
| 9 | `InLevelNode` | 0x80083EA0 | fn_80083EA0 | fn_80083E50 | Condition_InLevelNode |
| 10 | `PlayerInLevelNode` | 0x80083F4C | fn_80083F4C | fn_80083EFC | Condition_PlayerInLevelNode |
| 11 | `EitherPlayerInLevelNode` | 0x80083FB4 | fn_80083FB4 | fn_80083EFC | Condition_EitherPlayerInLevelNode |
| 12 | `NodeRange` | 0x800840A8 | fn_800840A8 | fn_80084058 | Condition_LevelNodeRange (substr) |
| 13 | `GotTriggerArea` | 0x800840FC | fn_800840FC |  | Condition_GotTriggerArea |
| 14 | `PlayerInTriggerArea` | 0x8008415C | fn_8008415C | fn_80084128 | Condition_PlayerInTriggerArea |
| 15 | `Player2InTriggerArea` | 0x80084200 | fn_80084200 | fn_80084128 | Condition_Player2InTriggerArea |
| 16 | `EitherPlayerInTriggerArea` | 0x800842A4 | fn_800842A4 | fn_80084128 | Condition_EitherPlayerInTriggerArea |
| 17 | `BaddyInTriggerArea` | 0x800843B0 | fn_800843B0 | fn_8008437C | Condition_BaddyInTriggerArea |
| 18 | `GoodyInTriggerArea` | 0x80084430 | fn_80084430 | fn_800843FC | Condition_GoodyInTriggerArea |
| 19 | `OpponentInTriggerArea` | 0x800844B0 | fn_800844B0 | fn_8008447C | Condition_OpponentInTriggerArea |
| 20 | `GotOpponent` | 0x8008454C | fn_8008454C |  | Condition_GotOpponent |
| 21 | `OpponentIsAThreat` | 0x80084578 | fn_80084578 |  | Condition_OpponentIsAThreat |
| 22 | `OpponentOnSamePath` | 0x800845B0 | fn_800845B0 |  | Condition_OpponentOnSamePath |
| 23 | `OpponentRange` | 0x80084600 | fn_80084600 |  | Condition_OpponentRange |
| 24 | `NearestOpponentRange` | 0x80084628 | fn_80084628 |  | Condition_NearestOpponentRange |
| 25 | `YawToOpponent` | 0x80084650 | fn_80084650 |  | Condition_YawToOpponent |
| 26 | `OpponentBelow` | 0x80084700 | fn_80084700 |  | Condition_OpponentBelow |
| 27 | `OriginRange` | 0x8008474C | fn_8008474C |  | Condition_OriginRange |
| 28 | `OpponentToOrigin` | 0x800847B8 | fn_800847B8 |  | Condition_OpponentToOrigin |
| 29 | `PlayerToOrigin` | 0x80084824 | fn_80084824 |  | Condition_PlayerToOrigin |
| 30 | `OpponentToLocator` | 0x800848C4 | fn_800848C4 | fn_80084890 | Condition_OpponentToLocator |
| 31 | `OpponentToLocatorXZ` | 0x80084928 | fn_80084928 | fn_80084890 | Condition_OpponentToLocatorXZ |
| 32 | `OpponentToLocatorY` | 0x8008498C | fn_8008498C | fn_80084890 | Condition_OpponentToLocatorY |
| 33 | `PlayerToLocator` | 0x80084A08 | fn_80084A08 | fn_800849D4 | Condition_PlayerToLocator |
| 34 | `NearestPlayerToLocator` | 0x80084A74 | fn_80084A74 | fn_800849D4 | Condition_NearestPlayerToLocator |
| 35 | `OnPath` | 0x80083BBC | fn_80083BBC |  | Condition_OnPath |
| 36 | `TimeOffPath` | 0x80083BE8 | fn_80083BE8 |  | Condition_TimeOffPath |
| 37 | `PathBlocked` | 0x80084B3C | fn_80084B3C |  | Condition_PathBlocked |
| 38 | `InterruptID` | 0x80084B68 | fn_80084B68 |  | Condition_InterruptID |
| 39 | `IAm` | 0x80084BEC | fn_80084BEC | fn_80084BAC | Condition_IAm |
| 40 | `StuckTime` | 0x80084C60 | fn_80084C60 |  | Condition_StuckTime |
### AI_Actions_API @ 0x801BC288

| # | String | Func Addr | Func Name | Extra | Mac Candidates |
|---|--------|-----------|-----------|-------|----------------|
| 0 | `Idle` | 0x8007F8A8 | fn_8007F8A8 |  | Action_Idle |
| 1 | `SetState` | 0x80084C88 | fn_80084C88 |  | Action_SetState |
| 2 | `ResetTimer` | 0x80084D24 | fn_80084D24 |  | Action_ResetTimer |
| 3 | `RetreatFromNearestOpponent` | 0x800809AC | fn_800809AC |  | Action_RetreatFromNearestOpponent |
| 4 | `RetreatFromOpponent` | 0x80080800 | fn_80080800 |  | Action_RetreatFromOpponent |
| 5 | `MoveAwayFromPlayer` | 0x8008063C | fn_8008063C |  | Action_MoveAwayFromPlayer |
| 6 | `CircleOpponent` | 0x800803CC | fn_800803CC |  | Action_CircleOpponent |
| 7 | `CirclePlayer` | 0x8008014C | fn_8008014C |  | Action_CirclePlayer |
| 8 | `FollowPlayer` | 0x8007FF9C | fn_8007FF9C |  | Action_FollowPlayer |
| 9 | `MoveAwayFromOpponent` | 0x8007FDBC | fn_8007FDBC |  | Action_MoveAwayFromOpponent |
| 10 | `FollowOpponent` | 0x8007FC1C | fn_8007FC1C |  | Action_FollowOpponent |
| 11 | `FacePlayer` | 0x80084D40 | fn_80084D40 |  | Action_FacePlayer |
| 12 | `FaceOpponent` | 0x80080B50 | fn_80080B50 |  | Action_FaceOpponent |
| 13 | `IgnoreWallSplines` | 0x80084EA8 | fn_80084EA8 |  | Action_IgnoreWallSplines |
| 14 | `CheckWallSplines` | 0x80084F2C | fn_80084F2C |  | Action_CheckWallSplines |
| 15 | `NoTerrain` | 0x80084FB0 | fn_80084FB0 |  | Action_NoTerrain |
| 16 | `FlatTerrain` | 0x800850D8 | fn_800850D8 |  | Action_FlatTerrain |
| 17 | `ShadowTerrain` | 0x8008516C | fn_8008516C |  | Action_ShadowTerrain |
| 18 | `DontUseShadowTerrain` | 0x80085200 | fn_80085200 |  | Action_DontUseShadowTerrain |
| 19 | `DontPush` | 0x80085294 | fn_80085294 |  | Action_DontPush |
| 20 | `CanSeeBehind` | 0x80085328 | fn_80085328 |  | Action_CanSeeBehind |
| 21 | `RequiresLOS` | 0x800853BC | fn_800853BC |  | Action_RequiresLOS |
| 22 | `SetFullPathSearch` | 0x80085440 | fn_80085440 |  | Action_SetFullPathSearch |
| 23 | `SetViewDistance` | 0x80081068 | fn_80081068 |  | Action_SetViewDistance |
| 24 | `SetMaxViewHeight` | 0x800813B0 | fn_800813B0 |  | Action_SetMaxViewHeight |
| 25 | `SetMinViewHeight` | 0x8008120C | fn_8008120C |  | Action_SetMinViewHeight |
| 26 | `SetHearDistance` | 0x80081554 | fn_80081554 |  | Action_SetHearDistance |
| 27 | `SetMoveRadius` | 0x800854D4 | fn_800854D4 |  | Action_SetMoveRadius |
| 28 | `GoToNode` | 0x800816F8 | fn_800816F8 |  | Action_GoToNode |
| 29 | `GoToNodeRandom` | 0x800819A4 | fn_800819A4 |  | Action_GoToNodeRandom |
| 30 | `GoToOrigin` | 0x80081B7C | fn_80081B7C |  | Action_GoToOrigin |
| 31 | `GoToLocator` | 0x800820DC | fn_800820DC |  | Action_GoToLocator |
| 32 | `SetLocator` | 0x80081E00 | fn_80081E00 |  | Action_SetLocator |
| 33 | `FollowPath` | 0x80085628 | fn_80085628 |  | Action_FollowPath |
| 34 | `MoveAwayFromNode` | 0x8008296C | fn_8008296C |  | Action_MoveAwayFromNode |
| 35 | `OverrideAnimation` | 0x80082C18 | fn_80082C18 |  | Action_OverrideAnimation |
| 36 | `BlockPath` | 0x80082E18 | fn_80082E18 |  | Action_BlockPath |
| 37 | `PathConnectionObstacle` | 0x800856F8 | fn_800856F8 |  | Action_PathConnectionObstacle |
| 38 | `PathConnectionMaxLength` | 0x80083190 | fn_80083190 |  | Action_PathConnectionMaxLength |
| 39 | `NoLosCheck` | 0x80085044 | fn_80085044 |  | Action_NoLosCheck |
| 40 | `ResetToOrigin` | 0x800857FC | fn_800857FC |  | Action_ResetToOrigin |
| 41 | `SetInterrupt` | 0x800833DC | fn_800833DC |  | Action_SetInterrupt |
| 42 | `ClearInterrupt` | 0x8008584C | fn_8008584C |  | Action_ClearInterrupt |
| 43 | `SetIgnoreAntinodes` | 0x800858D8 | fn_800858D8 |  | Action_SetIgnoreAntinodes |
| 44 | `NoShadows` | 0x800859BC | fn_800859BC |  | Action_NoShadows |
### Level_Conditions @ 0x801E0D14

| # | String | Func Addr | Func Name | Extra | Mac Candidates |
|---|--------|-----------|-----------|-------|----------------|
| 0 | `Active` | 0x80135728 | fn_80135728 |  | Condition_Active |
| 1 | `GotBlaster` | 0x80135754 | fn_80135754 |  | Condition_GotBlaster |
| 2 | `GlynTest` | 0x80135798 | fn_80135798 | fn_801357A0 | Condition_GlynTest |
| 3 | `IsAlive` | 0x8013642C | fn_8013642C | fn_801363EC | Condition_IsAlive |
| 4 | `IsOnScreen` | 0x801365AC | fn_801365AC | fn_8013656C | Condition_IsOnScreen |
| 5 | `OnObject` | 0x801368F4 | fn_801368F4 | fn_80136898 | Condition_OnObject |
| 6 | `OnSameObjectAsPlayer` | 0x80136974 | fn_80136974 |  | Condition_OnSameObjectAsPlayer |
| 7 | `PlayerOnObject` | 0x80136710 | fn_80136710 | fn_801366B4 | Condition_PlayerOnObject |
| 8 | `EitherPlayerOnObject` | 0x801367D8 | fn_801367D8 | fn_8013677C | Condition_EitherPlayerOnObject |
| 9 | `OnGround` | 0x80136634 | fn_80136634 |  | Condition_OnGround |
| 10 | `BeenAlerted` | 0x80136678 | fn_80136678 |  | Condition_BeenAlerted |
| 11 | `PlayerOnGround` | 0x80136604 | fn_80136604 |  | Condition_PlayerOnGround |
| 12 | `LocatedCollectable` | 0x8012D56C | fn_8012D56C |  | Condition_LocatedCollectable |
| 13 | `CollectedCount` | 0x801369E0 | fn_801369E0 |  | Condition_CollectedCount |
| 14 | `SpawnCount` | 0x80136A40 | fn_80136A40 |  | Condition_SpawnCount |
| 15 | `BehindCamera` | 0x80136A84 | fn_80136A84 |  | Condition_BehindCamera |
| 16 | `LocatorOnScreen` | 0x80136B4C | fn_80136B4C | fn_80136B2C | Condition_LocatorOnScreen |
| 17 | `Blocking` | 0x80136BA8 | fn_80136BA8 |  | Condition_Blocking |
| 18 | `BeenHit` | 0x80136BFC | fn_80136BFC |  | Condition_BeenHit |
| 19 | `HoverPhase` | 0x80136C54 | fn_80136C54 |  | Action_SetHoverPhase (substr); Condition_HoverPhas |
| 20 | `HitPoints` | 0x80136CEC | fn_80136CEC | fn_80136CAC | Condition_HitPoints; Action_SetHitPoints (substr) |
| 21 | `StuckTime` | 0x80136D58 | fn_80136D58 |  | Condition_StuckTime |
| 22 | `DoorLocked` | 0x801357CC | fn_801357CC | fn_801357A8 | Condition_DoorLocked |
| 23 | `DoorOpened` | 0x8013581C | fn_8013581C | fn_801357F8 | Condition_DoorOpened |
| 24 | `DoorOpenedByPlayer` | 0x80135848 | fn_80135848 | fn_801357F8 | Condition_DoorOpenedByPlayer |
| 25 | `DoorOpen` | 0x801358A8 | fn_801358A8 | fn_80135884 | Condition_DoorOpen |
| 26 | `ObstacleFinished` | 0x801358F8 | fn_801358F8 | fn_801358D4 | Condition_ObstacleFinished |
| 27 | `ObstacleChainPhase` | 0x8013596C | fn_8013596C | fn_80135948 | Condition_ObstacleChainPhase; Action_SetObstacleCh |
| 28 | `AnimationFinished` | 0x80137428 | fn_80137428 | fn_801373D8 | Condition_AnimationFinished |
| 29 | `GadgetActive` | 0x80135A08 | fn_80135A08 | fn_801359E4 | Condition_GadgetActive |
| 30 | `TubeActive` | 0x80135A64 | fn_80135A64 | fn_80135A40 | Condition_TubeActive |
| 31 | `PlayerUsingForce` | 0x80136240 | fn_80136240 | fn_80136218 | Condition_PlayerUsingForce |
| 32 | `PlayerDeflectingPart` | 0x801362E8 | fn_801362E8 |  | Condition_PlayerDeflectingPart |
| 33 | `EitherPlayerUsingForce` | 0x8012D420 | fn_8012D420 | fn_8013631C | Condition_EitherPlayerUsingForce |
| 34 | `ForceBeingUsed` | 0x8013636C | fn_8013636C | fn_80136344 | Condition_ForceBeingUsed |
| 35 | `UsingForce` | 0x80135AB8 | fn_80135AB8 | fn_80135A90 | Condition_UsingForce |
| 36 | `UsingForceLightning` | 0x80135B70 | fn_80135B70 |  | Condition_UsingForceLightning |
| 37 | `TurretAlive` | 0x80135BD8 | fn_80135BD8 | fn_80135BB0 | Condition_TurretAlive |
| 38 | `ForceComplete` | 0x801363C0 | fn_801363C0 | fn_80136398 | Condition_ForceComplete |
| 39 | `IAm` | 0x80135C44 | fn_80135C44 | fn_80135C04 | Condition_IAm |
| 40 | `IAmA` | 0x80135E8C | fn_80135E8C | fn_80135E18 | Condition_IAmA |
| 41 | `CanFightLikeAJedi` | 0x80135ED0 | fn_80135ED0 |  | Condition_CanFightLikeAJedi |
| 42 | `Side` | 0x80135FCC | fn_80135FCC | fn_80135F2C | Action_SetSide (substr); Condition_Side |
| 43 | `IAmAPartyCharacter` | 0x80135DDC | fn_80135DDC |  | Condition_IAmAPartyCharacter |
| 44 | `CategoryIs` | 0x80135D2C | fn_80135D2C | fn_80135CBC | Condition_CategoryIs |
| 45 | `PlayerCategoryIs` | 0x80135D88 | fn_80135D88 | fn_80135CBC | Condition_PlayerCategoryIs |
| 46 | `EitherPlayerIs` | 0x80136084 | fn_80136084 | fn_80136044 | Condition_EitherPlayerIs |
| 47 | `CheckFlag` | 0x801360F8 | fn_801360F8 | fn_801360B4 | Condition_CheckFlag |
| 48 | `CheckLevelFlag` | 0x80136184 | fn_80136184 | fn_80136140 | Condition_CheckLevelFlag |
| 49 | `CheckAreaFlag` | 0x801361F4 | fn_801361F4 | fn_801361B0 | Condition_CheckAreaFlag |
| 50 | `PlayerOnDoorTrigger` | 0x80136DA8 | fn_80136DA8 | fn_80136D84 | Condition_PlayerOnDoorTrigger |
| 51 | `EitherPlayerOnDoorTrigger` | 0x80136E04 | fn_80136E04 | fn_80136D84 | Condition_EitherPlayerOnDoorTrigger |
| 52 | `IsSetAlive` | 0x80136EB8 | fn_80136EB8 | fn_80136E74 | Condition_IsSetAlive |
| 53 | `Context` | 0x80136EF0 | fn_80136EF0 |  | Action_ResetContext (substr); Condition_Context |
| 54 | `Player2Active` | 0x80136F58 | fn_80136F58 |  | Condition_Player2Active |
| 55 | `AngleFromPlayerToMe` | 0x80136F7C | fn_80136F7C |  | Condition_AngleFromPlayerToMe |
| 56 | `AngleFromPlayer2ToMe` | 0x80137010 | fn_80137010 |  | Condition_AngleFromPlayer2ToMe |
| 57 | `NumBaddies` | 0x801370AC | fn_801370AC | fn_801370A4 | Condition_NumBaddies |
| 58 | `NumForceObjects` | 0x801371EC | fn_801371EC | fn_80137178 | Condition_NumForceObjects |
| 59 | `BeenToLevel` | 0x80137318 | fn_80137318 | fn_80137270 | Condition_BeenToLevel |
| 60 | `Message` | 0x801373BC | fn_801373BC | fn_80137370 | Action_SetMessage (substr); Condition_Message |
| 61 | `CutSceneStarted` | 0x80137518 | fn_80137518 | fn_801374F4 | Condition_CutSceneStarted |
| 62 | `CutSceneFinished` | 0x80137580 | fn_80137580 | fn_80137544 | Condition_CutSceneFinished |
| 63 | `CutScenePlaying` | 0x801375D0 | fn_801375D0 | fn_801375AC | Condition_CutScenePlaying |
| 64 | `RigidAnimFrame` | 0x801374D8 | fn_801374D8 | fn_80137488 | Condition_RigidAnimFrame |
| 65 | `RaceLap` | 0x8013761C | fn_8013761C |  | Condition_RaceLap |
| 66 | `SockDistanceToPlayer` | 0x80137650 | fn_80137650 |  | Condition_SockDistanceToPlayer |
| 67 | `SockDistanceToOpponent` | 0x801376F4 | fn_801376F4 |  | Condition_SockDistanceToOpponent |
| 68 | `SockXDistanceToPlayer` | 0x8013779C | fn_8013779C |  | Condition_SockXDistanceToPlayer |
| 69 | `PlayerDistanceAlongSock` | 0x8013786C | fn_8013786C |  | Condition_PlayerDistanceAlongSock |
| 70 | `FurthestPlayerDistanceAlongSock` | 0x8013788C | fn_8013788C |  | Condition_FurthestPlayerDistanceAlongSock |
| 71 | `FinishedSpline` | 0x8013791C | fn_8013791C |  | Condition_FinishedSpline |
| 72 | `IsSplineSet` | 0x801378CC | fn_801378CC |  | Condition_IsSplineSet |
| 73 | `CurrentHintId` | 0x80137984 | fn_80137984 |  | Condition_CurrentHintId |
| 74 | `HintAvailable` | 0x801379F8 | fn_801379F8 | fn_801379C8 | Condition_HintAvailable |
| 75 | `HintComplete` | 0x80137A3C | fn_80137A3C | fn_801379C8 | Action_SetHintComplete (substr); Condition_HintCom |
| 76 | `Freeplay` | 0x80137A80 | fn_80137A80 |  | Condition_Freeplay |
| 77 | `CameraYRot` | 0x80137AB4 | fn_80137AB4 |  | Condition_CameraYRot |
| 78 | `UsingForceFreeze` | 0x80137AF8 | fn_80137AF8 |  | Condition_UsingForceFreeze |
| 79 | `FromNode` | 0x80137BB8 | fn_80137BB8 | fn_80137B68 | Condition_FromNode; Action_MoveAwayFromNode (subst |
| 80 | `ToNode` | 0x80137C5C | fn_80137C5C | fn_80137C0C | Condition_ToNode; Action_GoToNode (substr) |
| 81 | `AIOverrideControl` | 0x80136514 | fn_80136514 | fn_801364D4 | Condition_AIOverrideControl; Action_SetAIOverrideC |
| 82 | `StuckDontJumpNow` | 0x80137CB8 | fn_80137CB8 |  | Condition_StuckDontJumpNow |
| 83 | `BoltsDontGetDeflectedBack` | 0x80137CE4 | fn_80137CE4 |  | Condition_BoltsDontGetDeflectedBack; Action_SetBol |
| 84 | `CheatProgress` | 0x80137D20 | fn_80137D20 |  | Condition_CheatProgress |
| 85 | `BigJumpComplete` | 0x80137D2C | fn_80137D2C |  | Condition_BigJumpComplete |
| 86 | `RespawnLocatorIs` | 0x80137DA0 | fn_80137DA0 | fn_80137D6C | Condition_RespawnLocatorIs |
| 87 | `InMiniCut` | 0x80137DD4 | fn_80137DD4 |  | Condition_InMiniCut |
| 88 | `MaulShouldRunAway` | 0x8012D7E0 | fn_8012D7E0 |  | Condition_MaulShouldRunAway |
| 89 | `DropBackInTimer` | 0x80137E08 | fn_80137E08 |  | Condition_DropBackInTimer |
### Level_Actions @ 0x801E1158

| # | String | Func Addr | Func Name | Extra | Mac Candidates |
|---|--------|-----------|-----------|-------|----------------|
| 0 | `Activate` | 0x80137EAC | fn_80137EAC |  | Action_Activate |
| 1 | `DeActivate` | 0x80137F28 | fn_80137F28 |  | Action_DeActivate |
| 2 | `GoToLevelPath` | 0x8013417C | fn_8013417C |  | Action_GoToLevelPath |
| 3 | `SetPath` | 0x8013ABF0 | fn_8013ABF0 |  | Action_SetPath |
| 4 | `GoToOriginalPath` | 0x801342CC | fn_801342CC |  | Action_GoToOriginalPath |
| 5 | `GoToCollectable` | 0x80139154 | fn_80139154 |  | Action_GoToCollectable |
| 6 | `DumpCollectables` | 0x8013923C | fn_8013923C |  | Action_DumpCollectables |
| 7 | `SnapToLocator` | 0x8012E5DC | fn_8012E5DC |  | Action_SnapToLocator |
| 8 | `SetLocator` | 0x8012E2F4 | fn_8012E2F4 |  | Action_SetLocator |
| 9 | `SetCounterLocator` | 0x80139278 | fn_80139278 |  | Action_SetCounterLocator |
| 10 | `SetTableLocator` | 0x80139324 | fn_80139324 |  | Action_SetTableLocator |
| 11 | `CallDexter` | 0x80139388 | fn_80139388 |  | Action_CallDexter |
| 12 | `SnapToOrigin` | 0x8012F3AC | fn_8012F3AC |  | Action_SnapToOrigin |
| 13 | `BigJumpToLocator` | 0x8012E9AC | fn_8012E9AC |  | Action_BigJumpToLocator |
| 14 | `BigJump` | 0x8012EC80 | fn_8012EC80 |  | Action_BigJump; Condition_BigJumpComplete (substr) |
| 15 | `SetDoomedEscapeLocator` | 0x8012EE28 | fn_8012EE28 |  | Action_SetDoomedEscapeLocator |
| 16 | `SnapToPosition` | 0x8012F090 | fn_8012F090 |  | Action_SnapToPosition |
| 17 | `SnapToSockPosition` | 0x8012F5B8 | fn_8012F5B8 |  | Action_SnapToSockPosition |
| 18 | `SetAnimation` | 0x8013ACA4 | fn_8013ACA4 |  | Action_SetAnimation |
| 19 | `AnimTimeRandom` | 0x8013AD20 | fn_8013AD20 |  | Action_AnimTimeRandom |
| 20 | `LockDoor` | 0x8013AD6C | fn_8013AD6C |  | Action_LockDoor |
| 21 | `UnLockDoor` | 0x8013ADA8 | fn_8013ADA8 |  | Action_UnLockDoor |
| 22 | `CloseDoor` | 0x8013ADE4 | fn_8013ADE4 |  | Action_CloseDoor |
| 23 | `OpenDoor` | 0x8013AE20 | fn_8013AE20 |  | Action_OpenDoor |
| 24 | `DoorSetStayOpen` | 0x8013AEC8 | fn_8013AEC8 |  | Action_DoorSetStayOpen |
| 25 | `SetDoorFinished` | 0x8013AF84 | fn_8013AF84 |  | Action_SetDoorFinished |
| 26 | `ActivateGadget` | 0x8013B048 | fn_8013B048 |  | Action_ActivateGadget |
| 27 | `SetObstacleChainPhase` | 0x8013B0BC | fn_8013B0BC |  | Action_SetObstacleChainPhase |
| 28 | `EndLoopingAnimation` | 0x8013B18C | fn_8013B18C |  | Action_EndLoopingAnimation |
| 29 | `CanOpenDoors` | 0x8013B1FC | fn_8013B1FC |  | Action_CanOpenDoors |
| 30 | `CanShootOffScreen` | 0x8013B2C0 | fn_8013B2C0 |  | Action_CanShootOffScreen |
| 31 | `CanCollectCoins` | 0x8013B384 | fn_8013B384 |  | Action_CanCollectCoins |
| 32 | `KeepWeaponOut` | 0x8013B448 | fn_8013B448 |  | Action_KeepWeaponOut |
| 33 | `SnapWeaponOut` | 0x8013B520 | fn_8013B520 |  | Action_SnapWeaponOut |
| 34 | `ResetContext` | 0x8013B680 | fn_8013B680 |  | Action_ResetContext |
| 35 | `PrefersPlayers` | 0x8013B734 | fn_8013B734 |  | Action_PrefersPlayers |
| 36 | `SetBoltsDontGetDeflectedBack` | 0x8013B7CC | fn_8013B7CC |  | Action_SetBoltsDontGetDeflectedBack |
| 37 | `AttackButtonMash` | 0x8013B864 | fn_8013B864 |  | Action_AttackButtonMash |
| 38 | `CanShootObstructions` | 0x8013B8FC | fn_8013B8FC |  | Action_CanShootObstructions |
| 39 | `SetTaggable` | 0x8013BA2C | fn_8013BA2C |  | Action_SetTaggable |
| 40 | `CannotDropIn` | 0x8013BB74 | fn_8013BB74 |  | Action_CannotDropIn |
| 41 | `CanAttack` | 0x8013BC84 | fn_8013BC84 |  | Action_CanAttack |
| 42 | `TakeDamage` | 0x8013BD64 | fn_8013BD64 |  | Action_TakeDamage |
| 43 | `TagCharacter` | 0x8013BE98 | fn_8013BE98 |  | Action_TagCharacter |
| 44 | `CanHitForceObjects` | 0x8013B994 | fn_8013B994 |  | Action_CanHitForceObjects |
| 45 | `SetSide` | 0x8012DC18 | fn_8012DC18 |  | Action_SetSide |
| 46 | `AttackOpponent` | 0x8012FB7C | fn_8012FB7C |  | Action_AttackOpponent |
| 47 | `SetDefensive` | 0x80137F84 | fn_80137F84 |  | Action_SetDefensive |
| 48 | `SetShootOpponents` | 0x80138184 | fn_80138184 |  | Action_SetShootOpponents |
| 49 | `SetBoss` | 0x80138220 | fn_80138220 |  | Action_SetBoss |
| 50 | `SetJumping` | 0x801383C8 | fn_801383C8 |  | Action_SetJumping |
| 51 | `UpdateSockPos` | 0x80138464 | fn_80138464 |  | Action_UpdateSockPos |
| 52 | `UseForce` | 0x80134574 | fn_80134574 |  | Action_UseForce |
| 53 | `DeflectPlayersPart` | 0x8013BF90 | fn_8013BF90 |  | Action_DeflectPlayersPart |
| 54 | `Snipe` | 0x801347F4 | fn_801347F4 |  | Action_Snipe |
| 55 | `EngageOpponent` | 0x8012FD28 | fn_8012FD28 |  | Action_EngageOpponent |
| 56 | `EngageObject` | 0x801302FC | fn_801302FC |  | Action_EngageObject |
| 57 | `Kill` | 0x80138500 | fn_80138500 |  | Action_Kill |
| 58 | `SetScriptState` | 0x80138618 | fn_80138618 |  | Action_SetScriptState |
| 59 | `SetAIOverrideControl` | 0x80138738 | fn_80138738 |  | Action_SetAIOverrideControl |
| 60 | `SetLastSafePathPos` | 0x80138818 | fn_80138818 |  | Action_SetLastSafePathPos |
| 61 | `SetIgnoreAntinodes` | 0x80138910 | fn_80138910 |  | Action_SetIgnoreAntinodes |
| 62 | `SetDontMove` | 0x801389F0 | fn_801389F0 |  | Action_SetDontMove |
| 63 | `CantDie` | 0x80138AD0 | fn_80138AD0 |  | Action_CantDie |
| 64 | `DontSetStoppedFlag` | 0x80138BB0 | fn_80138BB0 |  | Action_DontSetStoppedFlag |
| 65 | `ForceFreeze` | 0x80138C90 | fn_80138C90 |  | Condition_UsingForceFreeze (substr); Action_ForceF |
| 66 | `SetRespawnLocator` | 0x801382BC | fn_801382BC |  | Action_SetRespawnLocator |
| 67 | `SetFlag` | 0x80138DA8 | fn_80138DA8 |  | Action_SetFlag |
| 68 | `SetLevelFlag` | 0x80138EAC | fn_80138EAC |  | Action_SetLevelFlag |
| 69 | `SetAreaFlag` | 0x80138F84 | fn_80138F84 |  | Action_SetAreaFlag |
| 70 | `SetPlayType` | 0x8013904C | fn_8013904C |  | Action_SetPlayType |
| 71 | `PlaceAheadOfPlayer` | 0x8012DF14 | fn_8012DF14 |  | Action_PlaceAheadOfPlayer |
| 72 | `PressSpecialButton` | 0x8013950C | fn_8013950C |  | Action_PressSpecialButton |
| 73 | `PressTagButton` | 0x80139550 | fn_80139550 |  | Action_PressTagButton |
| 74 | `PressActionButton` | 0x80139594 | fn_80139594 |  | Action_PressActionButton |
| 75 | `UseWeapon` | 0x801395E4 | fn_801395E4 |  | Action_UseWeapon |
| 76 | `SetInvulnerable` | 0x801305D0 | fn_801305D0 |  | Action_SetInvulnerable |
| 77 | `DontPush` | 0x80130820 | fn_80130820 |  | Action_DontPush |
| 78 | `PressJumpButton` | 0x80139620 | fn_80139620 |  | Action_PressJumpButton |
| 79 | `AddToSet` | 0x80139664 | fn_80139664 |  | Action_AddToSet |
| 80 | `GoToDoorTrigger` | 0x8012D8F8 | fn_8012D8F8 |  | Action_GoToDoorTrigger |
| 81 | `ResetDoorTrigger` | 0x80137E10 | fn_80137E10 |  | Action_ResetDoorTrigger |
| 82 | `SetNearestSpline` | 0x801397A4 | fn_801397A4 |  | Action_SetNearestSpline |
| 83 | `SetRandomSpline` | 0x801398E0 | fn_801398E0 |  | Action_SetRandomSpline |
| 84 | `FollowSpline` | 0x80130A0C | fn_80130A0C |  | Action_FollowSpline |
| 85 | `SetControlSystem` | 0x80139710 | fn_80139710 |  | Action_SetControlSystem |
| 86 | `PlaceOnSock` | 0x80130C60 | fn_80130C60 |  | Action_PlaceOnSock |
| 87 | `FollowSock` | 0x80131078 | fn_80131078 |  | Action_FollowSock |
| 88 | `SetZeroAcceleration` | 0x80139A44 | fn_80139A44 |  | Action_SetZeroAcceleration |
| 89 | `BreakFormation` | 0x80139B48 | fn_80139B48 |  | Action_BreakFormation |
| 90 | `FormationMove` | 0x80139B70 | fn_80139B70 |  | Action_FormationMove |
| 91 | `CreateCreatures` | 0x801311F4 | fn_801311F4 |  | Action_CreateCreatures |
| 92 | `CreateSockCreatures` | 0x801318D0 | fn_801318D0 |  | Action_CreateSockCreatures |
| 93 | `CreateSplineCreatures` | 0x80132194 | fn_80132194 |  | Action_CreateSplineCreatures |
| 94 | `SetSpeed` | 0x80139D0C | fn_80139D0C |  | Action_SetSpeed |
| 95 | `SetRunSpeed` | 0x801326F0 | fn_801326F0 |  | Action_SetRunSpeed |
| 96 | `SetWalkSpeed` | 0x80139D84 | fn_80139D84 |  | Action_SetWalkSpeed |
| 97 | `SetHitPoints` | 0x80139E14 | fn_80139E14 |  | Action_SetHitPoints |
| 98 | `SetMessage` | 0x8013287C | fn_8013287C |  | Action_SetMessage |
| 99 | `AddPart` | 0x801329FC | fn_801329FC |  | Action_AddPart |
| 100 | `AddPartDebris` | 0x80132C40 | fn_80132C40 |  | Action_AddPartDebris |
| 101 | `ActivateTurret` | 0x80139F30 | fn_80139F30 |  | Action_ActivateTurret |
| 102 | `SetHoverPhase` | 0x8013A014 | fn_8013A014 |  | Action_SetHoverPhase |
| 103 | `UseCurrentSpeed` | 0x8013A0F8 | fn_8013A0F8 |  | Action_UseCurrentSpeed |
| 104 | `SetMaxMovementRange` | 0x80132E78 | fn_80132E78 |  | Action_SetMaxMovementRange |
| 105 | `SetDefaultMovementRange` | 0x8013A264 | fn_8013A264 |  | Action_SetDefaultMovementRange |
| 106 | `SetGravityHeight` | 0x8013A2C4 | fn_8013A2C4 |  | Action_SetGravityHeight |
| 107 | `ApplyGravity` | 0x8013A35C | fn_8013A35C |  | Action_ApplyGravity |
| 108 | `IgnoreShoveSystem` | 0x8013A3F8 | fn_8013A3F8 |  | Action_IgnoreShoveSystem |
| 109 | `CannotBeSeen` | 0x8013A494 | fn_8013A494 |  | Action_CannotBeSeen |
| 110 | `Sebulba` | 0x8014F86C | PodraceUpdateMine |  | Action_Sebulba |
| 111 | `RaceOpponent` | 0x8013301C | fn_8013301C |  | Action_RaceOpponent |
| 112 | `ZamMovement` | 0x80133278 | fn_80133278 |  | Action_ZamMovement |
| 113 | `SetVisibility` | 0x8013A530 | fn_8013A530 |  | Action_SetVisibility |
| 114 | `EnableSock` | 0x8013A608 | fn_8013A608 |  | Action_EnableSock |
| 115 | `MakeExplosion` | 0x801334CC | fn_801334CC |  | Action_MakeExplosion |
| 116 | `JudderGameCamera` | 0x8013A6E0 | fn_8013A6E0 |  | Action_JudderGameCamera |
| 117 | `CameraShake` | 0x8013A7E8 | fn_8013A7E8 |  | Action_CameraShake |
| 118 | `ResetGameCamera` | 0x8013A900 | fn_8013A900 |  | Action_ResetGameCamera |
| 119 | `PlayCutScene` | 0x8013A934 | fn_8013A934 |  | Action_PlayCutScene |
| 120 | `SetLevelPath` | 0x8013A9BC | fn_8013A9BC |  | Action_SetLevelPath |
| 121 | `ImmuneToKillTerrain` | 0x80138020 | fn_80138020 |  | Action_ImmuneToKillTerrain |
| 122 | `Respawnable` | 0x801380BC | fn_801380BC |  | Action_Respawnable |
| 123 | `SetPathCnxFlag` | 0x80134A14 | fn_80134A14 |  | Action_SetPathCnxFlag |
| 124 | `SetHint` | 0x8013AAB0 | fn_8013AAB0 |  | Action_SetHint |
| 125 | `SetHintComplete` | 0x8013AB40 | fn_8013AB40 |  | Action_SetHintComplete |
| 126 | `CancelHint` | 0x8013ABCC | fn_8013ABCC |  | Action_CancelHint |
| 127 | `CycleCharacter` | 0x801336A0 | fn_801336A0 |  | Action_CycleCharacter |
| 128 | `CnxController` | 0x80133AD8 | fn_80133AD8 |  | Action_CnxController |
| 129 | `ObjectController` | 0x80133EB0 | fn_80133EB0 |  | Action_ObjectController |
| 130 | `PlaySfx` | 0x80134C48 | fn_80134C48 |  | Action_PlaySfx |
| 131 | `CameraCut` | 0x80134EA8 | fn_80134EA8 |  | Action_CameraCut |
| 132 | `SetCircleDirection` | 0x801393E0 | fn_801393E0 |  | Action_SetCircleDirection |
| 133 | `DontRaycastLOS` | 0x8013C030 | fn_8013C030 |  | Action_DontRaycastLOS |
| 134 | `SetForceBack` | 0x8013C0C4 | fn_8013C0C4 |  | Action_SetForceBack |
| 135 | `SetForceLightningTarget` | 0x8013C1F4 | fn_8013C1F4 |  | Action_SetForceLightningTarget |
| 136 | `FaceCamera` | 0x8013C2E8 | fn_8013C2E8 |  | Action_FaceCamera |
| 137 | `FaceCharacter` | 0x8013C414 | fn_8013C414 |  | Action_FaceCharacter |
| 138 | `FollowPlayer` | 0x8013C4F4 | fn_8013C4F4 |  | Action_FollowPlayer |
| 139 | `SetFormationCommander` | 0x80139B98 | fn_80139B98 |  | Action_SetFormationCommander |
| 140 | `RemoveThrownForceObjects` | 0x80139C90 | fn_80139C90 |  | Action_RemoveThrownForceObjects |
| 141 | `SetLapTime` | 0x8015022C | PodraceLoadLapSettings |  | Action_SetLapTime |
| 142 | `CreatePod` | 0x8014F3EC | PodraceLoadSplineSettings |  | Action_CreatePod |
| 143 | `MushroomCollapse` | 0x8014EDCC | PodraceLoadTimeTrialSettings |  | Action_MushroomCollapse |
| 144 | `BoulderSection` | 0x800C5D68 | fn_800C5D68 |  | Action_BoulderSection |

---
## Stats

| Metric | Value |
|--------|-------|
| Dispatch tables | 6 |
| Total entries | 329 |
| Rename candidates | 319 |
| Mac functions indexed | 9502 |
