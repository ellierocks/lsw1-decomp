# Compiler evaluation

Evidence behind the ProDG pin and the matching diagnostics. Matching agents do not need this to
start; read it when a register or scheduling difference resists source changes. The pinned
toolchain and the matching loop are in [C matching](code_matching_workflow.md).

## Compiler audit and RTL diagnostics

The pinned `ProDG/3.9.3/cc1.exe` reports GCC 2.95.3 **SN BUILD v1.76**,
not v1.79 (SHA-256 `f72f09daad839440d8bed38b766e0207fcb8c9eacf0be1ca04e91dc60a91bdba`).
The installed CPP reports SN BUILD v2.16. Confirm a local installation with
`build/tools/wibo build/compilers/ProDG/3.9.3/cc1.exe -version`.

An audit of `ngccc.exe -v -O2 -c` found that its preprocessing command adds
`__OPTIMIZE__`, GCC version, PowerPC, big-endian, unsigned-char, EABI and SN
target defines, plus C language defines. Our wrapper currently forwards only
the configured preprocessing flags; it also drops `-G` options. These are known
differences to investigate with a selected retail target, not established fixes.
The current default remains unchanged: the full build and strict `NuRandInt`
baseline pass with it.

For a plateau, retain compiler-pass evidence without running ninja:

```sh
python3 tools/score_fn.py NuRandInt src/numath/nurand.c --diagnostics-dir /tmp/nurand-rtl -o /tmp/nurand.o
```

Use a new output directory per attempt. It contains `candidate.i`,
`candidate.prodg.s`, compiler version/options and output in `compiler.log`,
and exact stage arguments/exit codes in `commands.jsonl`. RTL dumps include
`.cse`, `.gcse`, `.loop`, `.lreg`, `.greg`, `.sched` and `.sched2` after
`candidate.i`. Look at local/global allocation for register differences and
the two scheduling dumps for instruction ordering. The wrapper accepts the
same `--diagnostics-dir` option. SDK mode rejects it. Diagnostic flags preserve
the complete `nurand.c` object byte for byte in the real-toolchain regression.
Compiler failure logs and intermediates remain available; dumps may be incomplete.
Keep these artifacts local.

[RE4's matching catalogue](https://github.com/adonis-singh/re4/blob/9b76ff003081d6ece8b838453fb8310743a972c7/docs/matching.md)
maps symptoms to source levers. Its native v1.79 build separates fast-cast and
classic conversion scratch-address representations; our packaged v1.76 does
not. Its additional aliasing patch reconstructs inferred RE4 compiler behavior
and has a C-global exception. Neither a compiler replacement, that patch nor
RE4's `-mfast-cast` flag is validated for LSW1 by this audit. Their instrumented
allocation hooks need a source-built compiler; see
[Native SN GCC v1.79 evaluation](#native-sn-gcc-v179-evaluation).

## Native SN GCC v1.79 evaluation

SN Systems released ProDG for NGC's GCC sources under the GPL
(`NGC/ProDGforNGCv393_Source_Code.zip`, 28,626,142 bytes, from
`https://archive.org/download/sn_sys_consoles_2/NGC/ProDGforNGCv393_Source_Code.zip`;
SHA-1 `01addd9bf5acbad2bf825b8ddf0a4f32db692742`, SHA-256
`4e4fc72f017db9cada5a5ba486286a936583840cfb39d1ede0439486fa3d0f77`).
It contains `GnuSrcNGC.txt` and `NGC_GNU_SRC.zip` (SN BUILD **v1.79**, 2003-06).
Download only that file, extract it into new empty directories and point
`SN_GCC_SRC` at the extracted `NGC_GNU_SRC/NGC`. [`tools/sn-gcc`](../tools/sn-gcc/README.md)
builds it as a 32-bit host `cc1` without touching the pinned compiler:

```sh
SN_GCC_SRC=/path/to/NGC_GNU_SRC/NGC tools/sn-gcc/build.sh build/sn-gcc/v179
SN_GCC_SRC=... tools/sn-gcc/build.sh build/sn-gcc/v179-diag alloc-diagnostics
python3 tools/score_fn.py NuRandInt src/numath/nurand.c --cc1-native build/sn-gcc/v179/cc1
python3 tools/compare_prodg_cc.py --out /tmp/cmp \
    --variant 'v179:cc1=build/sn-gcc/v179/cc1' --variant 'defines:defines=ngccc' \
    --variant 'fastcast:flag=-mfast-cast'
```

`configure.py --prodg-cc1-native PATH` and repeatable `--prodg-arg=ARG` apply the
same evaluation to ninja builds. Without them the ninja command line, the pinned
`cc1.exe` and the default codegen are unchanged. Variants stay separate runs so
each hypothesis is tested alone: compiler build (`--cc1-native`), driver
preprocessing defines (`--ngccc-defines`, plus `-G N` forwarded as `-GN`), cc1
flags, and the experimental alias patch.

Results on the four ProDG units (`nufile`, `nurand`, `nuvec`, `numtx`) and the
conversion-heavy draft functions `fn_8001448C`/`fn_8001459C`, all compared with
the strict verifier:

| Variant | Outcome |
| --- | --- |
| v1.79 `cc1` | Disassembly and strict results identical to v1.76; only `.comment` differs. Relinking the full DOL is byte-identical to retail (SHA-1 `fde58439…`). |
| ngccc driver defines | Identical objects. |
| `-G1024` | Identical objects. |
| `-mfast-cast` | Identical objects on these units. LSW1 retail has no GQR 1–7 use (all `psq` use qr0), so there is no retail evidence for it. |
| `shipped-build-mem-flags` | Identical objects; vendor provenance is unproven, so it stays unadopted. |

RE4's reported v1.76/v1.79 difference reproduces only synthetically: a source
mixing `-mfast-cast` and classic int/float conversions yields different `mr`
copies and registers. LSW1 uses neither fast-cast nor that mix, so v1.79 is not
a matching lever for it today. **The pin stays v1.76**; this build is for
experiments if a future target shows the mix.

The `alloc-diagnostics` patch is print-only: `GDBG=1` (global allocation order),
`GDBGV=1` (verbose), `LADBG=1` (local allocation quantities) and loop-hoisting
`LOOPDBG` lines (also under `GDBG=1`) write to stderr. Assembly was byte-identical to the plain v1.79 build with the
variables unset, set, and with `-mfast-cast`, on six sources. Use it for a
plateau where the RTL dumps do not explain a register choice:

```sh
GDBG=1 LADBG=1 python3 tools/score_fn.py NuRandInt src/numath/nurand.c \
    --cc1-native build/sn-gcc/v179-diag/cc1 --diagnostics-dir /tmp/nurand-alloc
```

The trace lands in `compiler.log`. Output is large for big functions; keep it
local. The patch drops RE4's behavior-changing switches (`GFORCE`, `NOEQV`,
`NOMALLOC`, scheduler/CSE/reload hooks).

Limits: results cover the units above, not every future function; v1.79 is a
host-compiled approximation of SN's Windows build (GCC 16 host, `-m32 -static`);
and the retail binary's exact compiler build is only known to be v1.76 for the
shipped `cc1.exe`, not for the game's original build.
