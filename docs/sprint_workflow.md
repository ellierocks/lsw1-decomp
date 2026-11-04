# Sprint workflow

Phase 3 matching runs as **sprints**: one unit, one branch, one PR. Setup and finish costs
(build, baseline, merge, CI) are paid once, so a sprint should cover as much of its unit as
is tractable. Earlier multi-unit batches are recorded in
[Phase 3 batch history](phase3_batches.md).

[Strategy](strategy.md) decides which lane comes next. This page covers the tools and the
evidence a sprint must produce. [Quickstart](matching_quickstart.md) steps 3 to 5 are the
per-function loop.

## Scope

- **Build before sweeping a fresh checkout.** Run `bash build.sh` to completion if
  the checkout has no generated target assembly. Bootstrap supplies tools and retail
  input, but does not split the DOL. Do not overlap this initial build with scoring:
  rebuilding the shared `wibo` executable can make compiler launches fail with
  `Text file busy`. A sweep with `NO-ASM` rows is incomplete; build, then rerun
  the affected units before using its ranking.
- **Sweep first.** Run `python3 tools/sweep.py --apply` before choosing the unit; it takes
  the automatic m2c matches across the whole game. Then run
  `python3 tools/sweep_permute.py --apply`, which runs the permuter unattended on the
  biggest near-misses. Rank units by near-miss bytes in `work/sweep/results.json`
  (size × exact ratio, 90% exact or better first). Each queued function's starting draft
  is `work/sweep/NAME/sweep_draft.c`. Automatic sweep and permuter base scoring pass
  `score_fn.py --no-queue`, so they do not add outside targets to an existing sprint
  queue. Interactive scoring still adds targets; use `--no-queue` explicitly for
  exploratory scans.
- **One unit per sprint.** Pick it from the sweep ranking or `next_target.py --by-unit`,
  then list its ready functions with `--unit UNIT --limit 40`. Siblings with one shape (a
  `Curve` and `Curve2` pair, setters, loop-per-parameter actions) reuse one draft's
  structure.
- **Ownership is by branch name.** Run `python3 tools/git_origin.py fetch origin` first.
  `--by-unit` then skips units owned by an open `t3/sprint-*` PR (asked through `gh`;
  `--no-remote` turns that off) or by any local or `origin` sprint branch not yet merged
  into `origin/main`, which covers a pushed sprint with no PR yet. Name the branch
  `t3/sprint-<unit-leaf>` and push it early. `work/` claim markers are local to each
  worktree and invisible elsewhere. Units are ranked by bytes in their first ten ready
  targets; `*` marks ten or fewer functions left, not promotion readiness.
- **Headers come with their consumers.** Add a shared-header field only when a function
  in the queue reads or writes it, and confirm it in the same sprint. Keep everything else
  as `char unk[N]` or a local view.
- **Promotion needs data too.** Check all owned sections before counting on a promotion
  path; matching every function does not emit missing data.

## Improve the workflow as you go

