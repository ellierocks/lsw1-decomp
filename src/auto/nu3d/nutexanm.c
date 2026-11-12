#include "types.h"
#include "game/levels.h"

/* Texture animation types: CrashWOC DWARF (nutexanimprog_s, nutexanimenv_s) with the
 * OpenSaga additions (mask at 0x1B8, code at 0x1BA), both confirmed against the pfta*
 * handlers and fn_80015C88 below. */
struct nutexanimprog_s {
    struct nutexanimprog_s *succ;   /* 0x000 */
    struct nutexanimprog_s *prev;   /* 0x004 */
    char name[32];                  /* 0x008 */
    s32 on_sig[32];                 /* 0x028 */
    s32 off_sig[32];                /* 0x0A8 */
    u32 on_mask;                    /* 0x128 */
    u32 off_mask;                   /* 0x12C */
    s16 xdef_ids[32];               /* 0x130 */
    s16 xdef_addrs[32];             /* 0x170 */
    s32 xdef_cnt;                   /* 0x1B0 */
    s16 eop;                        /* 0x1B4 */
    u8 flags;                       /* 0x1B6 */
    u8 pad1B7;                      /* 0x1B7 */
    s16 mask;                       /* 0x1B8 */
    s16 code[1];                    /* 0x1BA */
};

struct nutexanim_s {
    struct nutexanim_s *succ;       /* 0x00 */
    struct nutexanim_s *prev;       /* 0x04 */
    u16 *tids;                      /* 0x08 */
    s16 ntids;                      /* 0x0C */
    u8 dynalloc;                    /* 0x0E */
    u8 pad0F;
    struct numtl_s *mtl;            /* 0x10 */
    struct nutexanimenv_s *env;     /* 0x14 */
    char *name;                     /* 0x18 */
    char *progname;                 /* 0x1C */
};

struct nutexanimlist_s {
    struct nutexanim_s *nta;        /* 0x0 */
    struct nutexanimlist_s *succ;   /* 0x4 */
    struct nutexanimlist_s *prev;   /* 0x8 */
};

extern struct nutexanimprog_s *lbl_8040A2F8; /* parprog */
extern s32 NuFParGetInt(void *fp);
extern s32 NuFParGetWord(void *fp);
extern s32 strcasecmp(const char *a, const char *b);
extern u32 strlen(const char *s);

extern s32 lbl_80407BE0;
extern s32 lbl_80407BDC;
extern s32 lbl_8040AD2C;
extern s32 lbl_8040AD28;
extern s32 lbl_8040AD24;
extern s32 lbl_8040AD20;
extern s32 lbl_8040AD18;

extern u32 lbl_801B14D8[];

extern u32 lbl_8040A304;
extern u32 lbl_8040A300;
extern u32 lbl_8040A2FC;

void NuTexAnimSetSignals(u32 s)
{
    u32 old = lbl_8040A304;
    u32 x;
    lbl_8040A304 = s;
    x = s ^ old;
    lbl_8040A2FC = x & s;
    lbl_8040A300 = x & ~s;
}

void fn_80014248(u32 v)
{
    lbl_801B14D8[0] = v & 0xff;
    lbl_801B14D8[1] = (v >> 8) & 0xff;
    lbl_801B14D8[2] = (v >> 16) & 0xff;
    lbl_801B14D8[3] = v >> 24;
}

void fn_80014214(s32 a, s32 b)
{
    lbl_80407BE0 = b * lbl_8040AD2C / lbl_8040AD24 - lbl_8040AD18;
    lbl_80407BDC = a * lbl_8040AD28 / lbl_8040AD20;
}

void fn_80015F7C(void *fp)
{
    lbl_8040A2F8->mask = NuFParGetInt(fp);
}

