# LSW1 Decomp Agent Guide

Matching decompilation of LEGO Star Wars: The Video Game (2005) for GameCube.
Retail target: `GL5E4F`. Compiler: Metrowerks CodeWarrior for GameCube
(bz2 .exports, `DTK_FIXED=1`).

## Quick start

```sh
bash build.sh                                    # full build
ninja build/GL5E4F/report.json                   # refresh progress

python tools/ls1_match_plan.py nuanim            # see unnamed candidates
python tools/ls1_lookup_symbol.py 0x8001E76C     # cross-reference
python tools/ls1_rename.py --suggest fn_8001E76C # check Mac for name ideas
python tools/ls1_rename.py fn_8001E76C NuAnimKeyLerp  # apply + verify
python tools/ls1_task_pack.py NuAnimKeyLerp      # generate task pack for C work
python tools/matching_progress.py                # show current code matching progress
bash build.sh                                    # final check
```

## Symbol sources (priority order)

1. **CrashWOC decomp** (same Nu2 engine) — `docs/nu2_engine_reference.md`,
   `docs/symbol_donors/crashwoc_nu2_comparison.md`
2. **LSW1 Mac Demo** (closest lineage) — `docs/symbol_donors/mac_lsw1_demo_symbols.tsv`
3. **LSW2 Mac** (shared Nu2 engine) — `docs/symbol_donors/mac_lsw2_symbols.tsv`
4. **Struct offset analysis** — match struct field access patterns against
   known Nu2 structs in `src/` headers

## Tooling

| Command | What it does |
|---------|-------------|
| `ls1_match_plan.py <module>` | Shows remaining unnamed functions with struct evidence, Mac hints. Use `--show-all` or `--top N` |
| `ls1_lookup_symbol.py <name-or-addr>` | Cross-references a symbol across GC symbols.txt, Mac TSVs (with demangled signatures), call graph, and neighbor functions |
| `ls1_rename.py <old> <new>` | Applies a rename and verifies with build. Use `--check` to preview, `--suggest <fn>` to search Mac for matching names |
| `ls1_task_pack.py <name-or-addr>` | Generates `tasks/<Function>/` with `prompt.md`, `context.md`, `asm.s`, `related_symbols.md`, `verify.sh` for LLM-based decomp. Use `--out-dir` to set output root. Batch: `ls1_task_pack.py nuanim --top 3` |
| `matching_progress.py [--report] [--json]` | Shows objdiff code matching progress only, without symbol naming stats |

## Matching workflow (symbol recovery)

1. `python tools/ls1_match_plan.py <module>` — see candidates
2. `python tools/ls1_lookup_symbol.py <fn_addr>` — verify struct types in Mac
3. `python tools/ls1_rename.py --suggest <fn_name>` — check Mac for name hints
4. `python tools/ls1_rename.py <old> <new>` — apply and build-verify
5. `bash build.sh` — final check
6. Commit after each module phase

## C matching workflow

1. `python tools/ls1_task_pack.py <fn_name> --out-dir work` — generate task pack
2. Write matching C in `work/<Fn>/source.c` (or inline into `src/<module>/<file>.c`)
3. Add the object to `configure.py` and run `python3 configure.py && ninja`
4. Check match: `ninja build/GL5E4F/report.json` or `objdiff-cli`

## Module address ranges

| Module | Start | End | Est. size |
|--------|-------|-----|-----------|
| nucore/file | 0x800034A0 | 0x80006000 | ~11 KB |
| nucore/numem | 0x80006F74 | 0x80007468 | ~1.2 KB |
| nucore/error | 0x80007468 | 0x80008000 | ~3 KB |
| numath | 0x80008000 | 0x80012000 | ~40 KB |
| nu3dx/anim | 0x80016A00 | 0x80024000 | ~55 KB |
| nu3dx/render | 0x80024000 | 0x8005C000 | ~224 KB |
| nu3dx/scene | 0x8005C000 | 0x80090000 | ~208 KB |
| nusound | 0x80090000 | 0x800B0000 | ~128 KB |
| gamelib | 0x800B0000 | 0x80100000 | ~320 KB |
| gamecode | 0x80100000 | 0x8018CB00 | ~570 KB |

## Struct evidence quick reference

| Pattern | Struct | Offset layout |
|---------|--------|--------------|
| `KEY(r3)` | `nuanimkey_s` (0x10) | f32@0(time),4(dtime),8(c),12(d) |
| `CURVE(r9)` | `nuanimcurve_s` (0x10) | ptr/word@0(mask),4(keys),8(numkeys),12(flags) |
| `DAT2(r3)` | `nuanimdata2_s` (0x18) | u16@4(nnodes),6(ncurves), ptr@C(curves),10(flags),14 |
| `VEC3` | `nuvec_s` (0xC) | f32@0(x),4(y),8(z) |
| `PTR(rX:0x0-0x30)` | `nudathdr_s` (0x30) | s32@0(ver),4(nfiles),8(finfo*),C(treecnt),10(filetree*)... |

## Conventions

- Function names: `Nu{Subsystem}{Verb}` (engine), `Action_*`/`Condition_*` (dispatch)
- Struct tags: `nu{name}_s` (engine), `NU{NAME}_s` (game)
- Fields: `camelCase`
- Error strings: `str_FunctionName_Description`
- No comments in decomp source unless explaining a subtle match constraint
