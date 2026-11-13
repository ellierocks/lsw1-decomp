# Sprint workflow

This is a reference for matching tools and evidence, not a prescribed agent schedule.
Choose targets, search methods, scope and effort for the requested outcome. T3 owns
orchestration and continuation. [Strategy](strategy.md) describes current opportunities;
[quickstart](matching_quickstart.md) covers setup and scoring.

## Scope

- **Build before sweeping a fresh checkout.** Run `bash build.sh` to completion if
  the checkout has no generated target assembly. Bootstrap supplies tools and retail
  input, but does not split the DOL. Do not overlap this initial build with scoring:
  rebuilding the shared `wibo` executable can make compiler launches fail with
  `Text file busy`. A sweep with `NO-ASM` rows is incomplete; build, then rerun
  the affected units before using its ranking.
- **Automatic matches.** `sweep.py --apply` drafts and strict-scores ready targets;
  `sweep_permute.py --apply` searches near-misses. Use fresh results or rerun affected
  targets when sources or tools change. `work/sweep/results.json` ranks opportunities;
  `work/sweep/NAME/sweep_draft.c` holds drafts. Each pack's `asm.s` is refreshed from the
  current build before drafting (`tools/pack_asm.py`). Automatic scoring uses `--no-queue`;
  interactive scoring adds targets to an existing queue unless given that flag.
  m2c drafts and score verdicts are cached in `~/.cache/lsw1/sweep`, shared by every
  checkout, so a rerun after a tool fix rescores only the drafts that changed. A score is
  keyed on the scoring tools (`SCORE_TOOLS`), the unit source, its preprocessed context, the
  target's split object and assembly, and the body; set `LSW1_SWEEP_CACHE` to a private
  directory to time a cold run. Without
  `--apply`, `sweep_permute.py` can run in the background while you draft;
  `--apply-saved` then writes its confirmed matches.
- **Plan items are the scope.** `next_target.py`, `sweep_permute.py`, the near-miss lane
  and `hillclimb.py` default to the plan target set: the item this checkout claimed with
  `kickoff.py --start --item ID`, else the plan's next item ([strategy](strategy.md#rule)).
  `--all` restores the global order; `--off-plan "REASON"` starts or records anything else.
  Related functions can share types and draft shapes; an item is a grouping, not a limit on a
  tool fix's scope or on collecting its matches.
- **Ownership is by claim and branch name.** Run `python3 tools/git_origin.py fetch origin`
  first. An item is claimed by a record under `<git common dir>/lsw1-plan-claims/` (every
  local worktree sees it; stale once its worktree or branch is gone or its post-claim commits
  are merged into `origin/main`), by a local or `origin` `t3/plan-<item>` branch not yet
  merged, or by an open PR of that name (asked through `gh`; `--no-remote` turns that off).
  `python3 tools/plan.py` lists claims; `--release ID` drops a record. Legacy
  `t3/sprint-<unit-leaf>` branches still own their unit, and so the items listing it;
  `next_target.py --all --by-unit` skips such units (ranked by bytes in their first ten ready
  targets; `*` marks ten or fewer functions left, not promotion readiness). Other branch names are not covered;
  inspect active work before editing. `work/` claim markers are local to each worktree.
- **Shared types need evidence.** Confirm fields against target accesses and their
  consumers; leave unknown layout as padding or local views.
- **Promotion needs data too.** Check all owned sections before counting on a promotion
  path; matching every function does not emit missing data.

## Improve the workflow as you go

Improve the tools as part of matching, rather than repeatedly repairing their output by
hand. Drafting fixes, m2c patches, sweep repairs and new search dimensions can unlock
whole classes of functions. Do not wait for an arbitrary repeat count when the cause
and benefit are already clear.

### Choose by metric, record before and after

The [workflow metrics](workflow_metrics.md) are what a workflow change has to move:

- matched bytes per child-hour, the outcome
- exact-instruction gain per child-hour
- compile-error rate
- distinct candidates per unfinished function
- time to first compiling score
- park recovery rate
- score and build latency

[Workflow gaps](workflow_gaps.md) ranks the open gaps by the metric each one moves and
by how far that metric is from its target. Take the gap at the top unless a blocker in
front of you is cheaper to fix and moves the same metric.

Every workflow fix records one number before and one after. Use
`python3 tools/workflow_metrics.py report` or a cohort to note the metric's value on
the runs the fix will affect. After the fix, add a cohort and a `[[comparison]]` to
[`config/workflow_metrics.toml`](../config/workflow_metrics.toml). Put
`workflow_metrics.py compare`'s row in the PR, update the gap's row in the gap table,
and name everything else that changed (model, units, starting drafts). A fix that
moved no metric, or the wrong one, is recorded as well. For sweep blockers,
`sweep_report.py --json` before and `--compare` after is the per-class measure, as below.

