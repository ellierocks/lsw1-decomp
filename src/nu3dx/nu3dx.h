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
    u8 unknownC8[4];
    void* unknownCC; // GC: fn_8002963C stores the display scene's per-material entry here
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
    s16 unknown42; // GC: lha in fn_8002933C, compared against the camera LOD level
    u32 visible : 1;
    u32 onScreen : 1;
    u32 unknownFlag2 : 1;
    u32 noVisiTest : 1;
    u32 unknownFlags4 : 28;
    struct numtx_s* animMtx;
    s16 unknown4C; // GC: lha in fn_8002933C, converted to float
    u8 unknown4E[2];
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
    void* unknownB0; // GC: self-relative in the file, fixed up by fn_8002963C
    char* name;
    u32 flags;
    void* unknownBC; // GC: self-relative in the file, fixed up by fn_8002963C
    s32 instanceIx;
    struct numtx_s* animMtx;
    u8 unknownC8[8];
};

struct nugscnbounds_s {
    s32 numa;
    s32 numb;
    void* a; // GC: 0x10-byte entries
    void* b; // GC: 0x20-byte entries
};

// GC: draw command in nudisplayscene_s.displist; the list ends at type 0x8E.
struct nudispcmd_s {
    u8 type;
    u8 unknown01[3];
    void* data; // file: offset from the command itself
};

struct nudispentry0C_s {
    u8 unknown00[4];
    void* unknown04;
    void* unknown08;
};

struct nudispentry24_s {
    u8 unknown00[4];
    void* unknown04;
    u8 unknown08[8];
    void* unknown10;
    u8 unknown14[8];
    void* unknown1C;
    u8 unknown20[4];
};

// GC: per-material entry from nudisplayscene_s.mtlentries; fn_8002963C
// fixes the pointers up and stores each in numtl_s.unknownCC.
struct nudispmtl_s {
    void* unknown00;
    u8 unknown04[4];
    void* unknown08;
    void* unknown0C;
    void* unknown10;
    void* unknown14; // copy of unknown10 after the fixup
    void* unknown18;
};

struct nudispsub7C_s {
    u8 unknown00[4];
    void* unknown04;
    u8 unknown08[8];
    void* unknown10;
};

// GC: layout from fn_8002963C, which turns every self-relative pointer
// (stored as an offset from the field itself) into an absolute one.
struct nudisplayscene_s {
    void* unknown00;
    u8 unknown04[4];
    struct nudispcmd_s* displist;
    u8 unknown0C[4];
    s32 num0C;
    struct nudispentry0C_s* entries0C;
    void* unknown18;
    void* unknown1C;
    void* unknown20;
    void* unknown24;
    void* unknown28;
    void* unknown2C;
    void* unknown30;
    void* unknown34;
    struct nugscnbounds_s bounds; // 0x38: copy of the scene's bounds
    void* unknown48;
    u32 nummtl;
    struct numtl_s** mtls;
    struct nudispmtl_s** mtlentries;
    void* unknown58;
    void* unknown5C;
    s32 num24;
    struct nudispentry24_s* entries24;
    u8 unknown68[4];
    s32 numSpecials;
    struct nudisplayspecial_s* specials;
    u8 unknown74[4];
    struct nugscn_s* scene; // GC: back-pointer set by fn_80029EC8
    struct nudispsub7C_s* unknown7C;
    void* unknown80;
    u8 unknown84[4];
    void* unknown88; // fixed up only when lbl_8040AD98 > 1
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

// GC: portal set layout from fn_80028B88, which reads each table and turns the
// stored indices into pointers. nuqfnt.c keeps its own view of the same data.
struct nuportal_s {
    struct nuvec4_s plane;
    struct nuvec_s* vertices; // file: index into nuportalinfo_s.verts
    u8 unknown14[0xC];
};

struct nuroom_s {
    s16* unknown00;        // file: index into nuportalinfo_s.indices
    struct nuvec4_s* planes; // file: index into nuportalinfo_s.planes
    s16* portalIndices;    // file: index into nuportalinfo_s.portalix
    u8 unknown0C[0xC];
};

struct nuportalinfo_s {
    s32 numportals;
    struct nuportal_s* portals;
    s32 numrooms;
    struct nuroom_s* rooms;
    s32 numindices;
    s16* indices;
    s32 numplanes;
    struct nuvec4_s* planes;
    s32 numverts;
    struct nuvec_s* verts;
    s32 numportalix;
    s16* portalix;
    u8 unknown30[0x48];
    s32 depth; // GC: fn_80028B88 sets 0x100
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
    u8 unknown28[4];
    s32 numvtxsets; // GC: fn_80028DAC
    struct nuvtxset_s* vtxsets; // GC: 0xC-byte entries built by fn_80028DAC
    char* strings; // GC: fn_80028444 adds it to the texanim name offsets
    u8 unknown38[0xC];
    s32 numinstanim; // GC: 0x44..0x4C set by fn_800286B4
    struct nuinstanim_s* instanims;
    struct numtx_s* instanim_mtx;
    struct nuanimdata_s** instanim_data; // GC: fn_80029050; NULL skips instance animation
    u8 unknown54[0x28];
    s32 numtexanims; // GC: fn_80028444
    struct nutexanim_s* texanims;
    u8 unknown84[4];
    struct nuportalinfo_s portals; // GC: read by fn_80028B88 ("Reading Portal set...")
    u8 unknown104[0x7C];
    struct nugscnbounds_s bounds; // GC: 0x180, filled by fn_80029EC8 ("Reading Bounds set...")
    u8 unknown190[8];
    s32 numtex; // GC: texture headers loaded by NuMtlRender (0x800294C8)
    struct nughgtex_s* texs;
    struct nudisplayscene_s* displayScene;
    s32 numnamed; // GC: 0x1A4..0x1AC filled by fn_80029EC8 ('HNIB'/'0NIB' blocks)
    char** names;
    void** named;
    u8 unknown1B0[0x18];
    struct nuportalinfo_s* portalinfo; // GC: fn_80028B88 points it at portals
    u8 unknown1CC[0x10]; // GC: fn_80029EC8 allocates and clears 0x1DC bytes
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
