# Workflow gaps

Review of 2026-11-10 over the session threads of 2026-10-30 to 2026-11-10, PRs #1 to
#108, CI history and the tools. Gaps are ranked by how much matching work they cost
or put at risk. Each has its evidence and its state. Update the state when a gap closes;
delete the row when nothing remains to say.

| # | Gap | State |
|---|---|---|
| 1 | [Stale task packs change scores](#1-stale-task-packs-change-scores) | fixed, #111 |
| 2 | [Unfinished and parked work is invisible across worktrees](#2-unfinished-and-parked-work-is-invisible-across-worktrees) | fixed, #110 |
| 3 | [Transient download failures turn main red](#3-transient-download-failures-turn-main-red) | fixed, #109 |
| 4 | [`tools/sweep.py` is one module that every sprint edits](#4-toolssweeppy-is-one-module-that-every-sprint-edits) | fixed, #109, #114 |
| 5 | [The plan document carries results, and its next lead is not done](#5-the-plan-document-carries-results-and-its-next-lead-is-not-done) | fixed, #113, #116 |
| 6 | [Smaller recurring friction](#6-smaller-recurring-friction) | partly fixed |

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

[Strategy](strategy.md#phase-3-agent-draft-and-climb-for-game-code) says sprint results stay
in the PR, but its "Automatic progress" paragraph is a 50-line history of sweep figures
dated 2026-11-08. The first next lead of the 2026-11-08 handoff, rerunning the full sweep
above 0x100 with all merged repairs, had not been run two days and seven PRs later; the
credits sprint stopped on a rate limit with `fn_8010BA24` at 52/57.

Fixed: #113 reran the full sweep on fresh packs. #116 moved the dated figures to the
[sweep history](phase3_batches.md#sweep-history), left strategy with the method, and added
`tools/kickoff.py`, which prints the current figures and flags a stale sweep from the
commits touching its inputs since it ran.

## 6. Smaller recurring friction

- `sprint_finish.py --matched-from-diff` counted a `static inline` helper (`ForceObjsReady`,
  #98). That case was already fixed; #109 fixed `inline static` and attribute-prefixed
  helpers, and #116 a return type on the line above the name.
- `Text file busy` on the shared `wibo` appeared in six sessions between 2026-10-31 and
  2026-11-04. Downloads have replaced the binary atomically since; no later session hit it.
- Rate limits stopped about ten sessions mid-task. This is outside the repository; short
  scoped threads and the audit in gap 2 limit what a stopped session can strand.
- Code-identical functions fail `--strict-symbols` on string-literal addresses
  (`fn_80076DF0`). The oracle is right; these wait for `.rodata` ownership.
- The git guard blocks commits whose command runs in the main checkout. Use
  `git -C <worktree>`.
- `tools/quick_wins.py` reads one hard-coded split file, `auto_01_8001752C_text.s`, which a
  later split can leave behind (see gap 1). It does not feed scoring, so it was left; point
  it at the current units when next touched.
