# Workflow Status

Current verification baseline and known limits, as of 2026-10-29. Forges, CI jobs and
the PR process are described in [development workflow](development_workflow.md) and
[CI and progress reporting](ci_and_decomp_dev.md).

## Toolchain

- The game was built with **ProDG** (SN Systems GCC). `tools/prodg_cc.py` drives it.
  ProDG objects select `compiler="prodg"` and `compiler_version=PRODG_VERSION`
  explicitly, so they do not depend on the linker settings inherited from DTK.
- DTK remains in use for RVZ extraction, patched DOL splitting, ELF fixup and objdiff
  integration. Its inherited MWCC linker settings do not identify the game's compiler.
- Four existing units are ProDG. Two historical MWCC candidates are kept for research.
- `numath/nurand.c` is the first unit promoted to `Matching` and linked into the
  relinked DOL; see [nurand matching](nurand_matching.md). No other candidate has been
  promoted, and the relinked DOL as a whole still differs from retail.

## Verified on 2026-10-29

- The pinned patched DTK builds with Rust stable.
- A DOL extracted from the retail RVZ matches SHA-1
  `95cca08a19224775d1a8d6cc64601fb7d0080981`.
- Splitting produces 22 objects. All six comparison units compile and the report
  generates.
- With objdiff's lenient default relocation mode the report contained 2,032 /
  1,569,532 matched code bytes and 23 / 4,147 matched functions. The report now uses
  `functionRelocDiffs=all`: **1,920 / 1,569,532 matched code bytes**,
  **22 / 4,147 matched functions** (one `nuvec` false match removed) and 36 matched
  data bytes.
- Symbol naming is a separate measure. Its current figures are in the README.

## Checks

`bash build.sh` verifies the input hash, bootstraps the patched splitter, compiles
candidates, generates the comparison report, links `main.dol` and checks every promoted
unit's byte ranges against retail with `tools/dol_compare.py`. It does not produce a
matching relinked game. Requirements: Python 3.10+, Ninja, Rust stable and Cargo, and network access for
the pinned tools.

The portable CI jobs need no proprietary input: compile, unit tests, `configure.py`,
shell syntax and Dolphin map generation. The Forgejo `retail` job decrypts and
hash-checks the DOL, then runs `bash build.sh`. It fails if `ORIG_KEY` is missing.
GitHub's encrypted-DOL report job and map job remain GitHub-only.

Release tooling does not apply: this is not a versioned Rust crate.

## Dependencies

Renovate covers the supported dependency managers, including GitHub Actions. Compiler
and DTK pins are not bumped automatically. Changing them needs deliberate toolchain
verification: rebuild the patched DTK, re-check the retail hash, and compare the
report.
