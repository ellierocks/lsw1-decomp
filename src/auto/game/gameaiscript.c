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

extern char str_ScriptFlag_DontSetMax[];
extern char str_ScriptScope_Default[];

s32 Action_SetHitPoints(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 setMax = 1;
    s32 hp;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0)
            target = packet->owner->object;
        hp = *(u8 *)(*(u8 **)(*(u8 **)(target + 0x64) + 0x20) + 0xc2);
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                target = (u8 *)fn_8012D23C(sys, p + 10, 0);
            else if (NuStrICmp(params[i], str_ScriptFlag_DontSetMax) == 0)
                setMax = 0;
            else if (NuStrICmp(params[i], str_ScriptScope_Default) != 0)
                hp = fn_80083958(packet, params[i]);
        }
        if (target != 0) {
            if (setMax != 0)
                target[0x1084] = hp;
            target[0x1085] = hp;
        }
    }
    return 1;
}

extern char str_ScriptDir_In[];
extern char str_ScriptParam_Pivot[];
extern char str_ScriptScope_Global[];
extern char str_ScriptScope_Default[];

s32 Action_SetPlayType(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_ScriptScope_Global) == 0)
                *(s8 *)(*(u8 **)(object + 0xeec) + 0x7e6) = -1;
            else if (NuStrICmp(params[i], str_ScriptScope_Default) == 0)
                *(s8 *)(*(u8 **)(object + 0xeec) + 0x7e6) = 0;
            else if (NuStrICmp(params[i], str_ScriptDir_In) == 0)
                *(s8 *)(*(u8 **)(object + 0xeec) + 0x7e6) = 1;
            else if (NuStrICmp(params[i], str_ScriptParam_Pivot) == 0)
                *(s8 *)(*(u8 **)(object + 0xeec) + 0x7e6) = 2;
        }
    }
    return 1;
}

/* Character flag word at +0xF24, written by Action_Kill, Action_Respawnable,
 * Action_SetTaggable and Action_CannotDropIn. */
struct gameaiscript_f24_s {
    u32 unk0 : 23;
    u32 respawn : 1;
    u32 respawnAtOrigin : 1;
    u32 unk19 : 3;
    u32 notTaggable : 1;
    u32 cannotDropIn : 1;
    u32 unk1e : 2;
};

extern void fn_800D3054(void *, s32);
extern char str_ScriptCategory_Character[];
extern char str_ScriptParam_Respawn[];
extern char str_ScriptParam_RespawnAtOrigin[];

s32 Action_Kill(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 respawn = 0;
    s32 atOrigin = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptCategory_Character)) != 0)
                target = (u8 *)fn_8012D23C(sys, p + 10, 0);
            else if (NuStrICmp(params[i], str_ScriptParam_Respawn) == 0)
                respawn = 1;
            else if (NuStrICmp(params[i], str_ScriptParam_RespawnAtOrigin) == 0) {
                respawn = 1;
                atOrigin = 1;
            }
        }
        if (target != 0) {
            ((struct gameaiscript_f24_s *)(target + 0xf24))->respawn = respawn;
            ((struct gameaiscript_f24_s *)(target + 0xf24))->respawnAtOrigin = atOrigin;
            fn_800D3054(target, 5);
        }
    }
    return 1;
}

extern char lbl_801AD848[];

s32 Action_CannotDropIn(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 cannot = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                    target = (u8 *)fn_8012D23C(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD848)) != 0)
                    fn_8012D23C(sys, p + 7, 0);
                else if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    cannot = 1;
            }
            if (target != 0)
                ((struct gameaiscript_f24_s *)(target + 0xf24))->cannotDropIn = cannot;
        }
    }
    return 1;
}

extern void fn_8009CB88(s32, s32);
extern void fn_8009CBC4(s32, s32);
extern char str_ScriptParam_IX[];

s32 Action_EnableSock(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 enable = 1;
    s32 ix = -1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_IX)) != 0)
                ix = fn_80083958(packet, p + 3);
            else if (NuStrIStr(params[i], str_Bool_FALSE) != 0)
                enable = 0;
        }
        if (enable != 0)
            fn_8009CB88(*(s32 *)((u8 *)lbl_80409410 + 0x2dc), ix);
        else
            fn_8009CBC4(*(s32 *)((u8 *)lbl_80409410 + 0x2dc), ix);
    }
    return 1;
}

extern f32 lbl_801AEBD0[];

s32 Action_SetWalkSpeed(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(f32 *)(object + 0xf14) = lbl_801AEBD0[0];
        if (count != 0 && NuStrICmp(params[0], str_ScriptScope_Default) != 0)
            *(f32 *)(object + 0xf14) = fn_80083958(packet, params[0]);
    }
    return 1;
}

extern f32 lbl_801AEBCC[];

s32 Action_SetSpeed(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    if (packet != 0 && firstTime != 0) {
        *(f32 *)((u8 *)packet + 0x1bc) = lbl_801AEBCC[0];
        if (count != 0 && NuStrICmp(params[0], str_ScriptScope_Default) != 0)
            *(f32 *)((u8 *)packet + 0x1bc) = fn_80083958(packet, params[0]);
    }
    return 1;
}

s32 Action_DontRaycastLOS(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *obj;
    s32 i;

    if (firstTime != 0 && (obj = *(u8 **)((u8 *)lbl_80409410 + 0x2e0)) != 0) {
        *(u32 *)(obj + 0x210) |= 0x80000000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_False) == 0)
                *(u32 *)(*(u8 **)((u8 *)lbl_80409410 + 0x2e0) + 0x210) &= 0x7fffffff;
        }
    }
    return 1;
}

extern s32 fn_8008F294(void *, char *);
extern void fn_8008F57C(void *, s32);
extern void fn_8008A04C(s32, void *, void *, s32, s8);
extern u8 *gPlayer1Pointer[];
extern char str_ScriptParam_Name[];

