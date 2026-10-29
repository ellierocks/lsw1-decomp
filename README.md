# LEGO Star Wars: The Video Game Decompilation

A work-in-progress decompilation of **LEGO Star Wars: The Video Game** for the Nintendo GameCube.

## Target Version

| Field | Value |
|-------|-------|
| Game ID | `GL5E4F` |
| Platform | Nintendo GameCube |
| Region | USA / NTSC |
| Compiler | SN Systems ProDG (GCC), see [code matching workflow](docs/code_matching_workflow.md) |
| DOL SHA-1 | `95cca08a19224775d1a8d6cc64601fb7d0080981` |

## Project Status

Two separate measures. Symbol naming is not matching.

**Matched code** (objdiff report with strict relocation comparison, as of 2026-10-29;
`numath/nurand.c` is the first unit linked from source, see
[nurand matching](docs/nurand_matching.md)):

| Measure | Matched | Total | Progress |
|---------|---------|-------|----------|
| Code bytes | 1,920 | 1,569,532 | 0.12% |
| Functions | 22 | 4,147 | 0.53% |

**Symbol naming** (`config/GL5E4F/symbols.txt`, as of 2026-10-29):

| Section | Named | Total | Progress |
|---------|-------|-------|----------|
| `.text` (functions) | 1,463 | 4,201 | 34.8% |
| `.rodata` | 774 | 8,383 | 9.2% |
| `.bss` | 38 | 733 | 5.2% |

Naming is driven by donor evidence (CrashWOC, the LSW1 and LSW2 Mac builds). Matching
work targets the Nu2 engine's animation and math code first. Live figures:

