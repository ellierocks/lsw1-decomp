# Status: BLOCKED

## Current state
Control flow matches 100%. Only register allocation differs:
original uses r9/r11, compiled standalone uses r4/r5.

## Remaining work
- Recover a larger object-context pack for the original translation unit.
- Compile fn_80023C70 inside that broader source cluster.
- Once register allocation matches, verify with objdiff-cli.

## Notes
- The `subic.` vs `addic.` disassembly difference is not real —
  same encoding (0x34xxFFE8).
- MWCC versions GC/1.2.5n, 1.2.5, 1.3.2, 1.3.2r all produce identical output.
- Compiling `fn_80023C70` with the immediate local neighbors
  (`fn_80023B50`, `fn_80023D80`, `fn_80023D8C`, `fn_80023DB0`,
  `fn_800240EC`, `fn_80024124`) still produces `r4/r5`, not `r9/r11`.
- `lbl_80313268` is not a real 0x20 standalone object boundary.
  Evidence:
  `fn_80023C70` copies 0x68 bytes from that base region,
  `fn_80023CD0` allocates 0x68 bytes and initializes fields through +0x64,
  and `fn_80023F80` reads floats at +0x48/+0x4C from the same base.
  Treat it as the start of a larger camera-related state struct whose
  first field likely corresponds to the Mac donor `DoorExitCameraSplineName`.
