# Clip / portal cluster symbol recovery: evidence

Scope: `0x8001D0D8`–`0x80020400` in `config/GL5E4F/symbols.txt`, plus the two
suspect names left open by `animation_restart_evidence.md`
(`NuRndrWaterRippleUpdate`, the `NuAnimKey*` group). Symbol recovery only: no C
was added and no split, configure or build wiring changed.

## Read this first

- **Compiler.** LSW1 GC is built with SN Systems ProDG 3.9.3, not MWCC. Mac demo
  and CrashWOC sizes are used only to order and sanity-check names, never as
  codegen evidence. GC sizes differ from the Mac demo throughout (ProDG inlines
  small helpers), so size is a ranking signal at most.
- **Naming is not matching.** `report.json` is unchanged by these renames:
  1,920 matched code bytes of 1,569,532 and 22 of 4,147 matched functions, the
  same as `main` at `0787ab4`. (The 2,032 / 23 baseline quoted for `e7bbacf` was
  superseded when `numath/nurand.c` was promoted and the report moved to strict
  relocation comparison.) Per-function (unit, address, fuzzy %, size) rows are
  identical before and after.
- **Existing names were wrong, and so are callee names in generated asm.**
  `build/GL5E4F/asm` prints callees with the current symbol names, so e.g.
  `NuAnimKeyWrite` calling `NuTexLoadTextureFromDDSFile` or `NuASin` is the
  *old* wrong naming of the neighbours, not evidence. Every name below was
  derived from the function's own body plus independent signals.

## The key discovery: this is `nuportal_gen.c`

The cluster is the portal/frustum visibility module, not animation keys.
Independent evidence, none of it from the old names:

- `.rodata` holds `../nu2api.legogc/nu3d/ngc_obj/../nuportal_gen.c` at
  `0x80190974` immediately followed by the assert strings `Portal frustrum stack
  overflow.`, `Portal max frustra exceeded.` and `Overlapping portal rooms.`.
  The functions at `0x8001DCEC`, `0x8001E430` pass this path and `__LINE__`
  values `0xDD`, `0xE3`, `0xEB`, `0x20F`, `0x214`, `0x21B` to the assert helper.
- The Mac LSW1 demo has a contiguous portal module (`0x12fec0`–`0x132300`):
  `NuPortalInit`, `NuFrustrumAlloc`, `NuFrustrumCopy`, `drawPortal`,
  `drawSquare`, `drawViewFrustrum`, `drawClippedPortal`, `NuRoomContainsPoint`,
  `NuPortalWhichRoom`, `clipTestSphere`, `clipTestBox`, `transposeClipPlanes`,
  `buildFrustrum`, …, `checkPortalVisibility`, …, `clipPortal`,
  `createPortalFrustrum`, `NuPortalClipTest`, `roomRecursive`,
  `NuPortalVisibility`. The GC functions `0x8001DCEC`–`0x8001FAEC` appear in
  that same relative order (with the small helpers the GC build inlined
  missing), which is far stronger than any single size match.
- Frustum record layout, consistent across every function below: planes
  pointer at `+0x80` (16-byte `{a,b,c,d}` records), corner vectors at
  `+0x84`/`+0x90`, plane count `s16` at `+0x9C`, owning room `s16` at `+0x9E`,
  total size `0xA0`. Pool cursor/limit in `lbl_8040A328`/`lbl_8040A32C`, list
  count/array in `lbl_8040A330`/`lbl_8040A334` (max 16).

## Renames applied (16 symbol changes + 1 revert)

Confidence: **High** = body behaviour, Mac signature/argument shape and module
order all agree. **Medium** = role and order agree but the argument shape
differs slightly from the Mac signature, or the body is a debug helper with few
discriminating features.

