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

## Remaining Phase 1 work

These ten promoted units do not complete every SDK archive. Remaining
SDK ranges retain assembly fallback. No other probe row is promoted by name
or layout alone, and no unverified BSS or unsupported relocation is accepted.

| Candidate | Current limit | Next concrete check |
|---|---|---|
| GXLight | `GXInitLightSpot` has identical code but an unresolved `cosf` reference | Verify the math implementation at `8018ACC8` independently before assigning that name |
| GXPixel | `GXSetFog` has an unresolved `__cvt_fp2unsigned` reference | Verify the MW runtime helper at `80153184`, then prove constants and the full unit |
| GXFifo / GXMisc | Retained callbacks reference BSS; some name anchors are wrong | Establish complete BSS ownership and layout without accepting zero-filled data as proof |
| GXFrameBuf / GXInit | Probe mismatches include stale names and possible version drift | Re-anchor by instruction identity and compare retained functions before choosing any source edits |
| OSCache / OSMemory | Pooled data, reset records and additional call names need proof | Establish complete sections and independently resolve their relocations; OS was rebuilt in May 2004 |
| CARDStat | `CARDSetStatusAsync` is code-identical except call bindings | Verify `__CARDGetControlBlock`, `__CARDGetDirBlock`, `__CARDUpdateDir` and the runtime divide helper independently |

Continue library by library with the same full-unit and linked-range gates.
Do not infer that `NO_ANCHOR` proves absence, or that `MISMATCH` proves version
drift: stale names and dead stripping produced both kinds of misleading row.