s32 fn_80013890(void *arg0, void *arg1) {
    s32 temp_r9;
    s32 temp_r9_2;

    temp_r9 = (*(s32 *)((s8 *)(arg0) + (4)));
    if ((temp_r9 != 0) && (temp_r9 == (s32) (*(s32 *)((s8 *)(arg1) + (4))))) {
        return 1;
    }
    temp_r9_2 = (*(s32 *)((s8 *)(arg0) + (8)));
    if (temp_r9_2 != 0 && temp_r9_2 == (s32) (*(s32 *)((s8 *)(arg1) + (8)))) {
        return 1;
    }
    return 0;
}

void *NuMemAlloc(s32, s32 *, s32);
extern s32 lbl_8018FC1C[];

void fn_80015C88(s32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_r0;
    s32 var_r3;

    if (arg0 != NULL) {
        var_r3 = (*arg0 + 3) & 0xFFFFFFFC;
        *arg0 = var_r3 + 0xEC;
    } else {
        var_r3 = NuMemAlloc(0xEC, lbl_8018FC1C, 0x1BD);
    }
    if (var_r3 != 0) {
        (*(s32 *)((s8 *)(var_r3) + (0))) = arg3;
        (*(s32 *)((s8 *)(var_r3) + (4))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0x88))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xCC))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xD0))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xD4))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xD8))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xDC))) = arg1;
        (*(s32 *)((s8 *)(var_r3) + (0xE0))) = arg2;
        (*(s32 *)((s8 *)(var_r3) + (0xE4))) = 0;
        if (arg0 != NULL) {
            var_r0 = (*(s32 *)((s8 *)(var_r3) + (0xE8))) & 0x7FFFFFFF;
        } else {
            var_r0 = (*(s32 *)((s8 *)(var_r3) + (0xE8))) | 0x80000000;
        }
        (*(s32 *)((s8 *)(var_r3) + (0xE8))) = var_r0;
    }
}

/* Instance animation behind a special handle (levels.h: nuinstanim_s). Through the
 * special: special+0x40 -> +0x48. Through the display special: +0xC4, where -1 and 0 mean
 * none. */
struct nuinstanim_s *fn_8001384C(struct nuhspecial_s *sph) {
    if (sph->special != NULL) {
        return *(struct nuinstanim_s **)(*(s8 **)((s8 *)sph->special + 0x40) + 0x48);
    }
    if (sph->display != NULL && *(s32 *)((s8 *)sph->display + 0xC4) != -1
        && *(struct nuinstanim_s **)((s8 *)sph->display + 0xC4) != NULL) {
        return *(struct nuinstanim_s **)((s8 *)sph->display + 0xC4);
    }
    return NULL;
}

extern s32 NuStrICmp(const char *a, const char *b);

struct nugspline_s *NuSplineFind(struct nugscn_s *scene, const char *name) {
    struct nugspline_s *spline;
    s32 i;

    if (scene != NULL) {
        spline = scene->splines;
        for (i = 0; i < scene->numsplines; i++, spline++) {
            if (NuStrICmp(name, spline->name) == 0) {
                return spline;
            }
        }
    }
    return NULL;
}

extern struct nutexanimprog_s *lbl_80407BF0;   /* sys_progs */
extern struct nutexanimlist_s lbl_80312E50[64]; /* ntalsysbuff */
extern struct nutexanimlist_s *lbl_80407BF8;   /* ntal_free */
extern struct nutexanimlist_s *lbl_80407BF4;   /* ntal_first */
extern s32 lbl_8040A2F4;                       /* xdeflabtabcnt */
extern s32 lbl_8040A2F0;                       /* labtabcnt */
extern char lbl_8020A558[64][21];              /* labtab */
extern s32 lbl_80312D50[64];                   /* nta_labels */
extern void (*lbl_80407B1C)(void);             /* critical section enter */
extern void (*lbl_80407B20)(void);             /* critical section leave */

