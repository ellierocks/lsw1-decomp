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

Keep file-scope statics as the donor wrote them and let `--external-bss` replace their
storage: declaring them `extern` changes MWCC's code (it reloads values around
callbacks), so the importer always uses `external_bss=True` for units with `.bss` or
`.sbss`. `strip_unused.py` maps the donor's static names to `symbols.txt` objects by
name, which is why the importer renames the retail `lbl_` labels to the donor names.

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

The four-byte `SamplingRate` at `.sbss` `8040A920..8040A924` remains an
assembly fallback: the complete-unit verifier deliberately does not accept BSS
as proved data. The vendored source therefore references DTK's exact split-local
name, `SamplingRate_8040A920`, rather than defining storage in the C unit. This
keeps the BSS unchanged and out of the promoted unit; no BSS proof is claimed.
The full image remains byte-identical to retail (SHA-1
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
- **BSS ownership.** The retail BSS stays in fallback objects, but the compiled
  object defined the statics. `tools/strip_unused.py --external-bss` now turns
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
  retail's `.sbss` symbols. BSS is never promoted.
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
  BSS is never promoted.
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
| `SISamplingRateBSS.c` | The "Missing configuration" message was a deliberate sbss-only fallback split with no `Object`; it is now a `NonMatching` object and the DOL is unchanged. |

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
  `dvd.c`. `memcmp` maps to `Menu_SetupDisplay`, the retail label CARD already uses
  for it. The global `LastState` maps to `LastState_8040A9A8`, because OSResetSW owns
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

Retail `.text` `8018A10C..8018AB8C` (0xA80 bytes) is the Metrowerks debugger driver
(EXI channel 2 mailbox transport), not a dolsdk2004 unit.
`~/.cache/lsw1/dolsdk2004/src/` has only the stub replacements (`odemustubs`,
`odenotstub`, `amcstubs`, `amcnotstub`) and `db/db.c`, which retail already splits at
`80167280..8016736C`. Searching the donor and the opensaga cache for `DBGEXIImm`,
`DBGHandler`, `MWCallback` and `DBGReadStatus` finds nothing, so there is no source to
import and none was invented.

| Retail address | Size | Function |
|---|---:|---|
| `8018A10C`, `8018A110` | 0x4 each | lone `blr`s, unnamed (`fn_8018A10C`, `fn_8018A110`) |
| `8018A114` | 0x260 | `DBWrite` |
| `8018A374` | 0x8C | `DBRead` |
| `8018A400` | 0x9C | `DBQueryData` |
| `8018A49C` | 0x54 | `DBInitInterrupts` |
| `8018A4F0` | 0x78 | `DBInitComm` |
| `8018A568`, `8018A5A8` | 0x40, 0x3C | `DBGHandler`, `MWCallback` |
| `8018A5E4`, `8018A690`, `8018A76C`, `8018A848` | 0xAC, 0xDC, 0xDC, 0xAC | `DBGReadStatus`, `DBGWrite`, `DBGRead`, `DBGReadMailbox` |
| `8018A8F4` | 0x298 | `DBGEXIImm` |

The callers and callees agree with the names (`call_graph.tsv`): the `DB*` entry points
wrap `OSDisableInterrupts`/`OSRestoreInterrupts` around the `DBG*` helpers, all of which
funnel into `DBGEXIImm`, and `DBInitInterrupts`/`DBInitComm` call
`__OSMaskInterrupts`, `__OSUnmaskInterrupts` and `__OSSetInterruptHandler`. CrashWOC
links the same code with the same sizes: `crashwoc_fn_matches.tsv` has `DBGEXIImm`
(exact), `DBGHandler` and `DBWrite` (normalized body) paired to these addresses. CrashWOC
orders it `DBGEXIImm..DBWrite`, `DBOpen`, `DBClose`, then the `EXI2_*` stubs, while retail
runs in reverse. `DBClose`/`DBOpen` would then be the two `blr`s at `8018A10C` and
`8018A110`, with the `EXI2_*` stubs and `AMC_IsStub` not linked in LSW1.

The range sits directly after the HVQM4 decoder (`8018432C..8018A10C`, out of scope) and
before `fn_8018AB8C`/`_write`, which call `InitializeUART`/`WriteUARTN`, so the
range boundary is `8018A10C..8018AB8C`.

**Next hypothesis.** No donor source exists, so this is not an import. A hand-written
`DebuggerDriver.c` would have to be reconstructed from the retail (or CrashWOC) bytes with
the CodeWarrior GC/1.2.5n compiler, starting from `DBGEXIImm` and the `DBG*` helpers,
then given a split at `8018A10C..8018AB8C`, its own `.sbss`/`.bss` ranges (not yet
located) and the `DBOpen`/`DBClose` names. Until then the two `blr`s stay unnamed and the
`DB*` labels remain symbol-only, which is not matching progress.

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
