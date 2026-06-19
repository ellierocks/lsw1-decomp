# Shop Menu Architecture Notes

These notes track the current islands of certainty for the LSW1 shop UI and
the glitched submenu crash. Addresses are for `GL5E4F`.

## Current Crash Model

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

## Known Entry Methods

Two independent in-game routes currently reach the glitched shop menu:

| Route | Input pattern | Working interpretation |
| --- | --- | --- |
| Submenu-entry rhythm | Tap `A` multiple times with the right rhythm while entering a submenu such as Characters. | Re-enters or confirms during the submenu open transition, likely while category/callback state is only partially updated. |
| Exit/open overlap | From the same spot, tap `Y` to exit the shop and `A` at the right timing. | Overlaps shop close/back state with submenu accept/open state. This likely involves `gShopBackPending` and/or `gShopOpenSubMenuPending`. |

These routes are important because neither requires the secret-code menu. That
pushes the root cause toward a shop transition race: the visible menu can be in
a valid place, but `gShopCurrentCategory` or the installed update callback has
fallen into an invalid intermediate state. The crash still occurs later, when a
direction input causes `Shop_UpdateSubMenu` to scroll with null category
pointers.

## Known Runtime Watches

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

## Core Shop Functions

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

## Category State Layout

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

## Top-Level Transition State

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

## Callback Stack

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

## Scroll Path Certainties

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

## Secret-Code State

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

## Advantage Hypothesis

The plausible useful behavior is not "buy an expensive item for fewer studs"
through the normal purchase path. The stronger route is:

1. Use the normal code menu to leave any desired six-character code in
   `gShopSecretCodeBuffer`.
2. Reach a glitched state that accepts or checks the code against an unintended
   visible context.
3. If `Shop_UpdateCodes` runs its valid-code path, it may unlock the resolved
   character/extra for free.

The minimum proof for a useful route is:

| Watch | Expected useful result |
| --- | --- |
| `gShopSecretCodeBuffer` | Contains a known valid six-character code before accept. |
| `gShopCodeChangedOrMatched` | Becomes `1`. |
| `gShopUnlockedCodeItem` | Becomes non-null. |
| target item `+0x73` | Becomes owned/unlocked. |
| stud balance | Stays unchanged. |

If the crash happens before these state changes, then the glitched scroll path
is just a crash, not yet an exploit path.

## Runtime Checkpoints

The useful in-game observations are minimal:

| Moment | Values worth noting |
| --- | --- |
| After the final timing input, before direction | `gShopCurrentCategory`, `gShopUpdateCallback`, `gShopBackPending`, `gShopOpenSubMenuPending`, visible label. |
| After pressing direction | Whether `gShopSubMenuScrollDirection` becomes `1` or `2`, and whether the game crashes immediately. |

If the glitched menu shows a normal label but `gShopCurrentCategory` is `-1` or
`3`, the static crash model is strongly supported. No debugger workflow is
required for the current naming/decomp work.

## Fast Rename Stack

The fastest loop for this subsystem is:

1. Use donor symbols first. Direct LSW1 demo names beat descriptive guesses:
   `BuyShopItem`, `SelectShopItem`, `MoveSubItemsRight`,
   `MoveSubItemsLeft`, `DrawTopShelf`, `kamino_e`, `GameMenu`, and
   `atoz0to9icon`.
2. Use `docs/symbol_donors/gc_data_xrefs.tsv` to find hub data with many shop
   xrefs, then name the functions that initialize or consume that data.
3. Use `python tools/ls1_lookup_symbol.py <addr>` for call graph context before
   naming a hub.
4. Use Ghidra one function at a time for branch shape and local pointer roles,
   then immediately push confirmed names back into `symbols.txt`.
5. Re-import symbols into Ghidra after each naming batch so the next generated C
   is easier to read.

Good next Ghidra targets for accelerating names:

| Target | Why |
| --- | --- |
| `Shop_UpdateMainMenu` | Owns callback install/pop state and category sentinel values. |
| `Shop_UpdateSubMenu` | Central crash path and category array selection. |
| `Shop_DrawSubMenuAndTopShelf` | Tiny wrapper that names draw-callback relationships. |
| `fn_8014ABAC` | Large remaining non-code submenu 2D/overlay draw hub. |
| `fn_8014BBE0` | Predicate for whether the current selected item should be treated as usable/owned. |

## Decomp Queue

Use raw PPC as authority and Ghidra C as reference only.

1. `Shop_UpdateSubMenu`: recover category pointer selection, scroll direction,
   latch, and buy-on-scroll behavior.
2. `MoveSubItemsLeft` and `MoveSubItemsRight`: recover ring-index
   layout and animation globals.
3. `Shop_UpdateMainMenu`: recover how category/submenu callbacks are installed.
4. `Shop_UpdateCodes`: recover code-match/unlock behavior.
5. `SelectShopItem` and `BuyShopItem`: compare normal purchase vs code unlock.
