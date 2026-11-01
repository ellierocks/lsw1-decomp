# LSW1 game struct headers

The first Phase 3 game-header batch uses sparse GC layouts under
[`include/game`](../include/game). Engine headers remain independently owned.
Names describe the checked consumers; unknown bytes remain `char unk[N]`.
Except for the game-object array stride, `sizeof` is only the extent of the
confirmed view, not an allocation size. Do not use the other views for arrays.

## Selection and access ranking

Retail assembly was scanned in `auto/game/{creature,obstacle,pickups,turret}`,
`auto/characters/apiobject` and `auto/ai`. Ranked discovery signatures:

| Rank | Type | Candidate accessing functions | Signature |
| --- | --- | ---: | --- |
| 1 | `NUGAMEOBJECT_s` | 50 | `lwz +0x60` and `lwz +0x64` through the same register in one function |
| 2 | `NUAIPACKET_s` | 35 | `lwz/stw/lfs/stfs` through incoming r5 at one of the confirmed packet offsets, in aisys |
| 3 | `NUAISCRIPTPROCESS_s` | 2 | timer `+0x24` through incoming r4 in aisys |
| 4 | `NUCHARACTERDATA_s` | 1 | character-data load `+0x64`, then callback load `+0x1C` within four instructions |
| 4 | `NUAPIOBJECT_s` | 1 | owner `+0x50`, then float `+0x1E8` in `Condition_StuckTime` |

These are reproducible syntactic candidate counts, not a whole-program type or
alias proof: register reuse and other structs can create false hits, and moved
pointers and other members create misses. Character data is included despite
its narrow callback signature because it is reached from the highest-ranked
object. The strict consumers below establish the published fields. Local scan
lists are retained in `work/gameheaders_{character,ai}_rank.json`.

## Confirmed fields

[`creature.h`](../include/game/creature.h), `NUGAMEOBJECT_s`:

| Offset | Field / width | Retail evidence in `fn_800AA92C` |
| --- | --- | --- |
| `0x60` | `characterModel`, pointer | `lwz`, passed to renderer |
| `0x64` | `characterData`, pointer | `lwz`, then callback load |
| `0x208` | `motionFlags`, u32 | `lwz`, mask `0x800` skips draw |
| `0x28C` | `drawResult`, u8 | renderer result `stb`, subsequent `lbz` |
| `0x290` | `drawUpdated`, u8 | `stb 1` |
| `0xEEC` | `playerPacket`, pointer | `lwz`, argument formed at `+0x194` |
| `0xF28` | `flagsF28`, u32 | `lwz/stw`, draw mask `0x200`, result bit `0x100` |
| `0x1060` | `drawFlags`, u32 | `lwz`, renderer argument |
| `0x1080` | `drawMatrixEnabled`, u8 | `lbz`, selects matrix argument |

The loop advances by **0x1098** at `0x800AAA0C`. Matrix arguments at `+8`,
`+0xC8`, `+0x108`, `+0x148` remain opaque: passing an address does not prove
member extents. The API-object relationship is not established by this batch;
no embedding or cast between `NUGAMEOBJECT_s` and `NUAPIOBJECT_s` is declared.

[`character.h`](../include/game/character.h), `NUCHARACTERDATA_s`: `drawFn`
at `0x1C`, a function pointer taking the object and returning void, loaded and
called by the same loop. No total character-data size is established.

[`apiobject.h`](../include/game/apiobject.h), `NUAPIOBJECT_s`: `stuckTime`
at `0x1E8`, `lfs` in `Condition_StuckTime` after loading the owner from the packet.
The name identifies this condition's observable value; it does not establish
whether other consumers use it as a respawn timer.

[`ai.h`](../include/game/ai.h), `NUAIPACKET_s`:

| Offset | Field / width | Strict consumer |
| --- | --- | --- |
| `0x50` | `owner`, pointer | `Condition_StuckTime` |
| `0x54`, `0x58` | `nearestOpponent`, pointer; `nearestOpponentRange`, f32 | `Condition_NearestOpponentRange` |
| `0x64`, `0x68` | `opponent`, pointer; `opponentRange`, f32 | `Condition_GotOpponent`, `Condition_OpponentRange` |
| `0xF8` | `triggerArea`, pointer | `Condition_GotTriggerArea` |
| `0xFC` | `locator`, pointer | `Condition_GotLocator` |
| `0x118` | `pathFlags`, u32 | `Condition_OnPath`, mask `0x8000` |
| `0x1A0` | `movementFlags`, u32 | `Condition_PathBlocked`, mask `0x100` |
| `0x1C0` | `timeOffPath`, f32 | `Condition_TimeOffPath` |

Pointer target types for locator/area remain forward declarations. The same
header's `NUAISCRIPTPROCESS_s` confirms only `timer`, f32 at `0x24`, via
`Condition_Timer` (`lfs`) and `Action_ResetTimer` (`stfs`).

## Donors, matching and limits

OpenSaga pin `7bb2402aa902d08f0979892ca23ee80e809e5ee1` was inspected through
`legoapi/items/base/apiobject.h`, `legoapi/characters/core/character.h`,
`legoapi/ai/game/creature.cpp`, `legoapi/ai/core/aisysall.cpp` and
`gameapi/ai/aisys/aiscript_types.h`. The semantic leads transfer, but donor
layout does not: game-object stride is GC `0x1098`, callback GC `+0x1C`
versus donor `+0x20`; packet owner is GC `+0x50` versus donor `+0xD0`;
time off path is GC `+0x1C0` versus donor `+0x204`.
Donor `GotLocator`/trigger-area uses processor fields while GC uses the packet;
`PathBlocked` uses a different byte/mask; TCS `ResetTimer` parameter parsing is
absent from GC. These transfers were rejected, and all other regions stay opaque.

Each of the 12 consumers above has a task pack, donor review and
`hillclimb.py try --donor-note` record locally. Reproduce acceptance with:

```sh
python3 tools/verify_fn.py NAME --unit auto/ai/aisys/aisys --build --strict-symbols
python3 tools/verify_fn.py fn_800AA92C --unit auto/game/creature --build --strict-symbols
```

Eleven functions are new matches; `Condition_Timer` already matched and now uses
the shared type. None is parked. The draw loop improved from 58/64 to 64/64
instructions by keeping conditional arguments inline. `Condition_TimeOffPath`
improved from 4/7 to 7/7 with a minimized redundant guard found by
`tools/permute_setup.py Condition_TimeOffPath --run -- -j 4 --stop-on-zero --best-only`
(75 generated candidates). Manual/ledger attempts stayed below 15 per target;
shared-tool `Text file busy` retries are retained in the ledger. There were no
independent-store ordering near-misses requiring `order_search.py`.

Both units remain **NonMatching**: the other functions and complete unit sections
and relocations are unproven. **Zero promoted bytes**, no split, symbol, pin or
oracle changes. Timestamps live locally in `work/gameheaders_timing.txt`.
The pre-rebase report moves from 1,322 to 1,323 functions and 186,400 to
186,656 code bytes: objdiff does not count the ten new AI strict matches whose
constant relocations use section-relative rather than retail symbol names.
Strict verification checks the referenced bytes; the two measures stay separate.

Next: API-object position/flags and its relation to the creature prefix, character
data extents, then the pickup and turret records; prove each with additional
consumers before expanding these views.
