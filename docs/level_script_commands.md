# Level Script Commands

## Overview
Two dispatch tables control level (gameplay) script behavior:
- `Level_Actions` (145 entries) — at `0x801E1158`
- `Level_Conditions` (90 entries) — at `0x801E0D14`

These are the high-level script system that controls character behaviors, cutscenes, doors, gadget interactions, etc.

## Level Actions (`Action_*`)

| Idx | Name | Address | Description |
|-----|------|---------|-------------|
| 0 | `Action_Activate` | `0x80137EAC` | Activate the character/set |
| 1 | `Action_DeActivate` | `0x80137F28` | Deactivate the character/set |
| 2 | `Action_GoToLevelPath` | `0x8013417C` | Move to level path |
| 3 | `Action_SetPath` | `0x8013ABF0` | Set current path |
| 4 | `Action_GoToOriginalPath` | `0x801342CC` | Return to original path |
| 5 | `Action_GoToCollectable` | `0x80139154` | Move to collectable item |
| 6 | `Action_DumpCollectables` | `0x8013923C` | Drop carried collectables |
| 7 | `Action_SnapToLocator` | `0x8012E5DC` | Snap position to locator |
| 8 | `Action_SetLocator` | `0x8012E2F4` | Set target locator |
| 9 | `Action_SetCounterLocator` | `0x80139278` | Set counter locator |
| 10 | `Action_SetTableLocator` | `0x80139324` | Set table locator |
| 11 | `Action_CallDexter` | `0x80139388` | Call Dexter character |
| 12 | `Action_SnapToOrigin` | `0x8012F3AC` | Snap to spawn origin |
| 13 | `Action_BigJumpToLocator` | `0x8012E9AC` | Big jump to locator |
| 14 | `Action_BigJump` | `0x8012EC80` | Big jump in place |
| 15 | `Action_SetDoomedEscapeLocator` | `0x8012EE28` | Set doomed escape point |
| 16 | `Action_SnapToPosition` | `0x8012F090` | Snap to specific position |
| 17 | `Action_SnapToSockPosition` | `0x8012F5B8` | Snap to sock position |
| 18 | `Action_SetAnimation` | `0x8013ACA4` | Set character animation |
| 19 | `Action_AnimTimeRandom` | `0x8013AD20` | Randomize animation time |
| 20 | `Action_LockDoor` | `0x8013AD6C` | Lock a door |
| 21 | `Action_UnLockDoor` | `0x8013ADA8` | Unlock a door |
| 22 | `Action_CloseDoor` | `0x8013ADE4` | Close a door |
| 23 | `Action_OpenDoor` | `0x8013AE20` | Open a door |
| 24 | `Action_DoorSetStayOpen` | `0x8013AEC8` | Set door to stay open |
| 25 | `Action_SetDoorFinished` | `0x8013AF84` | Mark door animation done |
| 26 | `Action_ActivateGadget` | `0x8013B048` | Activate a gadget |
| 27 | `Action_SetObstacleChainPhase` | `0x8013B0BC` | Set obstacle chain phase |
| 28 | `Action_EndLoopingAnimation` | `0x8013B18C` | End looping animation |
| 29 | `Action_CanOpenDoors` | `0x8013B1FC` | Enable door opening ability |
| 30 | `Action_CanShootOffScreen` | `0x8013B2C0` | Enable off-screen shooting |
| 31 | `Action_CanCollectCoins` | `0x8013B384` | Enable coin collection |
| 32 | `Action_KeepWeaponOut` | `0x8013B448` | Keep weapon drawn |
| 33 | `Action_SnapWeaponOut` | `0x8013B520` | Snap weapon to drawn state |
| 34 | `Action_ResetContext` | `0x8013B680` | Reset AI context |
| 35 | `Action_PrefersPlayers` | `0x8013B734` | Prefer player as target |
| 36 | `Action_SetBoltsDontGetDeflectedBack` | `0x8013B7CC` | Bolts don't deflect |
| 37 | `Action_AttackButtonMash` | `0x8013B864` | Mash attack button |
| 38 | `Action_CanShootObstructions` | `0x8013B8FC` | Shoot through obstructions |
| 39 | `Action_SetTaggable` | `0x8013BA2C` | Make character taggable |
| 40 | `Action_CannotDropIn` | `0x8013BB74` | Cannot drop into level |
| 41 | `Action_CanAttack` | `0x8013BC84` | Enable attacking |
| 42 | `Action_TakeDamage` | `0x8013BD64` | Take damage from hit |
| 43 | `Action_TagCharacter` | `0x8013BE98` | Tag a character (swap) |
| 44 | `Action_CanHitForceObjects` | `0x8013B994` | Hit force objects |
| 45 | `Action_SetSide` | `0x8012DC18` | Set team side |
| 46 | `Action_AttackOpponent` | `0x8012FB7C` | Attack opponent |
| 47 | `Action_SetDefensive` | `0x80137F84` | Set defensive mode |
| 48 | `Action_SetShootOpponents` | `0x80138184` | Shoot at opponents |
| 49 | `Action_SetBoss` | `0x80138220` | Designate as boss NPC |
| 50 | `Action_SetJumping` | `0x801383C8` | Enable jumping |
| 51 | `Action_UpdateSockPos` | `0x80138464` | Update sock position |
| 52 | `Action_UseForce` | `0x80134574` | Use Force power |
| 53 | `Action_DeflectPlayersPart` | `0x8013BF90` | Deflect player projectile |
| 54 | `Action_Snipe` | `0x801347F4` | Snipe at player |
| 55 | `Action_EngageOpponent` | `0x8012FD28` | Engage opponent in combat |
| 56 | `Action_EngageObject` | `0x801302FC` | Interact with object |
| 57 | `Action_Kill` | `0x80138500` | Kill this character |
| 58 | `Action_SetScriptState` | `0x80138618` | Set script state |
| 59 | `Action_SetAIOverrideControl` | `0x80138738` | Override AI control |
| 60 | `Action_SetLastSafePathPos` | `0x80138818` | Save safe path position |
| 61 | `Action_SetIgnoreAntinodes` | `0x80138910` | Ignore anti-nodes |
| 62 | `Action_SetDontMove` | `0x801389F0` | Disable movement |
| 63 | `Action_CantDie` | `0x80138AD0` | Make immortal |
| 64 | `Action_DontSetStoppedFlag` | `0x80138BB0` | Suppress stopped flag |
| 65 | `Action_ForceFreeze` | `0x80138C90` | Freeze with Force |
| 66 | `Action_SetRespawnLocator` | `0x801382BC` | Set respawn point |
| 67 | `Action_SetFlag` | `0x80138DA8` | Set a flag |
| 68 | `Action_SetLevelFlag` | `0x80138EAC` | Set level-wide flag |
| 69 | `Action_SetAreaFlag` | `0x80138F84` | Set area flag |
| 70 | `Action_SetPlayType` | `0x8013904C` | Set play type |
| 71 | `Action_PlaceAheadOfPlayer` | `0x8012DF14` | Place ahead of player |
| 72 | `Action_PressSpecialButton` | `0x8013950C` | Press special button |
| 73 | `Action_PressTagButton` | `0x80139550` | Press tag button |
| 74 | `Action_PressActionButton` | `0x80139594` | Press action button |
| 75 | `Action_UseWeapon` | `0x801395E4` | Use equipped weapon |
| 76 | `Action_SetInvulnerable` | `0x801305D0` | Set invulnerable |
| 77 | `Action_DontPush` | `0x80130820` | Disable push force |
| 78 | `Action_PressJumpButton` | `0x80139620` | Press jump button |
| 79 | `Action_AddToSet` | `0x80139664` | Add to character set |
| 80 | `Action_GoToDoorTrigger` | `0x8012D8F8` | Go to door trigger |
| 81 | `Action_ResetDoorTrigger` | `0x80137E10` | Reset door trigger |
| 82 | `Action_SetNearestSpline` | `0x801397A4` | Set nearest spline |
| 83 | `Action_SetRandomSpline` | `0x801398E0` | Set random spline |
| 84 | `Action_FollowSpline` | `0x80130A0C` | Follow a spline path |
| 85 | `Action_SetControlSystem` | `0x80139710` | Set control system |
| 86 | `Action_PlaceOnSock` | `0x80130C60` | Place on sock object |
| 87 | `Action_FollowSock` | `0x80131078` | Follow sock object |
| 88 | `Action_SetZeroAcceleration` | `0x80139A44` | Zero acceleration |
| 89 | `Action_BreakFormation` | `0x80139B48` | Break formation |
| 90 | `Action_FormationMove` | `0x80139B70` | Move in formation |
| 91 | `Action_CreateCreatures` | `0x801311F4` | Create creatures |
| 92 | `Action_CreateSockCreatures` | `0x801318D0` | Create sock creatures |
| 93 | `Action_CreateSplineCreatures` | `0x80132194` | Create spline creatures |
| 94 | `Action_SetSpeed` | `0x80139D0C` | Set movement speed |
| 95 | `Action_SetRunSpeed` | `0x801326F0` | Set run speed |
| 96 | `Action_SetWalkSpeed` | `0x80139D84` | Set walk speed |
| 97 | `Action_SetHitPoints` | `0x80139E14` | Set HP |
| 98 | `Action_SetMessage` | `0x8013287C` | Set message text |
| 99 | `Action_AddPart` | `0x801329FC` | Add body part |
| 100 | `Action_AddPartDebris` | `0x80132C40` | Add breakable part |
| 101 | `Action_ActivateTurret` | `0x80139F30` | Activate turret gun |
| 102 | `Action_SetHoverPhase` | `0x8013A014` | Set hover phase |
| 103 | `Action_UseCurrentSpeed` | `0x8013A0F8` | Use current speed |
| 104 | `Action_SetMaxMovementRange` | `0x80132E78` | Set max movement range |
| 105 | `Action_SetDefaultMovementRange` | `0x8013A264` | Set default range |
| 106 | `Action_SetGravityHeight` | `0x8013A2C4` | Set gravity height |
| 107 | `Action_ApplyGravity` | `0x8013A35C` | Apply gravity to character |
| 108 | `Action_IgnoreShoveSystem` | `0x8013A3F8` | Ignore pushing |
| 109 | `Action_CannotBeSeen` | `0x8013A494` | Become invisible |
| 111 | `Action_RaceOpponent` | `0x8013301C` | Race opponent (podrace) |
| 112 | `Action_ZamMovement` | `0x80133278` | Zam movement type |
| 113 | `Action_SetVisibility` | `0x8013A530` | Set visible/invisible |
| 114 | `Action_EnableSock` | `0x8013A608` | Enable sock object |
| 115 | `Action_MakeExplosion` | `0x801334CC` | Spawn explosion |
| 116 | `Action_JudderGameCamera` | `0x8013A6E0` | Shake game camera |
| 117 | `Action_CameraShake` | `0x8013A7E8` | Shake camera |
| 118 | `Action_ResetGameCamera` | `0x8013A900` | Reset camera position |
| 119 | `Action_PlayCutScene` | `0x8013A934` | Play a cutscene |
| 120 | `Action_SetLevelPath` | `0x8013A9BC` | Set level path |
| 121 | `Action_ImmuneToKillTerrain` | `0x80138020` | Immune to kill terrain |
| 122 | `Action_Respawnable` | `0x801380BC` | Can respawn |
| 123 | `Action_SetPathCnxFlag` | `0x80134A14` | Set path connection flag |
| 124 | `Action_SetHint` | `0x8013AAB0` | Set gameplay hint |
| 125 | `Action_SetHintComplete` | `0x8013AB40` | Mark hint complete |
| 126 | `Action_CancelHint` | `0x8013ABCC` | Cancel hint |
| 127 | `Action_CycleCharacter` | `0x801336A0` | Cycle to next character |
| 128 | `Action_CnxController` | `0x80133AD8` | Connection controller |
| 129 | `Action_ObjectController` | `0x80133EB0` | Object controller |
| 130 | `Action_PlaySfx` | `0x80134C48` | Play sound effect |
| 131 | `Action_CameraCut` | `0x80134EA8` | Camera cut |
| 132 | `Action_SetCircleDirection` | `0x801393E0` | Set circle direction |
| 133 | `Action_DontRaycastLOS` | `0x8013C030` | Disable LOS raycast |
| 134 | `Action_SetForceBack` | `0x8013C0C4` | Push back with Force |
| 135 | `Action_SetForceLightningTarget` | `0x8013C1F4` | Force lightning target |
| 136 | `Action_FaceCamera` | `0x8013C2E8` | Face the camera |
| 137 | `Action_FaceCharacter` | `0x8013C414` | Face another character |
| 138 | `Action_FollowPlayer` | `0x8013C4F4` | Follow player |
| 139 | `Action_SetFormationCommander` | `0x80139B98` | Set formation leader |
| 140 | `Action_RemoveThrownForceObjects` | `0x80139C90` | Remove thrown objects |
| 144 | `Action_BoulderSection` | `0x800C5D68` | Boulder chase section |

