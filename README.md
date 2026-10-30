# LEGO Star Wars: The Video Game Decompilation

A work-in-progress decompilation of **LEGO Star Wars: The Video Game** for the Nintendo GameCube.

## Target Version

| Field | Value |
|-------|-------|
| Game ID | `GL5E4F` |
| Platform | Nintendo GameCube |
| Region | USA / NTSC |
| Compiler | SN Systems ProDG (GCC) for game code; CodeWarrior GC/1.2.5n for the Dolphin SDK. See [code matching workflow](docs/code_matching_workflow.md) |
| DOL SHA-1 | `95cca08a19224775d1a8d6cc64601fb7d0080981` |

## Progress

<!-- progress:start -->
**Matched code** (objdiff report with strict relocation comparison):

| Measure | Matched | Total | Progress |
|---------|---------|-------|----------|
| Code bytes | 80,244 | 1,569,612 | 5.11% |
| Functions | 397 | 4,150 | 9.57% |

**Symbol naming** (`config/GL5E4F/symbols.txt`):

| Section | Named | Total | Progress |
|---------|-------|-------|----------|
| `.text` (functions) | 1,499 | 4,205 | 35.6% |
| `.rodata` | 775 | 8,384 | 9.2% |
| `.bss` | 51 | 741 | 6.9% |
<!-- progress:end -->

CI refreshes these figures after successful GitHub builds on `main`.

```sh
python3 tools/matching_progress.py    # matched code from the local objdiff report
python3 tools/progress.py             # symbol naming and matching
```

Symbol naming and matched code are separate measures. Comparison candidates
are compiled without replacing retail code; promoted units are linked from C
and checked against retail ranges. The whole relinked DOL still differs from
retail. [C matching](docs/code_matching_workflow.md) explains the evidence and
remaining link differences.

## Prerequisites

- **Python 3.10+**, Rust stable and Cargo (to build the patched DTK)
- **Ninja**: `pip install ninja` or your system package manager
- **Network access** on first build, for the pinned DTK source and tools
- **Your own disc image** of LEGO Star Wars: The Video Game (GameCube, NTSC-U), or the project's encrypted DOL with its key

On Linux and macOS, [wibo](https://github.com/decompals/wibo) (a minimal 32-bit Windows binary wrapper) is downloaded automatically.

## Quick Start

1. **Clone the repository:**
   ```sh
   git clone https://github.com/ellierocks/lsw1-decomp.git
   cd lsw1-decomp
   ```

2. **Provide the retail DOL** at `orig/GL5E4F/sys/main.dol`. It is never committed.

   Option A, from your disc image. Build the patched DTK first, then extract:
   ```sh
   bash tools/build_patched_dtk.sh
   mkdir -p orig/GL5E4F/sys
   build/tools/dtk_patched vfs cp '/path/to/game.rvz:sys/main.dol' orig/GL5E4F/sys/main.dol
   ```

   Option B, for maintainers with `ORIG_KEY` supplied securely in the environment.
   The encrypted copy is committed:
   ```sh
   bash tools/orig_crypt.sh decrypt
   ```

   `build.sh` checks the SHA-1 before building anything, so a wrong file fails early.

3. **Build:**
   ```sh
   bash build.sh
   ```
   This verifies the input, bootstraps the patched splitter, compiles candidate objects,
   generates the objdiff report, links `main.dol` and checks promoted units against
   retail. It does not produce a fully matched, relinked game.

Development happens on GitHub, and Codeberg is a read-only mirror. See
[development workflow](docs/development_workflow.md) for checks and PRs.

## Working on the decomp

1. Follow [C matching](docs/code_matching_workflow.md): choose one function,
   compile a ProDG baseline, test one source hypothesis at a time, and verify
   object and linked ranges before promotion.
2. Use [research](docs/research.md) when types, callees or boundaries are unclear.
   Generated task packs and donor queues are leads; current assembly and symbols
   are the authority.
3. Submit a topic-branch PR with verification evidence and the checks in
   [development workflow](docs/development_workflow.md).

Never commit original game code or assets. Source is written from analysis of
the binary; donor projects supply evidence.

## Documentation

| Guide | Use |
|---|---|
| [Strategy](docs/strategy.md) | Phases and lanes: SDK import, whole-binary splits, agent draft-and-climb |
| [Matching quickstart](docs/matching_quickstart.md) | Bootstrap, pick a target, score, record and verify in six steps |
| [C matching](docs/code_matching_workflow.md) | Toolchain, bounded experiments, strict verification, promotion and link blockers |
| [Compiler evaluation](docs/compiler_evaluation.md) | ProDG audit, RTL diagnostics and SN GCC v1.79 results |
| [Research](docs/research.md) | Symbol recovery, donor mining, Ghidra, RAM dumps and evidence provenance |
| [Build configuration](docs/build_config.md) | Symbols, splits, compiler wiring and target-specific DTK constraints |
| [Development workflow](docs/development_workflow.md) | Checks, PRs, CI, progress artifacts and secrets maintenance |
| [Nu2 reference](docs/nu2_engine_reference.md) | Layout sketches, SDK evidence, file calls and camera state |
| [Menus and shop](docs/menu_system.md) | Callback/descriptor layouts, runtime fields and shop transitions |
| [Script dispatch](docs/scripts.md) | AI parser anchors and AI/level command tables |

Generated evidence remains in [symbol_donors](docs/symbol_donors/README.md),
[static call exports](docs/recomp_exports/call_graph_summary.md),
[archived task packs](docs/history/task_packs/index.md) and
[cross-version research](research/revisions/revision_graph.md).
Their names can be stale; use addresses to resolve current symbols.

## Dolphin map

```sh
mkdir -p artifacts
python3 tools/symbols_to_map.py --all --output artifacts/GL5E4F.map
```

Load the map through Dolphin's **Symbols → Load
Symbol Map**. Unnamed functions use `zz_<address>_` placeholders. The GitHub
map workflow also publishes the `GL5E4F-dolphin-map` artifact.

## Layout

`src/` and `include/` hold candidate C and types; `config/GL5E4F/` owns symbols
and splits; `tools/` and `tests/` own tooling and regressions. `orig/` holds local
retail/donor inputs (plaintext is ignored), `build/` holds generated objects and
reports, and ignored `work/` holds matching experiments.

## License

Released under CC0 1.0 Universal; see [LICENSE](LICENSE).
