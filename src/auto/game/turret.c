#include "game/turret.h"

typedef struct { s32 w[16]; } Blk40;
extern void* memset();
extern void (*fn_80009948(char*, s32))(char*, ...);
extern char str_SourceFile_Turret[];
extern char str_Assert_Turret_OutOfMemory[];
s32 fn_800716B0(s32);
s32 fn_80071720(s32, s32, s32, s32, s32, f32);
extern f32 lbl_801AEF54[];
extern void *lbl_80409410;
void fn_8002D9E8(void*, void*, void*, void*, void*, void*);
void fn_8002DAD0(void*);
void fn_8002DB1C(f32*);
void fn_800716D4(void*);
void fn_80071D90(s32);
extern char lbl_801B0ECC[];
extern char lbl_801B0EF0[];
extern f32 lbl_801AEF4C[];
extern f32 lbl_801AEF50[];
extern Blk40 *fn_800133FC(void *);
extern Blk40 *fn_800133E4(void *);
extern void fn_80013200(void *, s32);
s32 fn_80013890(s32, void *);
void fn_800AC020(s32, f32);
void NewRumbleAllPlayers(f32, f32);
void JudderGameCamera(void *, s32, s32, f32);
void fn_80120EF8(s32, s32, s32);
extern char MenuHeader[];
extern char *gPlayer1Pointer[];
extern f32 lbl_801AF000[];
extern f32 lbl_801AF004[];
extern f32 lbl_801AF008[];
extern f32 lbl_801AF00C[];

void fn_8013FA74(void) {
    struct { char unk0[0xC0]; f32 v[3]; char unkCC[0x78]; } buf;
    f32 out[4];

    memset(&buf, 0, 0x144);
    fn_80071720(*(s32*)((char*)lbl_80409410 + 0x760), (s32)&buf, 0, 0, 0x10, lbl_801AEF4C[0]);
    fn_800716D4(&buf);
    out[0] = buf.v[0];
    out[1] = buf.v[1];
    out[2] = buf.v[2];
    out[3] = lbl_801AEF50[0];
    fn_8002DB1C(out);
}

void fn_8013FB54(s32 arg0, s32 arg1) {
    fn_800716B0(arg1);
    if (arg0 != 0) {
        fn_80071720((*(s32 *)((s8 *)(lbl_80409410) + (0x760))), arg1, arg0, 0, -1, lbl_801AEF54[0]);
    }
}

void fn_8013FBB4(void) {
    char buf[0xC];

    memset(buf, 0, 0xC);
    fn_8002D9E8(lbl_801B0EF0, buf, lbl_801B0EF0, buf, lbl_801B0EF0, buf);
    fn_8002DAD0(lbl_801B0ECC);
}

void fn_8013FC1C(s32 arg0, f32 farg0) {
    char buf[0x144];

    memset(buf, 0, 0x144);
    fn_80071D90(0);
    fn_80071720(*(s32*)((char*)lbl_80409410 + 0x760), (s32)buf, arg0, 0, -1, farg0);
    fn_80071D90(1);
    fn_800716D4(buf);
}

void fn_801403BC(char *arg0) {
    char *base;
    char *p;
    s32 i;
    s32 *cnt;

    if (arg0 != NULL) {
        base = *(char **)(arg0 + 0x578);
        if (base != NULL) {
            cnt = (s32 *)base;
            p = base + 4;
            for (i = 0; i < *cnt; ) {
                char *o;
                s32 next = i + 1;
                char *np = p + 0x78;
                Blk40 *d = fn_800133FC(p + 0x18);
                *d = *fn_800133E4(p + 0x18);
                *(s32 *)(p + 0x30) = 0;
                *(s32 *)(p + 0x24) = 0;
                o = *(char **)(arg0 + 0x1b4);
                if (o != NULL && ((*(u32 *)(o + 0xd8) >> i) & 1)) {
                    char *e = base + i * 0x78;
                    *(u16 *)(e + 0x72) |= 0x80;
                    fn_80013200(p + 0x18, 0);
                } else {
                    *(u8 *)(p + 0x73) = *(u8 *)(p + 0x74);
                    *(u16 *)(p + 0x6e) = *(u16 *)(p + 0x70);
                    fn_80013200(p + 0x18, 1);
                }
                i = next;
                p = np;
            }
        }
    }
}

void fn_80140504(s32 a, char *self, s32 b, s32 idx) {
    if (!(*(u16 *)(self + 0x6E) & 1) && *(u8 *)(self + 0x73) != 0) {
        s32 t = *(u8 *)(self + 0x73) - 1;
        *(u8 *)(self + 0x73) = t;
        if ((u8)t == 0) {
            s32 i = 0;
            s32 found = 0;
            char *p;
            JudderGameCamera(MenuHeader, 2, 0, lbl_801AF000[0]);
            NewRumbleAllPlayers(lbl_801AF004[0], lbl_801AF008[0]);
            p = *(char **)((char *)lbl_80409410 + 0x5D4);
            if (p != 0 && i < *(s32 *)((char *)lbl_80409410 + 0x5D8)) {
                do {
                    if (fn_80013890(*(s32 *)p + 4, self + 0x18) != 0) {
                        found = 1;
                        fn_80120EF8(*(s32 *)p, 1, idx);
                        p = 0;
                    } else {
                        i++;
                        p += *(u16 *)(p + 8);
                    }
                } while (p != 0 && i < *(s32 *)((char *)lbl_80409410 + 0x5D8));
            }
            if (*(u16 *)(self + 0x6E) & 0x20) {
                *(u16 *)(self + 0x6E) |= 0x40;
                fn_80013200(self + 0x18, 1);
                return;
            }
            if (found == 0) {
                fn_80013200(self + 0x18, 0);
            }
            *(u16 *)(self + 0x6E) |= 0x80;
            return;
        }
        if (idx != -1) {
            char *pl = gPlayer1Pointer[idx];
            if (*(s32 *)(pl + 0x20C) & 0x01000000) {
                fn_800AC020(**(s32 **)(*(char **)(pl + 0xEEC) + 0x61C), lbl_801AF00C[0]);
            }
        }
    }
}

struct NUTURRET_s* CreateTurretSys(char** cursor, char** end, u32 value)
{
    struct NUTURRET_s* turret;
    *cursor = (char*)(((u32)*cursor + 3) & ~3);
    turret = (struct NUTURRET_s*)*cursor;
    if (*cursor + sizeof(*turret) >= *end) {
        fn_80009948(str_SourceFile_Turret, 0xaa)(str_Assert_Turret_OutOfMemory);
        return NULL;
    }
    *cursor += sizeof(*turret);
    if (turret != NULL) {
        memset(turret, 0, sizeof(*turret));
        turret->valueF04 = value;
    }
    return turret;
}

#define fn_8002D9E8 ((s32 (*)(s32 *, s32 *, void *, void *, void *, void *))fn_8002D9E8)
#define fn_8002DAD0 ((s32 (*)(s32 *))fn_8002DAD0)
extern s32 lbl_801E1720[];
extern s32 lbl_801E1744[];
extern s32 lbl_801E1768[];

void fn_8013FB08(void) {
    fn_8002D9E8(lbl_801E1744, lbl_801E1720, ((s8 *)lbl_801E1744) + 0xC, ((s8 *)lbl_801E1720) + 0xC, ((s8 *)lbl_801E1744) + 0x18, ((s8 *)lbl_801E1720) + 0x18);
    fn_8002DAD0(lbl_801E1768);
}
#undef fn_8002D9E8
#undef fn_8002DAD0