void fn_80015BD0(void)
{
    s32 i;

    lbl_80407BF0 = NULL;
    for (i = 0; i < 63; i++) {
        lbl_80312E50[i].succ = &lbl_80312E50[i + 1];
    }
    lbl_80407BF8 = &lbl_80312E50[0];
    lbl_8040A2FC = 0;
    lbl_80407BF4 = NULL;
    lbl_8040A2F4 = 0;
    lbl_8040A304 = 0;
    lbl_8040A300 = 0;
    lbl_80312E50[i].succ = NULL;
}

struct nutexanimprog_s *NuTexAnimProgFind(char *name)
{
    struct nutexanimprog_s *p;

    p = lbl_80407BF0;
    while (p != NULL) {
        if (strcasecmp(name, p->name) == 0) {
            break;
        }
        p = p->succ;
    }
    return p;
}

void fn_80015FA4(void *fp)
{
    s32 tex = NuFParGetInt(fp);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 0;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = tex;
}

void fn_80016000(void *fp)
{
    s32 a = NuFParGetInt(fp);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 1;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = a;
}

void fn_8001605C(void *fp)
{
    s32 a = NuFParGetInt(fp);
    s32 b = NuFParGetInt(fp);
    s32 c = NuFParGetInt(fp);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 2;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = a;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = b;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = c;
}

void fn_8001610C(void *fp)
{
    s32 sig = NuFParGetInt(fp);
    lbl_8040A2F8->on_sig[sig] = lbl_8040A2F8->eop;
    lbl_8040A2F8->on_mask |= 1 << sig;
}

void fn_80016154(void *fp)
{
    s32 sig = NuFParGetInt(fp);
    lbl_8040A2F8->off_sig[sig] = lbl_8040A2F8->eop;
    lbl_8040A2F8->off_mask |= 1 << sig;
}

struct nutexanimenv_s {
    struct nutexanimprog_s *prog;   /* 0x00 */
    s32 pc;                         /* 0x04 */
    s32 rep_count[16];              /* 0x08 */
    s32 rep_start[16];              /* 0x48 */
    s32 rep_ix;                     /* 0x88 */
    s32 ret_addr[16];               /* 0x8C */
    s32 ra_ix;                      /* 0xCC */
    s32 pause;                      /* 0xD0 */
    s32 pause_r;                    /* 0xD4 */
    s32 pause_cnt;                  /* 0xD8 */
    struct numtl_s *mtl;            /* 0xDC */
    u16 *tids;                      /* 0xE0 */
    s32 tex_ix;                     /* 0xE4 */
    u32 dynalloc : 1;               /* 0xE8 */
};

extern void NuMemFree(void *p, s32 *file, s32 line);
extern void NuTexAnimEnvProc(struct nutexanimenv_s *env);

void fn_80015D60(struct nutexanim_s *nta)
{
    if (nta->env != NULL && nta->env->dynalloc) {
        NuMemFree(nta->env, lbl_8018FC1C, 0x1A1);
    }
    if (nta->dynalloc) {
        NuMemFree(nta, lbl_8018FC1C, 0x566);
    }
}

void fn_80015DD0(struct nutexanim_s *nta)
{
    struct nutexanimlist_s *entry;

    lbl_80407B1C();
    entry = lbl_80407BF8;
    if (entry != NULL) {
        lbl_80407BF8 = entry->succ;
        entry->prev = NULL;
        entry->nta = nta;
        entry->succ = lbl_80407BF4;
        if (lbl_80407BF4 != NULL) {
            lbl_80407BF4->prev = entry;
        }
        lbl_80407BF4 = entry;
    }
    lbl_80407B20();
}

void fn_80015E48(struct nutexanim_s *nta)
{
    struct nutexanimlist_s *cur;

    lbl_80407B1C();
    for (cur = lbl_80407BF4; cur != NULL; cur = cur->succ) {
        if (cur->nta == nta) {
            if (cur->succ != NULL) {
                cur->succ->prev = cur->prev;
            }
            if (cur->prev != NULL) {
                cur->prev->succ = cur->succ;
            } else {
                lbl_80407BF4 = cur->succ;
            }
            cur->succ = lbl_80407BF8;
            lbl_80407BF8 = cur;
            lbl_80407B20();
            return;
        }
    }
    lbl_80407B20();
}

