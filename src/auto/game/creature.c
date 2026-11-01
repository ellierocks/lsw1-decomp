#include "game/creature.h"

extern s32 lbl_8040B058;
extern struct NUGAMEOBJECT_s* lbl_80409430;
extern s32 lbl_8040912C;
extern s32 fn_800A8DAC(void*, void*, void*, void*, void*, void*, struct NUGAMEOBJECT_s*, u32);
extern void fn_800AA4A8(struct NUGAMEOBJECT_s*);

void fn_800AA92C(void)
{
    s32 i = 0;
    struct NUGAMEOBJECT_s* object = lbl_80409430;

    for (; i < lbl_8040B058; i++, object++) {
        if (!(object->motionFlags & 0x800)) {
            object->drawResult = fn_800A8DAC(object->characterModel,
                (char*)object + 8, (char*)object + 0xC8,
                (object->flagsF28 & 0x200) ? (char*)object + 0x108 : NULL,
                object->drawMatrixEnabled ? (char*)object + 0x148 : NULL,
                (char*)object->playerPacket + 0x194, object, object->drawFlags);
            if (object->drawResult) {
                fn_800AA4A8(object);
            }
            object->drawUpdated = 1;
            if (object->drawResult) {
                object->flagsF28 |= 0x100;
            }
            if (lbl_8040912C == 0 && object->characterData->drawFn != NULL) {
                object->characterData->drawFn(object);
            }
        }
    }
}
