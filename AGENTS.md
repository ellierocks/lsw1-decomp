# LSW1 decomp agent guide

Matching decompilation of LEGO Star Wars: The Video Game (2005), GameCube
`GL5E4F`. Game/Nu2 code uses **SN Systems ProDG 3.9.3 (GCC)** via
`tools/prodg_cc.py`; the Dolphin SDK uses Nintendo CodeWarrior **GC/1.2.5n**
from doldecomp/dolsdk2004. CrashWOC's CodeWarrior game code generation does not transfer.

## Task routing

- Start with [strategy](docs/strategy.md) for the current phase; update it when a phase step lands.
- Matching: follow the [quickstart](docs/matching_quickstart.md) for bootstrap,
  target selection, scoring and handoff, and [C matching](docs/code_matching_workflow.md)
  for strict verification and promotion. SDK work uses [SDK import](docs/sdk_import.md#importing-a-unit-quickly).
- Game/Nu2 donor review: follow the [OpenSaga workflow](docs/symbol_donors/opensaga_matching.md#end-to-end-workflow).
  Read `donor_evidence.md` and record `hillclimb.py try --donor-note` once per
  target/donor revision; retain accepted facts, rejected transfers or an explicit
  no-hit/unavailable limit in the PR or plateau handoff.
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
