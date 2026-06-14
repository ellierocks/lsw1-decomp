# Context for fn_80023C70

## Module
`nuanim`

## Types
// From types.h
#ifndef TYPES_H
#define TYPES_H

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef float f32;
typedef double f64;
typedef unsigned short angle;
typedef signed int fxi;
typedef s32 fileHandle;
typedef int UNKWORD;
typedef void UNKTYPE;

#endif

// From nu3dx/nuanim.h
#ifndef NUAANIM_H
#define NUAANIM_H

#include "types.h"
#include "numath/numath.h"

struct nuanimkey_s {
    f32 time;
    f32 dtime;
    f32 c;
    f32 d;
};

struct nuanimcurve_s {
    u32 mask;
    struct nuanimkey_s* animkeys;
    u32 numkeys;
    u32 flags;
};

struct nuanimcurveset_s {
    s32 flags;
    f32* constants;
    struct nuanimcurve_s** set;
    char ncurves;
};

struct nuanimdata_s {
    f32 time;
    char* node_name;
    s32 nchunks;
    struct nuanimdatachunk_s** chunks;
};

struct nuanimdatachunk_s {
    s32 numnodes;
    s32 num_valid_animcurvesets;
    struct nuanimcurveset_s** animcurvesets;
    struct nuanimkey_s* keys;
    struct nuanimcurve_s* curves;
};

struct nuanimtime_s {
    f32 time;
    f32 time_offset;
    s32 chunk;
    u32 time_mask;
    u32 time_byte;
};

struct nuanimdata2_s {
    f32 endframe;
    s16 nnodes;
    s16 ncurves;
    s16 nchunks;
    struct nuanimcurve2_s* curves;
    char* curveflags;
    char* curvesetflags;
};

struct nuanimcurve2_s {
    union {
        f32 constant;
        struct nuanimcurvedata_s* curvedata;
    } data;
};

struct nuanimcurvedata_s {
    u32* mask;
    u16* key_ixs;
    void* key_array;
};

struct NUJOINTANIM_s {
    f32 rx, ry, rz;
    f32 tx, ty, tz;
    f32 sx, sy, sz;
    s16 max_rx, max_ry, max_rz;
    s16 min_rx, min_ry, min_rz;
    u8 joint_id;
    u8 flags;
};

void NuAnimCurve2CalcVal(struct nuanimcurve2_s* curves, f32 time, void* result);

#endif

// From nu3dx/nu3dx.h
#ifndef NU3DX_H
#define NU3DX_H

#include "types.h"
#include "numath/numath.h"

// ----- Enums -----

enum gobjtype_s {
    NUGOBJ_MESH = 0,
    NUGOBJ_FACEON = 1
};

enum nutextype_e {
    NUTEX_RGB16 = 0,
    NUTEX_RGBA16 = 1,
    NUTEX_RGB24 = 2,
    NUTEX_RGBA32 = 3,
    NUTEX_INTENSITY4 = 4,
    NUTEX_INTENSITY8 = 5,
    NUTEX_INTENSITYA4 = 6,
    NUTEX_INTENSITYA8 = 7,
    NUTEX_PAL4 = 8,
    NUTEX_PAL8 = 9,
    NUTEX_PAL4_A4 = 10,
    NUTEX_PAL8_A4 = 11,
    NUTEX_RZT32 = 23,
    NUTEX_NUMTEXTURETYPES = 26
};

enum nurndritemtype_s {
    NURNDRITEM_GEOM3D = 0,
    NURNDRITEM_GEOM2D = 1,
    NURNDRITEM_SKIN3D2 = 2,
    NURNDRITEM_GEOMFACE = 3
};

enum nuprimtype_e {
    NUPT_POINT = 0,
    NUPT_LINE = 1,
    NUPT_TRI = 2,
    NUPT_TRISTRIP = 3,
    NUPT_NDXTRI = 5,
    NUPT_NDXTRISTRIP = 6,
    NUPT_FACEON = 9,
    NUPT_QUADLIST = 10
};

enum nuvtxtype_e {
    NUVT_PS = 0x11,
    NUVT_LC1 = 0x51,
    NUVT_TLTC1 = 0x53,
    NUVT_SK3TC1 = 0x5D,
    NUVT_TC1 = 0x59
};