s32 Action_SetLevelPath(u8 *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    char *name = 0;
    char *p;
    s32 i;
    u8 *player;

    if (firstTime != 0 && *(u8 **)(sys + 0xc) != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Name)) != 0)
                name = p + 5;
        }
        if (fn_8008F294(sys, name) != 0) {
            for (i = 0; i < 8; i++) {
                player = gPlayer1Pointer[i];
                if (player != 0 && *(s32 *)(player + 0x20c) < 0) {
                    fn_8008F57C(player + 0x2a0, *(s32 *)(*(u8 **)(sys + 0xc) + 8));
                    fn_8008A04C(*(s32 *)((u8 *)lbl_80409410 + 0x2e8), player, player + 0x2a0, 0xff, player[0x285]);
                }
            }
        }
    }
    return 1;
}

extern void fn_800D875C(s32, s32);
extern char str_ScriptParam_ID[];

s32 Action_SetHint(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 id = -1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_ID)) != 0)
                id = fn_80083958(packet, p + 3);
        }
        fn_800D875C(id, 0);
    }
    return 1;
}

extern void fn_800D99C8(s32);

s32 Action_SetHintComplete(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 id = -1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_ID)) != 0)
                id = fn_80083958(packet, p + 3);
        }
        fn_800D99C8(id);
    }
    return 1;
}

extern void fn_8011E9C4(void);

s32 Action_PlayCutScene(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    char *name = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Name)) != 0)
                name = p + 5;
        }
        if (name != 0 && fn_8011E9F8(name) != -1)
            fn_8011E9C4();
    }
    return 1;
}

extern char str_ScriptParam_Origin[];

s32 Action_Respawnable(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        ((struct gameaiscript_f24_s *)(object + 0xf24))->respawn = 1;
        ((struct gameaiscript_f24_s *)(object + 0xf24))->respawnAtOrigin = 0;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_ScriptParam_Origin) == 0)
                ((struct gameaiscript_f24_s *)(object + 0xf24))->respawnAtOrigin = 1;
            else if (NuStrICmp(params[i], str_Bool_False) == 0)
                ((struct gameaiscript_f24_s *)(object + 0xf24))->respawn = 0;
        }
    }
    return 1;
}

extern void fn_80074880(void *, s32);
extern void fn_800B97DC(void);
extern s32 lbl_80408888;
extern u8 *lbl_8040AEFC;

s32 Action_RemoveThrownForceObjects(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;
    u8 *obj = lbl_8040AEFC;

    for (i = 0; i < lbl_80408888; i++, obj += 0x1f4) {
        if (*(s32 *)(obj + 0x128) < 0 && *(void **)(obj + 0x1a8) == (void *)fn_800B97DC)
            fn_80074880(obj, 0);
    }
    return 1;
}

extern void fn_800ABCC8(void);
extern s32 fn_800EC9B8(void *, void *);

s32 Action_TagCharacter(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    void *target = 0;
    void *tagger = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                    target = (void *)fn_8012D23C(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD848)) != 0)
                    tagger = (void *)fn_8012D23C(sys, p + 7, 0);
            }
            if (target != 0) {
                fn_800EC9B8(target, tagger);
                fn_800ABCC8();
            }
        }
    }
    return 1;
}

extern char lbl_801AD9FC[];

s32 Action_SetForceLightningTarget(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *source = 0;
    void *target = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            source = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                source = (u8 *)fn_8012D23C(sys, p + 10, 0);
            else if ((p = NuStrIStr(params[i], lbl_801AD9FC)) != 0)
                target = (void *)fn_8012D23C(sys, p + 7, 0);
        }
        if (source != 0 && target != 0)
            *(void **)(*(u8 **)(source + 0xeec) + 0x654) = target;
    }
    return 1;
}

s32 Action_SetTaggable(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    void *tagger = 0;
    s32 off = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                    target = (u8 *)fn_8012D23C(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD848)) != 0)
                    tagger = (void *)fn_8012D23C(sys, p + 7, 0);
                else if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    off = 1;
            }
            if (target != 0) {
                if (off != 0 && (*(u32 *)(target + 0x20c) & 0x01000000)) {
                    if (fn_800EC9B8(target, tagger) == 0)
                        off = 0;
                    fn_800ABCC8();
                }
                ((struct gameaiscript_f24_s *)(target + 0xf24))->notTaggable = off;
            }
        }
    }
    return 1;
}

extern char str_Entity_Baddie[];
extern char str_Entity_Baddy[];
extern char str_Entity_Goodie[];
extern char str_Entity_Goody[];

s32 Condition_SideInit(void *process, char *param)
{
    if (param != 0) {
        if (NuStrICmp(param, str_Entity_Baddy) == 0 || NuStrICmp(param, str_Entity_Baddie) == 0)
            return -1;
        if (NuStrICmp(param, str_Entity_Goody) == 0 || NuStrICmp(param, str_Entity_Goodie) == 0)
            return 1;
    }
    return 0;
}

extern char *lbl_801D63C4[];

s32 fn_80135CBC(void *process, char *param)
{
    s32 i;

    if (param != 0 && process != 0) {
        for (i = 0; i < 8; i++) {
            if (NuStrICmp(lbl_801D63C4[i], param) == 0)
                return i;
        }
    }
    return -1;
}

extern char lbl_801AD834[];

s32 Action_CanOpenDoors(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0x105c) |= 1;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], lbl_801AD834) == 0)
                *(u32 *)(object + 0x105c) |= 1;
            else if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                *(u32 *)(object + 0x105c) &= ~1;
        }
    }
    return 1;
}

/* Character flag word at +0xF28, written by the SetDontMove family. */
struct gameaiscript_f28_s {
    u32 unk0 : 7;
    u32 dontMove : 1;
    u32 cantDie : 1;
    u32 dontSetStopped : 1;
    u32 unka : 22;
};

s32 Action_SetDontMove(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 on = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptCategory_Character)) != 0)
                target = (u8 *)fn_8012D23C(sys, p + 10, 0);
            else if (NuStrICmp(str_Bool_FALSE, params[0]) == 0)
                on = 0;
        }
        if (target != 0)
            ((struct gameaiscript_f28_s *)(target + 0xf28))->dontMove = on;
    }
    return 1;
}

s32 Action_IgnoreShoveSystem(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf28) |= 0x10000000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                *(u32 *)(object + 0xf28) &= ~0x10000000;
        }
    }
    return 1;
}

s32 Action_CanShootOffScreen(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0x105c) |= 4;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], lbl_801AD834) == 0)
                *(u32 *)(object + 0x105c) |= 4;
            else if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                *(u32 *)(object + 0x105c) &= ~4;
        }
    }
    return 1;
}

