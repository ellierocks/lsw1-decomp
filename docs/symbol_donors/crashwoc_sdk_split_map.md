# CrashWOC SDK Split Map

CrashWOC is the best-case donor because it shares the same platform, engine
family, and Metrowerks toolchain. Its source tree gives a concrete split
layout that can be used immediately against the LSW1 SDK islands.

## Direct File Families

| CrashWOC path family | Likely LSW1 target | Notes |
|---|---|---|
| `src/system/gc/GX*.c`, `src/system/gc/__GXInit.c`, `src/system/gc/GXFifo.c`, `src/system/gc/GXAttr.c`, `src/system/gc/GXFrameBuf.c`, `src/system/gc/GXGeometry.c`, `src/system/gc/GXLight.c`, `src/system/gc/GXMisc.c`, `src/system/gc/GXPixel.c`, `src/system/gc/GXTev.c`, `src/system/gc/GXTexture.c`, `src/system/gc/GXTransform.c` | `sdk/GX` | Matches the `0x8017CA2C-0x80182708` island. Start here. |
| `src/system/gc/OS*.c` | `sdk/OS` | Same OS family as the medium island in `sdk_islands.md`. |
| `src/system/gc/mtx.c`, `src/system/gc/mtx44.c`, `src/system/gc/vec.c` | `sdk/MTX` / math helpers | Useful for the tail after GX is carved out. |
| `src/system/gc/AX*.c` | `sdk/AX` | Likely separate island after GX/OS. |
| `src/system/gc/CARD*.c` | `sdk/CARD` | Likely separate island after GX/OS. |
| `src/system/gc/AR*.c`, `src/system/gc/arq.c` | `sdk/AR` | Likely separate island after GX/OS. |
| `src/system/gc/DSP*.c` | `sdk/DSP` | Likely separate island after GX/OS. |
| `src/system/gc/DVD*.c` | `sdk/DVD` | Likely separate island after GX/OS. |
| `src/system/gc/Padclamp.c`, `src/system/gc/pad.c` | `sdk/PAD` | Likely separate island after GX/OS. |
| `src/system/gc/vi.c` | `sdk/VI` | Likely separate island after GX/OS. |
| `src/system/gs/*.c` | `sdk/GX` wrapper layer | These are GX-side wrappers and helpers, not game logic. |
| `src/runtime/libc/*`, `src/runtime/libm/*`, `src/runtime/libgcc/*`, `src/runtime/sn/*` | runtime/compiler | Separate from SDK proper; split after the SDK islands are stable. |

## Fastest Wins

1. Split `sdk/GX` first, starting with `__GXInit.c`, `GXInit.c`, `GXFifo.c`,
   `GXMisc.c`, and `GXTexture.c`.
2. Split `sdk/OS` next, using the `OS*.c` family.
3. Split `sdk/MTX` and runtime helpers after GX is mostly carved out.
4. Leave `AX`, `CARD`, `AR`, `DSP`, `DVD`, `PAD`, and `VI` for the follow-on
   pass unless a direct anchor appears in `symbols.txt`.

## Why This Helps

The previous raw symbol-order donor proved that GX names are present, but the
source tree is the stronger signal. CrashWOC already organizes the SDK into
human-sized modules, so the first real progress comes from matching those file
families to the named islands in `docs/symbol_donors/sdk_islands.md`.
