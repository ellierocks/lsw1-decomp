#include "types.h"

extern f32 lbl_801AEB88[];
extern f32 lbl_804099B0;
extern f32 lbl_804099E0;

f32 Condition_GlynTest(void)
{
    return lbl_804099E0;
}

s32 Condition_GlynTestInit(void)
{
    return 0;
}

s32 Condition_NumBaddiesInit(void)
{
    return 0;
}

f32 Condition_CheatProgress(void)
{
    return lbl_801AEB88[0];
}

f32 Condition_DropBackInTimer(void)
{
    return lbl_804099B0;
}

struct gameaiscript_owner_s {
    u8 *object;
};

struct gameaiscript_packet_s {
    u8 opaque[0x50];
    struct gameaiscript_owner_s *owner;
};

extern s32 NuStrICmp(char *, char *);
extern char str_ScriptBehavior_Rotational[];
extern char str_Bool_FALSE[];

s32 Action_SetControlSystem(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        object[0x351] = 0;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_ScriptBehavior_Rotational) == 0)
                object[0x351] = 1;
        }
    }
    return 1;
}

s32 Action_PrefersPlayers(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0) {
        *(u32 *)(object + 0xf28) |= 0x40000000;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    *(u32 *)(object + 0xf28) &= ~0x40000000;
            }
        }
    }
    return 1;
}

extern char str_Bool_False[];

s32 Action_SetBoltsDontGetDeflectedBack(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0) {
        *(u32 *)(object + 0xf28) |= 0x2000000;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    *(u32 *)(object + 0xf28) &= ~0x2000000;
            }
        }
    }
    return 1;
}

s32 Action_CanShootObstructions(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0) {
        *(u32 *)(object + 0xf24) |= 0x1;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    *(u32 *)(object + 0xf24) &= ~0x1;
            }
        }
    }
    return 1;
}

s32 Action_CanHitForceObjects(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0) {
        *(u32 *)(object + 0xf24) |= 0x2000000;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    *(u32 *)(object + 0xf24) &= ~0x2000000;
            }
        }
    }
    return 1;
}

s32 Action_ImmuneToKillTerrain(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf24) |= 0x200;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_False) == 0)
                *(u32 *)(object + 0xf24) &= ~0x200;
        }
    }
    return 1;
}

s32 Action_SetShootOpponents(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf24) |= 0x1000000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_False) == 0)
                *(u32 *)(object + 0xf24) &= ~0x1000000;
        }
    }
    return 1;
}

s32 Action_SetBoss(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf24) |= 0x2;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_False) == 0)
                *(u32 *)(object + 0xf24) &= ~0x2;
        }
    }
    return 1;
}

s32 Action_UpdateSockPos(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf24) |= 0x4000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_False) == 0)
                *(u32 *)(object + 0xf24) &= ~0x4000;
        }
    }
    return 1;
}

s32 Action_ApplyGravity(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf28) &= ~0x20000000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                *(u32 *)(object + 0xf28) |= 0x20000000;
        }
    }
    return 1;
}

extern void *lbl_80409410;
extern s32 fn_8012741C(void *, struct gameaiscript_owner_s *, s32, s32);

f32 Condition_NumForceObjects(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char *name, u32 flags)
{
    s32 include = flags & 1;
    if (packet != 0 && packet->owner != 0 && (flags & 2) != 0)
        return fn_8012741C(lbl_80409410, packet->owner, include, 0);
    else
        return fn_8012741C(lbl_80409410, packet->owner, include, 1);
}

extern f32 fn_80083958(void *, char *);
extern f32 lbl_801AEBD8[];

s32 Action_SetGravityHeight(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        f32 height = lbl_801AEBD8[0];
        *(u32 *)(object + 0xf28) &= ~0x20000000;
        *(f32 *)(object + 0x1058) = height;
        for (i = 0; i < count; i++)
            *(f32 *)(object + 0x1058) = fn_80083958(process, params[i]);
    }
    return 1;
}

extern u8 *lbl_80409004;
extern u8 *lbl_80409008;
extern f32 lbl_801AEA88[];
extern f32 lbl_801AEA98[];
extern f32 lbl_801AEA9C[];
extern f32 lbl_801AEAA8[];
extern s32 fn_8000ABDC(f32, f32);

f32 Condition_AngleFromPlayerToMe(void *sys, void *process,
    struct gameaiscript_packet_s *packet, char *name, void *argument)
{
    u8 *owner;
    u8 *player;
    if (packet == 0 || (owner = (u8 *)packet->owner) == 0 || (player = lbl_80409004) == 0)
        return lbl_801AEA88[0];
    return (f32)fn_8000ABDC(*(f32 *)(owner + 0x6c) - *(f32 *)(player + 0x6c),
        *(f32 *)(owner + 0x74) - *(f32 *)(player + 0x74)) * lbl_801AEA98[0];
}