s32 Action_CanCollectCoins(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0x105c) |= 2;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], lbl_801AD834) == 0)
                *(u32 *)(object + 0x105c) |= 2;
            else if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                *(u32 *)(object + 0x105c) &= ~2;
        }
    }
    return 1;
}

s32 Action_DontSetStoppedFlag(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 on = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptCategory_Character)) != 0)
                target = (u8 *)fn_8012D23C(sys, p + 10, 0);
            else if (NuStrICmp(str_Bool_FALSE, params[0]) == 0)
                on = 0;
        }
        if (target != 0)
            ((struct gameaiscript_f28_s *)(target + 0xf28))->dontSetStopped = on;
    }
    return 1;
}

s32 Action_CantDie(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 on = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptCategory_Character)) != 0)
                target = (u8 *)fn_8012D23C(sys, p + 10, 0);
            else if (NuStrICmp(str_Bool_FALSE, params[0]) == 0)
                on = 0;
        }
        if (target != 0)
            ((struct gameaiscript_f28_s *)(target + 0xf28))->cantDie = on;
    }
    return 1;
}

s32 Action_CannotBeSeen(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf28) |= 0x08000000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                *(u32 *)(object + 0xf28) &= ~0x08000000;
        }
    }
    return 1;
}

s32 Action_SetDefensive(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf24) |= 0x20000000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_False) == 0)
                *(u32 *)(object + 0xf24) &= ~0x20000000;
        }
    }
    return 1;
}

s32 Action_SetJumping(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf24) |= 0x20000;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_False) == 0)
                *(u32 *)(object + 0xf24) &= ~0x20000;
        }
    }
    return 1;
}

/* Character flag word at +0x20C; bit 0x4000 set by Action_SetIgnoreAntinodes_Level. */
struct gameaiscript_20c_s {
    u32 unk0 : 17;
    u32 ignoreAntinodes : 1;
    u32 unk12 : 14;
};

s32 Action_SetIgnoreAntinodes_Level(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 on = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptCategory_Character)) != 0)
                target = (u8 *)fn_8012D23C(sys, p + 10, 0);
            else if (NuStrICmp(str_Bool_FALSE, params[0]) == 0)
                on = 0;
        }
        if (target != 0)
            ((struct gameaiscript_20c_s *)(target + 0x20c))->ignoreAntinodes = on;
    }
    return 1;
}

s32 fn_800955A8(s32, void *);

s32 Action_AnimTimeRandom(void *sys, void *process, void *arg2) {
    void **temp_r4;
    void *temp_r4_2;

    if (arg2 != NULL) {
        temp_r4 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r4 != NULL) {
            temp_r4_2 = *temp_r4;
            if (temp_r4_2 != NULL) {
                fn_800955A8((*(s32 *)((s8 *)(temp_r4_2) + (0x60))), temp_r4_2 + 8);
            }
        }
    }
    return 1;
}

s32 Action_BreakFormation(void *sys, void *process, void *arg2) {
    void *temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void **)((s8 *)(arg2) + (0xF0)));
        if (temp_r5 != NULL) {
            (*(s32 *)((s8 *)(temp_r5) + (0x50))) = (s32) ((*(s32 *)((s8 *)(temp_r5) + (0x50))) & 0xEFFFFFFF);
        }
    }
    return 1;
}

s32 fn_80106CD8(s32, s32);

s32 Action_CloseDoor(void *sys, void *process, void *packet, s32 *arg3, s32 arg4, s32 arg5) {
    if ((arg5 != 0) && (arg4 != 0)) {
        fn_80106CD8(*arg3, 0);
    }
    return 1;
}

s32 fn_8007EBD0(s32, s32 *);
extern s32 str_ScriptState_InActive[];

s32 Action_DeActivate(void *sys, s32 arg1, void *arg2) {
    void *temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void **)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((s32) (*(s32 *)((s8 *)(temp_r5) + (0))) != 0)) {
            (*(s32 *)((s8 *)(temp_r5) + (0x20C))) = (s32) ((*(s32 *)((s8 *)(temp_r5) + (0x20C))) & 0xFFF7FFFF);
            fn_8007EBD0(arg1, str_ScriptState_InActive);
        }
    }
    return 1;
}

s32 Action_DumpCollectables(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r5_2;

    if ((arg2 == NULL) || (temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5 == NULL) != 0)) || (temp_r5_2 = *temp_r5, ((temp_r5_2 == NULL) != 0))) {
        return 1;
    }
    (*(s32 *)((s8 *)((*(void **)((s8 *)(temp_r5_2) + (0xEEC)))) + (0x140))) = 0;
    return 1;
}

s32 Action_FormationMove(void *sys, void *process, void *arg2) {
    void *temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void **)((s8 *)(arg2) + (0xF0)));
        if (temp_r5 != NULL) {
            (*(s32 *)((s8 *)(temp_r5) + (0x50))) = (s32) ((*(s32 *)((s8 *)(temp_r5) + (0x50))) | 0x10000000);
        }
    }
    return 1;
}

s32 fn_80108710(s32, s32);

s32 Action_LockDoor(void *sys, void *process, void *packet, s32 *arg3, s32 arg4, s32 arg5) {
    if ((arg5 != 0) && (arg4 != 0)) {
        fn_80108710(*arg3, 0);
    }
    return 1;
}

s32 Action_PressActionButton(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r11;
    void *temp_r5_2;

    if ((arg2 == NULL) || (temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5 == NULL) != 0)) || (temp_r5_2 = *temp_r5, ((temp_r5_2 == NULL) != 0))) {
        return 1;
    }
    temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(temp_r5_2) + (0xEEC)))) + (0x61C)));
    (*(s32 *)((s8 *)(temp_r11) + (8))) = (s32) ((*(s32 *)((s8 *)(temp_r11) + (8))) | 0x40);
    (*(s32 *)((s8 *)(temp_r5_2) + (0xF24))) = (s32) ((*(s32 *)((s8 *)(temp_r5_2) + (0xF24))) | 0x08400000);
    return 1;
}