| Address | Old name | New name | GC size (Mac size) | Evidence | Conf. |
|---|---|---|---|---|---|
| `0x8001DCEC` | `fn_8001DCEC` | `NuFrustrumAlloc` | 0x120 (0x140) | `(int n)`: bump-allocates `0xA0 + n*16` (16-aligned), stores plane array at `+0x80`, count at `+0x9C`, appends to a list capped at 16. Three asserts with the `nuportal_gen.c` path and the two frustrum strings. Called by `buildFrustrum`, `createPortalFrustrum`, `roomRecursive`. Mac `NuFrustrumAlloc(int)`. | High |
| `0x8001DE0C` | `fn_8001DE0C` | `drawPortal` | 0x1CC (0x1F0) | Gated on a debug flag (`lbl_80407C90`); walks a portal's vertex ring, projects edges and draws them, then finds the nearest plane edge. Only caller is `roomRecursive`. Order position matches Mac `drawPortal(nuportal_s*, nuroom_s*, int)`; the GC body does not use a third argument. | Medium |
| `0x8001DFD8` | `fn_8001DFD8` | `drawSquare` | 0xE8 (0x50) | `(vec*, vec*, int colour)`: builds an 8-vertex line list (four edges) from the x/y of two corners and one colour, one draw call. Sole caller is `drawClippedPortal`, as in the Mac order. | Medium |
| `0x8001E0C0` | `fn_8001E0C0` | `drawViewFrustrum` | 0x14C (0x1C0) | No arguments; same debug-flag gate; builds the four far-plane corners, scales/adds the camera origin, draws the pyramid edges. Mac `drawViewFrustrum(void)`. Caller is `NuPortalVisibility`. | Medium |
| `0x8001E20C` | `fn_8001E20C` | `drawClippedPortal` | 0x224 (0x200) | `(portal, room, min*, max*, colour)`: debug-flag gate, draws the clipped screen rectangle (via `drawSquare`) and portal edges. Sole caller `clipPortal` passes the min/max pair and colour `0xFF00`. Mac `drawClippedPortal(nuportal_s*, nuroom_s*, nuvec_s*, nuvec_s*, int)`. | Medium |
| `0x8001E430` | `fn_8001E430` | `NuPortalWhichRoom` | 0x33C (0x2A0) | `(scn, vec*)` → `s16` room index or `-1`. Loops rooms testing the point against each room's planes, resolves ties between two matching rooms with the `Overlapping portal rooms.` assert (lines `0x20F`/`0x214`/`0x21B`), and falls back to portal-side tests. Many gamecode callers. The Mac `NuRoomContainsPoint` (0xB0) is inlined here. | High |
| `0x8001E76C` | `NuAnimKeyLerp` | `clipTestSphere` | 0xE4 (0x120) | `(vec4* centre+radius, frustum*)`: loops the 16-byte plane records at `+0x80`, count `lha +0x9C`, computes `n·c + d`, rejects (0) if below `-radius`, counts planes where the sphere is fully inside, returns 0/1/2. Plus one extra global plane (`lbl_803145D0`). Mac `clipTestSphere(nuvec4_s*, NUFRUSTRUM*)`. Not a key lerp: there is no time argument and no `nuanimkey_s` access. | High |
| `0x8001E850` | `NuAnimKeyCopy` | `clipTestBox` | 0x134 (0x190) | `(min*, max*, planes*, count)`: expands min/max to 8 corners on the stack, tests all 8 against each 16-byte plane (`ctr`-8 loop), returns 0 if all 8 outside one plane, 1 if all 8*count inside, else 2. Sits between `clipTestSphere` and `transposeClipPlanes` exactly as in Mac. Argument order differs from Mac's three-pointer-plus-int signature only in pointer typing. | High |
| `0x8001E984` | `NuAnimKeyRead` | `transposeClipPlanes` | 0x1DC (0x280) | `(frustum*)`: fills a `0x80`-byte block at the frustum with 4×4 transposed plane columns (SoA layout), padding unused planes from `lbl_803145D0`, branching on count 3…7. Called by `buildFrustrum` and `createPortalFrustrum` right after the planes are written. Mac `transposeClipPlanes(NUFRUSTRUM*)`. | High |
| `0x8001EB60` | `NuAnimKeyWrite` | `buildFrustrum` | 0x2C0 (0x300) | `(vec* min, vec* max, s16 room)`: `NuFrustrumAlloc(4)`, stores the two corner vectors at `+0x84`/`+0x90` and `room` at `+0x9E`, derives four side planes from the camera FOV (`lbl_8040A320`), writes the far plane, then calls `transposeClipPlanes` and returns the frustum. Mac `buildFrustrum(nuvec_s*, nuvec_s*, short)`. | High |
| `0x8001EE20` | `NuAnimKeyBlend` | `checkPortalVisibility` | 0x184 (0x1F0) | `(portal*, frustum*, scn*, room, room)` → 0/1/2: compares the two rooms' level bytes, tests the portal plane against the camera side, then tests each portal vertex against each frustum plane. Only caller is `roomRecursive`. Mac `checkPortalVisibility(nuportal_s*, NUFRUSTRUM*, nugscn_s*, short, short, short)` has one more argument, hence Medium. | Medium |
| `0x8001EFA4` | `fn_8001EFA4` | `clipPortal` | 0x2B0 (0x150) | `(portal*, room*, s16 room, parentFrustum*, NUFRUSTRUM** out)` → 0/1/4: transforms the portal polygon by the clip matrix, min/max extents (Mac `computePortalExtents`/`intesectBoxes` inlined), intersects with the parent frustum's `+0x84`/`+0x90` bounds, calls `drawClippedPortal`, `buildFrustrum`, stores the result through `*r7`. `roomRecursive` switches on the return value. Mac `clipPortal(nuportal_s*, nuroom_s*, int, NUFRUSTRUM*, NUFRUSTRUM**)`. | High |
| `0x8001F254` | `fn_8001F254` | `createPortalFrustrum` | 0x2F4 (0x230) | `(portal*, room)` → frustum: `NuFrustrumAlloc(vertexCount)`, sets room at `+0x9E`, for each portal edge subtracts from the camera position, crosses, normalizes, writes planes (flipped when the camera is behind the portal plane), then `transposeClipPlanes`. Called when `checkPortalVisibility` returns 1. Mac `createPortalFrustrum(nuportal_s*, int)`. | High |
| `0x8001F548` | `fn_8001F548` | `roomRecursive` | 0x2BC (0x260) | `(scn, frustum*, s16 room, s16 parent, int depth)`: self-recursive; marks the room visited (`|= 0x20000000` at `+0x10`), iterates the room's portal list (`+0x8`, count byte `+0xF`), calls `checkPortalVisibility`, then `createPortalFrustrum` or `clipPortal`, and recurses with `depth + 1`; copies a frustum (Mac `NuFrustrumCopy` inlined) before the 4-result case. Mac `roomRecursive(nugscn_s*, NUFRUSTRUM*, short, short, int)`. | High |
| `0x8001F804` | `fn_8001F804` | `NuPortalVisibility` | 0x2B4 (0x420) | `(scn*)` → 0/1: sets the portal tables (`lbl_8040A330`/`34`), captures the camera and view matrices, calls `NuPortalWhichRoom`, builds the root view frustum, `drawViewFrustrum`, clears every room's visited bit, calls `roomRecursive(scn, root, room, -1, 0)`. Mac `NuPortalVisibility(nugscn_s*)`. | High |
| `0x8001FAB8` | `fn_8001FAB8` | `NuPortalInit` | 0x34 (0x40) | No arguments: points the frustum pool at the static buffer `lbl_8020D8F4`, sets the limit at `+0x1000`, aligns the cursor. Same three callers as `NuPortalVisibility`. Mac `NuPortalInit(void)` is first in its module whereas GC puts this one last, so Medium. | Medium |

