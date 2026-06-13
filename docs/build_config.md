# Build Configuration Reference

This document covers the build system configuration formats and low-level CodeWarrior details needed for this decompilation.

## Symbols Reference

The `config/GL5E4F/symbols.txt` file contains all symbols for a module, one per line.

### Format

Numbers can be written as decimal or hexadecimal. Hexadecimal numbers must be prefixed with `0x`.

Comment lines starting with `//` or `#` are permitted, but are currently **not** preserved when updating the file.

```
symbol_name = section:address; // [attributes]
```

- `symbol_name` - The name of the symbol. (For C++, this is the mangled name, e.g. `__dt__13mDoExt_bckAnmFv`)
- `section` - The section the symbol is in.
- `address` - The address of the symbol. For DOLs, this is the absolute address (e.g. `0x80001234`). For RELs, this is the section-relative address (e.g. `0x1234`).

### Attributes

All attributes are optional, and are separated by spaces.

- `type:` The symbol type. `function`, `object`, or `label`.
- `size:` The size of the symbol.
- `scope:` The symbol's visibility. `global` (default), `local` or `weak`.
- `align:` The symbol's alignment.
- `data:` The data type used when writing disassembly. `byte`, `2byte`, `4byte`, `8byte`, `float`, `double`, `int`, `short`, `string`, `wstring`, `string_table`, `wstring_table`, `sjis`, or `sjis_table`.
- `hidden` Marked as "hidden" in the generated object. (Only used for extab)
- `force_active` Marked as "exported" in the generated object, and added to `FORCEACTIVE` in the generated `ldscript.lcf`. Prevents the symbol from being deadstripped.
- `noreloc` Prevents the _contents_ of the symbol from being interpreted as addresses. Used for objects containing data that look like pointers, but aren't.
- `noexport` When `export_all` is enabled, this excludes the symbol from being exported. Used for symbols that are affected by linker `-code_merging`.
- `stripped` Indicates a symbol that was stripped by the linker. Used for symbols that affect the [common BSS inflation bug](#common-bss), despite not existing in the final binary.

## Splits Reference

The `config/GL5E4F/splits.txt` file contains file splits for a module.

### Header

```yaml
Sections:
    section     [section attributes]
```

#### Attributes

- `type:` The section type. `code`, `data`, `rodata` or `bss`.
- `align:` The section alignment in bytes.
- `vaddr:` (REL only) The fixed virtual address of the section. When set, split and symbol addresses are written as absolute addresses.

### Files

```yaml
path/to/file.cpp: [file attributes]
    section     [section attributes]
    ...
```

- `path/to/file.cpp` The name of the source file, usually relative to `src`. The file does **not** need to exist to start.  
  This corresponds to an entry in `configure.py` for specifying compiler flags and other options.

#### File attributes

- `comment:` Overrides the `mw_comment_version` setting in `config.yml` for this file. See [`.comment` section](#codewarrior-comment-section).
  - `comment:0` is used to disable `.comment` section generation for a file that wasn't compiled with `mwcc`.  
  Example: `TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s: comment:0`  
  This file was assembled and only contains label symbols. Generating a `.comment` section for it will crash `mwld`.

- `order:` Allows influencing the resolved link order of objects. This is **not required**, as decomp-toolkit will generate the link order automatically. This can be used to fine-tune the link order for ambiguous cases.  
  Example:
  ```
  file1.cpp: order:0
    ...

  file2.cpp: order:1
    ...

  file3.cpp: order:2
    ...
  ```
  This ensures that `file2.cpp` is always anchored in between 1 and 3 when resolving the final link order.

#### Section attributes

- `start:` The start address of the section within the file. For DOLs, this is the absolute address (e.g. `0x80001234`). For RELs, this is the section-relative address (e.g. `0x1234`).
- `end:` The end address of the section within the file.
- `align:` Specifies the alignment of the section. If not specified, the default alignment for the section is used.
- `rename:` Writes this section under a different name when generating the split object. Used for `.ctors$10`, etc.
- `common` Only valid for `.bss`. See [Common BSS](#common-bss).
- `skip` Skips this data when writing the object file. Used for ignoring data that's linker-generated.

## Common BSS

When passed the `-common on` flag, `mwcc` will generate global BSS symbols as **common**. The linker deduplicates common symbols with the same name, and allocates an area at the **end** of `.bss` for them.

This is a legacy feature, allowing uninitialized global variables to be defined in headers without linker errors:

```c
// foo.h
int foo;
```

With `-common on`, any TU that includes `foo.h` will define `foo` as a **common** symbol. The linker will deduplicate `foo` across TUs, similar to weak symbols. Common symbols are then generated at the **end** of `.bss`, after all other `.bss` symbols.

With `-common off`, `foo` would be defined as a **global** symbol, and the linker would error out with a duplicate symbol error if `foo.h` was included in multiple TUs.

In `splits.txt`, common BSS can be defined with the `common` attribute:

```yaml
foo.cpp:
	.text       start:0x80047E5C end:0x8004875C
	.ctors      start:0x803A54C4 end:0x803A54C8
	.data       start:0x803B1B40 end:0x803B1B60
	.bss        start:0x803DF828 end:0x803DFA8C
	.bss        start:0x8040D4AC end:0x8040D4D8 common
```

As shown above, a file can contain both regular `.bss` and common `.bss`. Marking common `.bss` appropriately is important for determining the final link order.

### Detection

Example from Pikmin 2:
```
00016e60 00000c 805069c0  1 .bss 	utilityU.a PSMainSide_CreaturePrm.cpp
00016e60 00000c 805069c0  4 @3464 	utilityU.a PSMainSide_CreaturePrm.cpp
00016e6c 000048 805069cc  4 saoVVOutput_direction___Q214JStudio_JStage14TAdaptor_light 	JSystem.a object-light.cpp
00016eb4 0000d0 80506a14  4 saoVVOutput___Q214JStudio_JStage14TAdaptor_actor 	JSystem.a object-actor.cpp
```

In this example, we see a symbol from `utilityU.a PSMainSide_CreaturePrm.cpp`. We know that this file is very close to the _end_ of the link order. Afterwards, there's a symbol from `JSystem.a object-light.cpp`, which is very close to the _beginning_ of the link order.

A file can't be both at the beginning and end of the link order, so it's a strong indication that `saoVVOutput_direction___Q214JStudio_JStage14TAdaptor_light` marks the beginning of the common BSS section.

One other indication from this example is the lack of a `.bss` section symbol from `JSystem.a object-actor.cpp` and any following files in the link order. Section symbols aren't generated for common BSS.

Without a map, it's harder to tell if there's a common BSS section, but guesses can be made. When looking at XREFs in Ghidra, if a symbol is close to the _end_ of `.bss`, but has XREFs from various addresses close to the _beginning_ of `.text`, it could be an indication of common BSS.

For games built with older versions of the linker, the inflation bug (described below) can also be used to detect common BSS.

### Inflation bug

In older versions of the linker (<= GC 2.6?), when calculating the size of common symbols, the linker will accidentally set the size of the first common symbol in a TU to the size of the _entire_ common section in that TU.

Example from Pikmin 2:

```
# Section Addr | Size | Addr | Alignment | Name | File
00017260 000188 80506dc0  4 mPadList__10JUTGamePad 	JSystem.a JUTGamePad.cpp
000173e8 000030 80506f48  4 mPadStatus__10JUTGamePad 	JSystem.a JUTGamePad.cpp
00017418 0000c0 80506f78  4 mPadButton__10JUTGamePad 	JSystem.a JUTGamePad.cpp
000174d8 000040 80507038  4 mPadMStick__10JUTGamePad 	JSystem.a JUTGamePad.cpp
00017518 000040 80507078  4 mPadSStick__10JUTGamePad 	JSystem.a JUTGamePad.cpp
00017558 00000c 805070b8  4 sPatternList__19JUTGamePadLongPress 	JSystem.a JUTGamePad.cpp
```

In this example, `mPadList__10JUTGamePad` is the first common symbol in the TU, and was inflated to include the size of all other common symbols in the TU. In reality, it's only supposed to be `0xC` bytes, given `0x188 - 0x30 - 0xC0 - 0x40 - 0x40 - 0xC`.

This can be useful to determine if symbols are in the same TU without a map: if a `.bss` symbol is much larger than expected, it could be the first common symbol in a TU. One can subtract the sizes of following symbols to find the true size of the symbol, along with the end of the TU's common symbols.

To reproduce this behavior, the `.comment` section must be present in the object. See [`.comment` section](#codewarrior-comment-section) for more details.

## CodeWarrior `.comment` section

Files built with `mwcc` contain a `.comment` section:

```
$ powerpc-eabi-readelf -We object.o

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        00000000 000034 000708 00  AX  0   0  4
  ...
  [16] .comment          PROGBITS        00000000 00153b 0001b4 01      0   0  1
```

The `.comment` section contains information that `mwld` uses during linking, primarily symbol alignment and a "force active" / export flag.

If missing, `mwld` will **not** adjust the alignment of symbols or remove any unused symbols.

This behavior is quite useful in some cases. When we split our program into objects, we're working from the final post-aligned, post-stripped result, and don't want the linker to make any changes. Most decompilation projects rely on this behavior unintentionally, since their generated objects don't contain a `.comment` section. (For example, objects built with `powerpc-eabi-as`.)

However, we need the `.comment` section for some purposes:

- Reproducing the [common BSS inflation bug](#common-bss) requires the `.comment` section present, due to the above. The linker inflates the size of the first common BSS symbol in a TU, but won't actually move any data around unless the `.comment` section is present.
- In newer versions of the linker, using common BSS at all _without_ a valid `.comment` section will cause an internal linker error.

When the `.comment` section is generated, decomp-toolkit will mark all global symbols as "exported" to prevent any deadstripping, since the presence of the `.comment` section itself enables deadstripping.

Generating the `.comment` section and setting the "export" flag is also useful to prevent the linker from removing entire objects. A missing `.comment` section will prevent the removal of unused symbols _inside_ of an object, but the linker will still remove the entire object itself if it thinks it's unused.

### Contents

The contents of this section follow a very simple format:

#### Header

`[0x0 size: 0xB]` Magic: `43 6F 64 65 57 61 72 72 69 6F 72` ("CodeWarrior")  

`[0xB size: 0x1]` Version(?): `XX`

It's not known whether this field actually affects `mwld` in any way, but it's configurable for completeness sake. (See `mw_comment_version` in `config.yml`.)

Known values:

- `08` (8) - CodeWarrior for GameCube 1.0+
- `0A` (10) - CodeWarrior for GameCube 1.3.2+
- `0B` (11), `0C` (12) - CodeWarrior for GameCube 2.7+ (difference unknown)
- `0E` (14), `0F` (15) - CodeWarrior for GameCube 3.0a3+ (difference unknown)

`[0xC size: 0x4]` Compiler version: `XX XX XX XX`

First 3 bytes are major, minor, and patch version numbers.  
4th byte is unknown, but is always `01`.

Example: `Version 2.3.3 build 144` -> `02 03 00 01`  
Often the `.exe`'s properties (which `--help` reads from) and the internal version number (here) will differ.

`[0x10 size: 1]` Pool data: `XX`

- `00` - Data pooling disabled
- `01` - Data pooling enabled

`[0x11 size: 1]` Float type: `XX`

- `00` - Floating point disabled
- `01` - Software floating point
- `02` - Hardware floating point

`[0x12 size: 2]` Processor type: `00 16` (Gekko)

`[0x14 size: 1]` Unknown, always `2C`. Possibly the start of symbol entries.

`[0x15 size: 1]` "Quirk" flags: `XX`

Bitfield of miscellaneous flags. Known flags:

- `01` - "Incompatible return small structs"
- `02` - "Incompatible SFPE double params"
- `04` - "Unsafe global reg vars"

`[0x16 size: 22]` Padding until `0x2C`

#### Symbol entry

At `0x2C` is the first symbol entry. There is one 8 byte entry per ELF symbol.

This includes the "null" ELF symbol, so the first entry will be all 0's.

`[0x0 size: 4]` Alignment: `XX XX XX XX`

`[0x4 size: 1]` Visibility flags(?): `XX`

Known values:

- `00` - Default
- `0D` - Weak
- `0E` - Unknown, also weak?

`[0x5 size: 1]` Active flags(?): `XX`

Known values:

- `00` - Default
- `08` - Force active / export. Prevents the symbol from being deadstripped.  
  When applied on a section symbol, the entire section is kept as-is. This is used
  by `mwcc` when data pooling is triggered (indicated by a symbol like `...data.0`), likely to prevent the hard-coded section-relative offsets from breaking.  
  Can also be set using `#pragma force_active on` or `__declspec(export)`.
- `10` - Unknown
- `20` - Unknown

`[0x6 size: 2]` Padding(?): `00 00`
