# Claude Symbol Recovery Handoff

You are continuing symbol recovery for the LSW1 GameCube decomp in `/home/ellie/Projects/lsw1-decomp`.

Goal: keep expanding named functions toward 35-45% using donor-backed evidence only. Do not force matches. Keep `.bss` conservative.

## Current Status

- Build passes with `bash build.sh`.
- Tool syntax passes with:
  `python -m py_compile tools/nu2_fn_match.py tools/mac_anchor_rename_queue.py tools/binary_mining_pipeline.py tools/sdk_anchor_rename_queue.py tools/sdk_island_analysis.py`
- Current naming stats:
  - functions: `939 / 4138` named (`22.69%`)
  - `.bss`: `11 / 733` named (`1.50%`)
  - data-ish: `803 / 11806` named (`6.80%`)
- Refreshed queues:
  - `docs/symbol_donors/nu2_fn_rename_queue.tsv`: `HIGH=0 MEDIUM=0 LOW=74`
  - `docs/symbol_donors/sdk_anchor_rename_queue.md`: `HIGH=0 MEDIUM=0 LOW=126`
  - `docs/symbol_donors/sdk_islands.md`: `HIGH=78 MEDIUM=154 REVIEW=0`

## Important Process Notes

- Do not run multiple `ls1_rename.py` writes in parallel. It can lose concurrent symbol-file edits. Sequential batches in a shell loop are OK.
- Prefer this pattern for applying names:
  `printf 'y\n' | python tools/ls1_rename.py fn_XXXXXXXX NewName`
- Run `bash build.sh` and refresh queues after each meaningful island.
- Use exact/norm body evidence and manual assembly semantics over positional queue rows when they conflict.

## Applied In This Pass

Startup / init:

- `fn_80003300 -> __init_registers`
- `fn_80003334 -> __init_fpu`
- `fn_80003468 -> __init_hardware`
- `fn_8015C008 -> __OSFPRInit`
- `fn_8015DC04 -> __OSCacheInit`
- `fn_801638A0 -> __init_user`

SDK runtime:

- `fn_8015C130 -> OSGetConsoleType`
- `fn_8015C91C -> __OSSetExceptionHandler`
- `fn_8015C938 -> __OSGetExceptionHandler`
- `fn_8016017C -> __OSSetInterruptHandler`
- `fn_80160198 -> __OSGetInterruptHandler`
- `fn_80162A3C -> __OSReschedule`
- `fn_80162A6C -> OSYieldThread`

DVD:

- `fn_80167780 -> __DVDFSInit`
- `fn_801677B8 -> DVDConvertPathToEntrynum`
- `fn_80167AAC -> DVDOpen`
- `fn_80167B74 -> DVDClose`

GX:

- `fn_8017FACC -> GXReadDrawSync`
- `fn_8017FAD8 -> GXPixModeSync`
- `fn_8017FAFC -> GXPokeAlphaMode`
- `fn_8017FFFC -> GXSetLineWidth`
- `fn_8018003C -> GXSetPointSize`
- `fn_8018007C -> GXEnableTexOffsets`
- `fn_801800C4 -> GXSetCullMode`
- `fn_801800EC -> GXSetCoPlanar`
- `fn_80180120 -> __GXSetGenMode`
- `fn_80180BB4 -> GXClearBoundingBox`
- `fn_801830C4 -> GXSetColorUpdate`
- `fn_801830F0 -> GXSetAlphaUpdate`
- `fn_8018311C -> GXSetZMode`
- `fn_80183150 -> GXSetZCompLoc`
- `fn_80183184 -> GXSetPixelFmt`
- `fn_80183258 -> GXSetDither`
- `fn_80183284 -> GXSetDstAlpha`
- `fn_801832C0 -> GXSetFieldMask`
- `fn_801832F8 -> GXSetFieldMode`
- `fn_80183728 -> GXSetViewportJitter`
- `fn_80183780 -> GXSetViewport`
- `fn_801837C8 -> GXSetScissor`
- `fn_80183840 -> GXSetScissorBoxOffset`
- `fn_80183880 -> GXSetClipMode`
- `fn_8018392C -> GXSetGPMetric`
- `fn_80184174 -> GXReadGPMetric`
- `fn_8018431C -> GXClearGPMetric`

