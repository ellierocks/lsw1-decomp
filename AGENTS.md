# LSW1 decomp agent guide

Matching decompilation of LEGO Star Wars: The Video Game (2005), GameCube
`GL5E4F`. Game and Nu2 code use **SN Systems ProDG 3.9.3 (GCC)**, driven by
`tools/prodg_cc.py`. The linked Dolphin SDK is Nintendo's CodeWarrior
**GC/1.2.5n** build, matched from doldecomp/dolsdk2004. CrashWOC uses
CodeWarrior too; its game code generation does not transfer.

## Default task

Start with the [matching quickstart](docs/matching_quickstart.md): `tools/bootstrap.sh`,
then `tools/next_target.py` to choose a target. Follow the current phase in
[strategy](docs/strategy.md): SDK import first,
then whole-binary splits, then agent draft-and-climb for game code. For SDK units, `tools/sdk_probe.py --pending`
ranks the queue and `tools/sdk_import_unit.py UNIT --apply` imports and proves a unit
([quick import](docs/sdk_import.md#importing-a-unit-quickly)). Claim one
library, TU or function per worktree branch. Search with `tools/score_fn.py`
(fast, no ninja). Record candidates worth keeping with `tools/hillclimb.py try`,
and accept only with `verify_fn.py --build --strict-symbols`
([C matching](docs/code_matching_workflow.md)). Park a plateau with its best
score, its remaining mismatch and one concrete next hypothesis. Naming and
donor mining serve a selected matching target. Consult the
[OpenSaga lane](docs/symbol_donors/opensaga_matching.md) routinely for Nu2
evidence: start with small parser/helpers, reuse confirmed GC header layouts,
and expand only after strict matches.

A strict function match is **VERIFIED**, not **PROMOTED**. Promotion also needs
unit sections, relocations and linked retail ranges to match without regression.
Never weaken the oracle, change pins or split boundaries to improve a score.
Do not report renamed symbols as matching progress.

## Sources

- [Strategy](docs/strategy.md): phases, lanes and task choice. Update it when a
  phase step lands.
- [Quickstart](docs/matching_quickstart.md): commands in order for one matching target.
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
GitHub PRs, portable checks and CI. Push only to GitHub; Codeberg is a
mirror. Required CI must pass on the current head before merge.

Run `bash build.sh` when changing splits, configuration, compiler wiring,
candidate sources or reports, then inspect `tools/matching_progress.py`.
Retain baseline/best attempt, target addresses, strict verification commands,
promoted bytes and remaining limits in matching PRs. Keep raw experiment logs
and proprietary artifacts local.

`CLAUDE.md` imports this file. Setup and the documentation index are in [README.md](README.md).
