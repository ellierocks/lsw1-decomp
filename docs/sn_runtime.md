# SN runtime (newlib libc and libgcc)

LSW1's ProDG link includes SN Systems' runtime: newlib 1.8.2 `libc` objects and
the GCC 2.95 `libgcc2` DImode helpers, plus libsn, libm and crt pieces. This page
records what is imported under `src/sn/`, where the sources come from, the flags
that reproduce retail and what is still assembly fallback.

## Provenance and licences

| Path | Source | Licence |
|---|---|---|
| `src/sn/libc/**/*.c` | newlib 1.8.2 (`newlib/libc/{ctype,stdlib,string,reent,stdio}`), unmodified | BSD-style per file; see `src/sn/COPYING.NEWLIB` |
| `src/sn/include/` | newlib 1.8.2 `newlib/libc/include`, plus gcc `ginclude` headers (`stddef.h`, `stdarg.h`, `va-ppc.h`, `stdbool.h`, `iso646.h`, `limits.h`) | newlib: as above; gcc headers: GPL-2+ with the runtime exception |
| `src/sn/libgcc/libgcc2.c` | SN Systems' GPL source release of GCC 2.95 (`NGC_GNU_SRC/NGC/gcc/libgcc2.c`), trimmed to the matched `L_*` sections | GPL-2+ with the libgcc exception |
| `src/sn/libgcc/longlong.h` | same release, unmodified | GPL-2+ **without** the exception; macro expansions land in the objects |
| `src/sn/libgcc/libgcc_config.h` | written for this project: `BITS_PER_UNIT`, `WORDS_BIG_ENDIAN`, `LONG_DOUBLE_TYPE_SIZE` and the `LIBGCC2_*` equivalents that the GCC build normally generates | project |
| `src/sn/libgcc/_*.c` | one-line wrappers: `#define L_<name>` then `#include "libgcc2.c"` | project |

Every file was taken from upstream text. The RE4 GameCube decomp
(`adonis-singh/re4`) was used only as read-only evidence for object lists, flags
and structure: its licence covers tools, config and docs, not `src/` or `include/`,
so none of its sources are copied. Nothing here ships retail bytes.

Match edits are limited to what the compiler needs: the `#define L_...` wrapper
for libgcc and the trimmed `libgcc2.c`. The newlib files are byte-for-byte
upstream. (An `fdlibm` unit would be the other permissive source; no libm unit
is imported yet.)

## Flags

`configure.py` defines `cflags_sn_libc` and `cflags_sn_libgcc`. Retail uses
`-O2 -mfast-cast -fno-common -G 1024 -DNDEBUG=1`. Two details matter:

- The production `prodg` ninja rule does not add the target defines that the
  `ngccc` driver injects before `cc1`. The runtime flags therefore pass them
  explicitly: `-D__GNUC__=2 -D__GNUC_MINOR__=95 -DPPC -D__PPC__ -D__PPC
  -D__CHAR_UNSIGNED__ -D_BIG_ENDIAN -D__BIG_ENDIAN__ -D__PTRDIFF_TYPE__=int
  -D__SIZE_TYPE__=unsigned -D_CALL_SYSV -D__SN__ -DSN_TARGET_NGC -D__NOASMPP__
  -D_LANGUAGE_C -D__LANGUAGE_C -DLANGUAGE_C`. Without `__GNUC__=2` the libgcc
  `mode(DI)` types produce the wrong sizes.
- The reent wrappers (`closer`, `fstatr`, `lseekr`, `readr`, `writer`) match only
  with `-G 1024`, which puts `errno` in small data.

## Imported units (33 objects, all promoted)

Strict `verify_fn.py --strict-symbols` and `verify_unit.py --functions` pass for
every function; the whole image stays at 0 differing bytes.

