# Startup cluster notes: `fn_8009CC00`, `fn_80038628`, and `NuInitHardware`

This note uses the direct PPC `bl`/`bla` export only. It does not assert a
runtime sequence, entrypoint, or function role.

## Direct-edge relationship

- `Menu_FullReset` directly calls `fn_8009CC00` at callsite `0x800A0870`
  (`0x4BFFC391`, target `0x8009CC00`). In `Menu_FullReset`'s static callsite
  layout this is its first exported direct callee; this is not runtime-order
  proof.
- `fn_8009CC00` directly calls `fn_80038628` at callsite `0x8009CC30`
  (`0x4BF9B9F9`, target `0x80038628`).
- The same caller directly calls `NuInitHardware` at callsite `0x8009CCD0`
  (`0x4BF9BDC9`, target `0x80038A98`).
- No direct exported edge connects `fn_80038628` to `NuInitHardware` in either
  direction.

The two calls are therefore sibling direct callees of `fn_8009CC00`. Their
callsite addresses are nearby in the caller's instruction layout, but address
layout is not temporal proof. Direct evidence does not show that
`fn_80038628` runs before, inside, or after `NuInitHardware` at runtime.

## Direct platform/Nu calls from `NuInitHardware`

The export records these directly named platform/Nu-facing callees:

- `NuDatFileOpenSize` at `0x800393EC`.
- `NuSoundInitV` at `0x800394F0`.
- `AXFXSetHooks` at `0x800395B4`.
- `NuRndrFootPrints` at `0x800395C0`.

No direct exported callee matching `OS*`, `GX*`, `PAD*`, `SI*`, `DVD*`, `VI*`,
`AI*`, `AR*`, or `NuFile*` appears in this function's direct-callee TSV. That
absence does not exclude indirect calls or calls through its unnamed callees.

## Generic direct callees of `fn_80038628` worth resolving

These are evidence-priority targets, not proposed names:

- `fn_8016C468` and `fn_801680CC`: direct calls at `0x8003865C` and
  `0x80038664`, adjacent in instruction layout to named OS/PAD calls.
- `fn_8016AD64`: direct call at `0x800386B8`, near `PADRecalibrate` in the
  caller's instruction layout.
- `fn_80038538`: direct call at `0x800386C4`; it is immediately before
  `fn_80038628` in the symbol table.
- `fn_8015F88C`: direct call at `0x800387EC` in the same function's OS-facing
  call region.
- `fn_80009998`: seven direct callsites in this function; repeated use makes
  it an efficient target for direct inspection.

## Rename status

No rename is justified. The evidence establishes shared caller context and
direct call edges only; it does not establish whether `fn_80038628` is SDK
glue, game initialization, or another platform setup routine.
