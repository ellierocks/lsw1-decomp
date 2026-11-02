# Build configuration

`configure.py` selects comparison/linked objects, compiler pins and flags.
`config/GL5E4F/config.yml` tells DTK where to find the retail DOL, symbols and
splits. Keep these files consistent when adding a unit; see
[C matching](code_matching_workflow.md#promotion) for the verification gate.

## Symbols

`config/GL5E4F/symbols.txt` uses absolute GameCube addresses:

```text
NuRandInt = .text:0x8000C0C0; // type:function size:0x24
```

Use the actual size from the binary. Common attributes are `type:function|object|label`, `size:`, `scope:global|local|weak`,
`align:` and `data:` (`float`, `double`, `4byte`, `string`, etc.). `noreloc`
prevents pointer-looking contents from becoming relocations; `force_active`
prevents dead stripping. Apply these only with evidence. The DTK configuration
schema is documented in [config.example.yml](../config/GL5E4F/config.example.yml).

Use `tools/ls1_rename.py` for verified names. Check sizes and references before
merging labels into a struct or splitting an object; a name change alone does
not establish a boundary. Generated reports can retain old names.

## Splits

`config/GL5E4F/splits.txt` declares section types/alignments and per-unit ranges.
Ranges are absolute and end-exclusive. Existing nurand wiring:

```text
Sections:
    .text       type:code align:4
    .sdata      type:data align:8

numath/nurand.c:
    .text       start:0x8000C0C0 end:0x8000C1B8
    .sdata      start:0x80407B40 end:0x80407B64
```

This is an excerpt, not a replacement for the full section list. A source path
must also be configured as an `Object` in `configure.py` to compile C. Unassigned
ranges remain target objects. DTK infers link order; do not force an order or
shrink ranges to obtain a higher match score. Assign code and owned data together.

Relevant optional attributes are section `align:`, `rename:` and `skip`, plus
file `order:` and `comment:`. `comment:0` disables generated Metrowerks metadata
for an object; it is linker compatibility, not a ProDG compiler setting.
Do not apply common-BSS or CodeWarrior `.comment` recipes from other games
without evidence that the target needs them.

### Proposed game TU boundaries

`tools/tu_boundaries.py` proposes `.text` units for the game and Nu2 code below
the first Dolphin split and writes `work/tu_boundaries.tsv`. The committed
`splits.txt` initially carried its 78 provisional `auto/` units. To regenerate them after
a symbol or split change, run
`python3 tools/tu_boundaries.py --write-splits config/GL5E4F/splits.txt`.
This replaces the existing `auto/` blocks and keeps every other unit. Check that
the DOL still matches before committing. Its evidence:

- **`__FILE__` strings.** Each TU's `.rodata` block holds its own source path,
  for example `../nu2api.legogc/nucore/ngc_obj/../nufile.c`. The blocks follow
  `.text` link order. There are 92 `.c`/`.cpp` paths; 188 functions load one.
  SDK paths are in `.data`/`.sdata` and are not used.
- **Other `.rodata` loads.** A function's literal loads usually fall between
  its TU's path string and the next one. These are weak evidence.
- **Mac demo adjacency.** Base names are taken from the Itanium-mangled Mac
  symbols. 364 consecutive GC pairs are also adjacent on Mac, and the
  alignment keeps them together.
- **Not padding.** Consecutive functions tile exactly, at known unit
  boundaries as well, so gaps carry no signal.

A monotone DP assigns functions to the path-string TUs, or to an anonymous
gap before each one. With the current symbols, it assigns 3316 functions to
70 TUs, and no path load lands in another TU. 5 of the 69 boundaries have no
slack. The `start_range` column gives the equally good start addresses for each
TU, and the DP picks the earliest.

The 78 `auto/` units are cut around existing splits and appended after them.
They link a byte-identical DOL (SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`),
and an incremental rebuild takes about 9 s.

The `auto/` units are provisional and give game code a target object per TU.
When a TU's C is matched, give its unit the real path (for example
`gamelib/terrain.c`), drop the `auto/` prefix, and fix the boundary from the
matched code. Change a boundary only on evidence. Revise it first where it
blocks promotion, such as an absorbed TU.

Limits:

- TUs without a path string are absorbed into neighbours. Examples are
  `game/levels.c` with 482 functions and `game/obstacle.c`, `game/gameaiscript.c`
  and `ai/aisys/aisys.c`, each with 168 to 282 functions.
- The next evidence to use is the 186 per-TU header copies of `VuMath.h`
  (175) and `nudlist_gen.h` (11). There are 2 to 9 copies between some pairs of
  consecutive `.c` paths, and each extra copy marks an anonymous TU.
- `.data`, `.bss`, `.sdata` and `.sdata2` are not proposed yet. `.rodata` is (below).

### Provisional `.rodata` ranges

`tools/rodata_splits.py` gives 59 game/Nu2 units a `.rodata` range taken from their code.
It reads the `.rela.text` relocations in each built target object, maps them to `.rodata`
symbols, and uses the span from the first referenced symbol to the end of the last one.
These spans follow `.text` link order. A unit gets no range when:

- its span overlaps another unit's. Investigate `__FILE__` xrefs and actual shared
  literal targets before merging; overlapping envelopes alone do not prove a
  common TU (see the [family audit](#split-tu-family-audit));
- it is `Matching`, because its compiled object would have to supply the range
  (`--include-matching` proposes it for a separate proof);
- its code references no `.rodata`.

Unreferenced bytes between two spans stay unsplit. The ranges relink a byte-identical DOL.
They let objdiff resolve a unit's literals inside its own target object. Under
`functionRelocDiffs=all` a literal counts only when its value and section offset match
retail. That happens when the unit's literal layout lines up with retail, which is also
what promotion needs. Adding the ranges took three functions to 100% (`numtl`'s
`fn_800248CC` and two in `nugraph_4`). The rest of the strict-only gap stays separate.

#### Split-TU family audit

The `nuanim_1` / `nu3dx/nuanim` / `nuanim_2` comparison units are now combined
as `nu3dx/nuanim.c`, covering their unchanged outer `.text` envelope
`0x80016558..0x80018F78`. The two internal cuts at `0x80016A98` and
`0x8001752C` were artificial comparison boundaries:

- `NuAnimCurve2CalcVal` in the middle piece and `NuAnimCurveCreate`,
  `NuAnimDataCreate`, `NuAnimDataLoad` and other functions in the final piece
  reference the **same** `__FILE__` object at `0x80190074`,
  `../nu2api.legogc/nu3d/ngc_obj/../nuanim.c`.
- The first piece's `fn_80016944` references constants at `0x801901E4`,
  `0x801901E8` and `0x801901F0`, interleaved within the same literal block
  used by the middle/final pieces. Its reference envelope is
  `0x8018FFD0..0x801901F8`, not a preceding independent block.
- Removing the cuts lets `rodata_splits.py --write` assign the union
  `0x8018FFD0..0x80190374` (0x3A4 bytes). Source bodies are preserved;
  all 22 previously strict matches remain strict. This consolidates comparison
  ownership, not a promotion or proof of the unresolved timebar-prefix boundary.

The other families remain separate after the initial audit:

| Family | Evidence and remaining work |
|---|---|
| `nufile_1/2/3` | All share the path object at `0x8018CB9C`; `_1/_2` also share `NuFileClose - close failed, retrying` at `0x8018CD24`. Promoted `nufile_size` and `nufile` slices lie between them; preserve those promotions when choosing a consolidation scheme. |
| `nufile_ngc_1/2` | Both share the path object at `0x8018E304`; promoted `nufile_exists` lies between them. Same promotion-preservation requirement. |
| `nufont_ngc` through `nuqfnt_ngc` | Each has its own source-path xrefs (`0x8019167C`, `0x801918E8`, `0x80191AC4`, `0x80191C4C`, `0x80191D80`); no shared named `.rodata` relocation target was found across these five units. Overlapping spans do not justify merging them. Investigate boundary placement and symbol extents first. |
| `aiscript/aisys` | Distinct path objects at `0x8019BC94` and `0x8019C6DC`; both reference `param` at `0x8019BD48` and `Base` at `0x8019BD80`. These common words alone do not prove a common TU; investigate cross-boundary ownership before merging. |
| `creature/levels` | Distinct path objects at `0x8019FA30` and `0x801A24A0`, with no shared named `.rodata` relocation target found. Investigate boundary placement and symbol extents first. |

These are target relocation/path observations, independent of objdiff scores.
The union does not establish that every symbol-sized aggregate in the proposed
range is a single original literal, or identify original input sections.

#### Alignment evidence and limits

dtk warns that 34 provisional spans begin at a 4-aligned address but contain an
8-aligned double. A **reference span is not proof of one original input section**.
For example, `nustring` begins at `0x8018D88C`, immediately after `numem`'s
`VuMath.h` strings; its double at `0x8018D8F0` is absolutely 8-aligned but is at
span offset `0x64`. Rounding the span start or reducing symbol alignment would
change the evidence rather than explain it. Keep these spans provisional.

Pinned ProDG 3.9.3 (`cc1` SN BUILD v1.76), with the wrapper's `-O2`, gives these
results for `use("a string", 123.456)`:

| Probe | Result |
|---|---|
| Default | One `.rodata`, string offset 0, double offset `0x10`, section alignment 8 |
| `-fdata-sections` | Same literal pool; no `.rodata.cst*` sections |
| `-ffunction-sections -fdata-sections` | Same pool; only the function section changes; current DTK fixup rejects this probe |
| `-malign-section-names` | One `.rodata.lit4`, still aligned 8 and containing both literals |
| `#pragma pack(4)` | Same literal pool and double alignment |
| `#pragma section rodata=".rodata.str"` | No section change |
| Named `const` objects with `__attribute__((section(...)))` | Separate string and double input sections are supported |

GNU `powerpc-eabi-as` and the shipped `NgcAs.exe` (also with `-sn`) agree on
the default pool's alignment. The pinned SN linker honors it: linking that
object at `0x8018D88C` inserts four bytes **before the string**, rather than
rebasing the pool's internal double alignment. A linker start address alone
therefore does not reproduce retail's offsets. `-mno-strict-align`,
`-mno-bit-align`, `-mno-eabi`, `-msdata=none`, `-mno-fp-in-toc` and
`-fno-const-strings` also leave this pool unchanged; `-fmerge-constants` and
`-m[no-]align-double` are not accepted by this compiler.

Separate input sections **can** reproduce the address relationship without
weakening alignment. A synthetic pinned-compiler probe declares:

```c
const char strings[0x64] __attribute__((section(".rodata.str"))) = "probe";
const double constant __attribute__((section(".rodata.cst8"))) = 123.456;
```

After a four-byte predecessor at `0x8018D888`, the SN linker places `strings`
at `0x8018D88C` and `constant` at `0x8018D8F0` using:

```text
.rodata 0x8018D888 : { *(.rodata) *(.rodata.str) *(.rodata.cst8) }
```

This is a demonstrated mechanism, **not proof of retail's original flags,
section names or declarations**. The DOL has lost its input-section metadata;
the tests above do not establish how retail generated those sections. No
compiler override, linker-script change or split rewrite is justified yet.
Promotion of an affected span still requires reproducing its complete layout
and linked addresses; a strict function match alone cannot settle this.
Units without this alignment conflict can be verified independently.

`tu_boundaries.py --write-splits` rewrites the `auto/` blocks and drops their `.rodata`
lines. Afterwards, run `bash build.sh`, then `python3 tools/rodata_splits.py --write`.

## Per-TU ProDG flags

`cflags_prodg` is the shared game/Nu2 baseline (`-O2`), not proof of the
original flags for every provisional unit. An `Object` can replace its `cflags`
in the same style as `sdk_cflags(unit)`. Once a unit has a confirmed override,
make a named list/helper beside `cflags_prodg` and pass it only to that object's
`cflags=`. Derive the list from the baseline, replacing the optimization option
rather than leaving competing `-O` options. Preserve includes and defines.
No ProDG per-TU override is currently confirmed or installed.

`tools/prodg_cc.py` routes `-O*`, `-f*` and `-m*` to the pinned cc1; preprocessing
options go to CPP. For small-data experiments use **two tokens** `-G 0`, not
`-G0`: the wrapper explicitly joins the former for cc1, but drops the latter.
Unknown options can be silently dropped, so inspect retained commands and
compiler output before interpreting a trial. `-fno-common` concerns tentative
definitions and section placement; it cannot be inferred from setters of
extern objects. Likewise, unknown-size extern arrays affect SDA selection.
See [the acceptance procedure](code_matching_workflow.md#per-tu-flag-investigations).

### Podrace investigation (Phase 3)

The batch-4 claim that podrace demonstrates different flags was a hypothesis.
No tested flag set explains its four parked functions. The stronger evidence
is that the provisional `auto/game/podrace` range absorbs debugger assembly
after `PodraceLoadLapSettings` ends at `0x80150398`. The first suspect routine
starts at `0x801503A0`; this is a boundary lead, not an authorized split change.

| Parked address | Retail signature | Baseline reconstructed probe |
| --- | --- | --- |
| `0x801507CC` | `lis/ori r3` with HI/LO relocations to `lbl_802EAC60`, then `li r4,1; stw r4,0x6d4(r3)` | `lis r9` with HA relocation to base+offset, `li r0,1; stw` with LO relocation; 4 vs 5 instructions |
| `0x80150D80` | HI/LO `lis/ori r4` to `lbl_801E19DC`, `li r5,0; stw r5,0(r4)` | HA/LO addressing via r9, zero in r0; 4 vs 5 instructions |
| `0x80150DBC` | MMIO address in r3, `lwz r4; andi. r4,r4,0x14; li r4,0; blr` | Discarded masked volatile read loses the mask and returns zero in r3; 5 vs 6 instructions |
| `0x80150418` | `li r6,0; stw r6,0x6ec(r31); li r3,0; blr` | Ordinary global-base store materializes an address; 5 vs 4 instructions |

These are not simply scheduling differences. `fn_80150DE8` calls
`fn_80150DBC` then immediately branches on its CR0 result. The callee leaves
the MMIO pointer in r3 and clears r4 **without changing CR0**. Ordinary C
return-value conventions do not express that interface. `fn_80150418` uses
inherited r31 without establishing a stack frame; nearby routines load r31
with `lbl_802EAC60`, a state base. Others save LR in r30 and use private calls.
`fn_80150F34` restores CPU state and executes `rfi` at `0x80151008`.
The same region's data includes `snPause() : Stopped.`, an exception-vector
patch error and a non-debug-mode file-server error. Together these support an
SN debugger runtime/assembly attribution, not a game-wide optimization change.
The nearby `EnableMetroTRKInterrupts` name alone does not identify its vendor.
Debugger/transport assembly continues through at least `0x80152D00`, but the
whole remaining split is not assembly: later functions have ordinary ProDG C
prologues, and arithmetic runtime code appears near `0x8015BD40..0x8015BEF4`.
The [cross-game body evidence](symbol_donors/nu2_gc_body_confirmations.tsv)
also finds `fn_801503A0` unchanged in LSW2 and Narnia.

The historical best drafts/ledgers were not retained in the available checkout.
The experiment therefore reconstructed minimal probes, without claiming to
reproduce their old best scores: integer stores to unknown-size extern arrays
at the first two addresses; a discarded masked volatile read of `0xcc006000`
followed by `return 0`; and a store to global base+`0x6ec` followed by `return 0`.
All experiments and raw objects remain local under `work/tu_flags/`.
The three existing source functions were included as regression controls.

Pinned ProDG 3.9.3 cc1 (SN v1.76) was used for 70 compile trials:

- Baseline, plus the Cartesian product of `-O0`, `-O1`, `-O2`, `-O3`, `-Os`;
  neither/both/each disabled scheduler (`-fno-schedule-insns`,
  `-fno-schedule-insns2`); and default/`-fno-omit-frame-pointer` (40 trials).
- Individually on `-O2`: `-fno-inline`, `-finline-functions`, `-fno-common`,
  `-fcommon`, `-fomit-frame-pointer`, `-fno-peephole`, `-fno-gcse`,
  `-fno-expensive-optimizations`, `-mno-sdata`, `-msdata=none`, `-msdata=eabi`,
  and `-G` thresholds 0, 4, 8, 32, 1024 (16 trials).
- Individually: `-mrelocatable`, `-mrelocatable-lib`, `-meabi`, `-mno-eabi`,
  `-msdata=sysv`, `-msdata=data`, `-mcall-sysv`, `-mcall-eabi`, `-mcpu=750`,
  `-mcpu=603`; plus no-inline with both schedulers disabled, and no-inline
  with each of `-O1` and `-O3` (13 trials).

All compiled; none strictly matched any parked probe. Thirty-three trials
preserved all three controls. `-O0` adds frames/spills even to empty functions;
forcing a frame pointer also breaks the controls. `-G 0` changes the known
setter's SDA accesses and breaks it. No-common and inlining cannot repair the
private register/CR interface. Strict `verify_fn.py --build --strict-symbols`
checks on temporarily installed baseline probes returned MISMATCH for the four
parks and MATCH for `fn_80146F1C`, `fn_8014CF8C`, `fn_801508F4`; the probe source
was then removed. No code was accepted or promoted.

A retail sweep covered all 76 `auto/` text ranges (78 units including units
without text), with 3,318 symbol bodies. It found no r30/r31 stack-frame
establishments/restores of the tested `mr`/zero-offset `addi` forms. All 12
`mflr r30` bodies and all 21 call-bearing bodies without an r1 adjustment
were in `auto/game/podrace`. No other TU has this same signature. This does
not prove uniform scheduling, inlining, common-data or small-data settings:
bytes alone, without equivalent source and owned-data definitions, cannot
identify those flags. The evidence-supported list of non-default flag TUs is
currently empty; podrace's tail is an assembly/boundary candidate instead.

Next: identify the debugger runtime and its entry/exit conventions, then
establish its real text/data ownership before changing splits or choosing an
assembly implementation. Do not spend another C permutation budget trying to
make a normal ABI return reproduce CR0/r4 results.

## Dolphin SDK and retail linker

`DolphinLib` uses MWCC `GC/1.2.5n` with dolsdk2004's release flags:
`-O4,p -inline auto`, unsigned char by default, and `SDK_REVISION=1` / `RELEASE`.
`sdk_cflags` replaces flags for signed-char DVD, MTX/MTX44 and CARDOpen units,
and `-O3,p` EXIBios; it does not append a competing optimization level.
SDK-only include paths select the vendored libc shims. `-MD` records header
dependencies even with `-I-`.

SDK objects run `dtk elf fixup`, then `tools/strip_unused.py --target ...`.
The retained names come from the independently split target object. The
RE4-derived stripper removes unused functions/data while shifting symbols,
relocations and MWCC comment records, and rejects dangling references. Local
pooled data is retained; unused global data follows the reference's eight-byte
granularity. `.sdata2` is normalized to SN/DTK read-only ELF metadata.

An object that opts in with `external_bss=True` (only GXInit so far) also passes
`--external-bss config/GL5E4F/symbols.txt`. MWCC generates different code for
extern and statically defined objects, so the source keeps the donor's static
BSS definitions. The stripper then replaces them with undefined references to
the fallback `.bss` symbols and empties the section. It aborts unless every
name, size and relative offset agrees with `symbols.txt`. Those fallback symbols
must be global, and BSS contents stay unpromoted.

The DOL uses the existing `ProDG/3.9.3/ngcld.exe` through
`tools/run_prodg_link.py`, with `config/GL5E4F/ldscript.ld`. This fixes MWLD's
SDA register selection and startup symbol differences without changing compiler
pins or patching the linked image. MWLD support remains available in the build
generator for its original configurations. The current whole image matches
retail. See [SDK import evidence](sdk_import.md) for ranges and limits.

## Splitter and startup constraints

`tools/build_patched_dtk.sh` pins DTK and applies
`tools/patches/dtk-skip-invalid-extab.patch`. The patch handles this ProDG
binary's exception tables and symbol map; an unpatched DTK is not equivalent.
Build from that script rather than substituting the similarly named release tag.

Two target-specific exceptions remain in `config.yml`:

- `skip_cfa_ranges` excludes `.text:0x80151AFC..0x8015BF48` because the initial
  analyzer overextended control flow into known functions. Review this region
  manually before deriving new function boundaries from it.
- `block_relocations` excludes `.init:0x8000315C`, whose startup address cannot
  be resolved to a section by DTK. This permits splitting; it does not solve
  the linked startup differences described in [C matching](code_matching_workflow.md#remaining-link-differences).

Compiler/DTK pin changes require a deliberate retail rebuild, strict object
verification, linked-range comparison and report comparison. Renovate does not
validate these toolchain assumptions.
