# Related symbols for fn_8001C20C

## Callees (43)

- `fn_7FFD66BC` @ 0x7FFD66BC
- `fn_7FFD6740` @ 0x7FFD6740
- `fn_7FFD677C` @ 0x7FFD677C
- `fn_7FFD69F0` @ 0x7FFD69F0
- `fn_7FFD6A68` @ 0x7FFD6A68
- `fn_7FFD6AAC` @ 0x7FFD6AAC
- `fn_7FFD6DE0` @ 0x7FFD6DE0
- `fn_7FFD6E54` @ 0x7FFD6E54
- `fn_7FFD7188` @ 0x7FFD7188
- `fn_7FFD7250` @ 0x7FFD7250
- `fn_7FFD7584` @ 0x7FFD7584
- `fn_7FFDF7D8` @ 0x7FFDF7D8
- `fn_7FFDF808` @ 0x7FFDF808
- `fn_7FFDF850` @ 0x7FFDF850
- `fn_7FFE4FF0` @ 0x7FFE4FF0
- `fn_7FFE5020` @ 0x7FFE5020
- `fn_7FFE5448` @ 0x7FFE5448
- `fn_7FFE5698` @ 0x7FFE5698
- `fn_7FFE57F0` @ 0x7FFE57F0
- `fn_7FFE5BEC` @ 0x7FFE5BEC
- `fn_7FFE60F8` @ 0x7FFE60F8
- `fn_7FFE61B8` @ 0x7FFE61B8
- `fn_7FFE61F4` @ 0x7FFE61F4
- `fn_7FFE6230` @ 0x7FFE6230
- `fn_7FFE66EC` @ 0x7FFE66EC
- `fn_8001C20C` @ 0x8001C500
- `fn_8001C20C` @ 0x8001C654
- `fn_8001C20C` @ 0x8001C78C
- `fn_8001C20C` @ 0x8001C878
- `fn_8001CA54` @ 0x8001CBF0
- `fn_80038210` @ 0x80038518
- `fn_8003B874` @ 0x8003B99C
- `fn_8003BA5C` @ 0x8003BEE8
- `fn_8003BA5C` @ 0x8003C090
- `fn_8003BA5C` @ 0x8003C290
- `fn_8003BA5C` @ 0x8003C68C
- `fn_8003FD38` @ 0x8003FD4C
- `fn_8004901C` @ 0x80049170
- `fn_8005E2EC` @ 0x8005E5A0
- `fn_80092230` @ 0x80092744
- `fn_80501D78` @ 0x80501D78
- `fn_80502120` @ 0x80502120
- `fn_8050251C` @ 0x8050251C

## Data references

- `0x801907D4`
- `0x801907D8`
- `0x801907DC`
- `0x801907E0`
- `0x801907E4`
- `0x801907E8`
- `0x801907F0`
- `0x801B0F08`
- `0x801B1638`
- `0x801B1678`
- `0x801B16B8`
- `0x801B16F8`
- `0x801B1738`
- `0x801B1778`
- `0x801B17B8`
- `0x801B17F8`
- `0x801B1838`
- `0x801B1878`
- `0x8020D7D8`
- `0x80313268`
- `0x803142D4`
- `0x80407C50`
- `0x80407C5C`
- `0x80407C60`
- `0x80407C64`
- `0x80408118`
- `0x8040811C`
- `0x8040AD3C`
- `0x8040AD40`
- `0x8040AD44`
- `0x8040AD48`
- `0x8040AD4C`
- `0x8040AD50`
- `fn_0x8000ABDC`
- `fn_0x8000B080`
- `fn_0x8000B144`
- `fn_0x8000D134`
- `fn_0x8000E6B0`
- `fn_0x8000EA74`
- `fn_0x8000EB44`
- `fn_0x8000EBE8`
- `fn_0x8000ECA4`
- `fn_0x8000EEDC`
- `fn_0x8001C20C`
- `fn_0x800234A8`
- `fn_0x8002423C`
- `fn_0x800244A8`
- `fn_0x80024520`
- `fn_0x800256E4`
- `fn_0x80027BE4`
- `fn_0x8002D0FC`
- `fn_0x80039BE0`

## Object notes

- `0x80313268` is the base of a recovered `0x68` camera-state object
  (`struct nucamstate_s` in `src/nu3dx/nu3dx.h`).
- This function provides the strongest field-offset evidence for that object.
- Confirmed offsets used here:
  - `0x20`, `0x28` from the matrix block
  - `0x40` `fov`
  - `0x44` `aspect`
  - `0x48` `nearclip`
  - `0x4C` `farclip`
  - `0x50` `nearclip_alt`
  - `0x54` `farclip_alt`
  - `0x5C..0x67` tail vector (`pos`)
- The old BSS labels at `0x80313288`, `0x80313298`, `0x803132B0`, and
  `0x803132B4` should be treated as internal offsets, not object boundaries,
  while analyzing this function.
