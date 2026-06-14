# SDK Symbol Order Sources

Place local Dolphin SDK, CrashWOC, or other known-library symbol-order
sources in this directory. `python tools/sdk_anchor_rename_queue.py` scans
`*.txt`, `*.lst`, `*.map`, `*.sym`, and `*.tsv` files here, including nested
import folders.

If the source is a raw map or symbol dump, normalize it first:

```sh
python tools/sdk_symbol_source_import.py /path/to/crashwoc/framework.map --out-dir docs/symbol_donors/sdk_symbol_sources/imported
```

For a whole donor tree, add `--recursive` and point the tool at the root
directory.

Common inputs:

1. Crash: Wrath of Cortex GameCube decomp outputs, because it matches the same
   platform, engine family, and Metrowerks toolchain.
2. Reconstructed SDK source trees from Dolphin or another decomp project, when
   they include an accompanying linker map or symbol-order dump.
3. Existing GameCube/Wii decomp map files such as `framework.map`, `dol.map`,
   `sdk.map`, or `main.map`.
4. Debug-build symbol tables or map files from retail-era development builds.

The queue does not require a strict file format. It reads symbol-looking
tokens in source order and ignores addresses and other noise. For map files,
that means you can usually drop the file in directly, or normalize them first
if you want a cleaner TSV.

Accepted input is intentionally loose. Any token that looks like a symbol name
is considered, and address prefixes are ignored. Examples:

```text
__GXInitRevisionBits
GXInit
__GXInitGX
GXInitFifoBase
```

```text
8017CA2C __GXInitRevisionBits
8017CBD0 GXInit
8017D1D0 __GXInitGX
```

The matcher uses already-named GC SDK functions as anchors. If a source has two
anchors in the same order, unnamed GC functions between those anchors are
aligned against source symbols in that gap. Exact-count gaps are emitted as
higher-confidence candidates.
