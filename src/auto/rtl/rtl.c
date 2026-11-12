#include "types.h"
#include "auto/rtl/rtl.h"

extern s32 lbl_804087E4;
extern s32 lbl_80408738;
extern s32 lbl_804087C8;
extern s32 lbl_804087D4;

s32 fn_8006EB90(void)
{
    return lbl_80408738;
}

void fn_8006F7E0(s32 a)
{
    lbl_804087C8 = a;
}

void fn_8006F8A8(s32 a)
{
    lbl_804087D4 = a;
}

s32 fn_80071D90(s32 a)
{
    s32 t0 = lbl_804087E4;
    lbl_804087E4 = a;
    return t0;
}

extern void* lbl_804087D8;
extern s32 lbl_804087DC;
extern u8* fn_80040DE8(void*, s32);

struct RtlVec { u32 x, y, z; };

s32 fn_80071AEC(s32 index, struct RtlVec* value)
{
    struct RtlVec* p;
    if (lbl_804087D8 && index >= 0 && index < lbl_804087DC &&
        (p = (struct RtlVec*)fn_80040DE8(lbl_804087D8, index)) && value) {
        *p = *value;
        return 1;
    }
    return 0;
}

s32 fn_80071B78(s32 index, f32 a, f32 b)
{
    u8* p;
    if (lbl_804087D8 && index >= 0 && index < lbl_804087DC &&
        (p = fn_80040DE8(lbl_804087D8, index))) {
        *(f32*)(p + 0x3c) = a;
        if (a > b) *(f32*)(p + 0x40) = a;
        else *(f32*)(p + 0x40) = b;
        return 1;
    }
    return 0;
}

s32 fn_80071C04(s32 index, struct RtlVec* a, struct RtlVec* b)
{
    u8* p;
    if (lbl_804087D8 && index >= 0 && index < lbl_804087DC &&
        (p = fn_80040DE8(lbl_804087D8, index)) && (a || b)) {
        if (a) *(struct RtlVec*)(p + 0x24) = *a;
        if (b) *(struct RtlVec*)(p + 0x30) = *b;
        return 1;
    }
    return 0;
}

extern u8 lbl_803D4680[];
extern s32 lbl_8040AEF8;
extern s32 lbl_804087CC;
extern void* memcpy(void*, const void*, ...);
extern s32 NuFileRead(s32, void*, s32);

f32 fn_8006F28C(void)
{
    f32 value;
    u8* dest = (u8*)&value;
    s32 remaining = 4;
    u8* bytes = dest;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    if (lbl_804087D4) {
        u8 t = *(u8*)&value;
        *(u8*)&value = bytes[3];
        bytes[3] = t;
        t = bytes[1];
        bytes[1] = bytes[2];
        bytes[2] = t;
    }
    return value;
}

s32 fn_8006F37C(void)
{
    s32 value;
    u8* dest = (u8*)&value;
    s32 remaining = 4;
    u8* bytes = dest;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    if (lbl_804087D4) {
        u8 t = *(u8*)&value;
        *(u8*)&value = bytes[3];
        bytes[3] = t;
        t = bytes[1];
        bytes[1] = bytes[2];
        bytes[2] = t;
    }
    return value;
}

s16 fn_8006F55C(void)
{
    s16 value;
    u8* dest = (u8*)&value;
    s32 remaining = 2;
    u8* bytes = dest;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    if (lbl_804087D4) {
        u8 t = *(u8*)&value;
        *(u8*)&value = bytes[1];
        bytes[1] = t;
    }
    return value;
}

u16 fn_8006F63C(void)
{
    u16 value;
    u8* dest = (u8*)&value;
    s32 remaining = 2;
    u8* bytes = dest;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    if (lbl_804087D4) {
        u8 t = *(u8*)&value;
        *(u8*)&value = bytes[1];
        bytes[1] = t;
    }
    return value;
}

s8 fn_8006F71C(void)
{
    s8 value;
    u8* dest = (u8*)&value;
    s32 remaining = 1;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    return value;
}

