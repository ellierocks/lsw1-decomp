# Nu2 Engine Reference — from Crash: Wrath of Cortex Decomp

Confirmed OpenSaga GC parser fields live in
[`nucore/nufpar.h`](../src/nucore/nufpar.h); see the
[layout evidence and limits](symbol_donors/opensaga_matching.md#shared-gc-layouts).

Game-side character, creature, API-object and AI packet layouts are documented
in [LSW1 game struct headers](game_structs.md).

## Source

Crash: Wrath of Cortex GameCube decomp: https://github.com/denzi-gh/crashwoc-decomp
Same engine (Nu2), same platform (GC), but a **different compiler**: CrashWOC is
Metrowerks CodeWarrior, whereas LSW1 is SN Systems ProDG (GCC). Struct layouts, names and algorithms are hypotheses for LSW1. Check field
offsets and behavior against the target assembly before using them in C.
The declarations below are layout sketches, not a header to copy; the owning
project types are in `src/` headers.

## Subsystem Boundaries

| Directory | Contents |
|-----------|----------|
| `nucore/` | File I/O, memory allocator, error handling, text parser, core types |
| `numath/` | Vectors (3D+4D), 4x4 matrices, quaternions, planes, trig table, random, float helpers |
| `nu3dx/` | Scene graph, geometry/objects, materials, textures, animation, lights, cameras, hierarchical objects (skeletal), rendering pipeline, viewports, glass/water/haze, skinning |
| `nusound/` | 3D positional audio, looping, streaming, banks |
| `gamelib/` | Game bridge: wind, terrain collision, debris/particles, editor objects, cutscenes, SFX |
| `gamecode/` | Game-specific: characters, game state/save, camera rails, level data |
| `system/` | Platform SDK wrappers (gc/, gs/, ss/) |

## Struct Definitions

### `nucore/` — Core Types

```c
struct nunrand_s { long idum; };                        // 0x4
struct nucolour3_s { f32 r, g, b; };                    // 0xC
struct memexternal_s { union variptr_u* ptr, *end; };   // 0x8
struct nudatinfo_s { s32 foffset, flen, uplen; s32 ppack:1; }; // 0x10
struct nudfnode_s { short childix, sibix; char* txt; }; // 0x8
struct nudathdr_s { s32 ver, nfiles; nudatinfo_s* finfo; s32 treecnt; nudfnode_s* filetree; s32 leafnamesize; char* leafnames; s32 dfhandle, fh; short intalloc, openmode; s32 start_lsn; void* memdatptr; }; // 0x30
struct nudatfile_s { nudathdr_s* ndh; s32 start, len, fix, used; }; // 0x14
struct numemfile_s { char *start, *end, *curr; enum nufilemode_e mode; s32 used; }; // 0x14
struct nuiffhdr_s { s32 blk, size; };                   // 0x8
struct BlockInfo { nuiffhdr_s hdr; s32 pos; };          // 0xC
struct nufpcomjmp_s { char* fname; void (*func)(nufpar_s*); }; // 0x8
struct nufpar_s { s32 handle; char fbuff[4096], lbuff[257], wbuff[259]; s32 line_num, line_pos, cpos, buffstart, buffend; nufpcomjmp_s* comstack[8]; s32 compos, size; };
```

### `numath/` — Math

```c
struct nuvec_s { f32 x, y, z; };                        // 0xC
struct nuvec4_s { f32 x, y, z, w; };                    // 0x10
struct nuivec_s { int x, y, z; };                       // 0xC
struct nuivec4_s { int x, y, z, w; };                   // 0x10
struct numtx_s { f32 _00,_01,_02,_03, _10,_11,_12,_13, _20,_21,_22,_23, _30,_31,_32,_33; }; // 0x40
struct Mtx { f32 m11..m44; };                           // 0x40
struct Quat { float x,y,z,w; };                         // 0x10
struct nuangvec_s { int x, y, z; };                     // 0xC
```

### `nu3dx/` — 3D Engine (most important)

**Scene graph:**
```c
struct NUNODE_s { char* name; unsigned int id, type; int nchildren; struct NUNODE_s *parent, *child, *next; void* data; }; // 0x20
struct nuscene_s { int nnodes; char** names; NUNODE_s** nodes; char* strings; int numtids; short* tids; int nummtls; numtl_s** mtls; int numgobjs; nugobj_s** gobjs; int numsplines; nuspline_s* splines; nuvec_s* spline_cvs; NUNODE_s* root_node; nugscn_s* gscene; }; // 0x3C
struct nugscn_s { short* tids; int numtid; numtl_s** mtls; int nummtl; int numgobj; nugobj_s** gobjs; int numinstance; nuinstance_s* instances; int numspecial; nuspecial_s* specials; void* splinedata; int numsplines; nugspline_s* splines; char* nametable; int numexspecials; nuspecial_s* exspecials; int exallocix; nuinstanim_s* instanimblock; nuanimdata_s** instanimdata; int numinstanims, numinstanceixs; short* instanceixs; short crossfade, crossfaderate; int crossfadegobj, crossfadefirst; int numtexanims; nutexanim_s* texanims; short* texanim_tids; short* instancelightix; }; // 0x74
struct nuinstance_s { numtx_s mtx; int objid; nuinstflags_s flags; nuinstanim_s* anim; short room_group; char special_flag; }; // 0x50
struct nuspecial_s { numtx_s mtx; nuinstance_s* instance; char* name; nuspecialflags flags; }; // 0x50
```

**Geometry:**
```c
struct nugeom_s { nugeom_s* next; numtl_s* mtl; s32 mtl_id; enum nuvtxtype_e vtxtype; s32 vtxcnt, vtxmax, hVB; void* basisvbptr; nuprim_s* prim; nuskin_s* skin; NUVTXSKININFO_s* vtxskininfo; NUBLENDGEOM_s* blendgeom; }; // 0x30
struct nuprim_s { nuprim_s* next; enum nuprimtype_e type; unsigned short cnt, max; unsigned short* vid; nuplane_s* pln; int idxbuff, cached; short skinmtxlookup[16]; }; // 0x3C
struct nugeomitem_s { nurndritem_s hdr; numtx_s* mtx; nugeom_s* geom; f32** blendvals; s16 instancelights_index[3]; s16 hShader; }; // 0x24
struct nuskin_s { nuskin_s* next; int vtxoffset, vtxcnt, mtxcnt; int* mtxid; float* weights; }; // 0x18
struct NUBLENDGEOM_s { int nblends; nuvec_s** blend_offsets; int* ix; nuvec_s* offsets; nuvec_s* ooffsets; int hVB; int blendindex[10]; }; // 0x40
```

**Game object:**
```c
struct nugobj_s { nugobj_s *sysnext, *syslast; enum gobjtype_s type; nugeom_s* geom; nufaceongeom_s* faceon_geom; float bounding_radius_from_origin, bounding_rsq_from_origin; nuvec_s bounding_box_min, max, center; float bounding_radius_from_center, bounding_rsq_from_center; int ngobjs; nugobj_s* next_gobj; nuvec_s origin; int ignore, culltype; }; // 0x64
struct NUHGOBJ_s { short* tids; int numtid; numtl_s** mtls; int nummtl; NUJOINTDATA_s* joints; numtx_s* T; numtx_s* INV_WT; unsigned char* joint_ixs; NULAYERDATA_s* layers; NUPOINTOFINTEREST_s* points_of_interest; unsigned char* poi_ixs; char* string_table; int string_table_size; float tbrdist; NUSHADOWDATA_s* shadow_data; float sphere_radius, sphere_yoff; nuvec_s min, max; float cylinder_yoff, cylinder_height, cylinder_radius; NUCOLLISIONDATA_s* collision_data; int numtexanims; nutexanim_s* texanims; short* texanim_tids; unsigned char num_joints, num_joint_ixs, num_layers, num_points_of_interest, num_poi_ixs, shadowoff; }; // 0x80
```

**Material:**
```c
struct numtlattrib_s { unsigned int alpha:2, filter:2, fx:2, utc:2, vtc:2, cull:2, zmode:2, lighting:2, colour:1, fill:1, atst:3, aref:8, afail:2, uvmode:1; }; // 0x4
struct numtl_s { numtl_s* next; numtlattrib_s attrib; nucolour3_s ambient, diffuse; union nufx_u fx1,fx2,fx3,fx4; f32 power, alpha; s32 tid; s16 alpha_sort; u8 fxid, special_id; s16 K; u8 L; u8 uanmmode:4, vanmmode:4; f32 du, dv, su, sv; }; // 0x54
```

**Texture:**
```c
struct nutex_s { enum nutextype_e type; int width, height, mmcnt; void* bits; int* pal; short decal, linear; }; // 0x1C
```

**Camera:**
```c
struct nucamera_s { numtx_s mtx; f32 fov, aspect, nearclip, farclip, portalnearclip; nuvec_s scale; }; // 0x60
```

**Light:**
```c
struct nulight_s { nucolour3_s ambient, diffuse; numtx_s mtx; }; // 0x58
```

**Animation:**
```c
struct nuanimkey_s { f32 time, dtime, c, d; };               // 0x10
struct nuanimcurve_s { u32 mask; nuanimkey_s* animkeys; u32 numkeys, flags; }; // 0x10
struct nuanimcurveset_s { s32 flags; f32* constants; nuanimcurve_s** set; char ncurves; }; // 0x10
struct nuanimdata_s { float time; char* node_name; int nchunks; nuanimdatachunk_s** chunks; }; // 0x10
struct nuanimdata2_s { f32 endframe; s16 nnodes, ncurves, nchunks; nuanimcurve2_s* curves; char* curveflags; char* curvesetflags; }; // 0x18
struct nuanimcurve2_s { union { float constant; nuanimcurvedata_s* curvedata; } data; }; // 0x4
struct NUJOINTANIM_s { float rx,ry,rz, tx,ty,tz, sx,sy,sz; short max_rx, max_ry, max_rz, min_rx, min_ry, min_rz; unsigned char joint_id, flags; }; // 0x34
```

**Render pipeline:**
```c
struct nurndritem_s { nurndritem_s* next; enum nurndritemtype_s type; s32 flags; s16 lights_index; }; // 0x10
```

**Vertex types:**
```c
struct nuvtx_tc1_s { nuvec_s pnt, nrm; int diffuse; float tc[2]; };      // 0x24
struct nuvtx_sk3tc1_s { nuvec_s pnt; float weights[2]; float indexes[3]; nuvec_s nrm; int diffuse; float tc[2]; }; // 0x38
```

### LSW1-confirmed `nu3dx` layouts

These layouts are in [`nu3dx/nu3dx.h`](../src/nu3dx/nu3dx.h). They were confirmed
against the 24 accessors in `auto/nu3d/nuspecial` (`0x80012C44..0x80013900`). Each
kept field has an LSW1 load or store of the stated width. Unconfirmed ranges are
`unknownXX` padding. Several CrashWOC/OpenSaga offsets do not hold on GC: the donor
`nugscn_s` puts `gobjs` at 0x14 too, but the special and display fields differ.

| Struct | Size | Field (offset, access) | Evidence |
|---|---|---|---|
| `nugscn_s` | ≥ 0x1A4 | `gobjs` 0x14 `lwz`; `numInstance` 0x18; `instances` 0x1C (0x50 stride); `numSpecial` 0x20; `specials` 0x24; `displayScene` 0x1A0 `lwz` | `NuSpecialFind`, `NuSpecialFindMulti`, `fn_800135B0` |
| `nuinstance_s` | 0x50 | `mtx` 0x00; `objid` 0x40 `lha`; flag word 0x44: `visible` bit 31, `onScreen` bit 30, `noVisiTest` bit 28; `animMtx` 0x48 | `fn_800131B8`..`fn_80013310`, `fn_80013380`, `fn_80013618` |
| `nuspecial_s` | 0x50 | `mtx` 0x00; `instance` 0x40; `name` 0x44 | `NuSpecialFind*`, `fn_80013350`, `NuSpecialGetInstAnim` |
| `nudisplayscene_s` | ≥ 0x74 | `numSpecials` 0x6C; `specials` 0x70 (0xD0 stride) | `NuSpecialFind*` |
| `nudisplayspecial_s` | 0xD0 | `mtx` 0x00; `drawMtx` 0x40; `boundsMin` 0x80; `boundsMax` 0x90; `center` 0xA0; `radius` 0xAC `lfs`; `name` 0xB4; `flags` 0xB8 (0x2 visible, 0x80 no visibility test); `instanceIx` 0xC0; `animMtx` 0xC4 (NULL or -1 means none) | `fn_800133FC`, `fn_80013454`, `fn_800136CC`, `fn_800137D0`, `fn_800135B0` |
| `nuhspecial_s` | 0xC | `scene` 0x0; `special` 0x4; `display` 0x8 | every accessor; set by `NuSpecialFind*` |
| `nugobj_s` | ≥ 0x50 | `boundingRadiusFromOrigin` 0x14; `boundingBoxMin/Max/Center` 0x1C/0x28/0x34; `boundingRadiusFromCenter` 0x40; `nextGobj` 0x4C | `fn_80013618`, `fn_800136CC`, `fn_80013764` |

`nuhspecial_s` is a handle: exactly one of `special` (legacy scene) and `display`
(display scene) is set. In the legacy path, `NuSpecialFindMulti` stores a display
pointer counted from NULL in 0xD0 steps when `displayScene` is set; the donor stores
NULL. Two `symbols.txt` names are wrong by body: `NuSpecialFindMultiWC` is DrawAt and
`NuSpecialGetInstAnim` is GetName. They are not renamed yet.

### LSW1-confirmed engine layouts (batch 2)

The second shared-header batch confirmed these engine structs. The material, geometry and
scene layouts are in [`nu3dx/nu3dx.h`](../src/nu3dx/nu3dx.h). The animation layouts were
already in [`nu3dx/nuanim.h`](../src/nu3dx/nuanim.h), and this batch confirmed them
unchanged. The file layouts are in [`nucore/nufile.h`](../src/nucore/nufile.h). Every kept
field has an LSW1 access of the stated width in a strictly matched function. As in the
first batch, the CrashWOC (PS2) layouts for `numtl_s` and `nugeom_s` do not hold on GC,
and neither does the OpenSaga (PC) layout for `numtl_s`.

| Struct | Size | Field (offset, access) | Evidence |
|---|---|---|---|
| `numtl_s` | 0x11C | bitfield word 0xA4: `version` u16 (`lhz`/`sth`), `used` bit 0x8000 (`lwz`/`ori`, cleared with `rlwinm 0,17,15`), `renderPlane` byte 0xA7 (`stb`); `next` 0xB4; bytes 0xB8, 0xB9, 0xBD..0xC0, 0xC2, 0xC7; f32 0xD0..0xD8, 0xF0, 0xFC..0x108; `tid` 0xF4 | `fn_80024A34` (pool of 100 at `lbl_8020E9B4`, `memset` 0x11C), `fn_800248CC`, `fn_80024C64`, `fn_80024CD4`, `fn_800282E8` |
| `nugeom_s` | ≥ 0x4C | `next` 0x00; freed buffer 0x28; `prim` 0x2C (list); `skin` 0x30 (list); `vtxskininfo` 0x34; `blendgeom` 0x48 | `fn_80024704`, `NuGobjInit` |
| `nuprim_s` | ≥ 0x18 | `next` 0x00; freed buffers 0x10, 0x14 | `fn_80024854` |
| `nuskin_s` | 0x18 | donor layout holds: `next` 0x00, `mtxid` 0x10, `weights` 0x14 | `fn_80024704` |
| `NUBLENDGEOM_s` | — | donor layout holds: `blend_offsets` 0x4, `ix` 0x8, `offsets` 0xC | `NuGobjInit` |
| `nufaceongeom_s` | ≥ 0x20 | `next` 0x00; freed buffers 0x10, 0x1C | `NuGobjInit`, `fn_800247FC` |
| `nugobj_s` | ≥ 0x50 | adds `sysnext` 0x00, `syslast` 0x04 (list head `lbl_80407D0C`), `geom` 0x0C, `faceonGeom` 0x10 | `NuGobjInit`, `NuGobjClose` |
| `nugscn_s` | ≥ 0x1A4 | adds `tids` 0x00 (**words**, not the donor's `s16`), `numtid` 0x04, `mtls` 0x08, `nummtl` 0x0C, `texanims` 0x80 | `fn_800282E8`, `fn_8002A3A8` |
| `nuanimdata2_s` | — | `endframe` 0x0 `lfs`; `nnodes` 0x4, `ncurves` 0x6, `nchunks` 0x8 `lha`; `curves` 0xC; `curveflags` 0x10; `curvesetflags` 0x14 | `fn_8001678C`, `NuAnimData2CalcTime`, `NuAnimData2LoadBuff` |
| `nuanimcurvedata_s` | 0xC | `mask` 0x0; `key_ixs` 0x4; `key_array` 0x8 | `fn_8001678C` (pointer relocation) |
| `nuanimtime_s` | 0x18 | `time` 0x0; `time_offset` 0x4; `chunk` 0x8; `time_mask` 0xC; `time_byte` 0x10 | `fn_80016944`, `NuAnimData2CalcTime` |
| `nuanimdata_s` / `nuanimdatachunk_s` / `nuanimcurveset_s` / `nuanimcurve_s` | 0x10 / 0x14 / 0x10 / 0x10 | `time` 0, `nchunks` 8, `chunks` 0xC; `numnodes` 0, `animcurvesets` 8; `constants` 4, `set` 8, `ncurves` 0xC `stb`; `animkeys` 4, `numkeys` 8 (key stride 0x10) | `NuAnimDataCreate`, `NuAnimDataChunkCreate`, `NuAnimCurveSetCreate`, `NuAnimCurveCreate` |
| `numemfile_s` | 0x14 | donor layout holds: `start` 0, `end` 4, `curr` 8 (`lwzx` with 0x14 stride) | `fn_800059A0` |
| `BlockInfo` | 0xC | `hdr.size` 0x4, `pos` 0x8 (block stack `lbl_801FD080`, depth `lbl_80407A40`) | `NuFileEndBlockRead` |

Some of these structs appear only through local views, because no shared header owns them
yet:

- **Timebar** (`nuanim_1`). The view is `vals[16][2]` at 0x4, `start[16]` at 0x84,
  `which[16]` at 0x144 and `active[16]` at 0x1C4 (bytes).
- **Animation file header** (`nuanim_2`). The header is `{version, base, data}`, and the
  loader rebases `data` by `*buf - base`.
- **ANI4 header** (`nu3dx/nuanim`). It has a `u16` frame count at 0xA (`lhz`).
- **File device** (`nufile_3`). The directory separator is a byte at 0x10.
- **Glass state** (`nugobj`). Two 0x40-byte blocks at `lbl_8020E934` and `lbl_8020E974`;
  `NuGlassClose` struct-copies the second into its argument.

`fn_80024A34` is the material allocator. It takes the first entry whose `used` bit is
clear, clears it, and sets the defaults listed above; it also sets the same
`renderPlane` that it passes to `fn_80024DB8`. `fn_800248CC` is the material-system
init and builds the three default materials (`lbl_80407D50`, `lbl_80407D4C` and
`lbl_80407D54`).

### `nusound/` — Sound

```c
struct NuSndLoopInfo_s { nuvec_s* pos; s16 playing, channel, timer, pad, vol_l, vol_r; }; // 0x10
```


## Library Boundaries

A first-pass map of likely game, runtime, and SDK regions for `GL5E4F`. The analysis is based on lightweight checks: `strings`, generated symbols, and section layout. Treat ranges as working hypotheses until validated against target code and split/link evidence.

### High-Level Text Layout

| Range | Confidence | Notes |
| --- | --- | --- |
| `.init:0x80003100-0x8000348C` | High | Startup/init section. Entry symbol is `__start` at `0x80003100`. |
| `.text:0x800034A0-0x80150F04` | Medium | Mostly game/engine code based on address order and game strings before the SDK string cluster. |
| `.text:0x80150F04-0x8015BF48` | Low | Problematic analyzer region. `dtk` over-extends control flow from `0x80151AFC`; keep this region isolated until manually reviewed. |
| `.text:0x8015BF48-0x8015BFC0` | Medium | Small bridge/thunk region immediately before recognized low-level PPC helpers. |
| `.text:0x8015BFC0-0x8018CAFC` | High | Dolphin SDK / runtime tail region. Confirmed by SDK release strings and named symbols. |

The last generated text function is `fn_8018C51C` with size `0x5E0`, ending at `0x8018CAFC`. `.rodata` begins at `0x8018CB00`.

### Confirmed SDK Symbols

| Symbol | Address | Notes |
| --- | --- | --- |
| `__start` | `.init:0x80003100` | DOL entry. Renamed from generated `fn_80003100`. |
| `PPCMfhid2` | `.text:0x8015BFC0` | Low-level PPC helper. Likely start of SDK/runtime block. |
| `PPCMthid2` | `.text:0x8015BFC8` | Low-level PPC helper. |
| `PPCMtwpar` | `.text:0x8015BFD0` | Low-level PPC helper. |
| `OSRegisterVersion` | `.text:0x8015CAA8` | OS version registration. |
| `OSRegisterResetFunction` | `.text:0x80160F14` | OS reset callback registration. |
| `__GXInitRevisionBits` | `.text:0x8017CA2C` | GX init code anchor. |
| `GXInit` | `.text:0x8017CBD0` | GX init. |
| `__GXInitGX` | `.text:0x8017D1D0` | GX init internals. |
| `GXInitFifoBase` | `.text:0x8017DC3C` | GX FIFO setup. |
| `GXSetCPUFifo` | `.text:0x8017DD24` | GX FIFO setup. |
| `GXSetGPFifo` | `.text:0x8017DE4C` | GX FIFO setup. |
| `__GXFifoInit` | `.text:0x8017E008` | GX FIFO setup. |
| `GXSetMisc` | `.text:0x8017F5F4` | GX misc state. |
| `__GXPEInit` | `.text:0x8017FDB0` | GX pixel engine init. |
| `GXInitTexCacheRegion` | `.text:0x80181A34` | GX texture cache setup. |
| `GXInitTlutRegion` | `.text:0x80181B28` | GX TLUT setup. |
| `__GXSetTmemConfig` | `.text:0x80181DEC` | GX texture memory setup. |
| `__GXSetIndirectMask` | `.text:0x80182628` | GX indirect state. |
| `__GXFlushTextureState` | `.text:0x80182658` | GX texture state flush. |

### SDK String Evidence

`strings -a orig/GL5E4F/sys/main.dol` shows these Dolphin SDK release markers:

| Library | Release marker |
| --- | --- |
| OS | `May 21 2004 09:28:09 (0x2301)` |
| EXI | `Apr  5 2004 04:14:14 (0x2301)` |
| SI | `Apr  5 2004 04:14:16 (0x2301)` |
| DVD | `Apr  5 2004 04:14:51 (0x2301)` |
| VI | `Apr  7 2004 04:13:59 (0x2301)` |
| PAD | `Apr  5 2004 04:14:49 (0x2301)` |
| AI | `Apr  5 2004 04:15:02 (0x2301)` |
| AR | `Apr  5 2004 04:15:03 (0x2301)` |
| ARQ | `Apr  5 2004 04:15:04 (0x2301)` |
| AX | `Apr  5 2004 04:15:05 (0x2301)` |
| DSP | `Apr  5 2004 04:15:32 (0x2301)` |
| CARD | `Apr  5 2004 04:15:35 (0x2301)` |
| GX | `Apr  5 2004 04:14:28 (0x2301)` |

### Problem Region

`dtk dol info` and the initial split reported:

```text
Control flow from 1:0x80151AFC hit known function 1:0x801510E8
Overlapping functions 1:0x80151AFC-1:0x8015BF48 -> 1:0x80152038
```

Current config skips CFA for:

```yaml
skip_cfa_ranges:

- start: .text:0x80151AFC
  end: .text:0x8015BF48
```

The generated symbols in this region are mostly labels, followed by short functions at `0x8015BF48` through `0x8015BF98`, then recognized PPC helpers at `0x8015BFC0`.


## File and datpack call evidence

String xrefs identified these symbol anchors (naming evidence, not C matches):

- `NuFileOpen` at `0x800035F0`
- `NuFilePos` at `0x800039BC`
- `NuFileClose` at `0x80003C34`
- `NuFileLoadBuffer` at `0x80003DBC`
- `NuFileBeginBlkRead` at `0x80004390`
- `NuDatFileLoadBufferLsn` at `0x800044A8`
- `NuDatFileLoadBuffer` at `0x80004630`
- `NuDatFileOpen` at `0x80004CF8`
- `NuDatOpenEx` at `0x80004F6C`
- `NuFileEndBlockRead` at `0x80005B1C`
- `NuMemAlloc` at `0x80006F74`
- `NuMemFree` at `0x8000726C`
- `NuPPLoadBuffer` at `0x800082A4`
- `NuSpecialFind` at `0x80012D00`
- `NuSpecialFindMulti` at `0x80012E94`

Control-flow evidence:

- `NuFileLoadBuffer` calls `NuDatFileLoadBuffer` only when the DAT context global is active.
- `NuDatFileLoadBuffer` first calls `NuDatFileLoadBufferLsn`; on success it returns that result directly.
- If the LSN helper fails, `NuDatFileLoadBuffer` calls `NuDatFileOpen`, performs seek/read retry loops, emits the `Unpacking` message, and returns the loaded byte count or `0`.


## Camera state boundary

- `lbl_80313268` is currently typed in `symbols.txt` as a `0x20` BSS object,
  but the camera-state cluster proves that boundary is too small:
  - `fn_80023C70` copies `0x68` bytes from `lbl_80313268`
  - `fn_80023CD0` allocates `0x68` bytes and initializes fields through `+0x64`
  - `fn_80023F80` reads floats from `lbl_80313268 + 0x48` and `+0x4C`
- Donor evidence in `docs/symbol_donors/mac_data_to_gc_verified.md`
  links the start of this region to `DoorExitCameraSplineName`.
- Working conclusion: `lbl_80313268` is the start of a larger
  camera-related state struct, with `DoorExitCameraSplineName` as an
  offset-0 field or prefix, not the whole object.
- Stronger boundary conclusion:
  - The live object spans `0x80313268..0x803132CF` (`0x68` bytes total)
  - The next independently referenced object begins at `0x803132D0`
  - Therefore `lbl_80313288`, `lbl_80313298`, `lbl_803132B0`, and
    `lbl_803132B4` are field-level artifacts inside the same larger state block
- Recovered shared type:
  - `struct nucamstate_s` in `src/nu3dx/nu3dx.h`
  - Layout:
    - `0x00..0x3F` `numtx_s mtx`
    - `0x40` `fov`
    - `0x44` `aspect`
    - `0x48` `nearclip`
    - `0x4C` `farclip`
    - `0x50` `nearclip_alt`
    - `0x54` `farclip_alt`
    - `0x58` `unk58`
    - `0x5C..0x67` `nuvec_s pos`
- Evidence for the field split:
  - `fn_8001C20C` passes `0x40/0x44/0x48/0x4C` as four scalar camera/projection
    parameters to `fn_800234A8`
  - `fn_8001C20C` compares `0x50/0x54` against zero and falls back to
    `0x48/0x4C`, indicating alternate clip-range fields
  - `fn_8001C20C` passes `state + 0x5C` by pointer into `fn_8000EA74`,
    so the tail is a 3-float vector, not padding


## Recovering types and boundaries

Use access width, offsets, allocation/copy size, callers and callees together.
A 12-byte stride suggests a vector; a matching donor size alone does not prove
a type. In particular, nearby generated data labels may be fields inside one
larger object, as the camera block demonstrates.

`ls1_match_plan.py <module>` supplies current symbol context. Confirm a proposed
split against `config/GL5E4F/splits.txt`, data xrefs and linked ranges. The
clustering output in [proposed splits](symbol_donors/proposed_splits.md) contains
catch-all groups and is a discovery aid, not an object map.