enum nufaceontype_s {
    NUFACEON_FACEY = 1,
    NUFACEON_FACEON = 0
};

// ----- Vertex types -----

struct nuvtx_tc1_s {
    struct nuvec_s pnt, nrm;
    s32 diffuse;
    f32 tc[2];
};

struct nuvtx_lc1_s {
    struct nuvec_s pnt;
    s32 diffuse;
    f32 tc[2];
};

struct nuvtx_ps_s {
    struct nuvec_s pnt;
    s32 diffuse;
};

struct nuvtx_sk3tc1_s {
    struct nuvec_s pnt;
    f32 weights[2];
    f32 indexes[3];
    struct nuvec_s nrm;
    s32 diffuse;
    f32 tc[2];
};

// ----- Render items -----

struct nurndritem_s {
    struct nurndritem_s* next;
    enum nurndritemtype_s type;
    s32 flags;
    s16 lights_index;
};

// ----- Material -----

struct numtlattrib_s {
    u32 alpha : 2;
    u32 filter : 2;
    u32 fx : 2;
    u32 utc : 2;
    u32 vtc : 2;
    u32 cull : 2;
    u32 zmode : 2;
    u32 lighting : 2;
    u32 colour : 1;
    u32 fill : 1;
    u32 atst : 3;
    u32 aref : 8;
    u32 afail : 2;
    u32 uvmode : 1;
};

struct numtl_s {
    struct numtl_s* next;
    struct numtlattrib_s attrib;
    struct nucolour3_s ambient, diffuse;
    union { u32 words[4]; } fx1, fx2, fx3, fx4;
    f32 power, alpha;
    s32 tid;
    s16 alpha_sort;
    u8 fxid, special_id;
    s16 K;
    u8 L;
    u8 uanmmode : 4, vanmmode : 4;
    f32 du, dv, su, sv;
};

// ----- Texture -----

struct nutex_s {
    enum nutextype_e type;
    s32 width, height, mmcnt;
    void* bits;
    s32* pal;
    s16 decal, linear;
};

// ----- Primitive -----

struct nuprim_s {
    struct nuprim_s* next;
    enum nuprimtype_e type;
    u16 cnt, max;
    u16* vid;
    void* pln;
    s32 idxbuff, cached;
    s16 skinmtxlookup[16];
};

// ----- Geometry -----

struct nugeom_s {
    struct nugeom_s* next;
    struct numtl_s* mtl;
    s32 mtl_id;
    enum nuvtxtype_e vtxtype;
    s32 vtxcnt, vtxmax, hVB;
    void* basisvbptr;
    struct nuprim_s* prim;
    struct nuskin_s* skin;
    struct NUVTXSKININFO_s* vtxskininfo;
    struct NUBLENDGEOM_s* blendgeom;
};

struct nugeomitem_s {
    struct nurndritem_s hdr;
    struct numtx_s* mtx;
    struct nugeom_s* geom;
    f32** blendvals;
    s16 instancelights_index[3];
    s16 hShader;
};

// ----- Skinning -----

struct nuskin_s {
    struct nuskin_s* next;
    s32 vtxoffset, vtxcnt, mtxcnt;
    s32* mtxid;
    f32* weights;
};

struct NUBLENDGEOM_s {
    s32 nblends;
    struct nuvec_s** blend_offsets;
    s32* ix;
    struct nuvec_s* offsets;
    struct nuvec_s* ooffsets;
    s32 hVB;
    s32 blendindex[10];
};

// ----- Game Object -----

struct nugobj_s {
    struct nugobj_s* sysnext;
    struct nugobj_s* syslast;
    enum gobjtype_s type;
    struct nugeom_s* geom;
    void* faceon_geom;
    f32 bounding_radius_from_origin;
    f32 bounding_rsq_from_origin;
    struct nuvec_s bounding_box_min, max, center;
    f32 bounding_radius_from_center;
    f32 bounding_rsq_from_center;
    s32 ngobjs;
    struct nugobj_s* next_gobj;
    struct nuvec_s origin;
    s32 ignore, culltype;
};

// ----- Lights -----

