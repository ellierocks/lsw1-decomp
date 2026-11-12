# SN runtime (newlib libc and libgcc)

LSW1's ProDG link includes SN Systems' runtime: newlib 1.8.2 `libc` objects and
the GCC 2.95 `libgcc2` DImode helpers, plus libsn, libm and crt pieces. This page
records what is imported under `src/sn/`, where the sources come from, the flags
that reproduce retail and what is still assembly fallback.

## Provenance and licences

| Path | Source | Licence |
|---|---|---|
| `src/sn/libc/**/*.c`, `src/sn/libc/**/*.h` | newlib 1.8.2 (`newlib/libc/{ctype,errno,locale,reent,stdio,stdlib,string}`); upstream text except the edits marked `SN Systems:` ([SN edits](#sn-edits)) | BSD-style per file; see `src/sn/COPYING.NEWLIB` |
| `src/sn/libm/math/` | newlib 1.8.2 fdlibm: `libm/math/{s_floor,e_fmod,e_log,e_log10}.c` and `libm/common/fdlibm.h`, unmodified | SunPro's permissive notice, kept in each file |
| `src/sn/libm/common/`, `src/sn/libm/math/{sf,kf,ef}_*.c` | newlib 1.8.2 fdlibm float kernels (`libm/{common,math}`), each `.c` with a one-line provenance comment; `libm/common/fdlibm.h` adds a `#ifdef __PRODG__` block that declares the sizes of `two_over_pi[198]`, `npio2_hw[32]`, `init_jk[3]` and `PIo2[11]` | SunPro's permissive notice, kept in each file |
| `src/sn/libm/{fmod,log,log10}.c` | written for this project: the `__ieee754_*` renames, then `#include "math/e_*.c"` | project |
| `src/sn/include/` | newlib 1.8.2 `newlib/libc/include`, plus gcc `ginclude` headers (`stddef.h`, `stdarg.h`, `va-ppc.h`, `stdbool.h`, `iso646.h`, `limits.h`). `sys/reent.h` and `va-ppc.h` carry `SN Systems:` edits | newlib: as above; gcc headers: GPL-2+ with the runtime exception |
| `src/sn/libgcc/libgcc2.c` | SN Systems' GPL source release of GCC 2.95 (`NGC_GNU_SRC/NGC/gcc/libgcc2.c`), trimmed to the matched `L_*` sections, with the upstream block that restores the normal C types before `L__main` | GPL-2+ with the libgcc exception |
| `src/sn/libgcc/gbl-ctors.h` | same release, unmodified apart from CRLF line ends | GPL-2+ with the libgcc exception |
| `src/sn/libgcc/longlong.h` | same release, unmodified | GPL-2+ **without** the exception; macro expansions land in the objects |
| `src/sn/libgcc/libgcc_config.h` | written for this project: `BITS_PER_UNIT`, `WORDS_BIG_ENDIAN`, `LONG_DOUBLE_TYPE_SIZE` and the `LIBGCC2_*` equivalents that the GCC build normally generates | project |
| `src/sn/libgcc/_*.c`, `src/sn/libgcc/__main.c` | one-line wrappers: `#define L_<name>` then `#include "libgcc2.c"` | project |
| `src/sn/libsn/dummy.c` | reconstructed ProDG C console/syscall stubs (`0x8018AB8C..0x8018ACC8`) | project |

The newlib text is the `newlib-1.8.2.tar.gz` release from
`sourceware.org/pub/newlib` (sha256
`e52667ed78595dd1919f4a303f8b658b1d57b926e085e8021851b279143cf3ef`). The SN files
come from `NGC_GNU_SRC.zip` in SN's *ProDG for NGC v3.9.3 Source Code* release.
The RE4 GameCube decomp (`adonis-singh/re4`) was used only as read-only evidence
for object lists, flags and structure: its licence covers tools, config and docs,
not `src/` or `include/`, so none of its sources are copied. That includes its
SN `include/prodg/va-ppc.h`, which is marked "All Rights Reserved" and is not in
SN's GPL release. Nothing here ships retail bytes.

Match edits are limited to what the compiler needs and what retail code proves:
the `#define` wrappers, the trimmed `libgcc2.c` and the `SN Systems:` edits below.
The float libm kernels also need the `#ifdef __PRODG__` size declarations in
`src/sn/libm/common/fdlibm.h`: `cc1.exe` must know the sizes of `two_over_pi`,
`npio2_hw`, `init_jk` and `PIo2` before `make_decl_rtl` runs on their initializers,
which places them in `.sdata2` as retail has them. `ef_rem_pio2` and
`kf_rem_pio2` still address `two_over_pi` with `lis`/`addi`, not retail's `@sda21`.
Every other newlib and fdlibm file is byte-for-byte upstream. `.gitattributes` marks the upstream trees `-whitespace` so `git diff --check` leaves their trailing blanks alone.

## SN edits

SN built newlib with changes that retail code shows. Each edit is marked
`SN Systems:` in the source and is the smallest change that reproduces the
retail body:

| File | Edit | Retail evidence |
|---|---|---|
| `include/sys/reent.h` | `FILE._ubuf` is 10 bytes, not 3 | `ungetc` stores `_ubuf[9]` at `FILE+0x49`; `_nbuf` is at `FILE+0x4A` |
| `libc/stdio/local.h` | `__sinit` has no prototype | `crclr cr1eq` before each `CHECK_INIT` call in `fflush`, `__srefill` and `ungetc` |
| `libc/stdio/local.h` | `FREEUB` is a no-op | `__srefill` neither frees nor clears `_ub._base`; `malloc` and `free` are `OSPanic` stubs |
| `libc/stdio/ungetc.c` | buffer overflow writes a message with `_write` instead of calling `__submore` | `ungetc` calls `_write` with the string at `0x801B056C` |
| `libc/stdio/fread.c` | a local byte loop (`fread_copy`, `0x80159DC4`) replaces `memcpy` | `fread` calls the static copier |
| `libc/stdio/findfp.c` | `__sinit` links a static glue of ten more `FILE`s; `std` is prototyped and does not store `_bf._size`; `__sinit` comes before `_cleanup_r` | the `.bss` glue at `0x802F3704..0x802F3AD0`; retail `std` and function order. Retail strips `__sfp`, so its loop is a reconstruction |
| `libc/stdio/makebuf.c` | no `_malloc_r` path: every stream is unbuffered on its one-byte `_nbuf` | `__smakebuf` ends after the `fstat` block with the `__SNBF` store |
| `include/va-ppc.h` | `va_list.gpr` is a plain (signed) `char`; the 4-byte integral `va_arg` tests the fit unsigned and clamps `gpr` to 8 when an argument overflows | every `__svfscanf` `va_arg`: `extsb`, `addi r0,r9,1; cmplwi r0,8`, then `cmpwi r9,7` and `stb` of 8 on the overflow path |

The `va-ppc.h` path is rebuilt from those instruction sequences. SN's own
header is not in its GPL release (see above), so only what `__svfscanf`
exercises is changed: the floating, aggregate and `long long` paths stay
upstream, with `fpr` still `unsigned char`.

SN's libm is fdlibm with the `__ieee754_*` kernels exported under the plain
names: `log10` calls `log` directly, and no `w_*.c` wrapper exists in retail.
The wrappers in `src/sn/libm/` therefore define `__ieee754_fmod`, `__ieee754_log`
and `__ieee754_log10` to the plain names. `floor` is `s_floor.c` unchanged.

## Flags

`configure.py` defines `cflags_sn_libc`, `cflags_sn_libgcc`,
`cflags_sn_libgcc_main`, `cflags_sn_libsn` and `cflags_sn_libm`. Retail uses `-O2 -mfast-cast -fno-common -DNDEBUG=1`
with a per-library small-data threshold. These details matter:

- The production `prodg` ninja rule does not add the target defines that the
  `ngccc` driver injects before `cc1`. The runtime flags therefore pass them
  explicitly: `-D__GNUC__=2 -D__GNUC_MINOR__=95 -DPPC -D__PPC__ -D__PPC
  -D__CHAR_UNSIGNED__ -D_BIG_ENDIAN -D__BIG_ENDIAN__ -D__PTRDIFF_TYPE__=int
  -D__SIZE_TYPE__=unsigned -D_CALL_SYSV -D__SN__ -DSN_TARGET_NGC -D__NOASMPP__
  -D_LANGUAGE_C -D__LANGUAGE_C -DLANGUAGE_C`. Without `__GNUC__=2` the libgcc
  `mode(DI)` types produce the wrong sizes.
- newlib and libm use `-G 8` and `-DMB_CAPABLE`. The reent wrappers (`closer`,
  `fstatr`, `lseekr`, `readr`, `writer`) need `errno` in small data, which
  `-G 8` keeps; `_mbtowc_r` needs `MB_CAPABLE`.
- libgcc uses `-G 1024`, except `__main.o`. That object reads its 4-byte
  `initialized` flag with `lis/lwz` from `.bss`, not r13-relative `.sbss`, so it
  was built below `-G 4`; `cflags_sn_libgcc_main` uses `-G 0`, which emits it
  with `.lcomm` as retail has it.
- `libsn/dummy.c` uses `cflags_sn_libsn` (`cflags_sn_libc` with `-G 0`): the
  static UART init flag at `.data:0x801FD048` is addressed with `@ha`/`@l`, not
  small data.
- The fdlibm float kernels (`sf_*`, `kf_*`, `ef_*`) use `cflags_sn_libm`: `-O2
  -msafe-sda -G 1024 -fno-builtin -mstrict-align -D__PRODG__`, without
  `-mfast-cast`, so the `int`-to-`float` conversions in `ef_rem_pio2` and
  `kf_rem_pio2` use the IEEE `xoris`/`stw`/`lfd`/`fsubs` sequence and pool their
  literals in `.sdata`. The double-precision objects (`floor`, `fmod`, `log`,
  `log10`) keep `cflags_sn_libc`.

`SnObject(path, cflags, strip=False)` wraps each entry. `strip=True` runs
`tools/strip_unused.py` as a post-build step: it keeps only the functions the
unit's split target has, reproducing the SN linker's dead-stripping
(`_sprintf_r`, `_setlocale_r`, `__sfp`, `__do_global_dtors`, ...). For ProDG
objects it also:

- drops a local static whose references all come from removed functions, or
  that nothing references (`lc_ctype` in locale; fdlibm's named constants,
  whose code loads `.LC` literals instead);
- cuts dead data as a whole object, with no 8-byte granule: retail keeps none
  of `__main.o`'s 4-byte `__do_global_dtors` pointer. MWCC objects keep the
  reference's 8-byte granules;
- re-encodes an assembler-resolved branch (see below) that crosses a removed
  function, and fails if one lands inside a removed function.

GNU as resolves a `bl` or tail `b` to a static function in the same section and
emits no relocation, while DTK's split target has an `R_PPC_REL24`. Step 6 of
`tools/prodg_cc.py` runs `tools/prodg_branch_relocs.py`, which adds that
relocation (addend 0, against the destination function) to every ProDG object,
so objdiff and `verify_unit.py` see what the target has (`__sinit` calling
`std`, `fread` calling `fread_copy`). Loops back to a function's own first
instruction are left alone.

## Imported units (55 objects, all promoted)

Strict `verify_fn.py --strict-symbols` and `verify_unit.py --functions` pass for
every promoted unit; the whole image stays at 0 differing bytes.

| Group | Units |
|---|---|
| ctype | `isdigit`, `isspace`, `tolower`, `toupper` |
| errno, locale | `errno` (`__errno`), `locale` (`_localeconv_r`, `localeconv`) |
| stdlib | `atof`, `exit`, `mbtowc_r`, `rand` (`srand`, `rand`), `strtol` (`_strtol_r`, `strtol`), `strtoul` (`_strtoul_r`, `strtoul`) |
| string | `memchr`, `memcmp`, `memcpy`, `memmove`, `memset`, `strcasecmp`, `strcat`, `strcmp`, `strcpy`, `strlen`, `strncmp`, `strncpy`, `strstr` |
| reent | `_close_r`, `_fstat_r`, `_lseek_r`, `_read_r`, `_write_r` |
| stdio | `fflush`, `findfp` (`std`, `__sinit`, `_cleanup_r`), `fread` (`fread_copy`, `fread`), `fwalk`, `makebuf`, `refill` (`lflush`, `__srefill`), `sprintf`, `sscanf` (`eofread`, `sscanf`), `stdio` (`__sread`, `__swrite`, `__sseek`, `__sclose`), `ungetc`, `vfscanf` (`__svfscanf`, `__sccl`), `vprintf`, `vsprintf` |
| libm | `floor`, `fmod`, `log`, `log10`; float: `sf_fabs` (`fabsf`), `sf_copysign` (`copysignf`). `NonMatching`: `sf_cos` (`cosf`), `kf_cos` (`__kernel_cosf`), `kf_sin` (`__kernel_sinf`), `sf_floor` (`floorf`) and `sf_scalbn` (`scalbnf`) strictly match but have no split range for their `.sdata2` literals yet; `ef_rem_pio2` (`__ieee754_rem_pio2f`) and `kf_rem_pio2` (`__kernel_rem_pio2f`) are at 200/212 exact instructions because ours forms `two_over_pi`'s address with `lis`/`addi` where retail uses `@sda21` |
| libsn | `dummy` (`fn_8018AB8C`, `fn_8018ABE8`, `_write`, `_close`, `_fstat`, `_lseek`, `_read`) |
| libgcc | `__main` (`__do_global_ctors`, `__main`), `__ashldi3`, `__ashrdi3`, `__divdi3`, `__lshrdi3`, `__moddi3`, `__udivdi3`, `__umoddi3` |

`sprintf`, `vsprintf`, `findfp`, `__main` and the libm objects build with
`strip=True`.

The four libgcc division objects each own a 256-byte `__clz_tab` copy in
`.sdata2` (`0x8040B308`, `0x8040B408`, `0x8040B508`, `0x8040B608`), split out of
the former `lbl_8040B300` object. The first 8 bytes at `0x8040B300` stay with the
auto-generated unit.

The seven `NonMatching` `libm` units define file-scope `static const float`
scalars (`one`, `C1..C6`, `half`, `S1..S6`, `zero`, `two8`, `invpio2`, `pio2_*`,
`huge`, `two25`, `twom25`, `tiny`) that `cc1.exe` folds into `.sdata` literal pools
while also emitting unreferenced `.sdata2` symbols that the retail SN linker
stripped; `verify_unit.py` therefore keeps those seven units `NonMatching` while
`sf_fabs` and `sf_copysign` (which have only `.text`) are promoted.

`vprintf` and `memmove` sit at `0x8018BD4C..0x8018BE6C`, far from the rest of the
runtime, because retail links them with the libm tail.

Data owned by the new units, split out of larger auto symbols:

| Unit | Data |
|---|---|
| `vfscanf` | `basefix.3` at `.rodata:0x801AFF98` (34 bytes), split from `lbl_801AFF78`, which keeps the `__cvt_fp2unsigned` double (`0x801AFF78`) and is followed by a DVD message string (`str_801AFF80`) |
| `mbtowc_r` | strings at `.rodata:0x801AFFBC..0x801AFFD4`, JIS tables at `.data:0x801E1C88..0x801E1FE8` |
| `locale` | `lconv` at `.rodata:0x801B0430`; only `__mb_cur_max` (`0x80409FBC`) is in `.sdata` |
| libm | literal pools at `.rodata:0x801B0488..0x801B0568`, one per object; fmod's is the named `Zero` |
| `ungetc`, `findfp` | messages at `.rodata:0x801B056C..0x801B05E0`; `findfp`'s glue `FILE`s at `.bss:0x802F3704..0x802F3AD0` |
| `__main` | `initialized.10` at `.bss:0x802F3AD0`; retail folds `.ctors`/`.dtors` into the start of `.rodata`, named `__CTOR_LIST__` (`0x8018CB00`, `{-1, 0}`) and `__DTOR_LIST__` (`0x8018CB20`) |

## Boundary evidence

`auto/game/podrace.c` had swallowed the runtime. Its `.text` now ends at
`0x801503A0`: the last podrace function is `PodraceLoadLapSettings`, the range
`0x80150398..0x801503A0` is zero padding, and `fn_801503A0` (the ppcdown code)
starts the runtime region. Its provisional `.rodata` range ended at `0x801B0460`
only because podrace `.text` once covered the runtime; its last literal ends at
`0x801AFF78`, which is now the range end. `PPCMfmsr` at `0x8015BEF4` is the first SDK symbol after
libgcc. The 8 bytes at `0x8015AFFC..0x8015B004` between `__divdi3` and
`__lshrdi3` are `.long 0; blr`. The newlib `exit` calls it as `_exit`, so it
keeps that name and stays in the auto-generated text. The newlib pad blocks
(`0x801541B4..0x8015A938`) were split only at function starts that retail
bodies prove. No boundary was moved to raise a score.

## Renames

- `0x80155C20` `Menu_SetupDisplay` is `memcmp`; `0x8015AA00` `UI_DrawText` is
  `__ashldi3`; `lbl_80155DE8` is `strcasecmp`. Matching bodies prove each.
  The `#define memcmp Menu_SetupDisplay` and `#define strncpy lbl_80156184`
  workarounds in the CARD and DVD sources are removed.
- Pad-block labels named by matching bodies: `lbl_8015525C` `vsprintf`,
  `lbl_80155678` `srand`, `lbl_801556A8` `malloc`, `lbl_801556E4` `free` (game
  callers in `levels.c`, `nufilepak.c` and `numem.c` now use these names),
  `lbl_80156A70` `_vfprintf_r`, `lbl_80157F88` `_vfiprintf_r` and
  `lbl_8015A9C8` `__main`. `fread_copy` names SN's static copier, which has no
  upstream name.
- libsn syscall stubs: `0x8018AC38` `_write`, `0x8018AC58` `_close`, `0x8018AC78`
  `_fstat`, `0x8018AC88` `_lseek`, `0x8018ACA8` `_read`. The former `_write` at
  `0x8018ABE8` is `fn_8018ABE8`: the `_write` stub calls it, so it is the lower
  layer, not newlib's `_write`.
- newlib float libm (`0x8018ACC8..0x8018BD4C`): `0x8018ADA4` is `__kernel_cosf`,
  `0x8018AE90` `__kernel_sinf`, `0x8018AF3C` `__ieee754_rem_pio2f`, `0x8018B2B0`
  `__kernel_rem_pio2f`, `0x8018BB34` `floorf` and `0x8018BBFC` `scalbnf`,
  named from the matching fdlibm bodies (formerly `fn_` labels). In `.sdata2`, `0x8040B91C` is `two_over_pi`,
  `0x8040BC34` `npio2_hw`, `0x8040BCB4` `init_jk` and `0x8040BCC0` `PIo2`.
- Externs the units need: `_ctype_` (`0x801B032C`, split out of `lbl_801AFF70`),
  `_impure_ptr` (`0x80409FC0`, `.sdata`), `errno` (`0x8040B2E0`, `.sbss`),
  `_exit`, and `vfprintf` (`0x80156998`).

## Not imported

| Item | Cause |
|---|---|
| libsn `FSasync`, `sndvd` and asm-body units (`pad_01_801531E8_text`, `pad_01_8018CAFC_text`, `_exit`) | no upstream source; `sn/libsn/dummy.c` (`0x8018AB8C..0x8018ACC8`) is promoted, while `pad_01_801531E8_text`, `pad_01_8018CAFC_text` and `_exit` remain in plan item `sn-tail` ([plan](strategy.md#runtime-and-sdk-routes)) |
| Debugger driver `0x8018A10C..0x8018AB8C` | CodeWarrior SDK code, not libsn; promoted from the CC0 MSSB `DebuggerDriver.c` in #120 ([plan](strategy.md#runtime-and-sdk-routes), [range](sdk_import.md#debugger-driver-range-8018a10c8018ab8c)) |
| `malloc`, `free`, `strtod` (`0x801556A8..0x80155A50`) | SN-written, no upstream source: `malloc` and `free` are `OSPanic` stubs that also pass their call site (`mflr; subi 4`); `strtod` is a short `isspace`/`isdigit` scanner, not newlib's Bigint `_strtod_r`. Hand-match |
| vfprintf block (`0x801562B8..0x8015927C`: five statics, `vfprintf`, `_vfprintf_r`, a static, `_vfiprintf_r`) | SN rewrite of newlib's `vfprintf.c`/`vfiprintf.c`: output goes through a static that calls `_write` (no `__sprint`/`__sfvwrite`), the float `cvt` is replaced by code calling `floor`, `fmod` and `log10` (no `_dtoa_r`), and `vfprintf` dispatches to `_vfiprintf_r` or `_vfprintf_r`. Hand-match, now with SN's `va_arg` |
| `impure`, `_xeh`, `_x_exit`, `_x_pure` | do not compile or fix up yet (endianness config, `_register_malloc`, elf fixup failure) |
| `strchr`, `strncat`, `strrchr`, `atoi`, `qsort`, other stdio (`fopen`, ...) | compile but do not match; not investigated |
| `__negdi2`, `__cmpdi2`, `__ucmpdi2` | no match yet / not wired |

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

### Exception vectors and transport follow-up

The second `cw-runtime` pass reconstructs the complete residual
`80151724..80153184` range as
[`TRK_MINNOW_DOLPHIN/exception.c`](../src/dolphin/TRK_MINNOW_DOLPHIN/exception.c).
Its **54 entries / 6,752 bytes are promoted**. This is independently reconstructed
SN assembly, using the same pinned GC/1.2.5n `asm`/`nofralloc` mechanism as
`ppcdown`; no donor bodies, compiler overrides or verifier changes are involved.
The new unit exactly occupies the old auto range. The preceding `ppcdown` end
and following conversion start remain `80151724` and `80153184`.

The installer at `80151AFC` proves the two vector-template boundaries: its
preincrement copies start at `80151724` (0x54 bytes) and `80151778` (0xB0 bytes).
The templates finish their executable paths with `rfi` at `80151764` and
`80151814`; their following four zero words apiece are embedded template data,
retained with `opword 0`. The two zero words at `80152408..80152410` also stay
inside their assembly range. They are not fabricated executable functions.

Entry names below retain the existing `lbl_` spelling where one existed;
otherwise they are `fn_` plus the address. Sizes run to the next listed entry,
except that the twelve patch slots are individually eight bytes.

| Entry address(es) | Boundary and original-assembly evidence |
|---|---|
| `80151724` | Generic vector copied as 0x54 bytes; saves r31, SRR0/1 and LR at absolute low-memory offsets `0xDE8..0xDF8`, enables translation through SRR1 and transfers to `80151828` using `rfi`. |
| `80151778` | Program vector copied as 0xB0 bytes; preserves CR/XER in SPRG0/1, tests the exception cause and either branches to the saved OS vector in BSS or reaches the same SRR0/1 transfer. |
| `80151828` | SRR0 destination of both templates; saves GPRs through inherited r31 without a C frame, replaces r1/r2/r13 and joins command dispatch. |
| `80151AFC` | Called by `ppcdown`; installs vector instruction words, copies both exact templates, backs up the OS vector, and patches its branch displacement. Error paths read inherited r31; manually ordered eight-byte frame. |
| `80151C90` | Separate entry after the installer's `blr`; saves r31 in SPRG1, supplies exception number 4 in SPRG2 and branches to the common GPR save at `80151D1C`. |
| `80151CA8` | Address installed as the data-storage vector branch target; saves r31/SRR0 in SPRGs and `rfi`s at `80151CC0` to `80151CC4`. |
| `80151CC4` | SRR0 continuation; tests the `ppcdown` recovery guard, restores CR and either `rfi`s at `80151CF8` to `80150418` or sets exception number 3 before the common save. Ends with a branch to `801518FC`. |
| `80151DD4` | Separate leaf after that dispatch branch; returns the workspace's transport selector using the assembly unit's explicit `lis/ori` address convention. |
| `80151DE4` | Called by `ppcdown`; restores FPSCR, paired singles and all FPRs, including ABI-preserved registers, without a compiler frame. Ends at `80151F08`'s `blr`. |
| `80151F0C` | Paired save entry called by `ppcdown`; writes all FPRs and FPSCR, tests HID2 and saves paired singles. Ends at `80152034`'s `blr`. |
| `80152038` | `bl` target of the instruction decoder; consumes r4/r10 and returns its mask in private r12/r0 state, with no C ABI return convention. |
| `80152070` | Called by exception dispatch; decodes the faulting instruction against saved registers, retains LR in unsaved r30, and reads the inherited workspace. Two return paths share that private LR; eight trailing zero bytes are retained. |
| `80152410` | Direct exception-dispatch branch target; packet loop uses inherited r31, invokes callbacks through workspace offsets, and enters command handlers through a table. |
| `80152524` | Command-table target; acknowledges the packet then joins `80152544` to update saved r3/PC and tail-branch to `ppcdown` register restoration. |
| `80152578` | Command-table target; sets the step flag and branches into the packet loop at `8015247C`, without a prologue or return. |
| `8015258C` | Command-table target; sets the same flag, puts -1 in private r3 and branches into `80152544`. |
| `801525A8` | Branch target for local syscall commands; retains the command in unsaved r30, calls the local helpers, updates the saved context and branches to register restoration. |
| `80152610` | Called by `ppcdown`; overwrites the twelve following branch instructions with trap words `0x10..0x1B`. It returns directly at `801526D0`. |
| `801526D4` | Common `bl`/tail-branch destination of those slots; explicitly ordered LR/frame saves call `OSReport` and return -1. |
| `80152700`, `80152708`, `80152710`, `80152718`, `80152720`, `80152728`, `80152730`, `80152738`, `80152740`, `80152748`, `80152750`, `80152758` | Twelve independently address-taken eight-byte patch slots. Each initially contains a branch to `801526D4` and an unreachable `blr`; the writer at `80152610` proves that neither instruction may be discarded or merged. |
| `80152760` | `ppcdown` transport initialization callback; receives two wire words in r5/r6, keeps them across later helper calls and validates the `SNDL` signature. Explicit `lis/ori` pairs and manually ordered frame match the surrounding assembly. |
| `801528B4` | `ppcdown` interrupt-install callback; installs `80152CE0`, masks/unmasks the observed EXI interrupt, and returns before the next callback. Uses the same explicit address pairs and frame order. |
| `801528FC` | `ppcdown` query callback; leaves the wire word in private r5 across the deselect helper and only then moves it to r3. A normal opaque C call could clobber that live volatile register. |
| `80152984`, `80152B40` | Separate receive/send callbacks installed by `ppcdown`, each ending in its own `blr`. DMA loops retain private r5 byte counts across the completion helper; send also has an explicit CTR delay loop. |
| `80152CD8`, `80152CDC` | Two distinct address-taken `ppcdown` callbacks, each exactly one `blr`; kept as separate entries in the assembly transport callback family. |
| `80152CE0` | Interrupt-handler address used by `801528B4`; acknowledges the hardware and invokes the stored callback after setting its pending byte. Explicit address pairs and eight-byte frame preserve the assembly helper layout. |
| `80152D34`, `80152D4C`, `80152D60` | `bl` targets for EXI select, deselect and handshake; private helpers with explicit hardware stores. The handshake returns r5 state and uses an exact six-iteration CTR delay. Deselect preserves callers' live r5/r6. |
| `80152D88` | `bl` target for DMA completion; only clobbers r3/r4, preserving callers' private r5 count, and polls the EXI transfer bit until clear. |
| `80152D9C`, `80152DDC`, `80152E10`, `80152E4C` | Separate byte/word read/write `bl` targets, bounded by their `blr`s; manual eight-byte frames retain LR through r4 and call `80152D88`. Their restricted clobbers preserve r5/r6 for the initialization/query callers. |
| `80152E7C`, `80152ECC`, `80152F3C`, `80152FA4`, `80153008`, `80153078`, `801530E0` | Seven distinct transport operations, bounded by their returns at `80152EC8`, `80152F38`, `80152FA0`, `80153004`, `80153074`, `801530DC`, `80153180`. They share manual `stmw/lmw r28` frames, including unused saves, and call the same restricted-clobber handshake/read/write helpers. The last operation retains the fifteen-iteration CTR delay. |

Shared interior entries remain labels, not extra functions:
`8015175C` and `8015180C` capture the relocated vector PC through `bl`/LR;
`801517D8` restores the program-vector CR/XER; `801518FC` and `80151D1C`
join context-save paths; `8015247C` and `80152544` join packet/command paths.
CodeWarrior `entry` exports these labels. Local branches to the same instruction
use local labels where the target has no relocation, preserving both the complete
section relocation set and each strict function proof. The original instruction
at `80152784` is `ori r0,r0,...`, despite the disassembler's `nop` alias; its
nonzero low half and data relocation must be retained.

The installer copies the original 0x104-byte OS vector range to `802F3368..802F346C`;
`801517D4` branches there. `symbols.txt` now identifies that instruction buffer
inside the old workspace symbol, shortening the preceding symbol at the exact
buffer start. The BSS split, owner, addresses and storage remain unchanged; no
BSS is promoted. Three external branch destinations in the following fallback
range (`801532DC`, `80153458`, `801538A0`) gain labels for relocation proof;
that neighbouring text range is unchanged and unpromoted.

Every entry passes its individual
`python3 tools/verify_fn.py NAME --build --strict-symbols` check. The exception
unit passes `verify_unit.py dolphin/TRK_MINNOW_DOLPHIN/exception --build
--functions` (**54/54**, complete `.text` and relocations). The preceding
`ppcdown` unit still passes **33/33** and its complete-unit proof.
After selecting the exception object as `Matching`, `bash build.sh` and
`dol_compare.py --linked-units` pass with **zero differing bytes**, including
all 6,752 newly promoted bytes. The rebuilt DOL retains SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`.

The baseline for this pass is **365,384 matched code bytes / 2,452 functions**;
the result is **372,136 / 2,506**, a gain of **6,752 bytes / 54 functions**.
After rebasing onto main `cce8790`, including the nufile and HVQM4 work, the report is
**381,752 bytes / 2,526 functions**; this change still contributes only
6,752 bytes / 54 entries. Matched data stays **128,546 bytes**. Replacing the old single pad function
with 54 discovered entries changes the function denominator from 4,201 to
4,254; the code-byte denominator remains 1,602,028. Discovery itself is not
credited: the gain is the compiled unit that passed both promotion gates.

### Conversion plateau after the exception follow-up

`__cvt_fp2unsigned` remains **NonMatching**, at **16/18 exact instructions**
(**94.11% objdiff**, strict **MISMATCH**). This pass rechecked the existing cast
and bounded the additional search to three C addressing forms: an explicit
external bound, a volatile constant load, and a volatile pointer to the constant.
They scored 9/18, 9/18 and 6/18 respectively. None recovers retail's
`lis/addi; lfd 0(r9)` while retaining the existing instruction sequence.
The volatile-pointer form materializes an address but adds pointer storage and
changes the stack frame. The best source remains unchanged.

The C cast reproduces the conversion paths, CR combination, stack layout and
register choices, which is strong compiler-generated evidence. The neighbouring
assembly bridges and the extra `addi` do not establish an assembly origin for
this body. It is therefore parked in `config/plan.toml`, with the next hypothesis
to recover the SN `tealeaf` conversion source/addressing convention or try its
original compiler revision in a score-only experiment. Compiler pins and the
strict oracle are unchanged. Plan revision 6 names the exception source unit
and records this plateau, closing `cw-runtime` with one parked function.

`sprint_finish.py` passes with 54 verified entries, one parked conversion and
zero deferred entries. The bench records 706.5 active seconds (11.8 minutes),
**275.2 verified entries/hour / 34,405 verified bytes/hour**. The complete test
suite passes **619 tests, two skipped**. `plan.py --no-remote` reports no revision
warnings.

Raw reconstruction, score and verification logs stay under ignored
`work/cw-runtime/`; no Dolphin session or sweep was run.
