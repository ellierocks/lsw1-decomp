# LSW1 decomp agent guide

Matching decompilation of LEGO Star Wars: The Video Game (2005), GameCube
`GL5E4F`. The compiler is **SN Systems ProDG (GCC)**, driven by
`tools/prodg_cc.py`. CrashWOC uses CodeWarrior; its code generation does not
transfer to this target.

## Default task

Run the bounded loop in [C matching](docs/code_matching_workflow.md). Choose
one function, establish a compiled baseline, change one source hypothesis,
rebuild and record strict verification evidence with `tools/hillclimb.py`.
Keep the best source. Park a plateau with its remaining mismatch and one concrete
next hypothesis. Naming and donor mining support the selected C target.

A strict function match is **VERIFIED**, not **PROMOTED**. Promotion also needs
unit sections, relocations and linked retail ranges to match without regression.
Never weaken the oracle, change pins or split boundaries to improve a score.
Do not report renamed symbols as matching progress.

## Sources

- [Research](docs/research.md): lookup/rename tools, donor mining and provenance,
  Ghidra and runtime evidence. Check rejected renames before naming a target.
- [Nu2 reference](docs/nu2_engine_reference.md): donor layout sketches and target
  evidence. Confirm types against LSW1 accesses before using them in C.
- [Build configuration](docs/build_config.md): symbols, splits and DTK constraints.
- [Menus](docs/menu_system.md) and [scripts](docs/scripts.md): subsystem evidence.

Current source, headers, `config/GL5E4F/symbols.txt` and target assembly take
precedence over names in generated reports or historical task packs.

## Conventions

- Function names: `Nu{Subsystem}{Verb}` (engine), `Action_*`/`Condition_*` (dispatch).
- Struct tags: `nu{name}_s` (engine), `NU{NAME}_s` (game).
- Fields: `camelCase`; error strings: `str_FunctionName_Description`.
- Four-space C indentation; follow surrounding code.
- No comments in decomp source unless explaining a subtle match constraint.

## Completion

Follow [development workflow](docs/development_workflow.md) for topic branches,
Forgejo PRs, portable checks and CI. Push only to Forgejo; GitHub and Codeberg
are mirrors. Required CI must pass on the current head before merge.

Run `bash build.sh` when changing splits, configuration, compiler wiring,
candidate sources or reports, then inspect `tools/matching_progress.py`.
Retain baseline/best attempt, target addresses, strict verification commands,
promoted bytes and remaining limits in matching PRs. Keep raw experiment logs
and proprietary artifacts local.

`CLAUDE.md` imports this file. Setup and the documentation index are in [README.md](README.md).
