# Build configuration

`configure.py` selects comparison/linked objects, compiler pins and flags.
`config/GL5E4F/config.yml` tells DTK where to find the retail DOL, symbols and
splits. Keep these files consistent when adding a unit; see
[C matching](code_matching_workflow.md#promotion) for the verification gate.

## Symbols

`config/GL5E4F/symbols.txt` uses absolute GameCube addresses:

```text
NuRandInt = .text:0x8000C0C0; // type:function size:0x24
```

Use the actual size from the binary. Common attributes are `type:function|object|label`, `size:`, `scope:global|local|weak`,
`align:` and `data:` (`float`, `double`, `4byte`, `string`, etc.). `noreloc`
prevents pointer-looking contents from becoming relocations; `force_active`
prevents dead stripping. Apply these only with evidence. The DTK configuration
schema is documented in [config.example.yml](../config/GL5E4F/config.example.yml).

Use `tools/ls1_rename.py` for verified names. Check sizes and references before
merging labels into a struct or splitting an object; a name change alone does
not establish a boundary. Generated reports can retain old names.

## Splits

`config/GL5E4F/splits.txt` declares section types/alignments and per-unit ranges.
Ranges are absolute and end-exclusive. Existing nurand wiring:

```text
Sections:
    .text       type:code align:4
    .sdata      type:data align:8

numath/nurand.c:
    .text       start:0x8000C0C0 end:0x8000C1B8
    .sdata      start:0x80407B40 end:0x80407B64
```

This is an excerpt, not a replacement for the full section list. A source path
must also be configured as an `Object` in `configure.py` to compile C. Unassigned
ranges remain target objects. DTK infers link order; do not force an order or
shrink ranges to obtain a higher match score. Assign code and owned data together.

Relevant optional attributes are section `align:`, `rename:` and `skip`, plus
file `order:` and `comment:`. `comment:0` disables generated Metrowerks metadata
for an object; it is linker compatibility, not a ProDG compiler setting.
Do not apply common-BSS or CodeWarrior `.comment` recipes from other games
without evidence that the target needs them.

## Dolphin SDK and retail linker

`DolphinLib` uses MWCC `GC/1.2.5n` with dolsdk2004's release flags:
`-O4,p -inline auto`, unsigned char by default, and `SDK_REVISION=1` / `RELEASE`.
`sdk_cflags` replaces flags for signed-char DVD, MTX/MTX44 and CARDOpen units,
and `-O3,p` EXIBios; it does not append a competing optimization level.
SDK-only include paths select the vendored libc shims. `-MD` records header
dependencies even with `-I-`.

SDK objects run `dtk elf fixup`, then `tools/strip_unused.py --target ...`.
The retained names come from the independently split target object. The
RE4-derived stripper removes unused functions/data while shifting symbols,
relocations and MWCC comment records, and rejects dangling references. Local
pooled data is retained; unused global data follows the reference's eight-byte
granularity. `.sdata2` is normalized to SN/DTK read-only ELF metadata.

An object that opts in with `external_bss=True` (only GXInit so far) also passes
`--external-bss config/GL5E4F/symbols.txt`. MWCC generates different code for
extern and statically defined objects, so the source keeps the donor's static
BSS definitions. The stripper then replaces them with undefined references to
the fallback `.bss` symbols and empties the section. It aborts unless every
name, size and relative offset agrees with `symbols.txt`. Those fallback symbols
must be global, and BSS contents stay unpromoted.

The DOL uses the existing `ProDG/3.9.3/ngcld.exe` through
`tools/run_prodg_link.py`, with `config/GL5E4F/ldscript.ld`. This fixes MWLD's
SDA register selection and startup symbol differences without changing compiler
pins or patching the linked image. MWLD support remains available in the build
generator for its original configurations. The current whole image matches
retail. See [SDK import evidence](sdk_import.md) for ranges and limits.

## Splitter and startup constraints

`tools/build_patched_dtk.sh` pins DTK and applies
`tools/patches/dtk-skip-invalid-extab.patch`. The patch handles this ProDG
binary's exception tables and symbol map; an unpatched DTK is not equivalent.
Build from that script rather than substituting the similarly named release tag.

Two target-specific exceptions remain in `config.yml`:

- `skip_cfa_ranges` excludes `.text:0x80151AFC..0x8015BF48` because the initial
  analyzer overextended control flow into known functions. Review this region
  manually before deriving new function boundaries from it.
- `block_relocations` excludes `.init:0x8000315C`, whose startup address cannot
  be resolved to a section by DTK. This permits splitting; it does not solve
  the linked startup differences described in [C matching](code_matching_workflow.md#remaining-link-differences).

Compiler/DTK pin changes require a deliberate retail rebuild, strict object
verification, linked-range comparison and report comparison. Renovate does not
validate these toolchain assumptions.
