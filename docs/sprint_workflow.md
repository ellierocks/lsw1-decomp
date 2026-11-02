# Sprint workflow

Phase 3 matching runs as **sprints**. A sprint is one thread, one unit and one PR, and it
finishes inside a single context window. The queue is sized by context, not by a fixed
count: fixed setup and finish costs (build, baseline, rebase, CI, merge) are paid once, so a
sprint should match as much as its context allows. It replaces the
multi-unit batches recorded in [Phase 3 batch history](phase3_batches.md). Those batches
ran out of context and stalled on small issues. Fan-out workers were rate-limited and
added nothing; one batch got 0 matches from 4 workers. Rebase drift, conflicts in shared
docs and long write-ups then delayed each merge.

[Strategy](strategy.md) decides which lane comes next. This page decides how to work
through it. [Quickstart](matching_quickstart.md) steps 3 to 5 are the per-function loop.

## Scope

- **One unit per sprint.** Pick it with `next_target.py --by-unit`, then list the unit's
  ready functions with `--unit UNIT --limit 40`. Work them in waves of about 10, related
  families first. Choose a related family of functions inside one unit.
  Do not spread across several units. Siblings with one shape (a `Curve` and `Curve2`
  pair, setters, loop-per-parameter actions) beat unrelated bodies: draft one, then reuse
  its structure. Aim for 0x100 to 0x300 bytes per function; tiny accessors add few bytes
  and large unique bodies tend to plateau.
- **Check active sprints first.** Run `git fetch` first. `--by-unit` then skips units owned by an open
  `t3/sprint-*` PR (asked through `gh`; `--no-remote` turns that off) or by any local or
  `origin` sprint branch not yet merged into `origin/main`. That covers a sprint that has
  been pushed but has no PR yet. Units with 10 or fewer functions left are listed first
  (marked `*`). Branch names must follow `t3/sprint-<unit-leaf>` and be pushed early for
  this to work. `work/` claim markers are local to each worktree, so other threads cannot
  see them.
- **Headers come with their consumers.** Add a shared-header field only when a function
  in the queue reads or writes it, and confirm it in the same sprint. Keep everything else
  as `char unk[N]` or a local view. A sprint that only changes headers matches nothing.
- **No workers.** Do not start subagents or workers in a sprint. To run work in parallel,
  the maintainer launches separate threads on separate units.
- **A finishable unit beats a large one.** If a unit has 10 or fewer functions left,
  aim for the whole unit, because then it can be promoted.

## Kickoff (read only this much)

1. AGENTS.md and this page.
2. Run `python3 tools/next_target.py --unit UNIT --limit 40` and save the list as the
   queue in the first message of the thread.
3. Run `bash build.sh` once if `build/` is stale, then `python3 tools/sprint_finish.py --baseline`
   to record the objdiff figures, the strict-only count and the DOL hash. It refuses to run
   when the checkout is behind `origin/main`; rebase first so the docs and tools are current.

Ghidra is optional. For a function with loops, switches or nested conditions, run
`python3 tools/ghidra_decompile.py decompile FN ...` for that wave and read `work/FN/ghidra.c`
next to the m2c draft. Skip it for straight-line leaves and paired-single code. If `status`
says the project is missing, run `setup` (about a minute); see
[Ghidra and REA](research.md#ghidra-and-rea). Read `work/NAME/context.md` only when a draft
fails to compile or a type is unclear.

Do not read the batch history or the whole C matching guide up front. When a mismatch
looks familiar, search the guide's notes (`grep -n` on the instruction or the pattern)
and the "Failure modes" bullets in [batch history](phase3_batches.md).

## Per-function budget

| Step | Limit |
|---|---|
| Draft: `tools/draft_fn.py NAME` for an m2c start (with `work/NAME/ghidra.c` as a reference), then fast `score_fn.py` attempts | 8 attempts without improvement |
| Each attempt: `score_fn.py NAME src/UNIT.c --body try.c --diff --apply` | scores `try.c` in place of NAME's definition, prints the TARGET/MINE listing on a mismatch, and writes it into the unit only on a strict MATCH |
| `variant_search.py NAME src/UNIT.c -j 8` once the draft is within about 10 instructions | one run |
| `order_search.py`, if only the order differs | one run |
| Permuter, only at about 90% exact or better | one run of at most 180 s (`timeout 180`) |
| Then | park and move on; do not come back in this sprint |

The first `hillclimb.py try` records `--donor-note`. When the pinned OpenSaga checkout has
no name hit, `hillclimb.py` records the no-hit limit itself; pass a note only when you
searched or read donor code. A parked function adds one row to the PR's Parked table and
a `work/NAME/status.md` note giving the best score, the remaining mismatch and the next
hypothesis.

**After each wave**, commit the matches, then take the next wave from the same unit's list.
**Stop the sprint** and finish when any of these holds:

- the unit has no ready functions left;
- about 60% of the context window is used (finish, CI fixes and merge need the rest);
- three functions in a row are parked (the rest of the unit is likely harder; leave it).

Do not start a second unit in the same thread; open the next sprint as a new thread.

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
that figure, not an adjusted objdiff total. In a unit with a `.rodata` range
([provisional ranges](build_config.md#provisional-rodata-ranges)), such a function counts once
the unit's literals sit at retail offsets. An `extern lbl_` reference already matches by name.
To promote the unit, its C must emit every literal in the range, strings included, in retail order.

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
Sprint: UNIT. Queue: next_target.py --unit UNIT --limit 40 (or: fn_A, fn_B, ...).
Follow docs/sprint_workflow.md. Branch t3/sprint-<unit-leaf>. Merge when CI passes.
```