s32 Action_PressJumpButton(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r11;
    void *temp_r5_2;

    if ((arg2 == NULL) || (temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5 == NULL) != 0)) || (temp_r5_2 = *temp_r5, ((temp_r5_2 == NULL) != 0))) {
        return 1;
    }
    temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(temp_r5_2) + (0xEEC)))) + (0x61C)));
    (*(s32 *)((s8 *)(temp_r11) + (8))) = (s32) ((*(s32 *)((s8 *)(temp_r11) + (8))) | 0x20);
    return 1;
}

s32 Action_PressSpecialButton(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r11;
    void *temp_r5_2;

    if ((arg2 == NULL) || (temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5 == NULL) != 0)) || (temp_r5_2 = *temp_r5, ((temp_r5_2 == NULL) != 0))) {
        return 1;
    }
    temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(temp_r5_2) + (0xEEC)))) + (0x61C)));
    (*(s32 *)((s8 *)(temp_r11) + (8))) = (s32) ((*(s32 *)((s8 *)(temp_r11) + (8))) | 0x80);
    return 1;
}

s32 Action_PressTagButton(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r11;
    void *temp_r5_2;

    if ((arg2 == NULL) || (temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5 == NULL) != 0)) || (temp_r5_2 = *temp_r5, ((temp_r5_2 == NULL) != 0))) {
        return 1;
    }
    temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(temp_r5_2) + (0xEEC)))) + (0x61C)));
    (*(s32 *)((s8 *)(temp_r11) + (8))) = (s32) ((*(s32 *)((s8 *)(temp_r11) + (8))) | 0x10);
    return 1;
}

s32 fn_800C71AC();
extern s32 lbl_804093AC;

s32 Action_SetTableLocator(void *sys, void *process, void *arg2, char *param, s32 arg4, s32 arg5) {
    void **temp_r5;
    void *temp_r31;

    if (((s32) lbl_804093AC == 0x7B) && (arg5 != 0) && (arg2 != NULL)) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r31 = *temp_r5;
            if (temp_r31 != NULL) {
                (*(s32 *)((s8 *)(temp_r31) + (0x39C))) = fn_800C71AC();
            }
        }
    }
    return 1;
}

s32 fn_801087C4(s32, s32);

s32 Action_UnLockDoor(void *sys, void *process, void *packet, s32 *arg3, s32 arg4, s32 arg5) {
    if ((arg5 != 0) && (arg4 != 0)) {
        fn_801087C4(*arg3, 0);
    }
    return 1;
}

s32 Action_UseWeapon(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r5_2;

    if ((arg2 == NULL) || (temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5 == NULL) != 0)) || (temp_r5_2 = *temp_r5, ((temp_r5_2 == NULL) != 0))) {
        return 1;
    }
    (*(s32 *)((s8 *)(temp_r5_2) + (0xF28))) = (s32) ((*(s32 *)((s8 *)(temp_r5_2) + (0xF28))) | 0x80000000);
    return 1;
}

extern f32 lbl_801AE8BC[];
extern f32 lbl_801AE8C0[];

f32 Condition_Active(void *sys, void *process, void *arg2) {
    if ((arg2 != NULL) && ((s32) (*(u8 *)((s8 *)(arg2) + (0x16C))) == 2)) {
        return lbl_801AE8BC[0];
    }
    return lbl_801AE8C0[0];
}

extern f32 lbl_801AE9B0[];
extern f32 lbl_801AE9B4[];

f32 Condition_AIOverrideControl(void *sys, void *process, void *arg2, char *param, void *arg4) {
    void **temp_r5;
    void *var_r9;

    var_r9 = NULL;
    if (arg4 != NULL) {
        var_r9 = arg4;
    } else if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            var_r9 = *temp_r5;
        }
    }
    if ((var_r9 != NULL) && ((*(s32 *)((s8 *)(var_r9) + (0x20C))) & 0x800000)) {
        return lbl_801AE9B0[0];
    }
    return lbl_801AE9B4[0];
}

extern f32 lbl_801AE9D0[];
extern f32 lbl_801AE9D4[];

f32 Condition_BeenAlerted(void *sys, void *process, void *arg2) {
    void **temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((s32) (*(s32 *)((s8 *)(*temp_r5) + (0xEFC))) != 0)) {
            return lbl_801AE9D0[0];
        }
    }
    return lbl_801AE9D4[0];
}

extern f32 lbl_801AEA2C[];
extern f32 lbl_801AEA30[];

f32 Condition_BeenHit(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r5_2 = *temp_r5;
            if ((temp_r5_2 != NULL) && (((*(f32 *)((s8 *)(temp_r5_2) + (0x1044))) > lbl_801AEA2C[0]) || ((s32) (*(s32 *)((s8 *)(temp_r5_2) + (0xF24))) < 0))) {
                return lbl_801AEA30[0];
            }
        }
    }
    return lbl_801AEA2C[0];
}

extern f32 lbl_801AEB8C[];
extern f32 lbl_801AEB90[];

f32 Condition_BigJumpComplete(void *sys, void *process, void *arg2) {
    void **temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(*temp_r5) + (0xEEC)))) + (0x7DF))) == 0x24)) {
            return lbl_801AEB8C[0];
        }
    }
    return lbl_801AEB90[0];
}

extern f32 lbl_801AEA24[];
extern f32 lbl_801AEA28[];

f32 Condition_Blocking(void *sys, void *process, void *arg2) {
    u8 temp_r0;
    void **temp_r5;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r5_2 = *temp_r5;
            if ((temp_r5_2 != NULL) && ((temp_r0 = (*(u8 *)((s8 *)((*(void **)((s8 *)(temp_r5_2) + (0xEEC)))) + (0x7DF))), (((s8) temp_r0 == 0xD) != 0)) || ((s8) temp_r0 == 0x1C))) {
                return lbl_801AEA24[0];
            }
        }
    }
    return lbl_801AEA28[0];
}

extern f32 lbl_801AEB80[];
extern f32 lbl_801AEB84[];

f32 Condition_BoltsDontGetDeflectedBack(void *sys, void *process, void *arg2) {
    void **temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((*(s32 *)((s8 *)(*temp_r5) + (0xF28))) & 0x02000000)) {
            return lbl_801AEB80[0];
        }
    }
    return lbl_801AEB84[0];
}

extern f32 lbl_801AEA00[];

