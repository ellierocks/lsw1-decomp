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

extern f32 AIParamToFloat(void *, char *);
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
            *(f32 *)(object + 0x1058) = AIParamToFloat(process, params[i]);
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

s32 CancelHint();

s32 Action_CancelHint(void) {
    CancelHint();
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

s32 FindObstacle(s32);

void Condition_DoorLockedInit(void) {
    FindObstacle(lbl_80409410);
}

s32 FindObstacle(s32);

void Condition_DoorOpenInit(void) {
    FindObstacle(lbl_80409410);
}

s32 FindObstacle(s32);

void Condition_DoorOpenedInit(void) {
    FindObstacle(lbl_80409410);
}

extern s32 lbl_804091F8;

f32 Condition_Freeplay(void) {
    return (f32) (f64) lbl_804091F8;
}

s32 FindObstacle(s32);

void Condition_GadgetActiveInit(void) {
    FindObstacle(lbl_80409410);
}

extern s32 lbl_8040AFA8;

f32 Condition_InMiniCut(void) {
    return (f32) (f64) lbl_8040AFA8;
}

s32 AIPathFindLocator();

void Condition_LocatorOnScreenInit(void) {
    AIPathFindLocator();
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

s32 FindObstacle(s32);

void Condition_ObstacleChainPhaseInit(void) {
    FindObstacle(lbl_80409410);
}

s32 FindObstacle(s32);

void Condition_ObstacleFinishedInit(void) {
    FindObstacle(lbl_80409410);
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

s32 FindForceObject(void *, char *, s32);

s32 Condition_ForceCompleteInit(void *process, char *param)
{
    return FindForceObject(lbl_80409410, param, 0);
}

s32 Condition_PlayerUsingForceInit(void *process, char *param)
{
    return FindForceObject(lbl_80409410, param, 0);
}

s32 Condition_ForceBeingUsedInit(void *process, char *param)
{
    return FindForceObject(lbl_80409410, param, 0);
}

s32 Condition_EitherPlayerUsingForceInit(void *process, char *param)
{
    return FindForceObject(lbl_80409410, param, 0);
}

s32 GetNamedGameObject(void *, char *, s32);

s32 Condition_EitherPlayerIsInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = GetNamedGameObject(process, param, 0);
    }
    return result;
}

s32 Condition_AIOverrideControlInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = GetNamedGameObject(process, param, 0);
    }
    return result;
}

s32 Condition_IsOnScreenInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = GetNamedGameObject(process, param, 0);
    }
    return result;
}

s32 Condition_HitPointsInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = GetNamedGameObject(process, param, 0);
    }
    return result;
}

s32 fn_80135C04(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = GetNamedGameObject(process, param, 0);
    }
    return result;
}

s32 Condition_UsingForceInit(void *process, char *param)
{
    return FindForceObject(lbl_80409410, param, 0);
}

s32 Condition_IsAliveInit(void *process, char *param)
{
    s32 result = 0;
    if (param != 0 && process != 0) {
        result = GetNamedGameObject(process, param, 0);
    }
    return result;
}

s32 FindCutScene(char *);

s32 Condition_CutSceneStartedInit(void *process, char *param)
{
    return FindCutScene(param);
}

s32 Condition_CutScenePlayingInit(void *process, char *param)
{
    return FindCutScene(param);
}

extern void ObjHitObj(void *, void *, s32);

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
                    target = (void *)GetNamedGameObject(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD850)) != 0)
                    source = (void *)GetNamedGameObject(sys, p + 7, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD858)) != 0)
                    amount = AIParamToFloat(packet, p + 7);
            }
            if (target != 0)
                ObjHitObj(source, target, amount);
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
                target = (void *)GetNamedGameObject(sys, p + 10, 0);
            else if ((p = NuStrIStr(params[i], lbl_801AD9F4)) != 0)
                force = AIParamToFloat(packet, p + 7);
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
                mul = AIParamToFloat(packet, p + 4);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_TimeEq)) != 0)
                duration = AIParamToFloat(packet, p + 5);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_SpeedEq)) != 0)
                speed = AIParamToFloat(packet, p + 6);
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
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            else if (NuStrICmp(params[i], str_ScriptFlag_DontSetMax) == 0)
                setMax = 0;
            else if (NuStrICmp(params[i], str_ScriptScope_Default) != 0)
                hp = AIParamToFloat(packet, params[i]);
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
    u32 unk0 : 3;
    u32 keepWeaponOut : 1; /* Action_KeepWeaponOut */
    u32 unk4 : 19;
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
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
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
                    target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD848)) != 0)
                    GetNamedGameObject(sys, p + 7, 0);
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
                ix = AIParamToFloat(packet, p + 3);
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
            *(f32 *)(object + 0xf14) = AIParamToFloat(packet, params[0]);
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
            *(f32 *)((u8 *)packet + 0x1bc) = AIParamToFloat(packet, params[0]);
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

extern s32 AISysSetLevelPath(void *, char *);
extern void AISysCharacterSetPath(void *, s32);
extern void AISysGetCharacterPathPos(s32, void *, void *, s32, s8);
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
        if (AISysSetLevelPath(sys, name) != 0) {
            for (i = 0; i < 8; i++) {
                player = gPlayer1Pointer[i];
                if (player != 0 && *(s32 *)(player + 0x20c) < 0) {
                    AISysCharacterSetPath(player + 0x2a0, *(s32 *)(*(u8 **)(sys + 0xc) + 8));
                    AISysGetCharacterPathPos(*(s32 *)((u8 *)lbl_80409410 + 0x2e8), player, player + 0x2a0, 0xff, player[0x285]);
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
                id = AIParamToFloat(packet, p + 3);
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
                id = AIParamToFloat(packet, p + 3);
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
        if (name != 0 && FindCutScene(name) != -1)
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
extern void PartKill_ForceThrow(void);
extern s32 lbl_80408888;
extern u8 *lbl_8040AEFC;

s32 Action_RemoveThrownForceObjects(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;
    u8 *obj = lbl_8040AEFC;

    for (i = 0; i < lbl_80408888; i++, obj += 0x1f4) {
        if (*(s32 *)(obj + 0x128) < 0 && *(void **)(obj + 0x1a8) == (void *)PartKill_ForceThrow)
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
                    target = (void *)GetNamedGameObject(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD848)) != 0)
                    tagger = (void *)GetNamedGameObject(sys, p + 7, 0);
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
                source = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            else if ((p = NuStrIStr(params[i], lbl_801AD9FC)) != 0)
                target = (void *)GetNamedGameObject(sys, p + 7, 0);
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
                    target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD848)) != 0)
                    tagger = (void *)GetNamedGameObject(sys, p + 7, 0);
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
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
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
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
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
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
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

/* Character flag word at +0x20C; bit 0x4000 set by Action_SetIgnoreAntinodes_Level,
 * bit 0x800000 by Action_SetAIOverrideControl. */
