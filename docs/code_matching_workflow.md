# C matching

Use ProDG to reproduce retail `GL5E4F`, then replace fallback assembly only
when object sections, relocations and linked ranges are proven. A function
can be **VERIFIED** without its unit being **PROMOTED**. Symbol names and
partial instruction scores do not measure promoted code.

## Toolchain

LSW1 GC game and Nu2 code use **SN Systems ProDG (GCC)**. The linked Dolphin
SDK libraries are Nintendo's CodeWarrior **GC/1.2.5n** builds; compile them
with the dolsdk2004 release flags (`tools/sdk_probe.py`, `score_fn.py --compiler
sdk`). CrashWOC uses CodeWarrior; its algorithms and types are donor evidence,
but its compiler flags and register-allocation behavior do not transfer.
[Strategy](strategy.md) decides which lane a task belongs to.

`tools/prodg_cc.py` drives preprocessing, ProDG `cc1`, `powerpc-eabi-as`
and DTK ELF fixup. Use the pinned `PRODG_VERSION` and `cflags_prodg` from
`configure.py` for a comparison object:

```py
Object(NonMatching, "numath/foo.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg)
```

`NonMatching` compiles the candidate for comparison while the retail target
object remains linked. A source file under `work/` is not compiled automatically.
`Matching` selects the compiled object for the final link. DTK's inherited
Metrowerks linker settings do not identify the game's compiler.

### Compiler audit and RTL diagnostics

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

### Native SN GCC v1.79 evaluation

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

## Select and prepare

1. Read current symbol evidence and rejected renames. Prefer a small function with
   a known prototype, useful types, few callees and a comparison unit already
   configured. Use `ls1_task_pack.py` and donor queues as discovery aids, not proof.
2. Check `tools/matching_progress.py`, then run `bash build.sh` once to establish
   a working baseline. Keep the ProDG version and flags pinned. Do not start
   source tuning when the compiler, target split or retail input is broken.
3. Generate a task under ignored `work/`. Generation refuses to overwrite a
   populated task. Configure the source as a **NonMatching** ProDG comparison
   object; `work/Foo/source.c` is not compiled automatically. Prefer editing an
   existing comparison unit. Keep fallback assembly linked until promotion.
4. Write the signature and a plausible C body from the retail assembly and
   verified types. Treat donor implementations as evidence about algorithms;
   their MWCC code generation is not this game's ProDG code generation.

## Measure and iterate

From the repository root (replace the example function, unit and source):

```sh
python3 configure.py
python3 tools/hillclimb.py try NuVecScale --unit numath/nuvec \
  --source src/numath/nuvec.c --source src/numath/numath.h \
  --hypothesis 'baseline with existing vector types'
python3 tools/hillclimb.py status NuVecScale --unit numath/nuvec
python3 tools/verify_fn.py NuVecScale --unit numath/nuvec --build --strict-symbols --full
```

For the search itself, use the fast scorer. It compiles one file straight to a
temporary object, without ninja or a ledger, in well under a second:

```sh
python3 tools/score_fn.py NuVecScale src/numath/nuvec.c
python3 tools/score_fn.py NuVecScale scratch.c --compile-only -o out.o   # permuter compile step
```

Its score has the same order as `hillclimb.py`. Record a candidate with
`hillclimb.py try` when it improves the incumbent or settles a hypothesis;
exploratory attempts do not need ledger entries.

The agent proposes changes; the local compiler and verifier judge them. This
workflow uses the active agent session and does not invoke a model API or purchase
compute. One owner writes each candidate; independent targets may use separate
worktrees when explicitly delegated under the session's agent policy.

For each attempt:

1. Inspect the first meaningful divergence and state one falsifiable hypothesis:
   signedness, expression order, pointer stride, loop shape, lifetime, or float
   precision. Change one related source choice and compile again.
2. Run `hillclimb.py try` with the hypothesis and **every edited source/header**
   supplied through repeated `--source`. It runs the strict verifier with
   `--build`; stale objects and compiler errors cannot become improvements.
3. Inspect the report, not just the score. Lower lexicographic score wins:
   fully verified match first, fewer differences, smaller instruction-count
   difference, more exact instructions, fewer unverified references. This is a
   search heuristic; aligned instruction counts can be discontinuous and a
   score improvement does not prove semantics or permit promotion.
4. Continue from the best candidate. Revert a losing experiment by deliberately
   editing your own source back to its recorded snapshot. The tool never resets
   the checkout. A neutral or worse experiment can inform a later hypothesis,
   but it does not replace the incumbent best.