Fixing a tool gap mid-run pays off in every unit that has the same gap. In
[#87](https://github.com/ellierocks/lsw1-decomp/pull/87), four mid-run `fix(tools)` commits
(sweep compile repairs, `__ashldi3` shifts, the float `cror` fold, initialized locals in
`variant_search.py`) brought in matches outside the unit and let searches find fixes that
had been made by hand.

- **Fix repeated gaps.** When the same manual workaround, compile error or search gap
  recurs, fix the tool in this branch with a regression test in `tests/`, committed as
  `fix(tools): ...` or `feat(tools): ...`.
- **Then collect the wins.** Rerun the fixed tool where it can now succeed: for a sweep fix,
  `sweep.py --apply` on the affected units (the whole game when the fix is general); for a
  search tool, this sprint's parked near-misses. Matches outside the unit go into their own
  `feat(match)` commit in the same PR. Verify them strictly like any other match.
- **Encode hand wins.** If a hand change that produced a match is mechanical, add it as a
  search dimension or a sweep repair pass; otherwise record it as one quirk bullet in
  [C matching](code_matching_workflow.md#measure-and-iterate).
- **Redesigns are leads, not sprint work.** Record them in the PR's Workflow line. Never
  weaken the oracle (verifier strictness, compiler pins, split boundaries) to get a match.
- **Record the yield.** The finish record's Workflow line lists the branch's tooling
  commits; add what each one unlocked. A parked row whose plateau comes from a tool gap
  says so in its `next:`.

## Kickoff

1. Take the unit's near-misses from `work/sweep/results.json`, then fill with
   `python3 tools/next_target.py --unit UNIT --limit 40`.
2. Run `bash build.sh` if `build/` is stale, then
   `python3 tools/sprint_finish.py --baseline --queue FN1 FN2 ...` to record the objdiff
   figures, the strict-only count and the DOL hash. It refuses to run when the checkout is
   behind `origin/main`; update first (merge, or rebase while nothing is pushed). If it
   cannot fetch, it warns and checks against the last fetched `origin/main`.

Reuse the checkout's toolchain and build. Bootstrap only if prerequisites are missing;
for matching setup use `LSW1_NO_GHIDRA=1 bash tools/bootstrap.sh` and request Ghidra
separately when needed. Do not start decompilation while setup still owns its project.
`python3 tools/draft_fn.py FN1 FN2 ... --no-ghidra --print` drafts several functions in one
call and prints each draft with its assembly, avoiding a headless Ghidra launch per draft.
Drafts declare globals that the target reaches through `@ha/@l` as arrays read with
`sym[0]`: ProDG puts scalar externs in small data, so a scalar declaration can never match
those loads.

For Ghidra, `python3 tools/ghidra_decompile.py decompile FN ...` writes `work/FN/ghidra.c`;
if `status` says the project is missing, run `setup` (about a minute). See
[Ghidra and REA](research.md#ghidra-and-rea). `work/NAME/context.md` holds the draft's type
context. Familiar mismatches are searchable in the C matching guide (`grep -n` on the
instruction or pattern) and in the "Failure modes" bullets of the
[batch history](phase3_batches.md).

## Queue accounting

`sprint_finish.py` rejects queue entries that are neither matched, parked nor deferred.
When a baseline exists, `score_fn.py` adds any function it scores to the saved queue;
`sprint_finish.py --queue-add FN...` adds functions by hand. List entries that were never
attempted or left unfinished with `--deferred FN ...`; they stay distinct from `--parked`
plateaus. A parked row needs a measured score, the remaining mismatch and a next
hypothesis, so it records an actual bounded search. For an existing baseline without a
queue, supply the full `--queue` at finish (repeated names are ignored).

### Optional measurement

Timing and usage are recorded only when a run wants throughput figures; finish does not
require them.

- `sprint_finish.py --phase setup|matching|tooling|finish|paused` starts or switches the
  timer (`--baseline` moves a timer still in setup to `matching`); `--timing` reports it.
  A gap between tool events counts as active time only up to the **idle cap** (15 min;
  `--idle-cap SECONDS` or `LSW1_IDLE_CAP`), so an unpaused stall cannot inflate the figure.
  An interrupted run resumes its existing timer. Missing historical measurements remain
  unknown.
- `tools/sprint_bench.py` (`start --model MODEL --launched-at EPOCH`,
  `time LABEL -- CMD`, `mark LABEL`, `report`, `compare`) logs wall-time spend per step
  and per `score_fn.py` attempt to `~/.cache/lsw1/bench/`, which outlives the worktree.
- `sprint_finish.py --usage work/provider-usage.json` imports provider-reported per-run
  cumulative totals:

  ```json
  {"run_id":"provider-run-id","model":"exact-provider-model","source":"provider export reference",
   "input_tokens":1000,"cached_input_tokens":800,"output_tokens":200,"cost_usd":null}
  ```

  Cached input is a subset of input and reasoning tokens are already in output; never add
  either twice. Reimporting a run replaces its counters; decreasing counters or a changed
  identity is rejected. `cost_usd` is the provider-reported charge or null, never an
  estimate. Do not import thread-lifetime totals as one run. Without exports, usage is
  unknown.

Do not project whole-project completion from one easy-function run.

## Per-function search

- Write new functions only through `score_fn.py --body try.c --apply`. A function not yet
  in SOURCE is appended for scoring, and SOURCE changes only on a strict MATCH, so parking
  needs no cleanup.
- At 95% exact or better `score_fn.py` prints `NEAR MISS`. Common causes are expression or
  operand order for a commutative instruction, how a pointer is formed (`&a[i][j]`,
  `a[i] + j`, an explicit field), declaration or initialisation placement, and inline
  helper boundaries. In #55, hoisting one `i = 0` moved `fn_8013D9FC` from 51/71 to 70/71.
- **Type a shared struct before drafting its family.** When several queued functions share
  one object, run `ghidra_decompile.py xrefs` and add the accessed fields to the header.
  Casts such as `*(s32*)((char*)p + 0x74c)` hide aliasing that changes the schedule.

| Tool | Use and bound |
|---|---|
| `score_fn.py NAME src/UNIT.c --body try.c --diff --apply` | scores `try.c` in place of NAME's definition, prints the TARGET/MINE listing on a mismatch, writes the unit only on a strict MATCH |
| `hillclimb.py try` | ledger of recorded candidates; reports `PARK` after 8 consecutive attempts without improvement ([defaults](code_matching_workflow.md#measure-and-iterate)) |
| `variant_search.py NAME src/UNIT.c -j 8` | once the draft is within about 10 instructions; over `--limit` it tries the fewest-change variants first |
| `order_search.py --climb` | when only statement order differs (up to 7 statements) |
| Permuter | at about 90% exact or better, after m2c cleanup, with a `/* permute: begin */` focus region; bursts of at most 180 s (`timeout 180`) |

The first `hillclimb.py try` records `--donor-note`. When the pinned OpenSaga checkout has
no name hit, `hillclimb.py` records the no-hit limit itself; pass a note only when you
searched or read donor code. A parked function gets a row in the PR's Parked table and a
`work/NAME/status.md` note with the best score, the remaining mismatch and the next
hypothesis.

### Long permuter runs

Unattended long runs (`tools/permute_long.sh`) start only when the user asks for one; a
parked near-miss records a permuter run as its next hypothesis for the user to queue.
The queue survives the session.

- Read `work/permute/queue_summary.txt` (or `tools/permwatch.py --once`) for results.
  Review each improved target with `tools/permute_review.py NAME`, comparing prototypes
  and externs as well as the body (in #65 the `fn_800FC5C0` fix was `fn_80095558`
  returning `void`). Port a score-0 output with `score_fn.py --apply` and
  `verify_fn.py --strict-symbols`.
- Give overnight runs a backlog well beyond what the slots can use, so early matches do not
  leave slots idle:
  `tools/permute_long.sh 7 2 --total 8 $(python3 tools/permute_backlog.py --slots 7 --hours 8 --per 2 --limit 60)`.
  Targets not started stay in the backlog for the next run.

## Finish

```sh
python3 tools/sprint_finish.py FN1 FN2 ... --unit UNIT --merge \
  --parked "FN3=41/51: <mismatch>; next: <hypothesis>"
```

Commit first. With `--unit` given as the source path, `--matched-from-diff` adds every
function newly defined in it since the merge base. Use `--merge` once the branch is pushed
(it fetches, merges `origin/main` and aborts cleanly on a conflict). `--rebase` is only for
a branch with nothing pushed; it refuses when a commit it would rewrite is on the remote.
The script runs `bash build.sh` once, strict-verifies each function against that build,
runs `git diff --check origin/main`, compares the DOL hash and the report with the kickoff
baseline, and prints the record below. Fill in Donor and Headers and each tooling commit's
yield. It exits non-zero on any failure.

A function can pass the strict verifier yet sit below 100% in objdiff, because a literal reloc
targets `.rodata+N` rather than the retail `lbl_` symbol. Those do not move `matched_functions`.
`matching_progress.py --strict-gap` (and `tools/strict_gap.py`) counts them separately; report
that figure, not an adjusted objdiff total. In a unit with a `.rodata` range
([provisional ranges](build_config.md#provisional-rodata-ranges)), such a function counts once
the unit's literals sit at retail offsets. An `extern lbl_` reference already matches by name.
To promote the unit, its C must emit every literal in the range, strings included, in retail order.

If main moved since the baseline, the report delta includes main's changes; record
"includes main drift: yes" rather than building a separate baseline worktree. Run
`verify_unit.py UNIT --build` only when every function in the unit matches, then promote
if it passes.

## Records

The PR body is the sprint record. There is no per-sprint section in `strategy.md` and no
tracked timing file.

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
- Workflow: <tooling commits and what each unlocked, or none>
```

Only durable facts go into docs, in the same PR:

- a confirmed struct field goes into the header, with a short comment naming its consumer;
- a new layout fact goes into the table in [Nu2](nu2_engine_reference.md) or
  [game structs](game_structs.md);
- a new compiler quirk becomes one bullet in [C matching](code_matching_workflow.md), and a
  tool change when it is mechanical (see [improve the workflow](#improve-the-workflow-as-you-go)).

Update `strategy.md` only when the plan changes, for example when a lane is finished or
the lane order changes.

## Kickoff prompt

```text
Sprint: UNIT. Queue: next_target.py --unit UNIT --limit 40 (or: fn_A, fn_B, ...).
Follow docs/sprint_workflow.md. Branch t3/sprint-<unit-leaf>.
```
