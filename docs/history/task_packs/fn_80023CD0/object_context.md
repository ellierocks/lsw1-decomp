# Object Context for fn_80023CD0

## Recovered object

- Base: `0x80313268`
- Size: `0x68`
- Type: `struct nucamstate_s`

## Boundary evidence

- `fn_80023C70` copies `0x68` bytes from `0x80313268`
- `fn_80023CD0` allocates `0x68` bytes and initializes fields through `+0x64`
- `fn_80023F80` reads camera values from `+0x48` and `+0x4C`
- The next independently referenced object begins at `0x803132D0`

## Field layout

- `0x00..0x3F` `numtx_s mtx`
- `0x40` `fov`
- `0x44` `aspect`
- `0x48` `nearclip`
- `0x4C` `farclip`
- `0x50` `nearclip_alt`
- `0x54` `farclip_alt`
- `0x58` `unk58`
- `0x5C..0x67` `nuvec_s pos`

## Notes for matching

- This function is a constructor/allocator for the same object shape later
  consumed by `fn_8001C20C` and `fn_80023F80`.
- Prefer typed field writes over raw offset commentary when drafting C.
