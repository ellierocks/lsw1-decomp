# Code Matching Workflow

This project now has a donor-guided C matching path, but it is not a fully
automatic “drop in a C file and the build will link it” flow yet.

The reliable loop today is:

1. Use donor evidence to choose a candidate C body.
2. Compile it in the project.
3. Compare the emitted instruction stream against the target asm.
4. Only then consider splitting the fallback assembly and promoting the source
   into the normal build path.

## Toolchain: ProDG (SN Systems GCC)

LSW1 GC was built with **ProDG (SN GCC), not Metrowerks.** ProDG `cc1` reproduces
the retail codegen exactly (high-register allocation, `lis/ori` constants,
record-form `mr.` null checks); no MWCC version/flag does. `numath/nurand.c`
matches all five functions under ProDG.

ProDG is the split GCC pipeline, driven by `tools/prodg_cc.py`
(`CPP -> cc1 -> powerpc-eabi-as -> dtk elf fixup`) behind a `prodg` ninja rule.
Opt a module in per-object:

```py
Object(NonMatching, "numath/foo.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg)
```

A unit is only promoted (`Matching`) once its section bytes, relocations and
linked DOL range match retail; `verify_fn.py` alone is not sufficient. See
[nurand_matching.md](nurand_matching.md) for the procedure and
`tools/dol_compare.py`.

`verify_fn.py` compares ProDG objects against the retail target (see
[Verifying a candidate](#verifying-a-candidate)). Remaining small diffs are
ordinary source-idiom tuning, not a toolchain wall.

## Current status

- `dtk dol split` works with the pinned patch built by `tools/build_patched_dtk.sh`
  (`bash build.sh`); older notes that say it panics predate that patch.
- `tools/verify_fn.py` is the strict, relocation-aware check for C candidates.
- `tools/crashwoc_code_queue.py` is the current CrashWOC candidate generator.
- `tools/asm_function_slice.py` is planning-only by default; do not treat
  `--write` as a ready-to-link workflow until the candidate is proven to match.

## Tool roles

| Tool | Role |
|------|------|
| `tools/crashwoc_code_queue.py` | Reads `docs/symbol_donors/nu2_fn_matches.tsv` and a local CrashWOC checkout, then emits one-to-one C candidates. Default is `exact` (byte-identical, mostly trivial stubs); pass `--include-norm` for the substantial normalized candidates. |
| `tools/verify_fn.py` | Compares instruction words and relocations of one function in your built object with the DTK-split retail target (and the retail DOL). This is the main “does this compile like the target?” check; see below. |
| `tools/asm_function_slice.py` | Locates the target in the fallback asm and emits a split plan or generated slice artifacts. It does not change build wiring on its own. |
| `tools/symbols_to_map.py --all` | Generates the Dolphin map with `zz_<address>_` placeholders. It is unrelated to candidate verification. |
| `tools/matching_progress.py` | Reads objdiff progress only. It is not a candidate verifier. |

## Recommended sequence

```sh
python3 tools/crashwoc_code_queue.py /tmp/crashwoc-decomp --include-norm
python3 configure.py
python3 tools/verify_fn.py NuVec4MtxTransformVU0 --unit numath/nu_asm --build
python3 tools/asm_function_slice.py NuVec4MtxTransformVU0
```

Notes:

- Pass `--compiled-symbol` to `verify_fn.py` if the source object emits a
  different symbol name than the target function name.
- The current CrashWOC queue is written to
  `docs/symbol_donors/crashwoc_code_match_queue.tsv` and `.md`.
- The `NuVec4MtxTransformVU0` candidate in `src/numath/nu_asm.c` is currently a
  nonmatching source port candidate, not a linked replacement.

## Verifying a candidate

Prerequisite: `bash build.sh` once (patched DTK, split target objects under
`build/GL5E4F/obj`, retail DOL at `orig/GL5E4F/sys/main.dol`).

```sh
python3 tools/verify_fn.py NuRandInt                       # check an already-built object
python3 tools/verify_fn.py NuVecScale --unit numath/nuvec --build   # rebuild first
python3 tools/verify_fn.py Foo --object work/Foo/source.o  # arbitrary candidate object
python3 tools/verify_fn.py Foo --json                      # machine-readable
```

What is compared, per instruction: the 32-bit word with relocation-patched
fields masked out, plus each relocation's type, symbol and addend. Register,
immediate, opcode and instruction-count differences are all failures;
mnemonic-order similarity is printed for orientation only and never counts.
Branches that leave the function without a relocation are resolved to symbols
before comparing. The target object bytes are also checked against the retail
DOL, and an `.rodata`/`.sdata2` constant referenced through a relocation is
compared with the retail bytes at the target symbol's address (width from the
load/store instruction).

Exit codes: `0` match, `1` mismatch, `2` error (unknown symbol, missing or stale
object, build/tool failure, inconsistent target), `3` code matches but some
references are only mapped by consistency (`--strict-symbols`). Verdicts:

| Verdict | Meaning |
|---------|---------|
| `MATCH` | Code identical; every reference verified by name, same `symbols.txt` address, or data bytes. |
| `MATCH (unverified references)` | Code identical, but some referenced symbols differ by name (for example `fseed` vs `lbl_80407B40`) and could only be mapped consistently. Lines under `references:` say why. |
| `MISMATCH` | Any word, relocation, length or constant differs. |

Staleness: without `--object`, the compiled object must be up to date
according to `ninja -n` (falls back to source mtime); otherwise the tool exits
2 instead of reporting on an old object. `--build` runs ninja and exits 2 with
the compiler output if it fails; `--allow-stale` skips the check.

Limitations (what a `MATCH` does not prove):

- Only the named function's code and the data it references through
  relocations are compared; data layout, symbol placement, section choice
  (mine `.sbss` vs retail `.sdata`), alignment and linker ordering are not.
- Referenced data is compared only for the width of the first load or store
  that uses it (at most 8 bytes); the rest of a table or struct is not checked.
- Two separate literals in mine that retail shares through one symbol are
  reported as a mismatch (the name mapping is strictly one-to-one).
- Exit 0 includes `MATCH (unverified references)`; scripts that must reject
  names mapped only by consistency should pass `--strict-symbols` (exit 3).
- Address-taken data (`addi rX, sym@l`, SDA21 `li`) and bss have no width or
  retail bytes to compare and stay "unverified".
- Debug-only differences and anything after linking (SDA base, final addresses)
  are out of scope; objdiff and a relinked DOL remain the final authority.
- The side-by-side text uses `dtk elf disasm`; if it fails the verdict is
  unaffected and raw words are shown.

Regression tests: `tests/test_verify_fn.py` (synthetic objects everywhere; the
genuine ProDG/retail cases run after `bash build.sh` and are required in the
Forgejo retail CI job).

## Linking a verified function

When a candidate becomes a true match:

1. Add the C file to `configure.py` as a linked object.
2. Add a matching split entry in `config/GL5E4F/splits.txt`.
3. Re-run `python3 configure.py && ninja`.
4. Re-check the object with `tools/verify_fn.py` (expect `MATCH`).

The new source in `src/numath/nu_asm.c` is currently a nonmatching candidate,
so it is present for comparison and iteration, not as a linked replacement yet.
