# OpenSaga matching lane

Use OpenSaga routinely alongside CrashWOC and Mac evidence when preparing a
Nu2 target. Start from GC assembly and current symbols, consult the relevant
lane evidence, then write and measure a GC candidate with ProDG 3.9.3. The
read-only donor is `opensagadev/saga`, pinned at
`7bb2402aa902d08f0979892ca23ee80e809e5ee1`: Android x86 TCS, reconstructed
headers, no DWARF. Donor names, layouts, compiler behavior and percentages do
not establish GC identity. Do not execute donor scripts or copy donor source.

The [repository-wide donor audit](opensaga_audit.md) records the remaining
string/math, AI, accessor and cutscene leads, plus observed semantic differences.
Use its queue after the parser/helpers below; shared names are not matches.

## Evidence and initial queue

[File/parser evidence](opensaga_file_parser_evidence.md) supplies the first
small targets. This is a discovery queue, not a claim that targets have split
units or verified names. Check `next_target.py`, current symbols, rejected
renames and worktree ownership before claiming one target. Establish a
NonMatching comparison unit from real boundaries if needed; do not invent a
split to improve a score.

| Order | Current GC symbol / address | First question |
| --- | --- | --- |
| 1 | `NuFileReadWChar` / `0x8000596C` (`0x34`) | Confirm the 16-bit read wrapper, its return width and relocation to `NuFileRead`. |
| 2 | `NuFileReadChar` / `0x80005934` (`0x38`) | Confirm the byte read wrapper and signed return extension. |
| 3 | `NuFileExists` / `0x8000A3D0` (`0x3C`) | Match the open/close existence check; donor file-size behavior differs. |
| 4 | `NuFileOpenSize` / `0x800056E0` (`0x84`) | Confirm file-size semantics and handle-dependent branches. |
| 5 | `NuAllocFPar` / `0x80006B14` (`0x164`) | Match allocation using three GC parser slots and fixed buffers. |
| 6 | `NuFParCreate` / `0x80006C78` (`0x15C`) | Use confirmed helper signatures to match BOM handling and initialization. |

Score with `score_fn.py --compiler prodg`, retain improvements with
`hillclimb.py try` (include edited shared headers via `--source`), and accept
only with `verify_fn.py NAME --unit UNIT --build --strict-symbols`.
Keep baseline/best scores, addresses, exact verification commands and remaining
mismatches in the matching PR. Park a plateau with one concrete next hypothesis.
The initial queue is now PROMOTED; see [matching results](opensaga_matching_results.md)
for ten new strict matches, twelve promoted functions, commands and the next
queue. Historical tool-availability and build figures describe those runs;
bootstrap and measure the current checkout instead of treating them as blockers.

## Shared GC layouts

[`src/nucore/nufpar.h`](../../src/nucore/nufpar.h) records only GC-supported
fields. Retail `NuAllocFPar` uses `lwzx` at `0x80006BD4` and `stwx` at
`0x80006C04` on slot offset zero for the buffer pointer; its loop increments
slot offsets by `0x54C`, buffers by `0x1000`, and visits three slots.
`NuFParCreate` supplies the other accesses listed in the parser evidence:

| Offset | Header member | Evidence |
| --- | --- | --- |
| `0x000` | `fileBuffer` | allocator pointer load/store |
| `0x004` | `file` | file handle store |
| `0x50C` | `lineNum` | 32-bit `-1` initialization |
| `0x510`, `0x514`, `0x518` | `linePos`, `charPos`, `bufferStart` | line/byte reader accesses |
| `0x51C` | `bufferEnd` | refill limit, 32-bit `-1` initialization |
| `0x520` | `commandStack[8]` | indexed pointer store in push, command table dispatch |
| `0x540` | `commandPos` | stack push/pop, 32-bit `-1` initialization |
| `0x544` | `fileSize` | 32-bit size store |
| `0x548`, `0x549` | `isUtf16`, `isUtf8` | byte BOM flags |

The complete stride is `0x54C` on GC's 32-bit ABI. Opaque ranges deliberately
include the filename starting at `+8` and the word buffer starting at `+0x30A`:
those start offsets do not prove their extents. Do not import the donor's
separator pointers or four-slot pool. Confirm additional widths, extents and
signedness from consumers before replacing opaque bytes. The header is layout
evidence, not a strict code match.

The [animation evidence](opensaga_animation_evidence.md) corroborates the
existing shared `nuanim.h` legacy key/curve/time layouts; reuse those types.
GC mask-byte indexing differs from the donor. New-format type 1/2 key layouts
do not transfer. [AI script evidence](opensaga_ai_script_evidence.md) records
an 8-byte parameter element at script `+0x18`, but does not establish the full
script object layout; keep unproven fields out of shared definitions.

## Expansion gate

The gate is to demonstrate strict matches in the parser/helper queue with the
pinned ProDG toolchain and resolved references, record the commands and results,
and only then widen the lane to AI keyword handlers, legacy animation or
archive/DAT routines. The existing AI `69/78` and animation `43/105` drafts are
parked hypotheses.

**Passed for parser/helpers:** the [recorded strict matches and promotion](opensaga_matching_results.md)
select the next small target. This does not validate other donor layouts or turn
the parked AI/animation drafts into matches. Do not split a
branch out of a larger retail function to claim a match. A strict function
match is VERIFIED; promotion still requires the whole-unit section,
relocation and linked-range checks in [C matching](../code_matching_workflow.md#promotion).
