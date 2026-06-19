# LEGO Star Wars: The Video Game Decompilation

A work-in-progress decompilation of **LEGO Star Wars: The Video Game** for the Nintendo GameCube.

## Target Version

| Field | Value |
|-------|-------|
| Game ID | `GL5E4F` |
| Platform | Nintendo GameCube |
| Region | USA / NTSC |
| DOL SHA-1 | `95cca08a19224775d1a8d6cc64601fb7d0080981` |

## Project Status

| Section | Named | Total | Progress |
|---------|-------|-------|----------|
| `.text` (functions) | 1,053 | 4,197 | 25.1% |
| `.rodata` | 774 | 8,383 | 9.2% |
| `.bss` | 11 | 733 | 1.5% |

All code is currently in auto-generated assembly stubs. Active work is focused on function naming via donor binary analysis (CrashWOC, LSW1 Mac demo, LSW2 Mac) and writing matched C source for the Nu2 animation subsystem.

Track live progress:
```sh
python3 tools/progress.py
```

## Prerequisites

- **Python 3.8+**
- **Ninja** build system — `pip install ninja` or system package manager
- **A clean disc image** of LEGO Star Wars: The Video Game (GameCube, NTSC-U)

On Linux and macOS, [wibo](https://github.com/decompals/wibo) (a minimal 32-bit Windows binary wrapper) is downloaded automatically.

## Quick Start

1. **Clone the repository:**
   ```sh
   git clone https://github.com/yourusername/lsw1-decomp.git
   cd lsw1-decomp
   ```

2. **Place your disc image** in `orig/GL5E4F/`.

3. **Build:**
   ```sh
   ./build.sh
   ```
   Handles tool download, `dtk dol split`, symbol fixes, and the initial ninja build.

> **Known issue:** `dtk dol split` may panic with a regex error on certain symbol configurations. `build.sh` handles this automatically. If the build breaks, run `rm -rf build/ && ./build.sh`.

## Contributing

### Naming Symbols

The fastest path to progress. Update `config/GL5E4F/symbols.txt` with descriptive names (use `PascalCase` — e.g. `NuAnimLoad` instead of `fn_8001A234`).

The AI-assisted workflow:

```sh
python tools/ls1_match_plan.py nuanim          # candidates in a module
python tools/ls1_lookup_symbol.py 0x8001E76C   # cross-ref a function
python tools/ls1_rename.py --suggest fn_8001E76C  # check Mac demo for name hints
python tools/ls1_rename.py fn_8001E76C NuAnimKeyLerp  # apply + verify build
```

Refresh donor evidence first if queues look stale:
```sh
python tools/binary_mining_pipeline.py
```

See [AGENTS.md](AGENTS.md) for the full symbol recovery workflow used in active sessions.

### Decompiling Functions

1. Pick a function from `build/GL5E4F/asm/` (start with small ones — under `0x60` bytes).
2. Generate a task pack:
   ```sh
   python tools/ls1_task_pack.py fn_8001E76C
   ```
3. Write matching C in `src/<module>/<file>.c`.
4. Register it in `configure.py` and rebuild:
   ```sh
   python3 configure.py && ninja
   ```
5. Verify match with [objdiff](https://github.com/encounter/objdiff).

Open C tasks are in [`tasks/`](tasks/index.md).

### Code Style

- C/C++: 4-space indent, follow existing style
- Function names: `Nu{Subsystem}{Verb}` (engine), `Action_*` / `Condition_*` (AI dispatch)
- Struct tags: `nu{name}_s` (engine), `NU{NAME}_s` (game)
- Fields: `camelCase`

### Before Submitting

1. `ninja` — builds without errors
2. `python3 tools/progress.py` — progress hasn't regressed
3. If you added C code, verify with objdiff

### No Copyrighted Code

Never commit original game code or assets. All source must be written from scratch based on analysis.

## Documentation

### Build System
- [Build Configuration](docs/build_config.md) — `symbols.txt` / `splits.txt` formats, BSS, CodeWarrior `.comment` section

### Game Research
- [Engine Analysis](docs/engine_analysis.md) — Nu2 engine subsystems, string xrefs, library boundaries
- [External Sources](docs/external_sources.md) — PAL GC, PS2/Xbox lineage, PC version analysis
- [Menu System](docs/menu_system.md) — Debug menu, memory watches, callback tables
- [Game AI](docs/game_ai.md) — AI action/condition dispatch tables (`Action_*`, `Condition_*`)
- [Level Scripts](docs/level_script_commands.md) — In-world script command grammar
- [Nu2 Engine Reference](docs/nu2_engine_reference.md) — Nu2 structs and subsystem layout (from CrashWOC)

### Matching
- [Matching Notes](docs/matching_notes.md) — Compiler quirks, register allocation, diff-fixing techniques
- [Struct Typing Analysis](docs/struct_typing_analysis.md) — Field-offset patterns and inferred struct layouts
- [Module Split Plan](docs/splits_plan.md) — Proposed `.c` source file boundaries

### Cross-Version Research
- [`research/revisions/`](research/revisions/revision_graph.md) — Build archaeology: revision graph, string similarity, and leads across GC/PS2/Xbox/PC builds
- [`docs/symbol_donors/`](docs/symbol_donors/) — Mac demo symbols, CrashWOC matches, Nu2 body confirmations, rename queues

## Project Structure

```
.
├── build.sh                    # Full build helper (handles dtk workarounds)
├── configure.py                # Project configuration and ninja build generator
├── config/GL5E4F/
│   ├── config.yml              # dtk split configuration
│   ├── symbols.txt             # Symbol definitions (~16,800 symbols)
│   ├── splits.txt              # Section split definitions
│   └── build.sha1              # Expected DOL checksum
├── src/                        # Decompiled C/C++ source files
├── include/                    # C/C++ header files
├── tasks/                      # Per-function task packs for C decompilation
├── docs/                       # Research notes and reference documentation
│   └── symbol_donors/          # Rename queues, match TSVs, donor analysis
├── research/
│   └── revisions/              # Build revision archaeology and graph
├── orig/                       # Original game files (gitignored)
├── build/                      # Build artifacts (gitignored)
└── tools/                      # Build scripts and analysis utilities
```

## Resources

- **[Discord: GC/Wii Decompilation](https://discord.gg/hKx3FJJgrV)** — Join `#dtk` for help
- **[objdiff](https://github.com/encounter/objdiff)** — Local diffing tool for matching
- **[decomp.me](https://decomp.me)** — Collaborate on function matches
- **[decomp.dev](https://decomp.dev)** — Decompilation progress hub and API

## License

This repository contains no game assets or copyrighted code. Documentation and build scripts are licensed under CC0 1.0 Universal.
