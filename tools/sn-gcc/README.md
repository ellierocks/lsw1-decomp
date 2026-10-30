# Native SN GCC (evaluation only)

Builds SN Systems' GCC 2.95.3 "SN BUILD v1.79 for Nintendo Gamecube" `cc1`
for Linux from SN's GPL source release. It never replaces
`build/compilers/ProDG/3.9.3` (the pinned v1.76 baseline) and never deletes files.

```sh
SN_GCC_SRC=<extracted NGC_GNU_SRC/NGC> tools/sn-gcc/build.sh OUT_DIR [alloc-diagnostics]
```

- `patches/linux-host.patch` is always applied (32-bit host build).
- `patches/alloc-diagnostics.patch` adds env-gated, print-only allocation traces
  (`GDBG`, `GDBGV`, `LADBG`; loop-hoisting `LOOPDBG` lines print under `GDBG`). Output is unchanged when unset.
- `patches/shipped-build-mem-flags.patch` is an experimental RE4 alias hypothesis;
  **not adopted** for LSW1.

Use a fresh `OUT_DIR` per patch list. Source provenance, results and limits are in
[docs/compiler_evaluation.md](../../docs/compiler_evaluation.md#native-sn-gcc-v179-evaluation).
Adapted from adonis-singh/re4 `tools/sn-gcc` (CC0).
