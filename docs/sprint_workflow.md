# Sprint workflow

Phase 3 matching runs as **sprints**. A sprint is one thread, one unit, a queue of up to
10 functions and one PR, and it finishes inside a single context window. It replaces the
multi-unit batches recorded in [Phase 3 batch history](phase3_batches.md). Those batches
ran out of context and stalled on small issues. Fan-out workers were rate-limited and
added nothing; one batch got 0 matches from 4 workers. Rebase drift, conflicts in shared
docs and long write-ups then delayed each merge.

[Strategy](strategy.md) decides which lane comes next. This page decides how to work
through it. [Quickstart](matching_quickstart.md) steps 3 to 5 are the per-function loop.

## Scope

- **One unit per sprint.** Pick it with `next_target.py --by-unit`, then take the queue
  with `--unit UNIT --limit 10`. Choose a related family of functions inside one unit.
  Do not spread across several units. Siblings with one shape (a `Curve` and `Curve2`
  pair, setters, loop-per-parameter actions) beat unrelated bodies: draft one, then reuse
  its structure. Aim for 0x100 to 0x300 bytes per function; tiny accessors add few bytes
  and large unique bodies tend to plateau.
- **Check open PRs first.** `--by-unit` asks `gh` for open `t3/sprint-*` PRs and skips the
  units they own (`--no-remote` turns that off), and it lists units with 10 or fewer
  functions left first (marked `*`). Branch names must follow `t3/sprint-<unit-leaf>` for
  that to work. `work/` claim markers are local to each worktree, so other threads
  cannot see them.
- **Headers come with their consumers.** Add a shared-header field only when a function
  in the queue reads or writes it, and confirm it in the same sprint. Keep everything else
  as `char unk[N]` or a local view. A sprint that only changes headers matches nothing.
- **No workers.** Do not start subagents or workers in a sprint. To run work in parallel,
  the maintainer launches separate threads on separate units.
- **A finishable unit beats a large one.** If a unit has 10 or fewer functions left,
  aim for the whole unit, because then it can be promoted.

## Kickoff (read only this much)

1. AGENTS.md and this page.
2. Run `python3 tools/next_target.py --unit UNIT --limit 10` and save the list as the
   queue in the first message of the thread.
3. Run `bash build.sh` once if `build/` is stale, then `python3 tools/sprint_finish.py --baseline`
   to record the objdiff figures, the strict-only count and the DOL hash.
4. Run `python3 tools/ghidra_decompile.py decompile FN1 ... FN10` to write `work/FN/ghidra.c`
   for the queue in one run, and read it next to each m2c draft. The bootstrap creates the
   project; if `status` says it is missing, run `setup` first (about a minute). See
   [Ghidra and REA](research.md#ghidra-and-rea).

Do not read the batch history or the whole C matching guide up front. When a mismatch
looks familiar, search the guide's notes (`grep -n` on the instruction or the pattern)
and the "Failure modes" bullets in [batch history](phase3_batches.md).

## Per-function budget

| Step | Limit |
|---|---|
| Draft: `tools/draft_fn.py NAME` for an m2c start (with `work/NAME/ghidra.c` as a reference), then fast `score_fn.py` attempts | 8 attempts without improvement |
| `variant_search.py NAME src/UNIT.c -j 8` once the draft is within about 10 instructions | one run |
| `order_search.py`, if only the order differs | one run |
| Permuter, only at about 90% exact or better | one run of at most 180 s (`timeout 180`) |
| Then | park and move on; do not come back in this sprint |

The first `hillclimb.py try` records `--donor-note`. A target with no donor hit records
the no-hit limit in one line. A parked function adds one row to the PR's Parked table and
a `work/NAME/status.md` note giving the best score, the remaining mismatch and the next
hypothesis.

**Stop the sprint** when the queue is empty, or when 10 functions are matched or parked.
After that, finish this sprint. Do not start a second unit in the same thread; open the
next sprint as a new thread.

## Finish (one pass)

```sh
python3 tools/sprint_finish.py FN1 FN2 ... --unit UNIT --rebase \
  --parked "FN3=41/51: <mismatch>; next: <hypothesis>"
```

Commit first. The script rebases, runs `bash build.sh` once, strict-verifies each function
against that build, runs `git diff --check origin/main`, compares the DOL hash and the
report with the kickoff baseline, and prints the record below with only Donor and Headers
left to fill in. It exits non-zero on any failure.

A function can pass the strict verifier yet sit below 100% in objdiff, because a literal reloc
targets `.rodata+N` rather than the retail `lbl_` symbol. Those do not move `matched_functions`.
`matching_progress.py --strict-gap` (and `tools/strict_gap.py`) counts them separately; report
that figure, not an adjusted objdiff total.

Rebase once, at the end. If main moved, the report delta includes main's changes; say so
in the PR rather than building a separate baseline worktree. Run `verify_unit.py UNIT
--build` only when every function in the unit matches, then promote if it passes.

## Records

The PR body is the sprint record. Keep it to the template below. There is no per-sprint
section in `strategy.md` and no timing file.

```markdown
Sprint: `UNIT` (N matched, M parked)

| Function | Result |
|---|---|
| fn_X | VERIFIED |
| fn_Y | parked 41/51: <mismatch>; next: <hypothesis> |

- Promoted: none | UNIT (bytes), verify_unit and linked range clean
- Report: functions A → B, code bytes C → D (includes main drift: yes/no); strict-only E → F
- DOL: byte-identical (`bash build.sh`); `matching_progress.py` passes
- Donor: <one line, or "no OpenSaga hits">
- Headers: <fields added with their consumer, or none>
```

Only durable facts go into docs, in the same PR:

- a confirmed struct field goes into the header, with a short comment naming its consumer;
- a new layout fact goes into the table in [Nu2](nu2_engine_reference.md) or
  [game structs](game_structs.md);
- a new compiler quirk becomes one bullet in [C matching](code_matching_workflow.md).

Update `strategy.md` only when the plan changes, for example when a lane is finished or
the lane order changes.

## Git hook

The Git guard inspects the directory a command starts in. Run `git -C <worktree> commit ...`
or start the command inside the worktree; `cd X && git commit` from the main checkout is
rejected as a commit on `main`.

## Merge and clean up

Push, open the PR, link it, then watch it and end the turn. When required CI passes on the
current head, merge with `gh pr merge --merge`, stop watching, and delete the branch. If CI
fails, fix it in the same thread. If the fix needs a new idea rather than a correction,
park the function and drop it from the PR.

## Kickoff prompt

```text
Sprint: UNIT. Queue: next_target.py --unit UNIT --limit 10 (or: fn_A, fn_B, ...).
Follow docs/sprint_workflow.md. Branch t3/sprint-<unit-leaf>. Merge when CI passes.
```
