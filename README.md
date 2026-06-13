# LEGO Star Wars: The Video Game Decompilation

A work-in-progress decompilation of **LEGO Star Wars: The Video Game** for the Nintendo GameCube.

## Target Version

- **Game ID:** `GL5E4F`
- **Platform:** Nintendo GameCube
- **Region:** USA / NTSC
- **DOL SHA-1:** `95cca08a19224775d1a8d6cc64601fb7d0080981`

## Prerequisites

- **Python 3.8+** — Install from [python.org](https://www.python.org/downloads/) (Windows), or use your system package manager (macOS/Linux).
- **Ninja** build system — Install via `pip install ninja` (Windows) or `brew install ninja` (macOS), or your Linux package manager.
- **A clean disc image** of LEGO Star Wars: The Video Game (GameCube, NTSC-U).

On Linux and macOS, [wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

On Windows, native tooling is recommended (no WSL/msys2 needed). [objdiff](https://github.com/encounter/objdiff) cannot get filesystem notifications under WSL.

## Quick Start

1. **Clone the repository:**
   ```sh
   git clone https://github.com/yourusername/lsw1-decomp.git
   cd lsw1-decomp
   ```

2. **Place your disc image:**
   Copy your clean disc image to `orig/GL5E4F/`. The build system will extract the DOL automatically.

3. **Run the build script:**
   ```sh
   ./build.sh
   ```
   This handles the full build process including tool downloads and a workaround for a known decomp-toolkit issue. The first build takes several minutes.

   **Manual build (alternative):**
   ```sh
   python3 configure.py
   build/tools/dtk dol split config/GL5E4F/config.yml build/GL5E4F
   python3 tools/fix_symbols.py          # Fix duplicate symbols if dtk panics
   python3 tools/add_documented_symbols.py  # Apply names from research docs
   python3 tools/batch_rename_strings.py    # Apply names from DOL string extraction
   ninja
   ```

## Known Issues

**dtk split panic:** The `dtk dol split` command may panic with a regex error when processing certain symbol configurations. The `build.sh` script handles this automatically by:
1. Running the split with a timeout
2. Fixing duplicate symbols in `symbols.txt` via `tools/fix_symbols.py`
3. Creating a minimal `config.json` if needed

If you encounter build issues:
```sh
rm -rf build/
./build.sh
```

## Project Status

The project is in early stages with:
- **16,713 symbols** identified
- **4,138 functions** detected
- **1.57 MB of code** across 8 translation units
- **Baseline build** established

All code is currently in auto-generated assembly. The next phase is to begin decompiling functions into C/C++ source files.

Track progress with:
```sh
ninja progress
```
Or check `build/GL5E4F/report.json` for detailed metrics.

## Documentation

### Build & Configuration
- [Build Configuration Reference](docs/build_config.md) - symbols.txt/splits.txt formats, common BSS, and CodeWarrior `.comment` section

### Game Research
- [Menu System](docs/menu_system.md) - Debug menu/LEGO Options, memory watches, menu callback tables, and runtime stack
- [Engine Analysis](docs/engine_analysis.md) - Library boundaries, Nu2 engine subsystems, string xrefs, AI script parser, and external references
- [External Sources](docs/external_sources.md) - PAL GC, LSW2 PS2 prototype, TCS Wii prototype, and PC version analysis

### Reference
- `docs/reference/CoolsWatches260522.dmw` - Dolphin Memory Watch file

## Contributing

### How to Contribute

**Decompiling Functions:** Pick a function from `build/GL5E4F/asm/` (start with small ones!), create a `.c` or `.cpp` file in `src/`, write equivalent C/C++ code, update `configure.py` to use your source file, and verify with `ninja`. Use [objdiff](https://github.com/encounter/objdiff) to compare your output with the original.

**Naming Symbols:** Update names in `config/GL5E4F/symbols.txt` using descriptive PascalCase (`Player_Init` instead of `fn_800034A0`). See the [Build Configuration Reference](docs/build_config.md) for the symbol format.

**Common commands:**
```sh
./build.sh                           # Full build with workarounds
python3 configure.py                 # Regenerate build files
ninja                                # Build the project
ninja progress                       # Show decompilation progress
python3 tools/fix_symbols.py         # Fix duplicate symbols after dtk split
python3 tools/add_documented_symbols.py  # Apply names from research docs
```

### Code Style
- C/C++: 4-space indent, follow existing style, meaningful variable names
- Assembly: Keep auto-generated asm files unchanged in `build/GL5E4F/asm/`
- Function names: `PascalCase` (e.g., `Player_Init`, `Menu_Update`)
- Variable names: `camelCase` or `snake_case` (e.g., `playerHealth`, `g_currentMenu`)

### Before Submitting
1. Run `ninja` — should build without errors
2. Run `ninja progress` — verify progress hasn't regressed
3. If you added C code, verify it matches the original assembly with objdiff

### Important Notes
- **No copyrighted code:** Never commit original game code or assets
- **Clean room only:** All code must be written from scratch based on analysis
- **Attribution:** If you use information from other projects, credit them

## Resources

- **[Discord: GC/Wii Decompilation](https://discord.gg/hKx3FJJgrV)** — Join `#dtk` for help
- **[objdiff](https://github.com/encounter/objdiff)** — Local diffing tool for matching
- **[decomp.me](https://decomp.me)** — Collaborate on function matches
- **[decomp.dev](https://decomp.dev)** — Decompilation progress hub and API

## Project Structure

```
.
├── build.sh                    # Build helper script (handles dtk workarounds)
├── configure.py                # Project configuration and build generator
├── config/GL5E4F/              # Version-specific configuration
│   ├── config.yml              # Main configuration file
│   ├── symbols.txt             # Symbol definitions (16,713 symbols)
│   ├── splits.txt              # Section split definitions
│   └── build.sha1              # Expected checksums
├── build/                      # Build artifacts (gitignored)
├── orig/                       # Original game files (gitignored)
├── docs/                       # Documentation and research notes
│   ├── build_config.md         # Build system reference
│   ├── menu_system.md          # Menu system analysis
│   ├── engine_analysis.md      # Engine internals analysis
│   ├── external_sources.md     # Cross-version source analysis
│   └── reference/              # Reference files (memory watches, etc.)
├── include/                    # C/C++ header files (to be created)
├── src/                        # C/C++ source files (to be created)
└── tools/                      # Build scripts and utilities
    ├── fix_symbols.py          # Symbol deduplication helper
    ├── add_documented_symbols.py   # Apply names from research docs
    ├── batch_rename_strings.py     # Apply names from DOL string extraction
    ├── project.py              # Build system utilities
    └── ninja_syntax.py         # Ninja file generation
```

## License

This repository does not contain any game assets or copyrighted code. It is a clean-room implementation that requires the original game to build.

The documentation and build scripts are licensed under CC0 1.0 Universal.