void fn_8006F7E8(u8* dest, s32 remaining)
{
    while (remaining > 0) {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    }
}

extern s32 lbl_804087D0;
extern s32 fn_800059A0(s32, void*, s32);
extern s32 fn_80005764(s32);

s32 fn_8006F1D8(void)
{
    if (lbl_804087CC != -1) {
        if (lbl_804087D0 && lbl_8040AEF8) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: fn_800059A0(lbl_804087CC, lbl_803D4680, lbl_8040AEF8);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
        switch (lbl_804087C8) {
        case 1:
            fn_80005764(lbl_804087CC);
            lbl_804087CC = -1;
            lbl_804087C8 = 0;
            return 1;
        case 0: return 0;
        case 2: return 0;
        }
    }
    return 0;
}

extern s32 lbl_80408758;
extern u32 lbl_802C4EC8[];
extern void fn_80027694(s32, s32, s32, s32, u32, u32);

void fn_8006EBC4(u8* item, s32 x, s32 y, s32 w, s32 h)
{
    s32 a, b, c;
    if (*(u32*)(item + 0x10) & 0x40000000) {
        w *= 16;
        x *= 16;
        y *= 8;
        h *= 8;
        a = x + w / 4;
        fn_80027694(x, y, a, y + h, *(u32*)(item + 0x24), lbl_802C4EC8[lbl_80408758]);
        b = x + w / 2;
        fn_80027694(a, y, b, y + h, *(u32*)(item + 0x24), lbl_802C4EC8[lbl_80408758]);
        c = x + w * 3 / 4;
        fn_80027694(b, y, c, y + h, *(u32*)(item + 0x24), lbl_802C4EC8[lbl_80408758]);
        fn_80027694(c, y, x + w, y + h, *(u32*)(item + 0x24), lbl_802C4EC8[lbl_80408758]);
    }
}

extern s32 lbl_804087E0;
extern char lbl_801990A8[], lbl_801990D0[];
extern void* fn_80040B4C(s32, s32, void*, void*, s32);
extern void (*fn_80009948(char*, s32))();

s32 fn_80071E0C(void* arg, s32* data, s32 count)
{
    s32 local = *data;
    lbl_804087D8 = fn_80040B4C(count, 0x8c, arg, &local, 0x10);
    if (!lbl_804087D8)
        (*fn_80009948(lbl_801990A8, 0x26f))(lbl_801990D0, count);
    lbl_804087E0 = 0;
    lbl_804087DC = count;
    return count;
}

s32 fn_8006EB98();
extern void *lbl_80408720;
extern s32 lbl_80408724;

void fn_8006EB40(void) {
    s32 (*temp_r0)();

    if (((s32) lbl_80408724 != 0) && ((void *) lbl_80408720 != NULL)) {
        temp_r0 = (*(s32 (**)())((s8 *)(lbl_80408720) + (0x30)));
        if (temp_r0 != NULL) {
            temp_r0();
        }
    } else {
        fn_8006EB98();
    }
}

s32 fn_8002D9E8(s32, s32, s32, s32, s32, s32);
s32 fn_8002DAD0(s32);

void fn_800716D4(s32 arg0) {
    fn_8002D9E8(arg0 + 0x9C, arg0 + 0x78, arg0 + 0xA8, arg0 + 0x84, arg0 + 0xB4, arg0 + 0x90);
    fn_8002DAD0(arg0 + 0xC0);
}

extern s32 lbl_804087DC;

s32 fn_80071A70(s32 arg0, s8 arg1) {
    void *temp_r3;

    if (((s32) lbl_804087D8 != 0) && (arg0 >= 0) && (arg0 < (s32) lbl_804087DC) && ((u32) (arg1 - 1) <= 7U)) {
        temp_r3 = fn_80040DE8(lbl_804087D8, arg0);
        if (temp_r3 != NULL) {
            (*(s8 *)((s8 *)(temp_r3) + (0x58))) = arg1;
            return 1;
        }
    }
    return 0;
}

