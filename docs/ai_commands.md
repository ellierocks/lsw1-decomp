# AI Commands Reference

## Overview
Two dispatch tables control AI behavior:
- `AI_Actions_API` (45 entries) — at `0x801BC288`
- `AI_Conditions_API` (41 entries) — at `0x801BC090`

Dispatched via `AIScriptParseConditions` and `AIScriptLoadScp`.

## AI Actions (`Action_*`)

| Index | Name | Address | Size | Description |
|-------|------|---------|------|-------------|
| 0 | `Action_Idle` | `0x8007F8A8` | 0x354 | No-op tick |
| 1 | `Action_SetState` | `0x80084C88` | 0x9C | Set AI finite-state state |
| 2 | `Action_ResetTimer` | `0x80084D24` | 0x1C | Reset internal timer to 0 |
| 3 | `Action_RetreatFromNearestOpponent` | `0x800809AC` | — | Retreat from nearest enemy |
| 4 | `Action_RetreatFromOpponent` | `0x80080800` | — | Retreat from current target |
| 5 | `Action_MoveAwayFromPlayer` | `0x8008063C` | — | Move away from player |
| 6 | `Action_CircleOpponent` | `0x800803CC` | — | Circle around opponent |
| 7 | `Action_CirclePlayer` | `0x8008014C` | — | Circle around player |
| 8 | `Action_FollowPlayer` | `0x8007FF9C` | — | Follow player entity |
| 9 | `Action_MoveAwayFromOpponent` | `0x8007FDBC` | — | Move away from opponent |
| 10 | `Action_FollowOpponent` | `0x8007FC1C` | — | Follow opponent entity |
| 11 | `Action_FacePlayer` | `0x80084D40` | - | Turn to face player |
| 12 | `Action_FaceOpponent` | `0x80080B50` | — | Turn to face opponent |
| 13 | `Action_IgnoreWallSplines` | `0x80084EA8` | — | Disable wall-spline collision |
| 14 | `Action_CheckWallSplines` | `0x80084F2C` | — | Enable wall-spline collision |
| 15 | `Action_NoTerrain` | `0x80084FB0` | — | Disable terrain following |
| 16 | `Action_FlatTerrain` | `0x800850D8` | — | Enable flat terrain mode |
| 17 | `Action_ShadowTerrain` | `0x8008516C` | — | Enable shadow terrain |
| 18 | `Action_DontUseShadowTerrain` | `0x80085200` | — | Disable shadow terrain |
| 19 | `Action_DontPush` | `0x80085294` | — | Disable push physics |
| 20 | `Action_CanSeeBehind` | `0x80085328` | — | Enable 360° vision |
| 21 | `Action_RequiresLOS` | `0x800853BC` | — | Require line-of-sight |
| 22 | `Action_SetFullPathSearch` | `0x80085440` | — | Full pathfinding mode |
| 23 | `Action_SetViewDistance` | `0x80081068` | — | Set view distance |
| 24 | `Action_SetMaxViewHeight` | `0x800813B0` | — | Set max view height |
| 25 | `Action_SetMinViewHeight` | `0x8008120C` | — | Set min view height |
| 26 | `Action_SetHearDistance` | `0x80081554` | — | Set hearing range |
| 27 | `Action_SetMoveRadius` | `0x800854D4` | — | Set movement radius |
| 28 | `Action_GoToNode` | `0x800816F8` | — | Move to nav node |
| 29 | `Action_GoToNodeRandom` | `0x800819A4` | — | Move to random nav node |
| 30 | `Action_GoToOrigin` | `0x80081B7C` | — | Move to spawn origin |
| 31 | `Action_GoToLocator` | `0x800820DC` | — | Move to locator |
| 32 | `Action_SetLocator` | `0x80081E00` | — | Set target locator |
| 33 | `Action_FollowPath` | `0x80085628` | — | Follow path spline |
| 34 | `Action_MoveAwayFromNode` | `0x8008296C` | — | Move away from node |
| 35 | `Action_OverrideAnimation` | `0x80082C18` | — | Override current anim |
| 36 | `Action_BlockPath` | `0x80082E18` | — | Block a path connection |
| 37 | `Action_PathConnectionObstacle` | `0x800856F8` | — | Treat connection as obstacle |
| 38 | `Action_PathConnectionMaxLength` | `0x80083190` | — | Set max path connection length |
| 39 | `Action_NoLosCheck` | `0x80085044` | — | Disable LOS check |
| 40 | `Action_ResetToOrigin` | `0x800857FC` | — | Reset position to origin |
| 41 | `Action_SetInterrupt` | `0x800833DC` | — | Set interrupt flag |
| 42 | `Action_ClearInterrupt` | `0x8008584C` | — | Clear interrupt flag |
| 43 | `Action_SetIgnoreAntinodes` | `0x800858D8` | — | Ignore anti-nodes |
| 44 | `Action_NoShadows` | `0x800859BC` | — | Disable shadow rendering |