Records are immutable numbered JSON files under `work/hillclimb/<identity>/`.
They include hypotheses, source text and hashes, Git head, build configuration /
verifier hashes, command, errors and the complete report. Identity includes the
function, unit and optional compiled symbol: repeat `--compiled-symbol` for status.
Only one writer can record a target at a time. An interrupted writer may leave
`.lock`; confirm no writer remains before removing that lock. Logs stay local;
retain a concise evidence summary and final C in the PR, not raw game artifacts.

Defaults: 40 attempts, 8 consecutive attempts without improvement, 120 seconds
per verifier invocation. Errors and timeouts consume attempts. `PARK` means stop
and save the best attempt, remaining mismatch, rejected ideas and one concrete
next hypothesis in the task's `status.md`; select a different target. Do not
restart a ledger to hide exhausted work. A deliberately changed search budget or
compiler/type context must be explained in the handoff; use a new `--ledger` for
a new context so incompatible scores are never compared. Changes to stored
configuration, symbol/split or verifier hashes block new attempts in that ledger. Source/header context changes still require
the agent to decide whether prior scores are comparable.
An attempt budget is not an agent token budget: follow account usage limits too.

## Verifying a candidate

Prerequisite: `bash build.sh` once (patched DTK, split target objects under
`build/GL5E4F/obj`, retail DOL at `orig/GL5E4F/sys/main.dol`).

```sh
python3 tools/verify_fn.py NuRandInt --strict-symbols      # check an already-built object
python3 tools/verify_fn.py NuVecScale --unit numath/nuvec --build --strict-symbols # rebuild first
python3 tools/verify_fn.py Foo --object work/Foo/source.o  # arbitrary candidate object
python3 tools/verify_fn.py Foo --json                      # machine-readable
```

Pass `--compiled-symbol <name>` when the candidate emits a different function
name from the target. Use `--strict-symbols` for all acceptance checks.

What is compared, per instruction: the 32-bit word with relocation-patched
fields masked out, plus each relocation's type, symbol and addend. Register,
immediate, opcode and instruction-count differences are all failures;
mnemonic-order similarity is printed for orientation only and never counts.
Branches that leave the function without a relocation are resolved to symbols
before comparing. The target object bytes are also checked against the retail
DOL, and an `.rodata`/`.sdata2` constant referenced through a relocation is
compared with the retail bytes at the target symbol's address (width from the
load/store instruction).

Exit codes: `0` match, `1` mismatch, `2` error (unknown symbol, missing or stale
object, build/tool failure, inconsistent target), `3` code matches but some
references are only mapped by consistency (`--strict-symbols`). Verdicts:

| Verdict | Meaning |
|---------|---------|
| `MATCH` | Code identical; every reference verified by name, same `symbols.txt` address, or data bytes. |
| `MATCH (unverified references)` | Code identical, but some referenced symbols differ by name (for example `fseed` vs `lbl_80407B40`) and could only be mapped consistently. Lines under `references:` say why. |
| `MISMATCH` | Any word, relocation, length or constant differs. |

Staleness: without `--object`, the compiled object must be up to date
according to `ninja -n` (falls back to source mtime); otherwise the tool exits
2 instead of reporting on an old object. `--build` runs ninja and exits 2 with
the compiler output if it fails; `--allow-stale` skips the check.

Limitations (what a `MATCH` does not prove):

- Only the named function's code and the data it references through
  relocations are compared; data layout, symbol placement, section choice
  (mine `.sbss` vs retail `.sdata`), alignment and linker ordering are not.
- Referenced data is compared only for the width of the first load or store
  that uses it (at most 8 bytes); the rest of a table or struct is not checked.
- Two separate literals in mine that retail shares through one symbol are
  reported as a mismatch (the name mapping is strictly one-to-one).
- Exit 0 includes `MATCH (unverified references)`; scripts that must reject
  names mapped only by consistency should pass `--strict-symbols` (exit 3).
- Address-taken data (`addi rX, sym@l`, SDA21 `li`) and bss have no width or
  retail bytes to compare and stay "unverified".
- Debug-only differences and anything after linking (SDA base, final addresses)
  are out of scope; objdiff and a relinked DOL remain the final authority.
- The side-by-side text uses `dtk elf disasm`; if it fails the verdict is
  unaffected and raw words are shown.

Regression tests: `tests/test_verify_fn.py` (synthetic objects everywhere; the
genuine ProDG/retail cases run after `bash build.sh` and are required in the
Forgejo retail CI job).