| Group | Units |
|---|---|
| ctype | `isdigit`, `isspace`, `tolower`, `toupper` |
| stdlib | `exit`, `strtol` (`_strtol_r`, `strtol`), `strtoul` (`_strtoul_r`, `strtoul`) |
| string | `memchr`, `memcmp`, `memcpy`, `memmove`, `memset`, `strcasecmp`, `strcat`, `strcmp`, `strcpy`, `strlen`, `strncmp`, `strncpy`, `strstr` |
| reent | `_close_r`, `_fstat_r`, `_lseek_r`, `_read_r`, `_write_r` |
| stdio | `vprintf` |
| libgcc | `__ashldi3`, `__ashrdi3`, `__divdi3`, `__lshrdi3`, `__moddi3`, `__udivdi3`, `__umoddi3` |

The four libgcc division objects each own a 256-byte `__clz_tab` copy in
`.sdata2` (`0x8040B308`, `0x8040B408`, `0x8040B508`, `0x8040B608`), split out of
the former `lbl_8040B300` object. The first 8 bytes at `0x8040B300` stay with the
auto-generated unit.

`vprintf` and `memmove` sit at `0x8018BD4C..0x8018BE6C`, far from the rest of the
runtime, because retail links them with the libm tail.

## Boundary evidence

`auto/game/podrace.c` had swallowed the runtime. Its `.text` now ends at
`0x801503A0`: the last podrace function is `PodraceLoadLapSettings`, the range
`0x80150398..0x801503A0` is zero padding, and `fn_801503A0` (the ppcdown code)
starts the runtime region. `PPCMfmsr` at `0x8015BEF4` is the first SDK symbol after
libgcc. The 8 bytes at `0x8015AFFC..0x8015B004` between `__divdi3` and
`__lshrdi3` are `.long 0; blr`. The newlib `exit` calls it as `_exit`, so it
keeps that name and stays in the auto-generated text. No boundary was moved to raise a score.

## Renames

- `0x80155C20` `Menu_SetupDisplay` is `memcmp`; `0x8015AA00` `UI_DrawText` is
  `__ashldi3`; `lbl_80155DE8` is `strcasecmp`. Matching bodies prove each.
  The `#define memcmp Menu_SetupDisplay` and `#define strncpy lbl_80156184`
  workarounds in the CARD and DVD sources are removed.
- libsn syscall stubs: `0x8018AC38` `_write`, `0x8018AC58` `_close`, `0x8018AC78`
  `_fstat`, `0x8018AC88` `_lseek`, `0x8018ACA8` `_read`. The former `_write` at
  `0x8018ABE8` is `fn_8018ABE8`: the `_write` stub calls it, so it is the lower
  layer, not newlib's `_write`.
- Externs the units need: `_ctype_` (`0x801B032C`, split out of `lbl_801AFF70`),
  `_impure_ptr` (`0x80409FC0`, `.sdata`), `errno` (`0x8040B2E0`, `.sbss`),
  `_exit`, and `vfprintf` (`0x80156998`).

## Not imported