extern s32 lbl_8040A30C;
extern f32 lbl_8040A308;
extern s16 lbl_8040A310;

void fn_80015EFC(void)
{
    struct nutexanimlist_s *list;
    struct nutexanim_s *nta;

    lbl_8040A30C = 0x44;
    lbl_8040A308 = 0.016666668f;
    lbl_8040A310 = -1;
    for (list = lbl_80407BF4; list != NULL; list = list->succ) {
        for (nta = list->nta; nta != NULL; nta = nta->succ) {
            if (nta->env != NULL) {
                NuTexAnimEnvProc(nta->env);
            }
        }
    }
}

extern void NuStrCpy(char *dst, const char *src);
extern void *(*fn_80009948(s32 *file, s32 line))(const char *fmt, ...);
extern char lbl_8018FC88[];

static inline s32 LabTabFind(char *name)
{
    s32 i;

    if (strlen(name) > 20) {
        name[20] = 0;
    }
    for (i = 0; i < lbl_8040A2F0; i++) {
        if (strcasecmp(lbl_8020A558[i], name) == 0) {
            return i;
        }
    }
    if (lbl_8040A2F0 > 63) {
        fn_80009948(lbl_8018FC1C, 0x249)(lbl_8018FC88);
    }
    NuStrCpy(lbl_8020A558[lbl_8040A2F0++], name);
    return lbl_8040A2F0 - 1;
}

void fn_8001619C(void *fp)
{
    s32 idx;

    NuFParGetWord(fp);
    idx = LabTabFind((char *)fp + 0x30A);
    lbl_80312D50[idx] = lbl_8040A2F8->eop;
}

extern char lbl_8018FC64[];

static inline s32 ParGetCCInline(void *fp)
{
    s32 cc;

    NuFParGetWord(fp);
    switch (*((char *)fp + 0x30A)) {
    case '=':
        cc = 0;
        break;
    case '<':
        switch (*((char *)fp + 0x30B)) {
        case '>':
            cc = 5;
            break;
        case '=':
            cc = 3;
            break;
        default:
            cc = 1;
            break;
        }
        break;
    case '>':
        cc = 2;
        if (*((u8 *)fp + 0x30B) == '=') {
            cc = 4;
        }
        break;
    case '!':
        cc = 5;
        break;
    default:
        fn_80009948(lbl_8018FC1C, 0x22C)(lbl_8018FC64, (char *)fp + 0x30A, *(s32 *)((char *)fp + 0x50C));
        break;
    }
    return cc;
}

/* pftaUntiltex (opcode 0xF: condition, value) with ParGetCC inlined. */
void pftaUntiltex(void *fp)
{
    s32 cc = ParGetCCInline(fp);
    s32 val = NuFParGetInt(fp);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 0xF;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = cc;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = val;
}

struct nutxtglyph_s {
    f32 u0;     /* 0x00 */
    f32 v0;     /* 0x04 */
    f32 u1;     /* 0x08 */
    f32 v1;     /* 0x0C */
    u8 w;       /* 0x10: advance in pixels */
    u8 h;       /* 0x11 */
    u8 pad12[2];
};

/* Fixed-point text metrics (font pointer at lbl_80407BC0, scale at BCC..BD8). */
struct nutxtfont_s {
    u8 pad0[0xC];
    s16 h;      /* 0x0C */
    u8 padE[6];
    s16 w;      /* 0x14 */
    s16 gap;    /* 0x16 */
    void *mtl;  /* 0x18 */
    u8 pad1C[4];
    u8 map[256];    /* 0x020: character -> glyph index, 0xFF = none */
    struct nutxtglyph_s glyph[1];   /* 0x120 */
};