f32 Condition_CollectedCount(void *sys, void *process, void *arg2) {
    void **temp_r5_2;
    void *temp_r5;

    if (arg2 != NULL) {
        temp_r5_2 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5_2 != NULL) {
            temp_r5 = *temp_r5_2;
            if (temp_r5 != NULL) {
                return (f32) (f64) (*(u32 *)((s8 *)((*(void **)((s8 *)(temp_r5) + (0xEEC)))) + (0x140)));
            }
        }
    }
    return lbl_801AEA00[0];
}

extern f32 lbl_801AEA70[];

f32 Condition_Context(void *sys, void *process, void *arg2) {
    void **temp_r5_2;
    void *temp_r5;

    if ((arg2 == NULL) || (temp_r5_2 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5_2 == NULL) != 0)) || (temp_r5 = *temp_r5_2, ((temp_r5 == NULL) != 0))) {
        return lbl_801AEA70[0];
    }
    return (f32) (f64) (s8) (*(u8 *)((s8 *)((*(void **)((s8 *)(temp_r5) + (0xEEC)))) + (0x7DF)));
}

extern f32 lbl_801AEAE4[];
extern f32 lbl_801AEAE8[];

f32 Condition_CutSceneFinished(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((*(s32 *)((s8 *)(arg4) + (0x88))) & 0x80000)) {
        return lbl_801AEAE4[0];
    }
    return lbl_801AEAE8[0];
}

extern f32 lbl_801AEAEC[];
extern f32 lbl_801AEAF0[];
extern s32 lbl_80409940;
extern s32 lbl_8040B25C;

f32 Condition_CutScenePlaying(void *sys, void *process, void *packet, char *param, s32 arg4) {
    if ((((s32) lbl_8040B25C != -1) || ((s32) lbl_80409940 != -1)) && ((arg4 == -1) || (arg4 == (s32) lbl_8040B25C) || (arg4 == (s32) lbl_80409940))) {
        return lbl_801AEAEC[0];
    }
    return lbl_801AEAF0[0];
}

extern f32 lbl_801AE8CC[];
extern f32 lbl_801AE8D0[];

f32 Condition_DoorLocked(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((*(s32 *)((s8 *)(arg4) + (0x18))) & 8)) {
        return lbl_801AE8CC[0];
    }
    return lbl_801AE8D0[0];
}

extern f32 lbl_801AE8E4[];
extern f32 lbl_801AE8E8[];

f32 Condition_DoorOpen(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((*(s32 *)((s8 *)(arg4) + (0x18))) & 0x80)) {
        return lbl_801AE8E4[0];
    }
    return lbl_801AE8E8[0];
}

extern f32 lbl_801AE8DC[];
extern f32 lbl_801AE8E0[];

f32 Condition_DoorOpenedByPlayer(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((*(s32 *)((s8 *)(arg4) + (0x18))) & 0x1000) && ((s32) (*(s32 *)((s8 *)(arg4) + (0xA0))) == (s32) lbl_80409004)) {
        return lbl_801AE8DC[0];
    }
    return lbl_801AE8E0[0];
}

extern f32 lbl_801AE8D4[];
extern f32 lbl_801AE8D8[];

f32 Condition_DoorOpened(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((*(s32 *)((s8 *)(arg4) + (0x18))) & 0x1000)) {
        return lbl_801AE8D4[0];
    }
    return lbl_801AE8D8[0];
}

extern f32 lbl_801AE968[];
extern f32 lbl_801AE96C[];

f32 Condition_EitherPlayerIs(void *sys, void *process, void *packet, char *param, s32 arg4) {
    if (((s32) lbl_80409004 == arg4) || ((s32) lbl_80409008 == arg4)) {
        return lbl_801AE968[0];
    }
    return lbl_801AE96C[0];
}

extern f32 lbl_801AEB20[];
extern f32 lbl_801AEB24[];
extern f32 lbl_801AEB28[];

f32 Condition_FinishedSpline(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r5_2;

    if ((arg2 == NULL) || (temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50))), ((temp_r5 == NULL) != 0)) || (temp_r5_2 = *temp_r5, ((temp_r5_2 == NULL) != 0))) {
        return lbl_801AEB20[0];
    }
    if ((s32) (*(s32 *)((s8 *)(temp_r5_2) + (0x69C))) != 0) {
        if ((s32) (*(u8 *)((s8 *)(temp_r5_2) + (0x6A3))) != 0) {
            return lbl_801AEB24[0];
        }
        return lbl_801AEB28[0];
    }
    return lbl_801AEB24[0];
}

extern f32 lbl_801AE998[];
extern f32 lbl_801AE99C[];

f32 Condition_ForceBeingUsed(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((*(u16 *)((s8 *)(arg4) + (0x20))) & 0x1000)) {
        return lbl_801AE998[0];
    }
    return lbl_801AE99C[0];
}

extern f32 lbl_801AE9A0[];
extern f32 lbl_801AE9A4[];

f32 Condition_ForceComplete(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((*(u16 *)((s8 *)(arg4) + (0x20))) & 1)) {
        return lbl_801AE9A0[0];
    }
    return lbl_801AE9A4[0];
}

extern f32 lbl_801AE908[];
extern f32 lbl_801AE90C[];

f32 Condition_GadgetActive(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((s32) (*(u8 *)((s8 *)(arg4) + (0x22))) == 2) && ((s32) (*(u8 *)((s8 *)(arg4) + (0x23))) != 0)) {
        return lbl_801AE908[0];
    }
    return lbl_801AE90C[0];
}

extern f32 lbl_801AE8C4[];
extern f32 lbl_801AE8C8[];

f32 Condition_GotBlaster(void *sys, void *process, void *arg2) {
    void *temp_r5;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void **)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r5_2 = (*(void **)((s8 *)(temp_r5) + (0x64)));
            if ((temp_r5_2 != NULL) && ((*(s32 *)((s8 *)(temp_r5_2) + (4))) & 0x80)) {
                return lbl_801AE8C4[0];
            }
        }
    }
    return lbl_801AE8C8[0];
}

extern f32 lbl_801AEA50[];

