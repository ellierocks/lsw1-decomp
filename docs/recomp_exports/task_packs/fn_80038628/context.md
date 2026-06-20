# Evidence task pack: `fn_80038628`

- Address: `0x80038628`
- Size: `0x470` (from `symbols.txt`)
- Assembly source: `build/GL5E4F/asm/auto_01_8001752C_text.s`

## Stack/register observations

- Prologue stack allocation: `stwu r1, -0x50(r1)`.
- General-purpose registers mentioned in extracted assembly: `r0`, `r1`, `r3`, `r4`, `r5`, `r6`, `r7`, `r8`, `r9`, `r10`, `r11`, `r12`, `r25`, `r26`, `r27`, `r28`, `r29`, `r30`, `r31`.
- These are mechanical assembly observations, not an inferred ABI or signature.

## Direct call evidence

- Direct callees: **39**; direct callers: **1**.
- `direct_callees.tsv` and `direct_callers.tsv` are derived from `docs/recomp_exports/call_graph.tsv` using PPC `bl`/`bla` evidence.
- The legacy `docs/symbol_donors/call_graph.tsv` is intentionally not used here.

## Platform-boundary calls

- `OSExceptionInit` at callsite `0x80038658` (instruction=0x48123B3D; direct_target=0x8015C194).
- `PADInit` at callsite `0x80038660` (instruction=0x481366D9; direct_target=0x8016ED38).
- `GXInit` at callsite `0x80038674` (instruction=0x4814455D; direct_target=0x8017CBD0).
- `PADRecalibrate` at callsite `0x800386B0` (instruction=0x48136575; direct_target=0x8016EC24).
- `DVDSetAutoInvalidation` at callsite `0x800386C0` (instruction=0x48131B91; direct_target=0x8016A250).
- `OSGetArenaHi` at callsite `0x800386D4` (instruction=0x48124F15; direct_target=0x8015D5E8).
- `OSReferentSize` at callsite `0x800386E4` (instruction=0x48124EFD; direct_target=0x8015D5E0).
- `OSInitAlloc` at callsite `0x800386F8` (instruction=0x48124DE9; direct_target=0x8015D4E0).
- `OSSetArenaHi` at callsite `0x80038764` (instruction=0x48124E95; direct_target=0x8015D5F8).
- `VIGetTvFormat` at callsite `0x80038804` (instruction=0x48135571; direct_target=0x8016DD74).
- `VIGetTvFormat` at callsite `0x80038810` (instruction=0x48135565; direct_target=0x8016DD74).
- `OSGetLanguage` at callsite `0x8003881C` (instruction=0x4812972D; direct_target=0x80161F48).

## Data/xref evidence

- Existing `gc_data_xrefs.tsv` rows: **4**; copied verbatim to `data_xrefs.tsv`.
- No string interpretation is asserted by this pack.

## Confidence notes

- HIGH: direct caller/callee rows include a PPC `bl`/`bla` instruction and direct target from the recomp callgraph export.
- MED: nearby symbols and existing data-xref table rows provide locality/reference evidence only.
- LOW: any functional description or rename hypothesis below is unaccepted and requires independent review.

## Suggested rename candidates — UNACCEPTED

- `GamePlatformInit` — LOW; motivated only by the direct calls to OS/GX/PAD initialization-named functions.
- `GameStartupSetup` — LOW; same limited evidence.
- Keep the current name `fn_80038628`; this pack does not authorize a rename.

## Verification/build

```sh
python3 configure.py
ninja build/GL5E4F/main.dol
# Then compare the relevant object/function in objdiff.
```
