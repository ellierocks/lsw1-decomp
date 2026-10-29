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
`numath/nurand.c` is the first unit promoted to `Matching` and linked into the
relinked DOL; see [nurand_matching.md](nurand_matching.md). No other candidate
has been promoted, and the relinked DOL as a whole still differs from retail.

The pinned DTK patch built successfully with Rust stable. A DOL extracted
from the local retail RVZ matches SHA-1
`95cca08a19224775d1a8d6cc64601fb7d0080981`. Splitting produced 22 objects;
compiling all six comparison units and generating the report succeeded.
With objdiff's default lenient relocation mode the report contained 2,032 /
1,569,532 matched code bytes and 23 / 4,147 matched functions; the report now
uses `functionRelocDiffs=all`, giving 1,920 bytes and 22 functions (one false
match in `nuvec` removed) plus 36 matched data bytes. Symbol naming is a separate measure.

## Checks

`bash build.sh` verifies the input hash, bootstraps the patched splitter,
compiles candidates, generates the comparison report, links `main.dol` and
checks every promoted unit's byte ranges against retail with
`tools/dol_compare.py`. It does not claim a completely matching relinked game. Python 3.10+, Ninja, Rust stable/Cargo,
and network access for the pinned tools are needed.

Portable CI runs `python3 -m compileall -q configure.py tools tests`,
`python3 -m unittest discover -s tests`, `python3 configure.py`, shell syntax
checks and Dolphin map generation without proprietary input. The tooling job
passed locally in the Forgejo runner image. Existing GitHub encrypted-DOL
build/report and map artifact jobs remain GitHub-only because their artifact
actions require GitHub. Forgejo ORIG_KEY is provisioned from the verified `orig` password-store entry.
The Forgejo retail job decrypts and hash-checks the DOL, then runs `bash build.sh`
to compile candidates and generate the report. It fails if the key is missing. Release-plz is not applicable: this is not a versioned Rust crate.

Renovate configuration covers supported dependency managers such as Actions;
compiler and DTK pins still require deliberate toolchain verification.
