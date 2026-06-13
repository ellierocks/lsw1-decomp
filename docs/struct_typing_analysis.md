# LSW1 Function Struct Typing Analysis

Based on Crash: Wrath of Cortex Nu2 engine struct layouts.
Reference: `docs/nu2_engine_reference.md`

## Methodology

For each unnamed function in `.text`, cross-reference:
1. Address range → subsystem (file, memory, math, anim, scene, etc.)
2. Named callers/callees from call graph → known Nu2 API functions
3. CrashWOC struct definitions → match sizes, field offsets, subsystem conventions

---

## 1. nucore/file (0x800034A0–0x80009000) — 76 unnamed

**Struct types**: `nudathdr_s` (0x30), `nudatinfo_s` (0x10), `nudfnode_s` (0x8), `nudatfile_s` (0x14), `numemfile_s` (0x14), `fileinfo_s`, `nuiffhdr_s` (0x8), `BlockInfo` (0xC)

**Named anchors**: NuFileOpen, NuFileClose, NuFilePos, NuFileLoadBuffer, NuFileBeginBlkRead, NuFileEndBlockRead, NuDatFileOpen, NuDatFileLoadBuffer, NuDatFileLoadBufferLsn, NuDatOpenEx

| Unnamed | Evidence | Proposed Type |
|---------|----------|---------------|
| `fn_800034A0` | Called by NuFileClose; 0x150 bytes | `NuFileInit` — struct nudathdr_s*, s32 handle |
| `fn_80003AEC` | Called by NuFileOpen, NuFilePos; calls NuFileLoadBuffer, NuFileClose | `NuFileSeek` — struct numemfile_s*, s32 offset, s32 whence |
| `fn_800040E0` | Called by NuFileOpen(3x), NuFileLoadBuffer, NuFileClose; calls NuFileBeginBlkRead, NuDatFileLoadBuffer | `NuFileBufferOp` — struct filebuff_s*, s32 op, void* buf, s32 size |
| `fn_80004BA4` | Called by NuDatFileLoadBuffer; calls NuDatFileOpen | `NuDatReadChunk` — struct nudathdr_s*, s32 offset, s32 size |
| `fn_8000556C` | Called by NuDatOpenEx | `NuDatOpenHelper` — char* path, struct nudathdr_s** out |
| `fn_800055EC` | Called by NuDatOpenEx | `NuDatFindFile` — struct nudathdr_s*, char* name, struct nudatinfo_s** out |
| `fn_800056E0` | Called by NuDatOpenEx | `NuDatReadTree` — struct nudathdr_s*, struct nudfnode_s* tree |
| `fn_80005F5C` | Called by NuDatFileLoadBuffer, NuDatFileLoadBufferLsn | `NuDatDecompress` — s32 mode, void* src, void* dst, s32 size |
| `fn_80007900` | Called by NuMemAlloc(2x), NuFileClose | `NuMemPoolOp` — struct memexternal_s*, s32 size |
| `fn_800081CC` | Called by NuPPLoadBuffer(2x) | `NuPPDecode` — void* dst, void* src, s32 width, s32 height |

---

## 2. nucore/memory (0x80006000–0x8000F000) — 150 unnamed

**Struct types**: `memexternal_s` (0x8), `filebuff_s` (0x4)

**Named anchors**: NuMemAlloc, NuMemFree, NuPPLoadBuffer

This range also contains file decompression helpers (called by NuDatFileLoadBuffer).

| Unnamed | Evidence | Proposed Type |
|---------|----------|---------------|
| `fn_80006E1C` | Calls NuMemAlloc | `NuMemAlign` — s32 size, s32 align |
| `fn_800076E4` | Called by NuMemAlloc(2x), NuMemFree | `NuMemPoolInit` — void* pool, s32 size |
| `fn_80010120` | Called by NuMemFree | `NuMemFreeInternal` — void* ptr, struct memexternal_s* pool |
| `fn_80010C30` | Called by NuMemFree | `NuMemFreeSmallHelper` |
| `fn_80010EF8` | Called by NuMemAlloc | `NuMemAllocInternal` — s32 size, struct memexternal_s* pool |
| `fn_80011070` | Called by NuMemAlloc | `NuMemAllocSmallHelper` |

---

## 3. numath (0x8000F000–0x80017000) — 105 unnamed

**Struct types**: `nuvec_s` (0xC), `nuvec4_s` (0x10), `numtx_s` (0x40), `Quat` (0x10), `nuivec_s` (0xC)

**Named anchors**: NuSpecialFind, NuSpecialFindMulti, NuAnimCurve2CalcVal

Functions in this range are vector/matrix operations (NuVec*, NuMtx*, NuQuat*) plus some scene object lookup code.

Likely Nu2 functions present but unnamed:
- `NuVecAdd` / `NuVecSub` / `NuVecScale` / `NuVecCross` / `NuVecDot`
- `NuMtxMul` / `NuMtxTranspose` / `NuMtxInv` / `NuMtxSetIdentity`
- `NuQuatSlerp` / `NuQuatMul` / `NuQuatNormalise`

| Unnamed | Evidence | Proposed Type |
|---------|----------|---------------|
| `fn_8000F094` | In math range, 0x16C bytes | `NuVecXxx` — struct nuvec_s*, params |
| `fn_8000F65C` | Called by NuDatOpenEx (not math — data loading) | `NuDataXxx` — struct nudathdr_s* |
| `fn_80012D00` | **Already named** NuSpecialFind | Takes struct nuscene_s*, char* |
| `fn_80012E94` | **Already named** NuSpecialFindMulti | Takes struct nuscene_s*, char* |

---

## 4. nu3dx/anim (0x80016A00–0x80024000) — 96 unnamed

