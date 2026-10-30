# Related symbols for fn_80023CD0

## Callees (2)

- `fn_7FFB0704` @ 0x7FFB0704
- `fn_7FFCB95C` @ 0x7FFCB95C

## Data references

- `0x80190DE4`
- `0x80190E2C`
- `0x80190E30`
- `0x80190E34`
- `0x80190E38`
- `0x80190E3C`
- `fn_0x8000DC10`
- `fn_0x80023CD0`

## Object notes

- This function allocates and initializes a recovered `0x68` camera-state
  object compatible with `struct nucamstate_s` in `src/nu3dx/nu3dx.h`.
- Field evidence:
  - writes scalar camera parameters at `+0x40`, `+0x44`, `+0x48`, `+0x4C`
  - writes additional clip-like values at `+0x50`, `+0x54`, `+0x58`
  - writes a 3-float tail through `+0x64`
- The live global camera-state base is `0x80313268`; do not treat the old
  fragmented BSS labels around `0x80313288..0x803132B4` as separate objects.