extern struct nutxtfont_s *lbl_80407BC0;
extern struct nutxtfont_s *lbl_80407E04;
extern s32 lbl_80407BC4;
extern s32 lbl_80407BC8;
extern s32 lbl_80407BCC;
extern s32 lbl_80407BD0;
extern s32 lbl_80407BD4;
extern s32 lbl_80407BD8;
extern s32 lbl_8040AD04;
extern s32 lbl_8040AD14;
extern s32 lbl_8040AD1C;

void fn_800140EC(struct nutxtfont_s *f)
{
    s32 t;

    if (f != NULL) {
        if (lbl_80407E04 == NULL) {
            lbl_80407E04 = f;
        }
    } else {
        f = lbl_80407E04;
    }
    t = ((f->h << 3) * lbl_80407BD0) >> 4;
    lbl_80407BC0 = f;
    lbl_8040AD1C = ((f->h << 3) * lbl_80407BD8) >> 4;
    lbl_8040AD04 = t;
    lbl_80407BC4 = (f->gap * lbl_80407BCC) >> 4;
    lbl_80407BC8 = (f->gap * lbl_80407BD4) >> 4;
    lbl_8040AD14 = ((f->w << 3) * lbl_80407BD0) >> 4;
    lbl_8040AD18 = ((f->w << 3) * lbl_80407BD8) >> 4;
}

/* NONMATCHING 9/37: scheduling of the scale divides and stores; see config/plan.toml. */
void fn_80014180(s32 sx, s32 sy)
{
    lbl_80407BCC = sx;
    lbl_80407BD0 = sy;
    lbl_80407BD4 = sx * lbl_8040AD28 / lbl_8040AD20;
    lbl_80407BD8 = sy * lbl_8040AD2C / lbl_8040AD24;
    lbl_8040AD18 = (lbl_80407BC0->w << 3) * lbl_80407BD8 >> 4;
    lbl_80407BC8 = (lbl_80407BC0->gap * lbl_80407BD4) >> 4;
    lbl_8040AD1C = (lbl_80407BC0->h << 3) * lbl_80407BD8 >> 4;
    lbl_8040AD04 = (lbl_80407BC0->h << 3) * lbl_80407BD0 >> 4;
    lbl_8040AD14 = (lbl_80407BC0->w << 3) * lbl_80407BD0 >> 4;
    lbl_80407BC4 = (lbl_80407BC0->gap * lbl_80407BCC) >> 4;
}

void fn_80014274(struct nutexanimprog_s *p)
{
    s16 pc;
    s32 lp;

    pc = 0;
    while (pc < p->eop) {
        switch (p->code[pc]) {
        case 2:
            pc += 4;
            break;
        case 3:
            pc += 5;
            break;
        case 4:
            pc += 3;
            break;
        case 5:
        case 6:
            break;
        case 7:
            pc += 3;
            break;
        case 11:
            lp = pc + 3;
            pc += 4;
            p->code[lp] = lbl_80312D50[p->code[lp]];
            break;
        case 9:
            lp = pc + 1;
            pc += 2;
            p->code[lp] = lbl_80312D50[p->code[lp]];
            break;
        case 10:
            lp = pc + 1;
            pc += 2;
            p->code[lp] = lbl_80312D50[p->code[lp]];
            break;
        case 12:
            pc += 1;
            break;
        case 13:
            pc += 3;
            break;
        case 14:
            pc += 1;
            break;
        case 15:
            pc += 3;
            break;
        case 16:
            pc += 1;
            break;
        case 17:
            pc += 2;
            break;
        case 0:
            pc += 2;
            break;
        case 1:
            pc += 2;
            break;
        }
    }
}

void fn_800143B8(void *fp)
{
    s32 a = NuFParGetInt(fp);
    s32 b = NuFParGetInt(fp);
    s32 c = NuFParGetInt(fp);
    s32 d = NuFParGetInt(fp);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 3;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = a;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = b;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = c;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = d;
}

