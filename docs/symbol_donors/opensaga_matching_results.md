# OpenSaga parser/helper matching results

The first [OpenSaga lane](opensaga_matching.md) pass converts GC assembly into
ProDG 3.9.3 C using the donor pinned at
`7bb2402aa902d08f0979892ca23ee80e809e5ee1` as semantic evidence. No donor
scripts were executed or donor source copied. These are strict matches and
promoted retail ranges, separately from symbol recovery.

## Matched and promoted code

| GC function | Address | Bytes | First / best exact instructions |
| --- | --- | --- | --- |
| `NuFileReadChar` | `0x80005934` | `0x38` | 14/14 → 14/14 |
| `NuFileReadWChar` | `0x8000596C` | `0x34` | 13/13 → 13/13 |
| `NuFileExists` | `0x8000A3D0` | `0x3C` | 15/15 → 15/15 |
| `NuFileOpenSize` | `0x800056E0` | `0x84` | 21/33 → 33/33 |
| `NuAllocFPar` | `0x80006B14` | `0x164` | 89/89 → 89/89 |
| `NuFParCreate` | `0x80006C78` | `0x15C` | 81/87 → 87/87 |
| `NuFParPushCom` | `0x80006DD4` | `0x30` | 9/12 → 12/12 |
| `NuFParPopCom` | `0x80006E04` | `0x18` | 6/6 → 6/6 |
| `NuFParGetInt` | `0x80006E1C` | `0xB4` | 45/45 → 45/45 |
| `NuFParGetFloat` | `0x80006ED0` | `0x74` | 25/29 → 29/29 |

The first two readers already in `nufile.c`, `NuFileReadInt` (`0x800058DC`)
and `NuFileReadShort` (`0x80005908`), also pass strict verification (11/11
instructions each). All twelve functions are now linked from C: **1,460 bytes
promoted**, including 88 bytes from those existing comparison candidates.
Ten functions / 1,372 bytes are newly matched. No data bytes are promoted.

The partial source units follow existing whole-function boundaries:

| Unit | Owned `.text` retail range | Bytes |
| --- | --- | --- |
| `nucore/nufile` | `0x800058DC`–`0x800059A0` | 196 |
| `nucore/nufile_size` | `0x800056E0`–`0x80005764` | 132 |
| `nucore/nufile_exists` | `0x8000A3D0`–`0x8000A40C` | 60 |
| `nucore/nufpar` | `0x80006B14`–`0x80006F44` | 1,072 |

These are partial retail TUs, not claims that all of `nufile.c` or `nufpar.c`
has been recovered. Parser allocator/create/stack/value functions share one
contiguous comparison unit. The float reader references the existing retail
zero at `lbl_8018D394`; its surrounding constant pool remains retail-owned.
No symbol or constant-pool boundary was changed to obtain a match.

## What transferred and what did not

Donor locations: `src/nu2api/nucore/nucore_includes.c` contains read wrappers
(around lines 2151–2157), open-handle size dispatch (1428), numeric parser
readers (4040–4063), command stack helpers (4157–4213), allocator and parser
open/create (4346–4444). `src/nu2api/nufile/nufpar.h` provides semantic
leads, not the GC layout.

- Byte read is zero-initialized and sign-extended; wide read is zero-initialized
  and unsigned. The existing signed short reader is not interchangeable.
- GC open-handle size dispatch uses 32-bit results, 40-byte disk slots, and
  **20-byte** memory-file slots. The shared `nufile.h` now retains the unknown
  trailing four bytes rather than imposing its former 16-byte stride.
- `NuFileExists` opens and closes, unlike donor `NuFileSize(name) > 0`.
  Its GC close callee remains `fn_80005764`; existing `NuFileClose` at
  `0x80003C34` is a different target. No conflicting rename was applied.
- GC parser allocation uses three slots of `0x54C`, not four, and `0x1000`
  buffers. The allocator's implicit/old-style `memset` declaration reproduces
  retail CR1 clearing; a normal prototype changes its calling sequence.