f32 Condition_AngleFromPlayer2ToMe(void *sys, void *process,
    struct gameaiscript_packet_s *packet, char *name, void *argument)
{
    u8 *owner;
    u8 *player;
    if (packet == 0 || (owner = (u8 *)packet->owner) == 0 || (player = lbl_80409008) == 0)
        return lbl_801AEA9C[0];
    return (f32)fn_8000ABDC(*(f32 *)(owner + 0x6c) - *(f32 *)(player + 0x6c),
        *(f32 *)(owner + 0x74) - *(f32 *)(player + 0x74)) * lbl_801AEAA8[0];
}

s32 fn_800D9A60();

s32 Action_CancelHint(void) {
    fn_800D9A60();
    return 1;
}

extern f32 lbl_801AEB58[];
extern s32 lbl_803E0278[];

f32 Condition_CameraYRot(void) {
    return (f32) (f64) lbl_803E0278[0] * lbl_801AEB58[0];
}

s32 fn_800D99AC();

f32 Condition_CurrentHintId(void) {
    return (f32) (f64) fn_800D99AC();
}

s32 fn_80108E6C(s32);

void Condition_DoorLockedInit(void) {
    fn_80108E6C(lbl_80409410);
}

s32 fn_80108E6C(s32);

void Condition_DoorOpenInit(void) {
    fn_80108E6C(lbl_80409410);
}

s32 fn_80108E6C(s32);

void Condition_DoorOpenedInit(void) {
    fn_80108E6C(lbl_80409410);
}

extern s32 lbl_804091F8;

f32 Condition_Freeplay(void) {
    return (f32) (f64) lbl_804091F8;
}

s32 fn_80108E6C(s32);

void Condition_GadgetActiveInit(void) {
    fn_80108E6C(lbl_80409410);
}

extern s32 lbl_8040AFA8;

f32 Condition_InMiniCut(void) {
    return (f32) (f64) lbl_8040AFA8;
}

s32 fn_8008F720();

void Condition_LocatorOnScreenInit(void) {
    fn_8008F720();
}

s32 NuStrIStr(s32, s32 *);
extern s32 str_Entity_InRange[];
extern s32 str_Entity_Throwable[];

s32 Condition_NumForceObjectsInit(s32 arg0, s32 arg1) {
    s32 var_r31;

    var_r31 = 0;
    if ((arg1 != 0) && (arg0 != 0)) {
        if (NuStrIStr(arg1, str_Entity_Throwable) != 0) {
            var_r31 = 1;
        }
        if (NuStrIStr(arg1, str_Entity_InRange) != 0) {
            var_r31 |= 2;
        }
    }
    return var_r31;
}

s32 fn_80108E6C(s32);

void Condition_ObstacleChainPhaseInit(void) {
    fn_80108E6C(lbl_80409410);
}

s32 fn_80108E6C(s32);

void Condition_ObstacleFinishedInit(void) {
    fn_80108E6C(lbl_80409410);
}

extern f32 lbl_801AE990[];
extern f32 lbl_801AE994[];

f32 Condition_PlayerDeflectingPart(void) {
    if (((void *) lbl_80409004 != NULL) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409004) + (0xEEC)))) + (0x64C))) != 0)) {
        return lbl_801AE990[0];
    }
    return lbl_801AE994[0];
}

extern f32 lbl_801AE9C0[];
extern f32 lbl_801AE9C4[];

f32 Condition_PlayerOnGround(void) {
    if (((void *) lbl_80409004 != NULL) && ((*(s32 *)((s8 *)(lbl_80409004) + (0x284))) & 0xFFFF00)) {
        return lbl_801AE9C0[0];
    }
    return lbl_801AE9C4[0];
}

extern s32 lbl_8040B108;

f32 Condition_RaceLap(void) {
    return (f32) (f64) lbl_8040B108;
}

s32 fn_80109294(s32);

void Condition_TubeActiveInit(void) {
    fn_80109294(lbl_80409410);
}

extern void *lbl_80408DF0;

f32 fn_8012D1CC(s32 arg0) {
    return (*(f32 *)((s8 *)((*(void **)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0x20)))) + (0x84)));
}

extern void *lbl_80408DF0;

f32 fn_8012D1E8(s32 arg0) {
    return (*(f32 *)((s8 *)((*(void **)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0x20)))) + (0x88)));
}

extern void *lbl_80408DF0;

f32 fn_8012D204(s32 arg0) {
    return (*(f32 *)((s8 *)((*(void **)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0x20)))) + (0x8C)));
}

extern void *lbl_80408DF0;

f32 fn_8012D220(s32 arg0) {
    return (*(f32 *)((s8 *)((*(void **)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0x20)))) + (0x90)));
}

s32 fn_80109128(s32);

void fn_80136D84(void) {
    fn_80109128(lbl_80409410);
}
