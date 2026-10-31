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

```sh
python3 tools/next_target.py
```

It lists undone functions inside split units, sorted by direct callee count and
then size, with their unit, source and OpenSaga lookup state. Donor name leads
break otherwise equal ranking ties. Choose a `ready` row that is not `claimed`. An empty list does not mean the work is finished:
most game code lies outside split units (Phase 2 in [strategy](strategy.md)).

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

## 4. Score candidates (fast loop)

```sh
python3 tools/score_fn.py NAME src/UNIT.c
```

It takes well under a second and prints the verdict, score, exact instructions
and the first difference. Change one source choice per attempt, guided by that
first difference.

If only the order of independent stores or loads differs, mark those statements
with `/* order: begin */` and `/* order: end */` in a scratch copy and search every
ordering ([order search](code_matching_workflow.md#store-and-load-order-search)):

```sh
python3 tools/order_search.py NAME work/NAME/order.c --const-locals -j 8
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
