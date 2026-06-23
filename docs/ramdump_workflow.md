# RAM-dump game-state inspection

Most of this decomp is static: struct layouts, data addresses and `fn_` purposes
are inferred from ASM alone. A raw RAM dump from a running game turns those
inferences into ground truth — every GC address resolves to real bytes.

`tools/ramdump.py` reads **raw Dolphin MEM1 dumps**. GC MEM1 starts at
`0x80000000` and is 24 MiB (`0x1800000`); a raw dump is exactly that blob, so
address `0x80000000` == file offset `0`. The console is big-endian, so all
multi-byte values decode big-endian.

## Capturing dumps

In Dolphin, run the game to a meaningful, stable state, then write a raw MEM1
dump (memory debugger → save, or a memory-dump hotkey/tool). Aim for
**big-picture snapshots** that isolate game state:

| Label    | State to capture                         |
|----------|------------------------------------------|
| `title`  | Title screen (minimal game state loaded) |
| `hub`    | Cantina / hub world                      |
| `shop`   | Character/shop screen                    |
| `level1` | Inside a specific level                  |
| `status` | A status / pause / save screen           |

Put the files under `dumps/` (git-ignored — they're 24 MiB each) and list them
in `dumps/manifest.txt` so you can use labels instead of paths.

## Commands

```sh
# Read typed value(s). --type: u8/s8/u16/s16/u32/s32/f32/f64/ptr/vec3/vec4/mtx/cstr/hex
python tools/ramdump.py read hub 0x80407B40 --type f32 --count 4
python tools/ramdump.py read hub 0x80407B50 --type ptr      # annotates the target symbol

# Resolve a symbol name, then read/hexdump it
python tools/ramdump.py sym hub NuRandSeed --type u32

# Name the symbol covering an address (great for pointers / vtable entries)
python tools/ramdump.py whatis 0x8000C720        # -> NuVecMtxRotate+0x20

# Hexdump a region
python tools/ramdump.py dump hub 0x80407B40 --len 0x80

# Scavenge ASCII strings (find state/level/object names)
python tools/ramdump.py strings hub --range 0x80400000 0x80410000
```

## The high-value workflow: diff snapshots

Diffing two states is the fastest way to locate **live state variables** — the
globals that drive game logic. Bound the diff to a section or address range to
cut framebuffer/heap noise.

```sh
# Which 32-bit cells changed between title screen and level? Labeled by nearest symbol.
python tools/ramdump.py diff title level1 --section sdata
python tools/ramdump.py diff hub shop --type f32 --range 0x80400000 0x80420000
```

What this unlocks for matching:

- **Confirm struct field offsets & sizes** by reading live objects instead of
  guessing — directly de-risks layout assumptions in the C ports.
- **Resolve `fn_` names via vtables** — read an object's vtable pointer
  (`read … --type ptr`), then `whatis` each slot to tie unnamed functions to a class.
- **Confirm globals/statics, enum values, the SDA base, and string tables**, all
  of which feed symbol naming and data-section reconstruction.
