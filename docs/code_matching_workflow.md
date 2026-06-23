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
Object(NonMatching, "numath/foo.c", mw_version=PRODG_VERSION, cflags=cflags_prodg)
```

`verify_fn.py` works unchanged on ProDG objects. Remaining 1–2 instruction diffs
are now ordinary source-idiom tuning, not a toolchain wall.

## Current status

- `dtk dol split` is still not the thing to depend on for this repo.
- `tools/verify_fn.py` is the current scoring tool for C candidates.
- `tools/crashwoc_code_queue.py` is the current CrashWOC candidate generator.
- `tools/asm_function_slice.py` is planning-only by default; do not treat
  `--write` as a ready-to-link workflow until the candidate is proven to match.

## Tool roles

| Tool | Role |
|------|------|
| `tools/crashwoc_code_queue.py` | Reads `docs/symbol_donors/nu2_fn_matches.tsv` and a local CrashWOC checkout, then emits one-to-one C candidates. Default is `exact` (byte-identical, mostly trivial stubs); pass `--include-norm` for the substantial normalized candidates. |
| `tools/verify_fn.py` | Disassembles the built object and target asm, then compares mnemonic order and counts. This is the main “does this compile like the target?” check. |
| `tools/asm_function_slice.py` | Locates the target in the fallback asm and emits a split plan or generated slice artifacts. It does not change build wiring on its own. |
| `tools/symbols_to_map.py --all` | Generates the Dolphin map with `zz_<address>_` placeholders. It is unrelated to candidate verification. |
| `tools/matching_progress.py` | Reads objdiff progress only. It is not a candidate verifier. |

## Recommended sequence

```sh
python3 tools/crashwoc_code_queue.py /tmp/crashwoc-decomp --include-norm
python3 configure.py
ninja build/GL5E4F/src/numath/nu_asm.o
python3 tools/verify_fn.py NuVec4MtxTransformVU0 --unit numath/nu_asm
python3 tools/asm_function_slice.py NuVec4MtxTransformVU0
```

Notes:

- Pass `--compiled-symbol` to `verify_fn.py` if the source object emits a
  different symbol name than the target function name.
- `verify_fn.py` compares instruction streams directly from the built object and
  the checked-in asm dump, so it works even when the objdiff split flow is
  unavailable.
- The current CrashWOC queue is written to
  `docs/symbol_donors/crashwoc_code_match_queue.tsv` and `.md`.
- The `NuVec4MtxTransformVU0` candidate in `src/numath/nu_asm.c` is currently a
  nonmatching source port candidate, not a linked replacement.

## Linking a verified function

When a candidate becomes a true match:

1. Add the C file to `configure.py` as a linked object.
2. Add a matching split entry in `config/GL5E4F/splits.txt`.
3. Re-run `python3 configure.py && ninja`.
4. Re-check the object with `tools/verify_fn.py`.

The new source in `src/numath/nu_asm.c` is currently a nonmatching candidate,
so it is present for comparison and iteration, not as a linked replacement yet.