struct gameaiscript_20c_s {
    u32 unk0 : 1;
    u32 dontPush : 1; /* Action_DontPush_Level */
    u32 unk2 : 6;
    u32 aiOverrideControl : 1;
    u32 unk9 : 8;
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
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
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

s32 CloseDoorObstacle(s32, s32);

s32 Action_CloseDoor(void *sys, void *process, void *packet, s32 *arg3, s32 arg4, s32 arg5) {
    if ((arg5 != 0) && (arg4 != 0)) {
        CloseDoorObstacle(*arg3, 0);
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

s32 GetTableLocator();
extern s32 lbl_804093AC;

s32 Action_SetTableLocator(void *sys, void *process, void *arg2, char *param, s32 arg4, s32 arg5) {
    void **temp_r5;
    void *temp_r31;

    if (((s32) lbl_804093AC == 0x7B) && (arg5 != 0) && (arg2 != NULL)) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r31 = *temp_r5;
            if (temp_r31 != NULL) {
                (*(s32 *)((s8 *)(temp_r31) + (0x39C))) = GetTableLocator();
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
            lbl_8040900C = AIParamToFloat(process, params[i]);
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

extern s32 FindAnimIX(s32, char *);
extern void fn_80095558(void *, s32);

s32 Action_SetAnimation(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 anim = -1;
    u8 *object;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0 && firstTime != 0) {
        if (count == 1)
            anim = FindAnimIX(*(s32 *)(object + 0x64), params[0]);
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

extern void CallDexter(void *);
extern s32 lbl_804093AC;

s32 Action_CallDexter(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    if (lbl_804093AC == 0x7b && firstTime != 0 && packet != 0 && packet->owner != 0 && packet->owner->object != 0)
        CallDexter(packet->owner->object);
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

extern void OpenDoorObstacle(char *, s32, s32, s32, f32);
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
            OpenDoorObstacle(door, 0, 1, instant, lbl_801AEBE8[0]);
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

extern void AIMoveInstruction(void *, s32, void *, s32, f32, f32);
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
                *(f32 *)((u8 *)packet + 0x134) = AIParamToFloat(packet, params[i]);
        }
    }
    if ((target = *(s32 *)(object + 0x660)) != 0) {
        if (*(s32 *)(object + 0x664) == 0)
            AIMoveInstruction(packet, target, object + 0x66c, 1, lbl_801AEBAC[0], lbl_801AEBAC[0]);
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

f32 fn_8009CB2C(void *, void *);
extern f32 lbl_801AEB04[];
extern void *lbl_80409410;

f32 Condition_SockDistanceToOpponent(s32 pad0, s32 pad1, void *arg2) {
    f32 temp_f31;
    u8 temp_r0;
    void **temp_r4;
    void **temp_r9;
    void *temp_r31;
    void *temp_r4_2;

    if (arg2 != NULL) {
        temp_r9 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r9 != NULL) {
            temp_r4 = (*(void ***)((s8 *)(arg2) + (0x64)));
            if (temp_r4 != NULL) {
                temp_r4_2 = *temp_r4;
                temp_r31 = *temp_r9;
                temp_r0 = (*(u8 *)((s8 *)(temp_r4_2) + (0x601)));
                if (((s8) temp_r0 != -1) && ((s8) temp_r0 == (s8) (*(u8 *)((s8 *)(temp_r31) + (0x601))))) {
                    temp_f31 = fn_8009CB2C((*(s32 *)((s8 *)(lbl_80409410) + (0x2DC))), temp_r4_2 + 0x600);
                    return temp_f31 - fn_8009CB2C((*(s32 *)((s8 *)(lbl_80409410) + (0x2DC))), temp_r31 + 0x600);
                }
            }
        }
    }
    return lbl_801AEB04[0];
}

f32 fn_8009CB2C(void *, void *);
extern f32 lbl_801AEB00[];
extern void *lbl_80409410;

f32 Condition_SockDistanceToPlayer(s32 pad0, s32 pad1, void *arg2) {
    f32 temp_f31;
    u8 temp_r0;
    void **temp_r5;
    void *temp_r31;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if ((temp_r5 != NULL) && ((void *) lbl_80409004 != NULL)) {
            temp_r0 = (*(u8 *)((s8 *)(lbl_80409004) + (0x601)));
            temp_r31 = *temp_r5;
            if (((s8) temp_r0 != -1) && ((s8) temp_r0 == (s8) (*(u8 *)((s8 *)(temp_r31) + (0x601))))) {
                temp_f31 = fn_8009CB2C((*(s32 *)((s8 *)(lbl_80409410) + (0x2DC))), lbl_80409004 + 0x600);
                return temp_f31 - fn_8009CB2C((*(s32 *)((s8 *)(lbl_80409410) + (0x2DC))), temp_r31 + 0x600);
            }
        }
    }
    return lbl_801AEB00[0];
}

u64 __ashldi3(s32, s32, u32);
extern s8 str_Bool_ON[];

s32 Action_SetAreaFlag(s32 pad0, s32 pad1, s32 pad2, void *arg3, s32 arg4, s32 arg5) {
    s32 var_r28;
    u32 temp_r3;
    void *var_r30;

    if (arg5 != 0) {
        var_r28 = 0;
        if (arg4 > 1) {
            var_r30 = arg3;
            do {
                temp_r3 = NuAToI((*(s8 **)((s8 *)(var_r30) + (0))));
                if (temp_r3 <= 0x1FU) {
                    if (NuStrICmp((*(s8 **)((s8 *)(var_r30) + (4))), str_Bool_ON) == 0) {
                        lbl_8040B188 |= (u32) (1LL << temp_r3);
                    } else {
                        lbl_8040B188 &= ~(u32) (1LL << temp_r3);
                    }
                }
                var_r28 += 2;
                var_r30 += 8;
            } while ((s32) (var_r28 + 1) < arg4);
        }
    }
    return 1;
}

#define NuAToI ((u32 (*)(s32))NuAToI)
#define NuStrICmp ((s32 (*)(s32, s32 *))NuStrICmp)
u64 __ashldi3(s32, s32, u32);
extern void *lbl_80409410;
#define str_Bool_ON (*(s32 (*)[])&str_Bool_ON)

s32 Action_SetLevelFlag(s32 pad0, s32 pad1, s32 pad2, void *arg3, s32 arg4, s32 arg5) {
    s32 *temp_r30;
    s32 *temp_r30_2;
    s32 var_r28;
    u32 temp_r3;
    void *var_r31;

    if (arg5 != 0) {
        var_r28 = 0;
        if (arg4 > 1) {
            var_r31 = arg3;
            do {
                temp_r3 = NuAToI((*(s32 *)((s8 *)(var_r31) + (0))));
                if (temp_r3 <= 0x1FU) {
                    if (NuStrICmp((*(s32 *)((s8 *)(var_r31) + (4))), str_Bool_ON) == 0) {
                        temp_r30 = (*(s32 **)((s8 *)(lbl_80409410) + (0x1B4)));
                        *temp_r30 |= (u32) (1LL << temp_r3);
                    } else {
                        temp_r30_2 = (*(s32 **)((s8 *)(lbl_80409410) + (0x1B4)));
                        *temp_r30_2 &= ~(u32) (1LL << temp_r3);
                    }
                }
                var_r28 += 2;
                var_r31 += 8;
            } while ((s32) (var_r28 + 1) < arg4);
        }
    }
    return 1;
}
#undef NuAToI
#undef NuStrICmp
#undef str_Bool_ON

extern f32 lbl_801AEB70[];
extern f32 lbl_801AEB74[];

f32 Condition_ToNode(s32 arg0, s32 pad1, void *arg2, s32 pad3, s32 arg4) {
    if ((arg2 != NULL) && (arg4 != 0) && (arg0 != 0) && ((s32) ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg2) + (0x10C)))) + (0x78))) + (*(u8 *)((*(s32 *)((s8 *)(arg2) + (0x110))) + ((*(u8 *)((s8 *)(arg2) + (0x114))) == 0)) * 0x58)) == arg4)) {
        return lbl_801AEB70[0];
    }
    return lbl_801AEB74[0];
}

extern f32 lbl_801AE9E8[];
extern f32 lbl_801AE9EC[];

f32 Condition_OnObject(s32 pad0, s32 pad1, void *arg2, s32 pad3, s32 arg4) {
    f32 temp_f0;
    f32 temp_f13;
    s16 temp_r11;
    void **temp_r5;
    void *temp_r5_2;

    if (arg2 != NULL) {
        temp_r5 = (*(void ***)((s8 *)(arg2) + (0x50)));
        if (temp_r5 != NULL) {
            temp_r5_2 = *temp_r5;
            if ((temp_r5_2 != NULL) && (arg4 != -1) && ((*(s32 *)((s8 *)(temp_r5_2) + (0x284))) & 0xFFFF00)) {
                temp_r11 = (*(s16 *)((s8 *)(temp_r5_2) + (0x282)));
                if (temp_r11 == arg4) {
                    temp_f13 = (*(f32 *)((s8 *)(temp_r5_2) + (0x70)));
                    temp_f0 = (*(f32 *)((s8 *)((*(void **)((s8 *)(((temp_r11 * 0x6C) + (*(s32 *)((s8 *)(lbl_8040AE4C) + (0x68))))) + (0x40)))) + (0x34)));
                    if (temp_f13 >= temp_f0) {
                        return lbl_801AE9E8[0];
                    }
                }
            }
        }
    }
    return lbl_801AE9EC[0];
}

extern char str_PlayerID_Player1[];
extern char str_Entity_Player[];
extern char str_PlayerID_Player2[];

/* Character flag word at +0x440, written by Action_SetZeroAcceleration. */
struct gameaiscript_440_s {
    u32 unk0 : 9;
    u32 clockwise : 1;
    u32 unk10 : 9;
    u32 zeroAcceleration : 1;
    u32 unk14 : 7;
    u32 rangeMode : 2; /* Action_SetMaxMovementRange */
    u32 unk1d : 3;
};

s32 Action_SetZeroAcceleration(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 on = 1;
    u8 *target = 0;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if (NuStrIStr(params[i], str_PlayerID_Player1) != 0 || NuStrIStr(params[i], str_Entity_Player) != 0)
                target = lbl_80409004;
            else if (NuStrIStr(params[i], str_PlayerID_Player2) != 0)
                target = lbl_80409008;
            else if (NuStrIStr(params[i], str_Bool_FALSE) != 0)
                on = 0;
        }
        if (target != 0)
            ((struct gameaiscript_440_s *)(target + 0x440))->zeroAcceleration = on;
    }
    return 1;
}

void fn_800ABA78(u8 *);
void ResetPlayerContexts(void *);

