# Related symbols for fn_80023C70

## Data references

- `0x80313268`
- `fn_0x80023C70`

## Boundary notes

- `0x80313268` is the base of a recovered `0x68` camera-state object,
  not a standalone `0x20` BSS object.
- Use `struct nucamstate_s` from `src/nu3dx/nu3dx.h` for field recovery.
- Current recovered layout:
  - `0x00..0x3F` `numtx_s mtx`
  - `0x40` `fov`
  - `0x44` `aspect`
  - `0x48` `nearclip`
  - `0x4C` `farclip`
  - `0x50` `nearclip_alt`
  - `0x54` `farclip_alt`
  - `0x58` `unk58`
  - `0x5C..0x67` `nuvec_s pos`
- Treat `lbl_80313288`, `lbl_80313298`, `lbl_803132B0`, and `lbl_803132B4`
  as offsets inside this object during reverse engineering.
