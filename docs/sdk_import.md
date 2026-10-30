# Dolphin SDK import evidence

The first Phase 1 import promotes nine complete SDK translation units from
[dolsdk2004](../src/dolphin/README.md). All 75 retained functions strictly match;
their sections, canonical relocations and linked ranges match retail. The
unreferenced donor functions are removed before comparison and linking, as
with the original SN linker. No candidate instruction bytes are replaced.

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

To repeat every SDK unit/function check after building:

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
`tools/sdk_probe.py --sdk /path/to/dolsdk2004 --strip-imported gx/GXTexture`
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

## Remaining Phase 1 work

With both imports, total matching is **25,848 code bytes / 149 functions /
2,324 matched data bytes**. Fourteen SDK units contribute 23,804 code bytes /
126 functions / 2,696 complete data bytes. The combined build retains the retail
SHA-1 and all complete-unit proofs.

These fourteen promoted units do not complete every SDK archive. Remaining
SDK ranges retain assembly fallback. No other probe row is promoted by name
or layout alone, and no unverified BSS or unsupported relocation is accepted.

| Candidate | Current limit | Next concrete check |
|---|---|---|
| GXFifo / GXMisc | Retained callbacks reference BSS; some name anchors are wrong | Establish complete BSS ownership and layout without accepting zero-filled data as proof |
| GXInit | Probe mismatches include stale names and possible version drift | Re-anchor by instruction identity and compare retained functions before choosing any source edits |
| OSCache / OSMemory | Pooled data, reset records and additional call names need proof | Establish complete sections and independently resolve their relocations; OS was rebuilt in May 2004 |

Continue library by library with the same full-unit and linked-range gates.
Do not infer that `NO_ANCHOR` proves absence, or that `MISMATCH` proves version
drift: stale names and dead stripping produced both kinds of misleading row.
