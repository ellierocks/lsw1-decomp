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
`gx/GXFifo.c` and `gx/GXInit.c`, plus `os/OSAlarm.c`, `os/OSArena.c`,
`os/OSInterrupt.c`, `os/OSError.c`, `os/OSReset.c`, `card/CARDBlock.c`,
`card/CARDCheck.c`, `db/db.c` and `pad/Padclamp.c`, which also drop donor
definitions that retail dead-stripped, plus `dvd/dvderror.c`, `dvd/dvdidutils.c`,
`dvd/dvdFatal.c`, `dvd/dvdqueue.c`, `ai/ai.c`, `ar/ar.c`, `ar/arq.c`,
`ax/AXProf.c` and `exi/EXIUart.c`, which declare their retail `.sbss` statics
`extern` (and drop dead-stripped donor functions where retail has none):
their unverified BSS references are externalized to DTK fallback names so storage
remains in assembly fallback. CARDDir uses `lbl_8030E320` for the shared card
control block. GXMisc also references a retail call label directly and omits
two functions whose strings are outside the retained retail unit. GXFifo also
references the retail synchronization call label and externalizes its unused
`__GXCurrentBP` definition. GXInit keeps the donor's static `FifoObj` and `gxData`
because MWCC generates different code for externs; the build then replaces their
`.bss` definitions with the fallback symbols (`strip_unused.py --external-bss`),
and its small-data references use DTK names. `ax/AXAlloc.c` and `ax/AXAux.c` are
unmodified donors that likewise keep their statics; `--external-bss` also
replaces their `.sbss` definitions, so their storage stays in the fallback data.

`tools/sdk_import_unit.py` copies donor sources and donor-local headers verbatim; the
pinned donor checkout is cloned by `tools/fetch_dolsdk.sh`. Prefer unmodified donors with
file-scope statics and `external_bss=True` over editing the C.

These units use Nintendo's CodeWarrior GC/1.2.5n release flags and the retail
ProDG 3.9.3 linker. Compiler flags, per-file overrides and dead stripping follow
[adonis-singh/re4](https://github.com/adonis-singh/re4), commit
`9b76ff003081d6ece8b838453fb8310743a972c7`. The MWCC part of its
`tools/strip_unused.py` and its `tools/run_prodg_link.py` are ported to this
repository. The strip tool reads the DTK target object's retained names instead
of RE4's `sym_map.tsv`, and normalizes `.sdata2`'s write flag to the SN/DTK
read-only convention. It never supplies replacement instructions or retail bytes.

Batch 3 adds `os/OSContext.c` unchanged from the donor, plus
`si/SIBios.c`, `dvd/dvdlow.c` and `card/CARDFormat.c`. SIBios hoists local
statics, gives the alarm callback a unique name and externalizes its small BSS;
its larger BSS definitions are externalized after compilation. dvdlow uses the
retail `__OSInitAlarm` name, a unique `DVDAlarmHandler` name and the same BSS
handling. CARDFormat uses the shared card control block name and drops the two
dead-stripped synchronous wrappers. All BSS storage stays in assembly fallback.

Batch 4 adds the matrix units, OSAlloc/OSLink/OSReboot, AX, DSP and five CARD
units. Dead-stripped functions are omitted except helpers needed for inlining or
constant/string pool order; the build strips their standalone bodies. OS and DSP
BSS definitions become externs. CARD references the shared fallback control block,
permission mask and retail libc names, and callbacks receive unique names. No BSS
storage is promoted. The matrix sources retain donor assembly verbatim.

See [SDK import evidence](../../docs/sdk_import.md) for ranges, verification,
progress and remaining work.
