# Object Context for fn_80023F80

## Recovered object

- Base: `0x80313268`
- Size: `0x68`
- Type: `struct nucamstate_s`

## Relevant fields used here

- `+0x48` `nearclip`
- `+0x4C` `farclip`

These values are compared against projected data derived from transforms built
from `0x801B1638` and `0x801B1878`.

## Boundary notes

- Do not treat `lbl_80313298` as a separate object while analyzing this
  function. It is inside the same recovered `0x68` camera-state block.
- The old donor mapping `DoorExitCameraSplineName -> lbl_80313268` is still
  useful as a prefix-field hint, but not as the boundary for the whole object.

## Cross-function context

- `fn_80023CD0` builds the same object shape
- `fn_80023C70` copies the same `0x68` object
- `fn_8001C20C` consumes the same state block extensively and provides the
  strongest field-offset evidence