s32 Action_ResetContext(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0)
            target = packet->owner->object;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                    target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            }
        }
        if (target != 0) {
            fn_800ABA78(target);
            ResetPlayerContexts(*(void **)(target + 0xeec));
        }
    }
    return 1;
}

s32 Action_KeepWeaponOut(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 on = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0)
            target = packet->owner->object;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    on = 0;
                else if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                    target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            }
        }
        if (target != 0)
            ((struct gameaiscript_f24_s *)(target + 0xf24))->keepWeaponOut = on;
    }
    return 1;
}

s32 Action_AddToSet(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;
    s32 set;

    if (firstTime != 0 && packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0) {
        for (i = 0; i < count; i++) {
            set = (s32)AIParamToFloat(packet, params[i]);
            if ((u32)set < 8)
                object[0x1082] |= 1LL << set;
        }
    }
    return 1;
}

extern char str_ScriptParam_State[];
extern void *fn_8008386C(char *, void *);
extern void AIScriptProcessorInit(void *, void *, void *, s32, s32, s32, s32, void *, void *);

s32 Action_SetScriptState(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    char *state = 0;
    char *p;
    void *found;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_State)) != 0)
                state = p + 6;
        }
        if (target != 0 && state != 0) {
            found = fn_8008386C(state, *(void **)(target + 0x2a0));
            if (found != 0) {
                *(s32 *)(target + 0x2d8) = 0;
                AIScriptProcessorInit(sys, target + 0x2a0, target + 0x2a0, 0, 0, 0, 0,
                    *(void **)(target + 0x2a0), found);
            }
        }
    }
    return 1;
}

extern void fn_801091AC(void *, char *);

s32 Action_ResetDoorTrigger(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        for (i = 0; i < count; i++) {
            if (NuStrIStr(params[i], str_ScriptParam_Name) != 0) {
                i++;
                if (i < count)
                    fn_801091AC(lbl_80409410, params[i]);
            }
        }
    }
    return 1;
}

extern f32 lbl_801AEAD0[];
extern f32 lbl_801AEAD4[];
extern void *lbl_80409410;

f32 Condition_AnimationFinished(s32 pad0, s32 pad1, s32 pad2, s32 pad3, void *arg4) {
    f32 *temp_r9;
    f32 temp_f0;
    f32 temp_f13;

    if ((arg4 != NULL) && ((s32) (*(s32 *)((s8 *)(arg4) + (0x50))) >= 0)) {
        temp_r9 = *(s32 *)(((*(u8 *)((s8 *)(arg4) + (0x5C))) * 4) + (*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1C0)))) + (0x50))));
        if (temp_r9 != NULL) {
            temp_f13 = (*(f32 *)((s8 *)(arg4) + (0x4C)));
            temp_f0 = *temp_r9;
            if ((temp_f13 >= temp_f0)) {
                return lbl_801AEAD0[0];
            }
        }
    }
    return lbl_801AEAD4[0];
}

extern f32 lbl_801AE9D8[];
extern f32 lbl_801AE9DC[];
#define lbl_80409004 (*(void *(*))&lbl_80409004)
#define lbl_8040AE4C (*(void *(*))&lbl_8040AE4C)

f32 Condition_PlayerOnObject(s32 pad0, s32 pad1, s32 pad2, s32 pad3, s32 arg4) {
    f32 temp_f0;
    f32 temp_f13;
    s16 temp_r11;

    if (((void *) lbl_80409004 != NULL) && (arg4 != -1) && ((*(s32 *)((s8 *)(lbl_80409004) + (0x284))) & 0xFFFF00)) {
        temp_r11 = (*(s16 *)((s8 *)(lbl_80409004) + (0x282)));
        if (temp_r11 == arg4) {
            temp_f13 = (*(f32 *)((s8 *)(lbl_80409004) + (0x70)));
            temp_f0 = (*(f32 *)((s8 *)((*(void **)((s8 *)(((temp_r11 * 0x6C) + (*(s32 *)((s8 *)(lbl_8040AE4C) + (0x68))))) + (0x40)))) + (0x34)));
            if ((temp_f13 >= temp_f0)) {
                return lbl_801AE9D8[0];
            }
        }
    }
    return lbl_801AE9DC[0];
}
#undef lbl_80409004
#undef lbl_8040AE4C

s32 Action_SetAIOverrideControl(void *sys, void *process, struct gameaiscript_packet_s *packet,
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
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            else if (NuStrICmp(str_Bool_FALSE, params[i]) == 0)
                on = 0;
        }
        if (target != 0)
            ((struct gameaiscript_20c_s *)(target + 0x20c))->aiOverrideControl = on;
    }
    return 1;
}

extern s32 str_ScriptState_Base[];
void ResetAICreature(void *, void *);

s32 Action_Activate(void *sys, s32 arg1, void *arg2) {
    u8 *owner;
    u8 *object;

    if (arg2 != NULL && (owner = *(u8 **)((u8 *)arg2 + 0x50)) != NULL && (object = *(u8 **)owner) != NULL) {
        if (object[0x346] == 0xff) {
            *(u32 *)(owner + 0x20c) |= 0x80000;
            fn_8007EBD0(arg1, str_ScriptState_Base);
        } else {
            ResetAICreature(object, sys);
        }
    }
    return 1;
}

s32 Action_SetFlag(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 i;
    u32 bit;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        for (i = 0; i + 1 < count; i += 2) {
            bit = NuAToI(params[i]);
            if (bit < 32) {
                if (NuStrICmp(params[i + 1], (char *)str_Bool_ON) == 0)
                    (*(struct gameaiscript_flags_s **)((u8 *)lbl_80409410 + 0x1b4))->flags[((u8 *)packet->owner)[0x291]] |= 1LL << bit;
                else
                    (*(struct gameaiscript_flags_s **)((u8 *)lbl_80409410 + 0x1b4))->flags[((u8 *)packet->owner)[0x291]] &= ~(1LL << bit);
            }
        }
    }
    return 1;
}

extern u8 *FindTurret(void *, char *);

s32 Action_ActivateTurret(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 on = 1;
    u8 *turret = 0;
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if (NuStrIStr(params[i], str_ScriptParam_Name) != 0) {
                i++;
                if (i < count)
                    turret = FindTurret(*(void **)((u8 *)lbl_80409410 + 0x578), params[i]);
            } else if (NuStrIStr(params[i], str_Bool_FALSE) != 0)
                on = 0;
        }
        if (turret != 0) {
            if (on != 0)
                *(u16 *)(turret + 0x6e) &= ~1;
            else
                *(u16 *)(turret + 0x6e) |= 1;
        }
    }
    return 1;
}

extern void fn_80013200(void *, s32);
extern void *memset(void *, s32, u32);

s32 Action_SetVisibility(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 on = 1;
    u8 special[0xc];
    char *p;
    s32 i;

    if (firstTime != 0) {
        memset(special, 0, sizeof(special));
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Name)) != 0)
                NuSpecialFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), special, p + 5, 1);
            else if (NuStrIStr(params[i], str_Bool_FALSE) != 0)
                on = 0;
        }
        if (fn_800130F4(special) != 0)
            fn_80013200(special, on);
    }
    return 1;
}

extern f32 lbl_801AEC00[];
s32 Action_FaceCharacter(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    char *p;
    s32 i;
    f32 t;
    f32 z;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                *(s32 *)((u8 *)packet + 0xb4) = GetNamedGameObject(sys, p + 10, 0);
        }
    }
    if (packet != 0 && *(s32 *)((u8 *)packet + 0xb4) != 0)
        *(s32 *)((u8 *)packet + 0x19c) = *(s32 *)((u8 *)packet + 0xb4) + 0x6c;
    t = *(f32 *)((u8 *)process + 0x40);
    z = lbl_801AEC00[0];
    if (t > z) {
        t -= time;
        *(f32 *)((u8 *)process + 0x40) = t;
        if (t <= z) {
            *(f32 *)((u8 *)process + 0x40) = z;
            return 1;
        }
    }
    return 0;
}

extern f32 fn_8000C038(void);
extern f32 lbl_801AEBB0[];
extern char str_ScriptDir_AntiClockwise[];
extern char str_ScriptDir_Clockwise[];
extern char str_ScriptDir_Reverse[];
extern char str_ScriptTarget_Random2[];

s32 Action_SetCircleDirection(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target;
    struct gameaiscript_440_s *f;
    s32 i;


    if (packet != 0 && packet->owner != 0 && (target = (u8 *)packet->owner->object) != 0 &&
        firstTime != 0 && count != 0) {

        for (i = 0; i < count; i++) {
            f = (struct gameaiscript_440_s *)(target + 0x440);
            if (NuStrICmp(params[i], str_ScriptDir_Clockwise) == 0)
                f->clockwise = 1;
            else if (NuStrICmp(params[i], str_ScriptDir_AntiClockwise) == 0)
                f->clockwise = 0;
            else if (NuStrICmp(params[i], str_ScriptDir_Reverse) == 0)
                f->clockwise = !f->clockwise;
            else if (NuStrICmp(params[i], str_ScriptTarget_Random2) == 0) {
                if (fn_8000C038() > 0.5f)
                    f->clockwise = 1;
                else
                    f->clockwise = 0;
            }
        }
    }
    return 1;
}

