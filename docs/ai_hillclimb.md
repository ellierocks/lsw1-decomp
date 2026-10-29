# AI matching hillclimb

The objective is to replace fallback assembly with C that reproduces retail
GL5E4F. Each session should leave either a verified improvement or a documented
reason to choose another target. Symbol recovery supports this objective; names
and mnemonic similarity do not count as matched code.

## Select and prepare

1. Read current symbol evidence and rejected renames. Prefer a small function with
   a known prototype, useful types, few callees and a comparison unit already
   configured. Use `ls1_task_pack.py` and donor queues as discovery aids, not proof.
2. Check `tools/matching_progress.py`, then run `bash build.sh` once to establish
   a working baseline. Keep the ProDG version and flags pinned. Do not start
   source tuning when the compiler, target split or retail input is broken.
3. Generate a task under ignored `work/`. Generation refuses to overwrite a
   populated task. Configure the source as a **NonMatching** ProDG comparison
   object; `work/Foo/source.c` is not compiled automatically. Prefer editing an
   existing comparison unit. Keep fallback assembly linked until promotion.
4. Write the signature and a plausible C body from the retail assembly and
   verified types. Treat donor implementations as evidence about algorithms;
   their MWCC code generation is not this game's ProDG code generation.

## Measure and iterate

From the repository root (replace the example function, unit and source):

```sh
python3 configure.py
python3 tools/hillclimb.py try NuVecScale --unit numath/nuvec \
  --source src/numath/nuvec.c --source src/numath/numath.h \
  --hypothesis 'baseline with existing vector types'
python3 tools/hillclimb.py status NuVecScale --unit numath/nuvec
python3 tools/verify_fn.py NuVecScale --unit numath/nuvec --build --strict-symbols --full
```

The agent proposes changes; the local compiler and verifier judge them. This
workflow uses the active agent session and does not invoke a model API or purchase
compute. One owner writes each candidate; independent targets may use separate
worktrees when explicitly delegated under the session's agent policy.

For each attempt:

1. Inspect the first meaningful divergence and state one falsifiable hypothesis:
   signedness, expression order, pointer stride, loop shape, lifetime, or float
   precision. Change one related source choice and compile again.
2. Run `hillclimb.py try` with the hypothesis and **every edited source/header**
   supplied through repeated `--source`. It runs the strict verifier with
   `--build`; stale objects and compiler errors cannot become improvements.
3. Inspect the report, not just the score. Lower lexicographic score wins:
   fully verified match first, fewer differences, smaller instruction-count
   difference, more exact instructions, fewer unverified references. This is a
   search heuristic; aligned instruction counts can be discontinuous and a
   score improvement does not prove semantics or permit promotion.
4. Continue from the best candidate. Revert a losing experiment by deliberately
   editing your own source back to its recorded snapshot. The tool never resets
   the checkout. A neutral or worse experiment can inform a later hypothesis,
   but it does not replace the incumbent best.

Records are immutable numbered JSON files under `work/hillclimb/<identity>/`.
They include hypotheses, source text and hashes, Git head, build configuration /
verifier hashes, command, errors and the complete report. Identity includes the
function, unit and optional compiled symbol: repeat `--compiled-symbol` for status.
Only one writer can record a target at a time. An interrupted writer may leave
`.lock`; confirm no writer remains before removing that lock. Logs stay local;
retain a concise evidence summary and final C in the PR, not raw game artifacts.

Defaults: 40 attempts, 8 consecutive attempts without improvement, 120 seconds
per verifier invocation. Errors and timeouts consume attempts. `PARK` means stop
and save the best attempt, remaining mismatch, rejected ideas and one concrete
next hypothesis in the task's `status.md`; select a different target. Do not
restart a ledger to hide exhausted work. A deliberately changed search budget or
compiler/type context must be explained in the handoff; use a new `--ledger` for
a new context so incompatible scores are never compared. Changes to the stored configuration, symbol/split or verifier hashes
block new attempts in that ledger. Source/header context changes still require
the agent to decide whether prior scores are comparable.
An attempt budget is not an agent token budget: follow account usage limits too.

## Verify and promote

`VERIFIED` means the strict verifier returned zero, including verified references.
Exit 1 is a mismatch, 2 a tooling/build error, 3 unresolved reference identity.
Do not rename a symbol just to silence a reference mismatch. Resolve it with
address, type and data evidence. Fix tooling failures before source tuning.

Before promotion, put the best source back, rebuild, and verify **every function
in the affected unit**. Plan the fallback split with `asm_function_slice.py`;
review generated slices and wire source/splits deliberately. Follow
[code matching workflow](code_matching_workflow.md) and
[nurand matching](nurand_matching.md): section bytes, relocations, data layout
and linked retail ranges must match. Run the full build and compare the new
report with baseline; previously matched or linked ranges must not regress.
A whole-DOL match remains the final goal; existing unrelated DOL differences do
not excuse a new mismatch in a promoted range.

Finish with a reviewable PR containing target addresses, baseline/best attempt,
exact verification commands, promoted bytes/functions, report delta, checks and
remaining limits. A useful unpromoted C improvement can be retained as
NonMatching with its evidence; report zero newly promoted bytes honestly. Stop
before quota exhaustion and leave the best source plus exact next action for the
next session. Do not spend a session refreshing all naming queues unless missing
evidence blocks the selected C target.