### Reverted to a placeholder

| Address | Old name | New name | GC size | Why |
|---|---|---|---|---|
| `0x8001BE94` | `NuRndrWaterRippleUpdate` | `fn_8001BE94` | 0x130 | No water or ripple code. Body: clones animation data (`NuAnimData2Clone(r3, tmp@lbl_80407C38+8, idx, t - const)`), reads a per-index flag byte at `+0x34`, builds a matrix from three float→int rotation values (flag bit 1) or identity, optional scale (bit 8), `NuMtxTranslate`, then negates the x-axis components (handedness flip). Sole caller is the cutscene camera update (`instNuGCutCamSysUpdate` at `0x8001C924`). The true name is not established. Rejected: Mac `NuGCutRigidCalcMtx(NUGCUTRIGID_s*, float, numtx_s*)` (0x350) and `NuGCutLocatorCalcMtx(…, nuanimtime_s*)` (0x2C0) — argument shape (extra int index) and size do not agree; Mac `Rippl…` functions (`AddRipple`, `UpdateRippleSet`, `DrawRipple`) are gamecode and take ripple-set pointers. | — |

### Existing name corroborated, unchanged

`NuPortalClipTest` (`0x8001FAEC`, 0xC4; Mac `0x131b90`, 0xD0): `(scn*, vec*, f32
radius, s16 room)`. Reads the scene's portal block at `+0x88` (`+0x74` current
room, `+0x70` frustum count, array at scene `+0xB8`), and for each frustum whose
room (`+0x9E`) equals the argument copies `{x,y,z,radius}` to the stack and calls
`clipTestSphere`, returning the first non-zero result. Matches the Mac
signature `NuPortalClipTest(nugscn_s*, nuvec_s*, float, short)` argument for
argument. Confidence: High.

## Rejected, ambiguous or not-in-cluster (left unrenamed)

