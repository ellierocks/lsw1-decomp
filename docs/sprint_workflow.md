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
  checkout, so a rerun after a tool fix rescores only the drafts that changed. Without
  `--apply`, `sweep_permute.py` can run in the background while you draft;
  `--apply-saved` then writes its confirmed matches.
- `next_target.py --by-unit` ranks units; `--unit UNIT --limit N` lists targets.
  Related functions can share types and draft shapes. Unit selection is a useful
  grouping, not a limit on a tool fix's scope or on collecting its matches.
- **Ownership is by branch name.** Run `python3 tools/git_origin.py fetch origin` first.
  `--by-unit` then skips units owned by an open `t3/sprint-*` PR (asked through `gh`;
  `--no-remote` turns that off) or by any local or `origin` sprint branch not yet merged
  into `origin/main`, which covers a pushed sprint with no PR yet. For a single-unit sprint, use
  `t3/sprint-<unit-leaf>` for automatic ownership detection. Other branch names and
  cross-unit work are not covered by this heuristic; inspect active work before editing. `work/` claim markers are local to each
  worktree and invisible elsewhere. Units are ranked by bytes in their first ten ready
  targets; `*` marks ten or fewer functions left, not promotion readiness.
- **Shared types need evidence.** Confirm fields against target accesses and their
  consumers; leave unknown layout as padding or local views.
- **Promotion needs data too.** Check all owned sections before counting on a promotion
  path; matching every function does not emit missing data.

## Improve the workflow as you go

Improve the tools as part of matching, rather than repeatedly repairing their output by
hand. Drafting fixes, m2c patches, sweep repairs and new search dimensions can unlock
whole classes of functions. Do not wait for an arbitrary repeat count when the cause
and benefit are already clear.

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
row is the function's name.

## Kickoff

Before choosing targets, run `python3 tools/worktree_cleanup.py --audit` and `--index-only`,
and check `work/archive/worktrees/INDEX.md` in the main checkout for work a sibling already parked.

For the queue-based sprint helper:

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
  `--idle-cap SECONDS` or `LSW1_IDLE_CAP`), which limits the contribution from idle gaps.
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
Sprint: UNIT. Queue: next_target.py --unit UNIT --limit 40 (or: fn_A, fn_B, ...).
Follow docs/sprint_workflow.md. Branch t3/sprint-<unit-leaf>.
```
