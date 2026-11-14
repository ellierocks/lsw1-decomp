# Workflow gaps

Review of 2026-11-10 over the session threads of 2026-10-30 to 2026-11-10, PRs #1 to
#108, CI history and the tools, extended on 2026-11-12 with the first [workflow
metrics](workflow_metrics.md) baseline. Each gap has its evidence and its state. Update
the state when a gap closes; delete the row when nothing remains to say.

## Ranking rule

A gap is ranked by the [workflow metric](workflow_metrics.md#metrics) it moves:

1. the metric's distance from its target in the latest baseline,
2. times the share of child time it touches,
3. with ties going to the cheaper fix.

Matched bytes per child-hour is the outcome; every other metric is ranked by how
directly it feeds that. A gap that no metric measures goes below the measured ones until
a metric covers it. If it matters, add that metric to `tools/workflow_metrics.py`.

A fix records the metric's value before and after in its PR and in this table
([recording a change](workflow_metrics.md#recording-a-change)).

### Open gaps, ranked

| Rank | Gap | Metric | Baseline 2026-11-12 | Target |
|---:|---|---|---|---|
| 1 | [8. Functions without a source unit are not swept](#8-functions-without-a-source-unit-are-not-swept) | matched bytes per child-hour (sweep) | 131 functions, 131,080 B unswept | 0 |
| 2 | [9. Strict-only functions wait on `.rodata` layout](#9-strict-only-functions-wait-on-rodata-layout) | promoted bytes (not per run) | 203 functions, 54,460 B | 0 |
| 3 | [13. Score latency is not yet measured](#13-score-latency-is-not-yet-measured) | `score_fn` latency | 0.13 s median (opus-gapfix-2026-11-13) | lower |
| 4 | [7. Sweep runtime and cache invalidation](#7-sweep-runtime-and-cache-invalidation) | build/score latency (sweep) | about 1,000 s cold, 238 s warm | lower |

Gaps 11 and 12 were ranked first and second until the fixes below. The first cohort
that used them (`opus-gapfix-2026-11-13`, apiobject) measured 0.5% compile errors and a
median of 48 distinct candidates per park.

### All gaps

| # | Gap | Metric moved | State |
|---|---|---|---|
| 1 | [Stale task packs change scores](#1-stale-task-packs-change-scores) | exact-instruction gain | fixed, #111 |
| 2 | [Unfinished and parked work is invisible across worktrees](#2-unfinished-and-parked-work-is-invisible-across-worktrees) | park recovery | fixed, #110 |
| 3 | [Transient download failures turn main red](#3-transient-download-failures-turn-main-red) | none (CI) | fixed, #109 |
| 4 | [`tools/sweep.py` is one module that every sprint edits](#4-toolssweeppy-is-one-module-that-every-sprint-edits) | none (merge friction) | fixed, #109, #114 |
| 5 | [The plan document carries results, and its next lead is not done](#5-the-plan-document-carries-results-and-its-next-lead-is-not-done) | start to first compiling score | fixed, #113, #116 |
| 6 | [Smaller recurring friction](#6-smaller-recurring-friction) | mixed | partly fixed |
| 7 | [Sweep runtime and cache invalidation](#7-sweep-runtime-and-cache-invalidation) | score and build latency | partly fixed, #119 |
| 8 | [Functions without a source unit are not swept](#8-functions-without-a-source-unit-are-not-swept) | matched bytes per child-hour | open |
| 9 | [Strict-only functions wait on `.rodata` layout](#9-strict-only-functions-wait-on-rodata-layout) | promoted bytes | partly fixed, numath promoted |
| 10 | [Verification counts were mistaken for new progress](#10-verification-counts-were-mistaken-for-new-progress) | matched bytes per child-hour (accounting) | accounting fixed, #163; the structural brief is in [sprint workflow](sprint_workflow.md#matching-brief) |
| 11 | [Search attempts mostly fail to compile](#11-search-attempts-mostly-fail-to-compile) | compile-error rate | fixed: searches refuse a failing seed and stop error runs; replay 47.1% → 4.1% (11×); measured 0.5%, searches 1 / 5,036; aisys pointer drafts 4/4 → 0/4 errors |
| 12 | [Functions are parked before 25 distinct hypotheses](#12-functions-are-parked-before-25-distinct-hypotheses) | distinct candidates; park recovery | fixed: `sprint_finish.py` refuses parks under 25; measured median 48, 3 of 3 parks at the floor |
| 13 | [Score latency is not yet measured](#13-score-latency-is-not-yet-measured) | `score_fn` latency | baseline 0.13 s median score, 0.9 s finish build |
| 14 | [Bench logs were polluted and lost](#14-bench-logs-were-polluted-and-lost) | all (data quality) | fixed |
| 15 | [Strict-only functions were counted as plan work](#15-strict-only-functions-were-counted-as-plan-work) | functions left (targeting) | fixed: plan functions left 1,306 → 1,147 |
| 16 | [Children got single functions, not items](#16-children-got-single-functions-not-items) | functions per child | fixed: `plan.py --batch N` and brief rule 6 |
| 17 | [Plateaued functions kept being scored](#17-plateaued-functions-kept-being-scored) | matched bytes per child-hour | fixed: `score_fn.py` prints a PLATEAU notice |
| 18 | [Body splicing dropped declarations needed before the new use](#18-body-splicing-dropped-declarations-needed-before-the-new-use) | compile-error rate | fixed: controlled evaluator replay 1/1 → 0/1 errors; two regression tests fail before the fix |
| 19 | [Fresh worktrees had no Ghidra project](#19-fresh-worktrees-had-no-ghidra-project) | start to first compiling score | fixed: `draft_fn.py` seeds the project; worktrees with `ghidra.c` 0 → all |
| 20 | [Drafts lost the unit's types](#20-drafts-lost-the-units-types) | compile-error rate; exact-instruction gain | fixed: drafts compile 30/60 → 54/60 |
| 21 | [Rodata float labels hide literal pools](#21-rodata-float-labels-hide-literal-pools) | exact-instruction gain (sweep) | fixed: `float_literals` repair; nuportal sweep +9 exact over 5 functions |
| 22 | [Plan items were cut to 15 functions](#22-plan-items-were-cut-to-15-functions) | functions per child | fixed: nuportal listed 15/22 → 22/22 |
| 23 | [Kickoff queued sweep matches as hand work](#23-kickoff-queued-sweep-matches-as-hand-work) | matched bytes per child-hour | fixed: sweep-matched functions handed out as hand work 1 → 0 (nuportal) |
| 24 | [`score_fn.py --apply` skipped silently](#24-score_fnpy---apply-skipped-silently) | start to first compiling score | fixed: unexplained skips 3 paths → 0; `--json` MATCH applied 0 → 1 |
| 25 | [Handoffs skipped the hypothesis floor](#25-handoffs-skipped-the-hypothesis-floor) | distinct candidates; compile-error rate | fixed: aisys-1 short handoffs accepted without a reason 7 → 0; ERROR events with a diagnostic class 0% → all new |
| 26 | [Order search needed hand markers](#26-order-search-needed-hand-markers) | distinct candidates | fixed: markers needed 1+ → 0; `--body` output 1,992-line unit → 76-line definition |
| 27 | [Declaration search permuted assignments](#27-declaration-search-permuted-assignments) | compile-error rate | fixed: same NuMtlRender seed, 96/279 errors (34.4%) → 0/183; no additional match |

## 1. Stale task packs change scores

`sweep.py` and `draft_fn.py` generated `work/sweep/NAME/asm.s` only when it was missing,
and `ls1_task_pack.py` refuses to overwrite an existing pack. A pack therefore kept the
assembly of the build it was first made from. After the relocation fixes in #94 and #104,
the cleanup session of 2026-11-10 measured `BoltHitGameObjects` at 297 from the main
checkout's pack of 2026-11-07 and 246 from fresh assembly. The m2c cache added in #108 keys
on the assembly text, so it faithfully cached drafts of the stale input.

The fix found a second cause. `extract_asm` searched every `.s` under `build/GL5E4F/asm`,
and dtk leaves the assembly of earlier splits there: 30 of 306 files in the main checkout
were leftovers, searched first, and 911 functions had a different body in a leftover than
in their current unit. Of the 1,522 packs in the main checkout's `work/sweep`, 11 were
stale on 2026-11-10: 6 from leftover files and 5 from later builds.

Fix: packs carry a stamp of their source assembly and refresh `asm.s` when the build
changes, keeping hand-written files, and extraction reads only the units the current build
lists (#111). Object packs' `combined.s` is not refreshed.

## 2. Unfinished and parked work is invisible across worktrees

Each session works in its own worktree; 23 existed on 2026-11-10. `git status` and the
ignored `work/` of one are invisible from the others.

- The `t3/sweep-speed` worktree held 8 uncommitted tool files for 6 days until the cleanup
  session found them (#108). The JP PC workflow child left a commit unpushed with no PR.
  Sessions of 2026-10-30 and 2026-10-31 ("Continue Phase One", "Integrate OpenSaga",
  "Phase 3: mid-size batch A", "Find Decompilation Goldmines") each found dirty or unpushed
  sibling worktrees by accident.
- Parked notes and best candidates live in each worktree's `work/`. The plateaus of #96,
  #97 and #99 reached the repository only because #101 copied them by hand into
  [batch history](phase3_batches.md).
- `work/archive/worktrees/INDEX.md` was last rebuilt on 2026-11-03. Only
  `worktree_cleanup.py` rebuilt it, only from archived worktrees, and removal is
  deliberately batched, so the index never covered live work.

Fix: `worktree_cleanup.py --audit` reports every worktree's dirty, unpushed and unmerged
state without changing anything, and `--index-only` now reads parked notes from live
worktrees (#110). [Sprint kickoff](sprint_workflow.md#kickoff) runs both. Its first run
listed 82 parked rows, 24 from live worktrees, and two worktrees with uncommitted files.

## 3. Transient download failures turn main red

`tools/download_tool.py` opened each URL once. The main push run 42476781878 (merge of
#102, 2026-11-08) and README run 42477421598 failed on `HTTP Error 504: Gateway Timeout`
while fetching binutils and wibo, so main showed a failed build and the README figures
went unpublished until the next merge. Superseded `ci/readme-progress-<sha>` branches were
also never deleted; 11 remained on `origin`.

Fix: bounded retry with backoff for transient failures, and a progress branch that main has
overtaken deletes itself (#109). The 11 existing branches are left for the next cleanup day.

## 4. `tools/sweep.py` is one module that every sprint edits

`tools/sweep.py` has about 1,750 lines and 100 top-level definitions, and 39 commits have
touched it. Sprint branches that add a repair all edit the same file: #97 and #98 both
needed conflict resolution in `sweep.py` and `variant_search.py`, and two branches each
added a module-level `SWITCH` regex, the later silently shadowing the earlier until
`test_switch_chain` failed for an unrelated reason
([batch history](phase3_batches.md#sprints-of-2026-11-0708-handoff)).

Fix so far: a test rejects a tool module that binds one top-level name twice (#109).

Fixed (#114): the repairs live in `tools/sweep_repairs/`, one module per family with
its regexes beside it, registered by stage and explicit order; a new repair is one new
file, and a taken slot or name fails at import ([how to add one](sprint_workflow.md#improve-the-workflow-as-you-go)).

## 5. The plan document carries results, and its next lead is not done

[Strategy](history/strategy_2026-11-10.md#phase-3-agent-draft-and-climb-for-game-code) says sprint results stay
in the PR, but its "Automatic progress" paragraph is a 50-line history of sweep figures
dated 2026-11-08. The first next lead of the 2026-11-08 handoff, rerunning the full sweep
above 0x100 with all merged repairs, had not been run two days and seven PRs later; the
credits sprint stopped on a rate limit with `fn_8010BA24` at 52/57.

Fixed: #113 reran the full sweep on fresh packs. #116 moved the dated figures to the
[sweep history](phase3_batches.md#sweep-history), left strategy with the method, and added
`tools/kickoff.py`, which prints the current figures and flags a stale sweep from the
commits touching its inputs since it ran.

## 6. Smaller recurring friction

- Sweep repairs accepted while both forms failed compilation could hide a later
  compiling form. Checkpoint retries recover those paths, including when the
  accumulating path eventually compiles. New repairs run as independent final
  alternatives so a locally better rewrite cannot block a better downstream path.
- Cached compile diagnostics carried a random `score_fn_*` temporary directory.
  The sweep strips it on both cache writes and reads, including older cache entries.

- `sprint_finish.py --matched-from-diff` counted a `static inline` helper (`ForceObjsReady`,
  #98). That case was already fixed; #109 fixed `inline static` and attribute-prefixed
  helpers, and #116 a return type on the line above the name.
- `Text file busy` on the shared `wibo` appeared in six sessions between 2026-10-31 and
  2026-11-04. Downloads have replaced the binary atomically since; no later session hit it.
- Rate limits stopped about ten sessions mid-task. This is outside the repository; short
  scoped threads and the audit in gap 2 limit what a stopped session can strand.
- Code-identical functions fail `--strict-symbols` on string-literal addresses
  (`fn_80076DF0`). The oracle is right; these wait for `.rodata` ownership (gap 9).
- The git guard blocks commits whose command runs in the main checkout. Use
  `git -C <worktree>`.
- `score_fn.py` adds every function it scores to the sprint queue, so rechecking a function
  main already matched (AIMoveInstruction, SetAIMessage and `fn_8008F968` in the apiobject
  benchmark) failed `sprint_finish.py --matched-from-diff` queue accounting. The workaround,
  naming them, listed 11 matches where 8 were new. Finish now leaves functions the unit
  already defined at the merge base out of accounting with a note (3 unresolved → 0;
  record 11 → 8). Both apiobject children hit the same refusal and passed `--queue` or
  `--deferred` by hand.
- `score_fn.py --body` failed with a cc1 redefinition when the body carried a `static inline`
  helper the unit already defined (the nugscn `NuGScnAlloc`/`NuVtxSetStride` drafts after
  a first `--apply`). The body's helper now replaces the unit's copy in place: rescoring such
  a body went from a compile error to a score (1 → 0 compile errors per rescore on
  `fn_80028DAC`, 2026-11-13; `tests/test_draft_score_helpers.py`).
- `tools/quick_wins.py` reads one hard-coded split file, `auto_01_8001752C_text.s`, which a
  later split can leave behind (see gap 1). It does not feed scoring, so it was left; point
  it at the current units when next touched.

## 7. Sweep runtime and cache invalidation

Measured on 2026-11-11 over the 1,353-row sweep, one run at a time with a private
`LSW1_SWEEP_CACHE`: the #117 tools took 1,616 s cold and 837 s warm; #119 takes about
1,000 s cold and 238 s warm, with the same verdict, exact count and target on every row.

- The warm run was Python under the GIL, not compiling: the globals repair's
  declaration pattern backtracks across long indentation on every failed line, 7 s for
  one 80 KB draft. A linear precheck now picks the lines it runs on (identical matches
  over 9,900 drafts and cached m2c outputs).
- Score keys dropped `configure.py` and `config.yml`, which `score_fn.py` never reads and
  which changed in 53 of 238 main commits, and now cover every module `score_fn.py`
  imports (a test checks the closure) and the retail split object `verify_fn.py` compares.
- A missing pack is written as `asm.s` in process instead of by `ls1_task_pack.py`.
- Compile errors drop the temporary directory before the 1,500-character cut and start at
  a whole line, so error text no longer differs between runs or begins mid-name.

Still open: the key covers the whole unit source, so applying one match rescores its
neighbours, and a cold run is now mostly ProDG compiles. A narrower key needs every
declaration the body can see; measure a resweep after an applied match before changing it.

## 8. Functions without a source unit are not swept

The blocker report records 131 unswept functions (131,080 bytes) without C source.
They need source-unit scaffolding and compiler wiring before the sweep can draft
them. This is missing coverage, not a compile-error repair; prioritize units by
expected matching yield and preserve the existing split boundaries.

## 9. Strict-only functions wait on `.rodata` layout

On 2026-11-11, 203 functions (54,460 bytes) passed `verify_fn.py --strict-symbols` but
scored below 100% in objdiff, because their units emit literals at other `.rodata`
offsets. `tools/rodata_layout.py` (`gap`, `diff UNIT`, `survey`) measures this and
classifies each byte. The cause in every affected unit is the same: retail ranges keep
the constant pools of dead-stripped functions and a `VuMath.h` string copy at each TU
start ([stripped constant pools](build_config.md#stripped-constant-pools-and-tu-starts)).
Retail-order definitions, the shared `VuMath.h` header and `rodata_layout.py
placeholders` reproduced and promoted `nutrig`, `nugraph_1`, `nugraph_3` and `nuvec`
(40 functions, 10,012 bytes).

Open: the other units are not code-complete, and most reference strings through
`extern lbl_` names that have to become literals first. Steps 1 and 3 of the mechanism
still raise strict-only functions in incomplete units once the functions before them
emit their pools. `nustring` also needs its string order explained and its range start
corrected.

## 10. Verification counts were mistaken for new progress

The 2026-11-12 Nu3d continuation exposed two separate problems. Short passes
ended while useful structural hypotheses remained, and the benchmark overstated
what those passes achieved. The `nuanim` run behind PR #162 reverified four
baseline matches (2,136 bytes) while adding zero matched report bytes. Kickoff
nevertheless displayed about 11,400 verified bytes per active hour as yield.
`sprint_finish.py` counted every successful final verification, and kickoff and
plan used that total to rank progress. The same issue credited PR #159's
pre-existing `fn_80015C88` match as a new result.

Kickoff now records strict results for the starting queue. Finish retains total
verification counts as validation evidence and separately records new strict
matches. Yield and plan warnings use the latter. Old baselines and newly added
queue entries without a baseline result remain unknown; report byte deltas
cannot replace the strict baseline because constant-pool differences and main
changes affect them. Historical logs are preserved, not retroactively guessed.

The benchmark logs also locate the effort problem: the second nuhgobj pass made
only 2–4 scoring calls per unfinished function, while the nuportal pass spent
724 calls on `NuAnimCurveEval` without a strict match: 684 returned ERROR
and only 40 produced compiling mismatches. Raw call counts therefore greatly
overstate the explored source shapes; a variant search needs a compiling seed
and its diagnostics inspected before treating its output as evidence of a plateau. Their recorded finish builds
took under a second, so those builds do not explain the low yield. The broad
sweep was also 58 input commits stale, limiting its value as a starting point.

Continuation should reconstruct types and control flow before tuning register
allocation. A low-scoring generated body is a draft, not a plateau; matching a
frame by adding scratch padding does not establish the target's local types.
Keep a child on its current unit while useful hypotheses remain, preserve each
measured candidate, and explain the unresolved instruction or data-layout issue
before deferring it. Do not count fresh worktrees, PRs, or reverified baseline
functions as matching gains.

## 11. Search attempts mostly fail to compile

Baseline 2026-11-12 (`workflow_metrics.py cohorts`): 23,053 of 48,978 scored attempts in
the 17 plan runs of 2026-11-11 were ERROR (47.1%). The Sol short passes reached 69.4%.
The runs that relied on `variant_search.py` reached 91–94%: hvqm4, nutexanm, the first
nucamera pass and nuportal. In nuportal, `NuAnimCurveEval` alone had 684 ERROR scores
out of 724. An ERROR attempt tests no hypothesis, but each one costs a full compile. The
Opus passes with the structural brief had 4.1%.

Search and hand-edit attempts were mixed in the log until now. Score events now carry
`source` (`manual`, `variant_search`, `order_search`), so the rate is reported per
source.

**Fix (2026-11-12).** `variant_search.py` and `order_search.py` (search and `--climb`)
now:

- score the unmodified source first and refuse to search when it does not compile,
  printing the compiler's diagnostic;
- stop after 64 consecutive ERROR variants (`--max-error-run`, 0 disables it);
- warn when more than 20% of a search's variants fail, naming the most common
  diagnostic;
- in `variant_search.py`, skip every combination that contains a rewrite which failed
  on its own.

Before → after, replayed on the logged attempts: a burst whose scores never compiled is
dropped, and any other burst is cut at its 64th consecutive ERROR.

| Cohort | Compile-error rate | Attempts |
|---|---|---|
| recent-2026-11-11 | 47.1% → 4.1% (11×) | 48,978 → 26,953 |
| sol-short-2026-11-12 | 69.4% → 30.0% (2.3×) | 1,048 → 424 |
| opus-brief-2026-11-12 | 4.1%, unchanged | 291, unchanged |

Most of the saving is two searches: `NuCameraCreate` (26,913 of 28,925 attempts ERROR)
and `IpicPlaneDec` (19,896 of 19,899). No logged match is lost. One stopped search,
`fn_80015BD0`, would have lost an exact-count improvement from 19 to 25, without a match.
The replay cannot model the pruning, and logs written under `-j` are in completion
order, so it approximates the seed.

**Measured (2026-11-13, `opus-gapfix-2026-11-13`).** Three Opus runs on apiobject (parent
and two children, 1.28 active hours) scored 5,268 attempts, 24 of them ERROR (0.5%):

| Source | Attempts | ERROR | Rate |
|---|---:|---:|---:|
| `variant_search` | 4,790 | 1 | 0.02% |
| `order_search` | 246 | 0 | 0% |
| `manual` | 232 | 23 | 9.9% (parent 16.0%, children 5.1% and 0%) |

That is below both replay predictions (4.1% for the 2026-11-11 runs, 30.0% for the Sol
passes) and below the structural-brief Opus cohort (4.1%). The searches account for 96%
of attempts and almost none of the errors; the error-run stop never had to fire. Hand
edits now carry the errors, and at least 3 of the parent's 20 came from scoring a mid-merge file.
Confounds: Opus rather than Sol, a different unit (apiobject, with OpenSaga bodies for
most functions), and searches started from seeds that already compiled, which the seed
check now requires. The logs do not record seed refusals, so how often the refusal itself
saved a burst is unknown.

**Measured repair (2026-11-13, `aisys-1`).** m2c inferred a local pointer minus
an integer-loaded base as a pointer, then multiplied it by a reciprocal to recover
a node index. ProDG rejects that multiplication. The final-stage
`pointer_byte_differences` repair casts the left operand to the GC address width
(`u32`) only for this narrow expression shape; ordinary pointer arithmetic and
pointer-pointer differences stay unchanged.

Replaying the same four drafts against the same baseline source and compiler
reduced compile errors **4/4 → 0/4 (100% → 0%)**: `Action_GoToNodeRandom`,
`Action_GoToNode`, `Action_MoveAwayFromNode` and `Action_BlockPath`. The
`aisys-pointer-before-2026-11-13` and `aisys-pointer-after-2026-11-13` metric
cohorts select those eight events, excluding an earlier replay against changed
source declarations. The full 32-function scoped sweep improved **5 → 1 errors
(15.625% → 3.125%)**; the remaining `AIScriptProcess` draft has a malformed
callback. This repair produced no automatic matches. All four repaired functions
subsequently matched after assembly-led layout and control-flow work. Three unit
tests cover the repair, exclusions and idempotence; all 91 sweep tests passed.
Local replay evidence is `work/pointer-repair-replay.json` and
`work/aisys-repair-sweep.log`.

## 12. Functions are parked before 25 distinct hypotheses

The median number of distinct compiled candidates per unfinished function was 1 in the
2026-11-11 runs and 2 in the Sol short passes. These counts are a proxy: distinct exact
counts, a lower bound, because old events have no codegen hash. The
[brief](sprint_workflow.md#matching-brief) asks for 25. A later pass matched 9 of the 17
functions the short passes left unmatched (52.9%). Seven of those nine took the Opus
passes 17–25 minutes, so they were not plateaus.

Score events now carry `codegen`, so the count is exact for new runs.

**Fix (2026-11-12).** `sprint_finish.py` refuses to finish when a `--parked` function has
fewer than 25 distinct codegen hashes across all discovered bench logs. The refusal names
each short function and its count. List the function under `--deferred` with the
hypotheses left, or pass `--short-park REASON` when the floor does not apply (for example
a function blocked on `.rodata` layout). The finish record keeps `parked_hypotheses` and
`short_park`. A function whose logs predate codegen hashes reads as unknown and is
allowed.

Before: all 9 of the 17 Sol leftovers that a later pass matched were under the floor
(proxy count), as were 16 of the 17 leftovers. Under the rule none of the 9 could have
been parked without a stated reason.

**Measured (2026-11-13, `opus-gapfix-2026-11-13`).** Distinct codegen hashes (exact) per
unfinished function:

| Function | Best | Distinct candidates | Park |
|---|---|---:|---|
| `fn_800902C0` | 350/351 | 125 | parked |
| `fn_80090FB0` | 103/136 | 48 | parked |
| `fn_8008FCD0` | 370/380 | 25 | parked; refused at 23, two more candidates, no gain |
| `fn_800913BC` | not attempted | none | `--short-park`: 0xC04 bytes, a long donor hand climb outside the pass |

The median is 48 (Sol short passes: 2, proxy). All three parks reached the floor, and
the one refusal cost two candidates. Park recovery is 0% so far, since no later pass has
run. Re-measure it when the next apiobject pass picks these up.
`workflow_metrics.py` first reported 25 here: two functions scored only while the merged
file failed to compile counted as unfinished with 0 candidates. It now skips
functions with no compiling score in a segment (cohort median 25 → 48).

## 13. Score latency is not yet measured

`score_fn.py` is the cost of every hypothesis, but no log recorded its wall time; only
finish builds were timed (median 0.8–0.9 s). Score events now carry `seconds`. Record the
first cohort's median here, then rank latency fixes (cache hits, compiling only the
function's TU) against it.

**Baseline (2026-11-13, `opus-gapfix-2026-11-13`).** The median `score_fn` wall time is
0.13 s (per run 0.14, 0.11 and 0.14 s, over 5,268 scores). The finish build median is 0.9 s.
At that cost 4,790 variant scores take about 10 minutes of compile time. Latency is not
what limits a pass now; hypothesis quality is. Keep the gap open for the sweep (gap 7),
where builds are long.

## 14. Bench logs were polluted and lost

Two problems damaged the logs:

- `test_no_queue_skips_queue_mutation_after_verifying` called `score_fn.main` without
  patching `bench_note`. Each test run in a checkout with an open bench run appended
  `f MATCH 0 0` to that run's real log. The test now patches it out, and
  `workflow_metrics.py` ignores non-ERROR scores with `target` 0.
- Raw event logs lived only in `work/` of each worktree, so a cleanup day deleted the
  evidence for every metric. `sprint_bench.py finish` now copies the log to
  `~/.cache/lsw1/bench/events/`, and `workflow_metrics.py archive` copies every log it
  can find. 38 logs were archived on 2026-11-12.

Parked and deferred function names now go into the finish record, so park recovery no
longer has to be inferred from scores alone.

## 15. Strict-only functions were counted as plan work

The apiobject pass of 2026-11-13 (`opus-gapfix-2026-11-13`) left `fn_8009098C` as the
item's only function. objdiff scored it 99.6%, but it already passed the strict verifier:
it waits on `.rodata` layout ([gap 9](#9-strict-only-functions-wait-on-rodata-layout)), not
on matching. Across the plan, 159 of the 162 open functions at 90% or more were strict-only.
`plan.py`, `kickoff.py` and `next_target.py` sent children to those functions, and an item
could not close.

`plan.py` now leaves strict-only functions out of an item's targets and lists them on a
`strict-only` line. `strict_gap.verified()` caches the verifier's answers in
`build/GL5E4F/strict_verified.json`, keyed on the report's mtime and size. The first call
after a build takes about 10 s; later calls take 0.2 s. `sprint_finish.py` and
`strict_gap.py` use the same cache.

Before → after: plan functions left 1,306 → 1,147 (1,005,800 → 959,992 bytes); closed
items 18 → 19 (apiobject closed).

## 16. Children got single functions, not items

The same pass matched 8 functions, against 14–25 in earlier runs. Each child received one
large function: child 2 spent its 12 minutes on `APILoadCharacterModels` alone. Once a
child finishes or parks its only function, it stops. `plan.py --next` names one item, so a
parent starting several children had no list of separate items to hand out.

`plan.py --batch N` lists up to N free items, each with its `kickoff.py --start --item`
command. It uses the current stage first, then looks ahead. Rule 6 of the [matching
brief](sprint_workflow.md#matching-brief) gives each child a whole item or queue.

Before → after: 1 item per `plan.py` call → N distinct items. Re-measure functions per
child in the next cohort.

## 17. Plateaued functions kept being scored

`fn_80090FB0` cost the parent 9.3 minutes after its best score stopped rising. Over that
cohort's logs, `sprint_metrics.plateau()` (25 distinct compiled candidates, the last 10
without a better exact count) fires on four functions. On three it fired before
2.2–11.4 minutes of further work with no gain: `fn_80090FB0`, `fn_800902C0` and
`fn_8008FCD0`. `APILoadCharacterModels` fired at 758/762 and still matched 4.6 minutes
later. The notice is therefore advice, not a stop.

`score_fn.py` prints `PLATEAU: ...` on hand-run scores once the rule holds. Search tools
set `$LSW1_BENCH_SOURCE` and stay quiet. Brief rule 6 says what to do next: park, try a new
hypothesis class, or move on.

Before → after: no signal → a notice at the plateau. The replay shows 13.6 minutes of
no-gain work across three functions, out of about 77 child-minutes in the cohort.

## 18. Body splicing dropped declarations needed before the new use

The nuanim evaluator draft declared `lbl_80190074` and the diagnostic callback
factory. Both already had correct extern declarations below the source stub, so
`score_fn.py --body` discarded the draft declarations. The replacement then used
an undeclared array and called an implicitly declared integer as a function pointer.
The candidate never reached the strict comparison.

The splicer now forwards the source's actual types before the replacement when
its body supplies externs declared later. Literal disabled branches are masked
when deciding which declarations exist, so a parked `#if 0` declaration cannot
remove an active body extern. Existing visible declarations are retained once.

Before → after: the identical evaluator candidate goes from compile ERROR to
254/449 instructions, **1/1 → 0/1 compile errors (100% → 0%)**. The controlled
replay cohorts `nuanim-forward-extern-before` and `nuanim-forward-extern-after`
in [workflow metrics config](../config/workflow_metrics.toml) use the same unit,
compiler, model and body; only the splice implementation changes. This measures
one blocker, not population throughput. The candidate remains unmatched.

The regression tests cover the real array/callback-factory failure with a host
C89 compile, disabled later declarations and an existing visible declaration.
Two tests fail against the original scorer and all three pass after the fix.

## 19. Fresh worktrees had no Ghidra project

The nuanim child (PR 166) reported "the local Ghidra project was unavailable". Ghidra 12.1
is installed and a shared seed existed, but only `tools/bootstrap.sh` ran
`ghidra_decompile.py setup`. T3 worktrees run `bash build.sh`, which never did, so
`draft_fn.py` printed a note and wrote no `ghidra.c`.

`draft_fn.py` now calls `ghidra_decompile.auto_setup()` when it would write `ghidra.c` and
the project is missing. That copies the shared seed only; a full import (about a minute) is
never started implicitly, and `LSW1_NO_GHIDRA=1` still opts out. The seed key also covered
`ghidra_decompile.py` itself, so any edit to the tool orphaned the seed and forced a
re-import. It now hashes the Java scripts and an `IMPORT_KEY` string, which changes only
when the ELF layout or the import arguments change.

Before → after: a fresh worktree's first draft had no `ghidra.c` → it has one. The first draft
takes 45 s once; later drafts with Ghidra take about 7 s, and 0.5 s with `--no-ghidra`.

## 20. Drafts lost the unit's types

The same child reported that "m2c pointer-return inference still loses evaluator float
expressions". `sweep.py` already gave m2c the unit source, preprocessed, as `--context`.
`draft_fn.py`, the tool children use by hand, ran m2c with no context. Every float return
and parameter came back as `void *` and struct fields as raw offsets.
`NuAnimCurve2CalcVal` drafted as `void *NuAnimCurve2CalcVal(void **, void *, s32)` with an
unparseable `fn_80009948` declaration. With the unit as context it drafts as
`f32 NuAnimCurve2CalcVal(struct nuanimcurve2_s *, struct nuanimtime_s *, s32)` with named
fields.

`draft_fn.py` now passes the owning unit's preprocessed source to m2c. `draft_m2c.c` is the
typed draft. `draft_m2c_plain.c` keeps the context-free draft for comparison, and
`--no-context` makes the plain draft the only one.

Before → after, measured on 60 random unmatched functions of 0x40–0x400 bytes (seed 7), each
scored with `score_fn.py --body` in its unit:

| Draft | Compiles | Mean exact instructions (all 60) |
|---|---:|---:|
| plain (before) | 30 / 60 | 21.3% |
| typed (after) | 54 / 60 | 41.8% |

Every function whose plain draft compiled also compiled typed. Of the 30 that compiled both
ways, the typed draft scored higher on 3, the same on 26 and lower on 1.

## 21. Rodata float labels hide literal pools

ProDG writes each function's float literals into its own `.rodata` pool, and m2c drafts
them as `extern f32 lbl_X[];` loads. The label and the literal load the same value, but
GCC schedules literal-pool loads differently: the `lis` order, and whether a constant
stays in a register across calls. In the nuportal sprint (s2-nu3d, 2026-11-13) three
hand matches needed only this change once their structure was right: `buildFrustrum`
(704 B), `drawViewFrustrum` (332 B) and `NuAnimCurveEval` (172 B), which had plateaued
at 145 candidates with labels.

`tools/sweep_repairs/literals.py` adds a final-stage `float_literals` repair. It reads
every single-`.float` 4-byte `.rodata` object from the retail assembly and rewrites a
label as the shortest C literal that rounds to the same bits, but only when every use is
`label[0]`. It produces the all-literal draft first, then each label alone. Near
`FLT_MAX`, dtk prints `3.4028235e38`, which is out of range for `struct.pack` and makes old
GCC warn, so the repair writes the truncated `3.4028234e+38f` instead.

Before → after (same unit source, nuportal sweep, exact instructions):
`NuAnimCurveEval` 33 → 36, `NuAnimCurveSetApplyBlendToJoint2` 256 → 258,
`NuAnimData2Fixup` 86 → 89, `NuAnimData2Relocate` 98 → 99 and `drawViewFrustrum`
44 → 45. No function scored lower. Tests: `tests/test_sweep_literals.py`.

## 22. Plan items were cut to 15 functions

`next_target.py --plan nuportal` listed 15 of the item's 22 functions. `collect()` returned
all 22, but the CLI's `--limit 15` default (meant for the whole-queue view) also cut plan
items, without saying so. A child took the 15 as its work list.

`--limit` now defaults to all rows for a plan item and 15 otherwise. Whenever rows are cut,
the output says how many and that `--limit 0` lists them all.

Before → after: nuportal listed 15/22 → 22/22. Tests: `tests/test_next_target.py`
(`PlanListingTests`, three tests that fail before the fix).

## 23. Kickoff queued sweep matches as hand work

`kickoff.py` named `transposeClipPlanes` as the next hand target after the parent's sweep
had already strict-matched it. Gap 15's filter does not apply: it reverifies only functions
the report scores at 90% or more from the built source, and a sweep MATCH that has not been
applied is not in the source yet. Kickoff's plan lane built its queue from the report and
ignored the sweep's MATCH rows.

The plan lane now puts sweep MATCH rows last (they stay queued, so finish accounting still
covers them). `--start` and the plan summary print `sweep already strict-matches N of them
(...); apply, don't hand-match` with the `sweep.py --unit U --apply` command, and the
suggested first `score_fn.py` call names the first function that is not a sweep match.

Before → after: sweep-matched functions handed out as hand work 1 → 0 in the nuportal
case. Test: `tests/test_kickoff.py::test_item_start_puts_sweep_matches_last_and_says_apply`.

## 24. `score_fn.py --apply` skipped silently

`--apply` writes only a strict MATCH. On a mismatch, a compile error or a verifier ERROR it
printed nothing about the skip, so children believed a candidate was in the source. With
`--json`, the code returned before the apply step, so even a MATCH was never applied.

Every skip now prints `not applied to SOURCE:` with the reason: the verdict, score and
exact count, `the candidate does not compile`, or `the verifier returned ERROR`. `--json`
applies a MATCH and writes its apply line to stderr, which keeps stdout valid JSON.
`sweep.py` uses `--apply` or `--json`, never both, so nothing is applied twice.

Before → after: unexplained skips 3 paths → 0; `--json` MATCH applied 0 → 1. Tests:
`tests/test_score_fn_apply.py` (three tests that fail before the fix).

## 25. Handoffs skipped the hypothesis floor

The Sol cohort of 2026-11-13 (`sol-batch-2026-11-13`) handed off the aisys-1 item early:
`AIScriptResolveReferences` 3, `fn_8007E97C` 3, `fn_8007EBD0` 5, `AIScriptProcessorInit` 2,
`fn_8007EFA0` 4, `fn_8007F130` 8 and `AIScriptProcess` 3 distinct compiled candidates, all
under the 25 floor (PR 169). `sprint_finish.py` refused short parks ([gap 12](#12-functions-are-parked-before-25-distinct-hypotheses)),
but `--deferred` had no floor, and the park refusal itself suggested deferring as the way
out. Deferring was the free path.

A `--deferred` function with 1–24 distinct compiled candidates now needs
`--short-handoff REASON` (time, a rate limit, a named blocker), just as a short park
needs `--short-park`. Functions with no compiled candidate on record can still be handed
off untouched. The table shows each deferred function's count, and the finish record keeps
`deferred_hypotheses` and `short_handoff`. The refusal no longer suggests `--deferred`.
The brief's rule 3 says the same.

The compile-error rate behind the 8.7% figure: the two Sol bench runs scored 2,062
attempts with 180 ERRORs (nuanim `t3-64a20ec0-…` 154/1,663; aisys `t3-d590fc6c-…`
26/399). 142 of them are one `variant_search` burst on `NuAnimCurve2CalcVal`, the
declaration-dependency class [gap 18](#18-body-splicing-dropped-declarations-needed-before-the-new-use)
fixed. Without it the rate is 38/1,920 (2.0%). Most of the rest are m2c first drafts that
did not compile. `Action_SetViewDistance`, `Action_SetMinViewHeight`,
`Action_SetMaxViewHeight` and `Action_SetHearDistance` all had ERROR first drafts in that
run; redrafted on current main with [gap 20](#20-drafts-lost-the-units-types)'s typed
drafts, they compile 4/4 (47/105 exact each). No class remains that this pass could fix
in the drafting or scoring tools.

ERROR bench events carried no diagnostic, so classes could only be guessed from
timestamps. `score_fn.py` now records `error`, the first compiler error with its location
and quoted names removed (`X undeclared (first use in this function)`), and
`workflow_metrics.py` reports the five commonest as `error_classes`.

Before → after: aisys-1 short handoffs accepted without a reason 7 → 0 (all 7 refused
until a reason is given); ERROR events with a diagnostic class 0% → every new one. Tests:
`tests/test_sprint_finish.py::test_short_handoffs_need_a_reason`,
`tests/test_workflow_metrics.py::test_error_events_record_a_diagnostic_class`.

## 26. Order search needed hand markers

`order_search.py` searched only blocks a child had marked by hand with
`/* order: begin */` and `/* order: end */` in a scratch file. With `--body`, it wrote
the whole spliced unit to `best.c`, which then had to be cut back to the definition before
`score_fn.py --body` could take it.

`--auto` marks the blocks itself: every run of two or more plain statements (assignments
and calls on lines of their own, at any brace depth; not declarations, control flow or
labels). When the runs together exceed `--limit`, it keeps the one nearest the seed's
first score difference (source position against instruction position, a rough mapping),
and `--climb` searches them all. With `--body` or `--auto`, `best.c` holds the definition
plus the body file's own externs, ready for `score_fn.py NAME SOURCE --body best.c --apply`.

Before → after: markers needed 1+ → 0. On the `Action_SetViewDistance` draft, `--auto`
found 3 runs (8 variants, 0 compile errors), and `best.c` went from the 1,992-line unit to
the 76-line definition, which scores the same through `--body`. Tests: `AutoBlockTests` in
`tests/test_order_search.py`.

## 27. Declaration search permuted assignments

`variant_search.py` accepted an absent separator between a type and a local name.
For `count = NuFileReadInt(fh);` in `NuMtlRender`, regex backtracking invented type
`coun` and variable `t`. The declaration-order dimension then moved that assignment
before `struct nughgtex_s *tex;`, producing C89 parse errors from a compiling seed.

Require whitespace or a pointer star between the type and name. Two regression
tests reproduce the real texture-loader case and reject plain assignments; both
fail with the previous parser. A controlled replay of the same source, compiler,
300-candidate budget and eight jobs gives 96/279 errors (34.4%) before and 0/183
after. The valid search is smaller because it permutes four declarations rather
than four declarations and an assignment. Best score stays 81/93; no match is
attributed to this tool fix. The cohorts and comparison live in
[`config/workflow_metrics.toml`](../config/workflow_metrics.toml).
