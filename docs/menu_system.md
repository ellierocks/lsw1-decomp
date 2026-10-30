# Menu System

This document covers the game's menu system, including the hidden debug menu ("LEGO Options"), the menu descriptor callback table, the runtime menu stack, and Dolphin memory watch addresses for runtime state.

## Overview

LEGO Star Wars contains a hidden debug menu accessible through the pause menu. The visible option is labeled "LEGO Option" and provides access to scene selection, debug toggles, and other development features. The menu system uses a descriptor-based architecture where each menu type (pause, debug, scene select, etc.) is defined by callback functions registered at startup and managed through a runtime stack.

## Related Reference

- The Dolphin Memory Watch file at [Dolphin watch definitions](reference/CoolsWatches260522.dmw) contains the saved watch definitions.
- Credits: Coolestto found most of the memory addresses tracked in this watch file.

## Debug Menu / LEGO Options

### Known Behavior

- Most releases keep the debug menu code in the game and hide the pause-menu option behind one global bit/word.
- The visible menu option is `LEGO Options`.
- Speedrun practice enables it with a Gecko 32-bit write code targeting `0x804090F0` and writing `1`. In Gecko format, the leading `04` is the code type, so `044090F0` refers to address `0x804090F0`.
- The Dolphin Memory Watch file labels `0x804090F3` as `Debug Toggle`. This is the low byte of the big-endian word at `0x804090F0`.
- The menu contains a `Go To Scene` table that scrolls through every scene.

### Current Anchors

The gate variable:
```txt
gLegoOptionsMenuEnabled = .sdata:0x804090F0; // type:object size:0x4 data:4byte
```

There are two direct references in the current generated assembly:

- `PauseMenu_HandleSelection` at `0x800B2414` reads `gLegoOptionsMenuEnabled`; if nonzero, one path calls `Menu_Open(7, -1)`.
- `PauseMenu_BuildOptions` at `0x800B2588` reads `gLegoOptionsMenuEnabled`; if nonzero and another pause/menu state word is zero, it appends the string at `str_PauseMenu_LegoOptions`.

`str_PauseMenu_LegoOptions` is the `LEGO Options` string:
```txt
str_PauseMenu_LegoOptions = .rodata:0x801A0354; // type:object size:0xD data:string
```

## Memory Watches

### Debug Menu / Pause Menu Addresses

These addresses are the highest priority for the hidden `LEGO Options` work:

| Address | Watch label | Current generated symbol | Notes |
| --- | --- | --- | --- |
| `0x804090F3` | `Debug Toggle` | inside `gLegoOptionsMenuEnabled` | Low byte of the 32-bit debug flag word. |
| `0x804090F0` | implied debug flag word | `gLegoOptionsMenuEnabled` | Gecko `044090F0 00000001` writes this word. |
| `0x80408A27` | `Menu depth` | inside `gMenuDepth` | Low byte of the 32-bit menu stack depth at `0x80408A24`; used to distinguish pause/debug/scene-select depth. |
| `0x80408F9B` | `Menu controller toggle` | pending | Runtime menu control state candidate. |
| `0x8040912F` | `Freeze game` | pending | Runtime freeze/pause state candidate. |
| `0x803D9161` | `Current Menu (26 = pause)` | `gMenuStack[1] + 0x15` | Low byte of runtime descriptor index halfword at offset `0x14`. |
| `0x803D915F` | `Menu length` | `gMenuStack[1] + 0x13` | Low byte of vertical maximum halfword at offset `0x12`. |
| `0x803D9157` | `Selection` | `gMenuStack[1] + 0x0B` | Low byte of vertical/current selection halfword at offset `0x0A`. |
| `0x803D914D` | `Timer (Hold 0 if using Menu 24)` | `gMenuStack[1] + 0x01` | Byte inside the first float/animation field; exact meaning still pending. |
| `0x803D9241` | `CurrentMenu (31=debug)` | `gMenuStack[2] + 0x15` | Low byte of runtime descriptor index halfword at offset `0x14`. |
| `0x803D9237` | `SelectionIndex` | `gMenuStack[2] + 0x0B` | Low byte of vertical/current selection halfword at offset `0x0A`. |
| `0x803D9321` | `CurrentMenu (33=sceneselect)` | `gMenuStack[3] + 0x15` | Low byte of runtime descriptor index halfword at offset `0x14`. |
| `0x803D931D` | `Horizontal length (21)` | `gMenuStack[3] + 0x11` | Low byte of horizontal maximum halfword at offset `0x10`. |
| `0x803D931F` | `Vertical length (10)` | `gMenuStack[3] + 0x13` | Low byte of vertical maximum halfword at offset `0x12`. |
| `0x803D9315` | `Horizontal selection` | `gMenuStack[3] + 0x09` | Low byte of horizontal/default selection halfword at offset `0x08`. |
| `0x803D9317` | `Vertical Selection` | `gMenuStack[3] + 0x0B` | Low byte of vertical/current selection halfword at offset `0x0A`. |

