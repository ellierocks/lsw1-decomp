#include "game/ai.h"

extern s32 lbl_80408CC8;
extern s32 lbl_80408CCC;
extern s32 lbl_80408D08;
extern s32 lbl_80408D0C;
extern s32 lbl_80408D10;
extern s32 lbl_80408D14;
extern s32 lbl_80408D18;
extern s32 lbl_80408D1C;
extern s32 lbl_80408D20;
extern s32 lbl_80408D24;
extern s32 lbl_80408D38;
extern s32 lbl_80408D3C;
extern s32 lbl_80408D40;
extern s32 lbl_80408D44;
extern s32 lbl_80408D48;
extern s32 lbl_80408D4C;

void fn_80083A3C(s32 a)
{
    lbl_80408CCC = a;
}

void fn_80083A44(s32 a)
{
    lbl_80408CC8 = a;
}

void fn_8008F224(s32 a)
{
    lbl_80408D08 = a;
}

void fn_8008F22C(s32 a)
{
    lbl_80408D0C = a;
}

void fn_8008F234(s32 a)
{
    lbl_80408D10 = a;
}

void fn_8008F23C(s32 a)
{
    lbl_80408D14 = a;
}

void fn_8008F244(s32 a)
{
    lbl_80408D18 = a;
}

void fn_8008F24C(s32 a)
{
    lbl_80408D1C = a;
}

void fn_8008F254(s32 a)
{
    lbl_80408D20 = a;
}

void fn_8008F25C(s32 a)
{
    lbl_80408D24 = a;
}

void fn_8008F264(s32 a)
{
    lbl_80408D38 = a;
}

void fn_8008F26C(s32 a)
{
    lbl_80408D3C = a;
}

void fn_8008F274(s32 a)
{
    lbl_80408D40 = a;
}

void fn_8008F27C(s32 a)
{
    lbl_80408D44 = a;
}

void fn_8008F284(s32 a)
{
    lbl_80408D48 = a;
}

void fn_8008F28C(s32 a)
{
    lbl_80408D4C = a;
}

f32 Condition_Timer(s32 unused3, struct NUAISCRIPTPROCESS_s* a)
{
    return a->timer;
}

f32 Condition_GotLocator(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (p && p->locator) return 1.0f;
    return 0.0f;
}

f32 Condition_GotTriggerArea(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (p && p->triggerArea) return 1.0f;
    return 0.0f;
}

f32 Condition_GotOpponent(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (p && p->opponent) return 1.0f;
    return 0.0f;
}

f32 Condition_OnPath(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (p && (p->pathFlags & 0x8000)) return 1.0f;
    return 0.0f;
}

f32 Condition_PathBlocked(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (p && (p->movementFlags & 0x100)) return 1.0f;
    return 0.0f;
}

f32 Condition_TimeOffPath(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (!p) return 0.0f;
    /* The redundant guard preserves retail null-path block placement. */
    if (p) return p->timeOffPath;
    return p->timeOffPath;
}

f32 Condition_OpponentRange(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (p && p->opponent) return p->opponentRange;
    return 3.402823466e+38f;
}

f32 Condition_NearestOpponentRange(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    if (p && p->nearestOpponent) return p->nearestOpponentRange;
    return 3.402823466e+38f;
}

f32 Condition_StuckTime(void* sys, void* proc, struct NUAIPACKET_s* p)
{
    struct NUAPIOBJECT_s* owner;
    if (p && (owner = p->owner)) return owner->stuckTime;
    return 0.0f;
}

s32 Action_ResetTimer(void* sys, struct NUAISCRIPTPROCESS_s* proc,
    struct NUAIPACKET_s* packet, void* params, s32 count, s32 start)
{
    if (start) proc->timer = 0.0f;
    return 1;
}

extern s32 NuStrICmp(char*, char*);
extern char lbl_8019BEA0[];

struct ai_action_owner_s {
    u8 pad0[0x208];
    u32 flags208;
    u32 flags20c;
};

struct ai_action_packet_s {
    u8 pad0[0x50];
    struct ai_action_owner_s* owner;
    u8 pad54[0x14c];
    u32 flags1a0;
};

s32 Action_IgnoreWallSplines(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && firstTime != 0) {
        packet->flags1a0 |= 0x01000000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->flags1a0 &= ~0x01000000;
            }
        }
    }
    return 1;
}

s32 Action_CheckWallSplines(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && firstTime != 0) {
        packet->flags1a0 |= 0x00800000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->flags1a0 &= ~0x00800000;
            }
        }
    }
    return 1;
}

s32 Action_RequiresLOS(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && firstTime != 0) {
        packet->flags1a0 |= 0x00002000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->flags1a0 &= ~0x00002000;
            }
        }
    }
    return 1;
}

s32 Action_NoTerrain(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c |= 0x04000000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c &= ~0x04000000;
            }
        }
    }
    return 1;
}

s32 Action_NoLosCheck(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c |= 0x00200000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c &= ~0x00200000;
            }
        }
    }
    return 1;
}

s32 Action_FlatTerrain(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c |= 0x08000000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c &= ~0x08000000;
            }
        }
    }
    return 1;
}

