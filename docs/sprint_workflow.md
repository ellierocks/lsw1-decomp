# Sprint workflow

Phase 3 matching runs as **sprints**. A sprint is one thread, one unit and one PR, and it
continues across context windows when needed to complete the requested scope. The queue is sized by context, not by a fixed
count: fixed setup and finish costs (build, baseline, rebase, CI, merge) are paid once, so a
sprint should match as much as its context allows. It replaces the
multi-unit batches recorded in [Phase 3 batch history](phase3_batches.md). Those batches
ran out of context and stalled on small issues. Fan-out workers were rate-limited and
added nothing; one batch got 0 matches from 4 workers. Rebase drift, conflicts in shared
docs and long write-ups then delayed each merge.

[Strategy](strategy.md) decides which lane comes next. This page decides how to work
through it. [Quickstart](matching_quickstart.md) steps 3 to 5 are the per-function loop.

## Scope

- **Sweep first.** Run `python3 tools/sweep.py --apply` before choosing the unit; it takes
  the automatic m2c matches across the whole game for free. Then pick the unit with
  the most near-miss bytes in `work/sweep/results.json` (size × exact ratio, 90% exact or
  better first). Each queued function's starting draft is `work/sweep/NAME/sweep_draft.c`.
- **One unit per sprint.** Pick it from the sweep ranking or `next_target.py --by-unit`, then list the unit's
  ready functions with `--unit UNIT --limit 40`. Work them in waves of about 10, related
  families first. Choose a related family of functions inside one unit.
  Do not spread across several units. Siblings with one shape (a `Curve` and `Curve2`
  pair, setters, loop-per-parameter actions) beat unrelated bodies: draft one, then reuse
  its structure. Aim for 0x100 to 0x300 bytes per function; tiny accessors add few bytes
  and large unique bodies tend to plateau.
- **Check active sprints first.** Run `git fetch` first. `--by-unit` then skips units owned by an open
  `t3/sprint-*` PR (asked through `gh`; `--no-remote` turns that off) or by any local or
  `origin` sprint branch not yet merged into `origin/main`. That covers a sprint that has
  been pushed but has no PR yet. Units are ranked by bytes in their first ten ready targets;
  `*` marks ten or fewer functions left, not promotion readiness. Branch names must follow `t3/sprint-<unit-leaf>` and be pushed early for
  this to work. `work/` claim markers are local to each worktree, so other threads cannot
  see them.
- **Headers come with their consumers.** Add a shared-header field only when a function
  in the queue reads or writes it, and confirm it in the same sprint. Keep everything else
  as `char unk[N]` or a local view. A sprint that only changes headers matches nothing.
- **No workers.** Do not start subagents or workers in a sprint. To run work in parallel,
  the maintainer launches separate threads on separate units.
- **Amortize setup over useful code.** Prefer a unit with several waves of related,
  tractable functions over a tiny queue. Inspect the first wave's assembly for
  reusable shapes before committing to it; bytes alone do not measure difficulty.
  Do not choose a three-function unit just because it is almost complete. A small
  unit is preferable only with a concrete advantage, such as a proven promotion
  path or a shared type that unlocks a larger family. Check all owned sections
  before claiming that path; matching every function does not emit missing data.

## Kickoff (read only this much)

1. AGENTS.md and this page.
2. Take the unit's near-misses from `work/sweep/results.json`, largest first, then fill
   with `python3 tools/next_target.py --unit UNIT --limit 40`. Save the list as the
   queue in the first message of the thread.
3. Run `bash build.sh` once if `build/` is stale, then `python3 tools/sprint_finish.py --baseline --queue FN1 FN2 ...`
   to record the objdiff figures, the strict-only count and the DOL hash. It refuses to run
   when the checkout is behind `origin/main`; rebase first so the docs and tools are current.

Reuse the checkout's toolchain and build. Bootstrap only if prerequisites are missing;
for matching setup use `LSW1_NO_GHIDRA=1 bash tools/bootstrap.sh` and request Ghidra
separately when needed. Do not start decompilation while setup still owns its project.
Generate m2c drafts for the whole wave in one call with
`python3 tools/draft_fn.py FN1 FN2 ... --no-ghidra --print`, which prints each draft and its
assembly without the byte column. Then request Ghidra for the wave in one call. This avoids
launching a headless process separately for every draft. Drafts already declare globals
that the target reaches through `@ha/@l` as arrays read with `sym[0]`. ProDG puts scalar
externs in small data, so a scalar declaration can never match those loads.

