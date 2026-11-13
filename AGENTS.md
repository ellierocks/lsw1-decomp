# LSW1 decomp agent guide

Matching decompilation of LEGO Star Wars: The Video Game (2005), GameCube
`GL5E4F`. Game/Nu2 code uses **SN Systems ProDG 3.9.3 (GCC)** via
`tools/prodg_cc.py`; the Dolphin SDK uses Nintendo CodeWarrior **GC/1.2.5n**
from doldecomp/dolsdk2004. CrashWOC's CodeWarrior game code generation does not transfer.

## Task routing

- Run `python3 tools/kickoff.py` first: it reports checkout, sweep and sibling-worktree state
  (`worktree_cleanup.py --audit` has the detail) and names the plan's next unclaimed item;
  `--start --item ID` claims it and records the baseline.
- **Work the plan's next item** ([strategy](docs/strategy.md), [`config/plan.toml`](config/plan.toml)).
  Go off plan only when Ellie asks, when a tooling fix unlocks more plan bytes than the item,
  or when Dolphin runtime evidence is asked for or needed to settle an ambiguity; state the
  off-plan choice and its reason in the PR. Revise the plan file when `tools/plan.py` says so.
  [Workflow gaps](docs/workflow_gaps.md) ranks the known tooling and process gaps.
- **Improve the workflow as you go.** Fix drafting, m2c, sweep and search tools when
  that unlocks matches more effectively than hand-editing each function. Preserve
  reproducible patches, test the fix and rerun it on affected targets to collect the
  wins. See [workflow improvement](docs/sprint_workflow.md#improve-the-workflow-as-you-go):
  pick fixes by the [workflow metrics](docs/workflow_metrics.md) they move and record
  each one's before → after number. Children follow the
  [matching brief](docs/sprint_workflow.md#matching-brief).
- [Sprint tools](docs/sprint_workflow.md) cover target selection, ownership detection,
  queue accounting and verification. Choose scope and search effort for the task;
  T3 owns orchestration and continuation.
- Matching: follow the [quickstart](docs/matching_quickstart.md) for bootstrap,
  target selection, scoring and handoff, and [C matching](docs/code_matching_workflow.md)
  for strict verification and promotion. SDK work uses [SDK import](docs/sdk_import.md#importing-a-unit-quickly).
- Game/Nu2 donor review: follow the [OpenSaga workflow](docs/symbol_donors/opensaga_matching.md#end-to-end-workflow).
  Read `donor_evidence.md` and record `hillclimb.py try --donor-note` once per
  target/donor revision; retain accepted facts, rejected transfers or an explicit
  no-hit/unavailable limit in the PR or plateau handoff. With no name hit, `hillclimb.py`
  records the no-hit limit itself.
- Binary evidence: [Ghidra and REA](docs/research.md#ghidra-and-rea) provide decompiles,
  callers and cross-references. The Japanese PC 1.0 exe has readable x86 bodies for
  paired GC functions; task packs list the pair ([PC builds](docs/research.md#pc-builds)).
  Confirm proposed types and names against target
  assembly; donor output and generated C are evidence, not the acceptance oracle.
- Runtime evidence: when a layout, caller or value stays ambiguous statically, check
  `python3 tools/dolphin.py status` and use the running game, or `dolphin.py launch` one
  on the desktop if none is running; `dolphin.py screenshot` shows the screen. Reach a level with
  `dolphin.py warp` once the user allows driving this session; otherwise ask for the
  state or Gecko code per [Dolphin runtime](docs/dolphin_runtime.md#asking-the-user).
  Dumps and traces stay local; unattended runs record a "Dolphin request" instead.
- Gameplay context: Ellie has speedrun LSW1 for 8 years and is the game's leading
  expert. Before guessing how something works in game (level flow, characters,
  abilities, enemies, objects, triggers, menus, glitches), ask her a short,
  self-contained question; a delegated or unattended run records it as an
  "Ellie question" in its PR or handoff and parks the dependent name or type.
  Engine internals nobody plays around (particle system, renderer, memory layout)
  are for static and runtime evidence instead. Her answers guide names and types;
  target assembly still decides them.
- Naming and research: [research](docs/research.md); check rejected renames first.
  [Nu2](docs/nu2_engine_reference.md), [menus](docs/menu_system.md) and
  [scripts](docs/scripts.md) hold subsystem evidence. Confirm donor types against LSW1 accesses.
- Configuration/splits: [build configuration](docs/build_config.md).
  Setup and the documentation index: [README](README.md).
- Checks and completion: [development workflow](docs/development_workflow.md#checks).
  Run `bash build.sh` for splits, configuration, compiler wiring, candidate sources
  or reports, then `python3 tools/matching_progress.py`. Docs-only work needs link,
  content-preservation and `git diff --check` validation; required current-head CI
  still gates merge. Retain the [matching evidence](docs/development_workflow.md#matching-evidence) in PRs.

## Invariants

A strict function match is **VERIFIED**, not **PROMOTED**. Promotion also needs
unit sections, relocations and linked retail ranges to match without regression.
Never weaken the oracle, change pins or split boundaries to improve a score.
Do not report renamed symbols as matching progress.

Current source, headers, `config/GL5E4F/symbols.txt` and target assembly take
precedence over names in generated reports or historical task packs.
Keep raw experiment logs and proprietary artifacts local.

Coding conventions live in [C matching](docs/code_matching_workflow.md#coding-conventions).
`CLAUDE.md` imports this file.
