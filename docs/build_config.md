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