GameCube memory is big-endian. The watch at `0x804090F3` points at the low byte of the word at `0x804090F0`, which explains why a 32-bit Gecko write of value `1` to `0x804090F0` enables the watched byte.

### Debug Menu Options

| Option | Value address | Label/string address | Generated symbol evidence |
| --- | --- | --- | --- |
| `Lift Player` | `0x80408FBC` | `0x801A0208` | `gDebugLiftPlayerEnabled`, `str_DebugMenu_LiftPlayer` |
| `FPS Display` | `0x80408FD0` | `0x801A0218` | `gDebugFpsDisplayEnabled`, `str_DebugMenu_FpsDisplay` |
| `Difficulty` | `0x803E0299` | `0x801A0228` | `gDifficulty`, `str_DebugMenu_Difficulty` |
| `Streaming display` | `0x80408FDF` | `0x801A0248` | `gDebugStreamingDisplayEnabled`, `str_DebugMenu_StreamingDisplay` |
| `Free Play` | `0x804091FF` / `0x804091FB` | pending | `lbl_804091FB` is the current generated byte symbol. |

Related display strings:

| Address | Current generated symbol | String |
| --- | --- | --- |
| `0x801A0194` | `str_DebugMenu_GoToScene` | `Go To Scene` |
| `0x801A0354` | `str_PauseMenu_LegoOptions` | `LEGO Options` |
| `0x801A0384` | `str_DebugMenu_DifficultyValue` | `Difficulty: ` |
| `0x801A5440` | `lbl_801A5440` | `Loading %s...` |
| `0x801A5450` | `lbl_801A5450` | `fps: %d` |

### Player Runtime Pointers

The watch file identifies `0x801D5338` onward as player pointer storage:

| Address | Watch label |
| --- | --- |
| `0x801D5338` | P1 pointer |
| `0x801D533C` | P2 pointer |
| `0x801D5340` | P3 pointer |
| `0x801D5344` | P4 pointer |
| `0x801D5348` | P5 pointer |
| `0x801D534C` | P6 pointer |

The same structure is heavily referenced by code around the pause/menu handlers.

### Other Useful Anchors

| Area | Addresses / notes |
| --- | --- |
| Inputs | `0x802292C0` digital inputs, `0x802292C2` left stick, `0x802292C4` right stick, `0x802292C6` analog L, `0x802292C7` analog R. |
| Studs | `0x803E049C` total balance, `0x8040B1AC` players total. |
| Game progress | `0x803E029A` superkit total, `0x8040B1A4` minikit total, `0x8040B1A8` True Jedi. |
| Camera | `0x803E01F4` X, `0x803E01F8` Y, `0x803E01FC` Z, `0x803E0276` tilt, `0x803E027A` pan. |
| Extras | `0x801D5996` through `0x801D5A26` tracks shop extras such as invincibility, silhouettes, moustaches, minikit detector, tea cups, brushes, and blaster modifiers. |

## Menu Definitions

### Registration

`Menu_InitDefinitions` calls the generic menu registration routine `fn_80078D08` with:

```txt
r3 = gMenuDefinitions
r4 = 0x1C
r5 = 9
r6 = fn_800B2E10
r7 = 1
```

