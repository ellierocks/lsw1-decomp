# Object Context for fn_8001C20C

## Recovered object

- Base: `0x80313268`
- Size: `0x68`
- Type: `struct nucamstate_s`

## Layout

- `0x00..0x3F` `numtx_s mtx`
- `0x40` `fov`
- `0x44` `aspect`
- `0x48` `nearclip`
- `0x4C` `farclip`
- `0x50` `nearclip_alt`
- `0x54` `farclip_alt`
- `0x58` `unk58`
- `0x5C..0x67` `nuvec_s pos`

## Why this function matters

- It copies an external `0x68` camera-state object into the live global state.
- It reads the projection parameters from `+0x40..+0x54`.
- It passes `state + 0x5C` by pointer, proving the tail is a 3-float vector.
- It is the central consumer tying together:
  - `fn_80023C70` (copy helper)
  - `fn_80023CD0` (allocator/initializer)
  - `fn_80023F80` (camera-state test/consumer)

## Analysis rule

- Treat the fragmented labels around `0x80313268` as field artifacts only.
- Use `struct nucamstate_s*` reasoning before attempting any name recovery or
  matching work in this cluster.
