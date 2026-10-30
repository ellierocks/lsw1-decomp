# Symbol donor artifacts: provenance and staleness

Most files here are either **donor evidence** (names and data taken from other
binaries, which are not affected by renames in `config/GL5E4F/symbols.txt`) or
**generated research output** that embeds GC symbol names as they were when the
tool last ran. Generated output is kept as research history and is not
refreshed on every rename. A name printed in a generated file is not evidence
for what a function is.

## OpenSaga

The [OpenSaga matching lane](opensaga_matching.md) links the pinned parser, AI
and animation evidence, the initial small-target queue and shared GC layouts.
It records the strict-match gate for expanding this donor lane. The
[matching results](opensaga_matching_results.md) record the promoted parser and
file helpers, the corrected parser layout and the next queue.

## Names corrected by PR #5 (`e7bbacf`)

The 18 address mappings are in
[animation_restart_evidence.md](animation_restart_evidence.md). The names that
were removed or rejected per address are in machine-readable form in
[rejected_renames.tsv](rejected_renames.tsv).

Treat any pre-correction name at these addresses as wrong. In stale files the
six rows that were `fn_` placeholders show `fn_XXXXXXXX`; the other twelve show
the old wrong name.

| Address | Name in stale artifacts | Correct name |
|---|---|---|
| `0x80017EA0` | `NuAnimCurve2SetApplyToJoint` | `NuAnimCurve2SetApplyToJointTransLoc` |
| `0x80018E64` | `fn_80018E64` | `NuAnimCurve2SetApplyToJoint` |
| `0x8001752C` | `NuAnimCurve2CalcValEx` | `NuAnimCurve2SetApplyBlendToJoint2TransLoc` |
| `0x80018E98` | `fn_80018E98` | `NuAnimCurve2SetApplyBlendToJoint2` |
| `0x80018558` | `NuAnimDataRead` | `NuAnimCurveSetApplyToMatrix` |
| `0x800187F4` | `NuAnimDataFindVersion` | `NuAnimCurve2SetApplyToMatrix` |
| `0x80018F78` | `NuAnimData2LoadBuffEx` | `NuHGobjEval` |
| `0x80019248` | `NuAnimData2FixPtrs` | `NuHGobjEvalAnim2Root` |
| `0x80019634` | `NuQuatSlerp` | `NuHGobjEvalDwa2` |
| `0x800197B8` | `NuAnimCurveSetApplyToMatrix` | `NuHGobjEvalDwaBlend2` |
| `0x80019A54` | `fn_80019A54` | `NuHGobjEvalAnimBlend2Root` |
| `0x80019F08` | `fn_80019F08` | `NuHGobjEvalAnim2` |
| `0x80019F30` | `fn_80019F30` | `NuHGobjEvalAnimBlend2` |
| `0x80018EF8` | `fn_80018EF8` | `NuAnimDataChunkCreate` |
| `0x80021784` | `NuAnimCurve2SetApplyBlendToJoint2TransLoc` | `fn_80021784` |
| `0x800225C4` | `NuAnimCurve2SetApplyToJointTransLoc` | `fn_800225C4` |
| `0x80022A88` | `NuAnimCurve2SetApplyToMatrix` | `fn_80022A88` |
| `0x8002303C` | `NuAnimCurve2SetApplyBlendToJoint2` | `fn_8002303C` |

## Generated from `symbols.txt` names (stale, not regenerated)

Embed GC symbol names for some or all of the 18 addresses above. Left as
committed so earlier research is preserved.

| File | Generator | Where the old names appear |
|---|---|---|
| `call_graph.tsv`, `call_graph_rename_suggestions.md`, `named_to_unnamed_callees.md` | `tools/call_graph_extractor.py` | caller/callee name columns, callee lists |
| `../recomp_exports/call_graph.tsv` | `tools/export_recomp_call_graph.py` | caller/callee name columns |
| `mac_anchor_rename_queue.md/.tsv` | `tools/mac_anchor_rename_queue.py` | `left_anchor` of the `0x800234A8` and `0x80023628` rows is the old name of `0x8002303C` |
| `gc_data_xrefs.tsv`, `mac_data_to_gc_verified.md` | `tools/gc_data_xref_scanner.py` | referencing-function names |
| `nu2_fn_matches.tsv`, `nu2_gc_body_confirmations.tsv` | `tools/nu2_fn_match.py` | GC name column (`fn_` placeholders now named) |
| `proposed_splits.md/.txt` | `tools/source_cluster.py` | function lists (`fn_` placeholders now named) |
| `fingerprint_matches.md` | `tools/archive/fingerprint_matcher.py` | match overlap names |
| `../../tasks/fn_80017EA0/`, `fn_8001752C/`, `fn_80018E64/`, `fn_80018DA8/`, `index.md` | `tools/ls1_task_pack.py` | task title and `related_symbols.md` callee names |
| `../../work/*.txt` rename batches | `tools/queue_to_batch.py` | `work/batch_medium.txt`, `mac_medium.txt`, `combined_batch.txt`, `sdk_batch.txt` still list wrong targets for `fn_80017EA0`, `fn_80019634`, `fn_800197B8` |

The `work/` batches are inputs to `tools/batch_rename.py`. Those `fn_` source
names no longer exist in `symbols.txt`, so the lines fail with `NOT FOUND`, and
`batch_rename.py` now also refuses any name listed in `rejected_renames.tsv`.

Not affected: files that list donor names for their own binaries
(`mac_*_symbols.tsv`, `mac_*_data_syms.tsv`, `crashwoc_*`,
`sdk_symbol_sources/`, `mac_gc_*`). A donor name such as `NuQuatSlerp` in a
Mac TSV is that binary's own symbol.

## Why these were not regenerated

Checked in a scratch copy of `main` with the local retail DOL:

- `tools/call_graph_extractor.py` runs from the DOL and `symbols.txt` alone, so
  it is reproducible, but the committed copies were generated from older
  symbols. Running it with the symbols from just before PR #5 still changes
  5,470 of 30,597 `call_graph.tsv` rows; running it with current symbols changes
  5,714. Only about 330 rows are due to the 18 PR #5 renames. Regenerating would
  therefore rewrite thousands of rows and sections of the suggestion reports
  that have nothing to do with the correction, and would drop the earlier
  research state those reports capture.
- The `mac-anchor-queue`, `nu2-body-matches` and `gc-data-xrefs` stages need
  Mac debug binaries and the CrashWOC/Nemo ELFs under `orig/mac/` and
  `orig/nu2/`. These are not present in a clean checkout, so those artifacts
  cannot be reproduced and cannot be partially refreshed without losing rows.

If a refresh is wanted later, regenerate everything for the same donor inputs in
one dedicated commit so the diff is reviewable, and keep the previous output in
git history.

## Rename suggestions

`tools/mac_anchor_rename_queue.py`, `tools/ls1_rename.py --suggest`,
`tools/ls1_rename.py <old> <new>` and `tools/batch_rename.py` read
`rejected_renames.tsv` and will not propose or apply a rejected name at that
address. The rejection is per address: `NuAnimCurve2SetApplyToJoint` is valid
at `0x80018E64` and rejected at `0x80017EA0`. Add a row when a rename is
reverted or a candidate is rejected with evidence.
Other queue generators (`nu2_fn_match.py`, `sdk_anchor_rename_queue.py`,
`crashwoc_*`) do not read it yet.
