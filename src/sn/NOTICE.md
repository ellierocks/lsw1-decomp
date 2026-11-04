# Third-party sources in `src/sn`

- `libc/` and `include/`: newlib 1.8.2, unmodified. Per-file BSD-style licences; see
  [COPYING.NEWLIB](COPYING.NEWLIB).
- `libgcc/libgcc2.c`, `libgcc/longlong.h` and the gcc `ginclude` headers in `include/`:
  GCC 2.95 from SN Systems' GPL source release, GPL-2+. `libgcc2.c` carries the libgcc
  runtime exception; `longlong.h` is plain GPL-2+.
- `libgcc/libgcc_config.h` and the `libgcc/_*.c` wrappers are project files.

Provenance, flags and the unit table: [docs/sn_runtime.md](../../docs/sn_runtime.md).
