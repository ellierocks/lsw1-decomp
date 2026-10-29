# nurand: first source-to-build promotion

`src/numath/nurand.c` is the first unit built from C **and linked into the
relinked DOL**. This page records the evidence, how the wiring works, and what
still blocks a whole-DOL match. It makes no claim beyond the unit's own byte
ranges.

## Result

| Check | Result |
|-------|--------|
| `.text` `0x8000C0C0-0x8000C1B8` (5 functions, 248 B) | identical to retail in the relinked DOL |
| `.sdata` `0x80407B40-0x80407B64` (36 B) | identical to retail in the relinked DOL |
| Object `.text` / `.sdata` bytes vs dtk target object | identical (SHA-1 of raw section bytes) |
| `R_PPC_EMB_SDA21` relocations (5) | same offsets, type and section-relative target |
| objdiff, strictest relocation mode (`functionRelocDiffs=all`) | 5/5 functions, 36/36 data bytes |
| Relinked DOL with nurand linked from source vs all-target-objects link | byte-identical (`fde58439...`) |

Nurand is now `Matching` in `configure.py`, so objdiff reports it
`complete` (1 / 22 units). Whole-DOL SHA-1 still does **not** match retail; see
[Remaining blockers](#remaining-whole-dol-link-blockers).

## Why the old checks were not enough

- `tools/verify_fn.py` compares mnemonic order and counts. It does not see
  operands, relocation targets or data.
- objdiff's default `functionRelocDiffs=none` counts a function as matching
  even when a relocation points at the wrong symbol. Strict mode changes the
  repository baseline from 23 functions / 2,032 B to **22 functions / 1,920 B**:
  `NuVecRotateAxis` was a false match because its float constant resolves to
  `.rodata+0x4` in the local object instead of retail `lbl_8018EF34`. That is a
  real relocation difference that would show up in a link. `configure.py` now
  sets `functionRelocDiffs=all` for the report.
- Neither tool looks at the *linked* values (SDA offsets, final addresses).
  `tools/dol_compare.py` does.

## Independent verification

All commands run from the repo root after `bash build.sh`.

1. **Raw bytes and relocations of the object** (does not use objdiff or
   `verify_fn.py`):

   ```sh
   B=build/binutils/powerpc-eabi
   T=build/GL5E4F/obj/numath/nurand.o     # dtk target object
   S=build/GL5E4F/src/numath/nurand.o     # compiled from src/numath/nurand.c
   for sec in .text .sdata; do
     $B-objcopy -O binary -j $sec $T /tmp/t.bin
     $B-objcopy -O binary -j $sec $S /tmp/s.bin
     cmp /tmp/t.bin /tmp/s.bin && echo "$sec identical"
   done
   $B-readelf -r $T; $B-readelf -r $S
   ```

   Both objects have 248 `.text` bytes (zeros at relocation sites) and 36
   `.sdata` bytes, and the same five relocations:

   | `.text` offset | Use | Target |
   |----------------|-----|--------|
   | `0x00`, `0x1C` | `NuRandInt` load/store | `.sdata+0` (`lbl_80407B40`) |
   | `0x48` | `NuRandSeed` store | `.sdata+0` |
   | `0x58` | `NuRandSetSeed` `li r3, global` | `.sdata+8` (`lbl_80407B48`) |
   | `0x84` | `NuRand` `li r8, global` | `.sdata+8` |

   The relocation *names* differ (`fseed` / `global_rand` locally,
   `lbl_80407B40` / `lbl_80407B48` in the dtk object), which is why this check
   compares section-relative targets. Names are reconciled below.

2. **Final image.** Relocations are only proven once the linker has resolved
   them:

   ```sh
   ninja build/GL5E4F/main.dol
   python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
   ```

   `--linked-units` takes every unit `objdiff.json` marks `complete` (that is,
   `Matching` in `configure.py`), looks up its section ranges in
   `config/GL5E4F/splits.txt`, and requires those bytes to equal retail. Output:

   ```
   ok   numath/nurand.c: 284 bytes identical to reference (0x8000C0C0-0x8000C1B8, 0x80407B40-0x80407B64)
   whole image: 1192 differing bytes (0 inside checked ranges)
        396  sdata2-via-r2 (retail uses r13)
          2  other text word in section 0
   ```

   `build.sh` runs this and fails if a linked unit differs.

3. **Negative control.** Changing the LCG increment in `NuRandIntPtr`
   (`0x3c6ef35f` to `0x3c6ef35e`) and relinking gives
   `FAIL numath/nurand.c: 2 of 284 bytes differ, first at 0x8000C0D7`, so the
   check can fail.

Unit tests for the comparison tool are in `tests/test_dol_compare.py`.

## Wiring

- `configure.py`: `Object(Matching, "numath/nurand.c", compiler="prodg", ...)`.
  `tools/project.py` then links the compiled object in place of the dtk target
  object for this split (`add_unit`), keeping link order from dtk's
  `config.json`. Nothing else changes: all other units still link their dtk
  target objects, so the relink is the same as before except for nurand.
- `config/GL5E4F/splits.txt` is unchanged. nurand keeps `.text` 248 B and
  `.sdata` 36 B.
- `build.sh` also links the DOL and runs `dol_compare.py --linked-units`.
- `tools/asm_function_slice.py` was not needed: dtk already splits this unit.

### Source changes

The pre-existing source matched `.text` but not data: it placed `fseed` and
`global_rand` in `.sbss`, whereas retail has 36 initialised bytes in `.sdata`.
ProDG 3.9.3 puts an explicit `= 0` initialiser in `.sdata` (it has no
zero-initialised-in-bss mode for this), so the source now declares:

| Retail address | Contents | Source |
|----------------|----------|--------|
| `0x80407B40` | `0x00000000` | `fseed` |
| `0x80407B44` | `0x00001365` | `unk_80407B44` (no reference found in code) |
| `0x80407B48` | `0x00000001` | `global_rand` |
| `0x80407B4C`-`0x80407B63` | `0 0 0 0 0 1` | `unk_80407B4C`..`unk_80407B60` |

ProDG's GCC keeps unreferenced statics and emits variables in definition order.
Objects larger than 8 bytes go to `.data`, so the 24-byte tail must be several
small objects; an array would land in `.data` and fail the layout.

Two things here are **not** established by evidence:

- Whether `unk_80407B44` and the tail belong to this translation unit. The tail
  has no direct references; it is carried verbatim because the current split
  includes it, and narrowing the split would leave `lbl_80407B48`
  (`size:0x1C` in `symbols.txt`) straddling the boundary. The Mac demo has a
  4-byte `global_rand`, so the real owner of the tail is probably a neighbour.
  Fix by splitting `symbols.txt` and `splits.txt` together (the `symbols.txt`
  change belongs to the symbol-naming workstream).
- The names `unk_*` are placeholders.

### Symbol names

Other retail units reference the dtk names. `fn_8000C038` (`NuRandFloat`) uses
`lbl_80407B40`, and a link with only local statics fails with
`undefined: 'lbl_80407B40'`. Rather than editing `symbols.txt`, `nurand.c` ends with
a top-level `asm` exporting `lbl_80407B40` and `lbl_80407B48` as aliases of
`fseed` and `global_rand`. The linked ELF shows `80407b40 d fseed` and
`80407b40 D lbl_80407B40` at the same address. Delete the `asm` block once
`symbols.txt` names those two symbols.

## Remaining whole-DOL link blockers

Relinking with **every** unit using its dtk target object (no source at all)
already differs from retail in 1,192 bytes, and promoting nurand does not change
that count. `ninja` with no target therefore still stops at the
`CHECK config/GL5E4F/build.sha1` step; `build.sh` builds `main.dol` explicitly
for that reason. The differences have two causes:

1. **Small-data base register (396 words, 1,188 bytes).** Retail `sdata2` data is
   addressed from `r13` (`0x8040FA40`, `.sdata` start + `0x8000`) using
   `R_PPC_EMB_SDA21`. mwldeppc rewrites the relocation against `.sdata2` to use
   `r2` (`0x80413300`, `.sdata2` start + `0x8000`). Every one of the 396 words has the same
   opcode and destination register with `rA` 13 vs 2 and a shifted immediate, and
   both resolve to the same effective address. mwld refuses to fold the
   sections in the linker script (`Section name '.sbss' not allowed in linker
   command file`, `Multiple output section name '.sdata' not allowed`), so this
   needs a different approach: a post-link patch, relocating `.sdata2`
   references in the objects, or a linker that resolves SDA21 per retail
   convention.
2. **`__start` (2 words, 4 bytes).** Retail sets `r2 = 0x8041FA40` (`ori r2,r2,0xfa40` at
   `0x8000310C`) and clears `.sbss` up to `0x8040B2E4` (`ori r5,r5,0xb2e4` at
   `0x80003160`, the instruction `config.yml` calls out in
   `block_relocations`). The linker script defines those from its own section
   layout, giving `0x80413300` and `0x8040BD40`. Both values would need to be
   pinned to retail.

Other things that stand between a linked unit and a matching game:

- Only nurand is linked from source. Under strict relocation comparison
  `nuvec` matches 10 / 13 functions and `numtx` 5 / 6, so those units cannot be
  promoted yet; `nu_asm` and `nuanim` are still built with MWCC placeholders,
  not ProDG. `nufile` is 2 / 2 under strict comparison and is the natural next
  candidate, but its link behaviour has not been checked.
- `fn_8000C038` (reads and updates `fseed`, so it is the float generator) and
  `NuRandIntSeeded` (`0x8000C07C`) sit directly before nurand's `.text` range, use
  the same data, and are grouped with these functions in the Mac demo. They
  should join this unit once matched; its `.text` split then starts at
  `0x8000C038`.
- The tail of nurand's `.sdata` (above) needs an owner.

## Promoting the next unit

1. Compile the unit; confirm `.text`, data sections and relocations with the
   object-level commands above.
2. Make its data layout match (sections and initial values), or the link will
   shift neighbouring `.sdata`/`.data` addresses.
3. Add aliases or rename the symbols so other units' references resolve.
4. Flip `NonMatching` to `Matching` in `configure.py`.
5. `bash build.sh` must print `ok <unit>` from `dol_compare.py`, with no new
   differing bytes outside the checked ranges (compare the 1,192 count).