| Address | Current | Candidate(s) | Why not renamed |
|---|---|---|---|
| (no function) | — | `NuFrustrumCopy` (Mac 0xF0) | No standalone GC function. A 0x94-byte struct copy plus plane copy is inlined at `roomRecursive` `0x8001F6D0`–`0x8001F794`. |
| (no function) | — | `NuRoomContainsPoint`, `computePortalExtents`, `intesectBoxes`, `clipRoomAgainstFrustrum`, `drawInstanceBounds` | Inlined into `NuPortalWhichRoom` / `clipPortal`; no addresses to name. |
| `0x8001D0D8` | `fn_8001D0D8` | none | 0x184. One-time init (sets `lbl_80407C88`); builds two sets of plane/matrix tables from a constant extent via `sqrt` and zero-fills. Called only from `0x8001DC4C`. Role (shadow volume? culling zone?) not established. |
| `0x8001D25C`, `0x8001D4B8`, `0x8001D7EC` | `fn_` | Mac `NuStencilShadows…Silhouette` (0xDC0/0x500/0x490) | Walks a spatial tree with bit-array visibility and `NuMtxMul`; sizes and shapes (0x25C/0x334/0x460) do not line up with the Mac silhouette functions. No strings. |
| `0x8001DC4C` | `fn_8001DC4C` | none | 0xA0 dispatcher over the three above (flag at `+0x28`, count `+0x1C`); only caller `fn_8000F488`. Name would restate unverified callees. |
| `0x8001FBB0`, `0x8001FD98` | `fn_` | `NuQFnt*` | These belong to `nuqfnt.c`, not the portal module: `0x8001FD98` passes `../nuqfnt.c` with lines `0x17A`/`0x192` and `Wordbuff overflow` to the assert helper. They are out of the clip cluster; naming the font code needs its own pass (Mac `NuQFntPrintJustifiedRS` 0x370 and `…RSW` 0x4B0 do not match 0x60C directly). |
| `0x8002043C` | `fn_8002043C` | `NuQFntPrint*`-style wrapper | Tail wrapper passing `r3 = 0` to `fn_8001FD98`; named only by guessing the callee. |
| `0x800203A4` | `fn_800203A4` | `NuQFntSetSpaceWidth` (Mac 0x30) | 8-byte store of `f1` to `lbl_80407CC4`, which `fn_800203C8` uses as a replacement width when the glyph is `0x20`, so the role is plausible, but the tiny-stub rule from `animation_restart_evidence.md` applies and the Mac function takes an extra `void*`. |
| `0x800203AC` | `fn_800203AC` | `NuQFntHeight` (Mac 0x40) | `font→+0x30[0] * font→+0x14 * global`; shape only. |
| `0x800203C8` | `fn_800203C8` | `NuQFntPrintLenW`-style | String width loop over `u16` codes; Mac candidates take different arguments. |
| `0x800234A8`, `0x80023628` and `NuCamera*` at `0x800239E4`+ | — | — | Outside the requested range; not reviewed here. The `NuCamera*` names in that block were not re-derived. |

## Stale references to leave for other owners

These still use the old names and were not touched (not owned by this change):

- `AGENTS.md` quick-start example `ls1_rename.py fn_8001E76C NuAnimKeyLerp` and
  the `KEY(r3)` / `nuanimkey_s` row of its struct-evidence table were derived
  from this misidentified block. `0x8001E76C` is `clipTestSphere`.
- Generated research artifacts (`call_graph.tsv`, `call_graph_rename_suggestions.md`,
  the mac-anchor queues, `named_to_unnamed_callees.md`, `nu2_fn_matches.tsv`,
  `work/*.txt`) still print the old names. Treat names in them as
  pre-correction; they were intentionally not regenerated.
- `rejected_renames.tsv` (added on `main` by the artifact-staleness PR) does not
  yet list the old names retired here (`NuAnimKeyLerp`, `NuAnimKeyCopy`,
  `NuAnimKeyRead`, `NuAnimKeyWrite`, `NuAnimKeyBlend`, `NuRndrWaterRippleUpdate`).
  Adding them is a follow-up so the rename tools cannot reintroduce them; this
  change deliberately touches only `symbols.txt` and this document.
- `tools/ls1_rename.py`, `tools/ls1_task_pack.py` and `tools/ls1_lookup_symbol.py`
  contain `NuAnimKey*` strings only as examples/donor lookups; no code depends
  on the GC names.

## Next C-matching queue (portal block)

Smallest and best-typed first; all still unmatched, none claimed here.

| Function | Size | Needs |
|---|---|---|
| `NuPortalInit` | 0x34 | `lbl_8040A324/28/2C` globals and the pool buffer |
| `clipTestSphere` | 0xE4 | `nuvec4_s`, frustum struct (planes `+0x80`, count `+0x9C`) |
| `NuPortalClipTest` | 0xC4 | frustum struct, scene portal block |
| `clipTestBox` | 0x134 | as above |
| `NuFrustrumAlloc` | 0x120 | `NuMemAlloc`-free bump pool, assert helper prototype |

## Verification

- `bash build.sh` (local retail DOL, patched DTK): succeeds. Report is
  unchanged: **1,920 / 1,569,532 matched code bytes, 22 / 4,147 functions**,
  36 / 2,617,856 data bytes. Whole-image comparison unchanged
  (`numath/nurand.c` 284 bytes identical, 1,192 differing bytes outside the
  checked ranges).
- Per-function report rows (unit, address, fuzzy %, size) are identical to the
  pre-change report; only names differ.
- `config/GL5E4F/symbols.txt`: addresses identical to `main`; only the name
  column changed on 17 lines; no duplicate names.
- Portable checks are listed in the PR description.
