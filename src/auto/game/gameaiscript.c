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

extern char *NuStrIStr(char *, char *);
extern char str_Entity_InRange[];
extern char str_Entity_Throwable[];

s32 Condition_NumForceObjectsInit(void *process, char *param)
{
    s32 flags = 0;

    if (param != 0 && process != 0) {
        if (NuStrIStr(param, str_Entity_Throwable) != 0)
            flags = 1;
        if (NuStrIStr(param, str_Entity_InRange) != 0)
            flags |= 2;
    }
    return flags;
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

f32 fn_8012D1E8(s32 arg0) {
    return (*(f32 *)((s8 *)((*(void **)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0x20)))) + (0x88)));
}

f32 fn_8012D204(s32 arg0) {
    return (*(f32 *)((s8 *)((*(void **)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0x20)))) + (0x8C)));
}

f32 fn_8012D220(s32 arg0) {
    return (*(f32 *)((s8 *)((*(void **)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0x20)))) + (0x90)));
}

s32 fn_80109128(s32);

void fn_80136D84(void) {
    fn_80109128(lbl_80409410);
}

u32 NuAToI(char *);

s32 Condition_CheckFlagInit(void *process, char *param)
{
    u32 bit;

    if (param != 0) {
        bit = NuAToI(param);
        if (bit <= 0x1FU) {
            return 1 << bit;
        }
    }
    return 0;
}

s32 Condition_IsSetAliveInit(void *process, char *param)
{
    u32 bit;

    if (param != 0) {
        bit = NuAToI(param);
        if (bit <= 7U) {
            return 1 << bit;
        }
    }
    return 0;
}

s32 Condition_CheckLevelFlagInit(void *process, char *param)
{
    u32 bit;

    if (param != 0) {
        bit = NuAToI(param);
        if (bit <= 0x1FU) {
            return 1 << bit;
        }
    }
    return 0;
}

s32 Condition_CheckAreaFlagInit(void *process, char *param)
{
    u32 bit;

    if (param != 0) {
        bit = NuAToI(param);
        if (bit <= 0x1FU) {
            return 1 << bit;
        }
    }
    return 0;
}

s32 fn_8012724C(void *, char *, s32);

s32 Condition_ForceCompleteInit(void *process, char *param)
{
    return fn_8012724C(lbl_80409410, param, 0);
}

s32 Condition_PlayerUsingForceInit(void *process, char *param)
{
    return fn_8012724C(lbl_80409410, param, 0);
}

s32 Condition_ForceBeingUsedInit(void *process, char *param)
{
    return fn_8012724C(lbl_80409410, param, 0);
}

s32 Condition_EitherPlayerUsingForceInit(void *process, char *param)
{
    return fn_8012724C(lbl_80409410, param, 0);
}

s32 fn_8012D23C(void *, char *, s32);

s32 Condition_EitherPlayerIsInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = fn_8012D23C(process, param, 0);
    }
    return result;
}

s32 Condition_AIOverrideControlInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = fn_8012D23C(process, param, 0);
    }
    return result;
}

s32 Condition_IsOnScreenInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = fn_8012D23C(process, param, 0);
    }
    return result;
}

s32 Condition_HitPointsInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = fn_8012D23C(process, param, 0);
    }
    return result;
}

s32 fn_80135C04(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = fn_8012D23C(process, param, 0);
    }
    return result;
}

s32 Condition_UsingForceInit(void *process, char *param)
{
    return fn_8012724C(lbl_80409410, param, 0);
}

s32 Condition_IsAliveInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = fn_8012D23C(process, param, 0);
    }
    return result;
}

s32 fn_8011E9F8(char *);

s32 Condition_CutSceneStartedInit(void *process, char *param)
{
    return fn_8011E9F8(param);
}

s32 Condition_CutScenePlayingInit(void *process, char *param)
{
    return fn_8011E9F8(param);
}

extern void fn_800D407C(void *, void *, s32);

extern char str_ScriptParam_Character[];
extern char lbl_801AD850[];
extern char lbl_801AD858[];

s32 Action_TakeDamage(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    void *target = 0;
    void *source = 0;
    s32 amount = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                    target = (void *)fn_8012D23C(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD850)) != 0)
                    source = (void *)fn_8012D23C(sys, p + 7, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD858)) != 0)
                    amount = fn_80083958(packet, p + 7);
            }
            if (target != 0)
                fn_800D407C(source, target, amount);
        }
    }
    return 1;
}

extern void fn_80103F64(void);
extern void fn_80103F70(void *, f32);
extern char lbl_801AD9F4[];
extern f32 lbl_801AEBF4[];

s32 Action_SetForceBack(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    f32 force = lbl_801AEBF4[0];
    void *target = 0;
    s32 enable = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                target = (void *)fn_8012D23C(sys, p + 10, 0);
            else if ((p = NuStrIStr(params[i], lbl_801AD9F4)) != 0)
                force = fn_80083958(packet, p + 7);
            else if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                enable = 0;
        }
        if (enable != 0) {
            if (target != 0)
                fn_80103F70(target, force);
        } else {
            fn_80103F64();
        }
    }
    return 1;
}

extern void fn_800B02E0(void *, f32, f32, f32);
extern u8 MenuHeader[];
extern f32 lbl_801AEBE0[];
extern f32 lbl_801AEBE4[];
extern char str_ScriptParam_Mul[];
extern char str_ScriptParam_SpeedEq[];
extern char str_ScriptParam_TimeEq[];

s32 Action_CameraShake(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    f32 mul = lbl_801AEBE0[0];
    f32 duration = lbl_801AEBE4[0];
    f32 speed = mul;
    char *p;
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Mul)) != 0)
                mul = fn_80083958(packet, p + 4);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_TimeEq)) != 0)
                duration = fn_80083958(packet, p + 5);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_SpeedEq)) != 0)
                speed = fn_80083958(packet, p + 6);
        }
        fn_800B02E0(MenuHeader, mul, duration, speed);
    }
    return 1;
}