## Do Not Force These Yet

`.init` remaining functions:

- `fn_80003254`: argv/debug block relocation helper. Behavior is clear, name is not donor-confirmed.
- `fn_800032C0`: pad/reset/debug check leading to `OSResetSystem`. Behavior is clear, name is not donor-confirmed.
- `fn_80003464`: 4-byte `blr`; too many possible aliases.

GX projection area:

- `fn_80183370` is still unnamed. The SDK positional queue suggests `GXSetProjection`, but the body only writes XF command `0x40` with two words after dirty-state checks, and it does not look like the normal projection matrix upload.
- `fn_801833E0` and `fn_80183484` look more projection-related. Inspect them before naming anything in this mini-island.
- The previous LOW queue row for viewport was shifted: semantics proved:
  - `fn_80183728 = GXSetViewportJitter`
  - `fn_80183780 = GXSetViewport`
  - `fn_801837C8 = GXSetScissor`
  - `fn_80183840 = GXSetScissorBoxOffset`

Mac/Crash MEDIUM queue:

- Leave the remaining `docs/symbol_donors/mac_anchor_rename_queue.md` MEDIUM candidates alone for now. They have large size mismatches, e.g. `NuAnimCurve2SetApplyToJoint` at `fn_80017EA0` is `0x6B8/0x3C`, and cutscene rows also look shifted.

## Best Next Targets

1. DVD waiting queue island around `0x8016A818`.
   Candidates in `sdk_anchor_rename_queue.md`:
   - `fn_8016A818 -> __DVDClearWaitingQueue`
   - `fn_8016A850 -> __DVDPushWaitingQueue`
   - `fn_8016A888 -> __DVDPopWaitingQueue`
   - `fn_8016A8F0 -> __DVDCheckWaitingQueue`
   Also body-backed:
   - `fn_8016B914 -> DVDLowWaitCoverClose`
   Inspect assembly first; this is a good next island.

2. GX poke island after `GXPokeAlphaMode`.
   Candidates:
   - `fn_8017FB10 -> GXPokeAlphaRead`
   - `fn_8017FB30 -> GXPokeAlphaUpdate`
   - `fn_8017FB48 -> GXPokeBlendMode`
   - `fn_8017FBAC -> GXPokeColorUpdate`
   - `fn_8017FBC4 -> GXPokeDstAlpha`
   - `fn_8017FBE8 -> GXPokeDither`
   - `fn_8017FC00 -> GXPokeZMode`
   Inspect the register writes; several should be straightforward.

3. SDK OS/SRAM island around `0x8016148C`.
   Candidates:
   - `fn_8016148C -> OSGetResetButtonState`
   - `fn_80161724 -> __OSInitSram`
   - `fn_80161784 -> __OSLockSram`
   - `fn_8016189C -> __OSLockSramEx`
   Gap is imperfect (`7:4`), so inspect carefully.

4. HVQM/Nemo exact body island around `0x8004AB38` and `0x80188C74`.
   These are lower architectural risk than gameplay guesses and likely useful for video subsystem naming.

## Suggested Startup Commands

```sh
git status --short
bash build.sh
python tools/binary_mining_pipeline.py nu2-body-matches sdk-anchor-queue sdk-islands
python tools/matching_progress.py
```

Then inspect the DVD waiting queue bodies:

```sh
sed -n '385900,386150p' build/GL5E4F/asm/auto_01_8001752C_text.s
rg "fn_8016A818|fn_8016A850|fn_8016A888|fn_8016A8F0|fn_8016B914" docs/symbol_donors/*.tsv docs/symbol_donors/*.md
```

Use `ls1_lookup_symbol.py` and `ls1_rename.py --check` before applying names.