| Item | Cause |
|---|---|
| libsn `dummy`, `FSasync`, `sndvd` and asm-body units | no upstream source; the console and syscall stubs are ProDG C and `_exit` is a crt word, worked as plan item `sn-tail` ([plan](strategy.md#runtime-and-sdk-routes)) |
| Debugger driver `0x8018A10C..0x8018AB8C` | CodeWarrior SDK code, not libsn; promoted from the CC0 MSSB `DebuggerDriver.c` in #120 ([plan](strategy.md#runtime-and-sdk-routes), [range](sdk_import.md#debugger-driver-range-8018a10c8018ab8c)) |
| `vfprintf`, `strtod`, `impure`, `_xeh`, `_x_exit`, `_x_pure` | do not compile or fix up yet (endianness config, `_register_malloc`, elf fixup failure) |
| `strchr`, `strncat`, `strrchr`, `atoi`, `mbtowc_r`, `qsort`, stdio (`sprintf`, `fflush`, `fopen`, ...) | compile but do not match; not investigated |
| `__negdi2`, `__cmpdi2`, `__ucmpdi2`, `__do_global_dtors`, `__main`, libm | no match yet / not wired |

## CodeWarrior runtime and debugger follow-up

Plan item `cw-runtime` was investigated from main `7db800a`. Its donor lead
needs three corrections, established by bodies rather than symbol names:

- `0x801503A0..0x80151724` is the leading SN `ppcdown` debugger assembly,
  including `EnableMetroTRKInterrupts`. MSSB's
  `src/Dolphin/TRK_MINNOW_DOLPHIN/dolphin_trk.c` implements a different debugger:
  its interrupt wrapper calls `EnableEXI2Interrupts`, whereas LSW1 manipulates
  the SN workspace and installs SN handlers. No MSSB TRK bodies were copied.
- `0x801531CC..0x801531E8` is seven SN `tealeaf` ABI bridge entries, each a
  tail branch to a retained libgcc implementation. MSSB's
  `src/Dolphin/Runtime/runtime.c` instead has full arithmetic implementations.
  Branch destinations prove the previously unnamed entries: `__mod2i` at
  `801531D8`, `__div2u` at `801531E0`, and `__mod2u` at `801531E4`.
- `0x801638A0..0x80163914` corresponds to MSSB's
  `src/Dolphin/os/__ppc_eabi_init.cpp` wrapper and constructor loop, not
  `Runtime/__init_cpp_exceptions.cpp`. The latter registers an exception
  fragment; LSW1 walks the null-terminated constructor array at `801FD050`.
  The loop is consequently renamed `__init_cpp`. The wrapper keeps its
  neutral `fn_801638A0` name because `__init_user` already names the separate
  empty `.init` entry at `80003464`.

The MSSB revision reviewed and used for the constructor loop is
`407dbcbe70f8be5d167d0b1f0a229f38e0359019` (CC0; see
[the source provenance](../src/dolphin/README.md) and
[licence](../src/dolphin/COPYING.MSSB)). RE4 was read only for object identity
and calling-convention evidence; none of its source was copied.

### Original assembly evidence

The `ppcdown` implementation is an assembly reconstruction from LSW1, emitted
with CodeWarrior `asm`/`nofralloc`, rather than a transfer from MSSB. Its
private calling convention requires assembly. This applies to the following
entries; ordinary compiler-generated C in neighbouring fallback ranges was
not transcribed.

| Entries (address suffixes; `fn_` unless named) | Evidence requiring the original assembly ABI |
|---|---|
| `801503A0`, `80150418` | Copy and recovery entry use inherited r31 and the `+0x6EC` exception guard; neither saves r31. |
| `80150428`, `EnableMetroTRKInterrupts`, `8015057C` | Explicit `lis/ori` addresses, manually managed LR calls, workspace callback offsets and nonstandard prologue/epilogue order; initialization patches other routines' instruction words. |
| `801507CC` | Assembly workspace setter uses `lis/ori` and returns its private r3/r4 values. |
| `801507E0`, `801508CC`, `801508F4` | Dispatcher and error/return entries inherit the workspace and branch among command handlers without C frames. |
| `801508F8`, `80150938`, `80150978`, `801509C8`, `80150A64`, `80150B54`, `80150B9C`, `80150C3C`, `80150D94`, `80150DE8`, `80150EAC` | Command handlers retain LR in r30, use r31 without saving it, and exchange packet fields with byte-reversed loads/stores; error paths branch into the common return entry. |
| `80150D64`, `80150D80`, `80150DBC`, `80150DD4` | Hardware helpers return private register/CR0 state consumed directly by their assembly callers. |
| `lbl_80150F04`, `80150F34` | Reset command falls through into register restoration and `rfi`; saved r31 and r1 are restored from the debugger workspace. |
| `8015100C`, `801510BC`, `801510E8` | TTY paths inherit r31, branch to the restore entry and share the interior entry at `8015110C`; that entry preserves LR in r29. |
| `8015114C`, `80151190`, `80151350`, `80151510` | Special-register and FPR/paired-single save/restore use the inherited debugger workspace; MSR/FPSCR and GQR state must be restored in the observed order. |
| Seven `tealeaf` entries | Single unconditional branches preserve the incoming CodeWarrior/libgcc EABI arguments and caller LR. |

`lbl_80150F4C` and `lbl_8015110C` identify shared interior assembly entries,
so compiled and target relocations agree even when another entry targets
an interior instruction. The reset command at `80150F04` is now an explicit
48-byte entry; five previously label-only arithmetic bridges are four-byte
functions. These symbol discoveries alone are not matching progress.

### Flags and promotion

The constructor donor strictly matches with pinned **GC/1.2.5n**, ordinary
SDK release flags plus **`-schedule off -opt nopeep`** for this unit only.
Scheduling otherwise hoists the array address into the prologue, and peephole
optimization removes the two branch-to-next-instruction chains. The donor's
GC/2.6 runtime settings do not apply. The hand-written assembly units use
the unchanged SDK flags; compiler pins, split boundaries of neighbouring
units, and verifier rules are unchanged.

| Source unit under `src/dolphin/` | Retail text range (end exclusive) | Strict entries | Promoted code bytes |
|---|---|---:|---:|
| `TRK_MINNOW_DOLPHIN/ppcdown` | `801503A0..80151724` | 33/33 | 4,996 |
| `Runtime/tealeaf` | `801531CC..801531E8` | 7/7 | 28 |
| `Runtime/__init_cpp` | `801638A0..80163914` | 2/2 | 116 |

All 42 entries pass individual `verify_fn.py NAME --unit UNIT --build
--strict-symbols` checks. All three units pass `verify_unit.py UNIT --build
--functions`; after selecting their compiled objects with `Matching`,
`bash build.sh` and `dol_compare.py --linked-units` pass with **zero differing
bytes**. Data/BSS stays with its existing owner. The complete rebuilt DOL
retains SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`.

Baseline matching was **354,044 code bytes / 2,375 functions**. Final matching
is **359,184 bytes / 2,417 functions**, an increase of **5,140 bytes / 42
entries**; matched data remains **127,502 bytes**. Of the original requested
37 functions / 5,144 bytes, **36 functions / 5,072 bytes** now strictly match
and are promoted. The additional six entries account for 68 bytes.

The report denominator changes from 1,591,228 bytes / 4,192 functions to
1,602,016 bytes / 4,200 functions. Besides the six newly explicit entries,
DTK now counts the residual fallback ranges `80151724..80153184` (6,752
bytes) and `801531E8..80154168` (3,968 bytes) as two `pad_` functions. They
remain unmatched assembly fallback. This is accounting exposed by splitting
the former auto unit, not newly decompiled code. The podrace boundary at
`801503A0` and every existing neighbouring split are retained.

### Remaining requested function

| Function | Best result | Next hypothesis |
|---|---|---|
| `__cvt_fp2unsigned` (`80153184`, 72 bytes) | ProDG C cast: **16/18 exact instructions**, **94.11% objdiff**, strict **MISMATCH**; not promoted | Retail materializes the constant's full address with `lis/addi`, then loads with `lfd 0(r9)`. Current ProDG folds the low relocation into `lfd`. Recover the SN `tealeaf` conversion source/addressing convention or investigate its original compiler revision using a separate score-only experiment. |

The candidate is `Runtime/cvt_fp2unsigned.c`, compiled with the pinned
ProDG flags and kept `NonMatching`. A C cast already reproduces the
comparison, CR combination, both conversion paths, stack layout and result
registers. Changes to scheduling, small-data threshold, TOC flags and
`-mfast-cast` did not fix the address materialization. MSSB's longer
saturating assembly conversion is rejected because LSW1 does not clamp
negative or overflowing inputs. This C-generated near-match has not been
replaced with a transcription of its retail instructions.

`config/plan.toml` is not on this main baseline; it belongs to the parent's
`t3/strategy-100` branch. Its `cw-runtime` item needs to track the new units,
retain the conversion as pending, and classify the remaining debugger
fallback as SN rather than an MSSB MetroTRK import. No plan file was copied
from that unmerged branch. Raw experiments and verification logs stay under
ignored `work/cw-runtime/`.