s32 fn_80071CC8(s32 arg0, u32 arg1, s32 *arg2) {
    if ((arg0 != 0) && (arg2 != NULL)) {
        if (arg1 > 0x80U) {
            return 0;
        }
        *arg2 = (arg1 * 0x8C) + arg0 + 0x10;
        return 1;
    }
    return 0;
}

s32 fn_8006ECF0();
extern s32 lbl_80408728;

s32 fn_8006EB98(void) {
    s32 var_r3;

    var_r3 = lbl_80408728;
    if (var_r3 != 0) {
        var_r3 = fn_8006ECF0();
    }
    return var_r3;
}

void fn_8007042C(void *, s32);

void fn_800716B0(void *arg0) {
    fn_8007042C(arg0, 0);
}

extern void fn_80040D00(void*);
void fn_800718FC(s32 id)
{
    u8* light;

    if (lbl_804087D8 && id >= 0 && id < lbl_804087DC) {
        light = fn_80040DE8(lbl_804087D8, id);
        if (light) {
            fn_80040D00(light);
            lbl_804087E0 -= 1;
        }
    }
}

extern void* fn_80040E14(void*, void*);
s32 fn_80071DA0(void)
{
    void* next;
    void* light;

    if (lbl_804087D8) {
        light = fn_80040E14(lbl_804087D8, NULL);
        if (light) {
            do {
                next = fn_80040E14(lbl_804087D8, light);
                fn_80040D00(light);
                light = next;
            } while (light);
        }
        lbl_804087E0 = 0;
    }
    return lbl_804087DC;
}

s32 fn_80071668(u8* set, s32 id)
{
    s32 i;
    if (set) {
        for (i = 0; i < 128; ++i) {
            if (set[0x5c] && *(u16*)(set + 0x6c) == id) return i;
            set += 0x8c;
        }
    }
    return -1;
}

extern f32 lbl_804087FC;
extern void fn_800712E4(void*, f32);
void fn_80071848(u8* set, f32 dt)
{
    void* p;
    u8* light;
    s32 i;
    lbl_804087FC = dt;
    if (lbl_804087D8) {
        p = fn_80040E14(lbl_804087D8, 0);
        while (p) {
            fn_800712E4(p, dt);
            p = fn_80040E14(lbl_804087D8, p);
        }
    }
    if (set) {
        light = set + 4;
        i = 0;
        if (light[0x58]) {
            do {
                fn_800712E4(light, dt);
                ++i;
                light += 0x8c;
            } while (i <= 127 && light[0x58]);
        }
    }
}

extern s32 lbl_804087F4;
void fn_8007057C(struct rtl_s* light, struct rtldata_s* data, f32 strength)
{
    s32 i, j;
    if (light->type != 1) {
        for (i = 0; i < 3; ++i) {
            if (strength > data->directionalStrength[i]) {
                for (j = 2; j != i; --j) {
                    data->directional[j] = data->directional[j - 1];
                    data->directionalStrength[j] = data->directionalStrength[j - 1];
                }
                data->directional[i] = light;
                data->directionalStrength[i] = strength;
                return;
            }
        }
    } else {
        for (i = 0; i < 3; ++i) {
            if (strength > data->ambientStrength[i]) {
                for (j = 2; j != i; --j) {
                    data->ambient[j] = data->ambient[j - 1];
                    data->ambientStrength[j] = data->ambientStrength[j - 1];
                }
                data->ambient[i] = light;
                data->ambientStrength[i] = strength;
                return;
            }
        }
    }
    lbl_804087F4 = 1;
}

