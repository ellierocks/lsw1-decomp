# OpenSaga AI script parser evidence

Lane evidence for the AI script parser keyword handlers. Donor source is the
read-only clone of `opensagadev/saga` at commit
`7bb2402aa902d08f0979892ca23ee80e809e5ee1` (Android x86 TCS, reconstructed
headers, no DWARF). Donor names, types and matching percentages are leads only;
they do not establish LSW1 identity. Target evidence is the retail GC DOL
`GL5E4F` through `tools/score_fn.py`.

Current parser helpers and layout corrections are in
[matching results](opensaga_matching_results.md). `NuGetFrameAdvance` is now
`NuFParGetWord`; the numeric readers are promoted. The PARAM measurements below
remain a historical, unverified draft.

## Target: `ScriptKeyword_PARAM` at `0x8007CAFC`

| Field | Value |
| --- | --- |
| Symbol (`config/GL5E4F/symbols.txt`) | `ScriptKeyword_PARAM = .text:0x8007CAFC`, size `0x138` |
| Source | none yet; no unit in `configure.py` or `splits.txt` |
| Compiler | ProDG `3.9.3` (`prodg`), `score_fn.py --compiler prodg` |
| Status | **not VERIFIED**; best draft 69/78 exact instructions |

### Identity evidence

- Donor keyword table `cfgtab_Script` (`aisysall.cpp:713-722`) maps
  `"PARAM"` to `xParam`. `xParam` (`aisysall.cpp:6202`) rejects a parser index
  `>= 4`, reads a word, then stores `params[idx].name` and
  `params[idx].default_val` from a float. The GC body does the same, in the
  same order: null check on the global, `idx > 3` early return, word read
  returning 0 early return, copy into the shared buffer, name store, then
  default-float store.
- GC stride: `params[idx]` addressed as `global + 0x18 + idx*8`
  (`slwi r30, r26, 3`, `stwx`, `stfsx` at `+0x1C`). So `name` is at `+0x0`
  and `default_val` at `+0x4` inside an 8-byte element, starting at `+0x18`
  of the script object.
- The copy is inlined (no call to a separate helper exists at the target
  address), matching donor `AIScriptCopyString`(`aisysall.cpp:626`) and
  `AIScriptBufferAlloc`(`aisysall.cpp:607`). The GC allocator aligns the
  cursor to 16 with `(addr + 0xf) & ~0xf`, and does not call `memset`, unlike
  the donor. Error strings `str_SourceFile_AIScriptC` (line `0x2D4` for
  too-small, `0x2D7` for no-buffer) are in `.rodata`.
- `docs/scripts.md` lists `ScriptKeyword_*` parsers as the `PARAM`/`STATE`
  keyword group. Consistent with the donor table.

### Conflicts found (do not apply without target evidence)

- `docs/symbol_donors/call_graph_rename_suggestions.md:80` suggests
  `fn_8007CC34` (size `0x220`) is `ScriptKeyword_PARAM`. This conflicts with
  `symbols.txt`, which names `ScriptKeyword_PARAM` at `0x8007CAFC` (size
  `0x138`, matching the `.fn` boundary in the split asm). The suggestion is
  stale for this address; `fn_8007CC34` is a separate function (`0x220` bytes)
  that this lane did not analyse.
- `NuGetFrameAdvance` (`0x800068F4`) is called as the word reader by both
  `fn_80006E1C` and `ScriptKeyword_PARAM`, which is donor `NuFParGetWord`
  behaviour. The current name looks wrong. Not renamed here; the file-parser
  lane should decide.
- `docs/symbol_donors/mac_anchor_rename_queue.md:191-192` (LOW) suggests
  `fn_80006E1C` = `NuVpGetClippingMtx` and `fn_80006ED0` = `NuVpGetScalingMtx`.
  Their use here (an integer index result and a float result taking the parser)
  does not fit those names. Treat both as unnamed callees.
- Data labels: `lbl_80408CC0` is used as the `AISCRIPT *load_aiscript` global
  (donor `aisysall.cpp:586`), `lbl_80408CB8`/`lbl_80408CBC` as the
  `VARIPTR *load_buff`/`load_endbuff` pair (donor `:584-585`). These are donor
  names with matching GC access patterns; they are not renamed.

## Measurement

Baseline: `bash build.sh` on this branch succeeded. Code matched
`1.6467806%` (`25,848 / 1,569,608` bytes), `149 / 4,149` functions, before
this lane. No tracked config, split or source change was made.

Draft attempts, scored with `tools/score_fn.py ScriptKeyword_PARAM <draft>
--compiler prodg`. Lower score is better; `exact` is matching instructions out
of 78.

| Draft | Change | Result |
| --- | --- | --- |
| v1 | straightforward first draft, inline alloc after `strlen` | 58/78 |
| v2 | buffer globals loaded before word check; `word` local; `+1` after length test | 67/78 |
| v3 | v2 with success branch as `buf->addr + len < buf_end->addr` | **69/78 (best)** |
| v4 | v3 with `dst` declared before `word` | 69/78 |
| v5 | v3 with `dst` initialised at declaration | 66/78 |
| v6 | v3 with `parser->word_buf` used directly | 66/78 |
| v7 | v3 with buffer globals loaded inside the word branch | 67/78 |

### Remaining mismatch (best draft v3)

First difference is at instruction 5: retail `mr r28, r3` (`7C7C1B78`), mine
`mr r29, r3`. The register roles are swapped: retail keeps the parser in
`r28` and `dst` in `r29`; mine has `dst` in `r28`. Everything after that is
the same instruction shape with swapped registers, so the exact count
undercounts. The structure (control flow, call order, offsets, store order) is
otherwise the same. Declaration order and initialisation did not move the
allocation.

