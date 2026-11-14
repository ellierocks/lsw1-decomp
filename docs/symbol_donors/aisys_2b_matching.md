# aisys-2b matching record, 2026-11-14

Follow-up to [aisys-2](aisys_2_matching.md), stacked on PR #174 at
`c39e0e74076ed1ffdf9fd23780ac5d8e58da1ee9`. Branch `t3/plan-aisys-2b`, bench run
`t3-c08da200-9cebbdadc00c8165-1794642623`. The four requested targets yielded
**one new strict VERIFIED function, 1,612 bytes; none PROMOTED**.

`AISysProcessCharacter` passes 403/403 instructions, relocations, strict symbol
references and referenced retail data. Recovering typed GC layouts, two 12-byte
vector locals, three-argument node refresh, the five-integer-argument path-position
call, literal float pools and the common movement tail produced the match.
The [owning evidence](opensaga_ai_script_evidence.md#aisys-2b-processor-layout-and-abi-evidence)
records the consumed offsets and ABI facts. No shared header was expanded.

The plan edit is confined to the aisys-2 note. The prior five parked functions
were not worked: fn_80083754, fn_800875F8, fn_80088AAC, fn_80088CD8 and
fn_8008911C. They appear as deferred in the finish tool's queue accounting only;
their previous parked state remains. No shape handler or other symbol was renamed.
Compiler pins, split boundaries and verification rules are unchanged.

## Bounded plateaus

Counts are distinct compiled codegen hashes recorded in this checkout, including
mechanical searches; repeated identical output counts once. These are retained
mismatching drafts, not accepted source or progress bytes.

| Function | Retained exact | Distinct candidates | Findings and next hypothesis |
|---|---:|---:|---|
| fn_80085A50 | 705/921 | 310 | Extracted repeated inline allocator; fixed independent version guards and typed the path-system pointer table. Next: typed path/node/connection loops, halfword reader merges and outer-index lifetime. |
| fn_80086A04 | 505/765 | 254 | Recovered stack arrays, four-argument nested loader, independent version gates, raw default-word copies and float stores. Next: typed system records and allocator cursor lifetimes; frame remains 0x2a0 versus 0x298. |
| AISysCharacterTestPathCnx | 375/771 | 257 | Recovered symmetric byte table, word object flag, byte inside-node stores, four local vectors, 0x58-stride subtraction and explicit 64-bit shift helper calls. Next: typed vector copies, shared branch tails and radius lifetimes; frame remains 0xa8 versus 0xb0. |

All four targets received OpenSaga review plus a `hillclimb.py try --donor-note`
record. All four were searched with `order_search.py --body ... --auto` from
compiling seeds. The three unresolved targets exceed the 25-candidate floor;
no short-handoff exception or permuter run was used.

For the loaders, GC's repeated allocator checks unsigned **unaligned cursor +
size < end**, aligns the result to 16 bytes, advances the cursor, and does not
zero memory itself. Preserve that strict bound and ordering. Donor allocation
zeroing is rejected. GC allocations observed here include system 0x138c,
path-system 12, path 0x88, node 0x58 and connection 0x28 bytes.

The original path-loader draft conflated a version comparison with a later count
comparison saved in another condition-register field. The ai2 loader similarly
needs separate version >6, >12 and >14 tests. Fixing those is semantic recovery,
not just register tuning. Some scratch drafts still need broader structure work;
none is installed merely because its score improved.

## Donor and JP PC limits

OpenSaga pin `7bb2402aa902d08f0979892ca23ee80e809e5ee1`,
`src/legoapi/ai/core/aisysall.cpp`, was reviewed through task-pack donor notes and
local bodies. No donor source was copied.

- `AISysProcessCharacter`: donor Initialise save/process/restore and distance/scale
  flow agree with GC. Reject modern alternate-processor and packet-flag layouts.
  JP `0x004DA810` is string-paired and supports Initialise plus 0/0.5 constants;
  only 14/29 calls align, so no PC offsets or argument counts transfer.
- `fn_80085A50`: no exact-name donor hit and no JP pair. The named donor path-loader
  and allocator offer algorithm leads only. Reject allocator memset and modern
  path/connection extensions; all widths and version tests come from GC assembly.
- `fn_80086A04`: donor load/storage and filename construction are useful leads.
  JP `0x004D2390` is an ai2 filename-string lead, with 95 calls versus GC's 114;
  alignment does not establish identity or layouts.
- `AISysCharacterTestPathCnx`: donor geometry/radius flow helps interpret branches.
  JP `0x004D8EF0` remains an inferred shape-callee pair (24/30 aligned calls).
  Its symmetric-table entry agrees with GC. Reject the modern donor checksum
  cache; GC reads and writes a symmetric byte bitset.

No gameplay-dependent name was introduced. The shape-handler naming question
remains owned by the separate task. No Dolphin session was driven, and no
runtime-only claim was used to accept the processor.

## Workflow repair

Automatic order search produced an ai2-loader winner that reset the allocation
result after the call, and a path-loader candidate that copied flags before their
store. Both were rejected. The repair adds call/update, direct scalar dependency
and repeated-lvalue barriers to automatic runs. Explicitly marked searches retain
their existing behavior, and memory aliasing still requires manual review.

Both #173 and #174 claimed workflow gap 27; this repair uses the next free number,
[28](../workflow_gaps.md#28-automatic-ordering-crossed-dependencies).
The [controlled replay](../workflow_metrics.md#automatic-order-dependency-barriers)
retains the same drafts and budgets: 650 → 650 compiled scores, compile errors
0 → 0, new strict matches 0 → 0. The native allocation fixture improves from
12/24 wrong-result variants to 0/2. Target search scores are
704/504/367 → 699/505/375; invalid earlier winners are not useful baselines for
matching gains. The best independent path-loader draft remains 705/921.
The processor matched before this repair; it is not attributed to the tool.

## Verification and report

- `bash build.sh`: zero differing DOL bytes; SHA-1
  `95cca08a19224775d1a8d6cc64601fb7d0080981`.
- `sprint_finish.py`: one new strict match; all queue entries accounted for.
- All 131 source-defined unit functions pass strict verification, with retail
  reference checks. Complete-unit promotion was not attempted.
- `matching_progress.py`: report functions **2,875 → 2,875**, matched code bytes
  **494,968 → 494,968**. Strict-only coverage **231 → 232 functions**,
  **89,736 → 91,348 bytes** (+1,612).
- 739 portable tests pass (two skipped); all 64 retail-required verifier tests
  pass. Compileall, configuration, symbol-map generation, shell syntax,
  local documentation links and `git diff --check` pass.

The finish record reports 1,608 active seconds through the matching/build gate;
subsequent documentation and PR preparation are outside that closed timer.
Raw task packs, binary decompiles, assembly, candidates and logs remain ignored
under `work/`. The retail DOL remains ignored by Git.
