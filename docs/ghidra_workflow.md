# Ghidra Helper Workflow

Ghidra is optional in this repo. The normal matching build does not require it.
Use it to generate C-like reference output for understanding control flow before
writing matching ProDG (SN GCC) C by hand.

## Setup

Install Ghidra and either put `analyzeHeadless` on `PATH`, or set:

```sh
export GHIDRA_INSTALL_DIR=/path/to/ghidra
```

The helper uses `orig/GL5E4F/sys/main.dol` by default.

## Import Symbols

Run this once after creating the Ghidra project, and rerun it after major symbol
renaming passes:

```sh
python tools/ghidra_decompile.py import-symbols
```

This applies names from `config/GL5E4F/symbols.txt` to the local Ghidra project.
The project is stored under `ghidra_out/project/`.

## Export One Function

```sh
python tools/ghidra_decompile.py decompile Shop_UpdateSubMenu
python tools/ghidra_decompile.py decompile 0x801486FC
```

Output goes to `ghidra_out/decomp/<Function>.c`.

## Export a Small Batch

```sh
python tools/ghidra_decompile.py decompile \
  Shop_UpdateMenu \
  Shop_UpdateMainMenu \
  Shop_UpdateSubMenu \
  Shop_UpdateCodes
```

The wrapper invokes the export script once per function so each result stays
isolated.

## How To Use the Output

Treat Ghidra output as reference material only:

- Use it to identify branches, locals, switch structure, pointer types, and
  likely helper boundaries.
- Rename symbols and struct fields in `symbols.txt` / headers based on confirmed
  evidence.
- Write matching source manually in `src/`.
- Verify with `bash build.sh`, `ninja`, and objdiff.

Do not commit Ghidra output as matching source. It is not ProDG-shaped C and
will usually need substantial cleanup to compile or match.

## Rename Handoff Loop

Use Ghidra as one part of the rename stack, not as a replacement for donor
evidence:

1. Pick a hub from `docs/shop_menu_architecture.md` or
   `docs/symbol_donors/gc_data_xrefs.tsv`.
2. Check direct donor names first with `rg` in `docs/symbol_donors/`.
3. Run `python tools/ls1_lookup_symbol.py <addr-or-name>` for call graph and
   neighbor context.
4. Export one Ghidra function with `python tools/ghidra_decompile.py decompile
   <name-or-addr>`.
5. Rename only confirmed functions/data in `config/GL5E4F/symbols.txt`.
6. Run `bash build.sh`.
7. Run `python tools/ghidra_decompile.py import-symbols` before the next Ghidra
   export so the generated C benefits from the new names.

Current best next shop targets:

```sh
python tools/ghidra_decompile.py decompile \
  Shop_UpdateMainMenu \
  Shop_UpdateSubMenu \
  fn_8014ABAC \
  fn_8014BBE0
```

## Shop Pass

For the shop crash investigation, export these first:

```sh
python tools/ghidra_decompile.py decompile \
  Shop_UpdateSubMenu \
  Shop_UpdateMainMenu \
  Shop_UpdateCodes \
  SelectShopItem \
  BuyShopItem
```

The first goal is to recover the shape of the category-table selection in
`Shop_UpdateSubMenu`, especially the case where `gShopCurrentCategory` is not
`0`, `1`, or `2`.
