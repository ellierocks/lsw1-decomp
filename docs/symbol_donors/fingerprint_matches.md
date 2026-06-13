# Function Fingerprint Matching Results

Matched 5 unnamed GC functions to Mac Action_/Condition_ functions
using callee-set overlap.

**Method:** For each unnamed GC function, compute the set of named functions it calls.
Compare against each Mac Action_/Condition_ function's callee set. Score by string
overlap of callee names (after stripping Action_/Condition_ prefix).

---
## Best Matches (score >= 1)

| GC Func | Size | Mac Match | Score | GC Callee Overlap | Mac Callees |
|---------|------|-----------|-------|-------------------|-------------|
| fn_80007F00 | 0x58 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | NuPPLoadBuffer, TerrShapeSideStep, TerrainPlatformEmbedded,  |
| fn_80007F58 | 0xA0 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | NuPPLoadBuffer, TerrShapeSideStep, TerrainPlatformEmbedded,  |
| fn_800081CC | 0xD8 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | NuPPLoadBuffer, TerrShapeSideStep, TerrainPlatformEmbedded,  |
| fn_80009E94 | 0x3D4 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | NuPPLoadBuffer, TerrShapeSideStep, TerrainPlatformEmbedded,  |
| fn_800E0790 | 0xB68 | `Action_ZamMovement` | 1.0 | NuPPLoadBuffer | NuPPLoadBuffer, TerrShapeSideStep, TerrainPlatformEmbedded,  |

---
## All Matches

- fn_80007F00 → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_80007F58 → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_800081CC → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_80009E94 → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)
- fn_800E0790 → `Action_ZamMovement` (score=1.0, overlap=NuPPLoadBuffer)

---
## Stats

| Metric | Value |
|--------|-------|
| GC unnamed functions | 3697 |
| GC named functions | 119 |
| GC unnamed with named callees | 3697 |
| Mac Action_/Condition_ functions | 540 |
| Raw matches | 5 |
| Best matches | 5 |
