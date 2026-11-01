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

// GC layout (0x11C), confirmed against auto/nu3d/ngc/numtl. The CrashWOC
// (0x54) and OpenSaga PC layouts do not hold: `next` is at 0xB4.
struct numtl_s {
    u8 unknown00[0xA4];
    u32 version : 16;
    u32 used : 1;
    u32 unknownA6Flags : 7;
    u32 renderPlane : 8;
    u8 unknownA8[0xC];
    struct numtl_s* next;
    u8 unknownB8;
    u8 unknownB9;
    u8 unknownBA[3];
    u8 unknownBD;
    u8 unknownBE;
    u8 unknownBF;
    u8 unknownC0;
    u8 unknownC1;
    u8 unknownC2;
    u8 unknownC3[4];
    u8 unknownC7;
    u8 unknownC8[8];
    f32 unknownD0[3];
    u8 unknownDC[0x14];
    f32 unknownF0;
    s32 tid;
    u8 unknownF8[4];
    f32 unknownFC[4];
    u8 unknown10C[0x10];
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

// GC: only `next` and the two freed buffers at 0x10/0x14 are confirmed
// (geometry destroy path); the donor puts `vid` at 0xC and `pln` at 0x10.
struct nuprim_s {
    struct nuprim_s* next;
    u8 unknown04[0xC];
    void* unknown10;
    void* unknown14;
};

// ----- Geometry -----

// GC layout, confirmed by the geometry destroy path (fn_80024704, NuGobjInit).
// The donor's 0x30 layout is 0x1C bytes short here.
struct nugeom_s {
    struct nugeom_s* next;
    u8 unknown04[0x24];
    void* unknown28;
    struct nuprim_s* prim;
    struct nuskin_s* skin;
    void* vtxskininfo;
    u8 unknown38[0x10];
    struct NUBLENDGEOM_s* blendgeom;
};

struct nufaceongeom_s {
    struct nufaceongeom_s* next;
    u8 unknown04[0xC];
    void* unknown10;
    u8 unknown14[8];
    void* unknown1C;
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
    struct nufaceongeom_s* faceonGeom;
    f32 boundingRadiusFromOrigin;
    u8 unknown18[4];
    struct nuvec_s boundingBoxMin;
    struct nuvec_s boundingBoxMax;
    struct nuvec_s boundingBoxCenter;
    f32 boundingRadiusFromCenter;
    u8 unknown44[8];
    struct nugobj_s* nextGobj;
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

struct nucamstate_s {
    struct numtx_s mtx;
    f32 fov;
    f32 aspect;
    f32 nearclip;
    f32 farclip;
    f32 nearclip_alt;
    f32 farclip_alt;
    f32 unk58;
    struct nuvec_s pos;
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
    s16 objid;
    u8 unknown42[2];
    u32 visible : 1;
    u32 onScreen : 1;
    u32 unknownFlag2 : 1;
    u32 noVisiTest : 1;
    u32 unknownFlags4 : 28;
    struct numtx_s* animMtx;
    u8 unknown4C[4];
};

struct nuspecial_s {
    struct numtx_s mtx;
    struct nuinstance_s* instance;
    char* name;
    u8 unknown48[8];
};

#define NUDISPLAYSPECIAL_VISIBLE 0x2
#define NUDISPLAYSPECIAL_NOVISITEST 0x80

struct nudisplayspecial_s {
    struct numtx_s mtx;
    struct numtx_s drawMtx;
    struct nuvec_s boundsMin;
    u8 unknown8C[4];
    struct nuvec_s boundsMax;
    u8 unknown9C[4];
    struct nuvec_s center;
    f32 radius;
    u8 unknownB0[4];
    char* name;
    u32 flags;
    u8 unknownBC[4];
    s32 instanceIx;
    struct numtx_s* animMtx;
    u8 unknownC8[8];
};

struct nudisplayscene_s {
    u8 unknown00[0x6C];
    s32 numSpecials;
    struct nudisplayspecial_s* specials;
};

struct nuhspecial_s {
    struct nugscn_s* scene;
    struct nuspecial_s* special;
    struct nudisplayspecial_s* display;
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

// GC: tids/numtid/mtls/nummtl/texanims confirmed by auto/nu3d/ngc/nugscn
// (material load and scene free); a scene tid entry is a word, not the
// donor's s16.
struct nugscn_s {
    s32* tids;
    s32 numtid;
    struct numtl_s** mtls;
    s32 nummtl;
    u8 unknown10[4];
    struct nugobj_s** gobjs;
    s32 numInstance;
    struct nuinstance_s* instances;
    s32 numSpecial;
    struct nuspecial_s* specials;
    u8 unknown28[0x58];
    void* texanims;
    u8 unknown84[0x11C];
    struct nudisplayscene_s* displayScene;
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

#endif
