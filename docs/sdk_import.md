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