```sh
python3 tools/progress.py             # naming and matching
python3 tools/matching_progress.py    # matching only, from the objdiff report
```

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
   build/tools/dtk_patched vfs cp '/path/to/game.rvz:sys/main.dol' orig/GL5E4F/sys/main.dol
   ```

   Option B, for project maintainers with the key. The encrypted copy is committed:
   ```sh
   ORIG_KEY='<passphrase>' bash tools/orig_crypt.sh decrypt
   ```

   `build.sh` checks the SHA-1 before building anything, so a wrong file fails early.

3. **Build:**
   ```sh
   bash build.sh
   ```
   This verifies the input, bootstraps the patched splitter, compiles candidate objects,
   generates the objdiff report, links `main.dol` and checks promoted units against
   retail. It does not produce a fully matched, relinked game.

Development happens on Forgejo, and the GitHub repository is a public mirror. See
[workflow status](docs/workflow_status.md) for what is verified and what is not.

## Contributing

Read [development workflow](docs/development_workflow.md) first. It covers branches,
pull requests, the checks to run, CI jobs and merge requirements.

### Naming Symbols

The fastest path to progress. Update `config/GL5E4F/symbols.txt` with descriptive names
in `PascalCase`, for example `NuAnimLoad` instead of `fn_8001A234`.

```sh
python3 tools/ls1_match_plan.py nuanim                 # candidates in a module
python3 tools/ls1_lookup_symbol.py 0x8001E76C          # cross-reference a function
python3 tools/ls1_rename.py --suggest fn_8001E76C      # donor name hints
python3 tools/ls1_rename.py fn_8001E76C NuAnimKeyLerp  # apply and build-verify
```

Refresh donor evidence first if the queues look stale:

```sh
python3 tools/binary_mining_pipeline.py
```

[AGENTS.md](AGENTS.md) has the full symbol recovery loop used in agent sessions.

### Decompiling Functions

C matching is a candidate loop, not a drop-in. A candidate stays out of the build until
it matches.

1. Generate a task pack for a function from `build/GL5E4F/asm/`:
   ```sh
   python3 tools/ls1_task_pack.py fn_8001E76C --out-dir work
   ```
2. Write the candidate in `work/<Fn>/source.c`, or directly in `src/<module>/<file>.c`.
3. Compile it and compare its instructions with the target:
   ```sh
   python3 configure.py && ninja build/GL5E4F/src/<unit>.o
   python3 tools/verify_fn.py <fn_name> --unit <unit>
   ```
4. Once it matches, add the split and object wiring, then rebuild.

The loop, its tool roles and their limits are in
[code matching workflow](docs/code_matching_workflow.md). Open C tasks are in
[`tasks/`](tasks/index.md).

### Code Style

- C/C++: 4-space indent, follow the surrounding code
- Function names: `Nu{Subsystem}{Verb}` (engine), `Action_*` / `Condition_*` (AI dispatch)
- Struct tags: `nu{name}_s` (engine), `NU{NAME}_s` (game)
- Fields: `camelCase`
- No comments in decomp source unless they explain a subtle match constraint

### Before Submitting

The checks run in CI on every PR. Run them locally first:

```sh
python3 -m compileall -q configure.py tools tests
python3 -m unittest discover -s tests
python3 configure.py
bash -n build.sh tools/build_patched_dtk.sh tools/orig_crypt.sh
```

With a retail DOL present, also run `bash build.sh` and confirm matched code has not
regressed. Full list and CI details: [development workflow](docs/development_workflow.md).

### No Copyrighted Code

Never commit original game code or assets. Source is written from analysis of the
binary. Donor projects are evidence, not sources to copy.

## Dolphin Symbol Map

`config/GL5E4F/GL5E4F.map` is generated from `symbols.txt` and is not tracked. Unnamed
functions appear as `zz_<address>_` placeholders, so recovered names stay grouped in
Dolphin's symbol views.

- **From CI:** download the `GL5E4F-dolphin-map` artifact from the latest successful
  **Build Dolphin symbol map** run on GitHub `main`.
- **Locally:**
  ```sh
  python3 tools/symbols_to_map.py --all -o GL5E4F.map
  ```

Load it in Dolphin with **Symbols → Load Symbol Map**.

## Documentation

### Process
- [Development Workflow](docs/development_workflow.md): remotes, branches, PRs, checks, CI and merging
- [CI and Progress Reporting](docs/ci_and_decomp_dev.md): CI jobs, decomp.dev, `ORIG_KEY`
- [Workflow Status](docs/workflow_status.md): verified baseline and known limits

### Build System
- [Build Configuration](docs/build_config.md): `symbols.txt` and `splits.txt` formats, BSS, `.comment` section

### Game Research
- [Engine Analysis](docs/engine_analysis.md): Nu2 engine subsystems, string xrefs, library boundaries
- [External Sources](docs/external_sources.md): PAL GC, PS2/Xbox lineage, PC version analysis
- [Menu System](docs/menu_system.md): debug menu, memory watches, callback tables
- [Game AI](docs/game_ai.md): AI action and condition dispatch tables (`Action_*`, `Condition_*`)
- [Level Scripts](docs/level_script_commands.md): in-world script command grammar
- [Nu2 Engine Reference](docs/nu2_engine_reference.md): Nu2 structs and subsystem layout (from CrashWOC)

### Matching
- [Code Matching Workflow](docs/code_matching_workflow.md): the candidate loop and its tools
- [Matching Notes](docs/matching_notes.md): compiler quirks, register allocation, diff-fixing techniques
- [Struct Typing Analysis](docs/struct_typing_analysis.md): field-offset patterns and inferred struct layouts
- [Module Split Plan](docs/splits_plan.md): proposed `.c` source file boundaries

### Cross-Version Research
- [`research/revisions/`](research/revisions/revision_graph.md): build archaeology, revision graph, string similarity, and leads across GC/PS2/Xbox/PC builds
- [`docs/symbol_donors/`](docs/symbol_donors/): Mac demo symbols, CrashWOC matches, Nu2 body confirmations, rename queues

## Project Structure

```
.
├── build.sh                    # Full build helper: verify, bootstrap DTK, compile, report
├── configure.py                # Project configuration and ninja build generator
├── config/GL5E4F/
│   ├── config.yml              # dtk split configuration
│   ├── symbols.txt             # Symbol definitions (~16,750 entries)
│   ├── splits.txt              # Section split definitions
│   └── build.sha1              # Expected DOL checksum
├── src/                        # Decompiled C/C++ source files (candidates until linked)
├── include/                    # C/C++ header files
├── tasks/                      # Per-function task packs for C decompilation
├── tests/                      # Portable tooling regression tests
├── tools/                      # Build adapters, verification and analysis utilities
├── docs/                       # Research notes, reference documentation and donor evidence
├── research/revisions/         # Build revision archaeology and graph
├── .github/workflows/          # CI definitions (Forgejo runs the same files)
├── orig/                       # Original game files (DOL is gitignored; encrypted copy is tracked)
└── build/                      # Build artifacts (gitignored)
```

## Resources

- **[Discord: GC/Wii Decompilation](https://discord.gg/hKx3FJJgrV)**: join `#dtk` for help
- **[objdiff](https://github.com/encounter/objdiff)**: local diffing tool for matching
- **[decomp.me](https://decomp.me)**: collaborate on function matches
- **[decomp.dev](https://decomp.dev)**: decompilation progress hub and API

## License

Released under CC0 1.0 Universal (see [LICENSE](LICENSE)). The repository contains no game
assets or copyrighted code.