f32 Condition_HitPoints(void *sys, void *process, void *arg2, char *param, void *arg4) {
    void **temp_r5;
    void *var_r7;

    var_r7 = arg4;
    if (var_r7 == NULL) {
        if (arg2 != NULL) {
            temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
            if (temp_r5 != NULL) {
                var_r7 = *temp_r5;
            }
        }
        if (var_r7 == NULL) {
            return lbl_801AEA50[0];
        }
        goto block_6;
    }
block_6:
    return (f32) (f64) (s8) (*(u8 *)((s8 *)(var_r7) + (0x1085)));
}

extern f32 lbl_801AEA40[];

f32 Condition_HoverPhase(void *sys, void *process, void *arg2) {
    void **temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            return (f32) (f64) (*(u8 *)((s8 *)((*(void **)((s8 *)(*temp_r5) + (0xEEC)))) + (0x7CB)));
        }
    }
    return lbl_801AEA40[0];
}

extern f32 lbl_801AE950[];
extern f32 lbl_801AE954[];

f32 Condition_IAmA(void *sys, void *process, void *arg2, char *param, s32 arg4) {
    void **temp_r5;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r5_2 = *temp_r5;
            if ((temp_r5_2 != NULL) && ((s16) (*(s16 *)((s8 *)(temp_r5_2) + (0x1074))) == arg4)) {
                return lbl_801AE950[0];
            }
        }
    }
    return lbl_801AE954[0];
}

extern f32 lbl_801AE948[];
extern f32 lbl_801AE94C[];

f32 Condition_IAmAPartyCharacter(void *sys, void *process, void *arg2) {
    void *temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void **)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((s8) (*(u8 *)((s8 *)(temp_r5) + (0x284))) != -1)) {
            return lbl_801AE948[0];
        }
    }
    return lbl_801AE94C[0];
}

extern f32 lbl_801AE930[];
extern f32 lbl_801AE934[];

f32 Condition_IAm_Level(void *sys, void *process, void *arg2, s8 *arg3, s32 arg4) {
    s8 *temp_r3_2;
    void **temp_r5;
    void *temp_r3;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((temp_r5_2 = *temp_r5, ((temp_r5_2 == arg4) != 0)) || ((temp_r3 = (*(void **)((s8 *)(temp_r5_2) + (0x64))), ((temp_r3 == NULL) == 0)) && (temp_r3_2 = (*(s8 **)((s8 *)(temp_r3) + (0xC))), ((temp_r3_2 == NULL) == 0)) && (NuStrICmp(temp_r3_2, arg3) == 0)))) {
            return lbl_801AE930[0];
        }
    }
    return lbl_801AE934[0];
}

extern f32 lbl_801AE9B8[];
extern f32 lbl_801AE9BC[];

f32 Condition_IsOnScreen(void *sys, void *process, void *arg2, char *param, void *arg4) {
    void **temp_r5;
    void *var_r9;

    var_r9 = NULL;
    if (arg4 != NULL) {
        var_r9 = arg4;
    } else if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            var_r9 = *temp_r5;
        }
    }
    if ((var_r9 != NULL) && ((s32) (*(u8 *)((s8 *)(var_r9) + (0x28C))) != 0)) {
        return lbl_801AE9B8[0];
    }
    return lbl_801AE9BC[0];
}

extern f32 lbl_801AEA68[];
extern f32 lbl_801AEA6C[];
extern u8 lbl_80409998;

f32 Condition_IsSetAlive(void *sys, void *process, void *arg2, char *param, s32 arg4) {
    if ((arg2 != NULL) && ((s32) (*(s32 *)((s8 *)(arg2) + (0x50))) != 0) && (lbl_80409998 & arg4)) {
        return lbl_801AEA68[0];
    }
    return lbl_801AEA6C[0];
}

s32 NuCameraClipTestBoundingSphere(s32, s32 *, f32);
extern f32 lbl_801AEA1C[];
extern f32 lbl_801AEA20[];
extern s32 lbl_801B0F08[];

f32 Condition_LocatorOnScreen(void *sys, void *process, void *packet, char *param, s32 arg4) {
    if ((arg4 != 0) && (NuCameraClipTestBoundingSphere(arg4 + 0x10, lbl_801B0F08, lbl_801AEA1C[0]) != 0)) {
        return lbl_801AEA1C[0];
    }
    return lbl_801AEA20[0];
}

extern f32 lbl_801AE9C8[];
extern f32 lbl_801AE9CC[];

f32 Condition_OnGround(void *sys, void *process, void *arg2) {
    void **temp_r5;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r5_2 = *temp_r5;
            if ((temp_r5_2 != NULL) && ((*(s32 *)((s8 *)(temp_r5_2) + (0x284))) & 0xFFFF00)) {
                return lbl_801AE9C8[0];
            }
        }
    }
    return lbl_801AE9CC[0];
}

extern f32 lbl_801AE9F0[];
extern f32 lbl_801AE9F4[];

f32 Condition_OnSameObjectAsPlayer(void *sys, void *process, void *arg2) {
    s16 temp_r5_3;
    void **temp_r5;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((void *) lbl_80409004 != NULL)) {
            temp_r5_2 = *temp_r5;
            if (((*(s32 *)((s8 *)(temp_r5_2) + (0x284))) & 0xFFFF00) && ((*(s32 *)((s8 *)(lbl_80409004) + (0x284))) & 0xFFFF00)) {
                temp_r5_3 = (*(s16 *)((s8 *)(temp_r5_2) + (0x282)));
                if ((temp_r5_3 != -1) && (temp_r5_3 == (s16) (*(s16 *)((s8 *)(lbl_80409004) + (0x282))))) {
                    return lbl_801AE9F0[0];
                }
            }
        }
    }
    return lbl_801AE9F4[0];
}

extern f32 lbl_801AEB94[];
extern f32 lbl_801AEB98[];

f32 Condition_RespawnLocatorIs(void *sys, void *process, void *arg2, char *param, s32 arg4) {
    if ((arg4 != 0) && (arg2 != NULL) && ((s32) (*(s32 *)((s8 *)(arg2) + (0x164))) == arg4)) {
        return lbl_801AEB94[0];
    }
    return lbl_801AEB98[0];
}

extern f32 lbl_801AEB78[];
extern f32 lbl_801AEB7C[];

f32 Condition_StuckDontJumpNow(void *sys, void *process, void *arg2) {
    if ((arg2 != NULL) && ((*(s32 *)((s8 *)(arg2) + (0x1A0))) & 0x80)) {
        return lbl_801AEB78[0];
    }
    return lbl_801AEB7C[0];
}