**Struct types**: `nuanimkey_s` (0x10), `nuanimcurve_s` (0x10), `nuanimcurveset_s` (0x10), `nuanimdata_s` (0x10), `nuanimdata2_s` (0x18), `nuanimcurve2_s` (0x4), `NUJOINTANIM_s` (0x34)

**Named anchors**: NuAnimCurve2CalcVal

| Unnamed | Evidence | Proposed Type |
|---------|----------|---------------|
| `fn_80016A98` | Calls NuAnimCurve2CalcVal | `NuAnimCurve2CalcValueLen` — struct nuanimcurve2_s*, void* dest, s32 count |
| `fn_8001752C` | Called by NuDatFileOpen, NuDatOpenEx, NuAnimCurve2CalcVal | `NuAnimLoadData` — char* path, struct nuanimdata2_s** out |
| `fn_80017EA0` | Called by NuAnimCurve2CalcVal, NuDatFileLoadBuffer; calls NuSpecialFind | `NuAnimApplyToNode` — struct nuanimdata2_s*, struct nuscene_s*, f32 time |
| `fn_800187F4` | Called by NuAnimCurve2CalcVal, NuDatFileLoadBuffer | `NuAnimCurveInterp` — struct nuanimcurve2_s*, f32 time, f32* out |
| `fn_8001B22C` | Called by NuFilePos, NuFileClose | File seek helper (not anim) |
| `fn_8001B7E8` | Called by NuDatFileOpen, NuFileOpen | File data loading (not anim) |

---

## 5. nucore/file2 (0x80024000–0x80070000) — 803 unnamed

**Struct types**: `nugeom_s` (0x30), `nugobj_s` (0x64), `numtl_s` (0x54), `nutex_s` (0x1C), `nuscene_s` (0x3C), `nucamera_s` (0x60), `nurndritem_s` (0x10), `nuprim_s` (0x3C), `nuskin_s` (0x18), `nulight_s` (0x58)

This is the bulk Nu2 engine — scene construction, object loading, rendering setup, material/texture/light management.

Best approach for this region: when decompiling a function, check if it:
- Calls `NuFile*` → likely scene/object loader
- Calls `NuMem*` → likely allocator for Nu2 structs
- Calls `NuAnim*` → likely animation system integrator
- References data with known struct sizes → match to `nugeom_s`, `numtl_s`, etc.

---

## 6. Named Function Signatures (Nu2 API)

These are the externally-facing Nu2 API functions with known signatures from CrashWOC:

```c
// nucore/nufile.h
s32 NuFileOpen(char* path, enum nufilemode_e mode);
void NuFileClose(s32 handle);
s32 NuFilePos(s32 handle);
s32 NuFileLoadBuffer(s32 handle, void* buffer, s32 offset, s32 size, s32 blockalign);
s32 NuFileBeginBlkRead(s32 handle, s32 size, struct nuiffhdr_s* hdr);
void NuFileEndBlockRead(s32 handle);
s32 NuDatOpenEx(struct nudathdr_s** out, char* path, char* basepath, s32 flag);
s32 NuDatFileLoadBuffer(struct nudathdr_s* ndh, s32 offset, void* buffer, s32 size);
s32 NuDatFileLoadBufferLsn(struct nudathdr_s* ndh, s32 lsn, s32 offset, void* buffer, s32 size);
s32 NuDatFileOpen(struct nudathdr_s* ndh, char* path, struct nudatfile_s* df, s32 flag);

// nucore/numem.h
void* NuMemAlloc(s32 size);
void NuMemFree(void* data);

// nu3dx/nuanim.h
void NuAnimCurve2CalcVal(struct nuanimcurve2_s* curves, f32 time, void* result);

// gamelib/main.h
struct nuinstance_s* NuSpecialFind(struct nuscene_s* scene, char* name);
s32 NuSpecialFindMulti(struct nuscene_s* scene, char* name, struct nuinstance_s** out, s32 max);
```

## 7. Data Layout Sizes (for struct identity verification)

| Size | Likely Struct |
|------|---------------|
| 0x4 | nuanimcurve2_s, nunrand_s, numtlattrib_s |
| 0x8 | nudfnode_s, nuiffhdr_s, memexternal_s, filebuff_s |
| 0xC | nuvec_s, nucolour3_s, BlockInfo, nuangvec_s, nuivec_s |
| 0x10 | nudatinfo_s, nuanimkey_s, nuanimcurve_s, nuanimcurveset_s, nuanimdata_s, nuvec4_s, Quat, nurndritem_s, NuSndLoopInfo_s |
| 0x14 | nudatfile_s, numemfile_s |
| 0x18 | nuanimdata2_s, nuskin_s |
| 0x1C | nutex_s |
| 0x20 | NUNODE_s |
| 0x24 | nugeomitem_s |
| 0x28 | NUPOINTOFINTEREST_s |
| 0x30 | nudathdr_s, nugeom_s, nuscene_s |
| 0x34 | NUJOINTANIM_s, NUJOINTDATA_s |
| 0x3C | nuprim_s, nuscene_s |
| 0x40 | numtx_s, Mtx, NUBLENDGEOM_s |
| 0x50 | nuinstance_s, nuspecial_s |
| 0x54 | numtl_s |
| 0x58 | nulight_s |
| 0x60 | nucamera_s |
| 0x64 | nugobj_s |
| 0x74 | nugscn_s |
| 0x80 | NUHGOBJ_s |
| 0xF4 | nupad_s |

## 8. Next Steps

1. Verify struct layouts by checking LSW1 DOL data xrefs against known sizes
2. Write starter `.h` files with struct definitions (in `src/nucore/`, `src/numath/`, `src/nu3dx/`)
3. When decompiling individual functions, use this struct reference to name parameters
