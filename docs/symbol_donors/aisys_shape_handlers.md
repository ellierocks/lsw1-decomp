# AISys antinode shape names

Naming follow-up to [PR #174](https://github.com/ellierocks767/lsw1-decomp/pull/174),
based on `c39e0e7`, requested by Ellie outside the matching plan. This changes
names only: **zero new VERIFIED or PROMOTED functions or bytes**.
Current source and symbols take precedence over historical generated queues.
`rejected_renames.tsv` contains no rejection for these three addresses.

## Accepted identities and direction

| GC address | Name | Retail behavior |
| --- | --- | --- |
| `0x80087F98` | `AISysCheckAntinode_Circle` | Tests `dx*dx + dz*dz < radius*radius`; redirects an interior movement point to the expanded circle perimeter, stepping around it toward the destination. |
| `0x800881D4` | `AISysCheckAntinode_Ellipse` | Inverse-rotates the relative point by region `+0x38`; uses semiaxes `+0x40/+0x44`, scales x by their ratio before atan, and tests against the radially expanded ellipse. An interior point becomes an expanded boundary point plus a signed tangent, rotated back to world space. |
| `0x800885CC` | `AISysCheckAntinode_Rectangle` | Inverse-rotates the movement point; tests both absolute local coordinates against half-extents `+0x40/+0x44` plus AI margin `+0x9c`. An interior point becomes an expanded edge point plus a signed edge tangent, rotated back to world space. |

All three **avoid the interior**; none contains characters inside an allowed
area. They adjust AI `+0x90..+0x98`, rather than directly writing character
position. The original destination is AI `+0x80..+0x88`, object position is
`+0x6c..+0x74`, and object `+4` points to the AI packet. Circle uses the caller's
region radius plus AI margin; ellipse/rectangle additionally use their own
semiaxes/half-extents. The shared timer `+0x140` and direction bit `+0x1a0 &
0x8000` keep the selected direction around an obstacle, with reversal logic.

Target assembly was read for the three handlers and their callers. OpenSaga
`src/legoapi/ai/core/aisysall.cpp` at pin
`7bb2402aa902d08f0979892ca23ee80e809e5ee1` supplies the same three names,
geometry, destination/boundary distinction and three-entry dispatch order.
The LSW2 Mac PPC symbols independently carry these exact names with
`APIOBJECT_s*, AIANTINODE_s*, nuvec_s*, float` parameters
(`mac_lsw2_symbols.tsv`, addresses `0x1303f8`, `0x130654`, `0x135c70`).
There is no corresponding name hit in the LSW1 demo symbol list. Donor
post-operation object flags, modern layouts and dynamic-pool sizes do not
transfer. No donor source was copied; no matching candidate was changed.

## Dispatch and loading

The retail DOL words at `lbl_801BC8B8` are exactly
`80087f98 800881d4 800885cc`: shape indices 0, 1 and 2 respectively.
A scan of every aligned word in the initialized DOL sections finds each
handler pointer only in that table. Ghidra finds the table use in
`fn_80088AAC`; its indirect call is `0x80088C68`, after loading region byte
`+0x4a` at `0x80088C50` and indexing the table at `0x80088C60`.
Ghidra reports only the first function-pointer xref, so the other two entries
were confirmed directly from retail bytes, not inferred from missing xrefs.

`fn_80088AAC` collects enabled (`+0x48`) records of stride `0x4c` from system
count/pointer `+0x1370/+0x1374`, plus 16 dynamic records at `lbl_801BC3F8`.
It rejects distant points and non-overlapping vertical ranges, then dispatches
by shape. Its two direct callers are `fn_80088CD8` at `0x800890F8` and
`fn_8008911C` at `0x80089424`, the 2D/3D creature-interaction variants.
Both are selected by `fn_80128D58` at `0x80129C78` / `0x80129C58` after
the per-character processing loop. The donor calls this outer routine
`GameAIProcess`; it remains address-named here. These are interaction-pass
helpers, not direct callees of `AISysProcessCharacter` or path-connection tests.

There is **no shape script keyword or parser registration**. Loader
`fn_80086A04` opens `levels\\%s\\%s\\%s.ai2`; for versions above 12 it
allocates the antinode records. Versions 15 and later read the rotation word,
two axis floats, then the shape byte into `+0x4a`. Earlier records retain the
zero-initialized circle type. Script loading occurs separately at the end via
`AIScriptLoadScriptTxt`. The existing `SetIgnoreAntinodes` action is a bypass:
it writes object `+0x20c` bit `0x4000`, tested by the dispatcher. It does not
register these handlers. `CirclePlayer` / `CircleOpponent` are distinct actions.

## Player reachability and Ellie's answers

Ellie reports that characters path around objects and that levels have clear
pathable areas, including rooms where they path around a camera-centered
cylinder. She also reports that separating both players far enough stops both.
These observations motivate the investigation but do not identify any particular
room's antinode record.

The outer interaction list is broader than AI script execution: characters
with object `+0x20c & 0x80000`, zero byte `+0x28f`, non-null `+0xeec` and
null `+0xef8` enter it at `0x80129954..0x80129990`. Nevertheless, the antinode
dispatcher explicitly skips any object with `+0x20c & 0x01004000` at
`0x80088BB0..0x80088BB8`. `0x01000000` is the player-control flag: the player
initialization routine `fn_800A489C` sets it for the initial player at
`0x800A4AD8` (or from per-slot control bytes at `0x800A4A4C`), and `MovePlayer`
uses it for human input / the debug lift-player gate. AI override is the
separate `0x00800000` bit set by `Action_SetAIOverrideControl`; the dispatcher
does not exempt a still-player-flagged object because of that override.

Thus AI-controlled companion characters can use the handlers, but
player-controlled characters are skipped before any shape handler runs.
This is **not the two-player separation limit**. That is a different system,
likely camera/player movement; its implementation was not pursued here.
No handler computes player-pair separation or a maximum inter-player distance.

Rejected names: containment / allowed-area handlers (wrong inequality and
outward steering), camera-cylinder handlers (no camera dependency),
player-separation handlers (player exclusion), and `Action_*` names (wrong
dispatch and ABI). Only the three functions are renamed; the table, loader,
interaction helpers and speculative shared structure types keep their current
names. No new shared-header prototype is needed for these source-only helpers.

`python3 tools/dolphin.py status` reported no Dolphin process. Nothing was
launched, stopped or driven. Dolphin request, if a specific room's cylinder
needs attribution: observe that room's loaded antinode records and the
`fn_80088AAC` dispatch against AI/player objects in an existing session.
This runtime question does not block the three supported static identities.

## Verification

Before and after the rename, all **129 emitted functions** in
`src/auto/ai/aisys/aisys.c` pass `verify_fn.py --strict-symbols --quiet` against
the built object and retail DOL. The five `static inline` helpers have no
standalone retail symbols; their callers are included in the 129 checks.
The source changes are exactly three function identifiers, with matching
symbol-map and plan-target updates. Historical matching records remain intact.

`bash build.sh`, `python3 tools/matching_progress.py`, local documentation-link
validation and `git diff --check` pass. The rebuilt DOL is byte-identical to
retail, SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`. Reported coverage
stays at 2,875 matched functions and 494,968 matched code bytes. All 733 portable
tests pass (two skipped), as do compileall, configuration generation, Dolphin
map generation and shell syntax checks. This is naming, not matching progress;
no unit promotion is attempted.
