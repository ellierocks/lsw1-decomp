# Workflow restart (2026-10-29)

Forgejo (`ellie/lsw1-decomp`) is primary. GitHub remains at
https://github.com/ellierocks/lsw1-decomp, public, with a Forgejo push mirror;
Codeberg is a private push mirror. Before migration, GitHub advertised only
`main` at `5e0446da03d00846ac8d1bfbdd38d48f8f5306b5`. That history was pushed
unchanged before configuring mirrors. No source, symbol or research files
were removed.

## Toolchain

Keep DTK for RVZ extraction, patched DOL splitting, ELF fixup and objdiff
integration. Its inherited MW linker settings do not identify the game's
compiler. ProDG objects now select `compiler="prodg"` and
`compiler_version=PRODG_VERSION` explicitly. The four existing ProDG units
remain ProDG; the two historical MWCC candidates remain available for research.
No candidate has been promoted to matching or wired into a replacement DOL.

The pinned DTK patch built successfully with Rust stable. A DOL extracted
from the local retail RVZ matches SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Splitting produced 22 objects;
compiling all six comparison units and generating the report succeeded.
The report contains 2,032 / 1,569,532 matched code bytes and 23 / 4,147
matched functions. Symbol naming is a separate measure.

## Checks

`bash build.sh` verifies the input hash, bootstraps the patched splitter,
compiles candidates and generates the comparison report. It does not claim
a completely matching relinked game. Python 3.10+, Ninja, Rust stable/Cargo,
and network access for the pinned tools are needed.

Portable CI runs `python3 -m compileall -q configure.py tools tests`,
`python3 -m unittest discover -s tests`, `python3 configure.py`, shell syntax
checks and Dolphin map generation without proprietary input. The tooling job
passed locally in the Forgejo runner image. Existing GitHub encrypted-DOL
build/report and map artifact jobs remain GitHub-only because their artifact
actions require GitHub. Forgejo does not yet have the existing ORIG_KEY secret;
its tooling check does not certify a retail build. Full comparison was verified
locally. Release-plz is not applicable: this is not a versioned Rust crate.

Renovate configuration covers supported dependency managers such as Actions;
compiler and DTK pins still require deliberate toolchain verification.