extern void fn_801034C4(void *, s32);
extern s32 lbl_804099E4;

s32 Action_ForceFreeze(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    u8 *target;
    char *p;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = packet->owner->object) != 0) {
        if (firstTime != 0) {
            lbl_804099E4 = lbl_804099E4 == 0;
            if (lbl_80409008 != 0 && lbl_804099E4 != 0)
                target = lbl_80409008;
            else
                target = lbl_80409004;
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptCategory_Character)) != 0)
                    target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            }
            fn_801034C4(object, (s32)target);
        }
        if (*(u8 **)(*(u8 **)(object + 0xeec) + 0x650) != 0) {
            target = *(u8 **)(*(u8 **)(object + 0xeec) + 0x650);
            AIMoveInstruction(packet, (s32)(target + 0x3c4), target + 0x3ac, 2, *(f32 *)(target + 0x33c), 0.75f);
            *(u8 **)((u8 *)packet + 0x19c) = target + 0x6c;
        }
        return 0;
    }
    return 1;
}

struct gameaiscript_vec_s {
    f32 x, y, z;
};

s32 Action_SetLastSafePathPos(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = (u8 *)packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptCategory_Character)) != 0)
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            else
                NuStrICmp(str_Bool_FALSE, params[i]);
        }
        if (target != 0) {
            *(struct gameaiscript_vec_s *)(target + 0x1dc) = *(struct gameaiscript_vec_s *)(target + 0x6c);
            *(struct gameaiscript_vec_s *)(target + 0x1c4) = *(struct gameaiscript_vec_s *)(target + 0x1dc);
        }
    }
    return 1;
}

extern f32 lbl_801AE9E0[];
extern f32 lbl_801AE9E4[];

f32 Condition_EitherPlayerOnObject(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 obj)
{
    u8 *p;
    s32 o;

    if ((p = lbl_80409004) != 0 && obj != -1 && (*(s32 *)(p + 0x284) & 0xFFFF00) &&
        *(s16 *)(p + 0x282) == obj &&
        *(f32 *)(p + 0x70) >= *(f32 *)(*(u8 **)(obj * 0x6c + *(s32 *)((u8 *)lbl_8040AE4C + 0x68) + 0x40) + 0x34))
        return lbl_801AE9E0[0];
    if ((p = lbl_80409008) != 0 && obj != -1 && (*(s32 *)(p + 0x284) & 0xFFFF00)) {
        o = *(s16 *)(p + 0x282);
        if (o == obj &&
            *(f32 *)(p + 0x70) >= *(f32 *)(*(u8 **)(o * 0x6c + *(s32 *)((u8 *)lbl_8040AE4C + 0x68) + 0x40) + 0x34))
            return lbl_801AE9E0[0];
    }
    return lbl_801AE9E4[0];
}

extern void fn_800A41D8(void *, s32, s32);

s32 Action_SetFormationCommander(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    u8 *f;
    u8 *m;
    s16 s;
    s32 a;
    s32 b;

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0 &&
        (f = *(u8 **)((u8 *)packet + 0xf0)) != 0 && *((u8 *)packet + 0xf4) == f[7] - 1 &&
        (f[8] == 1 || (u8)(f[7] % f[8]) == 1)) {
        *(u32 *)((u8 *)packet + 0x1a0) |= 0x40;
        s = *(s16 *)(object + 0x1074);
        if (s == 2 || s == 0x40 || s == 0x41) {
            fn_800A41D8(object, 0x3f, *(s16 *)(object + 0x1074));
            m = *(u8 **)(object + 0x64);
            object[0x1085] = (*(u8 **)(m + 0x20))[0xc2];
            object[0x1084] = (*(u8 **)(m + 0x20))[0xc2];
        } else if (s == 0x65) {
            fn_800A41D8(object, 0x67, 0x65);
            m = *(u8 **)(object + 0x64);
            object[0x1085] = (*(u8 **)(m + 0x20))[0xc2];
            object[0x1084] = (*(u8 **)(m + 0x20))[0xc2];
        }
    }
    return 1;
}

extern void *NuCameraGetViewMtx(void);
extern void SetHeadTarget(void *, s32, s32, f32, f32, f32);
extern char lbl_801ADA04[];
extern f32 lbl_801AEBF8[];
extern f32 lbl_801AEBFC[];

extern void *NuCameraGetViewMtx(void);
extern void SetHeadTarget(void *, s32, s32, f32, f32, f32);
extern char lbl_801ADA04[];
extern f32 lbl_801AEBF8[];
extern f32 lbl_801AEBFC[];

s32 Action_FaceCamera(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    u8 *m;
    s32 i;
    f32 t;
    f32 z;

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0) {
        if (firstTime != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(lbl_801ADA04, params[i]) == 0)
                    *((u8 *)packet + 0xb2) = 1;
                else
                    *(f32 *)((u8 *)process + 0x40) = AIParamToFloat(process, params[i]);
            }
        }
        if ((m = NuCameraGetViewMtx()) != 0) {
            *(u8 **)((u8 *)packet + 0x19c) = m + 0x30;
            if (*((u8 *)packet + 0xb2) != 0)
                SetHeadTarget(object, *(s32 *)(object + 0x43c), 4, 1.0f, 0.0f, 0.0f);
        }
        t = *(f32 *)((u8 *)process + 0x40);
        z = lbl_801AEBFC[0];
        if (t > z) {
            t -= time;
            *(f32 *)((u8 *)process + 0x40) = t;
            if (t <= z)
                *(f32 *)((u8 *)process + 0x40) = z;
            else
                goto fail;
        } else
            goto fail;
    }
done:
    return 1;
fail:
    return 0;
}

s32 Action_SetHoverPhase(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    u8 *ai;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0 &&
        *(s16 *)(object + 0x1074) == 7) {
        if (firstTime != 0) {
            *((u8 *)packet + 0xb2) = 1;
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    *((u8 *)packet + 0xb2) = 0;
            }
        }
        if (*((u8 *)packet + 0xb2) != 0) {
            if ((ai = *(u8 **)(object + 0xeec))[0x7cb] == 1)
                goto done;
        } else if ((ai = *(u8 **)(object + 0xeec))[0x7cb] != 1)
            goto done;
        goto set;
    }
done:
    return 1;
    do {
set:
        *(u32 *)(*(u8 **)(ai + 0x61c) + 8) |= 0x20;
    } while (0);
    return 0;
}

extern void *lbl_80408F58;
extern u8 *CheckAIMessage(void *, char *, s32);
extern char str_ScriptParam_Value[];
extern char str_ScriptParam_Increment[];
extern char str_ScriptParam_Decrement[];

s32 Action_SetMessage(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    f32 v = 0.0f;
    s32 mode = 0;
    char *name = 0;
    char *p;
    u8 *m;
    s32 i;

    if (firstTime != 0 && lbl_80408F58 != 0 && count != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Name)) != 0) {
                name = p + 5;
                AIParamToFloat(packet, p);
            } else if ((p = NuStrIStr(params[i], str_ScriptParam_Value)) != 0)
                v = AIParamToFloat(packet, p + 6);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_Increment)) != 0) {
                mode = 1;
                v = AIParamToFloat(packet, p + 10);
            } else if ((p = NuStrIStr(params[i], str_ScriptParam_Decrement)) != 0) {
                mode = -1;
                v = AIParamToFloat(packet, p + 10);
            }
        }
        if (name != 0 && (m = CheckAIMessage(lbl_80408F58, name, 0)) != 0) {
            switch (mode) {
            case -1:
                *(f32 *)(m + 0x28) -= v;
                break;
            case 0:
                *(f32 *)(m + 0x28) = v;
                break;
            case 1:
                *(f32 *)(m + 0x28) += v;
                break;
            }
        }
    }
    return 1;
}

extern char str_ScriptParam_Multiply[];
extern char str_ScriptParam_MaxDefault[];
extern char str_ScriptParam_Max[];