The local table itself is `0x1F8` bytes: 18 entries at `0x1C` bytes each. `fn_80078D08` copies `0x1C`-byte entries into `gRegisteredMenuDefinitions` at `0x801BB52C`, but the `r4 = 0x1C` argument behaves like a registration span or count, not a byte stride. The routine stores `r4 + 0x18` into `gRegisteredMenuDefinitionCount`, so the local table appears to be merged into a larger descriptor registry.

`Menu_ResetStack` at `0x80078E40` clears the runtime menu stack and sets `gMenuDepth` to zero after registration.

### Descriptor Layout

Each entry is seven 32-bit fields:

| Offset | Meaning | Evidence |
| --- | --- | --- |
| `0x00` | menu definition ID | First word of each row is the ID passed to/opened by menu transition calls. |
| `0x04` | enter/re-enter callback or null | Called after `Menu_Open` builds a menu, and again when returning to a previous menu after close. |
| `0x08` | build/open callback | Called by `Menu_Open`; pause/debug rows use the known build callbacks here. |
| `0x0C` | selection/action callback | Pause/debug rows use the known selection callbacks here. |
| `0x10` | close/exit callback or null | Called by `Menu_CloseCurrent` before popping the current menu. |
| `0x14` | packed runtime defaults/state | `Menu_Open` and `Menu_CloseCurrent` copy two 16-bit runtime values through this word when it is not `-1`. |
| `0x18` | flags | `0x01000000` appears on some menu rows. |

### Menu Open / Close

`Menu_Open` at `0x80075D00` opens a menu by descriptor ID. The effective signature is:

```c
Menu_Open(menuDefinitionId, initialSelection)
```

Several callers pass a third argument, often `-1`, but `Menu_Open` overwrites that register early and does not use the incoming value.

The routine searches `gRegisteredMenuDefinitions` for a descriptor whose first word matches `menuDefinitionId`. On success, it increments `gMenuDepth`, initializes the current `0xE0`-byte entry in `gMenuStack`, stores the descriptor index into runtime offset `0x14`, calls descriptor field `0x08`, then calls descriptor field `0x04` if present. The second argument seeds the runtime selection when it is in range.

If no matching descriptor is found, the current runtime menu object's descriptor index is set to `-1`.

`Menu_CloseCurrent` at `0x80076034` closes the current menu. It calls descriptor field `0x10` if present, copies the current runtime defaults back into descriptor field `0x14`, marks the runtime descriptor index as `-1`, decrements `gMenuDepth`, then reinitializes the previous stack entry and calls its field `0x04` callback when present.

### Confirmed Rows

| Row | ID | Field `0x08` | Field `0x0C` | Status |
| --- | --- | --- | --- | --- |
| `2` | `2` | `PauseMenu_BuildOptions` | `PauseMenu_HandleSelection` | Confirmed pause menu callbacks. |
| `7` | `7` | `DebugMenu_BuildOptions` | `DebugMenu_HandleSelection` | Confirmed hidden debug menu / LEGO Options callbacks. |