extern void fn_80070690(void*, void*, struct rtldata_s*, f32);
extern void fn_80070C0C(void*, struct rtldata_s*, void*, void*, s32, f32);
extern void fn_80071128(struct rtldata_s*);
extern const f32 lbl_80199898[];
void fn_80071720(void* set, struct rtldata_s* data, void* pos, void* rot, s32 identity, f32 scale)
{
    struct rtldata_s local;
    struct rtldata_s* p;
    f32 zero;
    if (!data) {
        p = &local;
        p->field140 = lbl_80199898[0];
        fn_8007042C(p, 1);
    } else {
        p = data;
        zero = lbl_80199898[0];
        p->field140 = zero;
        fn_8007042C(p, 0);
        if (p->cachedLight && p->cachedLight->uid != p->cachedUid) {
            p->cachedLight = 0;
            p->cachedUid = 0;
            p->cachedValue = zero;
        }
    }
    /* Separate passes preserve ProDG's floating-point register lifetimes. */
    do {
        if (lbl_804087D8 && lbl_804087E4)
            fn_80070C0C(0, p, pos, rot, identity, scale);
    } while (0);
    do {
        if (set) fn_80070C0C(set, p, pos, rot, identity, scale);
    } while (0);
    fn_80070690(pos, rot, p, scale);
    fn_80071128(p);
}

s32 fn_8006F46C(void)
{
    s32 value;
    u8* dest = (u8*)&value;
    s32 remaining = 4;
    u8* bytes = dest;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    if (lbl_804087D4) {
        u8 a = *(u8*)&value;
        u8 b = bytes[1];
        u8 d = bytes[3];
        *(u8*)&value = d;
        bytes[3] = a;
        bytes[1] = bytes[2];
        bytes[2] = b;
    }
    return value;
}

extern void* memset(void*, s32, ...);
extern const f32 lbl_80199134[], lbl_80199138[];
extern struct nuvec_s lbl_801B0EF0[];
void fn_8007042C(void* arg, s32 reset)
{
    struct rtldata_s* p = arg;
    struct nuvec_s black;
    struct nuvec_s* blackP = &black;
    struct rtldata_s* cursor;
    s32 i;
    memset(blackP, 0, 12);
    cursor = p;
    for (i = 0; i < 3; ++i) {
        cursor->ambient[0] = 0;
        cursor->directional[0] = 0;
        cursor->ambientStrength[0] = lbl_80199134[0];
        cursor->directionalStrength[0] = lbl_80199134[0];
        cursor = (struct rtldata_s*)((u8*)cursor + 4);
    }
    p->antiCount = 0;
    p->field120 = lbl_80199138[0];
    p->intensity[0] = p->intensity[1] = p->intensity[2] = p->colour = black;
    p->direction[0] = p->direction[1] = p->direction[2] = lbl_801B0EF0[0];
    if (reset) {
        p->cachedLight = 0;
        p->cachedUid = 0;
        p->cachedValue = lbl_80199134[0];
        p->shadowBlend = lbl_80199134[0];
        p->blendedValue = lbl_80199134[0];
    }
}

s32 fn_80071D08(s32 id, s32 enabled)
{
    struct rtl_s* p;
    s32 previous;
    u32 oldDisabled;
    if (!lbl_804087D8 || id < 0 || id >= lbl_804087DC ||
        !(p = (struct rtl_s*)fn_80040DE8(lbl_804087D8, id))) return 0;
    previous = p->disabled;
    oldDisabled = previous;
    previous = ~oldDisabled;
    p->disabled = enabled == 0;
    return previous & 1;
}

extern void* fn_80040C24(void*);
extern const f32 lbl_80199094[], lbl_80199098[], lbl_8019909C[], lbl_801990A0[], lbl_801990A4[];
extern struct nuvec_s lbl_801B0ECC[];
extern u16 lbl_80408804;
s32 fn_8006F8B0(void)
{
    struct rtl_s* p;
    if (!lbl_804087D8) return -1;
    p = fn_80040C24(lbl_804087D8);
    if (p) {
        p->type = 4;
        p->position = lbl_801B0ECC[0];
        p->innerRadius = lbl_80199094[0];
        p->outerRadius = lbl_80199098[0];
        p->primaryColour.x = lbl_80199094[0];
        p->primaryColour.y = lbl_80199094[0];
        p->primaryColour.z = lbl_80199094[0];
        p->secondaryColour.x = lbl_8019909C[0];
        p->secondaryColour.y = lbl_8019909C[0];
        p->secondaryColour.z = lbl_8019909C[0];
        p->type = 2;
        p->disabled = 0;
        p->field5e = 0;
        p->field60 = 0;
        p->onTime = lbl_801990A0[0];
        p->onRandom = lbl_801990A0[0];
        p->offTime = lbl_801990A0[0];
        p->offRandom = lbl_801990A0[0];
        p->timer = lbl_801990A4[0];
        p->yaw = p->pitch = 0;
        p->direction.x = lbl_801990A4[0];
        p->direction.y = lbl_801990A4[0];
        p->direction.z = lbl_80199094[0];
        p->period = lbl_801990A4[0];
        p->intensity = lbl_80199094[0];
        NuVecRotateX(&p->direction, &p->direction, p->pitch);
        NuVecRotateY(&p->direction, &p->direction, p->yaw);
        p->uid = lbl_80408804++;
        if (!lbl_80408804) ++lbl_80408804;
        {
            s32 id = *(u16*)((u8*)p - 4);
            ++lbl_804087E0;
            return id;
        }
    }
    return -1;
}