s32 Action_SetRunSpeed(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    f32 mul = 1.0f;
    f32 max = 3.4028235e38f;
    f32 speed = 0.0f;
    s32 multiply = 0;
    u8 *object;
    char *p;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0 && firstTime != 0) {
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptParam_Multiply)) != 0) {
                    multiply = 1;
                    mul = AIParamToFloat(packet, p + 9);
                } else if (NuStrICmp(params[i], str_ScriptParam_MaxDefault) == 0)
                    max = *(f32 *)(*(u8 **)(*(u8 **)(object + 0x64) + 0x20) + 0x18);
                else if ((p = NuStrIStr(params[i], str_ScriptParam_Max)) != 0)
                    max = AIParamToFloat(packet, p + 4);
                else
                    speed = AIParamToFloat(packet, params[i]);
            }
        }
        if (multiply != 0) {
            if (*(f32 *)(object + 0xf10) == 0.0f)
                *(f32 *)(object + 0xf10) = *(f32 *)(*(u8 **)(*(u8 **)(object + 0x64) + 0x20) + 0x18);
            *(f32 *)(object + 0xf10) *= mul;
            if (*(f32 *)(object + 0xf10) > max)
                *(f32 *)(object + 0xf10) = max;
        } else
            *(f32 *)(object + 0xf10) = speed;
    }
    return 1;
}

extern f32 fn_8000CDFC(void *, void *, void *);
extern void AISysCharacterSetPathCnx(void *, void *, void *, s32);

extern f32 fn_8000CDFC(void *, void *, void *);
extern void AISysCharacterSetPathCnx(void *, void *, void *, s32);

s32 Action_GoToLevelPath(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *pk = (u8 *)packet;
    u8 *node;
    s32 path;
    struct { f32 x, y, z, w, u, v; } tmp;

    if (packet == 0 || packet->owner == 0)
        return 1;
    if (firstTime != 0) {
        if (sys != 0 && (path = *(s32 *)(pk + 0x10c)) != 0 && *(s32 *)(pk + 0x110) != 0) {
            if (path == *(s32 *)(*(u8 **)((u8 *)sys + 0xc) + 8))
                return 1;
            AISysCharacterSetPath(pk, *(s32 *)(*(u8 **)((u8 *)sys + 0xc) + 8));
            AISysGetCharacterPathPos(*(s32 *)((u8 *)lbl_80409410 + 0x2e8), packet->owner, pk, 0xff, 1);
            return 1;
        }
    } else if ((node = *(u8 **)(pk + 0xb4)) != 0) {
        if (fn_8000CDFC((u8 *)packet->owner + 0x6c, node + 4, &tmp) < *(f32 *)(node + 0x14)) {
            memset(pk + 0x10c, 0, 0x18);
            AISysCharacterSetPath(pk, *(s32 *)(*(u8 **)((u8 *)sys + 0xc) + 8));
            if (node[0x2b] & 1)
                AISysCharacterSetPathCnx(pk, (u8 *)packet->owner + 0x6c,
                    (void *)(*(s32 *)(*(u8 **)(pk + 0x10c) + 0x7c) + *(u16 *)(node + 0x2c) * 0x28), 0);
            return 1;
        }
        AIMoveInstruction(pk, (s32)(node + 4), pk + 0xcc, 1, 0.0f, *(f32 *)(pk + 0x134));
    }
    return 0;
}

extern char lbl_801AD834[];
extern char lbl_801AD83C[];

s32 Action_SnapWeaponOut(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *target = 0;
    s32 side = -1;
    s32 on = 1;
    char *p;
    s32 i;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0)
            target = (u8 *)packet->owner->object;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0)
                    target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
                else if ((p = NuStrIStr(params[i], lbl_801AD83C)) != 0)
                    side = NuStrICmp(p + 9, lbl_801AD834) == 0;
                else if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    on = 0;
            }
        }
        if (target != 0) {
            if (on != 0) {
                (*(u8 **)(target + 0xeec))[0x7b4] = 1;
                *(f32 *)(*(u8 **)(target + 0xeec) + 0x6bc) = 1.0f;
                if (side != -1)
                    ((struct gameaiscript_f24_s *)(target + 0xf24))->keepWeaponOut = side;
            } else {
                (*(u8 **)(target + 0xeec))[0x7b4] = on;
                *(f32 *)(*(u8 **)(target + 0xeec) + 0x6bc) = 0.0f;
                ((struct gameaiscript_f24_s *)(target + 0xf24))->keepWeaponOut = 0;
            }
        }
    }
    return 1;
}

extern char str_ScriptParam_Multiplier[];
extern char str_ScriptParam_SnapToSpeed[];
extern void fn_80098190(s32, void *, s32, s32, void *);
extern void fn_80098858(void *);
extern void CurrentStart(void *, s32, s32);

s32 Action_UseCurrentSpeed(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 snap = 0;
    u8 *object;
    char *p;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0 && firstTime != 0) {
        *(u32 *)(object + 0xf24) |= 0x400;
        *(f32 *)(object + 0xf04) = 1.0f;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                *(u32 *)(object + 0xf24) &= ~0x400;
            else if ((p = NuStrIStr(params[i], str_ScriptParam_Multiplier)) != 0)
                *(f32 *)(object + 0xf04) = AIParamToFloat(packet, p + 11);
            else if (NuStrICmp(params[i], str_ScriptParam_SnapToSpeed) == 0)
                snap = 1;
        }
        if ((*(u32 *)(object + 0xf24) & 0x4400) == 0x400) {
            *(u32 *)(object + 0xf24) |= 0x4000;
            if (snap == 0)
                return 1;
            fn_80098190(*(s32 *)((u8 *)lbl_80409410 + 0x2dc), object + 0x6c, (s8)object[0x601],
                *(s16 *)(object + 0x602), object + 0x600);
            fn_80098858(*(u8 **)(object + 0xeec) + 0x14c);
        }
        if (snap != 0)
            CurrentStart(object, 1, 1);
    }
    return 1;
}

struct gameaiscript_sockpos_s {
    u8 unk0[8];
    struct gameaiscript_vec_s pos;
    u8 unk14[0x20];
};

extern f32 lbl_804099FC;
extern void MoveSockPosition(s32, void *, f32, struct gameaiscript_sockpos_s *);
extern void NuVecRotateX(struct gameaiscript_vec_s *, struct gameaiscript_vec_s *, s32);
extern void NuVecRotateY(struct gameaiscript_vec_s *, struct gameaiscript_vec_s *, s32);
extern void NuVecAdd(struct gameaiscript_vec_s *, struct gameaiscript_vec_s *, struct gameaiscript_vec_s *);
extern s32 NuAngAdd(s32, s32);

s32 Action_FollowSock(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    u8 *sock;
    struct gameaiscript_vec_s v;
    struct gameaiscript_sockpos_s info;

    if (packet == 0 || packet->owner == 0 || (object = (u8 *)packet->owner->object) == 0)
        return 1;
    sock = object + 0x600;
    if (!(*(u32 *)(object + 0xf24) & 0x4000))
        fn_80098190(*(s32 *)((u8 *)lbl_80409410 + 0x2dc), object + 0x6c, (s8)object[0x601],
            *(s16 *)(object + 0x602), sock);
    if (*(u32 *)(object + 0xf24) & 0x10000)
        MoveSockPosition(*(s32 *)((u8 *)lbl_80409410 + 0x2dc), sock, -lbl_804099FC, &info);
    else
        MoveSockPosition(*(s32 *)((u8 *)lbl_80409410 + 0x2dc), sock, lbl_804099FC, &info);
    if (*(f32 *)(object + 0xf18) != 0.0f || *(f32 *)(object + 0xf1c) != 0.0f) {
        v.x = *(f32 *)(object + 0xf18);
        v.y = *(f32 *)(object + 0xf1c);
        v.z = 0.0f;
        NuVecRotateX(&v, &v, *(u16 *)(object + 0x624));
        if (*(u32 *)(object + 0xf24) & 0x10000)
            NuVecRotateY(&v, &v, *(u16 *)(object + 0x626));
        else
            NuVecRotateY(&v, &v, NuAngAdd(*(u16 *)(object + 0x626), 0x8000));
    } else
        v.x = v.y = v.z = 0.0f;
    NuVecAdd(&v, &v, &info.pos);
    AIMoveInstruction(packet, (s32)&v, 0, 1, 0.0f, 0.0f);
    return 0;
}

extern void *fn_800076C4(void *);
extern void *fn_800076CC(void *, void *);
extern void fn_800E3928(void *, s32, char *, s32);
extern char str_ScriptParam_NameEq[];
s32 Action_PlaySfx(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time);

void GameAIScriptAddLevelSfx(void *level, void *scripts)
{
    u8 *script;
    u8 *state;
    u8 *action;
    char *p;
    s32 i;

    if (scripts == 0)
        return;
    for (script = fn_800076C4(scripts); script != 0; script = fn_800076CC(scripts, script)) {
        for (state = fn_800076C4(script + 0x10); state != 0; state = fn_800076CC(script + 0x10, state)) {
            for (action = fn_800076C4(state + 0x10); action != 0; action = fn_800076CC(state + 0x10, action)) {
                if (*(u8 **)(action + 0x10) != 0 && *(void **)(*(u8 **)(action + 0x10) + 4) == (void *)Action_PlaySfx) {
                    for (i = 0; i < *(s32 *)(action + 0xc); i++) {
                        if ((p = NuStrIStr((*(char ***)(action + 8))[i], str_ScriptParam_NameEq)) != 0)
                            fn_800E3928(level, 0, p + 5, -1);
                    }
                }
            }
        }
    }
}

