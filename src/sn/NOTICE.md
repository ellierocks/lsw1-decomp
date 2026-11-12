# Third-party sources in `src/sn`

- `libc/`, `libm/` and `include/`: newlib 1.8.2 (`libm` from SunPro `fdlibm` in newlib 1.8.2; `libm/common/fdlibm.h` adds a `#ifdef __PRODG__` block with explicit array-size forward declarations for the four `.sdata2` tables). Per-file BSD-style and SunPro licences; see [COPYING.NEWLIB](COPYING.NEWLIB).
- `libgcc/libgcc2.c`, `libgcc/longlong.h` and the gcc `ginclude` headers in `include/`:
  GCC 2.95 from SN Systems' GPL source release, GPL-2+. `libgcc2.c` carries the libgcc
  runtime exception; `longlong.h` is plain GPL-2+.
- `libc/stdlib/sn_stdlib.c` and `libc/stdio/sn_vfprintf.c` are project reconstructions.
  `libc/stdio/sn_vfprintf_core.h` adapts the newlib 1.8.2 BSD parser with reconstructed
  SN output/float paths; its original licence is retained in the file.
- `libgcc/libgcc_config.h`, the `libgcc/_*.c` wrappers and `libsn/dummy.c` are project files.

Provenance, flags and the unit table: [docs/sn_runtime.md](../../docs/sn_runtime.md).
