# Dolphin SDK sources

The HVQM4 candidate also adapts `src/hvqm4dec/hvqm4dec.c` and `include/hvqm4.h`
from [projectPiki/pikmin](https://github.com/projectPiki/pikmin), commit
`9a2163626a269ae7c699fc195cf91a5ca791044d`, licensed **CC0-1.0**
([license](https://github.com/projectPiki/pikmin/blob/9a2163626a269ae7c699fc195cf91a5ca791044d/LICENSE.MD)).
Its bodies, inline helpers and types are checked against GL5E4F independently;
Pikmin's Matching status does not establish an LSW1 match. See the
[HVQM4 evidence](../../docs/sdk_import.md#hvqm4-decoder-parked).

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
remains in assembly fallback. CARDDir and the other card files use `__CARDBlock` for
the shared card control block. GXMisc also references `PPCSync` directly and omits
two functions whose strings are outside the retained retail unit. GXFifo also
references `PPCSync` and externalizes its unused
`__GXCurrentBP` definition. GXInit keeps the donor's static `FifoObj` and `gxData`
because MWCC generates different code for externs; the build then replaces their
`.bss` definitions with the fallback symbols (`strip_unused.py --external-bss`),
and its small-data references use DTK names. `ax/AXAlloc.c`, `ax/AXAux.c`, `ax/AXCL.c`,
`ax/AXOut.c`, `os/OSMessage.c`, `os/OSSemaphore.c`, `os/OSResetSW.c` and `card/CARDRdwr.c`
are unmodified donors that likewise keep their statics; `--external-bss` also
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

`base/PPCArch.c` is an unchanged donor; its complete text and all 22 retained
functions match. AR and GX now use the proved `PPCSync` name directly.

`ax/AXVPB.c` and the data-only `ax/AXComp.c` are unchanged donors. AXVPB's
four large BSS arrays and three small counters are externalized after compilation;
all storage remains fallback. Its unused weak math constants are stripped by the
existing rules, leaving the complete retained 56-byte constant pool.

`card/CARDMount.c`, `card/CARDNet.c` and `card/CARDUnlock.c` are unchanged
donors. Their initialized data is owned by their splits. CARDOpen and CARDCreate
now use the proved `__CARDPermMask` symbol directly. The include search root
`src/dolphin` resolves CARDMount/CARDNet's `os/__os.h` include.

`os/OSRtc.c`, `os/OSFont.c` and `os/OSFatal.c` are unchanged donors. Their
BSS statics stay in fallback storage and are externalized after compilation.
OSFont owns its encoding tables and initialized constants; OSFatal owns its
message format and conversion constants.

`card/CARDBios.c` is the donor with two edits for version drift: retail calls
`__OSInitAlarm` (declared locally, as in `dvd/dvdlow.c`), and the two static
names that collide with `os/OSMemory` become `CARDOnReset` and
`CARDResetFunctionInfo`. CARDOpen and CARDCreate use the proved
`__CARDDiskNone` symbol directly. `card/CARDProgram.c` is not vendored: retail
does not link it (see `docs/sdk_import.md`).

`exi/EXIBios.c`, `vi/vi.c` and `pad/Pad.c` are unchanged donors except that Pad's
static `OnReset` is `PADOnReset`, because OSMemory already has a local `OnReset`, and its
static `Type` is `PADType`, because SIBios has a local `Type`. Each owns
its initialized data; BSS stays in fallback storage.

`hvqm4/hvqm4dec.c` is a NonMatching reconstruction, separate from the
dolsdk2004 imports. Its provenance is recorded in the source and the
[HVQM4 handoff](../../docs/sdk_import.md#hvqm4-decoder-parked).

`Runtime/__init_cpp.c` adapts the constructor loop from MSSB's CC0
`src/Dolphin/os/__ppc_eabi_init.cpp`, commit
`407dbcbe70f8be5d167d0b1f0a229f38e0359019`; its licence is
[COPYING.MSSB](COPYING.MSSB). `TRK_MINNOW_DOLPHIN/ppcdown.c` and
`Runtime/tealeaf.c` reconstruct SN's original assembly calling conventions
from LSW1, and `Runtime/cvt_fp2unsigned.c` is an independent, unpromoted
ProDG C candidate. These four files are separate from the dolsdk2004 imports.
MSSB's runtime and MetroTRK were reviewed but do not supply these SN bodies.
See [runtime evidence](../../docs/sn_runtime.md#codewarrior-runtime-and-debugger-follow-up).

`amcstubs/AmcExi2Stubs.c` is an unchanged dolsdk2004 donor. Its seven retained
functions form the mode-2 SN debugger transport table; `AMC_IsStub` is dead-stripped.
This corrects the historical `GXGetFifoBase`/`GXGetFifoSize` labels on two stubs.
`dvd/dvdlow_test.c` retains the unchanged dolsdk2004 `__DVDLowTestAlarm` body in
an exact separate range; its alarm objects remain owned by `dvd/dvdlow.c`.

`Runtime/__start.c` and `Runtime/__init_fpu.c` reconstruct SN's original
startup assembly from GL5E4F, with the reset-button C helper adapted from
`os/__start.c` in dolsdk2004. MSSB's CC0 `src/Dolphin/os/__start.c`,
`src/Dolphin/gx/GXFifo.c` and `src/Dolphin/amcstubs/AmcExi2Stubs.c` at
commit `407dbcbe70f8be5d167d0b1f0a229f38e0359019` were reviewed for the differing
startup/stub bodies; no MSSB text was copied for this batch. Its licence is
[COPYING.MSSB](COPYING.MSSB). Per-function assembly evidence and the startup
scheduling override are recorded in [SDK leftovers](../../docs/sdk_import.md#sdk-leftovers).