Ghidra is optional. For a function with loops, switches or nested conditions, run
`python3 tools/ghidra_decompile.py decompile FN ...` for that wave and read `work/FN/ghidra.c`
next to the m2c draft. Skip it for straight-line leaves and paired-single code. If `status`
says the project is missing, run `setup` (about a minute); see
[Ghidra and REA](research.md#ghidra-and-rea). Read `work/NAME/context.md` only when a draft
fails to compile or a type is unclear.

Do not read the batch history or the whole C matching guide up front. When a mismatch
looks familiar, search the guide's notes (`grep -n` on the instruction or the pattern)
and the "Failure modes" bullets in [batch history](phase3_batches.md).

## Measured runs

Active time is **measured, not budgeted**. Start the timer with
`python3 tools/sprint_finish.py --phase setup`, switch phases as work changes, and use
`--phase paused` during user and CI waits. Every `score_fn.py` result line ends with
`[active Ns]` while the timer runs. The finish record reports setup, matching, tooling
and finish seconds beside the verified byte gain, so throughput can be compared across
runs. Time never decides whether to keep working on a function; the per-function limits
below do.

This replaces the 600-second budget used in
[#53](https://github.com/ellierocks/lsw1-decomp/pull/53) to
[#55](https://github.com/ellierocks/lsw1-decomp/pull/55). In #55 the countdown drove
early parking (a 70/71 and a 72/80 near-miss were dropped to follow an attempt cap),
an early switch to finish, and padding work to reach a deferral gate. That run verified
280 bytes, against 656 in #53.

The run ends when the queue is exhausted, the maintainer stops it, or context pressure
forces a continuation checkpoint. List queue entries that were never attempted or left
unfinished with `--deferred FN ...`; they stay distinct from measured `--parked`
plateaus. This is bracketed wall time, not CPU time or inferred human attention.
Forgotten pauses invalidate efficiency claims, and an interrupted run resumes its existing
timer. Missing historical measurements remain unknown. `sprint_finish.py` gives every
`git fetch` a 60-second timeout. If `--baseline` cannot fetch, it warns and checks
freshness against the last fetched `origin/main`.

### Provider usage and cost

Import provider-reported **per-run cumulative totals** with
`python3 tools/sprint_finish.py --usage work/provider-usage.json`:

```json
{"run_id":"provider-run-id","model":"exact-provider-model","source":"provider export reference",
 "input_tokens":1000,"cached_input_tokens":800,"output_tokens":200,"cost_usd":null}
```

Cached input is a subset of input; never add it twice. Reimporting the same run replaces
its cumulative counters, and decreasing counters or changing identity is rejected.
Use a distinct provider run ID for each run. Import only this sprint's usage, and retain
provenance. Do not import thread-lifetime totals as per-run measurements. `cost_usd`
is the provider-reported dollar charge, or null when unavailable—not a guessed price,
subscription allocation or API-equivalent estimate. Reasoning tokens already included
in provider output totals must not be added again. The tool never scans conversations,
reads credentials or checks quotas. Without provider exports, usage and cost are unknown;
partial imports are labeled as covering imported runs only.

Report verified byte gain beside measured active time and usage coverage. Do not project
whole-project completion from one easy-function run. For an existing baseline without
a queue, supply the full `--queue` at finish.

## Per-function budget

Spend effort where the bytes are closest. Each attempt's score decides what comes next:

- Write new functions only through `score_fn.py --body try.c --apply`. A function that is
  not yet in SOURCE is appended for scoring, and SOURCE changes only on a strict MATCH,
  so parking needs no cleanup.
- **Near-misses are the priority.** At 95% exact or better, `score_fn.py` prints
  `NEAR MISS`. Do not park it on an attempt count. Read the whole diff, then try one
  targeted change per mismatch: expression order or operand order for a commutative
  instruction, how a pointer is formed (`&a[i][j]`, `a[i] + j`, an explicit field),
  declaration or initialisation placement, and inline helper boundaries. Then run
  `variant_search.py` once and `order_search.py` once if only the order differs. In
  #55, hoisting one `i = 0` moved `fn_8013D9FC` from 51/71 to 70/71.
- Below 95%, park after **3 attempts without improvement**. Below about 60% after the
  first attempt, check the structure against `ghidra.c` before trying again.
- **Type the family's struct before drafting.** When several queued functions share one
  object (a pool, a save block), run `ghidra_decompile.py xrefs` and add the fields those
  functions access to the header at the start. Casts such as `*(s32*)((char*)p + 0x74c)`
  slow every attempt and hide aliasing that changes the schedule.
- **Order the queue by shared shape**, not by size. After a match, try its shape
  siblings next. In #53, `fn_8013F24C` matched on its first attempt by reusing the shape
  of an earlier sibling.

The table below caps the search for one function.

| Step | Limit |
|---|---|
| Draft: `tools/draft_fn.py NAME` for an m2c start (with `work/NAME/ghidra.c` as a reference), then fast `score_fn.py` attempts | 8 attempts without improvement (3 below 95%) |
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
**Finish only after accounting for the complete selected queue, including timed deferrals.** `sprint_finish.py`
rejects missing outcomes. A parked row needs a measured score, the remaining mismatch
and a next hypothesis; it means an actual bounded search, not an unattempted function.
Context pressure is a continuation checkpoint: save the current candidate, remaining
queue and next command under `work/`, then resume the same unit and PR. Do not turn
it into a smaller final PR. Three consecutive bounded plateaus justify reassessing
the remaining queue; they do not prove unattempted functions are exhausted.

A completed wave is not automatically the end of the run. Refresh the unit queue and
continue related tractable work until the unit is exhausted. User-directed
stops take precedence; report partial results honestly rather than claiming completion.

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
section in `strategy.md` and no tracked timing file.

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