s32 Action_ShadowTerrain(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c |= 0x10000000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c &= ~0x10000000;
            }
        }
    }
    return 1;
}

s32 Action_DontUseShadowTerrain(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c |= 0x02000000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c &= ~0x02000000;
            }
        }
    }
    return 1;
}

s32 Action_CanSeeBehind(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c |= 0x00100000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c &= ~0x00100000;
            }
        }
    }
    return 1;
}

s32 Action_SetFullPathSearch(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c &= ~0x00002000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c |= 0x00002000;
            }
        }
    }
    return 1;
}

s32 Action_NoShadows(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && firstTime != 0 && packet->owner != 0) {
        packet->owner->flags208 |= 0x00002000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags208 &= ~0x00002000;
            }
        }
    }
    return 1;
}

s32 Action_DontPush(void* sys, void* process, struct ai_action_packet_s* packet,
                           char** params, s32 count, s32 firstTime, f32 time)
{
    s32 index;

    if (packet != 0 && packet->owner != 0 && firstTime != 0) {
        packet->owner->flags20c |= 0x40000000;
        for (index = 0; index < count; index++) {
            if (NuStrICmp(params[index], lbl_8019BEA0) == 0) {
                packet->owner->flags20c &= ~0x40000000;
            }
        }
    }
    return 1;
}

extern u64 UI_DrawText();
extern f32 lbl_8019C59C[];
extern f32 lbl_8019C5A0[];

f32 Condition_InTriggerArea(void* sys, void* process, u8* packet, char* params, u8* inputArea)
{
    u8* owner;
    u8* area;

    if (packet != 0 && (owner = *(u8**)(packet + 0x50)) != 0) {
        if (inputArea == 0) {
            area = *(u8**)(packet + 0xf8);
        } else {
            area = inputArea;
        }
        if (area != 0) {
            if ((*(u64*)(area + 0x30) & UI_DrawText(0, 1, owner[0x291])) != 0) {
                return lbl_8019C59C[0];
            }
        }
    }
    return lbl_8019C5A0[0];
}

extern f32 lbl_8019C5F8[];
extern f32 lbl_8019C5FC[];
f32 Condition_OpponentInTriggerArea(void* sys, void* process, u8* packet, char* params, u8* inputArea)
{
    u8* owner;
    u8* area;

    if (packet != 0 && (owner = *(u8**)(packet + 0x64)) != 0 && *(u8**)(packet + 0x50) != 0) {
        if (inputArea == 0) {
            area = *(u8**)(packet + 0xf8);
        } else {
            area = inputArea;
        }
        if (area != 0) {
            if ((*(u64*)(area + 0x30) & UI_DrawText(0, 1, owner[0x291])) != 0) {
                return lbl_8019C5F8[0];
            }
        }
    }
    return lbl_8019C5FC[0];
}

extern f32 lbl_8019C5D0[];
extern f32 lbl_8019C5D4[];

f32 Condition_PlayerInTriggerArea(u8* sys, void* process, u8* packet, char* params, u8* inputArea)
{
    u8* area = 0;

    if (sys != 0 && *(u8**)(sys + 0x1380) != 0) {
        if (inputArea != 0) {
            area = inputArea;
        } else if (packet != 0) {
            area = *(u8**)(packet + 0xf8);
        }
        if (area != 0) {
            if ((*(u64*)(area + 0x30) & UI_DrawText(0, 1, (*(u8**)(sys + 0x1380))[0x291])) != 0) {
                return lbl_8019C5D0[0];
            }
        }
    }
    return lbl_8019C5D4[0];
}

extern f32 fn_8000C038(void);
extern void NuFParPopCom(void*);
f32 Condition_Random(void)
{
    return fn_8000C038();
}
void fn_80083A4C(void* parser)
{
    NuFParPopCom(parser);
}

extern f32 lbl_8019C594[];
extern f32 lbl_8019C598[];
extern void* fn_8008F720(void*, char*);
extern void* fn_8008F7A0(void*, char*);
extern void* memset(void*, ...);

f32 Condition_CurrentLocatorIs(void* sys, void* process, struct NUAIPACKET_s* packet, char* params, void* locator)
{
    if (locator && packet && packet->locator == locator)
        return lbl_8019C594[0];
    return lbl_8019C598[0];
}

void fn_8008F1F0(void* packet)
{
    if (packet)
        memset(packet, 0, 0x134);
}

void* Condition_CurrentLocatorIsInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F720(sys, arg);
    return 0;
}

void* Condition_OpponentToLocatorInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F720(sys, arg);
    return 0;
}

void* Condition_PlayerToLocatorInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F720(sys, arg);
    return 0;
}

void* Condition_LocatorRangeInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F720(sys, arg);
    return 0;
}

void* Condition_InTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F7A0(sys, arg);
    return 0;
}

void* Condition_PlayerInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F7A0(sys, arg);
    return 0;
}

void* Condition_BaddyInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F7A0(sys, arg);
    return 0;
}

void* Condition_GoodyInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F7A0(sys, arg);
    return 0;
}

void* Condition_OpponentInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return fn_8008F7A0(sys, arg);
    return 0;
}