struct nulight_s {
    struct nucolour3_s ambient, diffuse;
    struct numtx_s mtx;
};

// ----- Camera -----

struct nucamera_s {
    struct numtx_s mtx;
    f32 fov, aspect, nearclip, farclip, portalnearclip;
    struct nuvec_s scale;
};

// ----- Scene graph -----

struct NUNODE_s {
    char* name;
    u32 id, type;
    s32 nchildren;
    struct NUNODE_s *parent, *child, *next;
    void* data;
};

struct nuinstance_s {
    struct numtx_s mtx;
    s32 objid;
    s32 flags;
    void* anim;
    s16 room_group;
    char special_flag;
};

struct nuspecial_s {
    struct numtx_s mtx;
    struct nuinstance_s* instance;
    char* name;
    s32 flags;
};

struct nuscene_s {
    s32 nnodes;
    char** names;
    struct NUNODE_s** nodes;
    char* strings;
    s32 numtids;
    s16* tids;
    s32 nummtls;
    struct numtl_s** mtls;
    s32 numgobjs;
    struct nugobj_s** gobjs;
    s32 numsplines;
    void* splines;
    struct nuvec_s* spline_cvs;
    struct NUNODE_s* root_node;
    struct nugscn_s* gscene;
};

struct nugscn_s {
    s16* tids;
    s32 numtid;
    struct numtl_s** mtls;
    s32 nummtl;
    s32 numgobj;
    struct nugobj_s** gobjs;
    s32 numinstance;
    struct nuinstance_s* instances;
    s32 numspecial;
    struct nuspecial_s* specials;
    void* splinedata;
    s32 numsplines;
    void* splines;
    char* nametable;
    s32 numexspecials;
    struct nuspecial_s* exspecials;
    s32 exallocix;
    void* instanimblock;
    void** instanimdata;
    s32 numinstanims, numinstanceixs;
    s16* instanceixs;
    s16 crossfade, crossfaderate;
    s32 crossfadegobj, crossfadefirst;
    s32 numtexanims;
    void* texanims;
    s16* texanim_tids;
    s16* instancelightix;
};

// ----- Hierarchical GO -----

struct NUJOINTDATA_s {
    struct numtx_s orient;
    struct nuvec_s locator_offset;
    char* name;
    s32 parent_ix;
    char flags;
};

struct NULAYERDATA_s {
    char* name;
    struct nugobj_s** gobjs;
    struct nugobj_s* skin_gobj;
    struct nugobj_s** blend_gobjs;
    struct nugobj_s* blend_skin_gobj;
};

struct NUPOINTOFINTEREST_s {
    struct numtx_s offset;
    char* name;
    u8 parent_joint_ix;
};

struct NUHGOBJ_s {
    s16* tids;
    s32 numtid;
    struct numtl_s** mtls;
    s32 nummtl;
    struct NUJOINTDATA_s* joints;
    struct numtx_s* T;
    struct numtx_s* INV_WT;
    u8* joint_ixs;
    struct NULAYERDATA_s* layers;
    struct NUPOINTOFINTEREST_s* points_of_interest;
    u8* poi_ixs;
    char* string_table;
    s32 string_table_size;
    f32 tbrdist;
    void* shadow_data;
    f32 sphere_radius, sphere_yoff;
    struct nuvec_s min, max;
    f32 cylinder_yoff, cylinder_height, cylinder_radius;
    void* collision_data;
    s32 numtexanims;
    void* texanims;
    s16* texanim_tids;
    u8 num_joints, num_joint_ixs, num_layers, num_points_of_interest, num_poi_ixs, shadowoff;
};

// ----- Functions -----

struct nuinstance_s* NuSpecialFind(struct nuscene_s* scene, char* name);
s32 NuSpecialFindMulti(struct nuscene_s* scene, char* name, struct nuinstance_s** out, s32 max);

#endif


## Compilation
- Compiler: Metrowerks CodeWarrior for GameCube (mwcceppc.exe)
- CPU: PPC 750CL (Gekko)
- Calling convention: PPC EABI (r3–r10 params, r3 return,
  r13–r31 callee-saved, stack frames aligned to 8 bytes)
- ABI: No full IEEE denormals; `frsqrte` for reciprocal sqrt
