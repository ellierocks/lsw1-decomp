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
| Debugger driver `0x8018A10C..0x8018AB8C` | CodeWarrior SDK code, not libsn; the CC0 MSSB `DebuggerDriver.c` is the lead ([plan](strategy.md#runtime-and-sdk-routes), [range](sdk_import.md#debugger-driver-range-8018a10c8018ab8c)) |
| `vfprintf`, `strtod`, `impure`, `_xeh`, `_x_exit`, `_x_pure` | do not compile or fix up yet (endianness config, `_register_malloc`, elf fixup failure) |
| `strchr`, `strncat`, `strrchr`, `atoi`, `mbtowc_r`, `qsort`, stdio (`sprintf`, `fflush`, `fopen`, ...) | compile but do not match; not investigated |
| `__negdi2`, `__cmpdi2`, `__ucmpdi2`, `__do_global_dtors`, `__main`, libm | no match yet / not wired |