## AI Conditions (`Condition_*`)

| Index | Name | Address | Description |
|-------|------|---------|-------------|
| 0 | `Condition_LocatorRange` | `0x80083AC8` | Distance to locator < threshold |
| 1 | `Condition_LocatorRangeXZ` | `0x80083B2C` | XZ distance to locator < threshold |
| 2 | `Condition_Timer` | `0x80083A6C` | Timer >= threshold (8B function!) |
| 3 | `Condition_Random` | `0x80083A74` | Random chance check |
| 4 | `Condition_GotLocator` | `0x80083B90` | Has a valid locator target |
| 5 | `Condition_CurrentLocatorIs` | `0x80083C38` | Current locator matches name |
| 6 | `Condition_InTriggerArea` | `0x80083CA0` | In trigger volume |
| 7 | `Condition_PlayerRange` | `0x80083D30` | Distance to player < threshold |
| 8 | `Condition_NearestPlayerRange` | `0x80083D90` | Distance to nearest player |
| 9 | `Condition_InLevelNode` | `0x80083EA0` | In a level nav node |
| 10 | `Condition_PlayerInLevelNode` | `0x80083F4C` | Player in nav node |
| 11 | `Condition_EitherPlayerInLevelNode` | `0x80083FB4` | Either player in nav node |
| 12 | `Condition_LevelNodeRange` | `0x800840A8` | Level node distance |
| 13 | `Condition_GotTriggerArea` | `0x800840FC` | Has trigger area reference |
| 14 | `Condition_PlayerInTriggerArea` | `0x8008415C` | Player in volume |
| 15 | `Condition_Player2InTriggerArea` | `0x80084200` | Player 2 in volume |
| 16 | `Condition_EitherPlayerInTriggerArea` | `0x800842A4` | Either player in volume |
| 17 | `Condition_BaddyInTriggerArea` | `0x800843B0` | Enemy in volume |
| 18 | `Condition_GoodyInTriggerArea` | `0x80084430` | Friendly in volume |
| 19 | `Condition_OpponentInTriggerArea` | `0x800844B0` | Opponent in volume |
| 20 | `Condition_GotOpponent` | `0x8008454C` | Has opponent target |
| 21 | `Condition_OpponentIsAThreat` | `0x80084578` | Opponent is threatening |
| 22 | `Condition_OpponentOnSamePath` | `0x800845B0` | Shared path with opponent |
| 23 | `Condition_OpponentRange` | `0x80084600` | Opponent distance |
| 24 | `Condition_NearestOpponentRange` | `0x80084628` | Nearest opponent distance |
| 25 | `Condition_YawToOpponent` | `0x80084650` | Angle to opponent |
| 26 | `Condition_OpponentBelow` | `0x80084700` | Opponent below me |
| 27 | `Condition_OriginRange` | `0x8008474C` | Distance from origin |
| 28 | `Condition_OpponentToOrigin` | `0x800847B8` | Opponent->origin distance |
| 29 | `Condition_PlayerToOrigin` | `0x80084824` | Player->origin distance |
| 30 | `Condition_OpponentToLocator` | `0x800848C4` | Opponent->locator distance |
| 31 | `Condition_OpponentToLocatorXZ` | `0x80084928` | XZ distance opponent->locator |
| 32 | `Condition_OpponentToLocatorY` | `0x8008498C` | Y distance opponent->locator |
| 33 | `Condition_PlayerToLocator` | `0x80084A08` | Player->locator distance |
| 34 | `Condition_NearestPlayerToLocator` | `0x80084A74` | Nearest player->locator |
| 35 | `Condition_OnPath` | `0x80083BBC` | Currently on path |
| 36 | `Condition_TimeOffPath` | `0x80083BE8` | Time since left path |
| 37 | `Condition_PathBlocked` | `0x80084B3C` | Path is blocked |
| 38 | `Condition_InterruptID` | `0x80084B68` | Interrupt ID matches |
| 39 | `Condition_IAm` | `0x80084BEC` | My name matches |
| 40 | `Condition_StuckTime` | `0x80084C60` | Time stuck in place |

## Script Keyword Parsers

| Parser | Address | Handler name | Keywords handled |
|--------|---------|-------------|-----------------|
| Script Parser 1 | `0x801BC040` | `AIScriptParseConditions` | CONDITIONS, ACTIONS, REFERENCESCRIPT, `}` |
| Script Parser 2 | `0x801BC068` | various | STATE, PARAM, REFERENCESCRIPT, DERIVEFROMSCRIPT |