### Matching brief

These rules come from the 2026-11-12 comparison
([measured](workflow_metrics.md#structural-brief-vs-short-passes): 39× matched bytes per
child-hour, compile errors 69% → 4%). Give them to every child and sprint.

1. **Recover structure from the target assembly first.** Before tuning a draft, read
   which offsets the target loads and stores, through which base registers, and at what
   widths. Recover the struct and pointer relationships from that: local struct sizes,
   pointers formed from `&local`, float values the target reuses, literal floats versus
   rodata labels, and argument order. Every Opus fix that day was of this kind. Fix
   types and layout; do not tune registers or pad frames with scratch locals. Register
   order is the last step, and only once the instruction stream matches.
2. **Diff by asm block.** Use `score_fn.py --diff` and walk the target one basic block
   at a time. Find the first block that differs and explain it before changing a later
   one.
3. **A plateau needs 25 distinct compiled candidates.** Count hypotheses the compiler
   saw as different, not calls: the `codegen` hash in the bench log tells you when two
   edits compiled to the same code. With fewer than 25, the function is not a plateau.
   Keep going, or hand it off as unfinished with the hypotheses that remain. Do not
   defer it while structural hypotheses remain. A later pass matched 53% of the
   functions the short passes left.
4. **Seed searches with compiling source.** `variant_search.py`, `order_search.py` and
   the permuter only test hypotheses when the variants compile. If more than about 20%
   of a search's scores are ERROR (`workflow_metrics.py report` gives the rate per
   source), stop it, read the diagnostics and fix the seed or the types. Runs that
   relied on searches lost 91–94% of their scores this way.