## Promotion

1. Restore the best source, rebuild, and run `verify_fn.py --build
   --strict-symbols` for **every function in the affected unit**. Resolve
   unverified names with address, type and data evidence.
2. Compare raw bytes of every owned section and all relocation offsets,
   types, addends and resolved section-relative targets against the DTK target
   object. Match data placement, initializers, alignment and symbol visibility.
3. For a partial assembly replacement, run `tools/asm_function_slice.py <name>`
   to plan the before/C/after split. `--write` emits slices only; review them
   and wire the source and `splits.txt` deliberately. Keep current boundaries
   unless binary evidence supports changing them.
4. Mark the verified unit `Matching` in `configure.py` and run `bash build.sh`.
   It compares promoted units with retail using `dol_compare.py --linked-units`.
   Compare the report and whole-image differences with the baseline too:
   previously matched code and unrelated linked ranges must not regress.
5. Retain target addresses, baseline/best attempt, exact verification commands,
   promoted bytes/functions, report delta and remaining limits in the PR.
   An improved `NonMatching` candidate is useful, but promotes zero bytes.

A strict function match proves only that function and the checked references.
Do not weaken the verifier, toolchain pins or split boundaries to improve a score.
The whole DOL remains the final goal; an existing link difference does not excuse
new differences in a promoted range.

### SDK complete-section gate

For a Dolphin unit, run `python3 tools/verify_unit.py dolphin/gx/GXTexture --build`
in addition to strict verification of every retained function. The gate compares
all allocated section extents, nonrelocated bytes and canonical relocations with
the DTK target, resolving local targets through the declared split bases and
external targets through unique symbols. It also checks retail bytes and encoded relocation destinations,
including branches, SDA references and initialized data pointers. SDA21 word/halfword offsets are normalized; unsupported
data relocation kinds, incomplete ownership and BSS fail closed.

`verify_fn.py` uses this same complete-unit proof to verify an address-taken
local table only when its section-relative address agrees with the reference.
An identical instruction sequence or consistent reference mapping alone still
cannot verify a table. This does not promote a unit automatically; the final
linked-range gate remains required. See [SDK import evidence](sdk_import.md)
for every promoted unit and repeatable verification commands.

### Object and linked-range checks

For the already promoted `numath/nurand` unit, after `bash build.sh`:

```sh
build/binutils/powerpc-eabi-objcopy -O binary -j .text build/GL5E4F/obj/numath/nurand.o /tmp/nurand-target.bin
build/binutils/powerpc-eabi-objcopy -O binary -j .text build/GL5E4F/src/numath/nurand.o /tmp/nurand-source.bin
cmp /tmp/nurand-target.bin /tmp/nurand-source.bin
build/binutils/powerpc-eabi-readelf -r build/GL5E4F/obj/numath/nurand.o
build/binutils/powerpc-eabi-readelf -r build/GL5E4F/src/numath/nurand.o
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
```

Repeat the byte comparison for `.sdata` and any other owned sections. Relocation
symbol spelling may differ; compare the actual section-relative targets.
`--linked-units` reads complete units from `objdiff.json` and their section ranges
from `splits.txt`. `tests/test_dol_compare.py` covers this check; retail verifier
regressions live in `tests/test_verify_fn.py`.

## nurand promotion evidence

The initial promoted unit owns `.text` `0x8000C0C0..0x8000C1B8` (five functions,
248 bytes) and `.sdata` `0x80407B40..0x80407B64` (36 bytes). At promotion, both
sections and all five `R_PPC_EMB_SDA21` relocations matched the target object;
all 284 linked bytes matched retail. Changing the LCG increment from
`0x3c6ef35f` to `0x3c6ef35e` made the linked-range check fail at `0x8000C0D7`.

| `.text` offset | Use | Target |
|---|---|---|
| `0x00`, `0x1C` | `NuRandInt` load/store | `.sdata+0` (`lbl_80407B40`) |
| `0x48` | `NuRandSeed` store | `.sdata+0` |
| `0x58` | `NuRandSetSeed` address | `.sdata+8` (`lbl_80407B48`) |
| `0x84` | `NuRand` address | `.sdata+8` |

### Source changes

The pre-existing source matched `.text` but not data: it placed `fseed` and
`global_rand` in `.sbss`, whereas retail has 36 initialised bytes in `.sdata`.
ProDG 3.9.3 puts an explicit `= 0` initialiser in `.sdata` (it has no
zero-initialised-in-bss mode for this), so the source now declares:

| Retail address | Contents | Source |
|----------------|----------|--------|
| `0x80407B40` | `0x00000000` | `fseed` |
| `0x80407B44` | `0x00001365` | `unk_80407B44` (no reference found in code) |
| `0x80407B48` | `0x00000001` | `global_rand` |
| `0x80407B4C`-`0x80407B63` | `0 0 0 0 0 1` | `unk_80407B4C`..`unk_80407B60` |

ProDG's GCC keeps unreferenced statics and emits variables in definition order.
Objects larger than 8 bytes go to `.data`, so the 24-byte tail must be several
small objects; an array would land in `.data` and fail the layout.

Two things here are **not** established by evidence:

- Whether `unk_80407B44` and the tail belong to this translation unit. The tail
  has no direct references; it is carried verbatim because the current split
  includes it, and narrowing the split would leave `lbl_80407B48`
  (`size:0x1C` in `symbols.txt`) straddling the boundary. The Mac demo has a
  4-byte `global_rand`, so the real owner of the tail is probably a neighbour.
  Fix by splitting `symbols.txt` and `splits.txt` together (the `symbols.txt`
  change belongs to the symbol-naming workstream).
- The names `unk_*` are placeholders.

### Symbol names

Other retail units reference the dtk names. `fn_8000C038` (float-generator candidate) uses
`lbl_80407B40`, and a link with only local statics fails with
`undefined: 'lbl_80407B40'`. Rather than editing `symbols.txt`, `nurand.c` ends with
a top-level `asm` exporting `lbl_80407B40` and `lbl_80407B48` as aliases of
`fseed` and `global_rand`. The linked ELF shows `80407b40 d fseed` and
`80407b40 D lbl_80407B40` at the same address. Delete the `asm` block once
`symbols.txt` names those two symbols.

## Remaining link differences

The [SDK import](sdk_import.md) resolves the historical differences below by
using the existing retail ProDG linker with explicit section addresses and SDA
bases. The current linked image has **zero differing bytes**. These observations
are retained to explain why MWLD must not silently replace the SN linker.

The initial nurand promotion left the same 1,192 differing bytes as an all-target
object relink. This is historical evidence, not a substitute for measuring the
current build:

- 396 `.sdata2` references (1,188 differing bytes) used `r2` after linking with
  `mwldeppc`; retail uses `r13`. Both reached the same effective addresses.
  Retail `r13` is `0x8040FA40`; the linker's `.sdata2` base is `0x80413300`.
  Folding sections in the linker script was rejected by `mwld`. Investigate
  relocation handling or linker behavior before source-level tuning here.
- Two `__start` words differed: the `r2` initializer at `0x8000310C` and the
  `.sbss` clear end at `0x80003160`. Retail uses `0x8041FA40` and `0x8040B2E4`;
  the relink used `0x80413300` and `0x8040BD40`.

`build.sh` requests `main.dol` explicitly because the default Ninja checksum
check still expects a whole-image match. The configured CFA exclusion and
blocked startup relocation are documented in [build configuration](build_config.md).
Do not hide these link differences with a broader exclusion.

The preceding functions at `0x8000C038` and `0x8000C07C` use nurand's state
and are grouped with it in the Mac demo. Matching them and establishing the
owner of the `.sdata` tail are separate evidence tasks before extending this unit.

## Discovery tools

- `ls1_task_pack.py <name-or-address> --out-dir work`: assembly, context and
  executable strict checks. Historical committed packs under `tasks/` may
  contain superseded names; resolve addresses against current `symbols.txt`.
- `crashwoc_code_queue.py /path/to/crashwoc-decomp --include-norm`: donor
  candidates from `symbol_donors/nu2_fn_matches.tsv`. The default exact queue
  is mostly trivial stubs; normalized candidates need full verification.
- `sdk_probe.py --sdk /path/to/dolsdk2004 --all --tsv work/sdk_probe.tsv`:
  compiles donor SDK units with MWCC 1.2.5n, anchors functions to retail by
  name or layout (skipping linker-stripped functions) and classifies each one
  as `MATCH`, `UNVERIFIED`, `CODE_SAME` (code identical, names missing),
  `MISMATCH`, `NO_ANCHOR` or `ERROR`. Layout anchors can coincide by size; the
  verifier rejects those.
- `matching_progress.py`: objdiff report, configured with
  `functionRelocDiffs=all`. Lenient relocation mode previously produced a
  false `NuVecRotateAxis` match against the wrong float constant.
- [Research](research.md): symbol lookup, donor provenance, Ghidra and RAM dumps.