void fn_8001448C(void *fp)
{
    s32 a = NuFParGetInt(fp);
    s32 b = NuFParGetInt(fp);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 4;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = (s32)((f32)a * 0.016666668f * 4096.0f);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = (s32)((f32)b * 0.016666668f * 4096.0f);
}

void fn_8001459C(void *fp)
{
    s32 a = NuFParGetInt(fp);
    s32 b = NuFParGetInt(fp);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 7;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = (s32)((f32)a * 0.016666668f * 4096.0f);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = (s32)((f32)b * 0.016666668f * 4096.0f);
}

extern char lbl_8020AA98[256][21];  /* xdeflabtab */
extern char lbl_8018FCB8[];

static inline s32 XDefLabTabFind(char *name)
{
    s32 i;

    if (strlen(name) > 20) {
        name[20] = 0;
    }
    for (i = 0; i < lbl_8040A2F4; i++) {
        if (strcasecmp(lbl_8020AA98[i], name) == 0) {
            return i;
        }
    }
    if (lbl_8040A2F4 > 255) {
        fn_80009948(lbl_8018FC1C, 0x25E)(lbl_8018FCB8);
    }
    NuStrCpy(lbl_8020AA98[lbl_8040A2F4++], name);
    return lbl_8040A2F4 - 1;
}

void fn_800146AC(void *fp)
{
    s32 idx;

    NuFParGetWord(fp);
    idx = XDefLabTabFind((char *)fp + 0x30A);
    lbl_8040A2F8->xdef_ids[lbl_8040A2F8->xdef_cnt] = idx;
    lbl_8040A2F8->xdef_addrs[lbl_8040A2F8->xdef_cnt] = lbl_8040A2F8->eop;
    lbl_8040A2F8->xdef_cnt++;
}

void fn_800147CC(void *fp)
{
    s32 idx;

    NuFParGetWord(fp);
    idx = LabTabFind((char *)fp + 0x30A);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 9;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = idx;
}

void fn_800148F4(void *fp)
{
    s32 idx;

    NuFParGetWord(fp);
    idx = XDefLabTabFind((char *)fp + 0x30A);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 0x11;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = idx;
}

/* pftaBtex (opcode 0xB: condition, value, label) with ParGetCC inlined. */
void pftaBtex(void *fp)
{
    s32 cc;
    s32 val;
    s32 idx;

    cc = ParGetCCInline(fp);
    val = NuFParGetInt(fp);
    NuFParGetWord(fp);
    idx = LabTabFind((char *)fp + 0x30A);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 0xB;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = cc;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = val;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = idx;
}

void fn_80014C5C(void *fp)
{
    s32 idx;

    NuFParGetWord(fp);
    idx = LabTabFind((char *)fp + 0x30A);
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = 0xA;
    lbl_8040A2F8->code[lbl_8040A2F8->eop++] = idx;
}

extern s32 vsprintf(char *buf, char *fmt, void *args);
extern s32 tolower();
extern s32 isdigit();
extern s32 toupper();
extern char lbl_8020A158[];
extern char *lbl_8040A2EC;
extern s32 lbl_80407E00;
extern s32 lbl_8040AD08;
extern s32 lbl_8040AD0C;

/* Formats into a scratch buffer and returns the widest line in fixed-point pixels;
 * "#h", "#d" and digit runs closed by '#' are inline codes with no width. */
