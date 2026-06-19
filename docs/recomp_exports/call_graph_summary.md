# Recomp call graph export

- Direct `bl`/`bla` edges: **18158**
- Platform-boundary edges: **2922**
- Evidence: PPC opcode 18 with LK=1; caller/callee are resolved only when each address falls within a sized `.text` symbol.
- Limitations: indirect calls, tail branches, unresolved targets, and symbols without usable sizes are excluded. This is call-edge evidence, not call order.

## Platform families present

- `AI`: 286
- `AR`: 42
- `AX`: 115
- `CARD`: 125
- `DVD`: 232
- `EXI`: 260
- `GX`: 609
- `NuDat`: 87
- `NuFile`: 223
- `NuRndr`: 102
- `NuSound`: 15
- `NuTex`: 32
- `OS`: 1049
- `PAD`: 82
- `SI`: 124
- `VI`: 94