extern s32 lbl_8040B058;
extern u8 *lbl_80409430;
extern char str_ScriptScope_Default2[];
extern char str_ScriptTarget_AllNonParty[];
extern char str_ScriptTarget_Locator[];

s32 Action_SetMaxMovementRange(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    f32 range = 0.0f;
    s32 all = 0;
    s32 mode = 1;
    u8 *c;
    u8 *object;
    s32 i;
    s32 n;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if (NuStrICmp(str_ScriptScope_Default2, params[i]) == 0)
                range = lbl_8040900C;
            else if (NuStrICmp(str_ScriptTarget_AllNonParty, params[i]) == 0)
                all = 1;
            else if (NuStrICmp(str_ScriptTarget_Locator, params[i]) == 0)
                mode = 2;
            else
                range = AIParamToFloat(process, params[i]);
        }
        if (all != 0) {
            n = lbl_8040B058;
            c = lbl_80409430;
            for (i = 0; i < n; i++, c += 0x1098) {
                if (*(s32 *)(c + 0x20c) < 0 && (*(u32 *)(c + 0x208) & 0x400)) {
                    *(f32 *)(c + 0x448) = range;
                    if (range > 0.0f)
                        ((struct gameaiscript_440_s *)(c + 0x440))->rangeMode = 1;
                    else
                        ((struct gameaiscript_440_s *)(c + 0x440))->rangeMode = 0;
                }
            }
        } else if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0) {
            *(f32 *)(object + 0x448) = range;
            if (range > 0.0f)
                ((struct gameaiscript_440_s *)(object + 0x440))->rangeMode = mode;
            else
                ((struct gameaiscript_440_s *)(object + 0x440))->rangeMode = 0;
        }
    }
    return 1;
}

extern s32 sprintf(char *, const char *, ...);
extern void *NuSplineFind(void *, char *);
extern void InitSplinePosition(void *, void *, s32, f32);
extern char str_Format_StringTwoDigits[];
extern char str_ScriptFlag_Looping[];

s32 Action_SetRandomSpline(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 looping = 0;
    u8 *object;
    void *spline;
    s32 n;
    s32 i;
    char name[64];

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0) {
        n = 0;
        for (i = 1; i < count; i++) {
            if (NuStrIStr(params[i], str_ScriptFlag_Looping) != 0)
                looping = 1;
        }
        for (i = 0; i <= 9; i++) {
            sprintf(name, str_Format_StringTwoDigits, params[0], i);
            if (NuSplineFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), name) != 0)
                n++;
        }
        n = n * fn_8000C038();
        sprintf(name, str_Format_StringTwoDigits, params[0], n);
        if ((spline = NuSplineFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), name)) != 0)
            goto found;
    }
    return 1;
found:
    InitSplinePosition(object + 0x69c, spline, looping, 0.0f);
    return 0;
}

extern char str_JumpType_JUMP[];
extern char str_JumpType_DBLJUMP[];
extern char str_JumpType_R2D2GLIDE[];
extern char str_JumpType_ZIPUP[];
extern char str_JumpType_USEHATCH[];
extern char str_JumpType_JARJARJUMP[];
extern char str_JumpType_HOVERTUBE[];
extern char str_JumpType_FORGOODIES[];
extern char str_JumpType_FORBADDIES[];
extern char str_JumpType_OBSTACLE[];
extern char str_JumpType_JUMP_NOW[];
extern char str_JumpType_DONT_JUMP_NOW[];
extern char str_JumpType_BLOCK[];
extern char str_JumpType_BIGJUMP[];

s32 ParseAIPathCnxFlag(char *name)
{
    if (NuStrICmp(name, str_JumpType_JUMP) == 0)
        return 1;
    if (NuStrICmp(name, str_JumpType_DBLJUMP) == 0)
        return 2;
    if (NuStrICmp(name, str_JumpType_R2D2GLIDE) == 0)
        return 4;
    if (NuStrICmp(name, str_JumpType_ZIPUP) == 0)
        return 8;
    if (NuStrICmp(name, str_JumpType_USEHATCH) == 0)
        return 0x10;
    if (NuStrICmp(name, str_JumpType_JARJARJUMP) == 0)
        return 0x20;
    if (NuStrICmp(name, str_JumpType_HOVERTUBE) == 0)
        return 0x40;
    if (NuStrICmp(name, str_JumpType_FORGOODIES) == 0)
        return 0x80;
    if (NuStrICmp(name, str_JumpType_FORBADDIES) == 0)
        return 0x100;
    if (NuStrICmp(name, str_JumpType_OBSTACLE) == 0)
        return 0x20000000;
    if (NuStrICmp(name, str_JumpType_JUMP_NOW) == 0)
        return 0x400;
    if (NuStrICmp(name, str_JumpType_DONT_JUMP_NOW) == 0)
        return 0x800;
    if (NuStrICmp(name, str_JumpType_BLOCK) == 0)
        return 0x80000000;
    if (NuStrICmp(name, str_JumpType_BIGJUMP) == 0)
        return 0x1000;
    return 0;
}

struct gameaiscript_18_s {
    s32 w[6];
};

extern void NuMtxToQuat(void *, void *, s32, f32);
extern char str_ScriptParam_TgtCharacter[];
extern char str_ScriptAction_Slam[];
extern char str_ScriptParam_JumpFactor[];

s32 Action_BigJump(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    f32 factor = 1.0f;
    s32 slam = 0;
    u8 *target = 0;
    u8 *object;
    char *p;
    s32 i;

    if (packet == 0 || packet->owner == 0 || (object = (u8 *)packet->owner->object) == 0)
        return 1;
    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_TgtCharacter)) != 0)
                target = (u8 *)GetNamedGameObject(sys, p + 14, 0);
            else if (NuStrICmp(params[i], str_ScriptAction_Slam) == 0)
                slam = 1;
            else if ((p = NuStrIStr(params[i], str_ScriptParam_JumpFactor)) != 0) {
                factor = AIParamToFloat(packet, p + 12);
                if (factor < 0.0f)
                    factor = 0.0f;
            }
        }
        if (target != 0) {
            NuMtxToQuat(object, target + 0x1c4, slam, factor);
            *(struct gameaiscript_18_s *)(object + 0x3ac) = *(struct gameaiscript_18_s *)(target + 0x3ac);
            *(struct gameaiscript_vec_s *)(object + 0x3c4) = *(struct gameaiscript_vec_s *)(target + 0x3c4);
        }
    }
    {
        s32 r = 1;
        if ((*(u8 **)(object + 0xeec))[0x7df] == 0x24)
            r = 0;
        return r;
    }
}

struct gameaiscript_f28b_s {
    u32 unk0 : 16;
    u32 stillDoTakeHitAnim : 1;
    u32 unk11 : 10;
    u32 invulnerable : 1;
    u32 unk1c : 4;
};

extern s32 (*lbl_80408D08)(char *);
extern s32 (*lbl_80408D10)(u8);
extern char str_ScriptParam_Type[];
extern char str_ScriptParam_Character[];
extern char str_Bool_FALSE[];
extern char str_ScriptFlag_StillDoTakeHitAnim[];

s32 Action_SetInvulnerable(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 types[10];
    s32 on = 1;
    s32 ntypes = 0;
    s32 takeHit = 0;
    u8 *target = 0;
    char *p;
    s32 t;
    s32 i;
    s32 j;
    s32 n;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Type)) != 0) {
                p += 5;
                if (lbl_80408D08 != 0 && lbl_80408D10 != 0) {
                    t = lbl_80408D08(p);
                    if (t != 0xff) {
                        t = lbl_80408D10(t);
                        if (t != 0xff && ntypes < 10)
                            types[ntypes++] = t;
                    }
                }
            } else if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0) {
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            } else if (NuStrICmp(params[i], str_Bool_FALSE) == 0) {
                on = 0;
            } else if (NuStrICmp(params[i], str_ScriptFlag_StillDoTakeHitAnim) == 0) {
                takeHit = 1;
            }
        }
        if (ntypes != 0) {
            target = lbl_80409430;
            for (i = 0; i < lbl_8040B058; i++, target += 0x1098) {
                if ((*(u32 *)(target + 0x20c) & 0x80080000) == 0x80080000
                    && (*(u32 *)(target + 0x208) & 0x400)) {
                    for (j = 0; j < ntypes; j++) {
                        if (*(s16 *)(target + 0x1074) == types[j]) {
                            if (on != 0) {
                                ((struct gameaiscript_f28b_s *)(target + 0xf28))->invulnerable = 1;
                                ((struct gameaiscript_f28b_s *)(target + 0xf28))->stillDoTakeHitAnim = takeHit;
                            } else {
                                ((struct gameaiscript_f28b_s *)(target + 0xf28))->invulnerable = 0;
                            }
                        }
                    }
                }
            }
        } else if (target != 0 && (*(u32 *)(target + 0x208) & 0x400)) {
            if (on != 0) {
                ((struct gameaiscript_f28b_s *)(target + 0xf28))->invulnerable = 1;
                ((struct gameaiscript_f28b_s *)(target + 0xf28))->stillDoTakeHitAnim = takeHit;
            } else {
                ((struct gameaiscript_f28b_s *)(target + 0xf28))->invulnerable = 0;
            }
        }
    }
    return 1;
}