s32 fn_8001394C(char *fmt, void *args)
{
    struct nutxtglyph_s *glyph;
    s32 w;
    char *q;
    u8 c;

    w = 0;
    lbl_8040AD08 = 0;
    lbl_8040AD0C = lbl_8040AD04 >> 3;
    if (lbl_80407BC0 != NULL) {
        vsprintf(lbl_8020A158, fmt, args);
        for (lbl_8040A2EC = lbl_8020A158; *lbl_8040A2EC != 0; lbl_8040A2EC++) {
            switch (*lbl_8040A2EC) {
            case '\r':
            case '\n':
                lbl_8040AD08 = (lbl_8040AD08 < w) ? w : lbl_8040AD08;
                w = 0;
                lbl_8040AD0C += lbl_8040AD04 >> 3;
                continue;
            case '#':
                q = lbl_8040A2EC + 1;
                if (tolower(*q) == 'h') {
                    q++;
                }
                if (tolower(*q) == 'd') {
                    q++;
                }
                while (isdigit(*q) != 0) {
                    q++;
                }
                if (*q == '#') {
                    lbl_8040A2EC = q;
                    continue;
                }
                break;
            }
            c = *lbl_8040A2EC;
            if (lbl_80407E00 != 0) {
                c = toupper(c);
            }
            c = lbl_80407BC0->map[c];
            if (c != 0xFF) {
                glyph = lbl_80407BC0->glyph;
                w += (((glyph[c].w - 1) << 4) * lbl_80407BCC) >> 4;
            } else {
                w += (((lbl_80407BC0->glyph[0].w - 1) << 4) * lbl_80407BCC) >> 4;
            }
        }
        lbl_8040AD08 = (lbl_8040AD08 < w) ? w : lbl_8040AD08;
    }
    return lbl_8040AD08;
}

/* Draws one glyph at the text cursor and returns its advance; with lbl_8040AD10 set,
 * digits are centred in the width of '0'. NONMATCHING 90/109: colour word 0 is loaded
 * with lwz+clrlwi in the target, and the font pointer stays in r27 across the call. */
extern void fn_80027868(s32 x, s32 y, s32 w, s32 h, f32 u0, f32 v0, f32 u1, f32 v1, u32 col, void *mtl);
extern s32 lbl_8040AD10;
extern s32 lbl_80407BDC;
extern s32 lbl_80407BE0;
extern s32 lbl_8040AD20;
extern s32 lbl_8040AD24;
extern s32 lbl_8040AD28;
extern s32 lbl_8040AD2C;

s32 fn_80013B28(s32 c)
{
    struct nutxtfont_s *f;
    struct nutxtglyph_s *g;
    struct nutxtglyph_s *dg;
    s32 gw;
    s32 gh;
    s32 adv;
    s32 dw;
    s32 dadv;
    s32 off;
    u8 ix;

    f = lbl_80407BC0;
    dw = 0;
    off = 0;
    if (f != NULL) {
    ix = f->map[(u8)c];
    if (ix != 0xFF) {
        g = &f->glyph[ix];
        gw = ((g->w << 4) * lbl_80407BD4) >> 4;
        gh = (((g->h - 1) << 3) * lbl_80407BD8) >> 4;
        adv = (((g->w + lbl_80407BC4) << 4) * lbl_80407BD4) >> 4;
        if (lbl_8040AD10 != 0 && c > '/' && c <= '9') {
            dg = &f->glyph[f->map['0']];
            if (dg != NULL) {
                dw = ((dg->w << 4) * lbl_80407BD4) >> 4;
                dadv = (((dg->w + lbl_80407BC4) << 4) * lbl_80407BD4) >> 4;
            }
            off = (dw - gw) >> 1;
        }
        fn_80027868((lbl_80407BDC + off) * lbl_8040AD20 / lbl_8040AD28,
                    lbl_80407BE0 * lbl_8040AD24 / lbl_8040AD2C,
                    gw * lbl_8040AD20 / lbl_8040AD28,
                    gh * lbl_8040AD24 / lbl_8040AD2C,
                    g->u0, g->v0, g->u1, g->v1,
                    (u8)lbl_801B14D8[3] << 24 | (u8)lbl_801B14D8[2] << 16 |
                        (u8)lbl_801B14D8[1] << 8 | (u8)lbl_801B14D8[0],
                    f->mtl);
        if (dw != 0) {
            adv = dadv;
        }
    } else {
        adv = (((f->glyph[0].w + lbl_80407BC8) << 4) * lbl_80407BD4) >> 4;
    }
    return adv;
    }
    return 0;
}
