#include "numath/numath.h"
#include "nu3dx/nu3dx.h"

struct nudlist_poi_s {
    struct numtx_s matrix;
    u8 pad40[4];
    u8 joint;
    u8 pad45[0xb];
};

struct nudlist_hgobj_s {
    u8 pad0[0x28];
    struct nudlist_poi_s* pois;
    u8* poiLookup;
};

void fn_8000EB44(struct numtx_s*, struct numtx_s*, struct numtx_s*);

struct nudlist_scene_s {
    s16* textures;
    s32 textureCount;
    struct numtl_s** materials;
    s32 materialCount;
    u32 flags;
    u8 pad14[0x60];
    void* allocations;
    u8 pad78[0xc];
    void* displayScene;
};
void fn_8002A5DC(struct nudlist_scene_s*);
void fn_80015E48(void*);
void fn_80024D44(struct numtl_s*);
void fn_80024C64(struct numtl_s*);
void NuFileWrite(s16);
void fn_80011070(void*);

void fn_8002B3A0(struct nudlist_scene_s* scene)
{
    s32 i;
    if (scene != 0) {
        switch (scene->flags & 3) {
        default:
        case 0:
            fn_8002A5DC(scene);
            break;
        case 1:
            if (scene->allocations != 0) {
                fn_80015E48(scene->allocations);
            }
            for (i = 0; i < scene->materialCount; i++) {
                while (scene->materials[i]->next != 0) {
                    fn_80024D44(scene->materials[i]);
                }
                fn_80024C64(scene->materials[i]);
            }
            for (i = 0; i < scene->textureCount; i++) {
                NuFileWrite(scene->textures[i]);
            }
        }
    }
    fn_80011070(scene->displayScene);
}

u8* NuHGobjGetPOI(u8* p, u32 i)
{
    u8 j;
    if (i < p[0x80]) {
        j = (*(u8**)(p + 0x2c))[i];
        if (j != 0xff) {
            return *(u8**)(p + 0x28) + j * 0x50;
        }
    }
    return 0;
}

void fn_8002B49C(struct nudlist_hgobj_s* obj, s32 index, struct numtx_s* basis,
                 struct numtx_s* joints, struct numtx_s* dst)
{
    struct nudlist_poi_s* poi = &obj->pois[obj->poiLookup[index]];
    u8 joint = poi->joint;
    if (joint == 0xff) {
        fn_8000EB44(dst, &poi->matrix, basis);
    } else {
        fn_8000EB44(dst, &poi->matrix, &joints[joint]);
        fn_8000EB44(dst, dst, basis);
    }
}
