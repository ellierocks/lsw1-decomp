# Dolphin SDK sources

The C sources and private headers here, and the public headers in
`include/dolphin/` and `include/dolphin.h`, are vendored from
[doldecomp/dolsdk2004](https://github.com/doldecomp/dolsdk2004), commit
`2328b4164b1a98422a2255d83ce9a5a7548990cc` (20 April 2004 SDK reconstruction).
The upstream repository has no LICENSE file at that commit. Vendoring was
approved by the repository owner; no license grant is inferred here.

The public Dolphin header tree is retained for subsequent library imports.
The upstream libc shim headers, `cmath.h` and `fake_tgmath.h` live under
`include/dolphin/` so SDK compilation can use their original include paths
without changing the game's headers. Promoted translation units are vendored
under `src/dolphin/`. Their C and private headers are unchanged except for
`si/SISamplingRate.c`, `dvd/fstload.c`, `card/CARDDir.c`, `gx/GXMisc.c` and
`gx/GXFifo.c`:
their unverified BSS references are externalized to DTK fallback names so storage
remains in assembly fallback. CARDDir uses `lbl_8030E320` for the shared card
control block. GXMisc also references a retail call label directly and omits
two functions whose strings are outside the retained retail unit. GXFifo also
references the retail synchronization call label and externalizes its unused
`__GXCurrentBP` definition.

These units use Nintendo's CodeWarrior GC/1.2.5n release flags and the retail
ProDG 3.9.3 linker. Compiler flags, per-file overrides and dead stripping follow
[adonis-singh/re4](https://github.com/adonis-singh/re4), commit
`9b76ff003081d6ece8b838453fb8310743a972c7`. The MWCC part of its
`tools/strip_unused.py` and its `tools/run_prodg_link.py` are ported to this
repository. The strip tool reads the DTK target object's retained names instead
of RE4's `sym_map.tsv`, and normalizes `.sdata2`'s write flag to the SN/DTK
read-only convention. It never supplies replacement instructions or retail bytes.

See [SDK import evidence](../../docs/sdk_import.md) for ranges, verification,
progress and remaining work.
