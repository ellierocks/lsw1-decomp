# Evidence task pack: `NuInitHardware`

- Address: `0x80038A98`
- Size: `0xB50` (from `symbols.txt`)
- Assembly source: `build/GL5E4F/asm/auto_01_8001752C_text.s`

## Stack/register observations

- Prologue stack allocation: `stwu r1, -0xe0(r1)`.
- General-purpose registers mentioned in extracted assembly: `r0`, `r1`, `r3`, `r4`, `r5`, `r6`, `r7`, `r8`, `r9`, `r10`, `r11`, `r12`, `r16`, `r17`, `r18`, `r19`, `r20`, `r21`, `r22`, `r23`, `r24`, `r25`, `r26`, `r27`, `r28`, `r29`, `r30`, `r31`.
- These are mechanical assembly observations, not an inferred ABI or signature.

## Direct call evidence

- Direct callees: **23**; direct callers: **1**.
- `direct_callees.tsv` and `direct_callers.tsv` are derived from `docs/recomp_exports/call_graph.tsv` using PPC `bl`/`bla` evidence.
- The legacy `docs/symbol_donors/call_graph.tsv` is intentionally not used here.

## Platform-boundary calls

- `NuDatFileOpenSize` at callsite `0x800393EC` (instruction=0x4BFCC119; direct_target=0x80005504).
- `NuGlassProcessScene` at callsite `0x8003944C` (instruction=0x4BFEAD11; direct_target=0x8002415C).
- `NuAnimInit` at callsite `0x800394AC` (instruction=0x4BFDF5C5; direct_target=0x80018A70).
- `NuSoundInitV` at callsite `0x800394F0` (instruction=0x4800AAB1; direct_target=0x80043FA0).
- `AXFXSetHooks` at callsite `0x800395B4` (instruction=0x4BFEE0A9; direct_target=0x8002765C).
- `NuRndrFootPrints` at callsite `0x800395C0` (instruction=0x4BFEC4B1; direct_target=0x80025A70).

## Data/xref evidence

- Existing `gc_data_xrefs.tsv` rows: **0**; copied verbatim to `data_xrefs.tsv`.
- Existing `gc_dol_string_renames.tsv` rows: **1**; copied verbatim to `string_xrefs.tsv`.
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