The Dolphin Memory Watch file uses runtime menu IDs `26 = pause`, `31 = debug`, and `33 = sceneselect`. These are not the same as the local `gMenuDefinitions` row IDs above. They appear to be runtime menu-object state values after the descriptor table has been registered or expanded (see [Menu Runtime Stack](#menu-runtime-stack)).

### Table Rows

| Row | ID | Callback A | Build/update | Selection/action | Callback B | Parent/default | Flags |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0` | `0` | null | `fn_800B3378` | `fn_800B326C` | null | `-1` | `0x00000000` |
| `1` | `1` | null | `fn_800B11B4` | `fn_800B0DB8` | null | `-1` | `0x00000000` |
| `2` | `2` | null | `PauseMenu_BuildOptions` | `PauseMenu_HandleSelection` | null | `-1` | `0x00000000` |
| `3` | `3` | null | `fn_800B26BC` | `fn_800B30A0` | null | `-1` | `0x00000000` |
| `4` | `4` | `fn_800B2F60` | `fn_800B08D8` | `fn_800B05CC` | `fn_800B2FB8` | `0` | `0x00000000` |
| `5` | `5` | null | `fn_800B0C70` | `fn_800B0B64` | null | `0` | `0x00000000` |
| `6` | `6` | null | `fn_800B3414` | `fn_800B1318` | null | `-1` | `0x00000000` |
| `7` | `7` | null | `DebugMenu_BuildOptions` | `DebugMenu_HandleSelection` | null | `0` | `0x01000000` |
| `8` | `8` | null | `SceneSelect_BuildOptions` | `SceneSelect_HandleSelection` | null | `-1` | `0x01000000` |
| `9` | `9` | `fn_800B2FD8` | `fn_800B1CA4` | `fn_800B1B74` | null | `-1` | `0x00000000` |
| `10` | `10` | null | `fn_800B34A4` | `fn_800B320C` | null | `0` | `0x01000000` |
| `11` | `11` | null | `fn_800B3434` | `fn_800B3134` | null | `-1` | `0x00000000` |
| `12` | `22` | null | `fn_800B359C` | `fn_800B299C` | null | `-1` | `0x00000000` |
| `13` | `23` | null | `fn_800B2ADC` | `fn_800B3648` | null | `-1` | `0x00000000` |
| `14` | `24` | `fn_800B3668` | `fn_800B36A8` | `fn_800B3688` | null | `-1` | `0x00000000` |
| `15` | `25` | `fn_800B36C8` | `fn_800B36EC` | `fn_800B36CC` | null | `-1` | `0x00000000` |
| `16` | `26` | null | `fn_800B370C` | `fn_800B2BB4` | null | `-1` | `0x00000000` |
| `17` | `27` | null | `fn_800B3754` | `fn_800B3734` | null | `-1` | `0x00000000` |

## Menu Runtime Stack

### Overview

The game maintains a stack of menu objects, each `0xE0` bytes in size. The current menu depth is tracked at `0x80408A24` (`gMenuDepth`). When a menu is opened, a new entry is pushed onto the stack; when closed, it's popped.

### Stack Layout

The Dolphin Memory Watch addresses line up with consecutive `0xE0`-byte stack slots:

| Runtime state | Depth | Entry base | Watch evidence |
| --- | --- | --- | --- |
| Pause menu | `1` | `0x803D914C` | Watches at `0x803D914D`, `0x803D9157`, `0x803D915F`, `0x803D9161`. |
| Debug / LEGO Options | `2` | `0x803D922C` | Watches at `0x803D9237`, `0x803D9241`. |
| Scene select | `3` | `0x803D930C` | Watches at `0x803D9315`, `0x803D9317`, `0x803D931D`, `0x803D931F`, `0x803D9321`. |

These are stack entries, not separate globals. For example:

```text
0x803D9161 - 0x803D914C = 0x15
0x803D9241 - 0x803D922C = 0x15
0x803D9321 - 0x803D930C = 0x15
```

All three watch labels point at the low byte of runtime entry halfword `0x14`.

### Known Runtime Fields

The first `0x18` bytes now have good evidence:

| Offset | Size | Working name | Evidence |
| --- | --- | --- | --- |
| `0x00` | float | transition/animation value | Set to a float during open/close and reset. |
| `0x04` | float | transition/animation value | Set beside offset `0x00` during open. |
| `0x08` | s16 | horizontal/default selection | Saved to descriptor field `0x14`; scene-select watch uses low byte as horizontal selection. |
| `0x0A` | s16 | vertical/current selection | Seeded from `Menu_Open`'s second argument; saved to descriptor field `0x16`; pause/debug watches use low byte as selected index. |
| `0x0C` | s16 | horizontal minimum | Initialized to zero and used as a lower bound for offset `0x08`. |
| `0x0E` | s16 | vertical minimum | Initialized to zero and used as a lower bound for offset `0x0A`. |
| `0x10` | s16 | horizontal maximum | Scene-select builder writes `21`; used as an upper bound for offset `0x08`. |
| `0x12` | s16 | vertical maximum | Scene-select builder writes `10`; used as an upper bound for offset `0x0A`. |
| `0x14` | s16 | registered menu descriptor index | Set by `Menu_Open`; `-1` means no current descriptor. Watch labels low byte as current menu ID. |
| `0x16` | u8 | previous descriptor index candidate | Set from the prior entry's descriptor index during `Menu_Open`. |
| `0x17` | s8 | selected child/substate candidate | Reset to `-1` during `Menu_Open`. |

## Confirmed Symbol Names

These names have been promoted into `config/GL5E4F/symbols.txt`:

| Symbol | Address | Evidence |
| --- | --- | --- |
| `PauseMenu_BuildOptions` | `0x800B2588` | Menu callback table and pause-menu strings. |
| `PauseMenu_HandleSelection` | `0x800B2414` | Menu callback table and `LEGO Options` gate branch. |
| `DebugMenu_BuildOptions` | `0x800B178C` | Menu callback table and debug option strings. |
| `DebugMenu_HandleSelection` | `0x800B14B8` | Menu callback table and debug option toggle handling. |
| `Menu_Open` | `0x80075D00` | Searches `gRegisteredMenuDefinitions` by descriptor ID and pushes a `gMenuStack` entry. |
| `Menu_CloseCurrent` | `0x80076034` | Calls the current descriptor's close callback and pops `gMenuDepth`. |
| `Menu_ResetStack` | `0x80078E40` | Clears `gMenuStack` and resets `gMenuDepth` after registration. |
| `gMenuDefinitions` | `0x801D576C` | Registered descriptor table containing the pause/debug callback pairs. |
| `gRegisteredMenuDefinitions` | `0x801BB52C` | Runtime descriptor registry searched by `Menu_Open`. |
| `gMenuStack` | `0x803D906C` | Runtime stack of `0xE0`-byte menu objects. |
| `gMenuDepth` | `0x80408A24` | Current menu stack depth. |
| `gRegisteredMenuDefinitionCount` | `0x80408A2C` | Number/span used when searching the registered descriptor table. |
| `Menu_InitDefinitions` | `0x800B3D88` | Registers `gMenuDefinitions` with the generic menu system. |
| `gLegoOptionsMenuEnabled` | `0x804090F0` | Gecko code target, watch-file low-byte label, and code gate. |
| `gDebugLiftPlayerEnabled` | `0x80408FBC` | Watch-file label and `Lift Player` toggle code. |
| `gDebugFpsDisplayEnabled` | `0x80408FD0` | Watch-file label, `FPS Display` toggle code, and `fps: %d` use. |
| `gDebugLevelStreamingEnabled` | `0x80408FD8` | `Level Streaming` toggle code. |
| `gDebugStreamingDisplayEnabled` | `0x80408FDC` | Watch-file low-byte label and `Streaming display` toggle code. |
| `gDebugPanelEnabled` | `0x80408FCC` | `Panel` toggle code. |
| `gDifficulty` | `0x803E0299` | Watch-file label and debug difficulty submenu code. |


## Shop state and callbacks

The shop evidence below supports reconstructing pointer selection, callback
stacks and transition state. The invalid-category crash path is established
statically; the timing mechanism that reaches it remains a hypothesis.

### Current Crash Model

The best-supported crash model is now narrower than the original callback
desync theory:

1. The glitched shop enters `Shop_UpdateSubMenu`.
2. `gShopCurrentCategory` is not `0`, `1`, or `2`.
3. `Shop_UpdateSubMenu` takes the invalid-category path and clears its local
   category pointers.
4. A left/right input sets `gShopSubMenuScrollDirection`.
5. The scroll path dereferences the null category-count pointer.

The two highest-value crash PCs are:

| Address | Instruction role |
| --- | --- |
| `0x80148974` | Direction `1` path, loads `lwz r5, 0(r28)` from category count pointer. |
| `0x801489D8` | Direction `2` path, loads `lwz r5, 0(r28)` from category count pointer. |

At both PCs, `r28 == 0` means the crash is explained. The other category
pointers should also be zero from the same invalid-category path: `r30` ring
indices, `r29` cursor/position array, and `r31` item table.

### Known Runtime Watches

| Watch | Symbol / evidence | Meaning |
| --- | --- | --- |
| `0x803D9197` | `gMenuStack + 0x12B`, or `gMenuStack[1] + 0x4B` if the menu frame stride is `0xE0` | Shop/session menu-frame flag. Not yet classified. |
| `0x803D9207` | `gMenuStack + 0x19B`, or `gMenuStack[1] + 0xBB` | Shop/session menu-frame flag. Not yet classified. |
| `0x803D920B` | `gMenuStack + 0x19F`, or `gMenuStack[1] + 0xBF` | Shop/session menu-frame flag. Not yet classified. |
| `0x80408A27` | `gMenuDepth + 3` | Low byte of the 32-bit menu stack depth at `0x80408A24`. |
| `0x80409B9B` | `lbl_80409B98 + 3` | Low byte of an adjacent shop sdata word. Needs classification. |
| `0x80409C00` | `gShopSecretCodeBuffer` | Six live code-entry chars. The code check copies these bytes and appends a local NUL. |
| `0x80409C0B` | `gShopScrollMoveSoundLatch + 3` | Low byte of the 32-bit move-sound latch at `0x80409C08`. Left/right sets the word to `1`; no-scroll clears it. |
| `0x8041A568` | not covered by current `symbols.txt` | Dolphin watch reports current submenu label string, e.g. `Hints`. This is outside the configured main DOL symbol ranges seen so far. |
| `0x8040B2A0` | `gShopSecretCodeCursor` | Current secret-code slot, clamped `0..5`. |
| `0x80407023` | `HintItemRingIndices + 3` | Low byte of the Hints ring index word at `0x80407020`. |
| `0x80406FB3` | `CharacterItemRingIndices + 3` | Low byte of the Characters ring index word at `0x80406FB0`. |
| `0x80407093` | `ExtraItemRingIndices + 3` | Low byte of the Extras ring index word at `0x80407090`. |
| `0x8040A7FC` | `gShopMenuTransitionTimer` | Big-endian float. `BC 88 88 89` is about `-1/60`, not a byte array. |

The byte watches on `0x80406FB3`, `0x80407023`, `0x80407093`, and
`0x80409C0B` are low bytes because the game is big-endian.

### Core Shop Functions

| Function | Evidence | Working role |
| --- | --- | --- |
| `Shop_InitShelfItemPositions` | Finds three shop position specials and copies `nuvec` rows into shelf position arrays. | Builds reusable shelf/item position templates. |
| `Shop_InitExtras` | Initializes `ExtraItems`, sets `gShopExtraItemCount`, and fills `GameMenu`/extra display specials. | Extra category setup. |
| `Shop_InitCodeChars` | Builds the `atoz0to9icon` table from A-Z plus 0-9 special names. | Secret-code character icon setup. |
| `Shop_Init` | Xrefs shop strings and initializes category item arrays. | Shop asset/menu setup. |
| `BuyShopItem` | Direct LSW1 demo donor name. Marks `shopitem + 0x73`, subtracts cost, sets save bits; uses item type byte at `+0x72`. | Normal purchase/apply routine. |
| `SelectShopItem` | Direct LSW1 demo donor name. Checks item type, owned state, and cost, then calls `BuyShopItem`. | Buy/select current shop item. |
| `MoveSubItemsRight` | Direct LSW1 demo donor name. | Move to the next/right submenu item. |
| `MoveSubItemsLeft` | Direct LSW1 demo donor name. | Move to the previous/left submenu item. |
| `Shop_UpdateSubMenu` | Selects category tables from `gShopCurrentCategory`; handles category left/right and buy/back. | Non-code submenu update. |
| `Shop_UpdateMainMenu` | Opens code or non-code submenus based on selected top-level item type. | Top-level shop menu update. |
| `Shop_UpdateMenu` | Outer shop update wrapper. | Shop update dispatcher. |
| `Shop_UpdateCodes` | Mutates `gShopSecretCodeCursor`, `gShopSecretCodeBuffer`, scans `ShopCodeTable`, and unlocks matching code items. | Secret-code submenu update. |
| `DrawTopShelf` | Direct LSW1 demo donor name; renders `kamino_e` shelf/menu items. | Top shelf draw. |
| `Shop_DrawCodeBuyFeedback` | Feedback draw/update path after code accept. | Code result feedback. |
| `Shop_DrawCodes` | Reads the six code chars and draws code UI. | Secret-code submenu draw. |
| `Shop_DrawSubMenuTitle` | Chooses text IDs from `gShopCurrentCategory` and draws the submenu title. | Submenu title draw. |
| `Shop_DrawSubMenu3D` | Reads the same category arrays as `Shop_UpdateSubMenu`. | 3D/draw path for category item rings. |
| `Shop_DrawSubMenuAndTopShelf` | Calls `gShopDrawAltCallback`, then `DrawTopShelf(gShopCurrentCategory)`. | Composite non-code submenu draw wrapper. |

### Category State Layout

`Shop_UpdateSubMenu` maps `gShopCurrentCategory` to four local category
pointers:

| Category | Ring index words | Item array | Per-item cursor/position array | Count word |
| --- | --- | --- | --- | --- |
| `0` Hints | `HintItemRingIndices` (`0x80407020`) | `HintItems` | `HintCurPos` | `gShopHintItemCount` |
| `1` Characters | `CharacterItemRingIndices` (`0x80406FB0`) | `CharacterItems` | `CharacterCurPos` | `gShopCharacterItemCount` |
| `2` Extras | `ExtraItemRingIndices` (`0x80407090`) | `ExtraItems` | `ExtraCurPos` | `gShopExtraItemCount` |
| other | `NULL` | `NULL` | `NULL` | `NULL` |

The invalid-category path is at `0x80148820..0x8014882C` and clears `r30`,
`r31`, `r29`, and `r28`. The later scroll code does not guard `r28` before
loading the count word.

`Shop_UpdateMainMenu` intentionally creates invalid category values during
top-level transitions:

| Address | State change | Meaning |
| --- | --- | --- |
| `0x80148D70` | `gShopCurrentCategory = previous - 1` | Move left on top-level shop categories. |
| `0x80148D78` | clamps below `0` back to `0` | Left edge clamp. |
| `0x80148DDC` | `gShopCurrentCategory = previous + 1` | Move right on top-level shop categories. |
| `0x80148DE8` | clamps above `2` to `3` | Right edge overshoot/sentinel, invalid for `Shop_UpdateSubMenu`. |
| `0x80148E54` | `gShopCurrentCategory = -1` | Begin submenu-open transition after A/select. |
| `0x80148E94` | `gShopCurrentCategory = -1` | Begin back/close transition from the main shop path. |
| `0x8014913C` | `gShopCurrentCategory = gShopPreviousSelection` | Restore previous category when submenu-open transition completes. |

This makes the likely race precise: if `Shop_UpdateSubMenu` becomes the active
update callback while `gShopCurrentCategory` is still `-1` or `3`, it will select
null category pointers and become vulnerable to the next left/right input.

### Top-Level Transition State

`Shop_UpdateMainMenu` appears to use these globals as a small transition state
machine:

| Global | Evidence | Current interpretation |
| --- | --- | --- |
| `gShopPreviousSelection` (`0x80409BEC`) | Stores the old category before category movement/open; set to `-1` after code entry setup and after some closes. | Previous valid top-level category, or `-1` when no previous shelf should animate. |
| `gShopBackPending` (`0x80409BF0`) | Set on Y/back paths at `0x80148E9C` and `0x80148C3C`; cleared at `0x80148C80`; can make `Shop_UpdateMainMenu` return `5`. | Close/back transition pending. |
| `gShopOpenSubMenuPending` (`0x80409C10`) | Set when A/select starts submenu open at `0x80148E5C`; cleared when open finalizes at `0x80149138`. | Submenu-open transition pending. |
| `gShopSubMenuTransitionState` (`0x8040A7F4`) | Set to `1` when installing code/non-code submenu callbacks; set to `-1` during back transitions. | Direction/state of submenu slide animation. |
| `gShopMenuTransitionTimer` (`0x8040A7FC`) | Loaded from `lbl_80409BFC` when starting category/submenu transitions, then decremented each frame. | Shared transition timer. |

The A-rhythm route should focus on `gShopOpenSubMenuPending` and callback
installation. The Y/A route should focus on simultaneous `gShopBackPending` and
`gShopOpenSubMenuPending` activity.

### Callback Stack

`Shop_UpdateMenu` owns the shop callback stack. When `gShopUpdateCallback` is
zero, it initializes the top-level shop state:

| Address | Initialization |
| --- | --- |
| `0x80149348` | `gShopUpdateCallback = Shop_UpdateMainMenu` |
| `0x80149354` | `gShopDrawCallback = Shop_DrawSubMenuTitle` |
| `0x8014934C` | `gShopPreviousSelection = -1` |
| `0x8014935C` | `gShopBackPending = 0` |
| `0x80149360` | `gShopCurrentCategory = 0` |
| `0x80149364` | `gShopUpdateCallbackDepth = 0` |
| `0x80149368` | `gShopDrawCallbackDepth = 0` |

When a submenu opens, `Shop_UpdateMainMenu` pushes the current callbacks into
`gShopUpdateCallbackStack` and `gShopDrawCallbackStack`, increments the depths,
and installs a new update/draw pair:

| Selected item type | Update callback | Draw callbacks |
| --- | --- | --- |
| `2` code entry | `Shop_UpdateCodes` | `Shop_DrawCodeBuyFeedback`, `Shop_DrawCodes` |
| other non-code submenu | `Shop_UpdateSubMenu` | `Shop_DrawSubMenu3D`, `fn_8014BE90` |

`Shop_UpdateMenu` pops those callback stacks when a callback returns `6`.
`Shop_UpdateMainMenu` returns `5` when closing the shop and `7` otherwise.

### Scroll Path Certainties

`gShopSubMenuScrollDirection` is a byte at `0x8040A7E4`.

| Value | Meaning in `Shop_UpdateSubMenu` |
| --- | --- |
| `0` | No category scroll; clears `gShopScrollMoveSoundLatch`. |
| `1` | Calls `MoveSubItemsLeft` after loading count from `r28`. |
| `2` | Calls `MoveSubItemsRight` after loading count from `r28`. |

The two scroll helpers are now separated:

| Function | Evidence | Working role |
| --- | --- | --- |
| `MoveSubItemsRight` (`0x801480E4`) | Shifts the ring-index window toward lower offsets, increments the active index at `+0x18`, and wraps to `0` when it reaches the category count. | Move to the next/right submenu item. |
| `MoveSubItemsLeft` (`0x801483F0`) | Shifts the ring-index window toward higher offsets, decrements the active index at `+0x00`, and wraps to `count - 1` below zero. | Move to the previous/left submenu item. |

`gShopScrollMoveSoundLatch` is a 32-bit word at `0x80409C08`. The watched
address `0x80409C0B` lights up because it is the low byte of that word:

| Address | Effect |
| --- | --- |
| `0x80148944` | Reads `gShopScrollMoveSoundLatch`. |
| `0x80148958` | Stores `1` to `gShopScrollMoveSoundLatch`. |
| `0x80148964` | Plays `str_Shop_MenuMove`. |
| `0x80148AD8` | Stores `0` to `gShopScrollMoveSoundLatch` when not scrolling. |

This latch is useful as a timing signal, but it is not itself the crash cause.

### Secret-Code State

The code submenu is entered from `Shop_UpdateMainMenu` when the selected
top-level item has type byte `2`:

| Address | Behavior |
| --- | --- |
| `0x80149144` | Reads selected item's type byte at `shopItem + 0x72`. |
| `0x8014914C` | Compares it to `2`. |
| `0x8014917C` | Clears `gShopSecretCodeCursor` to slot `0`. |
| `0x80149184` | Points at `gShopSecretCodeBuffer`. |
| `0x80149194` | Fills six bytes with `0x41`, giving `AAAAAA`. |
| `0x801491B0..0x801491C8` | Installs code update/draw callbacks. |

`Shop_UpdateCodes` clamps both editable indices:

| State | Range / meaning |
| --- | --- |
| `gShopSecretCodeCursor` | Slot `0..5`. |
| `gShopSecretCodeCharIndex` | Character table index `0..0x23`. |
| `ShopCodeChars` | 36 selectable chars, A-Z plus digits. |
| `ShopCodeTable` | 64 accepted code entries. |

On accept, `Shop_UpdateCodes` copies the six-byte buffer to the stack,
NUL-terminates it, and compares it against `ShopCodeTable`. A valid match
unlocks a character or extra without the normal stud-cost check visible in
`SelectShopItem` / `BuyShopItem`.

The six-byte buffer is therefore useful persistent state, but current evidence
does not show an overflow through normal code-menu input.