static inline s32 rtlFindTemplate(u8* set, s32 id)
{
    s32 i;
    if (set) {
        u8* p;
        i = 0;
        p = set;
        for (; i < 128; ++i) {
            if (p[0x5c] && *(u16*)(p + 0x6c) == id) return i;
            p += 0x8c;
        }
    }
    return -1;
}
s32 fn_8007195C(u8* set, s32 id)
{
    u8* source;
    s32 alloc;
    s32 found = rtlFindTemplate(set, id);
    if (found >= 0) {
        alloc = fn_8006F8B0();
        if (alloc >= 0) {
            struct rtl_s* light = (struct rtl_s*)fn_80040DE8(lbl_804087D8, alloc);
            source = set + found * 0x8c;
            *light = *(struct rtl_s*)(source + 4);
            return alloc;
        }
    }
    return -1;
}

extern f32 lbl_80199154[], lbl_80199158[], lbl_8019915C[], lbl_80199160[];
extern struct nuvec_s lbl_801BB46C[];
extern f32 lbl_804087FC, lbl_80408800;
extern f32 fn_8000C038(void);
extern void fn_8000CE8C(struct nuvec_s*, struct nuvec_s*, struct nuvec_s*, f32);
void fn_80071128(struct rtldata_s* p)
{
    f32 half, zero, one;
    if (p->cachedLight && lbl_804087FC != lbl_80199154[0] &&
        (p->cachedLight->type == 3 || p->cachedLight->type == 8)) {
        f32 random = fn_8000C038();
        half = lbl_80199158[0];
        p->shadowDirection.x = (random - half) * lbl_801BB46C[0].x + p->shadowDirection.x;
        p->shadowDirection.y = (fn_8000C038() - half) * lbl_801BB46C[0].y + p->shadowDirection.y;
        p->shadowDirection.z = (fn_8000C038() - half) * lbl_801BB46C[0].z + p->shadowDirection.z;
    }
    zero = lbl_80199154[0];
    if (p->cachedValue != zero) NuVecNorm(&p->shadowDirection, &p->shadowDirection);
    if (p->shadowBlend != zero) {
        one = lbl_8019915C[0];
        if (p->shadowBlend == one) {
            p->previousShadowDirection = p->blendedShadowDirection;
            p->shadowBlend = lbl_80199160[0];
            p->previousValue = p->blendedValue;
        }
        fn_8000CE8C(&p->blendedShadowDirection, &p->previousShadowDirection, &p->shadowDirection, p->shadowBlend);
        NuVecNorm(&p->blendedShadowDirection, &p->blendedShadowDirection);
        p->blendedValue = p->previousValue * p->shadowBlend + p->cachedValue * (one - p->shadowBlend);
        /* Keep the MAX operand order and repeated expression for ProDG. */
        p->shadowBlend = (p->shadowBlend - lbl_804087FC * lbl_80408800) < zero ?
            zero : (p->shadowBlend - lbl_804087FC * lbl_80408800);
    } else {
        p->blendedShadowDirection = p->shadowDirection;
        p->blendedValue = p->cachedValue;
    }
}
