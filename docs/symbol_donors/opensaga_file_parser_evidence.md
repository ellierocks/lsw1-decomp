# OpenSaga file/parser lane evidence

Lane: text parser and archive/DAT helpers, from OpenSaga pinned at
`7bb2402aa902d08f0979892ca23ee80e809e5ee1` (read-only clone). The donor is
Android x86 TCS with reconstructed headers and no DWARF. Its identities,
types and percentages are leads only; every claim below rests on GC bytes or
GC rodata.

Current implementation and verification supersede the initial run below: see
[matching results](opensaga_matching_results.md). Consumer analysis corrects two
original field labels: buffer end is `+1308`; command-stack position is `+1344`.

## Outcome (initial research run)

- **Renamed** `0x80006C78` from `NuVpInit` to `NuFParCreate` (size `0x15C`).
  The GC body is the file-parser open path, not a viewport initializer.
- **Corrected** two more misnamed addresses to placeholders, because the
  existing names are contradicted by GC code: `0x800081CC` (`NuGetChar` →
  `fn_800081CC`) and `0x800D8038` (`NuFileGetEndianSwap` → `fn_800D8038`).
- Recorded the allocator at `0x80006B14` as the next parser target
  (`NuAllocFPar`, not renamed in this change).
- **Not VERIFIED, not PROMOTED.** No ProDG toolchain exists in the main checkout
  (`build/` holds only the patched dtk), so no C candidate was compiled or
  scored. `score_fn.py`, `hillclimb.py try` and `verify_fn.py --strict-symbols`
  were not run. The default build (`bash build.sh`) passes with the renames;
  it links retail code for unmatched functions, so it proves nothing about
  NuFParCreate's bytes.

## Method

- Target bytes: `orig/GL5E4F/sys/main.dol` (SHA-1 matches the pin; the
  worktree has a local symlink to the main checkout's copy, gitignored).
  Text sections were read with the DOL section table and disassembled with
  `llvm-mc --disassemble -triple=powerpc-eabi -mcpu=gekko`. No OpenSaga script
  was executed.
- TU path strings in rodata locate parser code. `../nu2api.legogc/nucore/ngc_obj/../nufpar.c`
  is at `0x8018D224`; `../nu2api.legogc/nucore/ngc_obj/../nufile.c` is at
  `0x8018CB9C`; `nustring.c` is at `0x8018D88C`.
- Calls and data references were cross-checked with `lis`/`addi` and `bl`
  scans over the text sections. Callers and callees are listed in
  `config/GL5E4F/symbols.txt` and `tools/ls1_lookup_symbol.py`.

## NuFParCreate (`0x80006C78`, 0x15C)

Body, in order:

1. `fn_8000A3D0(filename)` (`NuFileExists`-like: open read mode 0, close,
   return 1 or 0). Returns 0 → NULL.
2. `NuFileOpen(filename, 0)` (`NuFileOpen` at `0x800035F0`). Handle zero → NULL.
3. `fn_80006B14()` allocates the parser slot (see below). NULL → close the
   handle and return NULL (`fn_80005764` is the file close path).
4. Stores `file_handle` at `+4`, and `-1` at `+1292`, `+1308`, `+1344`
   (donor `line_num`, `buf_end`, `command_pos` initialization).
5. Reads a 16-bit BOM with `fn_8000596C` (`NuFileRead` size 2, returns u16,
   matching donor `NuFileReadWChar`). `0xFFFE` → `is_utf16 = 2`; `0xFEFF` →
   `is_utf16 = 1`; `0xBBEF` then byte `0xBF` (read via `fn_80005934`, a
   `NuFileRead` size 1 returning `i8`) → `is_utf8 = 1`; `0xEFBB` then `0xBF` →
   `is_utf8 = 2`. Flags at `+1352` (`is_utf16`) and `+1353` (`is_utf8`).
6. No BOM (both flags zero) → `NuFileSeek(file, 0, 0)`.
7. `size = fn_800056E0(file)` stored at `+1348` (32-bit store).
8. Success → `NuStrCpy(parser + 8, filename)` (file name at `+8`), return the
   parser. The donor's `separator_list` and `separator_tokens` are **not**
   written here.

Donor match: the control flow above equals donor `NuFParCreate` with
`NuFParOpen` inlined. The GC body does not reach the donor's
`NuFParCreateGivenFH` or `NuFParCreateMem` paths.

### Callers (GC bl sites)

These are parser consumers; they need the parser by filename:

- `AIScriptLoadScp` `0x8007D2B8`
- `AIScriptLoadScriptTxt` `0x8007E434`
- `CreditBufferAlloc` `0x8010BE90`
- `fn_80014EC0` `0x80014F40`, `fn_8006C65C` `0x8006C66C`, `fn_800E2944` `0x800E2A3C`

The AI script callers belong to the `feat-opensaga-ai-script-recovery` lane.
Do not claim those functions from this evidence.

### Rejected name

`NuVpInit` at `0x80006C78` has no viewport body in GC. The CrashWOC
`NuVpInit` at `0x800BB8E8` shares the size `0x15C` only. The name was removed
and recorded in `docs/symbol_donors/rejected_renames.tsv`.

## NuAllocFPar candidate (`fn_80006B14`, 0x164)

Next target for this lane. Not renamed here.

- Assert strings: `Could not allocate fpar array` (`0x8018D320`, line 923),
  `Could not allocate fpar buffers` (`0x8018D340`, line 928),
  `Too many File Parsers open at once - maximum = %d` (`0x8018D360`, line 951,
  count argument 3). All three are in `nufpar.c`. The count fixes
  `PARSER_COUNT = 3`, not the donor's 4.
- Lazy init inline (donor `NuFParInit`): when `fpars` is NULL, `memset(fpars,
  0, 4068)` on `lbl_802015C8`, set `fparbuffers = lbl_802025AC`, then two
  assert checks.
- Loop over `i < 3`: when `fpars[i].file_buf == NULL`, zero the slot, set
  `file_buf = fparbuffers + i*0x1000`, zero the buffer, return `&fpars[i]`.
  Struct stride is `0x54C` (1356).
- Sole code users of `lbl_802015C8` and `lbl_802025AC` are in this function.
- Only direct caller in the bl scan is `NuFParCreate` (`0x80006CAC` in the
  pre-rename body). The donor's `NuAllocFPar` is static, so this single call
  site is expected.

Layout facts from GC (not donor): `sizeof(NUFPAR)` is `0x54C`; `fpars` is
`3 * 0x54C = 0xFE4` bytes; the buffer pool is `3 * 0x1000 = 0x3000`. The GC
struct stride is smaller than the donor header's layout (which adds separator
pointers), so the donor header is not a layout source for GC `NUFPAR`.