- `NuFParCreate` combines creation/opening and needs the retail shared failure
  tail. Store order resolved scheduling differences. **Corrected return type:**
  `NuStrCpy` returns a 32-bit count, not a pointer; the non-null GC path counts
  the terminating NUL, unlike Saga. Parser callers discard the result. See the
  [audit evidence](opensaga_audit.md#nustrcpy-return-contract). No flags, pins
  or verifier rules changed.
- GC has a single eight-entry command stack at `+0x520`; donor dual-stack
  fields do not transfer. **Corrected initial evidence:** `+0x51C` is the
  buffer end, `+0x540` the command position. Refill code at `0x80006080`–
  `0x80006108` establishes `charPos` (`+0x514`), `bufferStart` (`+0x518`)
  and `bufferEnd` (`+0x51C`). Push/pop at `0x80006DD4`/`0x80006E04` prove
  the stack index. Shared headers carry those facts; unproven buffer extents
  stay opaque.
- Numeric readers use the word buffer at `+0x30A`, copying to a 64-byte local
  with Unicode conversion when needed. Integer syntax includes `$`, `0x` and
  `0X` prefixes; float parsing returns the shared zero for an empty word.

Two contradicted callee names were corrected, without claiming their code as
matched: `NuGetFrameAdvance` → `NuFParGetWord` (`0x800068F4`), and
`NuGSceneDestroy` → `NuUnicodeToAscii` (`0x800080D4`). The former reads parser
line/word buffers at `+0x108`/`+0x30A`, handles quotes, spaces, tabs and commas,
and dispatches on UTF16. The latter loads 16-bit source characters and writes
bytes, including U+2018/2019/2013/2026/2122 substitutions to 91/92/96/85/99 hex.
These bodies and their strictly verified numeric-reader callers contradict the
old names. Rejected names are recorded in `rejected_renames.tsv`.

## Repeatable acceptance and promotion checks

Run `bash tools/bootstrap.sh` and `bash build.sh` with the pinned retail DOL.
For each function in the table (and the existing two readers), use its unit:

```sh
python3 tools/score_fn.py NAME src/UNIT.c --compiler prodg
python3 tools/hillclimb.py try NAME --unit UNIT --source src/UNIT.c \
  --source src/nucore/nufile.h --source src/nucore/nufpar.h \
  --hypothesis 'one GC-supported source choice'
python3 tools/verify_fn.py NAME --unit UNIT --build --strict-symbols
```

Every final function returned strict `MATCH`. All four units passed:

```sh
python3 tools/verify_unit.py nucore/nufile --build
python3 tools/verify_unit.py nucore/nufile_size --build
python3 tools/verify_unit.py nucore/nufile_exists --build
python3 tools/verify_unit.py nucore/nufpar --build
bash build.sh
python3 tools/matching_progress.py
```

The unit gate compares complete allocated sections and canonical relocations
against independently checked retail bytes. After marking the four units
`Matching`, linked-range checks pass for all promoted units. The entire
relinked DOL is byte-identical to both the pre-promotion build and retail.
`GenuineProdgTests.test_opensaga_parser_and_file_helpers_match_strictly` repeats
function and unit proofs in retail CI.

At this pass's base, objdiff reported 78,872 / 1,569,612 bytes, 387 / 4,150
functions (5.024936%). Afterward: **80,244 bytes, 397 functions (5.112346%)**.
Raw drafts, ledgers and reports stay under ignored `work/`.

## Next queue and limits

The strict-match expansion gate is satisfied. Prefer the remaining parser
helpers before opening larger subsystems:

1. `NuFParGetWord` (`0x800068F4`, `0x100`) and wide counterpart
   `fn_80006830` (`0xC4`): compare the GC masked indices and quote behavior
   with the donor. No C attempt in this pass.
2. `NuUnicodeToAscii` (`0x800080D4`, `0xF8`): independently confirmed body,
   including punctuation mapping; no C attempt in this pass.
3. Parser line readers (`0x8000606C`, `0x800061D8`, `0x80006398`) and
   interpreter (`0x800069F4`): larger branches and UTF16 behavior need bounded
   searches. No C attempt in this pass.
4. Resume [AI PARAM](opensaga_ai_script_evidence.md) from its 69/78 draft
   with the corrected shared parser layout. Next hypothesis: shorten the
   destination pointer's live range to fix the parser/destination register
   swap. Reestablish its baseline before comparing historical scores.
5. Resume [legacy animation](opensaga_animation_evidence.md) at 43/105;
   test the shared-tail index lifetime. GC mask-byte order differs, and new
   curve-format type 1/2 layouts remain rejected transfers.
6. DAT/archive bodies need individual GC comparisons; donor names alone do
   not justify importing their layouts or calling them matched.

This pass ends after the initial queue and adjacent numeric/stack helpers,
with account usage above the 80% advisory threshold. Other donor subsystems
have not been exhaustively mined; no untouched target is claimed as a plateau.
