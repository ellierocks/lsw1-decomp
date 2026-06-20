# Evidence task pack: `fn_8009CC00`

- Address: `0x8009CC00`
- Size: `0x14C` (from `symbols.txt`)
- Assembly source: `build/GL5E4F/asm/auto_01_8001752C_text.s`

## Stack/register observations

- Prologue stack allocation: `stwu r1, -0x28(r1)`.
- General-purpose registers mentioned in extracted assembly: `r0`, `r1`, `r3`, `r4`, `r5`, `r6`, `r7`, `r8`, `r9`, `r11`, `r29`, `r30`.
- These are mechanical assembly observations, not an inferred ABI or signature.

## Direct call evidence

- Direct callees: **11**; direct callers: **1**.
- `direct_callees.tsv` and `direct_callers.tsv` are derived from `docs/recomp_exports/call_graph.tsv` using PPC `bl`/`bla` evidence.
- The legacy `docs/symbol_donors/call_graph.tsv` is intentionally not used here.

## Platform-boundary calls

- `NuInitHardware` at callsite `0x8009CCD0` (instruction=0x4BF9BDC9; direct_target=0x80038A98).
- `NuCameraCreate` at callsite `0x8009CCD4` (instruction=0x4BF86FFD; direct_target=0x80023CD0).

## Data/xref evidence

- Existing `gc_data_xrefs.tsv` rows: **0**; copied verbatim to `data_xrefs.tsv`.
- Existing `gc_dol_string_renames.tsv` rows: **0**; copied verbatim to `string_xrefs.tsv`.
- No string interpretation is asserted by this pack.

## Confidence notes

- HIGH: direct caller/callee rows include a PPC `bl`/`bla` instruction and direct target from the recomp callgraph export.
- MED: nearby symbols and existing data-xref table rows provide locality/reference evidence only.
- LOW: any functional description or rename hypothesis below is unaccepted and requires independent review.

## Suggested rename candidates — UNACCEPTED

- No rename proposal is warranted by this evidence-only pack.

## Verification/build

```sh
python3 configure.py
ninja build/GL5E4F/main.dol
# Then compare the relevant object/function in objdiff.
```