Mac data labels for these addresses are wrong. `mac_data_to_gc_verified.md`
lists `nufile_blkinfo` at `lbl_802025AC`, but the GC code makes it the parser
file-buffer pool. The `fpars` name at `lbl_802015C8` fits the code.

## Other evidence checked

- `NuDatOpenEx` (`0x80004F6C`) and `NuPPUnpack` (`0x800085B0`) were not
  examined beyond their references. `nufile.c` string references sit inside
  `NuFileOpen`, `NuFilePos`, `NuFileClose`, `NuFileLoadBuffer`,
  `NuFileBeginBlkRead`, `NuDatFileLoadBufferLsn`, `NuDatFileLoadBuffer`,
  `NuDatFileOpen`, `NuDatOpenEx`, `NuFileEndBlockRead`. `NuDatOpenEx` remains a
  lead without a body comparison.
- `0x800081CC` (formerly `NuGetChar`) is a `nustring.c` UTF-8 to UTF-16
  decoder. It asserts `UTF8 character out of unicode range` at line 1493. It
  is not the donor byte reader. Renamed to `fn_800081CC`.
- `0x800D8038` (formerly `NuFileGetEndianSwap`, 0x62C) is game-side code. It
  calls `NuAngDiff` and game helpers. Renamed to `fn_800D8038`.
- Other lanes (`feat-animation-symbol-recovery`, `feat-sdk-import`,
  `feat-opensaga-ai-script-recovery`, `t3-68116529`) carry the same baseline
  names at `0x80006B14`, `0x80006C78`, `0x800081CC` in their `symbols.txt`
  and have no rename. The AI script lane owns the parser callers.

## Remaining limits

- No ProDG, so no strict verification and no C candidate.
- `NuFParCreate` body depends on `fn_8000A3D0`, `fn_8000596C`, `fn_80005934`,
  `fn_800056E0` and `fn_80005764`, which are unnamed in GC. Their identities
  need separate evidence before a source file can name them.
- Donor `NuFileExists` is `NuFileSize(name) > 0`. The GC `fn_8000A3D0` opens
  and closes the file, so the donor body is not reused.

## Next hypothesis

Write a `NuAllocFPar` candidate for `fn_80006B14` with the GC pool layout
(three parsers, `0x1000` buffers, struct stride `0x54C`) and verify it with
ProDG once the toolchain is installed. Then read `NuFParCreate`'s callee
stride offsets against the same struct before typing `NUFPAR` in any source.
