# LSW1 game struct headers

The first Phase 3 game-header batch uses sparse GC layouts under
[`include/game`](../include/game). Engine headers remain independently owned.
Names describe the checked consumers; unknown bytes remain `char unk[N]`.
Only explicitly documented strides or allocation sizes establish full extents;
other `sizeof` values describe confirmed views and must not be used for arrays.

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

## Round 2: API-object prefix

The GC collision routines `fn_80090FB0` and `fn_8009083C` use the same
object view. `fn_80090FB0` reads three floats at `+0x90`, `+0x94`, `+0x98`
for the collision separation, floats at `+0x1EC` and `+0x1F0` for its
radius/height calculation, and two 32-bit mask words each at `+0x1F8` and
`+0x200` (including the adjacent words). `fn_8009083C` reads the 32-bit
flags at `+0x20C` and reads/writes floats at `+0x78`, `+0x80`, `+0x210`
and `+0x218`; its typed C strictly matches 84/84 instructions. The middle
velocity word at `+0x7C` is read by `fn_800902C0`. These facts add the sparse
`NUAPIOBJECT_s` fields; other bytes remain opaque.

`fn_800A3224` accesses character model and data at `+0x60` and `+0x64`,
`motionFlags` at `+0x208`, the float at `+0x1E8`, and flags at `+0x20C`
on the same creature pointer. Together with the matching offsets and the
collision consumers, this supports a shared prefix. The two types remain
separate views pending proof of the allocation and embedding relationship.
OpenSaga's similarly named fields lie at different offsets; GC accesses
control this layout.

### Character-data view

| Offset | Width | GC access evidence |
| --- | --- | --- |
| `0x04` | word flags | `fn_800A2A9C` and `fn_800A3650` load and mask it |
| `0x1C` | callback pointer | `fn_800AA92C` loads and calls it; strict match retained |
| `0x20` | pointer | `fn_800A474C` loads it and accesses the pointed-to data |
| `0x2C` | float | `fn_800A2498` and `fn_800A41D8` use `lfs` |
| `0x38` | float | `fn_800A39A8` and `fn_800A41D8` use `lfs` |
| `0x40` | word flags | `fn_800A3650` loads it and combines flag masks |

The confirmed view extends through `0x44`; no allocation size or array stride
has been established. OpenSaga's `0x4C` character record is not GC size
proof, and its callback is at `0x20`, four bytes later than GC. Determining
full extent remains open.

### Pickup records

`fn_8013C620` iterates six groups, loading pointers at `+0x71C` and
counts at `+0x734` from the pool and stepping each record by `0x40`. It
stores a float at record `+0x2C` and bytes at `+0x38` and `+0x39`;
`fn_8013D2F4` reads and writes words at `+0x18` and `+0x1C`. The group
initialization draft is parked at 41/51: loop register allocation and float
load placement differ. Next, reshape the group loop to preserve the loop index
in r11 and defer the float load until after the count check.

A separate callback record has a float at `+0x100` and a signed halfword at
`+0x122`: `fn_8013F1B0` stores the float and `fn_8013F1DC` loads both.
Both consumers strictly match. Its full size is unknown. OpenSaga lookup found
no same-name hits and a pickup search did not establish an equivalent.

### Turret records

`fn_8014068C` aligns and clears an allocation of `0xF08` bytes and stores
the incoming word at `+0xF04`; its typed allocation consumer strictly matches.
`fn_801403BC` walks a separate record at stride `0x78`, copying halfwords
`+0x70` to `+0x6E` and bytes `+0x74` to `+0x73`; `fn_80140504` loads and
modifies `+0x6E` as a halfword flag field and `+0x73` as a byte counter.
The relationship between this array and the allocated turret record remains
unconfirmed. OpenSaga lookup found no same-name hit and a turret search did not
establish an equivalent.
