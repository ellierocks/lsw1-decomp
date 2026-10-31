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

## End-to-end workflow

The pin is owned by [`config/donors/opensaga.json`](../../config/donors/opensaga.json).
The provenance in older evidence pages describes their original measurements;
changing the pin requires a deliberate donor refresh and rechecking affected
conclusions, not an automatic update to upstream HEAD.

1. **Bootstrap.** `bash tools/bootstrap.sh` fetches a detached checkout at the
   pin into `$LSW1_CACHE/opensaga` (default `~/.cache/lsw1/opensaga`). Set
   `OPENSAGA` to use an existing checkout. `LSW1_NO_OPENSAGA=1` skips the fetch
   for offline/SDK-only setup. Explicitly retry with
   `python3 tools/opensaga.py fetch`. Existing modified or differently pinned
   checkouts are preserved; choose a fresh `OPENSAGA` path to resolve them.
2. **Select.** `next_target.py` attaches donor metadata to each candidate and
   uses name leads only to break equal callee-count/size ties. It never ranks by
   donor percentage or changes ownership/split requirements. `lookup` accepts
   a current GC name or address:

   ```sh
   python3 tools/opensaga.py lookup NuStrCpy
   python3 tools/opensaga.py lookup 0x80007B6C --json
   python3 tools/opensaga.py search cfgtab_Script --limit 10
   ```

3. **Prepare.** Both individual and object-neighborhood task packs write
   `donor_evidence.md`: pin, source owners, URLs, lookup state and review
   instructions. Read the GC body first, then the donor body and relevant
   callees/keyword tables. Exact name lookup is deliberately conservative and
   can return several source owners; inspect all plausible owners. A no-hit
   is not proof of absence: search names, callees or strings. Unassigned report
   functions and differently named equivalents may need this manual search.
4. **Draft and record.** Write C from GC assembly, using the donor as semantic
   evidence. Before the first game/Nu2 `hillclimb.py try`, provide
   `--donor-note` with inspected paths, GC-supported semantics/offsets and
   rejected differences. If lookup is unavailable or unsuccessful, record the
   actual limitation and searches instead. The ledger automatically stores
   lookup metadata, pin and review note; later attempts inherit the note for
   that revision. A new pin needs a new note. Existing ledgers without a note
   need one on their next attempt. `dolphin/` SDK units are exempt.
5. **Verify and finish.** Fast scoring, strict function verification and unit/
   linked-range promotion are unchanged. Keep the donor conclusion in the PR
   or plateau handoff alongside baseline/best score, verification commands,
   promoted bytes and next hypothesis. Put newly proven reusable field facts
   in the owning shared header/evidence page; keep unproven donor layouts out.

All lookups after bootstrap are offline and read-only. Search returns paths
and line numbers, not copied source. Wrong revisions, modified checkouts and
missing reports are **unavailable**, not no-hit results. These states do not
change the oracle or prohibit work from GC evidence; record the limit.
The review note is a provenance requirement, not automated proof that the
agent's interpretation is correct. Review still checks it against GC.

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