Best draft (v3), kept local at `build/scratch/aiscript_param_v3.c`
(not committed):

```c
void ScriptKeyword_PARAM(NUFPAR *parser) {
    u32 param_idx;
    char *word;
    char *dst;
    usize len;
    VARIPTR *buf;
    VARIPTR *buf_end;

    if (lbl_80408CC0 == NULL) {
        return;
    }
    param_idx = fn_80006E1C(parser);
    if (param_idx > 3) {
        return;
    }
    if (NuGetFrameAdvance(parser) == 0) {
        return;
    }

    dst = NULL;
    buf = lbl_80408CB8;
    buf_end = lbl_80408CBC;
    word = parser->word_buf;
    if (word != NULL) {
        len = NuStrLen(word);
        if (len != 0) {
            len += 1;
            if (buf != NULL && buf_end != NULL) {
                if (buf->addr + len < buf_end->addr) {
                    dst = (char *)((buf->addr + 0xf) & ~0xf);
                    buf->addr = (usize)dst + len;
                } else {
                    (*fn_80009948(str_SourceFile_AIScriptC, 0x2d4))(str_AIScriptBufferAlloc_BufferTooSmall);
                }
            } else {
                (*fn_80009948(str_SourceFile_AIScriptC, 0x2d7))(str_AIScriptBufferAlloc_NoBuffer);
            }
            NuStrCpy(dst, word);
        }
    }
    lbl_80408CC0->params[param_idx].name = dst;
    lbl_80408CC0->params[param_idx].default_val = fn_80006ED0(parser);
}
```

Required scratch types (not in the repo yet): `NUFPAR` with `word_buf` at
`+0x30A`, `VARIPTR` with `addr` at `+0`, and `AISCRIPT` with `params[4]` at
`+0x18` of 8-byte `{char *name; f32 default_val;}` elements.

## Next hypothesis

Retail allocates the parser pointer to a callee-saved register that ProDG
assigns before `dst`. The next thing to test is an ordering that gives
`parser` the lower-numbered register: for example, copy `parser` into a local
before the first call, or give `dst` a live range that begins after the
parser copy. If that still fails, check whether the retail source keeps `dst`
as a `VARIPTR`-derived pointer (no separate local) so it is never a named
variable.

## Limits

- Not VERIFIED: no exact function match, so no `configure.py` unit, no
  `splits.txt` range, and no promotion. The verifier needs an exact function
  match first.
- Local tool setup: ProDG `3.9.3` and `dtk_patched` were copied into the
  gitignored `build/` of this worktree; `orig/GL5E4F/sys/main.dol` was copied
  from the main checkout. None are committed.
- Callee and data names are not verified beyond the symbols above.


## `aisys-1` target-derived layout and ABI evidence

The [2026-11-13 matching record](aisys_1_matching.md) adds 25 strict VERIFIED
functions (16,432 bytes), without unit promotion. These partial views are
confirmed by GC accesses and exact code generation; they do not import the
full OpenSaga layouts. Earlier parser experiments above remain historical.

- Script parameters start at `+0x18`, stride 8, containing a name pointer and
  float value. The processor script pointer is `+4`; its action timer is
  `+0x40`. The null fallback in the locator inline parser loads
  `lbl_8019BF58`; the other new inline parser uses a literal zero.
- Movement calls use the interleaved ABI
  `AIMoveInstruction(void *, void *, f32, void *, s32, f32)`. Packet owner
  and opponent pointers are `+0x50` and `+0x64`. The face handler reads
  positions at object `+0x6c` and writes the packet vector at `+0xc0`.
- The system creature array pointer is `+0x14`, stride `0xa8`; its final
  four floats occupy `+0x98..+0xa4`. Setter handlers select the creature
  using the packet byte at `+0xa6`, not the donor's `+0x134`.
- Path nodes have stride `0x58`, vector at `+4`, float at `+0x14`, byte
  connection count at `+0x28` and connection pointer at `+0x30`. The path's
  node-array pointer is `+0x78`. Connections contain node-index bytes at
  `+0/+1` and two flag words from `+4`.
- Circle direction is represented by the packet word at `+0x1a0` with
  bitfield widths `9/1/22`; logical negation generates the target's
  `mfcr`/`rlwimi` update. `Action_CirclePlayer` uses the player for movement
  and the opponent pointer for its final facing calculation.
  `Action_MoveAwayFromPlayer` has no facing store.
- Locator selection copies the pointer at `+0xfc` to `+0xb4`; flags use
  bytes `+0xb2/+0xb3`. The object byte at `+0x285` and squared
  `(radius + global)` range calculation are GC-specific. `Action_SetLocator`
  parses numeric parameters using packet `+4`, rather than the processor.
- Animation override writes signed 16-bit values at `+0xa2`, then `+0xa4`.
  The loader uses 128 names of stride 32, a 128-byte filename and a
  `0x10b0` stack frame. It repeats resolution per system after loading.

OpenSaga at the pin above corroborates parsing and traversal, but its newer
condition fields, logging, reset fields, extra flags and Idle frames extension
do not transfer. GC Idle's fallback uses `params[0]`, not donor `params[i]`.
The Japanese PC pair limits and seven unfinished structural hypotheses are
retained in the matching record. Unknown gameplay meanings remain unnamed;
no gameplay naming inference or runtime driving was needed.