extern f32 lbl_801AEA54[];

f32 Condition_StuckTime_Level(void *sys, void *process, void *arg2) {
    void **temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            return (*(f32 *)((s8 *)(*temp_r5) + (0x1E8)));
        }
    }
    return lbl_801AEA54[0];
}

extern f32 lbl_801AE910[];
extern f32 lbl_801AE914[];

f32 Condition_TubeActive(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && ((s32) (*(u8 *)((s8 *)(arg4) + (0x2C))) != 0)) {
        return lbl_801AE910[0];
    }
    return lbl_801AE914[0];
}

extern f32 lbl_801AE928[];
extern f32 lbl_801AE92C[];

f32 Condition_TurretAlive(void *sys, void *process, void *packet, char *param, void *arg4) {
    if ((arg4 != NULL) && !((*(u16 *)((s8 *)(arg4) + (0x6E))) & 0x80)) {
        return lbl_801AE928[0];
    }
    return lbl_801AE92C[0];
}

extern f32 lbl_801AE920[];
extern f32 lbl_801AE924[];

f32 Condition_UsingForceLightning(void *sys, void *process, void *arg2) {
    void **temp_r5;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(*temp_r5) + (0xEEC)))) + (0x7DF))) == 0x27)) {
            return lbl_801AE920[0];
        }
    }
    return lbl_801AE924[0];
}

extern f32 lbl_8040900C;

s32 Action_SetDefaultMovementRange(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++)
            lbl_8040900C = fn_80083958(process, params[i]);
    }
    return 1;
}

extern f32 lbl_801AEB68[];
extern f32 lbl_801AEB6C[];

f32 Condition_FromNode(void *sys, void *process, void *packet, char *name, s32 node)
{
    if (packet != 0 && node != 0 && sys != 0
        && *(s32 *)(*(u8 **)((u8 *)packet + 0x10c) + 0x78)
               + (*(u8 **)((u8 *)packet + 0x110))[*(u8 *)((u8 *)packet + 0x114)] * 0x58 == node)
        return lbl_801AEB68[0];
    return lbl_801AEB6C[0];
}

extern s32 fn_80108DA8(void *, s32);
extern f32 lbl_801AE900[];
extern f32 lbl_801AE904[];

f32 Condition_ObstacleChainPhase(void *sys, void *process, void *packet, char *name, void *obstacle)
{
    s32 chain;

    if (obstacle != 0 && (chain = *(s8 *)((u8 *)obstacle + 0x71)) != -1)
        return (f32)fn_80108DA8(lbl_80409410, chain) + lbl_801AE900[0];
    return lbl_801AE904[0];
}

extern s32 fn_80103494(void *);
extern f32 lbl_801AEB5C[];

f32 Condition_UsingForceFreeze(void *sys, void *process, struct gameaiscript_packet_s *packet)
{
    if (packet == 0 || packet->owner == 0 || packet->owner->object == 0)
        return lbl_801AEB5C[0];
    return fn_80103494(packet->owner->object);
}

extern s32 fn_8012CCCC(struct gameaiscript_owner_s *);
extern f32 lbl_801AE958[];
extern f32 lbl_801AE95C[];

f32 Condition_CanFightLikeAJedi(void *sys, void *process, struct gameaiscript_packet_s *packet)
{
    if (packet != 0 && packet->owner != 0 && packet->owner->object != 0 && fn_8012CCCC(packet->owner) != 0)
        return lbl_801AE958[0];
    return lbl_801AE95C[0];
}

extern s32 fn_800CC284(struct gameaiscript_owner_s *, s32);
extern f32 lbl_801AE938[];
extern f32 lbl_801AE93C[];

f32 Condition_CategoryIs(void *sys, void *process, struct gameaiscript_packet_s *packet, char *name, s32 category)
{
    if (category != -1 && packet != 0 && packet->owner != 0 && fn_800CC284(packet->owner, category) != 0)
        return lbl_801AE938[0];
    return lbl_801AE93C[0];
}

extern s32 fn_800952FC(s32, char *);
extern void fn_80095558(void *, s32);

s32 Action_SetAnimation(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 anim = -1;
    u8 *object;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        if (count == 1)
            anim = fn_800952FC(*(s32 *)(object + 0x64), params[0]);
        if (anim != -1)
            fn_80095558(object + 8, anim);
    }
    return 1;
}

extern f32 lbl_801AE940[];
extern f32 lbl_801AE944[];

f32 Condition_PlayerCategoryIs(void *sys, void *process, void *packet, char *name, s32 category)
{
    if (category != -1 && lbl_80409004 != 0 && fn_800CC284((struct gameaiscript_owner_s *)lbl_80409004, category) != 0)
        return lbl_801AE940[0];
    return lbl_801AE944[0];
}

extern void fn_800C749C(void *);
extern s32 lbl_804093AC;

s32 Action_CallDexter(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    if (lbl_804093AC == 0x7b && firstTime != 0 && packet != 0 && packet->owner != 0 && packet->owner->object != 0)
        fn_800C749C(packet->owner->object);
    return 1;
}

extern f32 lbl_801AEA10[];

f32 Condition_SpawnCount(void *sys, void *process, void *packet)
{
    if (packet != 0)
        return *(u32 *)((u8 *)packet + 0x1b4);
    return lbl_801AEA10[0];
}

extern f32 lbl_801AEB14[];
extern f32 lbl_801AEB18[];
extern f32 lbl_801AEB1C[];

f32 Condition_IsSplineSet(void *sys, void *process, struct gameaiscript_packet_s *packet)
{
    u8 *object;

    if (packet == 0 || packet->owner == 0 || (object = packet->owner->object) == 0)
        return lbl_801AEB14[0];
    if (*(s32 *)(object + 0x69c) != 0)
        return lbl_801AEB18[0];
    return lbl_801AEB1C[0];
}

extern void fn_80106A30(char *, s32, s32, s32, f32);
extern char lbl_801AD820[];
extern f32 lbl_801AEBE8[];

s32 Action_OpenDoor(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;
    char *door = 0;
    s32 instant = 0;

    if (firstTime != 0 && count != 0) {
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], lbl_801AD820) == 0)
                instant = 1;
            else
                door = params[i];
        }
        if (door != 0)
            fn_80106A30(door, 0, 1, instant, lbl_801AEBE8[0]);
    }
    return 1;
}