extern void fn_800E6394(char *, f32, s32, f32, void *);
extern char str_ScriptParam_NameEq[];
extern char lbl_801AD878[];
extern char lbl_801AD884[];
extern char lbl_801AD890[];
extern char lbl_801AD89C[];
extern char str_ScriptParam_XEq[];
extern char str_ScriptParam_YEq[];
extern char str_ScriptParam_ZEq[];

s32 Action_PlaySfx(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    f32 repDelay = 0.0f;
    struct gameaiscript_vec_s v = { 3.4028235e38f, 3.4028235e38f, 3.4028235e38f };
    s8 playCount = 1;
    f32 startDelay = repDelay;
    char *name = 0;
    void *pos = 0;
    char *p;
    s32 i;

    if (firstTime != 0) {
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_NameEq)) != 0)
                name = p + 5;
            else if ((p = NuStrIStr(params[i], lbl_801AD878)) != 0)
                playCount = AIParamToFloat(packet, p + 10);
            else if ((p = NuStrIStr(params[i], lbl_801AD884)) != 0)
                repDelay = AIParamToFloat(packet, p + 9);
            else if ((p = NuStrIStr(params[i], lbl_801AD890)) != 0)
                startDelay = AIParamToFloat(packet, p + 11);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_XEq)) != 0)
                v.x = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_YEq)) != 0)
                v.y = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_ZEq)) != 0)
                v.z = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], lbl_801AD89C)) != 0)
                pos = (u8 *)GetNamedGameObject(sys, p + 14, 0) + 0x90;
        }
        if (pos == 0 && v.x != 3.4028235e38f && v.y != 3.4028235e38f && v.z != 3.4028235e38f)
            pos = &v;
        if (name != 0)
            fn_800E6394(name, startDelay, playCount, repDelay, pos);
    }
    return 1;
}

s32 Action_DontPush_Level(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 types[10];
    s32 on = 1;
    s32 ntypes = 0;
    u8 *target = 0;
    char *p;
    s32 t;
    s32 i;
    s32 j;
    s32 n;

    if (firstTime != 0) {
        if (packet != 0 && packet->owner != 0 && packet->owner->object != 0)
            target = packet->owner->object;
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Type)) != 0) {
                p += 5;
                if (lbl_80408D08 != 0 && lbl_80408D10 != 0) {
                    t = lbl_80408D08(p);
                    if (t != 0xff) {
                        t = lbl_80408D10(t);
                        if (t != 0xff && ntypes < 10)
                            types[ntypes++] = t;
                    }
                }
            } else if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0) {
                target = (u8 *)GetNamedGameObject(sys, p + 10, 0);
            } else if (NuStrICmp(params[i], str_Bool_FALSE) == 0) {
                on = 0;
            }
        }
        if (ntypes != 0) {
            target = lbl_80409430;
            for (i = 0; i < lbl_8040B058; i++, target += 0x1098) {
                if ((*(u32 *)(target + 0x20c) & 0x80080000) == 0x80080000
                    && (*(u32 *)(target + 0x208) & 0x400)) {
                    for (j = 0; j < ntypes; j++) {
                        if (*(s16 *)(target + 0x1074) == types[j]) {
                            ((struct gameaiscript_20c_s *)(target + 0x20c))->dontPush = on;
                        }
                    }
                }
            }
        } else {
            ((struct gameaiscript_20c_s *)(target + 0x20c))->dontPush = on;
        }
    }
    return 1;
}

struct gameaiscript_part_s {
    void *mtx;
    u32 unk4;
    void *dir;
    u32 unkc[2];
    f32 unk14;
    f32 unk18;
    f32 unk1c;
    u32 unk20;
    void *special;
    s32 unk28;
    u32 unk2c;
    s32 unk30;
    u32 unk34[4];
    void *collide;
    u32 unk48[17];
    f32 unk8c;
    u32 unk90[5];
};

extern struct gameaiscript_part_s lbl_801BB478;
extern f32 lbl_80409710;
extern f32 lbl_80409714;
extern f32 lbl_80408E9C;
extern void fn_80103A78(void *, void *, void *, f32, f32);
extern void fn_8000DC80(void *, void *);
extern void fn_80072410(void *);
extern void NewRumble(void *, f32);
extern s32 PartCollide();
extern char str_ScriptParam_X[];
extern char str_ScriptParam_Y[];
extern char str_ScriptParam_Z[];

s32 Action_AddPart(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 special[0xc];
    struct gameaiscript_vec_s pos;
    u8 dir[0x10];
    u8 mtx[0x40];
    struct gameaiscript_part_s part;
    char *p;
    s32 i;

    if (firstTime != 0) {
        memset(special, 0, sizeof(special));
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Name)) != 0)
                NuSpecialFind(*(void **)((u8 *)lbl_80409410 + 0x1c0), special, p + 5, 1);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_X)) != 0)
                pos.x = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_Y)) != 0)
                pos.y = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_Z)) != 0)
                pos.z = AIParamToFloat(packet, p + 2);
        }
        if (fn_800130F4(special) != 0) {
            fn_80103A78(dir, &pos, lbl_80409004, lbl_80409714, lbl_80409710);
            fn_8000DC80(mtx, &pos);
            part = lbl_801BB478;
            part.mtx = mtx;
            part.dir = dir;
            part.unk14 = 0.1f;
            part.unk18 = 0.1f;
            part.unk1c = lbl_80409710;
            part.unk28 = 0;
            part.special = special;
            part.unk30 = 0x29b;
            part.collide = PartCollide;
            part.unk8c = lbl_80408E9C;
            fn_80072410(&part);
            NewRumble(**(void ***)(*(u8 **)(lbl_80409004 + 0xeec) + 0x61c), 0.5f);
        }
    }
    return 1;
}

extern s32 PARTLookupType(char *);
extern void fn_80074D14(s32, void *, s32);
extern char str_ScriptParam_DXEq[];
extern char str_ScriptParam_DYEq[];
extern char str_ScriptParam_DZEq[];

s32 Action_AddPartDebris(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 special[0xc];
    struct gameaiscript_vec_s pos;
    struct gameaiscript_vec_s delta;
    s32 type = -1;
    char *p;
    u8 *obj;
    s32 i;

    memset(&delta, 0, sizeof(delta));
    if (firstTime != 0) {
        memset(special, 0, sizeof(special));
        for (i = 0; i < count; i++) {
            if ((p = NuStrIStr(params[i], str_ScriptParam_Name)) != 0)
                type = PARTLookupType(p + 5);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_DXEq)) != 0)
                delta.x = AIParamToFloat(packet, p + 3);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_DYEq)) != 0)
                delta.y = AIParamToFloat(packet, p + 3);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_DZEq)) != 0)
                delta.z = AIParamToFloat(packet, p + 3);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_XEq)) != 0)
                pos.x = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_YEq)) != 0)
                pos.y = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_ZEq)) != 0)
                pos.z = AIParamToFloat(packet, p + 2);
            else if ((p = NuStrIStr(params[i], str_ScriptParam_Character)) != 0) {
                if ((obj = (u8 *)GetNamedGameObject(sys, p + 10, 0)) != 0)
                    pos = *(struct gameaiscript_vec_s *)(obj + 0x90);
            }
        }
        if (type != -1) {
            NuVecAdd(&pos, &pos, &delta);
            fn_80074D14(type, &pos, 1);
        }
    }
    return 1;
}

extern void *AIPathFindNode(void *, void *, char *);

void *Condition_FromNodeInit(void *sys, char *param)
{
    u8 *level;

    if (sys != 0 && (level = *(u8 **)((u8 *)sys + 0xc)) != 0 && level[0] != 0)
        return AIPathFindNode(sys, *(void **)(level + 8), param);
    return 0;
}

s32 Condition_HintAvailableInit(void *process, char *param)
{
    if (param == 0)
        return 0;
    return NuAToI(param);
}

f32 Condition_ObstacleFinished(void *sys, void *process, void *packet, char *param, u8 *obstacle)
{
    if (obstacle != 0) {
        if (obstacle[0x22] == 2) {
            if (obstacle[0x23] != 0)
                return 1.0f;
        } else if (obstacle[0x1f] != 0 || (*(u32 *)(obstacle + 0x18) & 0x1000)) {
            return 1.0f;
        }
    }
    return 0.0f;
}

