# Matching quickstart

The shortest path from a prompt to one recorded C-matching attempt. Reference
details live in [C matching](code_matching_workflow.md) and the plan in
[strategy](strategy.md).

## 1. Bootstrap once per checkout

```sh
bash tools/bootstrap.sh
```

This fetches wibo, ProDG, binutils and objdiff, and builds the patched DTK. It
takes about 10 seconds on a warm cache and does not split the DOL. Verification
also needs `orig/GL5E4F/sys/main.dol`; see the [README](../README.md#prerequisites).
Run `bash build.sh` when you change splits, configuration or compiler wiring.

If a tool says the toolchain is missing, run the bootstrap again.

## 2. Pick a target

```sh
python3 tools/next_target.py
```

It lists undone functions inside split units, sorted by direct callee count and
then size, with their unit and whether a source file exists. Choose a `ready`
row that is not `claimed`. An empty list does not mean the work is finished:
most game code lies outside split units (Phase 2 in [strategy](strategy.md)).

## 3. Read the target

```sh
python3 tools/ls1_task_pack.py NAME --out-dir work
```

This writes `work/NAME/asm.s` and `context.md` (ignored by git). Check
`symbol_donors/rejected_renames.tsv` before using a name.

## 4. Score candidates (fast loop)

```sh
python3 tools/score_fn.py NAME src/UNIT.c
```

It takes well under a second and prints the verdict, score, exact instructions
and the first difference. Change one source choice per attempt, guided by that
first difference.

## 5. Record and accept

```sh
python3 tools/hillclimb.py try NAME --unit UNIT --source src/UNIT.c \
  --hypothesis 'one falsifiable change'
python3 tools/hillclimb.py status NAME --unit UNIT
python3 tools/verify_fn.py NAME --unit UNIT --build --strict-symbols
```

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