5. **Count only new work.** A function that was already MATCH when first scored is a
   reverification. Report `new_verified` ([gap
   10](workflow_gaps.md#10-verification-counts-were-mistaken-for-new-progress)).

Keep fixes reproducible and test the behavior they change. m2c changes belong in
`tools/patches/m2c-*.patch`, applied by `tools/fetch_m2c.sh` to its pinned checkout.
Use an isolated `M2C` checkout for experiments so other worktrees' shared cache is not
silently changed. Preserve the patch in the repo rather than leaving a cache-only fix.

Rerun the improved tool on affected targets and collect its strict matches, including
other units when relevant. Explain what the fix unlocked and retain useful compiler or
layout findings in the owning reference. Choose the implementation and commit structure
that fits the work; tool improvements are part of the task, not automatically a separate
project. Never weaken verification, compiler pins or split boundaries to manufacture wins.

To choose which fix to make, [sweep blockers](sweep_blockers.md) ranks what stops the last
full sweep by class, with functions and bytes. Regenerate it with
`python3 tools/sweep_report.py --write`; measure a fix with `--json` before and `--compare` after.

To add a sweep repair, add one new module to `tools/sweep_repairs/` and edit no shared
file. Decorate the rewrite with `@repair(stage, order)` (`from . import repair`); the
stages and their signatures are listed in `tools/sweep_repairs/__init__.py`. Pick an
order no other repair in that stage uses: the package imports every module itself, and
a taken slot or function name raises at import. Keep the module's regexes in the module,
and test the rewrite in a new test file of its own. The result flag written into a
row is the function's name. Use the `final` stage for new speculative repairs: it
tries each alternative against the completed pipeline, preserving that result.
It accepts one rewritten body or a bounded list; near-match variants also run on
each alternative. Earlier greedy stages can change which later repair wins.

## Kickoff

`python3 tools/kickoff.py` reports the checkout, sweep, sibling worktrees, measured yield and
off-plan lanes, and recommends the plan's next item. `python3 tools/kickoff.py --start --item
ID` claims the item and queues its functions; off plan, `--start UNIT [FN ...] --off-plan
"REASON"` (or `--start-lane sweep|near-misses --off-plan "REASON"`) queues the given names or
the unit's near misses plus `next_target.py --all --unit UNIT --limit 40`. Either prints
ownership conflicts, records the baseline
(objdiff figures, strict-only count, DOL hash) and opens the bench run; rerunning it only
adds names. It refuses when the checkout is behind `origin/main` (update first: merge, or
rebase while nothing is pushed; with no fetch it checks the last fetched `origin/main`) or
has no build. It does not rebuild: run `bash build.sh` when `build/` is missing or stale. For
work a sibling already parked, run `python3 tools/worktree_cleanup.py --index-only` and check
`work/archive/worktrees/INDEX.md` in the main checkout.

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
  `--idle-cap SECONDS` or `LSW1_IDLE_CAP`), which limits the contribution from idle gaps.
  An interrupted run resumes its existing timer. Missing historical measurements remain
  unknown.
- `tools/sprint_bench.py` (`start --model MODEL --launched-at EPOCH`,
  `time LABEL -- CMD`, `mark LABEL`, `report`, `compare`) logs wall-time spend per step
  and per `score_fn.py` attempt to `~/.cache/lsw1/bench/`, which outlives the worktree.
  Each score event carries the search tool that made it (`source`), the score's wall
  time (`seconds`) and a hash of the compiled code (`codegen`). Finish archives the raw
  log to `~/.cache/lsw1/bench/events/`, and `tools/workflow_metrics.py` computes the
  [workflow metrics](workflow_metrics.md) from it.
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

- To score and apply a candidate without installing a mismatch, use `score_fn.py --body try.c --apply`. A function not yet
  in SOURCE is appended for scoring, and SOURCE changes only on a strict MATCH, so parking
  needs no cleanup.
- At 95% exact or better `score_fn.py` prints `NEAR MISS`. Common causes are expression or
  operand order for a commutative instruction, how a pointer is formed (`&a[i][j]`,
  `a[i] + j`, an explicit field), declaration or initialisation placement, and inline
  helper boundaries. In #55, hoisting one `i = 0` moved `fn_8013D9FC` from 51/71 to 70/71.
- Shared types can help a family of functions; `ghidra_decompile.py xrefs` supplies
  access evidence before adding fields.
  Casts such as `*(s32*)((char*)p + 0x74c)` hide aliasing that changes the schedule.

| Tool | Behavior |
|---|---|
| `score_fn.py NAME src/UNIT.c --body try.c --diff --apply` | scores `try.c` in place of NAME's definition, prints the TARGET/MINE listing on a mismatch, writes the unit only on a strict MATCH |
| `hillclimb.py try` | ledger of recorded candidates; configurable `--patience`, `--max-attempts` and verifier `--timeout` ([defaults](code_matching_workflow.md#measure-and-iterate)) |
| `variant_search.py NAME src/UNIT.c -j 8` | searches source rewrites; over `--limit` it tries the fewest-change variants first |
| `order_search.py` | exhaustive statement-order search defaults to `--limit 5040` (7!); `--climb` handles larger spaces with a configurable `--budget` (default 2000 scored variants) |
| Permuter | searches candidate rewrites; `/* permute: begin */` focuses mutations. Choose runtime and parallelism for the target and available resources |

The first `hillclimb.py try` records `--donor-note`. When the pinned OpenSaga checkout has
no name hit, `hillclimb.py` records the no-hit limit itself; pass a note only when you
searched or read donor code. A parked function gets a row in the PR's Parked table and a
`work/NAME/status.md` note with the best score, the remaining mismatch and the next
hypothesis.

### Long permuter runs

`tools/permute_long.sh` manages persistent local queues with explicit slot and time
limits. Choose those limits for the task and host; the queue survives the session.
See [permuter operation](code_matching_workflow.md#measure-and-iterate) for output
review and stopping the managed jobs.

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
`python3 tools/rodata_layout.py diff UNIT` shows where the layout diverges and why;
stripped pools and extra TU header copies are carried with `rodata_layout.py placeholders`
([stripped constant pools](build_config.md#stripped-constant-pools-and-tu-starts)).

If main moved since the baseline, the report delta includes main's changes; record
"includes main drift: yes" rather than building a separate baseline worktree. Run
`verify_unit.py UNIT --build` only when every function in the unit matches, then promote
if it passes.

## Records

Keep matching results and tool improvements in the PR rather than duplicating them
in strategy or timing files. This template is an example of the evidence to retain:

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
Plan item: ID. Branch t3/plan-ID.
Run python3 tools/kickoff.py --start --item ID --model MODEL.
Follow the matching brief in docs/sprint_workflow.md#matching-brief.
```

Off plan (Ellie asked, a tooling fix, or runtime evidence):

```text
Sprint: UNIT (optionally: fn_A, fn_B, ...). Branch t3/sprint-<unit-leaf>.
Run python3 tools/kickoff.py --start UNIT [fn_A ...] --off-plan "REASON" --model MODEL.
Follow the matching brief in docs/sprint_workflow.md#matching-brief.
```
