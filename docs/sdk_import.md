# Dolphin SDK import evidence

The first Phase 1 import promotes nine complete SDK translation units from
[dolsdk2004](../src/dolphin/README.md). All 75 retained functions strictly match;
their sections, canonical relocations and linked ranges match retail. The
unreferenced donor functions are removed before comparison and linking, as
with the original SN linker. No candidate instruction bytes are replaced.

## Importing a unit quickly

Units that are one `.text` range plus file-scope `.bss`/`.sbss` statics (the common
case; AXAlloc and AXAux are examples) import with one command. Setup is shared
between worktrees: `bash tools/bootstrap.sh` links `build/tools`, `build/compilers`
and `build/binutils` and the retail DOL from `~/.cache/lsw1` or the primary
checkout, clones the pinned dolsdk2004 into `~/.cache/lsw1/dolsdk2004`
(`tools/fetch_dolsdk.sh`), and finishes in a second on a prepared machine.
`LSW1_CACHE` moves the cache, `DOLSDK2004` points at another donor checkout,
`LSW1_REFRESH=1` refetches tools after a pin change and `LSW1_NO_SHARE=1` keeps a
private toolchain.

```sh
bash build.sh                                                     # once per worktree: target objects
python3 tools/sdk_probe.py --pending --tsv work/sdk_pending.tsv   # import queue
python3 tools/sdk_import_unit.py ax/AXCL                          # plan only
python3 tools/sdk_import_unit.py ax/AXCL --apply                  # import + prove
```

The probe anchors donor functions to retail by name, by unique code bytes (so units
whose retail functions are still `fn_XXXXXXXX` are found) and by layout, and ignores
anchors that cannot fit in the unit's compiled size. The importer then copies the
source and donor-local headers, inserts the split and `SDKObject` in address order,
builds the split target, and pairs the donor's relocations with the target's to
rename unnamed functions, callees and statics in `symbols.txt` (rejected renames are
never applied; unresolved pairs are printed as `manual:`). It finishes with the
full-unit and per-function proof (`verify_unit.py --functions`, one process for every
function), the linked-image comparison, and prints the table row for this file.
Re-importing AXAlloc and AXAux this way from baseline `97e792b` reproduces the
hand-made splits, configuration, symbols and sources byte for byte, in about three
seconds each.

A failed `--apply` leaves the split, configuration and renames in place. The error
output starts with the lines that explain it (`strip_unused:`, `error`, `FAILED`,
`Missing`) before the last 30 lines of the build, and a `strip_unused:` failure also
lists the donor `.bss`/`.sbss` objects with their sizes and offsets. `verify_unit.py`
says why a proof was refused (first differing relocation or byte). Fix the symbols, then
rerun `verify_unit.py dolphin/UNIT --build --functions`. To start over:

```sh
git checkout config configure.py && rm src/dolphin/UNIT.c   # plus any copied donor headers
bash build.sh                                               # prerequisite for every import and probe
```

`tools/sdk_probe.py` and the importer need `build/GL5E4F/obj` from `bash build.sh` and
stop immediately without it.

Renames also rewrite the old `lbl_` name in existing sources, in one simultaneous pass
(so swaps and chains are safe).

**Name conflicts.** When a donor function is anchored to a retail function that already
carries a different SDK name, the plan prints `name conflict   retail NAME at ADDR vs
donor NAME, same size, code identical` when the code is equal outside relocations. Such
a proposal is checked against `docs/symbol_donors/rejected_renames.tsv` and, if the
donor name is held by another function, that holder is demoted to `fn_ADDR` (a same-named
static in another split, such as the second `OnReset`, blocks it instead). Chains and
swaps count a name vacated by another accepted proposal as free, and for a pairing
conflict the importer follows the callee into the donor unit that defines it and proposes
the whole shifted run. Nothing is renamed on size alone; functions of at most 16 bytes
count as identical only on a unique code match or next to a proven neighbour. Plans with
proposals are refused by `--apply` until you add `--accept-name-conflicts`, which applies
exactly the ones marked code-identical and prints the rest as `unproven`, `blocked` or
`rejected`.