void *Condition_ToNodeInit(void *sys, char *param)
{
    u8 *level;

    if (sys != 0 && (level = *(u8 **)((u8 *)sys + 0xc)) != 0 && level[0] != 0)
        return AIPathFindNode(sys, *(void **)(level + 8), param);
    return 0;
}

f32 Condition_Player2Active(void)
{
    if (lbl_80409008 != 0)
        return 1.0f;
    return 0.0f;
}

f32 Condition_PlayerDistanceAlongSock(void)
{
    if (lbl_80409004 != 0)
        return *(f32 *)(lbl_80409004 + 0x62c);
    return 0.0f;
}

s32 Condition_RespawnLocatorIsInit(void *process, char *param)
{
    if (param == 0)
        return 0;
    return ((s32 (*)(void *, char *))AIPathFindLocator)(process, param);
}

u8 *Condition_TurretAliveInit(void *process, char *param)
{
    return FindTurret(*(void **)((u8 *)lbl_80409410 + 0x578), param);
}

u8 *Condition_MessageInit(void *process, char *param)
{
    u8 *r = 0;

    if (param != 0 && process != 0 && lbl_80408F58 != 0)
        r = CheckAIMessage(lbl_80408F58, param, 0);
    return r;
}

extern s32 fn_8011EBF4(void *, s32);

s32 Condition_CutSceneFinishedInit(void *process, char *param)
{
    s32 idx = FindCutScene(param);

    return idx != -1 ? fn_8011EBF4(lbl_80409410, idx) : 0;
}

f32 Condition_FurthestPlayerDistanceAlongSock(void)
{
    u8 *p2;

    if (lbl_80409004 != 0) {
        p2 = lbl_80409008;
        if (p2 != 0 && *(f32 *)(p2 + 0x62c) > *(f32 *)(lbl_80409004 + 0x62c))
            return *(f32 *)(p2 + 0x62c);
        return *(f32 *)(lbl_80409004 + 0x62c);
    }
    return 0.0f;
}

extern void fn_80107A6C(void *, s32);

s32 Action_ActivateGadget(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    void *obstacle = 0;
    s32 i;

    if (firstTime != 0 && count != 0) {
        for (i = 0; i < count; i++)
            obstacle = ((void *(*)(void *, char *))FindObstacle)(lbl_80409410, params[i]);
        if (obstacle != 0)
            fn_80107A6C(obstacle, 0);
    }
    return 1;
}

extern void fn_80108930(void *, s32, s32);

s32 Action_EndLoopingAnimation(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    char *name = 0;
    void *obstacle;
    s32 i;

    if (firstTime != 0 && count != 0) {
        for (i = 0; i < count; i++)
            name = params[i];
        if (name != 0 && (obstacle = ((void *(*)(void *, char *))FindObstacle)(lbl_80409410, name)) != 0)
            fn_80108930(obstacle, 0, 0);
    }
    return 1;
}

f32 Condition_PlayerOnDoorTrigger(void *sys, void *process, void *packet, char *param, u8 *door)
{
    s32 i;

    if (door != 0) {
        for (i = 0; i < door[6]; i++) {
            if (*(u8 **)(((u8 **)(door + 0x14))[i] + 0xa0) == lbl_80409004)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 Condition_Side(void *sys, void *process, struct gameaiscript_packet_s *packet, char *param, s32 side)
{
    u8 *c;

    if (packet != 0 && (c = (u8 *)packet->owner) != 0) {
        if (side == 1) {
            if (*(u32 *)(c + 0x208) & 5)
                return 0.0f;
            return 1.0f;
        }
        if (side == -1) {
            if (*(u32 *)(c + 0x208) & 1)
                return 1.0f;
        } else if (side == 0) {
            if (*(u32 *)(c + 0x208) & 4)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 Condition_EitherPlayerOnDoorTrigger(void *sys, void *process, void *packet, char *param, u8 *door)
{
    u8 *who;
    s32 i;

    if (door != 0) {
        for (i = 0; i < door[6]; i++) {
            who = *(u8 **)(((u8 **)(door + 0x14))[i] + 0xa0);
            if (who == lbl_80409004)
                return 1.0f;
            if (lbl_80409008 != 0 && who == lbl_80409008)
                return 1.0f;
        }
    }
    return 0.0f;
}

struct gameaiscript_iama_s {
    u8 unk0[0xc];
    char *name;
    u8 unk10[0x38];
};
extern struct gameaiscript_iama_s lbl_801D2DD0[];

s32 Condition_IAmAInit(void *process, char *param)
{
    s32 i;

    if (param != 0 && process != 0) {
        for (i = 0; i <= 0x84; i++) {
            if (NuStrICmp(lbl_801D2DD0[i].name, param) == 0)
                return i;
        }
    }
    return -1;
}

struct gameaiscript_level_s {
    u8 unk0[0x1d0];
    u32 beenTo : 1;
    u32 unk1d0 : 31;
    u8 unk1d4[4];
};
extern struct gameaiscript_level_s lbl_803EB190[];

f32 Condition_BeenToLevel(void *sys, void *process, void *packet, char *param, s32 level)
{
    if (level != -1)
        return lbl_803EB190[level].beenTo;
    return 0.0f;
}

struct gameaiscript_f24b_s {
    u32 unk0 : 5;
    u32 buttonMash : 1;
    u32 unk6 : 26;
};

s32 Action_AttackButtonMash(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    u8 *object;
    s32 i;

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0) {
        ((struct gameaiscript_f24b_s *)(object + 0xf24))->buttonMash = 1;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_Bool_FALSE) == 0)
                    ((struct gameaiscript_f24b_s *)(object + 0xf24))->buttonMash = 0;
            }
        }
    }
    return 1;
}

extern s32 lbl_804093AC;
extern s32 GetCounterLocator(s32);
extern char str_CharacterName_Dexter[];

s32 Action_SetCounterLocator(void *sys, void *process, struct gameaiscript_packet_s *packet,
    char **params, s32 count, s32 firstTime, f32 time)
{
    s32 dexter = 0;
    u8 *object;
    s32 i;

    if (lbl_804093AC == 0x7b && firstTime != 0 && packet != 0 && packet->owner != 0
        && (object = (u8 *)packet->owner->object) != 0) {
        if (count != 0) {
            for (i = 0; i < count; i++) {
                if (NuStrICmp(params[i], str_CharacterName_Dexter) == 0)
                    dexter = 1;
            }
        }
        *(s32 *)(object + 0x39c) = GetCounterLocator(dexter);
    }
    return 1;
}

extern u8 lbl_801D68EC[];

s32 Condition_BeenToLevelInit(void *process, char *param)
{
    s32 i;

    if (param != 0 && process != 0 && *(u8 **)((u8 *)lbl_80409410 + 0x1ac) != 0) {
        for (i = 0; i < (*(u8 **)((u8 *)lbl_80409410 + 0x1ac))[0x3a]; i++) {
            if (NuStrICmp(param, *(char **)(lbl_801D68EC
                    + (*(s16 **)*(u8 **)((u8 *)lbl_80409410 + 0x1ac))[i] * 0xa8)) == 0)
                return i;
        }
    }
    return -1;
}

f32 fn_8012D13C(u8 *a)
{
    u8 *b;

    if (a != 0 && (b = *(u8 **)(a + 4)) != 0) {
        switch (b[0xb0]) {
        case 0:
            return *(f32 *)(*(u8 **)(*(u8 **)(a + 0x64) + 0x20) + 0x18) * lbl_80408E9C;
        case 1:
            return *(f32 *)(*(u8 **)(*(u8 **)(a + 0x64) + 0x20) + 0x14) * lbl_80408E9C;
        case 2:
            return *(f32 *)(*(u8 **)(*(u8 **)(a + 0x64) + 0x20) + 0x10) * lbl_80408E9C;
        }
    }
    return 0.0f;
}

struct gameaiscript_cam_s {
    u8 unk0[0xd0];
    struct gameaiscript_vec_s dir;
};
extern u8 gCameraState[];
extern void NuVecSub(void *, void *, void *);

f32 Condition_BehindCamera(void *sys, void *process, struct gameaiscript_packet_s *packet)
{
    struct gameaiscript_vec_s d;
    u8 *object;

    if (packet != 0 && packet->owner != 0 && (object = (u8 *)packet->owner->object) != 0) {
        NuVecSub(&d, object + 0x90, gCameraState);
        if (d.x * ((struct gameaiscript_cam_s *)MenuHeader)->dir.x
                + d.y * ((struct gameaiscript_cam_s *)MenuHeader)->dir.y
                + d.z * ((struct gameaiscript_cam_s *)MenuHeader)->dir.z < 0.0f)
            return 1.0f;
    }
    return 0.0f;
}
