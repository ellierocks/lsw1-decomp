# Related symbols for fn_80023F80

## Callees (4)

- `fn_7FFBE284` @ 0x7FFBE284
- `fn_7FFCF6AC` @ 0x7FFCF6AC
- `fn_7FFCF6DC` @ 0x7FFCF6DC
- `fn_8002406C` @ 0x8002406C

## Data references

- `0x80190E54`
- `0x801B1638`
- `0x801B1878`
- `0x80313268`
- `fn_0x8000A8AC`
- `fn_0x8000ED80`
- `fn_0x80023F80`

## Object notes

- `0x80313268` is the base of a recovered `0x68` camera-state object
  (`struct nucamstate_s`), not a standalone `0x20` BSS symbol.
- This function reads camera-state fields at:
  - `+0x48` `nearclip`
  - `+0x4C` `farclip`
- The helper at `fn_8002406C` still references `0x80313298`, but that address
  should be treated as an internal offset within the same larger camera-state
  block during analysis.