**`.bss`/`.sbss` granularity.** DTK splits a `u64` into two 4-byte labels and merges
adjacent arrays into one covering symbol. The importer merges the unnamed label into
the object before it when the donor accesses `NAME+4` and `label+0` and both are 4 bytes
(`OSTime`, `HoldUp`, `HoldDown`, `__OSStartTime`). When a retail symbol covers several
donor objects, the donor object sizes and offsets are printed, and the symbol is split
automatically only when the donor sizes tile it exactly; otherwise the objects are
listed as `manual:` (`AXOut`'s DSP task and local profile are examples).

**Includes.** `src/dolphin` is added as an include root (`-i src/dolphin`) when a donor
source includes a directory-qualified header such as `"os/__os.h"`; the header is taken
from the repo, or copied from the donor's `src/` tree.

Plans stop with an error, and `--apply` is refused, when the unit owns initialised
data (`.data`, `.rodata`, `.sdata`, `.sdata2`), when retail functions inside the
range are not matched by the donor, or when the range overlaps an existing split.
Those need the manual steps below (or, for statics that only dead-stripped functions
used, `strip_unused.py --external-bss` now drops them; see GXDisplayList below).

Keep file-scope statics as the donor wrote them: declaring them `extern` changes MWCC's
code (it reloads values around callbacks). A fresh import starts on the fallback, so the
importer emits `external_bss=True` for units with `.bss` or `.sbss`, and `--external-bss`
replaces the storage. `strip_unused.py` maps the donor's static names to `symbols.txt`
objects by name, which is why the importer renames the retail `lbl_` labels to the donor
names. Once the unit's code is verified, promote its BSS as described next.

### Promoting a unit's `.bss`/`.sbss`

BSS has no retail bytes, so it is proved by **layout**, never by zero contents. A unit
owns its `.bss`/`.sbss` (a `.bss`/`.sbss` range in its `splits.txt` entry and
`external_bss=False`) only when all of these hold:

1. The compiled section size equals the retail split range.
2. Every compiled symbol has a retail symbol with the same name, offset and size. The
   retail symbols tile the range with no gap or overlap. Alignment padding (under 32
   bytes, ending exactly at the next compiled object or the section end) may be either
   a dtk `gap_NN_ADDR_bss` symbol or extra size on the retail symbol. A zero-size MWCC
   `...bss.0` anchor is not an object. Merged retail symbols (`gxData`, `u64` halves)
   are split in `symbols.txt` first, as above, and only where the donor sizes tile them.
3. Every relocation into the section, from any unit, resolves to the retail address.
   `verify_unit.py --build` links the unit's objects with the shipped linker script and
   requires each compiled symbol to sit at `range start + offset` in `main.elf`.
4. The full linked image stays byte-identical to retail (the `bash build.sh` gate).

`tools/verify_unit.py dolphin/<unit> --build` enforces 1–3 and fails on any unexplained gap
or extra symbol; `build.sh` enforces 4. Work through it like this:

- Take the base of each donor static from `symbols.txt` (compile the unit raw, subtract
  the object offset). The base must be consistent across the section, and the unit
  order must follow `.text` order: BSS starts ascend with the `.text` starts.
- The range is `[base, base + compiled section size)`. A trailing alignment pad stays
  with dtk's gap unit, so the end need not be 8-aligned, but dtk rejects a gap unit
  that starts at an odd address (AXSPB below).
- Keep statics `static` in C. MWCC lays out and addresses a non-static uninitialised
  global differently (it moves it after the statics, which also shifts `.text`).
  `strip_unused.py` instead raises the compiled binding to what the target object
  declares, so another unit (a dtk auto unit, a game unit) can reference
  `AlarmForTimeout`, `AlarmForBreak` or `__AXUpdates` by their retail names.
  Mark such symbols `scope:global` in `symbols.txt`.
- Shrink a symbol's `symbols.txt` size only when it overhangs the unit's range end.
  Do not grow a size to swallow interior padding: objdiff then scores the section as a
  partial match and credits no data bytes, whereas a `gap_` symbol keeps the full
  match. Delete an unreferenced filler label (`lbl_8030FBB4` in OSFatal) so dtk can
  emit the gap itself.
- Common symbols follow MWCC's emission, so a non-static uninitialised global in a donor
  would be COMMON and land after the section's statics. No promoted unit has one; if you meet one
  the layout check will fail on the offset, and the unit stays on the fallback.

**Units promoted so far** (bytes of `.bss` + `.sbss`): OS 164, OSExec 8, OSFont 16, OSError 68,
OSResetSW 32, OSRtc 84, OSThread 2,564, OSFatal 728, EXIBios 196, SIBios 512, vi 428, Pad 128,
dvdfs 32, dvd 244, dvdqueue 32, dvdlow 208, AXAlloc 260, AXAux 11,572, AXCL 1,556, AXOut 19,124,
AXVPB 71,692, CARDBios 580, GXInit 1,584 (`.bss` only, after splitting `gxData`) and
SISamplingRate 4 (`SamplingRate`, folded back from the old `SISamplingRateBSS` unit).

**Remaining exceptions** (still `--external-bss`, reason recorded):

| Unit | Why it stays on the fallback |
|---|---|
| `ax/AXSPB` | The compiled `.bss` is `0x36` bytes; the gap unit dtk would need starts at the odd address `0x802FCB16`, and dtk aborts (`Invalid alignment for split`). The donor size is a MWCC-vs-retail rounding problem, not a layout mismatch. |
| `gx/GXDisplayList` | Defines `DisplayListFifo`, `OldCPUFifo` and `__savedGXdata`, but retail has no symbols for them: they sit unnamed in dtk's `lbl_8030EB90` (0x5D8, the tail of the old 0xb88 `gxData`). Requirement (b) cannot be proved without per-symbol offsets, and no xref evidence has been gathered to split the label. Without a retail range the compiled BSS would grow the image. |
| `OdemuExi2/DebuggerDriver` | Its 21-byte `.sbss` ends at `8040ACE5`; DTK rejects the following odd-address gap. All six statics are located by relocations, but their storage remains on the fallback. |
| `os/OSReset` | Carries the flag but defines no BSS statics; harmless, left alone. |

`ai/ai` is not on the fallback and has no retail `.bss`/`.sbss` range: its
`profile` and `__ai_src_time_*` are debug-only, so nothing needs owning.
| Game and Nu2 units | Out of scope. See [build_config](build_config.md#bss-layout-policy) for how the same rule applies later. |

Several units can be imported in parallel worktrees: each touches `splits.txt`,
`configure.py`, `symbols.txt` and this file at different lines, so conflicts are
mechanical. Rebase, rerun the importer's verify step (`verify_unit.py UNIT --build
--functions`) and regenerate the totals rather than merging numbers by hand.

## Measured result

Baseline is strategy commit `299b2fa`, built from the pinned retail DOL.
`tools/matching_progress.py` reports:

| Measure | Baseline | This import |
|---|---:|---:|
| Matched code | 2,044 bytes | 14,328 bytes |
| Matched functions | 23 | 98 |
| Matched data | 36 bytes | 1,444 bytes |
| Promoted SDK units | 0 | 9 |
| Whole-image differing bytes | 1,192 | 0 |

The final DOL also reproduces the complete retail SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`.

The SDK contributes **12,284 code bytes, 1,408 data bytes and 75 functions**.
Every imported unit reports 100% code and data matching, with strict relocation
comparison enabled. Other source-backed units retain their baseline matches.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Data bytes |
|---|---|---:|---:|---:|
| `os/OSSync` | `80162334..801623B8` | 2 | 132 | 0 |
| `os/OSMutex` | `80163914..80163984` | 1 | 112 | 0 |
| `dsp/dsp_debug` | `801756A4..801756F4` | 1 | 80 | 0 |
| `gx/GXGeometry` | `8017FE24..80180144` | 9 | 800 | 0 |
| `gx/GXTexture` | `801810EC..80182140` | 21 | 4,180 | 920 |
| `gx/GXBump` | `80182140..8018267C` | 10 | 1,340 | 24 |
| `gx/GXTev` | `8018267C..80182D38` | 15 | 1,724 | 116 |
| `gx/GXTransform` | `801833E0..8018392C` | 13 | 1,356 | 20 |
| `gx/GXPerf` | `8018392C..8018432C` | 3 | 2,560 | 328 |

Owned data ranges are in `config/GL5E4F/splits.txt`. The texture and performance
jump tables identify exact `.data` extents. The TEV tables occupy 116 bytes;
the following four alignment bytes remain fallback data, with the former
40-byte placeholder split into a 36-byte table and four-byte padding label.
Constant blocks are placed by their referenced retail addresses and complete
byte identity, including constants used only by removed donor functions.

## Integration and identity

The reference implementation is RE4's identical ProDG 3.9.3 / MWCC 1.2.5n mix;
its compiler release flags, override pattern, dead stripping and SN linker
wrapper are ported here. The compiler pins are unchanged. Using `ngcld.exe`
from the existing ProDG toolchain fixes the previous MWLD `.sdata2` r2/r13
selection and startup base/clear-end differences. The linker script declares
retail section addresses and startup bases; it contains no retail bytes.

The first proof was GXBump: ten strict matches, four stripped functions,
24 identical constant bytes, then a fully identical linked image. Further
units passed the same gates before being marked `Matching`.

Strict code identity corrected stale GX names, including `__GXSetVCD` at
`8017E734` and `__GXSetSUTexRegs` at `80181C70`. Their old bindings at
`8017C774` and `80181794` were removed; the latter is the verified
`GXLoadTexObjPreLoaded`. Other corrected identities include `GXInitTexObjTlut`,
`GXGetTexObjFmt`, `GXLoadTlut`, `GXSetTevKColor`, `GXSetTevOrder`,
`GXSetNumTevStages` and `__GXUpdateBPMask`. The texture register-ID arrays
match both their complete data block and code references. Rejected donor
renames were checked before naming; symbol changes alone count as zero bytes.

## Reproduction

Run the normal build, then the full-unit gate and strict function gate for
each imported unit:

```sh
bash build.sh
python3 tools/matching_progress.py
python3 tools/verify_unit.py dolphin/gx/GXTexture --build
python3 tools/verify_fn.py GXInitTexObj --build --strict-symbols
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
```

`verify_unit.py dolphin/LIB/UNIT --build --functions` also strictly verifies every
function of the unit in one process (about 0.6 s for a 13-function unit). To repeat
every SDK unit/function check after building:

```sh
python3 - <<'PY'
import json
import subprocess
from pathlib import Path
from tools.verify_fn import Elf, STT_FUNC

for unit in json.loads(Path('objdiff.json').read_text())['units']:
    source = unit.get('metadata', {}).get('source_path', '')
    if not source.startswith('src/dolphin/'):
        continue
    name = source[4:].rsplit('.', 1)[0]
    subprocess.run(['python3', 'tools/verify_unit.py', name, '--build'], check=True)
    for fn in Elf(Path('build/GL5E4F/src') / (name + '.o')).function_symbols():
        if fn.sym_type == STT_FUNC and fn.size:
            subprocess.run(['python3', 'tools/verify_fn.py', fn.name,
                            '--build', '--strict-symbols'], check=True)
PY
```

Re-probe the retained donor functions after naming with
`tools/sdk_probe.py --strip-imported gx/GXTexture` (or `--all --strip-imported` for
every imported unit; non-imported units are skipped)
(and the other imported units). The default probe still reports the complete
unstripped donor for discovery; the explicit imported mode enables full-unit
proofs after dead stripping. All 75 retained rows are `MATCH` in that mode.

`__OSSystemCallVectorStart/End` are zero-size assembly labels, not additional
functions. The verifier's full-unit proof compares every allocated section,
relocation offset/type/addend and resolved target, and all bytes outside
relocation fields. It independently checks encoded retail destinations, including code
branches, SDA references and initialized data pointers. Address-taken data becomes verified only after this proof succeeds;
unsupported layouts, data relocations and BSS remain unverified.

Portable tests cover stripped-function relocation shifts, data stripping
remainders, rejection of dangling references, MWCC comment records, complete
unit/table proofs, SDA21 normalization, and corrupted table contents/targets.
Raw probe output and verification logs remain ignored under `work/`.

## GXAttr follow-up

Starting from PR #18's merged `11663e1fa085611cdad55d99fb85de21cc0ea93e`,
unchanged donor `gx/GXAttr.c` promotes another **14 functions, 5,044 code bytes
and 640 data bytes**. Total matching is now **19,372 code bytes / 112 functions
and 2,084 data bytes**; ten SDK units contribute 17,328 code bytes, 2,048 data
bytes and 89 functions. The complete rebuilt DOL retains retail SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`, with zero differing bytes.

| Section | Retail range, end exclusive | Bytes |
|---|---|---:|
| `.text` | `8017E240..8017F5F4` | 5,044 |
| `.data` | `801FACC0..801FAF34` | 628 |
| `.sdata` | `8040A168..8040A174` | 12 |

The initial helper hypothesis was wrong: there is no standalone
`__GXXfVtxSpecs` body preceding `GXSetVtxDesc`. The helper is inlined into
`__GXSetVCD` at `8017E734`, whose 47 instructions strictly match the donor.
The SN linker removed its unused out-of-line copy, along with `GXGetVtxDescv`
and `GXGetVtxAttrFmtv`. No source or compiler-flag change is needed.

Strict code identity establishes `GXSetVtxDesc` at `8017E240`, `GXSetVtxDescv`
at `8017E4AC`, `GXSetTexCoordGen2` at `8017F338` and `GXSetNumTexGens` at
`8017F5B8`. False helper/vertex-desc bindings at `8017C764`, `8017C76C` and
`8018432C` are removed and recorded in the rejected-renames ledger.

All eight jump tables occupy the complete 628-byte `.data` block. Three
four-byte lookup arrays used by `__GXCalculateVLim` occupy `.sdata`. The
former eight-byte placeholder at `8040A170` is split into its four-byte array
and four bytes of following alignment padding, which remain fallback data.
The donor's section extent, array contents and retail references establish
this boundary; the complete-unit gate proves all bytes and table relocations.
The report's total-data denominator falls by four because DTK excludes that
separate padding range; no retail bytes are removed or changed.

The baseline promoted zero GXAttr bytes; the final unchanged donor promotes
the complete unit without a remaining mismatch. Reproduction after building:

```sh
python3 tools/verify_unit.py dolphin/gx/GXAttr --build
# Run verify_fn.py NAME --build --strict-symbols for all 14 retained functions
# using the function enumeration in the reproduction script above.
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported gx/GXAttr
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
sha1sum build/GL5E4F/main.dol
```

All 14 retained functions pass the strict function gate, all three sections
pass the complete-unit gate, and the retained donor probe reports `MATCH=14`.
Raw objects, baseline/build logs and probe output stay local under `work/`.

## GXLight follow-up

Starting from merged GXAttr PR #20 (`a92c397`), unchanged donor `gx/GXLight.c`
promotes **nine functions, 1,280 code bytes and 96 data bytes**. Total matching
is **20,652 code bytes / 121 functions / 2,180 data bytes**. Eleven SDK units
contribute 18,608 code bytes / 98 functions / 2,144 data bytes. Baseline GXLight
promotion was zero bytes; no source or compiler changes were needed.

| Section | Retail range, end exclusive | Bytes |
|---|---|---:|
| `.text` | `80180BEC..801810EC` | 1,280 |
| `.data` | `801FB128..801FB144` | 28 |
| `.sdata2` | `8040B810..8040B854` | 68 |

`GXInitLightSpot` at `80180C08` was incorrectly named `GXInitLightDistAttn`.
The latter and eleven other donor functions are dead stripped. The retained
functions are `GXInitLightAttn`, `GXInitLightSpot`, `GXInitLightPos`,
`GXInitLightColor`, `GXLoadLightObjImm`, `GXSetChanAmbColor`,
`GXSetChanMatColor`, `GXSetNumChans` and `GXSetChanCtrl`.

The unresolved call at `8018ACC8` is independently identified as newlib 1.8.2
fdlibm `cosf`, using RE4 commit `9b76ff003081d6ece8b838453fb8310743a972c7`'s
`src/lib/fdlibm/sf_cos.c`, rather than inferring a name from GXLight's call.
Pinned ProDG 3.9.3 with `-O2 -msafe-sda -G 1024 -fno-builtin -mstrict-align`
reproduces all 55 instructions and its zero constant. Independently compiled
`kf_cos.c` and `kf_sin.c` reproduce all 59 and 43 instructions at `8018ADA4`
and `8018AE90`, including all nine and seven referenced constants. The
argument reducer `ef_rem_pio2.c` reproduces all 212 instructions at `8018AF3C`
when compiled with `cc1plus.exe`, consistent with RE4's C++ libm wrappers;
compiling it as C gives only 200/212 exact instructions. Its table references
remain unverified, so no math unit or math bytes are promoted. Only the
independently established `cosf` name is assigned for this import. Raw source
objects and comparison logs remain under ignored `work/`.

The seven-entry jump table is proved completely, including each destination.
The constant block retains pooled literals used only by stripped functions;
the unused weak sqrt inline constants are stripped by the existing importer.
The donor's final float ends at `8040B854`; four following alignment bytes
remain fallback data. The old 32-byte placeholder at `8040B838` is split into
its actual float/double objects and that padding. DTK's reported data
denominator falls by eight after separating padding ranges; no retail bytes
are changed or omitted from the whole-image comparison.

All nine retained functions pass `verify_fn.py NAME --build --strict-symbols`.
The complete-unit gate passes all three sections, and the stripped donor
probe reports `MATCH=9`. Reproduction:

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/gx/GXLight --build
# Run the strict function gate for the nine names listed above.
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported gx/GXLight
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
sha1sum build/GL5E4F/main.dol
```

The complete rebuilt image has zero differing bytes and retains SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. There is no remaining GXLight
mismatch.

## GXPixel follow-up

Starting from merged GXLight PR #21 (`bfddf3f`), unchanged donor `gx/GXPixel.c`
promotes **twelve retained functions, 1,592 code bytes and 128 data bytes**. Total
matching is **22,244 code bytes / 133 functions / 2,308 data bytes**. Twelve SDK
units contribute 20,200 code bytes / 110 functions / 2,272 data bytes. The function
total rises by one because `__cvt_fp2unsigned` is now a typed function in
`symbols.txt`, so the denominator becomes 4,148.

| Section | Retail range, end exclusive | Bytes |
|---|---|---:|
| `.text` | `80182D38..80183370` | 1,592 |
| `.data` | `801FB4D8..801FB4F8` | 32 |
| `.sdata2` | `8040B8A8..8040B908` | 96 |

The baseline GXPixel promotion was zero bytes. The only source change is the
unchanged vendored donor file; the rest is symbol naming and unit wiring.

- `GXSetFog` at `80182D38` strictly matches its 532 bytes. Its call resolves to
  `80153184`, and the pinned MWCC object records that reference as
  `__cvt_fp2unsigned`.
- `__cvt_fp2unsigned` at `80153184` is 72 bytes. No donor function supplies its
  body, so the name rests on instruction identity rather than a donor compile. Its
  18 instructions match RE4's libsn `__cvt_fp2unsigned` in `src/lib/tealeaf.c`
  (RE4 commit `9b76ff003081d6ece8b838453fb8310743a972c7`) one for one, with the
  same branch displacements. The only difference is the constant's address: RE4 loads
  `0x80225C60`, while LSW1 loads `0x801AFF78`, which holds the double
  `0x41E0000000000000` (2^31). The routine compares its input with 2^31,
  converts with `fctiwz`, and for larger inputs subtracts 2^31 and restores the
  top bit with `xoris`. That is the conversion MWCC's float-to-unsigned call
  expects.
- `GXSetFogRangeAdj` at `80182F4C` replaces the stale `GXSetFogColor` name. Its
  292 bytes strictly match the donor `GXSetFogRangeAdj`, and the old name is in
  the rejected-renames ledger.
- Donor `GXSetFogColor` (48 bytes) and `GXInitFogAdjTable` (432 bytes) have no
  retail body. The retail fog code is contiguous from `GXSetFog` through
  `GXSetFogRangeAdj` to `GXSetBlendMode` at `80183070`, with no room for them. The
  SN linker dead-stripped both unreferenced functions, so they are absent from the
  retained set rather than inferred from the donor.

The `p2f[8] = {0, 1, 2, 3, 4, 4, 4, 5}` table referenced by `GXSetPixelFmt` is at
`801FB4D8`. A byte search of the DOL finds it exactly in the gap between the GXTev
and GXPerf `.data` ranges. `GXSetFog` loads eight small-data constants from
`8040B8A8` through `8040B8E0`. The donor object's `.sdata2` is exactly 96 bytes,
ending at GXTransform's `8040B908`; the final 40 bytes are pool entries that no
retained function references, as with GXLight's stripped-function constants. The
complete 96-byte block matches retail.

All twelve retained functions pass `verify_fn.py NAME --build --strict-symbols`.
The complete-unit gate passes all three sections, and the stripped donor probe
reports `MATCH=12`. Reproduction:

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/gx/GXPixel --build
# Run the strict function gate for the twelve names listed above.
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported gx/GXPixel
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
sha1sum build/GL5E4F/main.dol
```

The complete rebuilt image has zero differing bytes and retains SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. There is no remaining GXPixel
mismatch.

## GXFrameBuf follow-up

Starting from merged GXPixel PR #22 (`e453914`), unchanged donor
`gx/GXFrameBuf.c` promotes **14 functions and 2,728 code bytes**, with all
**424 owned data bytes** proved by the complete-unit gate. Total matching is
**24,972 code bytes / 147 functions / 2,324 matched data bytes**. Thirteen SDK
units contribute 22,928 code bytes / 124 functions / 2,696 complete data bytes.
Baseline GXFrameBuf promotion was zero bytes; no compiler or source changes
were needed.

| Section | Retail range, end exclusive | Bytes |
|---|---|---:|
| `.text` | `80180144..80180BEC` | 2,728 |
| `.data` | `801FAF90..801FB128` | 408 |
| `.sdata2` | `8040B800..8040B810` | 16 |

Six shifted names caused the apparent version drift. Strict comparison against
the donor establishes `GXSetDispCopyYScale` at `80180560`,
`GXSetCopyClear` at `8018062C`, `GXSetCopyFilter` at `801806A4`,
`GXSetDispCopyGamma` at `801808AC`, `GXCopyDisp` at `801808C0` and
`GXCopyTex` at `80180A28`. The stale `GXCopyTex` binding at `8017C89C`
is removed without assigning an unproved replacement identity. Its body and
the six shifted bindings are recorded in the rejected-renames ledger.

The retained text is contiguous from `GXAdjustForOverscan` through
`GXClearBoundingBox`, bounded by the already promoted GXGeometry and GXLight
units. The standalone `__GXGetNumXfbLines`, `GXGetNumXfbLines`,
`GXGetYScaleFactor` and `GXReadBoundingBox` copies are stripped. This does
not rest on NO_ANCHOR: there is no gap for them in the retained unit, and a
four-byte-aligned whole-image search masking only donor relocation fields
finds no complete instruction identity for any of those four bodies.
`GXSetDispCopyYScale` strictly reproduces all 51 instructions, including the
inlined line-count calculation; its conversion call resolves to the previously
identified `__cvt_fp2unsigned` at `80153184`. `GXSetTexCopyDst` retains its
verified `__GetImageTileCount` call at `801812AC`.

Five complete 60-byte render modes match retail byte for byte:
`GXNtsc480IntDf` at `801FAFA0`, `GXMpal480IntDf` at `801FB000`,
`GXPal264Int` at `801FB04C`, `GXPal528IntDf` at `801FB08C` and
`GXEurgb60Hz480IntDf` at `801FB0E0`. The other 27 donor modes each leave
four bytes under the existing eight-byte stripping rule: 300 retained mode
bytes plus 108 remnant bytes establish the complete 408-byte block. The
complete-unit gate proves every byte, including those remnants. The 16-byte
constant pool contains 256.0f, alignment and the unsigned-conversion double
used by stripped functions.

Objdiff reports 100% complete data but credits only the 16-byte constant pool
as matched data. Its `.data` comparison is 98.50746% despite exact raw bytes
and no data relocations; the target labels the remnant regions, while the
stripped donor has no symbols there. Thus this import adds 424 complete data
bytes but only 16 reported matched data bytes. No artificial source symbols or oracle changes are introduced to improve that
metric; the splits follow complete donor section ownership.

All 14 retained functions pass `verify_fn.py NAME --build --strict-symbols`;
the complete-unit gate passes three sections and the stripped probe reports
`MATCH=14`. Reproduction uses the function enumeration above:

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/gx/GXFrameBuf --build
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported gx/GXFrameBuf
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
sha1sum build/GL5E4F/main.dol
```

The complete image has zero differing bytes and retains retail SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. There is no remaining
GXFrameBuf code, section or relocation mismatch. Raw investigation and
verification logs remain under ignored `work/`.

## CARDStat follow-up

Starting from merged GXPixel PR #22 (`e453914`), unchanged donor `card/CARDStat.c`
promotes **two retained functions, 876 code bytes and no data**. Total matching is
**23,120 code bytes / 135 functions / 2,308 data bytes**. Thirteen SDK units
contribute 21,076 code bytes / 112 functions / 2,272 data bytes. The retained
functions are `UpdateIconOffsets` (static) and `CARDSetStatusAsync`.

| Section | Retail range, end exclusive | Bytes |
|---|---|---:|
| `.text` | `8017AE34..8017B1A0` | 876 |

The unit has no data sections in the donor object, so no data ownership is needed.

- `CARDGetStatus` and `CARDSetStatus` are absent from the retail image. A whole-`.text`
  masked search with a positive control found no match, so `strip_unused.py`
  removes them. The retained neighbours are adjacent in retail, which agrees.
- The callees are established by masked byte identity against the donor card
  objects, not by name: `__CARDGetControlBlock` at `801770C8`, `__CARDGetDirBlock`
  at `801777A4`, `__CARDUpdateDir` at `80177944`, and `__CARDIsWritable` at
  `80179CE0`. The stale names at `80176ECC`, `80177544` and `8017765C` moved out of
  their positions, so they are recorded in `rejected_renames.tsv`. `80176ECC` is
  `__CARDEraseSector`, which belongs to the wider CARDBios block and remains
  unrenamed here.
- The runtime divide helper at `801531D0` is 4 bytes and tail-calls `__divdi3` at
  `8015AA80`. Its body matches RE4's `_divdi3.c` with the pinned ProDG compiler
  and zero masked mismatches. `OSGetTime` is an `s64`, which needs this helper.
- Verification: `verify_fn.py UpdateIconOffsets --build --strict-symbols` and
  `verify_fn.py CARDSetStatusAsync --build --strict-symbols` both report MATCH.
  `verify_unit.py dolphin/card/CARDStat --build` passes its complete section.
  `sdk_probe.py --strip-imported card/CARDStat` reports two MATCH functions. The
  whole image has 0 differing bytes and retains SHA-1
  `95cca08a19224775d1a8d6cc64601fb7d0080981`.

## OSMemory follow-up

Starting from merged CARDStat (`main` at `a24d18b`), unchanged donor
`os/OSMemory.c` promotes **six retained functions, 728 code bytes and 16 data
bytes**. Total matching is **26,576 code bytes / 155 functions / 2,340 matched
data bytes**. Fifteen SDK units contribute 24,532 code bytes / 132 functions /
2,712 complete data bytes. Before this import, OSMemory contributed no matched
bytes. The source is the donor file copied verbatim and compiled with the
existing pinned flags.

| Section | Retail range, end exclusive | Bytes |
|---|---|---:|
| `.text` | `80160BB8..80160E90` | 728 |
| `.data` | `801E3E08..801E3E18` | 16 |

- The retained functions are, in retail order, `OnReset` (60 bytes, static),
  `MEMIntrruptHandler` (108, static), `Config24MB` (128, static asm),
  `Config48MB` (128, static asm), `RealMode` (24, static asm) and
  `__OSInitMemoryProtection` (280). They are contiguous: `fn_80160ADC` ends at
  `80160BB8` and `__OSReboot` starts at `80160E90`. The former `fn_80160BB8`
  is `OnReset`, confirmed by strict body identity rather than by layout.
- `ResetFunctionInfo` at `801E3E08` is the 16-byte reset record: the `OnReset`
  pointer, priority `0x7F` and two null links. It is owned by this unit. While
  the record sat outside the unit, `strip_unused` rejected `OnReset` as a
  removed reference from `.data`, so the record's `.data` range is included
  here and `OnReset` stays local.
- Strict call bindings resolve `__OSErrorTable` (`.bss` `802F3B50`),
  `__OSUnhandledException` (`8015E95C`), `OSDisableInterrupts` (`80160130`),
  `OSRestoreInterrupts` (`80160158`), `__OSSetInterruptHandler` (`8016017C`),
  `__OSMaskInterrupts` (`801604F8`), `__OSUnmaskInterrupts` (`80160580`),
  `OSRegisterResetFunction` (`80160F14`) and `DCInvalidateRange` (`8015D8D4`).
  The MMU and memory-controller accesses are absolute hardware addresses, so they
  need no symbol.
- `OSGetPhysicalMemSize`, `OSGetConsoleSimulatedMemSize` and `OSProtectRange`
  have no standalone body in retail. This is not inferred from `NO_ANCHOR`. The
  unstripped donor, compiled with the pinned flags, was searched word by word
  across the retail text sections, with relocation fields masked (including
  halfword `@ha`/`@lo` fields). The same search positively hit `OnReset`,
  `MEMIntrruptHandler` and `RealMode` at their retail addresses, which serves as
  the positive control. It found no match for the three absent functions.
  Absence therefore holds for code identical to the donor compile; a drifted
  standalone copy would need separate evidence. The three stay unnamed.
- The rejected-renames ledger has no entries for these addresses, so no
  candidate name was rejected. No May 2004 version drift was observed: all six
  retained bodies match strictly.

All six retained functions pass `verify_fn.py NAME --build --strict-symbols`.
`verify_unit.py dolphin/os/OSMemory --build` proves both complete sections, and
the stripped donor probe reports `MATCH=6`. Reproduction:

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/os/OSMemory --build
# Run the strict function gate for the six names listed above.
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported os/OSMemory
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
sha1sum build/GL5E4F/main.dol
```

The complete rebuilt image has zero differing bytes and retains SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. The matching-progress report counts
16 of 50 complete files. Raw probe and verification logs remain under ignored `work/`.

## OSCache follow-up

Donor `os/OSCache.c` is vendored unchanged and promoted. Its retail `.text` is
`8015D8C0..8015DCF8` (1,080 bytes, twelve functions); the other donor functions
are dead stripped. All twelve retained functions pass
`verify_fn.py NAME --build --strict-symbols`: nine resolve every reference
exactly, and `L2GlobalInvalidate`, `DMAErrorHandler` and `__OSCacheInit` pass
with consistency-bound references. Their retail names are already present.

Retail `0x8016731C` was named `PSMTXIdentity` in `symbols.txt`. Its bytes are
a 0x50-byte varargs prologue ending in `blr`, which matches the donor's
`void DBPrintf(char* str, ...) {}` in `db.c`. OSCache's debug calls in
`L2GlobalInvalidate`, `DMAErrorHandler` and `__OSCacheInit` branch there. The
name is changed to `DBPrintf`; the old
name is recorded in `docs/symbol_donors/rejected_renames.tsv`.

The retained `.data` is 554 bytes at `801E2B40..801E2D6A`, which matches retail
byte for byte. DTK does not accept an unaligned auto-split at `801E2D6A`, so the
split extends to the existing label boundary `801E2D70`. The six zero bytes
before that label are linker alignment that the donor object does not contain.
`verify_unit.py` previously rejected the 554-byte object against the 560-byte
range. It now accepts a donor data section that is shorter than its range only
when the missing tail is under 32 bytes and is all zero in both the target
object and the retail DOL; code, BSS, longer gaps and non-zero tails are still
rejected. With that rule `verify_unit.py dolphin/os/OSCache --build` proves both
complete sections, the unit is `Matching`, and the whole image remains identical
(SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`). The report counts 17 of 53
complete files.

## Probe and verifier unblockers

Two tooling faults made Phase 1 look further away than it was.

- `sdk_probe.py` omitted the SDK `src/` directory from the include path, so
  `#include "os/__os.h"` failed and six units (`card/CARDCheck`, `CARDFormat`,
  `CARDMount`, `CARDNet`, `dvd/dvd`, `dvd/dvderror`) reported `ERROR` with no
  evidence. They now compile.
- DTK renames a local symbol in a split object to `NAME_<address>`
  (for example `XYNTSC_801F5B40`). `SymbolTable.addr` did not resolve that form,
  so any function that touched a named static was reported as a relocation
  mismatch (`CODE_SAME`) even though `symbols.txt` held the name at that address.
  `addr` now resolves the suffixed form only when the unsuffixed name is in
  `symbols.txt` at exactly the suffixed address.

A full re-probe with both fixes moved discovery rows from 145 to 42 `CODE_SAME`,
from 146 to 294 `UNVERIFIED` and from 267 to 298 `MATCH` functions. Raw TSVs
stay under ignored `work/`. At that point four units contained only `MATCH`,
`UNVERIFIED` or `CODE_SAME` functions and were the cheapest next promotions.

## SISamplingRate follow-up

`si/SISamplingRate` now promotes two functions (264 code bytes) and its complete
initialized-data section (152 bytes). `SISetSamplingRate` and
`SIRefreshSamplingRate` both pass strict verification; `verify_unit.py` proves
the complete `.text` and `.data` sections, including relocations. The splits are
`.text` `80167178..80167280` and `.data` `801F5B40..801F5BD8`. `XYPAL` and the
`SISetSamplingRate` diagnostic string are named at their donor-proved addresses.

The four-byte `SamplingRate` at `.sbss` `8040A920..8040A924` was first left as
an assembly fallback (a separate `SISamplingRateBSS` unit) because the verifier
did not yet accept BSS. Under the layout policy it is now owned by the unit:
`SISamplingRate.c` defines `static u32 SamplingRate;`, the split carries
`.sbss 8040A920..8040A924`, and `verify_unit.py` proves the section by layout
(see [Promoting a unit's `.bss`/`.sbss`](#promoting-a-units-bsssbss)). The
full image remains byte-identical to retail (SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`).

Reproduction:

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/si/SISamplingRate --build
python3 tools/verify_fn.py SISetSamplingRate --build --strict-symbols
python3 tools/verify_fn.py SIRefreshSamplingRate --build --strict-symbols
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

## OSAudioSystem follow-up

The unchanged donor promotes both functions, 660 code bytes and 128 initialized
data bytes. The retail ranges are `.text` `8015D62C..8015D8C0` and `.data`
`801E2AC0..801E2B40`. The unstripped donor contains exactly these two functions;
neither requires removal. Both pass strict verification, and the complete unit
proof covers the DSP initialization block and every relocation.

The baseline was 27,920 matched code bytes, 169 functions and 2,492 matched
data bytes. The original probe found one MATCH and one CODE_SAME; the latter
exposed stale arena names. Retail getters and setters access the same respective
storage, and the following allocator uses the low arena, establishing the corrected
`OSGetArenaHi`, `OSGetArenaLo`, `OSSetArenaHi`, `OSSetArenaLo` and
`OSAllocFromArenaLo` identities at `8015D5E0..8015D62C`. Renaming contributes no
matching bytes. After promotion the report shows 28,580 code bytes, 171 functions
and 2,620 data bytes. All linked ranges and the whole image match retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. No BSS is promoted.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/os/OSAudioSystem --build
python3 tools/verify_fn.py __OSInitAudioSystem --build --strict-symbols
python3 tools/verify_fn.py __OSStopAudioSystem --build --strict-symbols
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported os/OSAudioSystem
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

## GXMisc follow-up

`gx/GXMisc` promotes 20 retained functions, 2,096 code bytes at
`.text` `8017F5F4..8017FE24`, with no initialized data. The baseline after
fstload was 29,156 matched code bytes, 173 functions and 2,744 data bytes;
the isolated result is 31,252 code bytes, 193 functions and 2,744 data bytes.
After integrating CARDDir, the combined result is 31,864 code bytes, 197
functions and 2,744 data bytes.
All retained functions pass strict verification and the complete text section,
including all relocations, passes the unit gate. The linked ranges and whole
image match retail, SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`.

The stripped donor probe retains exactly 20 functions: 12 MATCH and eight
UNVERIFIED, the latter requiring resolved fallback references. The source uses
`lbl_8040AC90`, `lbl_8040AC94`, `lbl_8040AC98` and `lbl_8040AC9C` for
callback and queue storage; their BSS remains assembly fallback. The donor's
`PPCSync` calls target `lbl_8015BF2C`, whose retail instructions are `sc; blr`;
the source references that exact label rather than asserting an unsupported
identity. `GXCompressZ16` and `GXDecompressZ16` are disabled in this unit:
their bodies and diagnostic strings are outside the retained retail unit.
The complete-unit proof and whole-image identity validate the retained unit;
`NO_ANCHOR` alone is not evidence of whole-binary absence. Other unretained
donor functions are removed by the existing strip step.

Instruction and relocation identity establish `__GXAbort`, `GXAbortFrame`,
`GXTokenInterruptHandler`, `GXSetDrawDoneCallback` and
`GXFinishInterruptHandler` at their corrected addresses. The earlier unrelated
anchors are replaced by address names. `OSInitThreadQueue` is the two-word
queue zeroing function at `80162514`; the retained initialization call and
its instructions support that identity. These renames claim no matching bytes.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/gx/GXMisc --build
# Run verify_fn.py --unit dolphin/gx/GXMisc --build --strict-symbols
# for each of the 20 functions reported by the stripped probe.
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported gx/GXMisc
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

## CARDDir follow-up

Starting from `e0e82fc` (OSAudioSystem), CARDDir promotes four functions and
612 code bytes, with no initialized data or BSS promotion. Its only allocated
section is `.text` `801777A4..80177A08`; the donor contains exactly those four
functions, so none is stripped. `WriteCallback` and `EraseCallback` match their
retail instruction bytes and the addresses loaded for the respective callbacks.
The complete-unit gate proves all 18 relocation offsets, types and targets.

The donor CARDBios `__CARDEraseSector` matches the 272-byte retail function at
`80176ECC` outside relocation fields; its call to `__CARDStart` lands at
`80176A38`. Likewise the donor CARDRdwr `__CARDWrite` matches the 104-byte
function at `8017C6EC` outside relocation fields, and its callback address and
write-page call land at `8017C604` and `80176D90`. The former symbol names at
these addresses were stale. The shared control block is referenced as the DTK
fallback `lbl_8030E320` (two 0x110-byte records); its BSS is not promoted.

Baseline was 28,580 code bytes, 171 functions and 2,620 data bytes; final is
29,192 code bytes, 175 functions and 2,620 data bytes. After integrating the
independently verified fstload import, the combined result is 29,768 code bytes,
177 functions and 2,744 data bytes, still byte-identical to retail. All four functions pass
`verify_fn.py --build --strict-symbols`, and `verify_unit.py dolphin/card/CARDDir
--build` passes. The unmodified upstream stripped probe gives one MATCH and three
UNVERIFIED because it retains the donor BSS name `__CARDBlock`; the imported
source uses the exact fallback name and passes strict and complete-unit gates.
The linked ranges and whole image match with zero differing bytes; SHA-1 is
`95cca08a19224775d1a8d6cc64601fb7d0080981`.

```sh
bash build.sh
python3 tools/matching_progress.py
python3 tools/verify_unit.py dolphin/card/CARDDir --build
for fn in __CARDGetDirBlock WriteCallback EraseCallback __CARDUpdateDir; do
    python3 tools/verify_fn.py "$fn" --build --strict-symbols
done
python3 tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported card/CARDDir
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units

```

## GXFifo

The retained `.text` range `0x8017DB08..0x8017E240` contains 16 functions,
1,848 bytes. The donor's `GXSetBreakPtCallback` is the 68-byte function at
`0x8017DFC4`: its callback load/store and interrupt calls identify it; the
previous `GXGetGPStatus` name was incorrect. Other donor functions outside the
retained range are stripped, including the donor's separate interrupt handlers,
which are inlined into `GXCPInterruptHandler` here.

The donor declarations for CPU and GP FIFO pointers, current thread, link flag,
overflow state, breakpoint callback and overflow count are externalized to the
existing DTK symbols `lbl_8040AC70`, `74`, `78`, `7C`, `80`, `84` and `88`
(full names carry the `lbl_8040AC` prefix). Their storage remains in fallback;
no BSS layout is inferred from zeroes. The unused `__GXCurrentBP` definition is
also externalized. `PPCSync` calls use the existing retail `lbl_8015BF2C` label.
The adapted source retains the donor compiler and strip step. All 111 retained
text relocations, including the BSS addresses, match their target offsets, kinds,
addends and resolved addresses under the complete-unit gate. The unadapted
`--strip-imported` probe fails because its retained code references donor-owned
BSS stripped from the target. Probing the adapted source instead reports all 16
retained functions MATCH; an unadapted discovery probe is not a promotion gate.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/gx/GXFifo --build
for fn in GXCPInterruptHandler GXInitFifoBase GXInitFifoPtrs GXInitFifoLimits \
    GXSetCPUFifo GXSetGPFifo GXSetBreakPtCallback __GXFifoInit \
    __GXFifoReadEnable __GXFifoReadDisable __GXFifoLink \
    __GXWriteFifoIntEnable __GXWriteFifoIntReset __GXCleanGPFifo \
    GXGetCPUFifo GXGetGPFifo; do
    python3 tools/verify_fn.py "$fn" --build --strict-symbols
done
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
```

The linked unit and full image match retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Relative to GXMisc baseline,
matching rises from 31,864 to 33,712 code bytes and 197 to 213 functions;
complete source-backed units rise from 22 to 23. No data bytes are promoted.

## Remaining Phase 1 work

With the OSMemory import, total matching was **26,576 code bytes / 155 functions /
2,340 matched data bytes**. Fifteen SDK units contributed 24,532 code bytes /
132 functions / 2,712 complete data bytes. The combined build retained the retail
SHA-1 and all complete-unit proofs.

OSCache is now promoted as the sixteenth unit; the report still shows 27,656 code
bytes / 167 functions because its functions were already credited, and complete
files rise from 16 to 17. SISamplingRate is the seventeenth SDK unit.
OSAudioSystem is the eighteenth, fstload the nineteenth, CARDDir the twentieth,
GXMisc the twenty-first, GXFifo the twenty-second and GXInit the twenty-third.
Current matching is **38,716 code bytes / 219 functions / 3,360 matched data
bytes**; the 23 SDK units contribute 35,592 code bytes / 184 functions / 3,732
complete data bytes. There are 24 complete source-backed files including the
non-SDK `nurand` unit. Remaining SDK ranges retain assembly fallback. No other probe row
is promoted by name or layout alone, and no unverified BSS or unsupported
relocation is accepted.

Continue library by library with the same full-unit and linked-range gates.
Do not infer that `NO_ANCHOR` proves absence, or that `MISMATCH` proves version
drift: stale names and dead stripping produced both kinds of misleading row.

## fstload import

On merged baseline `e0e82fc`, fstload contributed no promoted bytes. The donor
retains `cb` at `8016AE04` (216 bytes) and `__fstLoad` at `8016AEDC`
(360 bytes). Its compiled data and retail references establish `.data`
`801F66D8..801F6748` (112 bytes) and `.sdata` `8040A0D0..8040A0DC`
(12 bytes, including padding); the complete-unit check proves both sections.
The buffer, command block and three pointers remain in fallback BSS and are
not promoted. External references replace their donor definitions.

The existing local `block$16` symbol was renamed `fstloadBlock` without changing
its `802F49B8` address, `0x30` size or scope. DTK generates
`fstloadBlock_802F49B8`; both target and compiled objects use its matching
`R_PPC_ADDR16_HA` and `R_PPC_ADDR16_LO` relocations. Retail instructions at
`8016AF24` and `8016AF30` encode `lis r3,0x802f` and `addi r3,r3,0x49b8`,
forming that same address. Both functions pass strict verification, and the
complete unit passes all three sections. The linked unit contributes 576 code
bytes and 124 data bytes; the full image has zero differences and SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Progress is 29,156 code bytes,
173 functions and 2,744 data bytes (baseline 28,580 / 171 / 2,620).

```sh
bash build.sh
python3 tools/verify_fn.py cb --unit dolphin/dvd/fstload --build --strict-symbols
python3 tools/verify_fn.py __fstLoad --unit dolphin/dvd/fstload --build --strict-symbols
python3 tools/verify_unit.py dolphin/dvd/fstload --build
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```


## GXInit import

On baseline `157922f` GXInit adds 5,004 code bytes, six functions and 616 data
bytes. The retail GX release string is `Apr  5 2004 04:14:28`; the donor's
release time differs by two bytes. The unit owns `.text` `8017C77C..8017DB08`,
`.data` `801FAA80..801FACBC`, `.sdata` `8040A160..8040A164` and `.sdata2`
`8040B7D8..8040B800`. It holds `__GXDefaultTexRegionCallback` (`8017C77C`),
`__GXDefaultTlutRegionCallback` (`8017C878`), `__GXShutdown` (`8017C89C`),
`__GXInitRevisionBits`, `GXInit` and `__GXInitGX`. The earlier
`__GXDefaultTlutRegionCallback` name at `8017C754` belonged to a four-byte
neighbour and is now `fn_8017C754`.

Three causes blocked the earlier plateau, and none was a version difference:

- **Codegen.** Compiling the `FifoObj` and `gxData` externs gave 385
  instructions. MWCC hoists the address of a statically defined object into a
  callee-saved register at function entry and emits `addi r31,r4,...` directly.
  With an `extern` it emits `addi r0,...; mr r30,r0` instead. Explicit local,
  register and const-pointer variants of the extern cannot reproduce this. The
  donor-literal source (`static GXFifoObj FifoObj; static GXData gxData;` with
  `&FifoObj` at each use) matches all 384 instructions.
- **BSS ownership (superseded).** GXInit now owns `.bss 8030E560..8030EB90`
  under the layout policy and no longer uses `--external-bss`; the text below
  records the earlier fallback. `tools/strip_unused.py --external-bss` turned
  those `.bss` definitions into undefined references to the fallback symbols
  and empties the section. It accepts an object only when each name exists as a
  `.bss` entry in `symbols.txt`, its size fits, the whole section shares one
  address bias, no gap of eight bytes or more could hide another object and
  every relocation lands inside a named object. Otherwise it aborts. Only GXInit
  opts in (`SDKObject(..., external_bss=True)`), and BSS contents stay
  unpromoted. `FifoObj` became global (`FifoObj` instead of local
  `FifoObj_8030E560`) because a local symbol cannot be linked from another
  object; `lbl_8030E5E0` became `gxData`, both with unchanged address and size.
- **Link failure.** The linker exited 99 without a message because the source
  referenced `lbl_8040AC6C`, which is the local `resetFuncRegistered$145` that
  DTK emits as `resetFuncRegistered$145_8040AC6C`. Renaming it
  `GXResetFuncRegistered` (same address, size and scope) gives the C-compatible
  name `GXResetFuncRegistered_8040AC6C`, as for fstload.

`time` is a 64-bit `OSTime` read as two words in `__GXShutdown`. The retail
symbols split it into `lbl_8040AC60` and `lbl_8040AC64`, so a relocation to the
low word differed in addend (`+4`) from the target even though the address was
identical. Both symbols are now one eight-byte `lbl_8040AC60`; no address moved.

`verify_fn.py` compared the raw object bytes of sized pointer objects
(`__GXData` in `.sdata2`, `__GXVersion` in `.sdata`) with retail, where the
object holds zero plus a relocation. A complete-unit proof resolves each of those
relocations against the retail encoding, so a location it covers is now accepted
when the raw bytes disagree. Objects without a unit proof still fail on raw bytes.

The linked unit is identical to retail across all four ranges, the full image
has zero differences and the SHA-1 is `95cca08a19224775d1a8d6cc64601fb7d0080981`.
The six functions pass strict verification with no unverified references.

```sh
bash build.sh
for fn in __GXDefaultTexRegionCallback __GXDefaultTlutRegionCallback __GXShutdown \
    __GXInitRevisionBits GXInit __GXInitGX; do
  python3 tools/verify_fn.py $fn --unit dolphin/gx/GXInit --build --strict-symbols
done
python3 tools/verify_unit.py dolphin/gx/GXInit --build
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

## Batch 1: OS, CARD, db and pad units

Ten units were promoted together on baseline `e8c7276` (23 SDK units before,
33 after). The batch adds **13,068 code bytes and 60 functions**; matching
progress rises from 38,716 / 219 to 51,784 / 279 code bytes / functions and from
3,360 to 4,348 matched data bytes. The whole image keeps zero differences and
SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`. Every function that has a
named retail symbol passes `verify_fn.py --strict-symbols`, and every unit
passes `verify_unit.py` for all of its sections.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned data bytes |
|---|---|---:|---:|---:|
| `os/OSTime` | `801637C4..801638A0` | 4 | 220 | 96 |
| `os/OSInterrupt` | `80160130..8016099C` | 11 | 2,156 | 44 |
| `os/OSAlarm` | `8015CAD4..8015D2AC` | 9 | 2,008 | 16 |
| `os/OSArena` | `8015D5E0..8015D62C` | 5 | 76 | 4 |
| `os/OSError` | `8015E598..8015EC44` | 4 | 1,708 | 804 |
| `os/OSReset` | `80160F14..80161398` | 7 | 1,156 | 80 |
| `card/CARDBlock` | `801773A0..801777A4` | 6 | 1,028 | 0 |
| `card/CARDCheck` | `80177A08..80178944` | 7 | 3,900 | 0 |
| `db/db` | `80167280..8016736C` | 5 | 236 | 24 |
| `pad/Padclamp` | `8016E058..8016E29C` | 2 | 580 | 12 |

Evidence and limits:

- **Dead stripping.** The SN linker removed unreferenced donor functions and
  data. Retail `db` has no `DBStack`/`DBStackEnd`, and `Padclamp` has no
  `ClampCircle`, `PADClampCircle` or the `.sdata2` constants they pool, so those
  definitions were removed from the vendored source rather than left to pool
  unmatched data. `OSError`, `OSReset` and `OSTime` own static donor data
  (error table, reset state, `YearDays`) that retail retains.
- **Split boundaries.** Placeholder data symbols that straddled a boundary were
  split without moving any address (`db` string and its four padding bytes,
  `Padclamp` `ClampRegion` and the rest of its 36-byte neighbour, `OSReset`
  `801E3E66`, `OSTime` year tables). Splits must be 4-byte aligned, so alignment
  padding stays in fallback data where DTK requires it.
- **Misassigned names.** The CARD region names in `symbols.txt` were shifted;
  CARDBlock/CARDCheck were identified by function size sequence and strict
  comparison. `fn_801776F8` is the real `__CARDUpdateFatBlock`; the thunk at
  `801531DC` is a branch island to a 64-bit arithmetic shift-right helper and is
  named `__shr2i`. `fn_8016E188` is `PADClamp`. Local names that collided
  (`WriteCallback`, `EraseCallback`) became `CARDBlockWriteCallback` and
  `CARDBlockEraseCallback`.
- **BSS and small data.** Static BSS stays in fallback. OSError and OSReset use
  `external_bss=True`; CARD units refer to the shared card control block as
  `lbl_8030E320`; `db` declares `__DBInterface` and `DBVerbose` as externs for
  retail's `.sbss` symbols. BSS was not promoted at this stage; see
  [Promoting a unit's `.bss`/`.sbss`](#promoting-a-units-bsssbss).
- **Verifier change.** DTK emits a relocation for a branch back to its own
  function entry, while MWCC resolves it in the object. `verify_fn.py` accepts
  the pair as equal only when the assembled branch (same opcode, word-aligned)
  lands on the relocation's target; `verify_unit.py` applies the same rule per
  section and requires every other relocation to match exactly. Tests cover the
  accepted case and a displacement that must still mismatch.

Not promoted from this batch's research: `OSContext` (matched 52 of 54
instructions in `OSLoadContext`; the donor's interrupt endpoint
`__RAS_OSDisableInterrupts_end` is `OSDisableInterrupts+0xc` in retail, and its
string data ranges are unproven), `ai/ai` (the candidate range is
`8016FC28..801704C8`; its statics live in `.sbss` `8040AAA0` onward, which the
BSS externalizer does not cover, and `__AIVersion` sits in `.data` `801F6B88`
with its pointer in `.sdata` `8040A118`), and the AX units.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/db/db --build
python3 tools/verify_fn.py DBInit --unit dolphin/db/db --build --strict-symbols
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

## Batch 2: DVD, AI, AR, AX and EXI units

Nine units were promoted on baseline `47c416e` (33 SDK units before, 42
after). The batch adds **12,968 code bytes and 47 functions**; matching progress
rises from 51,784 / 279 to 64,752 / 326 code bytes / functions and from 4,348 to
5,624 matched data bytes. The whole image keeps zero differences and SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Every function passes
`verify_fn.py --strict-symbols` and every unit passes `verify_unit.py` for all
of its sections. `python3 -m unittest discover -s tests` runs 107 tests (2 skipped).

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned data bytes |
|---|---|---:|---:|---:|
| `dvd/dvdqueue` | `8016A850..8016AA48` | 5 | 504 | 0 |
| `dvd/dvderror` | `8016AA48..8016ABE0` | 2 | 408 | 72 |
| `dvd/dvdidutils` | `8016ABE0..8016ACD8` | 1 | 248 | 0 |
| `dvd/dvdFatal` | `8016ACD8..8016AE04` | 3 | 300 | 984 |
| `exi/EXIUart` | `80165360..80165918` | 4 | 1,464 | 0 |
| `ai/ai` | `8016FBE4..801704C8` | 19 | 2,276 | 72 |
| `ar/ar` | `801704C8..80171F3C` | 7 | 6,772 | 72 |
| `ar/arq` | `80171F3C..801722D8` | 5 | 924 | 76 |
| `ax/AXProf` | `801753CC..80175414` | 1 | 72 | 0 |

Evidence and limits:

- **Misassigned names.** `symbols.txt` names in this region were shifted.
  `AIRegisterDMACallback`, `AIRegisterStreamCallback`, `AIResetStreamSampleCount`
  and `AISetStreamTrigger` sat at game addresses `80143828..80144A88` and are now
  `fn_` names; the real AI code starts at `8016FBE4`. In AR, retail `ARInit` is at
  `801705FC` (not `ARAlloc`), `801706C0` is `ARGetBaseAddress` and `801706C8` is
  `ARGetSize`; the stray `ARGetBaseAddress` at `8013F540` became `fn_8013F540`.
  `fn_80165360` is `ProbeBarnacle`. In dvdFatal and dvdidutils the retained
  `fn_` functions and string tables (`Japanese`, `English`, `Europe`) were
  renamed to the donor names, because `strip_unused.py` removes any donor symbol
  absent from the DTK target object. `lbl_801560A4` is `strncmp`.
- **Dead stripping.** Retail dropped `strnlen` and `DVDGenerateDiskID`
  (dvdidutils), the queue inspection and dump functions (dvdqueue) and two AXProf
  functions, so they were removed from the vendored source. The unreferenced AI,
  AR and ARQ API functions need no edit because `strip_unused.py` removes them.
- **Statics and BSS.** The AI, AR, ARQ and EXIUart statics live in `.sbss`. The
  sources declare them `extern`, and `symbols.txt` names the retail slots after
  the donor statics (`__AID_Callback`, `bound_32KHz`, `__AR_Size`,
  `__ARQRequestPendingLo`, `Chan`, ...). The 8-byte `OSTime` statics replace the
  paired 4-byte placeholders. `WaitingQueue` in dvdqueue is externalized BSS.
  BSS was not promoted at this stage; see
  [Promoting a unit's `.bss`/`.sbss`](#promoting-a-units-bsssbss).
- **Version strings.** `__AIVersion`, `__ARVersion` and `__ARQVersion` are
  `.sdata` pointers to `.data` strings. Each pointer is padded to 8 bytes in
  retail; the 4-byte padding stays in fallback because DTK splits must be
  4-byte aligned. The ARQ string is 0x45 bytes, so its split ends at the aligned
  address `801F6C60` and the verifier accepts the short all-zero tail.
- **Shared call label.** AR calls the retail synchronization stub through
  `lbl_8015BF2C` (`#define PPCSync lbl_8015BF2C`), as GXFifo and GXInit do.
  The private `__ar.h` lives in `src/dolphin/os`, next to `__dvd.h` and `__os.h`.
- **Verifier change.** `verify_unit.py` compared the relocation addend as well
  as the resolved address, so a section symbol plus offset (the compiler's form)
  was rejected against a named symbol with a zero addend (DTK's form). It now
  compares kind, resolved address and mask only; the retail word is still
  checked independently. A test covers an accepted and a rejected addend.

Investigated but not promoted:

- `dvdfs`, `dvdlow`, `dvd`: candidates were wired at `80167780..801680C0`,
  `8016B044..8016BE88` and `801680C0..8016A850`. Stripping stopped at unmapped
  data: `dvdfs` needs the retail address of `__DVDLongFileNameFlag` (a text
  symbol currently holds that name), `dvdlow` needs the `AlarmForWA` storage and
  `dvd` its `BB2` BSS. Probe rows were mostly `UNVERIFIED`, not strict results.
- `EXIBios` (`80163984..80165360`): symbol names are correct, but `EXIInit` is
  `fn_80164BCC` and reads `lbl_8040A008`, a 0x38-byte `.sdata` block that mixes
  the EXI version pointer with the `EXIGetTypeString` strings, and `lbl_8040A850`
  shared with the OS code. Ownership of that block must be settled first.
- `AX`, `AXAlloc`, `AXAux`, `AXCL`, `AXOut`, `AXSPB`, `AXVPB`: retail order is
  suspected to be AX, AXAlloc, AXAux, AXCL, AXOut, AXSPB, AXVPB, AXProf; the
  `__AXGetStackHead` name at `801722FC` is probably `AXInitEx` territory. None
  was wired.
- `OSContext` (52 of 54 `OSLoadContext` instructions; the next hypothesis is a
  donor-side reference to `OSDisableInterrupts+0xc`), `SIBios`, `Pad`, `mtx`,
  `mtxvec`, `vi` and the CARD units were only probed.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/ai/ai --build
python3 tools/verify_fn.py AIInit --unit dolphin/ai/ai --build --strict-symbols
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```


## Batch 3: SI, OS context, CARD format and DVD low level

Four more SDK units are promoted from baseline `e8139f4`: 46 SDK units total.
The batch adds **14,120 code bytes and 61 functions**, taking measured matching
progress from 64,752 / 326 to **78,872 code bytes / 387 functions** (5.024936%
code). The report records 6,108 matched data bytes. The new unit sections own
776 initialized data bytes; objdiff's named-data count is separate from the
complete-section proof (SIBios reports 20 of 300 data bytes as named matches,
while all 300 pass that proof).

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned data bytes |
|---|---|---:|---:|---:|
| `os/OSContext` | `8015DCF8..8015E598` | 14 | 2,208 | 472 |
| `si/SIBios` | `80165918..80167178` | 24 | 6,240 | 300 |
| `card/CARDFormat` | `80179400..80179BE4` | 3 | 2,020 | 0 |
| `dvd/dvdlow` | `8016B044..8016BE88` | 20 | 3,652 | 4 |

The final clean rebuild has **zero differing bytes**, including the DOL header,
and SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`. Each retained function
passes the strict symbol gate, and all four units pass the complete-section
proof. No BSS is promoted. The portable suite passes 107 tests (2 skipped).

Integration details:

- **OSContext:** the donor source needs no edits. The restartable interrupt
  sequence uses labels `__RAS_OSDisableInterrupts_begin` at `80160130` and
  `__RAS_OSDisableInterrupts_end` at `8016013C`. Both are aliases inside the
  existing interrupt function, not new functions or moved boundaries.
- **SIBios:** the alarm callback at `80166878` is named `SIAlarmHandler` to
  avoid the other SDK callbacks. Function-local statics are hoisted so BSS can
  be externalized. The two `cmdTypeAndStatus` slots retain distinct names and
  addresses. The version string, `Si` and `Type` reside in `.data`; the version
  pointer and short strings occupy `.sdata` `8040A040..8040A054`.
- **CARDFormat:** the shared control block uses `lbl_8030E320`, as CARDDir does.
  Retail dropped `__CARDFormatRegion` and `CARDFormat`; the retained async
  functions and callback match without instruction changes.
- **dvdlow:** `AlarmForWA` is at `802F4A28`; the other alarms and command/buffer
  objects are mapped to fallback BSS. Small BSS statics are externs, including
  the 8-byte time values. `FirstRead` owns only `8040A0E0..8040A0E4`; alignment
  padding stays fallback. An explicit byte symbol at `8040A0DB` bounds the
  preceding fstload tail without changing its split. The donor call to
  `OSInitAlarm` resolves to retail `__OSInitAlarm`. The alarm callback is named
  `DVDAlarmHandler` to keep strict symbol lookup unambiguous. The old
  `DVDLowSeek` label at `801681A4` was misassigned; the verified implementation
  is `8016B880`.

The remaining Phase 1 candidates from batch 2 still need individual import
work: dvdfs, dvd, EXIBios, AX families, Pad, mtx, vi and remaining CARD/OS
units. This batch makes no promotion claims for those probes. For EXIBios the
next step remains identifying ownership of the shared `.sdata` block; for dvd
it is mapping the BSS references before the strict gate.

Reproduce the four complete-unit proofs with:

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/os/OSContext --build
python3 tools/verify_unit.py dolphin/si/SIBios --build
python3 tools/verify_unit.py dolphin/card/CARDFormat --build
python3 tools/verify_unit.py dolphin/dvd/dvdlow --build
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
python3 -m unittest discover -s tests
```

For each function symbol in each split target object, run
`python3 tools/verify_fn.py NAME --unit dolphin/LIB/UNIT --build --strict-symbols`.
The function counts above enumerate the expected strict checks (61 total).

## Batch 4: matrix, OS allocation, AX, DSP and CARD units

Fourteen units are promoted from baseline `77b25c2`, bringing the SDK total to
60 units. Measured progress increases from **80,244 bytes / 397 functions** to
**89,952 bytes / 451 functions**, a gain of **9,708 bytes / 54 functions**.
These are source matches; symbol renames are not counted as progress.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned data bytes |
|---|---|---:|---:|---:|
| `mtx/mtx` | `8016736C..80167608` | 6 | 668 | 16 |
| `mtx/mtxvec` | `80167608..801676E8` | 2 | 224 | 0 |
| `mtx/mtx44` | `801676E8..80167780` | 1 | 152 | 16 |
| `os/OSAlloc` | `8015D2AC..8015D5E0` | 7 | 820 | 4 |
| `os/OSLink` | `8016099C..801609B4` | 1 | 24 | 0 |
| `os/OSReboot` | `80160E90..80160F14` | 2 | 132 | 0 |
| `ax/AX` | `801722D8..8017237C` | 3 | 164 | 72 |
| `dsp/dsp` | `80175414..801756A4` | 9 | 656 | 132 |
| `dsp/dsp_task` | `801756F4..80175F78` | 5 | 2,180 | 320 |
| `card/CARDRead` | `8017A520..8017A94C` | 3 | 1,068 | 0 |
| `card/CARDWrite` | `8017A94C..8017AC80` | 3 | 820 | 0 |
| `card/CARDDelete` | `8017AC80..8017AE34` | 2 | 436 | 0 |
| `card/CARDOpen` | `80179BE4..8017A1D0` | 8 | 1,516 | 0 |
| `card/CARDCreate` | `8017A1D0..8017A520` | 2 | 848 | 0 |

Every retained function passes `verify_fn.py --build --strict-symbols`, and all
14 units pass `verify_unit.py --build`, including complete sections and
relocations. `bash build.sh` succeeds and the complete linked DOL has zero
byte differences, including its header: SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. The portable suite passes 107 tests
(2 skipped); all 43 verifier regression tests pass with
`VERIFY_FN_REQUIRE_RETAIL=1`.

The pinned dolsdk2004 donor and existing RE4 compiler/strip workflow are unchanged.
OSAlloc keeps inline list helpers; matrix identity keeps a stripped C helper to
preserve the constant pool order. DSP task code keeps a stripped helper to retain
its pooled string. Other dead functions are removed. OS/DSP globals remain
externs backed by existing assembly storage. CARD shares `lbl_8030E320`, uses
fallback names for the permission mask and libc references, and gives callbacks
unique names. No BSS is promoted. Original unit boundaries and the strict oracle
are preserved; newly owned sections subdivide existing fallback ranges only.

Strict verification corrects earlier misidentifications of OSDestroyHeap,
OSGetSaveRegion, matrix transforms, AX initialization/quit and CARD open/create
functions. AX call targets receive the names required by their verified callers;
their implementations remain fallback.

Reproduce with `bash build.sh`, then for each unit above run
`python3 tools/verify_unit.py dolphin/LIB/UNIT --build`. For every function in
its target object run
`python3 tools/verify_fn.py NAME --unit dolphin/LIB/UNIT --build --strict-symbols`.
Finally run `python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol`
and `python3 tools/matching_progress.py`.

Remaining imports include dvdfs, dvd, EXIBios, the other AX units, Pad, vi and
remaining CARD/OS units. Their unfinished experiments are preserved separately
at `wip/sdk-family-imports-fb9c5ca`; none are enabled by this batch. EXIBios still
needs shared small-data ownership resolved, and dvd needs its BSS references mapped.

## Batch 5: AX voice allocation and auxiliary buses

Two units are promoted from baseline `97e792b`, bringing the SDK total to 62.
Measured progress rises from **89,952 bytes / 451 functions** to
**92,336 bytes / 474 functions**, a gain of **2,384 bytes / 23 functions**.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned data bytes |
|---|---|---:|---:|---:|
| `ax/AXAlloc` | `8017237C..80172844` | 10 | 1,224 | 0 |
| `ax/AXAux` | `80172844..80172CCC` | 13 | 1,160 | 0 |

All 23 retained functions pass `verify_fn.py --build --strict-symbols`; both units
pass `verify_unit.py --build`, and the linked image has zero differing bytes.
Dead donor functions (`__AXCheckStacks`, `__AXPopFreeStack`, `__AXPushStackHead`,
`__AXPopStackFromBottom`, `AXSetVoicePriority`, `AXRegisterAux{A,B}Callback`) and the
inlined `__AXInitVoiceStacks` are stripped.

The donors keep their file-scope statics. Declaring them `extern` changes code
generation (the compiler reloads values around the callback), so the statics stay
`static` and `tools/strip_unused.py --external-bss` now externalizes `.sbss` as well
as `.bss`: each static is replaced by a reference to the `symbols.txt` object of the
same name, and the emptied section drops to alignment 1 so it cannot pad the retail
`.sbss` tail. The 17 AX bss/sbss labels were renamed to the donor names
(`__AXStackHead`, `__AXBufferAuxA`, `__AXClMode`, and so on); storage stays in
assembly fallback. `__AXSetPBDefault` is named from its `MATCH` row in the AXVPB probe.

Remaining AX units (AXSPB, AXVPB) still carry `MISMATCH` or `CODE_SAME` rows that
need comparing with retail before they can be promoted.

## Batch 6: AX command list and output, OS reset, messages, semaphores, card read/write

Six units are promoted from baseline `5ead3b3`, bringing the SDK total to 68.
Measured progress rises from **92,336 bytes / 474 functions** to
**98,620 bytes / 502 functions**, a gain of **6,284 bytes / 28 functions**.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned data bytes |
|---|---|---:|---:|---:|
| `ax/AXCL` | `80172CCC..80173404` | 7 | 1,848 | 0 |
| `ax/AXOut` | `80173404..80173C10` | 9 | 2,060 | 0 |
| `os/OSMessage` | `801609B4..80160BB8` | 3 | 516 | 0 |
| `os/OSSemaphore` | `8016220C..80162334` | 3 | 296 | 0 |
| `os/OSResetSW` | `80161398..80161724` | 2 | 908 | 0 |
| `card/CARDRdwr` | `8017C4C4..8017C754` | 4 | 656 | 0 |

All 28 retained functions pass `verify_unit.py --build --functions` (strict symbols),
and the linked image has zero differing bytes. All six sources are unmodified donors.

Hand work, each backed by relocation or size evidence rather than a score:

- **AXCL.** The probe's layout walk had given the 8-byte slot of the retail
  `AXSetCompressor` (a store) to the donor's 8-byte `AXGetMode` (a load). Layout
  anchoring now also compares code for functions of at most 16 bytes, so
  `AXGetMode` is the dead-stripped function.
- **AXSPB/AXOut neighbours.** Three older names were one slot off: `0x80173BCC` is
  `AXRegisterCallback` (68 bytes), `0x80173C10` is `__AXGetStudio` (12 bytes, called
  by `__AXNextFrame`) and `0x80173C1C` is `__AXPrintStudio` (1,016 bytes). A game
  function at `0x80140504` had been named `__AXOutNewFrame` and is `fn_80140504` again;
  the real one is `0x80173404`.
- **AXOut `.bss`/`.sbss`.** `lbl_802F8060` (0x4A00) was three donor arrays and is now
  `__AXOutBuffer`, `__AXOutSBuffer` and `__AXDramImage`. The DSP task is 0x50 bytes,
  `__AXLocalProfile` follows it at `0x802FCAB0` (0x38) and the `0x38` bytes after
  it belong to the next unit (`lbl_802FCAE8`). `__AXOsTime` is one 8-byte object.
- **OSResetSW.** `OSTime` statics (`__OSStartTime`, `HoldUp`, `HoldDown`) were split by
  DTK into two 4-byte `lbl_` entries because the code only touches the halves; each
  pair is now one 8-byte object.
- **CARDRdwr.** The retail callees were named `CARDInit` and `__CARDSetDiskID`; by
  size (0x134, 0x13C) and call position they are `__CARDReadSegment` and
  `__CARDWritePage`. The functions previously carrying those names (0x2A0 and
  0x110 bytes) are the donor's static `Retry` and `UnlockedCallback`. The other
  `CARDBios` names are probably shifted the same way; re-anchor them with the
  CARDBios batch. `lbl_8030E320` became `__CARDBlock`, and the importer now rewrites
  that name in the already-imported card sources.

Parked: `si/SISteeringAuto` is not in retail. Its `SamplingHandler` and
`SISetSteeringSamplingCallback` rows are the existing `pad/Pad` functions (retail
`SamplingHandler`, `PADSetSamplingCallback`); import `pad/Pad` instead.
`os/OSFatal`, `card/CARDUnlock` and `card/CARDMount` own `.data` or `.sdata2` and need
hand-written split ranges; CARDMount's include failure (`os/__os.h`) was a resolver bug
and is fixed, leaving only its `.data 0x40`. `ax/AXVPB`, `card/CARDBios` and
`os/OSThread` are blocked on initialised data too.

## Import workflow fixes

Retrospective items from Batch 6, fixed in the tools rather than by hand:

| Item | Result |
|---|---|
| Name conflicts | `--accept-name-conflicts`, evidence by code, rejected list honoured, chains and demotions (above). The CARDBios shift chain (`__CARDSyncCallback`, `__CARDExtHandler`, `__CARDExiHandler`, `__CARDTxHandler`, `__CARDStart`, `__CARDSync`) is found by a dry run; `OnReset` is blocked by the other `OnReset` inside `os/OSMemory`. AXSPB has no remaining conflicts and AXVPB shows none. VI, OS, DVD, OSRtc and dvdfs names show the same off-by-one shifts in the probe diff and are for their own batches. |
| `.bss` granularity | Half-label merge and exact-tiling split (above). |
| `strip_unused --external-bss` | Drops statics with no references from retained code and no symbol, keeping the constant-bias, overlap, hidden-gap and alignment checks. |
| `gx/GXDisplayList` | Imports and proves with the new drop rule. |
| Includes | `-i src/dolphin` root and donor lookup. |
| Failure output | Key lines first. |
| `SISamplingRateBSS.c` | The "Missing configuration" message was a deliberate sbss-only fallback split with no `Object`; it was later folded back into `SISamplingRate` (`.sbss 8040A920..8040A924`) once BSS became layout-verifiable. |

`sdk_probe.anchor` now prefers identical code over a name that sits on a different
function and never lets two donor functions claim one retail function, which is what let
shifted names mislead earlier runs.

Re-importing AXCL, AXOut, OSMessage, OSSemaphore, OSResetSW and CARDRdwr from the
Batch 6 baseline with `--apply --accept-name-conflicts` reproduces them with zero
differing bytes. The symbol diff against the hand edits is the AXSPB neighbour
`0x80173BCC`, which the tool leaves as `fn_80173BCC` where the hand edit used
`AXRegisterCallback`.

### GXDisplayList

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned data bytes |
|---|---|---:|---:|---:|
| `gx/GXDisplayList` | `80183370..801833E0` | 1 | 112 | 0 |

Measured progress rises from 98,620 bytes / 502 functions to **98,732 bytes / 503
functions**. The three `.bss` statics belong to dead-stripped functions and have no retail
symbols, so `externalize_bss` drops them.

## PPCArch import

On baseline `553ba7a`, unchanged donor `base/PPCArch.c` promotes 22 functions
and 276 code bytes at `0x8015BEF4..0x8015C008`, with no owned data or BSS.
The SDK total is now 70 units (the previous strategy count omitted the already
promoted GXDisplayList). Matching rises from 98,732 bytes / 503 functions
to 99,008 bytes / 525 functions; matched data remains 6,220 bytes.

The first import failed the complete-section gate because `PPCMthid0` at
`0x8015BF0C` and `PPCSync` at `0x8015BF2C` were labels, so dead stripping
omitted their eight-byte bodies. Retail instructions (`mtspr HID0,r3; blr`
and `sc; blr`) and their calls match the donor. Both are now function symbols;
existing AR/GX sources refer to `PPCSync` directly. Neither split boundaries
nor compiler flags changed after the initial import plan. The 16 newly classified
code bytes also increase the report's total code denominator to 1,569,628 bytes
and its function denominator to 4,152.

Validation:

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/base/PPCArch --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

The complete section and relocations match, all 22 functions strictly match,
and the whole linked image has zero differing bytes. The baseline promoted
zero PPCArch bytes; no donor edits were needed. Remaining Phase 1 units still
need their own import and promotion proof.

## AX voice parameters and compressor table

Baseline `d33e2d3` has 70 promoted SDK units, 99,008 matched code bytes and
525 matched functions. This batch promotes two unchanged donor units, bringing
the SDK total to 72 and adding 4,852 code bytes / 16 functions. Total matching
is 103,860 code bytes / 541 functions and 12,996 matched data bytes (previously
6,220). Neither unit had promoted bytes on the baseline.

| Unit (under `dolphin/`) | Retail range, end exclusive | Functions | Code bytes | Owned initialized data bytes |
|---|---|---:|---:|---:|
| `ax/AXVPB` | `.text 801740D8..801753CC` | 16 | 4,852 | 272 |
| `ax/AXComp` | `.data 801F6D80..801F87C0` | 0 | 0 | 6,720 |

AXVPB owns `.data 801F6CA8..801F6D80` (three cycle-cost tables plus four
alignment bytes) and `.sdata2 8040B7A0..8040B7D8`. The automatic importer
refuses initialized data; these ranges were added manually from the complete
tables and retained constant pool. The unstripped donor has 32 bytes of weak
inline math constants before that pool; the existing dead-strip rules remove
them. The preceding retail `8040B780..8040B7A0` constants remain fallback,
with no ownership claim. No existing split boundary or compiler flag changes.

The four donor BSS arrays tile `802FCB20..8030E320` at offsets `0`, `0x3D00`,
`0x4D00` and `0x8D00`, with sizes `0x3D00`, `0x1000`, `0x4000` and `0x8B00`.
They replace two coarse fallback symbols; code and relocations prove their
addresses. Their storage and the three small counters stay in assembly fallback.
`__AXServiceVPB` and `AXSetVoiceMix` are named by their strict matches.

All 16 functions pass strict verification, and all four owned sections across
the two units pass the complete-section and relocation gate. The whole linked
DOL remains byte-identical to retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Objdiff credits AXVPB with only
56 matched data bytes, although the full-unit gate proves all 272 initialized
bytes; its tables and padding account for the 216-byte difference. The report's
data denominator falls by four bytes when the padding becomes a separate label.
AXComp contributes all 6,720 matched table bytes.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/ax/AXVPB --build --functions
python3 tools/verify_unit.py dolphin/ax/AXComp --build
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

AXComp is data-only, so its gate intentionally omits `--functions` (that option
requires at least one function). AXSPB was left unpromoted here because its
retail studio address `802FCAE0` aliased `__AXLocalProfile+0x30`; the
[AX studio batch](#ax-studio-batch) resolves the layout.

## CARD mounting, serial number and unlock batch

From the AX batch (`43e361d`, merged as `f29cfc2`), three unchanged donors
promote 14 functions and 7,648 code bytes. SDK units total 75. Matching reaches
111,508 code bytes / 555 functions and 13,412 matched data bytes; the baseline
was 103,860 / 541 and 12,996 data bytes. No bytes in these units were promoted
on the baseline.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned initialized data bytes |
|---|---|---:|---:|---:|
| `card/CARDMount` | `80178944..80179400` | 7 | 2,748 | 64 |
| `card/CARDNet` | `8017B1A0..8017B264` | 1 | 196 | 4 |
| `card/CARDUnlock` | `8017B264..8017C4C4` | 6 | 4,704 | 360 |

Manual data ownership follows unique full donor-byte matches for CARDMount's
sector/latency tables at `801FA8D8..801FA918` and CARDUnlock's 352-byte
`CardData` at `801FA920..801FAA80`. The eight intervening alignment bytes
remain fallback. CARDNet owns `.sdata 8040A150..8040A154`: vendor ID, permission
mask and one zero alignment byte. CARDUnlock owns `.sdata 8040A158..8040A160`: its
four-byte `next` state plus four zero alignment bytes. Its relocation identifies
this small-data address independently of the many other retail words containing
one. All owned ranges pass the complete section and relocation gate; no BSS
is promoted, no existing unit boundaries move, and compiler pins are unchanged.
The added `-i src/dolphin` resolves the donor's `os/__os.h` include.

Code-identical CARDBios callees correct the shifted handler names:
`__CARDExtHandler` at `80175F7C`, `__CARDExiHandler` at `80176054`,
`__CARDTxHandler` at `8017616C` and `__CARDUnlockedHandler` at `80176214`.
These are supporting names, not extra matched functions. The donor header's
existing typed declarations are retained: rewriting its sync callback prototype
to the external-interrupt handler creates a type conflict. Dead-stripped sync
wrappers in CARDCheck/CARDStat likewise keep their donor names. CARDOpen and
CARDCreate now refer directly to `__CARDPermMask`.

The report credits 416 new data bytes: 64 in CARDMount and 352 in CARDUnlock.
The full-unit gate additionally proves their small data (12 bytes including
padding), which objdiff does not credit. The data denominator decreases by four
bytes when the permission-mask placeholder is separated from fallback padding.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/card/CARDMount --build --functions
python3 tools/verify_unit.py dolphin/card/CARDNet --build --functions
python3 tools/verify_unit.py dolphin/card/CARDUnlock --build --functions
python3 tools/verify_unit.py dolphin/card/CARDOpen --build --functions
python3 tools/verify_unit.py dolphin/card/CARDCreate --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

All 14 imported functions strictly match; all seven owned sections pass. The
whole DOL remains byte-identical to retail with SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Portable checks and the 169-test
suite with `VERIFY_FN_REQUIRE_RETAIL=1` pass (one skipped). CARDBios and
CARDProgram remain fallback; this batch does not claim their promotion.

## OS SRAM, fonts and fatal-screen batch

From the CARD batch (`ecd95bf`, merged as `a371f5c`), three unchanged donors
promote 26 functions and 8,964 code bytes. SDK units total 78. Matching reaches
120,472 code bytes / 581 functions and 13,488 matched data bytes; the baseline
was 111,508 / 555 and 13,412 data bytes. None of these units had promoted bytes
on the baseline. DTK's report denominator is now 1,569,632 code bytes / 4,153
functions after reanalysis with the corrected supporting names (previously
1,569,628 / 4,152); only the proved unit bytes count as matching progress.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned initialized data bytes |
|---|---|---:|---:|---:|
| `os/OSRtc` | `80161724..8016220C` | 15 | 2,792 | 0 |
| `os/OSFont` | `8015F5A4..80160130` | 7 | 2,956 | 2,848 |
| `os/OSFatal` | `8018BE6C..8018CAFC` | 4 | 3,216 | 68 |

Manual initialized-data splits follow unique complete donor-byte matches:
OSFont's tables at `801E3278..801E3D88` (including six zero alignment bytes)
and its constant at `8040B708..8040B710`; OSFatal's constant pool at
`8040BCF0..8040BD30`. Small-data references establish OSFont's `FontEncode`
at `80409FF8..8040A000` (two bytes plus six zero alignment bytes) and
OSFatal's four-byte message format at `8040A2A8..8040A2AC`. The two four-byte
labels at `8040B708` and `8040B70C` are one eight-byte donor constant. The
16 bytes after OSFatal's last double stay fallback, with no ownership claim.

OSRtc's existing local `Scb` fallback symbol becomes global so `--external-bss`
can resolve the donor name. OSFont's four `.sbss` statics are named and the
combined Sjis/fixed-pitch placeholder is split into two four-byte objects.
OSFatal's existing `8030FBA8..8030FE80` fallback block is split into
`FatalParam` (12 bytes), four alignment bytes and `FatalContext` (712 bytes).
Their addresses follow the retail base-register accesses and donor layout.
All BSS remains fallback; it is not promoted.

Strict code identity corrects the SRAM callback/initializer names and the
shifted VI retrace, callback and initialization names required by OSFatal.
VIConfigure, VIConfigurePan and VIGetRetraceCount are supporting callee names,
not additional promoted functions. `strlen` replaces the old call label;
CARDOpen and CARDCreate drop their label alias and still strictly match.
No compiler pins, oracle checks or existing unit boundaries change.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/os/OSRtc --build --functions
python3 tools/verify_unit.py dolphin/os/OSFont --build --functions
python3 tools/verify_unit.py dolphin/os/OSFatal --build --functions
python3 tools/verify_unit.py dolphin/card/CARDOpen --build --functions
python3 tools/verify_unit.py dolphin/card/CARDCreate --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

All 26 imported functions and eight complete sections pass. The full-unit
gate proves 2,916 initialized bytes, although objdiff credits only 76 new data
bytes (OSFont's eight-byte constant and OSFatal's 68 bytes). The whole DOL
remains byte-identical to retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Portable workflow checks and
169 tests with `VERIFY_FN_REQUIRE_RETAIL=1` pass (one skipped). OSExec,
OSThread and the VI unit still need their own promotion proof.

## AX studio batch

From `7f260a4`, the unchanged AXSPB donor promotes 5 functions and 1,224 code
bytes. SDK units total 79. Matching reaches 121,696 code bytes / 586 functions;
the baseline was 120,472 / 581. Data stays at 13,488 matched bytes.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned initialized data bytes |
|---|---|---:|---:|---:|
| `ax/AXSPB` | `80173C10..801740D8` | 5 | 1,224 | 0 |

`__AXGetStudio` and `__AXPrintStudio` address the studio at `802FCAE0`, and
retail `__AXOutNewFrame` stores `axNumVoices` to the same address through its
`__AXOutBuffer` base (`r31+0x4A80`). AXOut has no relocation to the profile's
tail, so in the linked image `__AXLocalProfile` owns only `0x30` bytes before the
32-byte-aligned `__AXStudio` (`802FCAE0..802FCB20`, which ends at `__AXPB`). The
symbols now say so: `__AXLocalProfile` is `0x30` bytes and `__AXStudio` replaces
`lbl_802FCAE8`. To keep AXOut's compiled `.bss` object within that size for
`--external-bss`, AXOut and `ax.h` declare `__AXLocalProfile` as `u64[6]` and
reach the fields through an `AXPROFILE*` cast; AXOut still matches all nine
functions. The nine AXSPB `.sbss` accumulators take their donor names. All BSS
remains fallback.

SISteeringAuto is not imported: its two anchors (`8016FAB4`, `8016FB14`) sit
inside the Pad range and are Pad's own `SamplingHandler` and
`PADSetSamplingCallback`, code-identical to the steering copies. The one-function
anchors for `axfx/axfx` (`8002765C`, name only) and `mtx/mtxstack` (`8004ADB4`, 8
bytes) lie in game code and are treated the same way.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/ax/AXSPB --build --functions
python3 tools/verify_unit.py dolphin/ax/AXOut --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

Both units pass one complete section and every function strictly. The whole DOL
remains byte-identical to retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Portable workflow checks and 174
tests with `VERIFY_FN_REQUIRE_RETAIL=1` pass (one skipped).

## DVD filesystem and drive state machine batch

From `5e3b54a`, dvdfs and dvd promote 58 functions and 12,496 `.text` bytes. The
objdiff report counts 56 functions and 12,112 bytes of new matched code. SDK units
total 81. Matching reaches 133,808 code bytes / 642 functions; the baseline was
121,696 / 586. Data reaches 13,888 matched bytes, up from 13,488.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned initialized data bytes |
|---|---|---:|---:|---:|
| `dvd/dvdfs` | `80167780..801680C8` | 11 | 2,376 | 1,016 |
| `dvd/dvd` | `801680C8..8016A850` | 47 | 10,120 | 424 |

The data ranges came from compiling each donor and matching its `.data` prefix in the
DOL: dvdfs owns `.data 801F5C00..801F5FF0` and `.sdata 8040A068..8040A070`, and dvd owns
`.data 801F5FF0..801F6178` and `.sdata 8040A070..8040A090`.

- **Names.** The retail label at `8015417C` is ProDG libc `tolower`, which dvdfs
  calls. The old dvdfs names were shifted by one function (`DVDReadAsyncPrio`,
  `DVDOpenDir`, `DVDCloseDir`), and the lone `blr` at `801680C8` is `fn_801680C8`, the
  first dvd function. In dvd, code-identical pairing renamed five symbols:
  `fn_801680CC` is `DVDInit`; the old `DVDInit`, `DVDPrepareStreamAsync` and
  `DVDPrepareStreamAbsAsync` labels are `cbForStateCheckID1`,
  `cbForStateMotorStopped` and `DVDCheckDisk`; and `fn_8016A2B0` is `DVDCancelAsync`.
  The former `DVDCancelAsync` at `801435B8` (different code) becomes `fn_801435B8`.
- **Source.** Both donors are unchanged copies apart from two defines at the top of
  `dvd.c`. `memcmp` used to map to `Menu_SetupDisplay`; that label was newlib `memcmp`
  and is now the real `memcmp` from [SN runtime](sn_runtime.md). The global `LastState` maps to `LastState_8040A9A8`, because OSResetSW owns
  the `LastState` symbol.
- **BSS.** Both units use `external_bss=True`. Their statics lose `scope:local` so that
  `strip_unused.py` can map them by name. `CancelAllSyncComplete` and `ResetCount`
  gained symbols, and dvd's `.sbss` ends at `8040A9AC`. All BSS remains fallback.
- **Tool fixes.** `verify_fn.py` now treats a relocated recursive `bl` on the compiled
  side as an entry branch, which `entryToPath` needs. `sdk_import_unit.py` no longer
  rewrites donor copies under `src/dolphin/` for a proposal whose retail name is a real
  SDK identifier. Before this fix it renamed `DVDPrepareStreamAsync` inside dvdfs.
  Each fix has a test.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/dvd/dvdfs --build --functions
python3 tools/verify_unit.py dolphin/dvd/dvd --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

Both units pass three complete sections and every function strictly. The whole DOL
remains byte-identical to retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. The portable workflow checks pass, and so
do 176 tests with `VERIFY_FN_REQUIRE_RETAIL=1` (one skipped).

## SDK tail batch: CARDErase, debugger stubs and the debugger driver range

No unit in this batch is promotable, so the SDK unit count stays at 81 and matching
stays at 133,808 code bytes / 642 functions (data 13,888). Each skip below was checked
against the surrounding retail range, not just the probe table.

| Unit | Probe result | Decision |
|---|---|---|
| `card/CARDErase` | `EraseCallback` CODE_SAME at `8017A94C`; `CARDEraseAsync` and `CARDErase` NO_ANCHOR | Skip: false anchor, dead-stripped |
| `odemustubs/odemustubs` | `DBInitComm`, `DBInitInterrupts` MATCH; `DBWrite` MISMATCH; the rest NO_ANCHOR | Skip: false anchors |
| `amcstubs/AmcExi2Stubs` | all NO_ANCHOR | Skip: not linked |
| `amcnotstub/amcnotstub` | `AMC_IsStub` NO_ANCHOR | Skip: not linked |

- **CARDErase.** The only anchor pairs donor `EraseCallback` with retail
  `CARDWriteWriteCallback` at `8017A94C..8017AABC`. The two are code-identical because both
  callbacks follow the same pattern, but that range already belongs to
  `dolphin/card/CARDWrite`, and the name `EraseCallback` is already used at `8017787C` in
  `CARDDir`. `CARDEraseAsync` (0x10C) and `CARDErase` (0x48) have no byte match anywhere
  in retail, and the card splits run gap-free from `CARDBlock` to `CARDRdwr`, so the
  unit is not linked. `__CARDEraseSector` (`80176ECC`) belongs to CARDBios, not this file.
- **odemustubs, AmcExi2Stubs, amcnotstub.** Their functions are 4 or 8 bytes (`blr`,
  `li r3,0; blr`, `li r3,1; blr`), so bytes alone anchor them to every identical stub in
  the DOL. The two `MATCH`es come from "layout from DBWrite" and land on the lone `blr`s
  at `8018A10C` and `8018A110`, which are not `DBInitComm`/`DBInitInterrupts`. Retail
  contains the real `DBInitComm` (`8018A4F0`, 0x78 bytes) and `DBInitInterrupts`
  (`8018A49C`, 0x54), so the stub versions cannot be linked at the same time. `DBWrite`
  is MISMATCH for the same reason: the donor returns 0 and retail is 0x260 bytes.
  `Hu_IsStub` and `AMC_IsStub` are not in the symbol map.

### Debugger driver range `8018A10C..8018AB8C`

The complete Metrowerks debugger transport is now **Matching** in
`src/dolphin/OdemuExi2/DebuggerDriver.c`: **14 functions, 2,688 code bytes and
4 initialized data bytes**. This supersedes the earlier no-source hypothesis:
[dolsdk2004](../src/dolphin/README.md) only has stub replacements, but
[MSSB's CC0 implementation](https://github.com/nlopez99/mssb-decomp/blob/407dbcbe70f8be5d167d0b1f0a229f38e0359019/src/Dolphin/OdemuExi2/DebuggerDriver.c)
contains the real EXI channel 2 mailbox transport. The source credits that commit.

| Retail address | Size | Function |
|---|---:|---|
| `8018A10C`, `8018A110` | 0x4 each | `DBClose`, `DBOpen` |
| `8018A114` | 0x260 | `DBWrite` |
| `8018A374` | 0x8C | `DBRead` |
| `8018A400` | 0x9C | `DBQueryData` |
| `8018A49C` | 0x54 | `DBInitInterrupts` |
| `8018A4F0` | 0x78 | `DBInitComm` |
| `8018A568`, `8018A5A8` | 0x40, 0x3C | `DBGHandler`, `MWCallback` |
| `8018A5E4`, `8018A690`, `8018A76C`, `8018A848` | 0xAC, 0xDC, 0xDC, 0xAC | `DBGReadStatus`, `DBGWrite`, `DBGRead`, `DBGReadMailbox` |
| `8018A8F4` | 0x298 | `DBGEXIImm` |

The existing `call_graph.tsv` evidence agrees: the `DB*` entry points wrap
interrupt handling around `DBG*` helpers that funnel into `DBGEXIImm`.
`crashwoc_fn_matches.tsv` also pairs `DBGEXIImm` exactly, and `DBGHandler` and
`DBWrite` by normalized body. CrashWOC orders the driver in the opposite direction;
MSSB's recorded addresses put `DBClose` and `DBOpen` first, as in retail.

MSSB's `configure.py` selects GC/1.2.5 for this library; its older unit Makefile
selects 1.2.5n with `-common off -inline deferred`. Local experiments with both
versions reproduce `DBGEXIImm`; the import uses the existing **GC/1.2.5n and
unmodified dolsdk2004 release flags**, with no compiler pin or flag override.
Deferred generation reverses the functions but changes inlining. Instead, the
retained definitions follow retail order, and an inline status body is shared by
`DBWrite` and the out-of-line `DBGReadStatus`. Unused donor stubs are omitted;
inlined helpers' unreferenced out-of-line copies are stripped normally.

`DBQueryData` restores interrupts inside the `!RecvDataLeng` block, as retail's
branch at `8018A420` skips the restore call when data is already pending. The
donor placed the restore outside that block. The dead assignment of the flag's
value to `pEXIInputFlag` is omitted; its subsequent address assignment matches
retail. The complete ordered section proof establishes `DBClose` then `DBOpen`
next to the verified `DBWrite`, rather than naming either stub from its size.

| Section | Retail range, end exclusive | Ownership |
|---|---|---|
| `.text` | `8018A10C..8018AB8C` | Promoted, all 14 functions |
| `.sdata` | `8040A1C0..8040A1C4` | Promoted `SendCount = 0x80` and three zero alignment bytes |
| `.sbss` | `8040ACD0..8040ACE5` | Located by relocations; remains on assembly fallback |

The `.sbss` relocations establish `MTRCallback` at `8040ACD0`, `DBGCallback` at
`8040ACD4`, `SendMailData` at `8040ACD8`, `RecvDataLeng` at `8040ACDC`,
`pEXIInputFlag` at `8040ACE0` and the byte `EXIInputFlag` at `8040ACE4`.
The compiled extent is 21 bytes. Owning that exact extent makes DTK abort with
`Invalid alignment for split` on the following gap at `8040ACE5`, so the unit uses
`external_bss=True`, like AXSPB. No padding objects or BSS ownership are invented.
The driver owns no `.bss` or `.data`. The neighboring HVQM4 range and every
function from `fn_8018AB8C` onward retain their boundaries.

From `origin/main` commit `7db800a412ed3fd3581c579ed88cbcd040c5c2ea`, matched code
increases **354,044 → 356,732 bytes**, functions **2,375 → 2,389**, and matched data
**127,502 → 127,506 bytes**. All 14 functions pass individual
`verify_fn.py NAME --unit dolphin/OdemuExi2/DebuggerDriver --build --strict-symbols`
checks; no function remains unmatched. The complete-unit gate passes both owned
sections and their relocations. The linked gate verifies all 2,692 owned bytes
and all previously promoted ranges. The whole DOL has zero differing bytes and
retains retail SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/OdemuExi2/DebuggerDriver --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

## OS thread, init, exec and EABI batch

From `0b1151b`, four more OS units are promoted: `os/__ppc_eabi_init`, `os/OSThread`,
`os/OS` and `os/OSExec`. They add 46 functions and 10,336 code bytes, and SDK units
total 85. Matching reaches 144,144 code bytes / 688 functions; the baseline was
133,808 / 642. Data reaches 13,918 matched bytes, up from 13,888.

| Unit (under `dolphin/`) | Retail range, end exclusive | Functions | Owned initialized data |
|---|---|---:|---|
| `os/__ppc_eabi_init` | `.init 80003464..8000348C` | 2 | none |
| `os/OSThread` | `.text 801623B8..801637C4` | 22 | `.data 801E3E68..801E4678`, `.sdata 8040A000..8040A008` |
| `os/OS` | `.text 8015C008..8015CAD4` | 14 | `.data 801E2338..801E2530`, `.sdata 80409FC8..80409FD8` |
| `os/OSExec` | `.text 8015EC44..8015F5A4` | 8 | `.data 801E3268..801E3273`, `.sdata 80409FF0..80409FF3` |

`sdk_import_unit.py` refuses units that sit in `.init` or own initialized data, so the
splits, `SDKObject` entries and data ranges were added by hand. Each data range is the
unique full-byte match of the donor's `.data` or `.sdata` in the retail DOL (relocated
words wildcarded); the `.sdata` of OSExec is the three bytes of `"%d"`.

- **OSTimer is a false unit.** The probe's range `8015CF8C..8015D20C` lies inside the
  promoted OSAlarm split, so there is nothing to import.
- **Version drift (May 2004 rebuild).** Source edits keep the retail bytes: the
  `.init` `__init_user` is an empty `blr` (the donor calls `__init_cpp`);
  `OSInit` calls `__OSInitAlarm`, not `OSInitAlarm`; and the three `__OSDBINTEND`
  expressions in `OSExceptionInit` use `__OSDBJUMPSTART`, because dtk keeps one
  label per address and the value is the same.
- **Run is one function.** dtk had split the asm `Run` (`8015EDCC`, `0x3C` bytes) at
  its first `blr` into `fn_8015EDCC` and `fn_8015EDF4`; `symbols.txt` now has one
  `Run` symbol. Its unreachable tail is part of the donor body.
- **Name collisions.** OS's static `BootInfo` is `OSBootInfoPtr` in source, because
  fstload owns a local `BootInfo` at `8040A930`. OSExec's static `Callback` is
  `ExecCallback` (the DVD `Callback` at `8040A9D0` is a different object), and
  `apploaderPosition` moves from function scope to file scope so that the compiler's
  `$NN` suffix does not hide it from `strip_unused.py --external-bss`.
- **Retail labels fixed.** `Menu_AddOption` (`801541B4`) is ProDG `sprintf`, `lbl_80155FB4`
  is `strcpy`, and `DVDInit` at `801680CC` replaces an earlier `DVDInit` mislabel;
  the stale `.text` symbol `__DVDLongFileNameFlag` became `fn_`. OSReset's
  `__OSRebootParams` is now a real symbol instead of a `#define` for `lbl_802F3B30`.
- **Linker symbols.** dtk synthesizes undefined relocations to `_stack_addr`,
  `_stack_end`, `__ArenaLo` and `__ArenaHi`. `ldscript.ld` defines them with the
  retail values (the old `_stack_end` value was wrong), and
  `verify_fn.SymbolTable.addr` falls back to the link script's absolute assignments
  for names `symbols.txt` lacks, so the verifier and the link agree. A test covers the
  fallback.
- **BSS.** OSThread, OS and OSExec use `external_bss=True`; their statics were made
  global in `symbols.txt` so that `strip_unused.py` can map them by name. All BSS
  remains fallback.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/os/__ppc_eabi_init --build --functions
python3 tools/verify_unit.py dolphin/os/OSThread --build --functions
python3 tools/verify_unit.py dolphin/os/OS --build --functions
python3 tools/verify_unit.py dolphin/os/OSExec --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

Every function strictly matches and every complete section matches retail. The whole
DOL remains byte-identical to retail, SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`.

## CARD bios batch

The donor `card/CARDBios.c` promotes 24 functions and 5,160 code bytes. Rebased after
the OS thread batch, SDK units total 86, and matching reaches 149,304 code bytes / 712
functions and 14,014 matched data bytes.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned initialized data bytes |
|---|---|---:|---:|---:|
| `card/CARDBios` | `80175F78..801773A0` | 24 | 5,160 | 96 |

The probe's single `MISMATCH` was version drift, not different code: the April 2004
OS calls `__OSInitAlarm` where the donor calls `OSInitAlarm`, which left an
unresolved relocation. CARDBios declares `__OSInitAlarm` locally, as `dvd/dvdlow.c`
does, and all 24 functions match strictly. Both static names that collide with
`os/OSMemory` (`OnReset`, `ResetFunctionInfo`) are renamed `CARDOnReset` and
`CARDResetFunctionInfo`, as OSAlarm does with `AlarmOnReset`, because
`--strict-symbols` refuses a name defined twice.

Retail names were shifted by one across the unit and are corrected from strict code
identity: `TimeoutHandler` (`801765E4`), `__CARDStart` (`80176A38`), `CARDInit`
(`80176FDC`), `__CARDSetDiskID` (`80177090`) and `CARDOnReset` (`80177334`, which
had carried the dead-stripped `__CARDSync` name; its remaining callers are
dead-stripped sync wrappers). Statics are marked `scope:local` except the two
`.sbss` halfwords, which `--external-bss` can only resolve as global.

Manual data ownership: `.data 801FA880..801FA8D8` is the 0x46-byte version string,
two alignment bytes and the 16-byte reset record, and `.sdata 8040A148..8040A150`
is `__CARDVersion` (pointer to the string) and `__CARDFreq` (4, `EXI_FREQ_16M`).
The `.sdata` and `.data` bytes were compared with the donor's complete compiled
sections. `__CARDDiskNone` (`8030E540`, 0x20), `__CARDEncode` and `__CARDFastMode`
(`.sbss 8040AC40`, `8040AC42`) are named from the retail base-register accesses in
`CARDInit` and `CARDGetFastMode`. All BSS remains fallback.

**CARDProgram is not in retail.** The probe reports `CODE_SAME` and `MISMATCH`, but
both anchors are the already promoted `CARDRead` functions: `ProgramCallback` is
code-identical to `CARDReadReadCallback` (same shape, `__CARDWrite` in place of
`__CARDRead`), and `CARDProgramAsync` was layout-anchored to `CARDReadAsync`, which it
equals in size only. The importer refuses the unit because `8017A6D8..8017A94C` lies
inside the `CARDRead` split. The card splits are contiguous from `CARDBios` to
`CARDRdwr` with no gap for a further object, and the probe finds no code anchor for
`CARDProgram` or for `CARDProgramAsync`, so the object was not linked into LSW1. It is not imported and
no symbols change.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/card/CARDBios --build --functions
python3 tools/verify_unit.py dolphin/card/CARDOpen --build --functions
python3 tools/verify_unit.py dolphin/card/CARDCreate --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

All 24 functions and three complete sections pass. The other 14 card units still
pass. The whole DOL remains byte-identical to retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`.

## VI, pad and EXI batch

Three unchanged donors promote 64 functions and 21,692 code bytes. Rebased after the
CARD bios batch, SDK units total 89, and SDK matching reaches 165,680 code bytes / 740
functions and 15,138 matched data bytes. When this batch was first measured from `5e3b54a`,
`NuVecRotateAxis` also moved from 99.64% to 100% in the objdiff report for a reason this
batch does not explain; it strictly matches under `verify_fn.py --strict-symbols`.
None of the units had promoted bytes on the baseline. The probe's `MISMATCH` rows were
dead-stripped donor functions: no version-drift fix was needed.

| Unit (under `dolphin/`) | Retail `.text` range, end exclusive | Functions | Code bytes | Owned initialized data bytes |
|---|---|---:|---:|---:|
| `exi/EXIBios` | `80163984..80165360` | 23 | 6,620 | 328 |
| `vi/vi` | `8016BEC0..8016E058` | 22 | 8,600 | 1,008 |
| `pad/Pad` | `8016E29C..8016FBE4` | 19 | 6,472 | 120 |

Data ranges come from unique complete donor-byte matches, including the alignment bytes
before the next owner: EXIBios `.data` `801F5918..801F5A28` and `.sdata` `8040A008..8040A040`,
vi `.data` `801F6748..801F6B30` (its jump table `@740` ends the range) and `.sdata`
`8040A0E8..8040A0F8` (`__VIVersion` and the `"vi.c"` string), Pad `.data`
`801F6B30..801F6B88` and `.sdata` `8040A0F8..8040A118`. The importer gained `--data
SECTION=START..END` for this (see [Importing a unit quickly](#importing-a-unit-quickly)).
Pad's `SamplingHandler` (`8016FAB4`) and `PADSetSamplingCallback` (`8016FB14`) are Pad
functions; the earlier si/SISteeringAuto anchors on them were false.

`.bss` and `.sbss` stay fallback. `symbols.txt` follows the donor layout so
`--external-bss` can bind the compiled statics:

- VI's `lbl_802F4AB8` (`0xF0`) is two `u16[59]` arrays, `regs` and `shdwRegs`, 8-byte
  aligned (`0x78` each). Every retail access uses `802F4AB8` as the base register.
  `message$360` is the function-local static in `VIGetTvFormat`, named as the compiler does.
- A donor static needs a global symbol for `strip_unused.py` (a `scope:local` entry is looked
  up as `NAME_<address>`), so VI's `CurrTvMode`, EXI's `Ecb` and `IDSerialPort1` and Pad's
  BSS lose the flag. `SymbolTable` keeps a global and a local of one name apart (the local
  resolves as `NAME_<address>`), but SIBios's split object still names its local `.data`
  `Type` without a suffix, so a global Pad `Type` hid it and five SIBios functions lost
  their reference proof. Pad's `Type` (`802F4C00`) is therefore `PADType` in `symbols.txt`
  and `Pad.c`.
- Two locals of one name stay duplicates, but `verify_fn.py` resolves a duplicated local
  inside the unit's own split ranges. This fixes OSError's `@13` string, which became
  ambiguous when dvdFatal added its own `@13`.
- Pad's static `OnReset` (`8016F9F8`) is referenced from its `.data` and must carry its donor
  name. A second local `OnReset` (OSMemory) would make both unverifiable by name, so Pad's
  is `PADOnReset` in `symbols.txt` and `Pad.c`; a static function's identifier does not
  change the code.

Callee names follow: `EXIInit`, `getTiming`, `GetCurrentDisplayPosition`, `VIGetNextField`,
`PADRead` and the CodeWarrior runtime labels `__shr2u` and `__shl2i`.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/exi/EXIBios --build --functions
python3 tools/verify_unit.py dolphin/vi/vi --build --functions
python3 tools/verify_unit.py dolphin/pad/Pad --build --functions
python3 tools/verify_unit.py dolphin/os/OSMemory --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
```

All three units pass three complete sections and every function strictly (23, 22 and 19);
OSMemory still passes six of six. The whole DOL remains byte-identical to retail, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`.
Portable workflow checks and 178 tests with `VERIFY_FN_REQUIRE_RETAIL=1` pass (one skipped).

## HVQM4 decoder (parked)

The decoder now has a split and a source candidate at
[`src/dolphin/hvqm4/hvqm4dec.c`](../src/dolphin/hvqm4/hvqm4dec.c), registered
**NonMatching**. Its 43 functions cover `8018432C..8018A10C` (24,032 bytes).
Thirty-three functions (16,296 bytes) strictly verify; **zero code or data bytes are
promoted**. The normal build retains assembly fallback for the complete unit.
The debugger range beginning at `8018A10C` is unchanged.

The source adapts CC0 MSSB reconstruction code at commit
`407dbcbe70f8be5d167d0b1f0a229f38e0359019`
([upstream PR #27](https://github.com/nlopez99/mssb-decomp/pull/27)), with
LSW1-specific control flow, structures and sound routines. Tilka/hvqm4 supplied
algorithm/layout evidence, not copied implementation. See the source provenance
comment. No Hudson SDK archive was needed. The unit builds with the dolsdk2004
release flags (`-O4,p -inline auto`) but **GC/1.2.5**, not the SDK library's
GC/1.2.5n; the initial MSSB `-inline noauto` lead is not the accepted configuration.

### Per-unit compiler override (GC/1.2.5)

`configure.py` pins this one object with `SDKObject(..., mw_version="GC/1.2.5")`;
the rest of the SDK stays on GC/1.2.5n and no flags change. Evidence: under
GC/1.2.5n, `HVQM4SetBuffer` (280/281) and `HVQM4DecodeIpic` (323/324) differ only
in where the epilogue places `mtlr` among the saved-register restores, and no
source shape moved it. Compiling the unchanged candidate with each MWCC GC version
showed that GC/1.0, 1.1 and 1.2.5 put `mtlr` where retail does, making both
functions strict matches while all 21 earlier strict matches still verify.
GC/1.1p1, 1.3 and later regress them. GC/1.2.5 is the closest version to
the SDK's and is the one pinned. Hudson shipped HVQM4 as separate middleware, so
an older compiler than the SDK's is plausible. The linked-image comparison stays
at zero differing bytes.

`score_fn.py --compiler sdk` now reads the unit's `mw_version` and `cflags`
(include directories, `-lang=c`) from `build.ninja`, so trial scores use the
same compiler as the build. Previously it fell back to GC/1.2.5n and failed to
find SDK headers. `--mw-version VER --mw-flags 'FLAGS'` still override both for
experiments without changing build pins or the verification oracle.

Historical GC/1.2.5 baseline: 23 strict results, checked against the retail DOL
and symbol destinations. The [Pikmin follow-up](#pikmin-cc0-donor-follow-up) adds ten:

| Verified function | Bytes |
|---|---:|
| `setHVQPlaneDesc` | 284 |
| `getByte` | 100 |
| `getBit` | 64 |
| `decodeHuff` | 112 |
| `IpicBlockDec` | 324 |
| `IpicLineDec` | 212 |
| `initMCHandler` | 140 |
| `_MotionComp_00` | 156 |
| `_MotionComp_10` | 412 |
| `_MotionComp_01` | 428 |
| `_MotionComp_11` | 684 |
| `fn_80188758` | 32 |
| `HVQM4InitSeqObj` | 36 |
| `HVQM4BuffSize` | 116 |
| `HVQM4SetBuffer` | 1,124 |
| `HVQM4DecodeIpic` | 1,296 |
| `HVQM4DecodePpic` | 36 |
| `HVQM4DecodeBpic` | 1,448 |
| `HVQM4DecSoundCreate` | 84 |
| `HVQM4DecSoundClose` | 56 |
| `HVQM4DecSoundDecode` | 344 |
| `HVQM4DecodePcm16Ch1` | 288 |
| `HVQM4DecodePcm16Ch2` | 360 |

The follow-up from main `7db800a` adds **three strict matches / 276 bytes**:
`getByte`, `getBit` and its inlined caller `decodeHuff`. Splitting the byte-reader
loads from their shifts, declaring its accumulator before the bit index, and
masking before the signed return reproduces retail's registers and final `extsh`.
Keeping the bit result in the unsigned accumulator reproduces the standalone
reader and the Huffman caller's shift/store schedule. All 18 earlier strict
matches remain intact.

`HVQM4DecodeIpic` improves from 322/324 to **323/324** by initializing the loop
index before the plane pointer. Its only remaining difference is the epilogue's
`mtlr` placement. `HVQM4SetBuffer` remains 280/281. Local declaration/sign variants,
explicit return and loop-shape experiments did not solve their epilogues under
GC/1.2.5n; the [GC/1.2.5 override](#per-unit-compiler-override-gc125) later solved both. That override is the only pin change; flags, verification rules and split boundaries are unchanged.

The GC/1.2.5 follow-up (rebased on main `0ca0aff`) adds **two strict matches /
2,420 bytes**: `HVQM4SetBuffer` and `HVQM4DecodeIpic`. The report reads
**368,076 code bytes / 2,455 functions** with matched data unchanged at 127,506
bytes. Without these two functions, main would be 365,656 / 2,453. It also raises
three unverified scores in the committed candidate:

- `MCBlockDecDCNest` reaches 107/108. Retail colors the parameters after the
  loop locals and reserves one unused 8-byte stack slot, so the source copies the
  parameters into locals declared after `j` and declares an unused local. Only
  the plane-pointer copy still reads `r3` instead of the state copy. Indexing,
  K&R definitions, casts, `register` and copy order did not change this.
- `IpicPlaneDec` reaches 50/60 after a sequential declaration-order sweep.
- `fn_80189FFC` has all 68 instructions identical once it uses a counted
  `for (n = count; n != 0; n--)` loop (retail's CTR loop) and `adpcm4Table[type & 3]`
  indexing. It still returns `MATCH (unverified references)`: `adpcm4Table` is an
  address-taken pointer table whose contents and relocations only the full-unit
  proof can check, so it is **not counted**.

`HVQM4InitDecoder` stays at 207/216. All twelve orderings of the two zero stores
and two reciprocal loops, separate counters, `do`-loops and `i + 1` indexing
emit loop guards (`cmpwi; bge`) that retail lacks, or lose the unrolling.
`WeightImBlock` drops to 23/102 under GC/1.2.5. All 22 valid declaration orders
score 22–23, because its gap is in instruction scheduling, not register colouring.

Earlier history: the report moved from **354,044 code bytes / 2,375 functions** to **354,320 /
2,378** before rebasing; matched data remains 127,502 bytes. After rebasing onto
main `8f5823e` (including the debugger driver and CW runtime), the final report is
**362,148 code bytes / 2,434 functions** and 127,506 matched data bytes. The HVQM4
change itself remains **276 bytes / three functions**, with no matched-data delta.
This credits candidate function matches; the unit still promotes **zero bytes**
and links its complete assembly fallback. After the GC/1.2.5 follow-up, **20
functions / 15,896 bytes** remain unverified.

Search covered the readers first, then Huffman, basis/nest, inter-picture and
ADPCM routines. Sequential `variant_search.py` runs tried at most 180 variants per
pass (the scheduling passes exhausted 19 and 99 variants). No permuter or
concurrent sweep ran. Ghidra output and the CC0 MSSB donor were checked for control
flow and layout; no Tilka implementation was copied. Searches plateaued without
another strict match, so unverified rewrites remain local rather than expanding
the committed source diff.

Historical pre-Pikmin scores for the 20 then-remaining functions follow.
The current ten are listed in the [follow-up](#pikmin-cc0-donor-follow-up).
Scores show exact / target
instructions, an informational measure, **not verification**. "Committed" is
the candidate source compiled with GC/1.2.5. "Earlier best" is the best local
experiment under GC/1.2.5n from the previous parking; those rewrites were not
committed and need re-scoring under GC/1.2.5.

| Remaining function | Committed | Earlier best | Next hypothesis |
|---|---:|---:|---|
| `HVQM4InitDecoder` | 207/216 | 207/216 | Retail has no loop guards: the `divTable` loop is unrolled 8× with the zero store scheduled inside it, and `mcdivTable[0] = 0` sits just before the CTR loop. Every loop/statement order tried still emits guards. |
| `_readTree` | 99/179 | 143/179 | Earlier reader definitions inline the outer bit/byte reads; nested recursive copies still call `getBit`. Recover that inline structure and the 0x38 frame. |
| `Ipic_BasisNumDec` | 2/177 | 127/177 | Two levels of Huffman/bit inlining recover the 177-instruction leaf body. Signed row counts, separate luma/chroma lifetimes and the 0x48 frame remain. |
| `IpicDcvDec` | 25/79 | 27/79 | Use signed row/column counts and a full-width predictor; retain `state + i * 0x14` instead of incrementing a second pointer. |
| `MakeNest` | 14/233 | 15/233 | Retail copies eight pixels per unrolled iteration; replace narrowing `u16` counters with the target's full-width counts before tuning register order. |
| `WeightImBlock` | 23/102 | 55/102 | Semantics and expression trees match retail; the gap is scheduling. Declaration order has no effect under GC/1.2.5. |
| `BasisBlockDec` | 9/334 | 16/334 | Retail has a separate one-basis path before the general accumulation path; recover its scalar accumulators and stack buffers. |
| `ReadBasisByte` | 49/252 | 63/252 | Keep the progressive sample pointer and retail's unsigned min/max chain; declaration order alone does not recover the scalar sample schedule. |
| `IpicPlaneDec` | 50/60 | 48/60 | Stride is loaded into a temporary and then copied (`lhz r6; addi r30,r6,0`); `u16` stride/width/height made it worse. Finish that copy and the stack-store order. |
| `MotionComp` | 13/59 | 10/59 | Hoisting destination, source and stride before helper selection reduces the size gap from 15 to three instructions; align parameter-copy and offset-expression order. |
| `decode_PB_cc` | 75/263 | 75/263 | Reconstruct the nested inline Huffman decodes and signed full-width run/type temporaries before register tuning. |
| `MCBlockDecBasis` | 79/651 | 78/651 | Recover the target's separate type-specific basis paths, scalar accumulation and original stack-buffer lifetimes. |
| `ReadBasisNibble` | 56/268 | 73/268 | Check full-width sample/min/max temporaries and the byte-reader pointer updates before inlined Huffman-factor decoding. |
| `MCBlockDecMCNest` | 30/114 | 38/114 | Preserve the luma target/stride across plane iteration; align per-plane motion offsets and block/type local lifetimes. |
| `MCBlockDecDCNest` | 107/108 | 74/108 | Only the plane copy reads `r3` (the parameter) instead of the state copy; find what makes retail copy from the coloured local. |
| `PpicPlaneDec` | 54/268 | 66/268 | Keep the saved initial baseline; recover nested Huffman inlining and motion-vector temporary lifetimes. The reader change alters this unverified caller's allocation. |
| `BpicPlaneDec` | 64/271 | 64/271 | Recover the motion-vector delta inline structure, then branch-local references and run state. |
| `fn_80189934` | 37/127 | 37/127 | Align the stream/header pointer lifetimes and high/low-nibble predictor updates; strict acceptance also needs ADPCM table contents/relocations proved. |
| `fn_80189B30` | 54/145 | 54/145 | Align the paired-channel header reads, predictors and table/index update ordering; table proof remains mandatory. |
| `fn_80189FFC` | 68/68 | 49/68 | Code is identical; strict acceptance needs `adpcm4Table` contents and relocations proved, which needs the full-unit proof (blocked by the externalized BSS section set) or a per-symbol relocation check in `verify_fn.py`. |

Owned data ranges are `.rodata` `801B0604..801B0640`, `.data`
`801FB640..801FD048`, `.bss` `8030F168..8030FBA8`, and `.sbss`
`8040ACB0..8040ACD0`. BSS storage is externalized to fallback. The full-unit gate
currently refuses the candidate because its initialized sections differ from the
split's section set including BSS; this is an expected refusal, not a proof of
initialized data or of internal BSS layout. Complete code, initialized section and
relocation proof, an approved BSS treatment and linked-range identity remain
mandatory before promotion. Do not change the oracle or boundaries to clear it.

Reproduce from a bootstrapped checkout:

```sh
bash build.sh
python3 tools/matching_progress.py
python3 tools/verify_fn.py HVQM4DecodeBpic --unit dolphin/hvqm4/hvqm4dec --build --strict-symbols
python3 tools/verify_unit.py dolphin/hvqm4/hvqm4dec --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
```

The full-unit command is expected to fail until the above blockers are resolved;
run `verify_fn.py --strict-symbols` individually for the table entries. At this follow-up's parking,
`bash build.sh`, all 23 individual strict checks and the linked-image comparison
pass with **zero differing bytes** across the whole image; none of the 21 earlier
strict matches regresses under GC/1.2.5. The full-unit command still refuses the externalized
BSS section set, so initialized data/relocations and BSS layout are not proved.
The earlier parking also passed 366 portable tests (two skipped), configuration,
map generation and shell syntax checks; this C-only follow-up reruns the retail
build and strict checks. Local baselines, Ghidra output, experimental source
snapshots, per-function scores and detailed logs remain in ignored
`work/hvqm4/` and `work/hvqm4b/`; the source and this handoff retain the reviewable result.

### Pikmin CC0 donor follow-up

Pikmin's Matching decoder supplies ten new strict matches, **8,160 code bytes**.
The unit advances from **23/43 functions / 8,136 bytes** to **33/43 / 16,296**,
with all 23 earlier strict matches preserved. **Zero bytes are promoted**: the
complete unit remains NonMatching and uses assembly fallback.

The donor is [projectPiki/pikmin](https://github.com/projectPiki/pikmin), commit
`9a2163626a269ae7c699fc195cf91a5ca791044d`, **CC0-1.0**
([license](https://github.com/projectPiki/pikmin/blob/9a2163626a269ae7c699fc195cf91a5ca791044d/LICENSE.MD)).
Reviewed inputs are `src/hvqm4dec/hvqm4dec.c`, `include/hvqm4.h` and
`configure.py`'s `cflags_base`. Bodies for all 12 shared nonverified functions
were tried with donor inline helpers and types, retaining GL5E4F's observed
layouts. Donor `int` locals matter: MWCC distinguishes them from this project's
`s32` (`long`), affecting initialization loops, nest copying and B-picture
scheduling. Explicit inline readers defined before their callers recover nested
Huffman decoding. The LSW1-only basis readers reuse those helpers with target
sample layouts; `ReadBasisNibble` takes a byte pointer. Retained nonmatching
rewrites improve the score without regressing an existing strict match.

| New strict function | Bytes |
|---|---:|
| `HVQM4InitDecoder` | 864 |
| `_readTree` | 716 |
| `Ipic_BasisNumDec` | 708 |
| `IpicDcvDec` | 316 |
| `MakeNest` | 932 |
| `WeightImBlock` | 408 |
| `ReadBasisByte` | 1,008 |
| `decode_PB_cc` | 1,052 |
| `ReadBasisNibble` | 1,072 |
| `BpicPlaneDec` | 1,084 |

Both projects use **GC/1.2.5**, `-O4,p -inline auto`, integer enums, hardware FP,
disabled C++ exceptions and the `cats off` pragma. Differences from our SDK flags:

| Setting | Pikmin `cflags_base` | Accepted SDK flags |
|---|---|---|
| Alignment | `-align powerpc` | Compiler default |
| FP contraction | `-fp_contract off` | Compiler default |
| Character type | Compiler default | `-char unsigned` |
| Strings | `-str reuse -multibyte` | Compiler default |
| Diagnostics | `warn_notinlined off`, `-maxerrors 1`, release `-w off` | `-warn pragmas -requireprotos` |
| Other controls | `-nosyspath -RTTI off` | `-I-`, SDK defines |

Include paths and project-specific version defines were adapted for this
checkout. Testing Pikmin's complete base flags with local include paths preserved
every strict match but added no matches or code gains. The accepted per-unit compiler and SDK flags are therefore
unchanged. This follows the [per-unit identification rule](code_matching_workflow.md#promotion).

The bounded searches exposed a workflow bug: temporary candidate paths lost the
unit's compiler override and flags. `order_search.py` and `variant_search.py` now
forward the original unit's MWCC version and shell-quoted flags. Focused tests
cover that forwarding. Serial reruns tried 140 plane, 140 DC-nest and four motion
variants; none improved the retained scores. No concurrent sweep ran.

Current remaining scores show exact / target instructions, **not verification**:

| Function | Score | Next evidence needed |
|---|---:|---|
| `BasisBlockDec` | 43/334 | Scalar accumulator and stack-buffer lifetimes; candidate has 333 instructions. |
| `IpicPlaneDec` | 50/60 | Stride copy and stack-store order; candidate has 59 instructions. |
| `MotionComp` | 16/59 | Parameter copies and offset expression scheduling; candidate has 62 instructions. |
| `MCBlockDecBasis` | 458/651 | Type-specific basis paths and stack lifetimes; candidate has 706 instructions. |
| `MCBlockDecMCNest` | 45/114 | Plane target/stride and motion-offset lifetimes; candidate has 118 instructions. |
| `MCBlockDecDCNest` | 107/108 | Initial plane copy must use the colored state local instead of the parameter; bounded declarations, qualifiers and types did not solve it. |
| `PpicPlaneDec` | 268/268 | All instructions agree; strict verification refuses unproved `mcbTypeNext` transition-table references. |
| `fn_80189934` | 37/127 | Stream/header lifetimes and ADPCM predictor updates; candidate has 131 instructions. |
| `fn_80189B30` | 54/145 | Paired-channel header/predictor scheduling; candidate has 149 instructions. |
| `fn_80189FFC` | 68/68 | All instructions agree; `adpcm4Table` contents and relocations still need full-unit proof. |

Tilka/hvqm4 commit `09700757304af4f439f0e50e7a0160cadc4d7a48`
(LGPLv2-or-later) was checked for newer basis/control-flow and layout evidence;
no implementation was copied. Pikmin's header also credits Bakuten Shoot Beyblade
2002. Repository and code searches found no usable CC0 decoder source for that
lead. Sound type/index variants did not improve the existing scores. Donor
copies, trials and raw logs remain under ignored `work/hvqm4c/`.

The externalized-BSS refusal was investigated without changing split boundaries.
Temporarily owning BSS reproduced the complete **2,624-byte `.bss` and 32-byte
`.sbss` layouts**, passing the existing BSS-layout checks. The unit gate then
reached the code-size mismatch: its diagnostic says `.text: donor section is
-272 bytes short of the retail range` (the candidate is 272 bytes too large).
Seven earlier strict matches became reference-unverified while the complete-unit
proof failed, so that configuration was rejected. `external_bss=True` remains;
the final unit gate still refuses the section-set mismatch. DebuggerDriver has
no BSS split ranges, so its externalization does not resolve this unit's existing
owned ranges. Once code matches, own the existing BSS ranges and prove all
sections/relocations before promotion. No verifier, pins or boundaries changed.

On the same rebased main (`58647c0`), the report moves from **383,968 code bytes /
2,541 functions** to **392,128 / 2,551**; matched data stays **128,546 bytes**.
The unit's report count is 24 → 34 functions / 8,408 → 16,568 bytes because it
credits `fn_80189FFC` despite unverified references. Strict counts above exclude
it and `PpicPlaneDec`. The original kickoff baseline predates unrelated main
merges; its larger cumulative delta is not attributed to this task.

Final validation: all 43 individual `verify_fn.py NAME --build --strict-symbols`
checks produce 33 MATCH, eight MISMATCH and two MATCH with unverified references.
`bash build.sh`, `matching_progress.py` and `sprint_finish.py` pass; the sprint
queue accounts for ten verified gains and nine parked targets (`fn_80189FFC`
was already excluded from that queue). The unit gate refuses the BSS section
set as described above. `dol_compare.py --linked-units` and the whole-image
comparison both report **zero differing bytes**, SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Portable validation passes
**621 tests (two skipped)**, compileall, configuration, map generation, shell
syntax, documentation links and `git diff --check`.

## SDK leftovers

From `4970a42`, all executable functions of plan item `sdk-leftovers` pass the
strict oracle. Four exact auto ranges are now source-backed Matching units;
existing split boundaries and compiler pins are unchanged.

| Unit under `dolphin/` | Section and retail range, end exclusive | Functions | Newly promoted code bytes |
|---|---|---:|---:|
| `Runtime/__start` | `.init 80003100..80003328` | 4 | 552 |
| `Runtime/__init_fpu` | `.init 80003334..80003464` | 1 | 304 |
| `dvd/dvdlow_test` | `.text 8016BE88..8016BEC0` | 1 | 56 |
| `amcstubs/AmcExi2Stubs` | `.text 8017C754..8017C77C` | 7 | 40 |

The DVD test uses its unchanged dolsdk2004 body, removed from the already imported
`dvdlow.c` where it had been stripped. Its two alarm objects keep their existing
owned BSS ranges in `dvdlow`; no storage is duplicated or externalized.

### Donor and stub identity

The first dolsdk2004 `gx/GXFifo.c` trial fails: `GXGetFifoBase` loads the base
pointer, whereas retail at `8017C758` only returns, and `GXGetFifoSize` loads
`fifo+8`, whereas `8017C75C` returns zero. MSSB's CC0 `gx/GXFifo.c` has only
unused placeholders for these functions. Neither body is a retail GX getter.

Retail `fn_8015057C` instead establishes their identity: debug mode 2 selects
the seven-pointer table `801E19E0`; mode 3 selects `801E19FC`, whose entries
are the real `DBInitComm`, `DBInitInterrupts`, `DBQueryData`, `DBRead`, `DBWrite`,
`DBOpen`, `DBClose`. The mode-2 table points in order at `8017C754`, `8017C758`,
`8017C75C`, `8017C764`, `8017C76C`, `8017C774`, `8017C778`. The entire 40-byte
section equals dolsdk2004 `amcstubs/AmcExi2Stubs.c`, in order:
`EXI2_Init`, `EXI2_EnableInterrupts`, `EXI2_Poll`, `EXI2_ReadN`, `EXI2_WriteN`,
`EXI2_Reserve`, `EXI2_Unreserve`. Its unreferenced `AMC_IsStub` is stripped.
MSSB's corresponding CC0 source agrees. The donor is imported unchanged, the
seven symbols follow the table identity, and the two false GX names are recorded
in `rejected_renames.tsv`. The names themselves earn no matching credit.

The MSSB revision reviewed is `407dbcbe70f8be5d167d0b1f0a229f38e0359019`;
its [CC0 licence](../src/dolphin/COPYING.MSSB) and
[provenance](../src/dolphin/README.md) are retained. No MSSB text is copied
for this batch. dolsdk2004 remains pinned at `2328b4164b1a98422a2255d83ce9a5a7548990cc`.

### Original startup assembly and flags

Both dolsdk2004 and MSSB `os/__start.c` use the SDK register-clearing and
ROM-copy setup. The first donor `__start` trial matches only 18/85 retail
instructions (87 donor instructions). The SN entry instead initializes its
linker bases, clears two fixed BSS extents and enters the SN debugger. Its
assembly reconstruction retains symbolic relocations and the current callee names.
The two BSS-end immediates retain retail's `80407A3C` and `8040B2E4`; the
existing blocked relocation at `8000315C` is unchanged.

| Function | Evidence that retail was assembly |
|---|---|
| `__start` (`80003100`, 340 bytes) | Establishes r1/r2/r13 before an ABI frame exists; owns r29-r31 across startup; writes SRR0/SRR1 and executes `rfi` to its own `+0x144` entry. |
| `fn_80003254` (`80003254`, 108 bytes) | Frameless boot-argument parser returns argc in r3 and argv in r4; every caller consumes both registers directly. It also writes the aligned argv address to low-memory ArenaHi. |
| `__init_registers` (`80003300`, 40 bytes) | Initializes GQR0..7 through `mtspr`; a frameless startup register sequence, unlike the SDK helper of this name. |
| `__init_fpu` (`80003334`, 304 bytes) | Enables MSR[FP], tests HID2[PSE], explicitly seeds all 32 paired and scalar FPRs, and writes FPSCR; normal C cannot express this register initialization. |

`fn_800032C0` remains C: it masks the low-memory pad-3 buttons with `0xEEF`
and calls `OSResetSystem(0, 0, FALSE)` when the chord is held. With the standard
GC/1.2.5n SDK flags its instructions score 15/16: the scheduler hoists
`lis r3,0x8000` ahead of the two stack stores. Adding only `-schedule off` to
`Runtime/__start` produces 16/16, and all four functions and the complete
552-byte section pass strict verification. There is no peephole or optimization
override for this unit. Other SDK compile commands are unchanged.

### Pad/dvdfs data and BSS proof

Pad and dvdfs were already promoted; their five remaining report rows have
identical instructions. Their data symbols were too coarse or mistyped:

- `CmdReadOrigin` and `CmdCalibrate` are `u32` values `41000000` and `42000000`,
  confirmed by their declarations and SI transfers, not two-byte strings. Each
  owns four bytes; the Pad `.sdata` section now scores completely.
- The `801F5D00..801F5D8C` and `801F5D8C..801F5FF0` covering labels are split into
  the donor's ten individual strings with their exact lengths and alignment gaps.
  Complete section/relocation proof and linked bytes remain identical. dvdfs
  `.data` now scores completely.

This credits five already compiled functions (2,284 bytes), plus 1,040 previously
uncredited data bytes (Pad `.sdata` 32, dvdfs `.data` 1,008). Before unrelated
main drift, the report is 361,872 -> 365,108 matched code bytes,
127,506 -> 128,546 matched data bytes and 2,431 -> 2,449 matched functions.
The actual new source promotion is 952 code bytes / 13 functions, separate
from those metadata corrections.

After rebasing onto main `0139228`, the report reaches **365,384 matched code
bytes, 128,546 matched data bytes and 2,452 matched functions**. Current main
has 362,148 / 127,506 / 2,434 respectively; the branch's gain stays
**3,236 code bytes, 1,040 data bytes and 18 functions**. The kickoff-to-final
change includes main's separate HVQM4 gain of 276 bytes / three functions.

`sprint_finish.py` closes the bench with 18 verified item functions / 3,236 bytes,
two parked non-functions and zero deferred functions in 771.8 active seconds:
84.0 functions per active hour, 15,094 verified bytes per active hour. The verified
count includes the five Pad/dvdfs metadata corrections; new promotion remains
13 functions / 952 bytes. Its generic “Promoted: none” line is superseded by the
separate complete-unit and linked-image proofs above.

The strict verifier previously rejected *sized* reads into owned BSS even when
`prove_unit` had proved its exact layout and canonical relocations. Address-taken
references already used that proof. Sized reads now require the same complete
proof, correct address and in-section width. No-proof, wrong-address, changed
layout and initialized-vs-BSS cases still fail. The regression test fails with
the old method and passes with the fix; Pad now passes 19/19 functions, dvdfs
11/11, and the unchanged dvdlow unit passes 20/20.

### Remaining report rows and reproduction

There are no executable functions left in this item. Two synthetic DTK functions
remain parked, with score N/A (zero executable instructions):

- `pad_00_80003328_init`: 12 zero bytes containing the overlapping zero constants
  read at `80003328` and `8000332C` by `__init_fpu`. Carving its neighbours makes
  DTK count these as another function (+12 total code bytes and +1 total function,
  without matched credit). Next: distinguish code-section constants in reports.
- `pad_00_8000348C_init`: 20 zero alignment bytes after `__init_hardware`, with no
  executable entry. Next: distinguish section padding in reports.

Both stay on the retail fallback. No fake assembly functions or matching bytes
are introduced for either range. Plan revision 5 follows the carved unit names
and parks those non-functions, so `plan.py --no-remote` has no revision warnings.

```sh
bash build.sh
python3 tools/verify_unit.py dolphin/Runtime/__start --build --functions
python3 tools/verify_unit.py dolphin/Runtime/__init_fpu --build --functions
python3 tools/verify_unit.py dolphin/dvd/dvdlow_test --build --functions
python3 tools/verify_unit.py dolphin/amcstubs/AmcExi2Stubs --build --functions
python3 tools/verify_unit.py dolphin/pad/Pad --build --functions
python3 tools/verify_unit.py dolphin/dvd/dvdfs --build --functions
python3 tools/verify_unit.py dolphin/dvd/dvdlow --build --functions
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
python3 -m unittest discover -s tests
```

Every retained function passes `verify_fn.py NAME --build --strict-symbols`
through the unit gate. All linked source-backed ranges and the whole DOL are
byte-identical to retail (zero differing bytes), SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. All 618 unit tests pass (two skipped),
as do Python compilation, map generation, shell syntax, local documentation links
and `git diff --check`. Raw trials stay in ignored `work/`.
