# Matching quickstart

The shortest path from a prompt to one recorded C-matching attempt. Reference
details live in [C matching](code_matching_workflow.md) and the plan in
[strategy](strategy.md).

## 1. Bootstrap once per checkout

```sh
bash tools/bootstrap.sh
```

This fetches wibo, ProDG, binutils and objdiff, and builds the patched DTK. It
does not split the DOL. Worktrees share one toolchain: `build/tools`,
`build/compilers`, `build/binutils` and the retail DOL are linked from
`~/.cache/lsw1` (or the primary checkout), so a new worktree is ready in about a
second. The pinned dolsdk2004 and OpenSaga donors are cloned into the same cache
(`LSW1_CACHE` sets the shared cache; the existing toolchain/SDK overrides remain
unchanged). Verification also needs `orig/GL5E4F/sys/main.dol`; see the [README](../README.md#prerequisites).
Run `bash build.sh` when you change splits, configuration or compiler wiring.

OpenSaga uses the pin in `config/donors/opensaga.json`; `OPENSAGA` overrides its
checkout and `LSW1_NO_OPENSAGA=1` skips the fetch. See the
[donor workflow](symbol_donors/opensaga_matching.md#end-to-end-workflow) for offline
setup and checkout validation.

If a tool says the toolchain is missing, run the bootstrap again. For an SDK unit
rather than a game function, skip to `tools/sdk_import_unit.py` in
[SDK import](sdk_import.md#importing-a-unit-quickly).

## 2. Pick a target

Follow the current phase in [strategy](strategy.md): SDK import first, then
whole-binary splits, then agent draft-and-climb for game code. Claim one library,
TU or function per worktree branch. Phase 3 game code runs as one-unit
[sprints](sprint_workflow.md): `python3 tools/next_target.py --by-unit` ranks units by
ready targets. Naming and donor mining serve that target.
For SDK units, `python3 tools/sdk_probe.py --pending` ranks the queue and
`python3 tools/sdk_import_unit.py UNIT --apply` imports and proves a unit.

```sh
python3 tools/next_target.py
python3 tools/next_target.py --min-size 0x80 --max-size 0x400 --unit auto/game/ --limit 50
```

It lists undone functions inside split units, sorted by direct callee count and
then size, with their unit, source and OpenSaga lookup state. Donor name leads
break otherwise equal ranking ties. Choose a `ready` row that is not `claimed`. An empty list does not mean the work is finished:
most game code lies outside split units (Phase 2 in [strategy](strategy.md)).
The default shows only the 15 smallest rows, so a lane of mid-size targets needs
`--min-size`, `--unit` (repeatable prefix) and a larger `--limit`. A `(new file)`
source means the unit has no C yet: create the `source_path` it names
(`src/<unit>.c`) and draft into it. If `configure.py` has no entry for the unit
yet (for example `auto/game/pickups`), add one beside the other `auto/` entries:
`Object(NonMatching, "auto/game/pickups.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg)`.
A unit missing from `src/` is work to start, not a blocker.

For game/Nu2 candidates, read the [OpenSaga lane](symbol_donors/opensaga_matching.md)
and its [remaining queue](symbol_donors/opensaga_audit.md#remaining-matching-queue).
Name leads need current identity, unit and ownership checks.

## 3. Read the target

```sh
python3 tools/ls1_task_pack.py NAME --out-dir work
```

This writes `work/NAME/asm.s`, `context.md` and `donor_evidence.md` (ignored by git).
Read the donor evidence, inspect listed source and relevant callees/keywords
against GC, and record what transfers or the no-hit/unavailable limit. Check
`symbol_donors/rejected_renames.tsv` before using a name.

For a first draft, `python3 tools/draft_fn.py NAME` writes `work/NAME/draft_m2c.c` from the
pinned [m2c](https://github.com/matt-kempster/m2c) (`tools/fetch_m2c.sh`, run by the
bootstrap). It does not compile until the arguments have real struct types; use it for the
control flow and the offsets.

It also writes `work/NAME/ghidra.c`, which shows named small-data globals, loops and
switches that m2c leaves raw. Read both. The bootstrap creates the Ghidra project; without
it, run `tools/ghidra_decompile.py setup` once. For "who else reads or writes this global", run
`tools/ghidra_decompile.py xrefs NAME`; REA covers callers, strings and tables interactively
([Ghidra and REA](research.md#ghidra-and-rea)).

## 4. Score candidates (fast loop)

```sh
python3 tools/score_fn.py NAME src/UNIT.c
```

It takes well under a second and prints the verdict, score, exact instructions
and the first difference. Change one source choice per attempt, guided by that
first difference. `--diff` adds the disassembled TARGET/MINE listing (mismatched rows
marked `!!`) instead of hex words. `--body try.c` scores `try.c`, one full definition
of NAME plus any new externs, in place of NAME in the unit source without editing it.
`--apply` writes it in only on a strict MATCH:

```sh
python3 tools/score_fn.py NAME src/UNIT.c --body /tmp/try.c --diff --apply
```

When the draft is close, try the known cheap rewrites in one run (declaration order of
locals, ternary against if/else, `s32`/`u32` for each integer, at most two changes at once):

```sh
python3 tools/variant_search.py NAME src/UNIT.c -j 8
```

If only the order of independent stores or loads differs, mark those statements
with `/* order: begin */` and `/* order: end */` in a scratch copy and search every
ordering ([order search](code_matching_workflow.md#store-and-load-order-search)):

```sh
python3 tools/order_search.py NAME work/NAME/order.c --const-locals -j 8
```

For any other close plateau, run decomp-permuter scored by the strict verifier
([details](code_matching_workflow.md#decomp-permuter)):

```sh
python3 tools/permute_setup.py NAME --run -- -j 8 --stop-on-zero --best-only
```

## 5. Record and accept

```sh
python3 tools/hillclimb.py try NAME --unit UNIT --source src/UNIT.c \
  --hypothesis 'one falsifiable change' \
  --donor-note '<inspected paths; GC-confirmed facts; differences or lookup limit>'
python3 tools/hillclimb.py status NAME --unit UNIT
python3 tools/verify_fn.py NAME --unit UNIT --build --strict-symbols
```

The first game/Nu2 attempt requires `--donor-note`; later attempts inherit it
for the same donor revision. Keep the review in the PR or plateau handoff. SDK
units use the Dolphin lane and are exempt.

Record a candidate with `try` when it improves the best score or settles a
hypothesis. Park the target after 8 attempts without improvement: write the best
score, the remaining mismatch and one next hypothesis in `work/NAME/status.md`,
then pick another target from step 2.

## 6. Rules that do not bend

- Never weaken the verifier, change pins or move a split to raise a score.
- A strict match is VERIFIED, not PROMOTED. Promotion needs the checks in
  [C matching](code_matching_workflow.md#promotion).
- Donor C is evidence. A function counts only after it matches with this
  game's compiler.
- Commit explicit paths on a topic branch and open a PR into `main`.