## Level Conditions (`Condition_*`)

| Idx | Name | Address | Description |
|-----|------|---------|-------------|
| 0 | `Condition_Active` | `0x80135728` | Character is active |
| 1 | `Condition_GotBlaster` | `0x80135754` | Has blaster equipped |
| 2 | `Condition_GlynTest` | `0x80135798` | Glyn debug test |
| 3 | `Condition_IsAlive` | `0x8013642C` | Is alive |
| 4 | `Condition_IsOnScreen` | `0x801365AC` | On screen |
| 5 | `Condition_OnObject` | `0x801368F4` | Standing on object |
| 6 | `Condition_OnSameObjectAsPlayer` | `0x80136974` | Same object as player |
| 7 | `Condition_PlayerOnObject` | `0x80136710` | Player on object |
| 8 | `Condition_EitherPlayerOnObject` | `0x801367D8` | Either player on object |
| 9 | `Condition_OnGround` | `0x80136634` | Touching ground |
| 10 | `Condition_BeenAlerted` | `0x80136678` | Has been alerted |
| 11 | `Condition_PlayerOnGround` | `0x80136604` | Player on ground |
| 12 | `Condition_LocatedCollectable` | `0x8012D56C` | Found collectable |
| 13 | `Condition_CollectedCount` | `0x801369E0` | Number collected |
| 14 | `Condition_SpawnCount` | `0x80136A40` | Number spawned |
| 15 | `Condition_BehindCamera` | `0x80136A84` | Behind camera view |
| 16 | `Condition_LocatorOnScreen` | `0x80136B4C` | Locator on screen |
| 17 | `Condition_Blocking` | `0x80136BA8` | Path blocked |
| 18 | `Condition_BeenHit` | `0x80136BFC` | Has been hit |
| 19 | `Condition_HoverPhase` | `0x80136C54` | Current hover phase |
| 20 | `Condition_HitPoints` | `0x80136CEC` | HP check |
| 21 | `Condition_StuckTime` | `0x80136D58` | Time stuck |
| 22 | `Condition_DoorLocked` | `0x801357CC` | Door is locked |
| 23 | `Condition_DoorOpened` | `0x8013581C` | Door was opened |
| 24 | `Condition_DoorOpenedByPlayer` | `0x80135848` | Player opened door |
| 25 | `Condition_DoorOpen` | `0x801358A8` | Door is currently open |
| 26 | `Condition_ObstacleFinished` | `0x801358F8` | Obstacle animation done |
| 27 | `Condition_ObstacleChainPhase` | `0x8013596C` | Chain phase check |
| 28 | `Condition_AnimationFinished` | `0x80137428` | Animation complete |
| 29 | `Condition_GadgetActive` | `0x80135A08` | Gadget is active |
| 30 | `Condition_TubeActive` | `0x80135A64` | Tube is active |
| 31 | `Condition_PlayerUsingForce` | `0x80136240` | Player using Force |
| 32 | `Condition_PlayerDeflectingPart` | `0x801362E8` | Player deflecting |
| 33 | `Condition_EitherPlayerUsingForce` | `0x8012D420` | Either player using Force |
| 34 | `Condition_ForceBeingUsed` | `0x8013636C` | Force being used nearby |
| 35 | `Condition_UsingForce` | `0x80135AB8` | Self using Force |
| 36 | `Condition_UsingForceLightning` | `0x80135B70` | Self using Force lightning |
| 37 | `Condition_TurretAlive` | `0x80135BD8` | Turret is alive |
| 38 | `Condition_ForceComplete` | `0x801363C0` | Force use complete |
| 39 | `Condition_IAm` | `0x80135C44` | My name matches |
| 40 | `Condition_IAmA` | `0x80135E8C` | My type matches |
| 41 | `Condition_CanFightLikeAJedi` | `0x80135ED0` | Can fight as Jedi |
| 42 | `Condition_Side` | `0x80135FCC` | Team side check |
| 43 | `Condition_IAmAPartyCharacter` | `0x80135DDC` | Is party character |
| 44 | `Condition_CategoryIs` | `0x80135D2C` | Category match |
| 45 | `Condition_PlayerCategoryIs` | `0x80135D88` | Player category match |
| 46 | `Condition_EitherPlayerIs` | `0x80136084` | Either player matches |
| 47 | `Condition_CheckFlag` | `0x801360F8` | Check flag |
| 48 | `Condition_CheckLevelFlag` | `0x80136184` | Check level flag |
| 49 | `Condition_CheckAreaFlag` | `0x801361F4` | Check area flag |
| 50 | `Condition_PlayerOnDoorTrigger` | `0x80136DA8` | Player on door trigger |
| 51 | `Condition_EitherPlayerOnDoorTrigger` | `0x80136E04` | Either player on trigger |
| 52 | `Condition_IsSetAlive` | `0x80136EB8` | Set member alive |
| 53 | `Condition_Context` | `0x80136EF0` | AI context check |
| 54 | `Condition_Player2Active` | `0x80136F58` | Player 2 is playing |
| 55 | `Condition_AngleFromPlayerToMe` | `0x80136F7C` | Player->self angle |
| 56 | `Condition_AngleFromPlayer2ToMe` | `0x80137010` | Player2->self angle |
| 57 | `Condition_NumBaddies` | `0x801370AC` | Number of enemies |
| 58 | `Condition_NumForceObjects` | `0x801371EC` | Number of force objects |
| 59 | `Condition_BeenToLevel` | `0x80137318` | Has visited level |
| 60 | `Condition_Message` | `0x801373BC` | Message triggered |
| 61 | `Condition_CutSceneStarted` | `0x80137518` | Cutscene started |
| 62 | `Condition_CutSceneFinished` | `0x80137580` | Cutscene finished |
| 63 | `Condition_CutScenePlaying` | `0x801375D0` | Cutscene is playing |
| 64 | `Condition_RigidAnimFrame` | `0x801374D8` | Rigid anim frame check |
| 65 | `Condition_RaceLap` | `0x8013761C` | Race lap check |
| 66 | `Condition_SockDistanceToPlayer` | `0x80137650` | Sock->player distance |
| 67 | `Condition_SockDistanceToOpponent` | `0x801376F4` | Sock->opponent distance |
| 68 | `Condition_SockXDistanceToPlayer` | `0x8013779C` | X distance sock->player |
| 69 | `Condition_PlayerDistanceAlongSock` | `0x8013786C` | Player progress on sock |
| 70 | `Condition_FurthestPlayerDistanceAlongSock` | `0x8013788C` | Furthest player on sock |
| 71 | `Condition_FinishedSpline` | `0x8013791C` | Spline traversal done |
| 72 | `Condition_IsSplineSet` | `0x801378CC` | Spline is assigned |
| 73 | `Condition_CurrentHintId` | `0x80137984` | Current hint ID |
| 74 | `Condition_HintAvailable` | `0x801379F8` | Hint is available |
| 75 | `Condition_HintComplete` | `0x80137A3C` | Hint is complete |
| 76 | `Condition_Freeplay` | `0x80137A80` | Freeplay mode active |
| 77 | `Condition_CameraYRot` | `0x80137AB4` | Camera Y rotation |
| 78 | `Condition_UsingForceFreeze` | `0x80137AF8` | Using Force freeze |
| 79 | `Condition_FromNode` | `0x80137BB8` | From nav node |
| 80 | `Condition_ToNode` | `0x80137C5C` | To nav node |
| 81 | `Condition_AIOverrideControl` | `0x80136514` | AI override active |
| 82 | `Condition_StuckDontJumpNow` | `0x80137CB8` | Don't jump when stuck |
| 83 | `Condition_BoltsDontGetDeflectedBack` | `0x80137CE4` | Bolts don't deflect |
| 84 | `Condition_CheatProgress` | `0x80137D20` | Cheat progress |
| 85 | `Condition_BigJumpComplete` | `0x80137D2C` | Big jump done |
| 86 | `Condition_RespawnLocatorIs` | `0x80137DA0` | Respawn locator matches |
| 87 | `Condition_InMiniCut` | `0x80137DD4` | In mini cutscene |
| 88 | `Condition_MaulShouldRunAway` | `0x8012D7E0` | Maul should flee |
| 89 | `Condition_DropBackInTimer` | `0x80137E08` | Drop back timer |