/* Script flag bytes in the level data at lbl_80409410+0x1B4, indexed by owner+0x291. */
struct gameaiscript_flags_s { u8 unk0[0x10]; u8 flags[1]; };
extern f32 lbl_801AE970[];
extern f32 lbl_801AE974[];

f32 Condition_CheckFlag(void *sys, void *process, struct gameaiscript_packet_s *packet, char *name, s32 flag)
{
    if (packet != 0 && packet->owner != 0
        && (*(struct gameaiscript_flags_s **)((u8 *)lbl_80409410 + 0x1b4))->flags[((u8 *)packet->owner)[0x291]] & flag)
        return lbl_801AE970[0];
    return lbl_801AE974[0];
}

s32 Action_DeflectPlayersPart(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;
    u8 *object;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf28) |= 0x80000;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    *(u32 *)(object + 0xf28) &= ~0x80000;
            }
        }
    }
    return 1;
}

extern void fn_8008F45C(void *, s32, void *, s32, f32, f32);
extern s32 (*lbl_80408D2C)(char *, void *);
extern f32 lbl_801AEBAC[];

s32 Action_GoToCollectable(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;
    u8 *object;
    s32 target;

    if (packet == 0 || packet->owner == 0 || (object = packet->owner->object) == 0)
        return 1;
    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C == 0 || lbl_80408D2C(params[i], (u8 *)packet + 0xb0) == 0)
                *(f32 *)((u8 *)packet + 0x134) = fn_80083958(packet, params[i]);
        }
    }
    if ((target = *(s32 *)(object + 0x660)) != 0) {
        if (*(s32 *)(object + 0x664) == 0)
            fn_8008F45C(packet, target, object + 0x66c, 1, lbl_801AEBAC[0], lbl_801AEBAC[0]);
        else
            *(s32 *)(object + 0x660) = 0;
    }
    return 0;
}

extern s32 NuSpecialFind(void *, void *, char *, s32);
extern void fn_800135B0(void *);
extern s32 fn_8005A5F8(void);
extern s32 lbl_8040AE4C;

s32 Condition_OnObjectInit(void *process, char *param)
{
    struct { s32 a, b, c; } special;

    if (lbl_8040AE4C != 0 && NuSpecialFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), &special, param, 1) != 0) {
        fn_800135B0(&special);
        return fn_8005A5F8();
    }
    return -1;
}

s32 Condition_PlayerOnObjectInit(void *process, char *param)
{
    struct { s32 a, b, c; } special;

    if (lbl_8040AE4C != 0 && NuSpecialFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), &special, param, 1) != 0) {
        fn_800135B0(&special);
        return fn_8005A5F8();
    }
    return -1;
}

s32 Condition_EitherPlayerOnObjectInit(void *process, char *param)
{
    struct { s32 a, b, c; } special;

    if (lbl_8040AE4C != 0 && NuSpecialFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), &special, param, 1) != 0) {
        fn_800135B0(&special);
        return fn_8005A5F8();
    }
    return -1;
}

extern s32 fn_800130F4(void *);
extern s32 fn_8001384C(void *);

s32 Condition_AnimationFinishedInit(void *process, char *param)
{
    s32 special[3];

    NuSpecialFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), special, param, 1);
    if (fn_800130F4(special) != 0)
        return fn_8001384C(special);
    return 0;
}

s32 Condition_RigidAnimFrameInit(void *process, char *param)
{
    s32 special[3];

    NuSpecialFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), special, param, 1);
    if (fn_800130F4(special) != 0)
        return fn_8001384C(special);
    return 0;
}

extern f32 lbl_801AE978[];
extern f32 lbl_801AE97C[];

f32 Condition_CheckLevelFlag(void *sys, void *process, void *packet, char *name, u32 flag)
{
    if (**(u32 **)((u8 *)lbl_80409410 + 0x1b4) & flag)
        return lbl_801AE978[0];
    return lbl_801AE97C[0];
}

extern u32 lbl_8040B188;
extern f32 lbl_801AE980[];
extern f32 lbl_801AE984[];

f32 Condition_CheckAreaFlag(void *sys, void *process, void *packet, char *name, u32 flag)
{
    if (lbl_8040B188 & flag)
        return lbl_801AE980[0];
    return lbl_801AE984[0];
}

extern f32 lbl_801AEACC[];

f32 Condition_Message(void *sys, void *process, void *packet, char *name, void *message)
{
    if (message != 0)
        return *(f32 *)((u8 *)message + 0x28);
    return lbl_801AEACC[0];
}

extern f32 lbl_801AEAD8[];

f32 Condition_RigidAnimFrame(void *sys, void *process, void *packet, char *name, void *rigid)
{
    if (rigid != 0)
        return *(f32 *)((u8 *)rigid + 0x4c);
    return lbl_801AEAD8[0];
}

extern u8 lbl_803EACC4[];
extern f32 lbl_801AEADC[];
extern f32 lbl_801AEAE0[];

f32 Condition_CutSceneStarted(void *sys, void *process, void *packet, char *name, s32 index)
{
    if (lbl_803EACC4[index] != 0)
        return lbl_801AEADC[0];
    return lbl_801AEAE0[0];
}

extern s32 fn_800D8664(void *);
extern f32 lbl_801AEB38[];
extern f32 lbl_801AEB3C[];

f32 Condition_HintAvailable(void *sys, void *process, void *packet, char *name, void *hint)
{
    if (hint != 0 && fn_800D8664(hint) != 0)
        return lbl_801AEB38[0];
    return lbl_801AEB3C[0];
}

extern s32 fn_800D98F0(void *);
extern f32 lbl_801AEB40[];
extern f32 lbl_801AEB44[];

f32 Condition_HintComplete(void *sys, void *process, void *packet, char *name, void *hint)
{
    if (hint != 0 && fn_800D98F0(hint) != 0)
        return lbl_801AEB40[0];
    return lbl_801AEB44[0];
}

s32 fn_800AFF58(s32 *);

s32 Action_ResetGameCamera(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime)
{
    if (firstTime != 0) {
        fn_800AFF58(MenuHeader);
    }
    return 1;
}
