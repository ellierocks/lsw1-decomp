# Nu2 Engine Reference — from Crash: Wrath of Cortex Decomp

## Source

Crash: Wrath of Cortex GameCube decomp: https://github.com/denzi-gh/crashwoc-decomp
Same engine (Nu2), same platform (GC), same compiler (Metrowerks).
Struct layouts, function names, and patterns transfer directly to LSW1.

## Naming Conventions

| Category | Convention | Example |
|----------|-----------|---------|
| Engine struct tag | `nu{name}_s` | `struct nufile_s`, `struct nuvec_s`, `struct nugeom_s` |
| Game struct tag | `NU{NAME}_s` or `struct {Name}_s` | `NUHGOBJ_s`, `struct creature_s` |
| Struct fields | `camelCase` | `node_name`, `numkeys`, `blend_time` |
| Engine functions | `Nu{Subsystem}{Verb}` | `NuFileOpen`, `NuMemAlloc`, `NuVecMtxTransform` |
| Basic types | lowercase short names | `u8`/`s8`/`u16`/`s16`/`u32`/`s32`/`f32`/`f64` |
| Angle type | `u16` (`angle`) | Range [0, 0x10000) |
| Fixed point | `s32` (`fxi`) | Signed int fixed-point |
| Error strings | `str_FunctionName_Description` | `str_NuFile_OpenFailed` |
| Static locals | plain `static type name;` | No `s_`/`l_` prefix |
| SDK functions | direct prototype decl | `void* memset(void* s, int c, unsigned int n);` |

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

### `nusound/` — Sound

```c
struct NuSndLoopInfo_s { nuvec_s* pos; s16 playing, channel, timer, pad, vol_l, vol_r; }; // 0x10
```

## LSW1 Nu* Symbol Mapping

| LSW1 Symbol | Crash Module | Notes |
|-------------|-------------|-------|
| `NuFileOpen` | `nucore/nufile.c` | Direct match |
| `NuFileClose` | `nucore/nufile.c` | Direct match |
| `NuFilePos` | `nucore/nufile.c` | Direct match |
| `NuFileLoadBuffer` | `nucore/nufile.c` | Direct match |
| `NuFileBeginBlkRead` | `nucore/nufile.c` | IFF chunk reading |
| `NuFileEndBlockRead` | `nucore/nufile.c` | IFF chunk reading |
| `NuDatFileOpen` | `nucore/nufile.c` | Direct match |
| `NuDatFileLoadBuffer` | `nucore/nufile.c` | Direct match |
| `NuDatFileLoadBufferLsn` | `nucore/nufile.c` | Direct match |
| `NuDatOpenEx` | `nucore/nufile.c` | Direct match |
| `NuMemAlloc` | `nucore/numem.c` | Direct match |
| `NuMemFree` | `nucore/numem.c` | Direct match |
| `NuPPLoadBuffer` | — | Packed-pixel loader (no crash equivalent) |
| `NuSpecialFind` | `gamecode/main.h` | Object lookup |
| `NuSpecialFindMulti` | `gamecode/main.h` | Multi-object lookup |
| `NuAnimCurve2CalcVal` | `nu3dx/nuanim.c` | Direct match |
| `NuRndrWaterLevel` | `nu3dx/nurndr.c` | Data symbol |
| `NuRndrHackWaterLevel` | `nu3dx/nurndr.c` | Data symbol |

## Matching Style

- **K&R function redeclaration**: `void* malloc_x();` — generates crclr
- **SDK wrapper ifdefs**: PS2 VU0 code wrapped in `#ifdef __MWERKS__`
- **`ASSERTMSGLINE`**: defined as empty for matching
- **String naming**: `str_FunctionName_Description` in `.rodata`
- **Global declarations**: guarded with `#ifndef FIRST` / `#define FIRST 0` across .c files
- **Include style**: Direct paths — `#include "nucore/nufile.h"`
