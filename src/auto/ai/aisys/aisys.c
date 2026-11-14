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

extern u64 __ashldi3();
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
            if ((*(u64*)(area + 0x30) & __ashldi3(0, 1, owner[0x291])) != 0) {
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
            if ((*(u64*)(area + 0x30) & __ashldi3(0, 1, owner[0x291])) != 0) {
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
            if ((*(u64*)(area + 0x30) & __ashldi3(0, 1, (*(u8**)(sys + 0x1380))[0x291])) != 0) {
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
extern void* AIPathFindLocator(void*, char*);
extern void* AISysFindArea(void*, char*);
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
        return AIPathFindLocator(sys, arg);
    return 0;
}

void* Condition_OpponentToLocatorInit(void* sys, char* arg)
{
    if (arg)
        return AIPathFindLocator(sys, arg);
    return 0;
}

void* Condition_PlayerToLocatorInit(void* sys, char* arg)
{
    if (arg)
        return AIPathFindLocator(sys, arg);
    return 0;
}

void* Condition_LocatorRangeInit(void* sys, char* arg)
{
    if (arg)
        return AIPathFindLocator(sys, arg);
    return 0;
}

void* Condition_InTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return AISysFindArea(sys, arg);
    return 0;
}

void* Condition_PlayerInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return AISysFindArea(sys, arg);
    return 0;
}

void* Condition_BaddyInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return AISysFindArea(sys, arg);
    return 0;
}

void* Condition_GoodyInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return AISysFindArea(sys, arg);
    return 0;
}

void* Condition_OpponentInTriggerAreaInit(void* sys, char* arg)
{
    if (arg)
        return AISysFindArea(sys, arg);
    return 0;
}

extern f32 lbl_8019C608[], lbl_8019C60C[], lbl_8019C670[];
extern f32 lbl_8019C5E8[], lbl_8019C5EC[], lbl_8019C5F0[], lbl_8019C5F4[];
extern void* (*lbl_80408DA4)(void*, char*);
extern void* AIPathFindNode(void*, void*, char*);

f32 Condition_OpponentIsAThreat(void* sys, void* process, struct NUAIPACKET_s* packet)
{
    if (packet && packet->opponent && (packet->movementFlags & 0x100000))
        return lbl_8019C608[0];
    return lbl_8019C60C[0];
}

void fn_8008F090(u8* p)
{
    if (p[0x49] & 1)
        memset(p, 0, 0x4c);
}

void* Condition_IAmInit(void* sys, char* arg)
{
    if (arg && lbl_80408DA4)
        return lbl_80408DA4(sys, arg);
    return 0;
}

f32 Condition_BaddyInTriggerArea(void* sys, void* process, u8* packet, char* params, u8* inputArea)
{
    u8* area = 0;
    if (inputArea) area = inputArea;
    else if (packet) area = *(u8**)(packet + 0xf8);
    if (area && (area[0x2a] & 4)) return lbl_8019C5E8[0];
    return lbl_8019C5EC[0];
}

f32 Condition_GoodyInTriggerArea(void* sys, void* process, u8* packet, char* params, u8* inputArea)
{
    u8* area = 0;
    if (inputArea) area = inputArea;
    else if (packet) area = *(u8**)(packet + 0xf8);
    if (area && (area[0x2a] & 2)) return lbl_8019C5F0[0];
    return lbl_8019C5F4[0];
}

void* Condition_InLevelNodeInit(u8* sys, char* arg)
{
    u8* level;
    if (sys && (level = *(u8**)(sys + 0xc)) && *level)
        return AIPathFindNode(sys, *(void**)(level + 8), arg);
    return 0;
}

void* Condition_PlayerInLevelNodeInit(u8* sys, char* arg)
{
    u8* level;
    if (sys && (level = *(u8**)(sys + 0xc)) && *level)
        return AIPathFindNode(sys, *(void**)(level + 8), arg);
    return 0;
}

void* fn_80084058(u8* sys, char* arg)
{
    u8* level;
    if (sys && (level = *(u8**)(sys + 0xc)) && *level)
        return AIPathFindNode(sys, *(void**)(level + 8), arg);
    return 0;
}

extern f32 lbl_8019C650[], lbl_8019C630[], lbl_8019C634[], lbl_8019C638[];
extern f32 lbl_8019C610[], lbl_8019C614[];

f32 Condition_OpponentBelow(void* sys, void* process, u8* packet)
{
    u8* opponent;
    if (packet && (opponent = *(u8**)(packet + 0x64))) {
        u8* owner = *(u8**)(packet + 0x50);
        if (*(f32*)(opponent + 0x70) < *(f32*)(owner + 0x70) - lbl_8019C630[0])
            return lbl_8019C634[0];
    }
    return lbl_8019C638[0];
}

f32 Condition_OpponentOnSamePath(void* sys, void* process, u8* packet)
{
    u8* opponent;
    u8* otherPacket;
    void* path;
    if (packet && (opponent = *(u8**)(packet + 0x64)) && (path = *(void**)(packet + 0x10c)) &&
        (otherPacket = *(u8**)(opponent + 4)) && path == *(void**)(otherPacket + 0x10c))
        return lbl_8019C610[0];
    return lbl_8019C614[0];
}


extern f32 NuVecDist(void*, void*, void*);
extern f32 NuVecXZDist(void*, void*, void*);
extern f32 lbl_8019C5C4[], lbl_8019C5A4[], lbl_8019C578[], lbl_8019C57C[], lbl_8019C648[], lbl_8019C64C[];

f32 Condition_NodeRange(void* sys, void* process, u8* packet, char* params, u8* node)
{
    u8* owner;
    f32 out[3], result;
    if (packet && (owner = *(u8**)(packet + 0x50)) && node)
        result = NuVecDist(node + 4, owner + 0x6c, out);
    else result = lbl_8019C5C4[0];
    return result;
}

f32 Condition_PlayerRange(u8* sys, void* process, u8* packet)
{
    u8* owner;
    u8* player;
    f32 out[3], result;
    if (packet && (owner = *(u8**)(packet + 0x50)) && sys && (player = *(u8**)(sys + 0x1380)))
        result = NuVecDist(player + 0x6c, owner + 0x6c, out);
    else result = lbl_8019C5A4[0];
    return result;
}


extern f32 lbl_8019C63C[], lbl_8019C640[], lbl_8019C644[];
f32 Condition_OriginRange(u8* sys, void* process, u8* packet)
{
    f32 out[3], result;
    if (packet && *(void**)(packet + 0x50) && packet[0xa6] != 0xff && sys)
        result = NuVecDist(packet + 0x100, *(u8**)(sys + 0x14) + packet[0xa6] * 0xa8 + 0x30, out);
    else result = lbl_8019C63C[0];
    return result;
}
f32 Condition_OpponentToOrigin(u8* sys, void* process, u8* packet)
{
    u8* opponent;
    f32 out[3], result;
    if (sys && packet && (opponent = *(u8**)(packet + 0x64)) && packet[0xa6] != 0xff)
        result = NuVecDist(opponent + 0x6c, *(u8**)(sys + 0x14) + packet[0xa6] * 0xa8 + 0x30, out);
    else result = lbl_8019C640[0];
    return result;
}
f32 Condition_PlayerToOrigin(u8* sys, void* process, u8* packet)
{
    u8* player;
    f32 out[3], result;
    if (sys && packet && (player = *(u8**)(sys + 0x1380)) && packet[0xa6] != 0xff)
        result = NuVecDist(player + 0x6c, *(u8**)(sys + 0x14) + packet[0xa6] * 0xa8 + 0x30, out);
    else result = lbl_8019C644[0];
    return result;
}

extern void* fn_800076C4(void*);
extern void* fn_800076CC(void*, void*);
extern f32 lbl_8019C674[], lbl_8019C678[];f32 Condition_IAm(void* sys, void* process, u8* packet, char* name, void* actor)
{
    u8* owner;
    u8* model;
    f32 result;
    if (packet && (owner = *(u8**)(packet + 0x50)) &&
        (owner == actor || ((model = *(u8**)(owner + 0x64)) && *(char**)(model + 0xc) && NuStrICmp(*(char**)(model + 0xc), name) == 0)))
        result = lbl_8019C674[0];
    else result = lbl_8019C678[0];
    return result;
}

extern void* fn_8008386C(char*, u8*);
s32 fn_8008F35C(u8* process, u32 priority, u32 id, char* name, f32 value)
{
    void* script;
    if (priority >= process[0x44] && (script = fn_8008386C(name, *(void**)(process + 4)))) {
        process[0x44] = priority;
        process[0x45] = id;
        *(f32*)(process + 0x48) = value;
        *(void**)(process + 0x4c) = script;
        return 1;
    }
    return 0;
}

extern char lbl_8019BFCC[];
extern char* NuStrIStr(char*, char*);
extern void fn_8008F3D4(void*, char*);
s32 Action_ClearInterrupt(void* sys, void* process, void* packet, char** args, s32 count, void* active)
{
    char* name = 0;
    s32 i;
    if (active && process) {
        for (i = 0; i < count; i++) {
            char* found = NuStrIStr(args[i], lbl_8019BFCC);
            if (found) name = found + 6;
        }
        if (name) fn_8008F3D4(process, name);
    }
    return 1;
}

struct ai_level_node { u8 bytes[0x58]; };
extern f32 lbl_8019C5AC[], lbl_8019C5B0[], lbl_8019C5B4[], lbl_8019C5B8[];
f32 Condition_InLevelNode(u8* sys, void* process, u8* packet, char* params, struct ai_level_node* node)
{
    if (packet && node && sys) {
        u8* level = *(u8**)(sys + 0xc);
        u8* data = *(u8**)(level + 8);
        if (*(s16*)(packet + 0xa0) == node - *(struct ai_level_node**)(data + 0x78))
            return lbl_8019C5AC[0];
    }
    return lbl_8019C5B0[0];
}
f32 Condition_PlayerInLevelNode(u8* sys, void* process, void* packet, char* params, struct ai_level_node* node)
{
    u8* player;
    if (sys && (player = *(u8**)(sys + 0x1380)) && node) {
        u8* level = *(u8**)(sys + 0xc);
        u8* data = *(u8**)(level + 8);
        u8* ai = *(u8**)(player + 4);
        if (*(s16*)(ai + 0xa0) == node - *(struct ai_level_node**)(data + 0x78))
            return lbl_8019C5B4[0];
    }
    return lbl_8019C5B8[0];
}

void fn_800838EC(u8* process, u8* script)
{
    void* state;
    if (process && script) {
        *(u8**)(process + 8) = script;
        *(void**)(process + 0xc) = 0;
        *(void**)(process + 0x10) = 0;
        state = fn_800076C4(script + 0x10);
        if (state) {
            *(void**)(process + 0xc) = state;
            *(u32*)(process + 0x3c) |= 0x80000000;
        } else *(void**)(process + 0xc) = state;
    }
}


extern f32 lbl_8019C5D8[], lbl_8019C5DC[];
f32 Condition_Player2InTriggerArea(u8* sys, void* process, u8* packet, char* params, u8* inputArea)
{
    u8* area = 0;

    if (sys != 0 && *(u8**)(sys + 0x1384) != 0) {
        if (inputArea != 0) {
            area = inputArea;
        } else if (packet != 0) {
            area = *(u8**)(packet + 0xf8);
        }
        if (area != 0) {
            if ((*(u64*)(area + 0x30) & __ashldi3(0, 1, (*(u8**)(sys + 0x1384))[0x291])) != 0) {
                return lbl_8019C5D8[0];
            }
        }
    }
    return lbl_8019C5DC[0];
}

extern f32 lbl_8019C5BC[], lbl_8019C5C0[];
f32 Condition_EitherPlayerInLevelNode(u8* sys, void* process, void* packet, char* params, struct ai_level_node* node)
{
    u8* player;
    if (sys && node) {
        player = *(u8**)(sys + 0x1380);
        if (player && *(s16*)(*(u8**)(player + 4) + 0xa0) == node - *(struct ai_level_node**)(*(u8**)(*(u8**)(sys + 0xc) + 8) + 0x78))
            return lbl_8019C5BC[0];
        player = *(u8**)(sys + 0x1384);
        if (player && *(s16*)(*(u8**)(player + 4) + 0xa0) == node - *(struct ai_level_node**)(*(u8**)(*(u8**)(sys + 0xc) + 8) + 0x78))
            return lbl_8019C5BC[0];
    }
    return lbl_8019C5C0[0];
}
extern f32 fn_8000CD08(void*, void*, void*);
extern f32 NuFsqrt(f32);
extern f32 lbl_8019C5A8[];
f32 Condition_NearestPlayerRange(u8* sys, void* process, u8* packet)
{
    f32 distance = lbl_8019C5A8[0];
    f32 out[3];
    if (packet && *(u8**)(packet + 0x50) && sys) {
        u8* player = *(u8**)(sys + 0x1380);
        if (player)
            distance = fn_8000CD08(player + 0x6c, *(u8**)(packet + 0x50) + 0x6c, out);
        player = *(u8**)(sys + 0x1384);
        if (player) {
            f32 other = fn_8000CD08(player + 0x6c, *(u8**)(packet + 0x50) + 0x6c, out);
            if (other < distance) distance = other;
        }
        if (distance != lbl_8019C5A8[0]) distance = NuFsqrt(distance);
    }
    return distance;
}
extern f32 lbl_8019C5E0[], lbl_8019C5E4[];extern void (*fn_80009948(char*, int))(char*, ...);
extern char lbl_8019C6DC[], str_AISysBufferAlloc_LevelBufferTooSmall[], str_AISysBufferAlloc_NoBuffer[];
void* fn_8008F0C8(u32* current, u32* end, u32 size)
{
    u32 result = 0;
    if (current && end) {
        if (*current + size < *end) {
            result = (*current + 15) & ~15;
            *current = result + size;
        } else fn_80009948(lbl_8019C6DC, 0x3bd)(str_AISysBufferAlloc_LevelBufferTooSmall);
    } else fn_80009948(lbl_8019C6DC, 0x3c1)(str_AISysBufferAlloc_NoBuffer);
    return (void*)result;
}
s32 AISysSetLevelPath(u8* sys, char* name)
{
    char* selected = 0;
    if (sys && *(u8**)(sys + 0xc)) {
        if (name) {
            s32 i;
            for (i = 0; i < **(u8**)(sys + 0xc); i++) {
                u8* level = *(u8**)(sys + 0xc);
                char** names = *(char***)(level + 4);
                if (NuStrICmp(names[i], name) == 0) {
                    selected = (*(char***)(*(u8**)(sys + 0xc) + 4))[i];
                    break;
                }
            }
        } else selected = **(char****)(*(u8**)(sys + 0xc) + 4);
        if (selected && selected != *(char**)(*(u8**)(sys + 0xc) + 8)) {
            *(char**)(*(u8**)(sys + 0xc) + 8) = selected;
            return 1;
        }
    }
    return 0;
}
extern char lbl_8019BF88[], lbl_8019BF90[], lbl_8019BFB4[];
extern u8* fn_8008A6D4(void*, void*, char*, char*, s32*);
s32 Action_PathConnectionObstacle(void* sys, void* process, void* packet, char** args, s32 count, void* active)
{
    s32 enabled = 0;
    s32 unused = 0;
    char* from = 0;
    char* to = 0;
    s32 i;
    if (sys && active) {
        for (i = 0; i < count; i++) {
            char* found = NuStrIStr(args[i], lbl_8019BF88);
            if (found) from = found + 5;
            else if ((found = NuStrIStr(args[i], lbl_8019BF90))) to = found + 3;
            else if (!NuStrICmp(args[i], lbl_8019BFB4)) enabled = 1;
        }
        if (from && to) {
            u8* connection = fn_8008A6D4(sys, 0, from, to, &unused);
            if (connection && (*(u32*)(connection + 4) & 0x20000000)) connection[0x21] = enabled;
        }
    }
    return 1;
}

void* fn_8008386C(char* name, u8* script)
{
    u8* entry = fn_800076C4(script + 0x10);
    if (!name) return 0;
    for (; entry; entry = fn_800076CC(script + 0x10, entry))
        if (!NuStrICmp(name, *(char**)(entry + 0x18))) return entry;
    return 0;
}
f32 Condition_InterruptID(void* sys, u8* process)
{
    if (process) return (f32)process[0x45];
    return lbl_8019C670[0];
}

f32 Condition_OpponentToLocatorY(void* sys, void* process, u8* packet, char* params, u8* inputLocator)
{
    u8* opponent;
    u8* locator;
    if (packet && (opponent = *(u8**)(packet + 0x64))) {
        if (inputLocator) locator = inputLocator;
        else locator = *(u8**)(packet + 0xfc);
        if (locator) return *(f32*)(opponent + 0x70) - *(f32*)(locator + 0x14);
    }
    return lbl_8019C650[0];
}

f32 Condition_LocatorRange(void* sys, void* process, u8* packet, char* params, u8* inputLocator)
{
    u8* object;
    u8* locator;
    f32 out[3], result;
    if (packet && (object = *(u8**)(packet + 0x50))) {
        if (inputLocator) locator = inputLocator;
        else locator = *(u8**)(packet + 0xfc);
        if (locator) result = NuVecDist(packet + 0x100, locator + 0x10, out);
        else result = lbl_8019C578[0];
    } else result = lbl_8019C578[0];
    return result;
}

f32 Condition_LocatorRangeXZ(void* sys, void* process, u8* packet, char* params, u8* inputLocator)
{
    u8* object;
    u8* locator;
    f32 out[3], result;
    if (packet && (object = *(u8**)(packet + 0x50))) {
        if (inputLocator) locator = inputLocator;
        else locator = *(u8**)(packet + 0xfc);
        if (locator) result = NuVecXZDist(packet + 0x100, locator + 0x10, out);
        else result = lbl_8019C57C[0];
    } else result = lbl_8019C57C[0];
    return result;
}

f32 Condition_OpponentToLocator(void* sys, void* process, u8* packet, char* params, u8* inputLocator)
{
    u8* object;
    u8* locator;
    f32 out[3], result;
    if (packet && (object = *(u8**)(packet + 0x64))) {
        if (inputLocator) locator = inputLocator;
        else locator = *(u8**)(packet + 0xfc);
        if (locator) result = NuVecDist(object + 0x6c, locator + 0x10, out);
        else result = lbl_8019C648[0];
    } else result = lbl_8019C648[0];
    return result;
}

f32 Condition_OpponentToLocatorXZ(void* sys, void* process, u8* packet, char* params, u8* inputLocator)
{
    u8* object;
    u8* locator;
    f32 out[3], result;
    if (packet && (object = *(u8**)(packet + 0x64))) {
        if (inputLocator) locator = inputLocator;
        else locator = *(u8**)(packet + 0xfc);
        if (locator) result = NuVecXZDist(object + 0x6c, locator + 0x10, out);
        else result = lbl_8019C64C[0];
    } else result = lbl_8019C64C[0];
    return result;
}

extern f32 lbl_8019C658[];
f32 Condition_NearestPlayerToLocator(u8* sys, void* process, u8* packet, char* params, u8* inputLocator)
{
    f32 distance = lbl_8019C658[0];
    f32 out[3];
    if (packet && sys) {
        u8* locator;
        if (inputLocator) locator = inputLocator;
        else locator = *(u8**)(packet + 0xfc);
        if (locator) {
            u8* player = *(u8**)(sys + 0x1380);
            if (player) distance = fn_8000CD08(player + 0x6c, locator + 0x10, out);
            player = *(u8**)(sys + 0x1384);
            if (player) {
                f32 other = fn_8000CD08(player + 0x6c, locator + 0x10, out);
                if (other < distance) distance = other;
            }
        }
        if (distance != lbl_8019C658[0]) distance = NuFsqrt(distance);
    }
    return distance;
}
extern f32 lbl_8019C654[];
f32 Condition_PlayerToLocator(u8* sys, void* process, u8* packet, char* params, u8* inputLocator)
{
    u8* player;
    u8* locator;
    f32 out[3], result;
    if (packet && sys && (player = *(u8**)(sys + 0x1380))) {
        if (inputLocator) locator = inputLocator;
        else locator = *(u8**)(packet + 0xfc);
        if (locator) result = NuVecDist(player + 0x6c, locator + 0x10, out);
        else result = lbl_8019C654[0];
    } else result = lbl_8019C654[0];
    return result;
}

void fn_8008F968(void *);

void *fn_8008EE48(s32 *arg0, u32 *arg1, s32 arg2) {
    s32 temp_r10;
    s32 temp_r11;
    s32 temp_r11_2;
    s32 var_r28;
    s32 var_r29;

    var_r29 = 0;
    if ((arg0 != NULL) && (arg1 != NULL)) {
        temp_r11 = *arg0;
        if ((u32) (temp_r11 + 0x18) < (u32) *arg1) {
            var_r29 = (temp_r11 + 0xF) & 0xFFFFFFF0;
            *arg0 = var_r29 + 0x18;
        } else {
            fn_80009948(lbl_8019C6DC, 0x3BD)(str_AISysBufferAlloc_LevelBufferTooSmall);
        }
    } else {
        fn_80009948(lbl_8019C6DC, 0x3C1)(str_AISysBufferAlloc_NoBuffer);
    }
    if (var_r29 != 0) {
        var_r28 = 0;
        memset((void *) var_r29, 0, 0x18);
        temp_r10 = arg2 * 0x2C;
        if ((arg0 != NULL) && (arg1 != NULL)) {
            temp_r11_2 = *arg0;
            if ((u32) (temp_r11_2 + temp_r10) < (u32) *arg1) {
                var_r28 = (temp_r11_2 + 0xF) & 0xFFFFFFF0;
                *arg0 = var_r28 + temp_r10;
            } else {
                fn_80009948(lbl_8019C6DC, 0x3BD)(str_AISysBufferAlloc_LevelBufferTooSmall);
            }
        } else {
            fn_80009948(lbl_8019C6DC, 0x3C1)(str_AISysBufferAlloc_NoBuffer);
        }
        (*(s32 *)((s8 *)(var_r29) + (4))) = var_r28;
        if (var_r28 != 0) {
            (*(s32 *)((s8 *)(var_r29) + (0))) = arg2;
            fn_8008F968((void *) var_r29);
        }
    }
    return (void *) var_r29;
}

struct aiscriptparam_s {
    char *name;
    f32 value;
};
struct aiscript_s {
    u8 pad0[0x18];
    struct aiscriptparam_s params[4];
};
struct aiparamproc_s {
    u8 pad0[4];
    struct aiscript_s *script;
    u8 pad8[0x38];
    f32 actionTimer;
};

extern s32 NuAToI(char *s);
extern s32 strcasecmp(const char *a, const char *b);
extern f32 fn_800077E8(char *s);
extern char str_ScriptToken_ParamLower[];
extern f32 lbl_8019C574[];

f32 AIParamToFloat(struct aiparamproc_s *processor, char *param) {
    char *cursor;
    u32 n;
    s32 i;

    if (param != NULL) {
        if (processor != NULL) {
            cursor = NuStrIStr(param, str_ScriptToken_ParamLower);
            if (cursor != NULL) {
                i = NuAToI(cursor);
                if ((u32)i < 4) return processor->script->params[i].value;
            } else if (processor->script != NULL) {
                for (i = 0; i < 4; i++) {
                    if (processor->script->params[i].name != NULL &&
                        strcasecmp(processor->script->params[i].name, param) == 0) {
                        return processor->script->params[i].value;
                    }
                }
            }
        }
        return fn_800077E8(param);
    }
    return lbl_8019C574[0];
}
static inline f32 aiParamFloat(struct aiparamproc_s *processor, char *param)
{
    char *cursor;
    s32 i;
    if (param != NULL) {
        if (processor != NULL) {
            cursor = NuStrIStr(param, str_ScriptToken_ParamLower);
            if (cursor != NULL) {
                i = NuAToI(cursor);
                if ((u32)i < 4) return processor->script->params[i].value;
            } else if (processor->script != NULL) {
                for (i = 0; i < 4; i++) {
                    if (processor->script->params[i].name != NULL &&
                        strcasecmp(processor->script->params[i].name, param) == 0) {
                        return processor->script->params[i].value;
                    }
                }
            }
        }
        return fn_800077E8(param);
    }
    return 0.0f;
}
struct aimovevec_s { f32 x, y, z; };
struct aimoveview_s {
    u8 pad0[0x9c];
    f32 field9c;
    u8 padA0[0x6c];
    struct aimovevec_s field10c;
    u8 pad118[0xc];
    struct aimovevec_s field124;
};

extern s32 (*lbl_80408D2C)(char*, void*);
extern f32 lbl_8019BE30[];
extern void AIMoveInstruction(void*,void*,f32,void*,s32,f32);
s32 Action_FollowOpponent(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet) return 1;
    if (firstTime) {
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(packet + 0x64);
    if (target && *(u8**)(target + 4)) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 1, *(f32*)(packet + 0x134));
    }
    return 0;
}

struct aicreatureview_s {
    u8 pad0[0x98];
    f32 field98;
    f32 field9c;
    f32 fielda0;
    f32 fielda4;
};

s32 Action_FollowPlayer(u8* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet || !sys || !*(void**)(sys + 0x1380)) return 1;
    if (firstTime) {
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(sys + 0x1380);
    if (target) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 1, *(f32*)(packet + 0x134));
    }
    return 0;
}

extern f32 lbl_8019BE7C[];
s32 Action_RetreatFromOpponent(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet) return 1;
    if (firstTime) {
        *(f32*)(packet + 0x134) = lbl_8019BE7C[0];
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(packet + 0x64);
    if (target && *(u8**)(target + 4)) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 2, *(f32*)(packet + 0x134));
    }
    return 0;
}

extern f32 lbl_8019BE84[];
s32 Action_RetreatFromNearestOpponent(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet) return 1;
    if (firstTime) {
        *(f32*)(packet + 0x134) = lbl_8019BE84[0];
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(packet + 0x54);
    if (target) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 2, *(f32*)(packet + 0x134));
    }
    return 0;
}

extern char str_ScriptParam_Face[];
s32 Action_MoveAwayFromOpponent(u8* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet) return 1;
    if (firstTime) {
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            if (!NuStrICmp(params[i], str_ScriptParam_Face)) packet[0xb2] = 1;
            else *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(packet + 0x64);
    if (target && *(u8**)(target + 4)) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 2, *(f32*)(packet + 0x134));
        if (packet[0xb2]) *(void**)(packet + 0x19c) = *(u8**)(packet + 0x64) + 0x6c;
    }
    return 0;
}

s32 Action_MoveAwayFromPlayer(u8* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet) return 1;
    if (firstTime) {
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            if (!NuStrICmp(params[i], str_ScriptParam_Face)) packet[0xb2] = 1;
            else *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(sys + 0x1380);
    if (target) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 2, *(f32*)(packet + 0x134));
    }
    return 0;
}

struct aisysview_s {
    u8 pad0[0x14];
    struct aicreatureview_s *creatures;
};

struct aicircleflags_s {
    u32 pad0 : 9;
    u32 direction : 1;
    u32 pad10 : 22;
};

extern char str_ScriptDir_AntiClockwise_2[], str_ScriptDir_Clockwise_2[], str_ScriptDir_Reverse_2[];
extern char str_ScriptParam_Facing[];
s32 Action_CircleOpponent(u8* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet) return 1;
    if (firstTime) {
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            if (!NuStrICmp(params[i], str_ScriptDir_AntiClockwise_2)) *(u32*)(packet + 0x1a0) &= ~0x400000;
            else if (!NuStrICmp(params[i], str_ScriptDir_Clockwise_2)) *(u32*)(packet + 0x1a0) |= 0x400000;
            else if (!NuStrICmp(params[i], str_ScriptDir_Reverse_2)) {
                ((struct aicircleflags_s*)(packet + 0x1a0))->direction = !((struct aicircleflags_s*)(packet + 0x1a0))->direction;
            }
            else if (!NuStrICmp(params[i], str_ScriptParam_Facing)) packet[0xb2] = 1;
            else *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(packet + 0x64);
    if (target && *(u8**)(target + 4)) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 3, *(f32*)(packet + 0x134));
        if (packet[0xb2]) *(void**)(packet + 0x19c) = *(u8**)(packet + 0x64) + 0x6c;
    }
    return 0;
}

s32 Action_CirclePlayer(u8* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s32 i;
    u8* target;
    if (!packet || !sys || !*(void**)(sys + 0x1380)) return 1;
    if (firstTime) {
        for (i = 0; i < count; i++) {
            if (lbl_80408D2C && lbl_80408D2C(params[i], packet + 0xb0)) continue;
            if (!NuStrICmp(params[i], str_ScriptDir_AntiClockwise_2)) *(u32*)(packet + 0x1a0) &= ~0x400000;
            else if (!NuStrICmp(params[i], str_ScriptDir_Clockwise_2)) *(u32*)(packet + 0x1a0) |= 0x400000;
            else if (!NuStrICmp(params[i], str_ScriptDir_Reverse_2)) {
                ((struct aicircleflags_s*)(packet + 0x1a0))->direction = !((struct aicircleflags_s*)(packet + 0x1a0))->direction;
            }
            else if (!NuStrICmp(params[i], str_ScriptParam_Facing)) packet[0xb2] = 1;
            else *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
        }
    }
    target = *(u8**)(sys + 0x1380);
    if (target) {
        struct aimoveview_s* ai = *(struct aimoveview_s**)(target + 4);
        AIMoveInstruction(packet, &ai->field124, ai->field9c, &ai->field10c, 3, *(f32*)(packet + 0x134));
        if (packet[0xb2]) *(void**)(packet + 0x19c) = *(u8**)(packet + 0x64) + 0x6c;
    }
    return 0;
}

extern char str_ScriptToken_Default[];
s32 Action_SetViewDistance(struct aisysview_s* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    u8* object;
    if (packet && (object = *(u8**)(packet + 0x50)) && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110) && firstTime) {
        if (packet[0xa6] != 0xff) {
            struct aicreatureview_s* creature = sys->creatures + packet[0xa6];
            *(f32*)(object + 0x234) = creature->field98;
        } else if (lbl_80408D3C) {
            *(f32*)(*(u8**)(packet + 0x50) + 0x234) = ((f32 (*)(s32))lbl_80408D3C)(**(s16**)(object + 0x60));
        } else {
            *(f32*)(object + 0x234) = 1.0f;
        }
        if (count && NuStrICmp(params[0], str_ScriptToken_Default))
            *(f32*)(*(u8**)(packet + 0x50) + 0x234) = aiParamFloat(processor, params[0]);
    }
    return 1;
}

s32 Action_SetMinViewHeight(struct aisysview_s* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    u8* object;
    if (packet && (object = *(u8**)(packet + 0x50)) && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110) && firstTime) {
        if (packet[0xa6] != 0xff) {
            struct aicreatureview_s* creature = sys->creatures + packet[0xa6];
            *(f32*)(object + 0x240) = creature->fielda4;
        } else if (lbl_80408D48) {
            *(f32*)(*(u8**)(packet + 0x50) + 0x240) = ((f32 (*)(s32))lbl_80408D48)(**(s16**)(object + 0x60));
        } else {
            *(f32*)(object + 0x240) = 1.0f;
        }
        if (count && NuStrICmp(params[0], str_ScriptToken_Default))
            *(f32*)(*(u8**)(packet + 0x50) + 0x240) = aiParamFloat(processor, params[0]);
    }
    return 1;
}

s32 Action_SetMaxViewHeight(struct aisysview_s* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    u8* object;
    if (packet && (object = *(u8**)(packet + 0x50)) && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110) && firstTime) {
        if (packet[0xa6] != 0xff) {
            struct aicreatureview_s* creature = sys->creatures + packet[0xa6];
            *(f32*)(object + 0x23c) = creature->fielda0;
        } else if (lbl_80408D44) {
            *(f32*)(*(u8**)(packet + 0x50) + 0x23c) = ((f32 (*)(s32))lbl_80408D44)(**(s16**)(object + 0x60));
        } else {
            *(f32*)(object + 0x23c) = 1.0f;
        }
        if (count && NuStrICmp(params[0], str_ScriptToken_Default))
            *(f32*)(*(u8**)(packet + 0x50) + 0x23c) = aiParamFloat(processor, params[0]);
    }
    return 1;
}

s32 Action_SetHearDistance(struct aisysview_s* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    u8* object;
    if (packet && (object = *(u8**)(packet + 0x50)) && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110) && firstTime) {
        if (packet[0xa6] != 0xff) {
            struct aicreatureview_s* creature = sys->creatures + packet[0xa6];
            *(f32*)(object + 0x238) = creature->field9c;
        } else if (lbl_80408D40) {
            *(f32*)(*(u8**)(packet + 0x50) + 0x238) = ((f32 (*)(s32))lbl_80408D40)(**(s16**)(object + 0x60));
        } else {
            *(f32*)(object + 0x238) = 1.0f;
        }
        if (count && NuStrICmp(params[0], str_ScriptToken_Default))
            *(f32*)(*(u8**)(packet + 0x50) + 0x238) = aiParamFloat(processor, params[0]);
    }
    return 1;
}

extern char lbl_8019BFBC[], lbl_8019BFC8[], lbl_8019BFD4[];
extern f32 lbl_8019BFDC[];
s32 Action_SetInterrupt(void* sys, struct aiparamproc_s* processor, void* packet, char** params, s32 count, s32 firstTime)
{
    f32 time = 0.0f;
    u8 priority = 0;
    u8 id = 0;
    char* state = 0;
    s32 i;
    if (firstTime && processor) {
        for (i = 0; i < count; i++) {
            char* found = NuStrIStr(params[i], lbl_8019BFBC);
            if (found) { found += 9; priority = (s32)aiParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i], lbl_8019BFC8))) { found += 3; id = (s32)aiParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i], lbl_8019BFCC))) state = found + 6;
            else if ((found = NuStrIStr(params[i], lbl_8019BFD4))) { found += 5; time = aiParamFloat(processor,found); }
        }
        if (state) fn_8008F35C(processor,priority,id,state,time);
    }
    return 1;
}

extern char lbl_8019BF6C[], lbl_8019BF78[], lbl_8019BF80[];
extern u8* lbl_80408DF0;
extern s16 FindAnimIX(void*,char*);
s32 Action_OverrideAnimation(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    s16 a = -1;
    s16 b = -1;
    s32 i;
    u8* object;
    if (packet && (object = *(u8**)(packet + 0x50)) && *(void**)object && firstTime) {
        for (i = 0; i < count; i++) {
            char* found;
            if (!NuStrICmp(params[i], lbl_8019BF6C)) a = *(s16*)(lbl_80408DF0 + 0xa);
            else if ((found = NuStrIStr(params[i], lbl_8019BF78))) a = FindAnimIX(*(void**)(*(u8**)(packet + 0x50) + 0x64), found + 5);
            else if ((found = NuStrIStr(params[i], lbl_8019BF80))) b = FindAnimIX(*(void**)(*(u8**)(packet + 0x50) + 0x64), found + 3);
            else processor->actionTimer = aiParamFloat(processor,params[0]);
        }
        if (b == -1) a = -1;
        *(s16*)(packet + 0xa2) = a;
        *(s16*)(packet + 0xa4) = b;
    }
    return 1;
}

extern char str_ScriptParam_MinTime[], str_ScriptParam_MaxTime[];
extern f32 lbl_8019BE2C[];
s32 Action_Idle(void* sys, struct aiparamproc_s* processor, void* packet, char** params, s32 count, s32 firstTime, f32 elapsed)
{
    f32 min = 0.0f;
    f32 max = min;
    s32 i;
    if (firstTime) {
        for (i = 0; i < count; i++) {
            char* found = NuStrIStr(params[i],str_ScriptParam_MinTime);
            if (found) { found += 8; min = aiParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i],str_ScriptParam_MaxTime))) { found += 8; max = aiParamFloat(processor,found); }
            else processor->actionTimer = aiParamFloat(processor,params[0]);
        }
        if (processor->actionTimer == 0.0f) {
            if (min != 0.0f && max != 0.0f) processor->actionTimer = fn_8000C038() * (max - min) + min;
            else processor->actionTimer = lbl_8019BE2C[0];
        }
    } else if (processor->actionTimer > min) {
        processor->actionTimer -= elapsed;
        if (processor->actionTimer <= min) {
            processor->actionTimer = min;
            return 1;
        }
    }
    return 0;
}

struct aipathnodeview_s {
    u8 pad0[4];
    struct aimovevec_s field4;
    u8 pad10[4];
    f32 field14;
    u8 pad18[0x10];
    u8 count;
    u8 pad29[7];
    u8** connections;
    u8 pad34[0x24];
};

extern f32 fn_8000CDFC(void*,void*,void*);
s32 Action_GoToNodeRandom(void* sys, void* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    struct aimovevec_s out;
    struct aipathnodeview_s* node;
    if (packet && *(void**)(packet + 0x50) && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110)) {
        if (firstTime) {
            if (count) {
                struct aipathnodeview_s* selected;
                s32 index = (s32)((f32)count * fn_8000C038());
                if (index >= count) index = count - 1;
                selected = AIPathFindNode(sys,*(void**)(packet + 0x10c),params[index]);
                *(void**)(packet + 0xb4) = selected;
                node = selected;
                if (node && node->count) {
                    u8* path = *(u8**)(packet + 0x10c);
                    u8* connection;
                    s32 nodeIndex = node - *(struct aipathnodeview_s**)(path + 0x78);
                    *(u8**)(packet + 0xcc) = path;
                    connection = node->connections[0];
                    *(u8**)(packet + 0xd0) = connection;
                    if (connection[0] == nodeIndex) *(f32*)(packet + 0xdc) = 0.0f;
                    else *(f32*)(packet + 0xdc) = 1.0f;
                    packet[0xd4] = 0;
                    *(u32*)(packet + 0xd8) |= 0x8000;
                    *(f32*)(packet + 0xe0) = 0.0f;
                    AIMoveInstruction(packet,&selected->field4,0.0f,packet + 0xcc,1,*(f32*)(packet + 0x134));
                } else return 1;
            }
        } else {
            node = *(struct aipathnodeview_s**)(packet + 0xb4);
            if (node) {
                f32 distance = fn_8000CDFC(packet + 0x100,&node->field4,&out);
                AIMoveInstruction(packet,&node->field4,0.0f,packet + 0xcc,1,*(f32*)(packet + 0x134));
                if (distance < node->field14) return 1;
            } else return 1;
        }
        goto finished;
    }
    return 1;
finished:
    return 0;
}

extern char lbl_8019BFA8[];
extern f32 lbl_8019BFB0[];
s32 Action_PathConnectionMaxLength(u8* sys, struct aiparamproc_s* processor, void* packet, char** params, s32 count, s32 firstTime)
{
    s32 direction = 0;
    char* from = 0;
    f32 length = 0.0f;
    char* to = 0;
    s32 i;
    if (sys && firstTime) {
        for (i = 0; i < count; i++) {
            char* found = NuStrIStr(params[i], lbl_8019BF88);
            if (found) from = found + 5;
            else if ((found = NuStrIStr(params[i], lbl_8019BF90))) to = found + 3;
            else if ((found = NuStrIStr(params[i], lbl_8019BFA8))) { found += 7; length = aiParamFloat(processor,found); }
        }
        if (from && to) {
            u8* connection = fn_8008A6D4(sys, *(void**)(*(u8**)(sys + 0xc) + 8), from,to,&direction);
            if (connection) {
                *(f32*)(connection + 0x24) = length;
                if (length != 0.0f && *(f32*)(connection + 0x1c) > length + *(f32*)(*(u8**)(*(u8**)(*(u8**)(sys + 0xc) + 8) + 0x78) + 0x10) + *(f32*)(*(u8**)(*(u8**)(*(u8**)(sys + 0xc) + 8) + 0x78) + 0x10)) {
                    *(u32*)(connection + 4) |= 0x08000000;
                    *(u32*)(connection + 8) |= 0x08000000;
                } else {
                    *(u32*)(connection + 4) &= ~0x08000000;
                    *(u32*)(connection + 8) &= ~0x08000000;
                }
            }
        }
    }
    return 1;
}

s32 Action_GoToNode(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    struct aimovevec_s out;
    struct aipathnodeview_s* node;
    if (packet && *(void**)(packet + 0x50) && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110)) {
        if (firstTime) {
            if (count) {
                struct aipathnodeview_s* selected;
                s32 i;
                for (i = 1; i < count; i++) {
                    if (!lbl_80408D2C || !lbl_80408D2C(params[i], packet + 0xb0))
                        *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
                }
                selected = AIPathFindNode(sys,*(void**)(packet + 0x10c),params[0]);
                *(void**)(packet + 0xb4) = selected;
                node = selected;
                if (node && node->count) {
                    u8* path = *(u8**)(packet + 0x10c);
                    u8* connection;
                    s32 nodeIndex = node - *(struct aipathnodeview_s**)(path + 0x78);
                    *(u8**)(packet + 0xcc) = path;
                    connection = node->connections[0];
                    *(u8**)(packet + 0xd0) = connection;
                    if (connection[0] == nodeIndex) *(f32*)(packet + 0xdc) = 0.0f;
                    else *(f32*)(packet + 0xdc) = 1.0f;
                    packet[0xd4] = 0;
                    *(u32*)(packet + 0xd8) |= 0x8000;
                    *(f32*)(packet + 0xe0) = 0.0f;
                    AIMoveInstruction(packet,&selected->field4,0.0f,packet + 0xcc,1,*(f32*)(packet + 0x134));
                } else return 1;
            }
        } else {
            node = *(struct aipathnodeview_s**)(packet + 0xb4);
            if (node) {
                f32 distance = fn_8000CDFC(packet + 0x100,&node->field4,&out);
                AIMoveInstruction(packet,&node->field4,0.0f,packet + 0xcc,1,*(f32*)(packet + 0x134));
                if (distance < node->field14) return 1;
            } else return 1;
        }
        goto finished;
    }
    return 1;
finished:
    return 0;
}

extern void AISysCharacterSetPath(void*,void*);
s32 Action_MoveAwayFromNode(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    struct aimovevec_s out;
    struct aipathnodeview_s* node;
    if (packet && *(void**)(packet + 0x50) && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110)) {
        if (firstTime) {
            if (count) {
                struct aipathnodeview_s* selected;
                s32 i;
                for (i = 1; i < count; i++) {
                    if (!lbl_80408D2C || !lbl_80408D2C(params[i], packet + 0xb0))
                        *(f32*)(packet + 0x134) = aiParamFloat(processor, params[i]);
                }
                selected = AIPathFindNode(sys,*(void**)(packet + 0x10c),params[0]);
                *(void**)(packet + 0xb4) = selected;
                node = selected;
                if (node && node->count) {
                    u8* path = *(u8**)(packet + 0x10c);
                    u8* connection;
                    s32 nodeIndex = node - *(struct aipathnodeview_s**)(path + 0x78);
                    AISysCharacterSetPath(packet,path);
                    connection = node->connections[0];
                    *(u8**)(packet + 0xd0) = connection;
                    if (connection[0] == nodeIndex) *(f32*)(packet + 0xdc) = 0.0f;
                    else *(f32*)(packet + 0xdc) = 1.0f;
                    packet[0xd4] = 0;
                    *(u32*)(packet + 0xd8) |= 0x8000;
                    *(f32*)(packet + 0xe0) = 0.0f;
                    AIMoveInstruction(packet,&selected->field4,0.0f,packet + 0xcc,2,*(f32*)(packet + 0x134));
                } else return 1;
            }
        } else {
            node = *(struct aipathnodeview_s**)(packet + 0xb4);
            if (node) {
                f32 distance = fn_8000CDFC(packet + 0x100,&node->field4,&out);
                if (distance < node->field14) return 1;
                AIMoveInstruction(packet,&node->field4,0.0f,packet + 0xcc,2,*(f32*)(packet + 0x134));
            } else return 1;
        }
        goto finished;
    }
    return 1;
finished:
    return 0;
}

extern f32 lbl_8019BEE0[];
extern void NuVecRotateY(void*,void*,s32);
extern void NuVecAdd(void*,void*,void*);
s32 Action_GoToOrigin(struct aisysview_s* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    struct aimovevec_s out;
    struct aicreatureview_s* creature;
    u8* object;
    s32 i;
    if (!packet || !(object = *(u8**)(packet + 0x50)) || !*(void**)(packet + 0x10c) || !*(void**)(packet + 0x110) || !(*(u32*)(object + 0x208) & 0x400) || packet[0xa6] == 0xff) return 1;
    creature = sys->creatures + packet[0xa6];
    if (firstTime) {
        *(f32*)(packet + 0x134) = lbl_8019BEE0[0];
        if (count) {
            for (i = 0; i < count; i++)
                if (!lbl_80408D2C || !lbl_80408D2C(params[i],packet + 0xb0))
                    *(f32*)(packet + 0x134) = aiParamFloat(processor,params[i]);
        }
        AIMoveInstruction(packet,(u8*)creature + 0x30,0.0f,(u8*)creature + 0x40,1,*(f32*)(packet + 0x134));
        *(f32*)(packet + 0xc0) = 0.0f;
        *(f32*)(packet + 0xc4) = 0.0f;
        *(f32*)(packet + 0xc8) = 1.0f;
        NuVecRotateY(packet + 0xc0,packet + 0xc0,*(s32*)((u8*)creature + 0x3c));
        NuVecAdd(packet + 0xc0,packet + 0xc0,(u8*)creature + 0x30);
    } else {
        f32 distance;
        AIMoveInstruction(packet,(u8*)creature + 0x30,0.0f,(u8*)creature + 0x40,1,*(f32*)(packet + 0x134));
        distance = fn_8000CDFC(packet + 0x100,(u8*)creature + 0x30,&out);
        if (distance < *(f32*)(packet + 0x134) * *(f32*)(packet + 0x134)) {
            *(void**)(packet + 0x19c) = packet + 0xc0;
            return 1;
        }
    }
    return 0;
}

extern char lbl_8019BEEC[], lbl_8019BEF4[], lbl_8019BF00[], lbl_8019BF08[], lbl_8019BF10[], lbl_8019BF18[];
extern s32 NuRand(s32);
s32 Action_SetLocator(void* sys, void* processor, u8* packet, char** params, s32 count, s32 firstTime)
{
    char filename[64];
    s32 a = 0;
    s32 b = 0;
    s32 n = 0;
    char* name = 0;
    u8* object;
    s32 i;
    if (packet && (object = *(u8**)(packet + 0x50)) && *(void**)object && *(void**)(packet + 0x10c) && *(void**)(packet + 0x110) && firstTime) {
        *(void**)(packet + 0xfc) = 0;
        if (count) {
            for (i = 0; i < count; i++) {
                char* found = NuStrIStr(params[i],lbl_8019BEEC);
                if (found) name = found + 5;
                else if (NuStrIStr(params[i],lbl_8019BEF4)) a = 1;
                else if (NuStrIStr(params[i],lbl_8019BF00)) b = 1;
                else if ((found = NuStrIStr(params[i],lbl_8019BF08))) { found += 9; n = (s32)aiParamFloat((struct aiparamproc_s*)packet,found); }
            }
            if (name) {
                if (b && *(s8*)(*(u8**)(packet + 0x50) + 0x284) != -1)
                    sprintf(filename,lbl_8019BF10,name,*(s8*)(*(u8**)(packet + 0x50) + 0x284));
                else if (a && *(void**)(*(u8**)(packet + 0x50) + 0x64))
                    sprintf(filename,lbl_8019BF18,name,*(s32*)(*(u8**)(*(u8**)(packet + 0x50) + 0x64) + 0xc));
                else if (n) sprintf(filename,lbl_8019BF10,name,NuRand(0) % n);
                else sprintf(filename,name);
                *(void**)(packet + 0xfc) = AIPathFindLocator(sys,filename);
            }
        }
    }
    return 1;
}

extern char str_ScriptPath_ScriptTxtFormat[], str_ScriptPath_ScpFormat[];
extern u32 lbl_8040AF30;
extern u8* NuFParCreate(char*);
extern void NuFParGetWord(void*);
extern s32 fn_80006398(void*);
extern void NuMemAllocFn(void*);
extern u8* AIScriptLoadScp(char*,char*,void*,void*);
extern void fn_80007610(void*,void*);
extern s32 fn_8007E064(void*,void*,void*,void*);
extern s32 sprintf(char*,char*,...);
void AIScriptLoadScriptTxt(char* path, void* arg1, void* arg2, u8* sys)
{
    char names[128][32];
    char filename[128];
    s32 n = 0;
    void* list;
    u8* parser;
    u8* script;
    s32 i;
    s32 done;
    list = sys ? sys + 0x18 : &lbl_8040AF30;
    sprintf(filename, str_ScriptPath_ScriptTxtFormat, path);
    parser = NuFParCreate(filename);
    if (parser) {
        while (fn_80006398(parser)) {
            NuFParGetWord(parser);
            sprintf(names[n++], str_ScriptPath_ScpFormat, parser + 0x30a);
        }
        NuMemAllocFn(parser);
    }
    for (i = 0; i < n; i++) {
        script = AIScriptLoadScp(names[i], path, arg1, arg2);
        if (script) {
            fn_80007610(list, script);
            if (sys) *(u32*)(script + 0x3c) |= 0x80000000;
        }
    }
    if (sys) {
        void* lastList = sys + 0x18;
        do {
            done = 1;
            for (script = fn_800076C4(lastList); script; script = fn_800076CC(lastList, script))
                if (!fn_8007E064(script, arg1, arg2, sys)) done = 0;
        } while (!done);
    }
}

struct aiconnectionview_s { u8 nodes[2]; u8 pad2[2]; u32 flags[2]; };
extern char lbl_8019BF94[], lbl_8019BFA0[];
static inline void aiBlockConnection(u8* path, char* from, char* to, s32 blocked)
{
    s32 direction = 0;
    u8* connection = 0;
    if (path && from && to) {
        struct aipathnodeview_s* first = AIPathFindNode(0,path,from);
        struct aipathnodeview_s* last = AIPathFindNode(0,path,to);
        if (first && last) {
            s32 index = last - *(struct aipathnodeview_s**)(path + 0x78);
            s32 i;
            for (i = 0; i < first->count; i++) {
                if (first->connections[i][0] == index) { connection = first->connections[i]; direction = 1; break; }
                if (first->connections[i][1] == index) { connection = first->connections[i]; direction = 0; break; }
            }
            if (connection) {
                if (blocked) ((struct aiconnectionview_s*)connection)->flags[direction] |= 0x80000000;
                else ((struct aiconnectionview_s*)connection)->flags[direction] &= 0x7fffffff;
            }
        }
    }
}
s32 Action_BlockPath(u8* sys, void* processor, void* packet, char** params, s32 count, s32 firstTime)
{
    s32 blocked = 1;
    s32 both = 0;
    char* from = 0;
    char* to = 0;
    s32 i;
    u8* pathsys;
    if (sys && (pathsys = *(u8**)(sys + 0xc)) && pathsys[0] && firstTime) {
        for (i = 0; i < count; i++) {
            char* found = NuStrIStr(params[i],lbl_8019BF88);
            if (found) from = found + 5;
            else if ((found = NuStrIStr(params[i],lbl_8019BF90))) to = found + 3;
            else if (!NuStrICmp(params[i],lbl_8019BF94)) both = 1;
            else if (!NuStrICmp(params[i],lbl_8019BFA0)) blocked = 0;
        }
        if (from && to) {
            aiBlockConnection(*(u8**)(*(u8**)(sys + 0xc) + 8),from,to,blocked);
            if (both) aiBlockConnection(*(u8**)(*(u8**)(sys + 0xc) + 8),to,from,blocked);
        }
    }
    return 1;
}

struct aifaceobj_s { u8 pad0[4]; void* field4; u8 pad8[0x64]; struct aimovevec_s position; };
extern void NuVecNorm(void*,void*);
extern char str_ScriptParam_FaceOffset[];
extern f32 lbl_8019BE9C[];
s32 Action_FaceOpponent(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime, f32 elapsed)
{
    f32 min = 0.0f;
    f32 max = min;
    s32 i;
    if (!packet) return 1;
    if (firstTime) {
        packet[0xb2] = 0;
        for (i = 0; i < count; i++) {
            char* found;
            found = NuStrIStr(params[i],str_ScriptParam_MinTime);
            if (found) { found += 8; min = aiParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i],str_ScriptParam_MaxTime))) { found += 8; max = aiParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i],str_ScriptParam_FaceOffset))) { found += 11; *(f32*)(packet + 0xb8) = aiParamFloat(processor,found); }
            else processor->actionTimer = aiParamFloat(processor,params[i]);
        }
        if (processor->actionTimer == 0.0f) {
            if (min != 0.0f && max != 0.0f) processor->actionTimer = fn_8000C038() * (max - min) + min;
            else processor->actionTimer = lbl_8019BE9C[0];
        }
    }
    {
        struct aifaceobj_s* opponent = *(struct aifaceobj_s**)(packet + 0x64);
        if (opponent && opponent->field4) {
            if (*(f32*)(packet + 0xb8) == 0.0f) *(void**)(packet + 0x19c) = &opponent->position;
            else {
                struct aimovevec_s direction;
                struct aifaceobj_s* object = *(struct aifaceobj_s**)(packet + 0x50);
                direction.x = opponent->position.z - object->position.z;
                {
                    f32 objectX = object->position.x;
                    f32 opponentX = opponent->position.x;
                    direction.y = 0.0f;
                    direction.z = objectX - opponentX;
                }
                NuVecNorm(&direction,&direction);
                {
                    struct aifaceobj_s* target = *(struct aifaceobj_s**)(packet + 0x64);
                    *(f32*)(packet + 0xc0) = direction.x * *(f32*)(packet + 0xb8) + target->position.x;
                    *(f32*)(packet + 0xc4) = target->position.y;
                    *(f32*)(packet + 0xc8) = direction.z * *(f32*)(packet + 0xb8) + target->position.z;
                    *(void**)(packet + 0x19c) = packet + 0xc0;
                }
            }
        }
    }
    if (processor->actionTimer > 0.0f) {
        processor->actionTimer -= elapsed;
        if (processor->actionTimer <= 0.0f) {
            processor->actionTimer = 0.0f;
            return 1;
        }
    }
    return 0;
}

extern char lbl_8019BF24[], lbl_8019BF30[], lbl_8019BF3C[], lbl_8019BF48[];
extern f32 lbl_8019BF58[], lbl_8019BF5C[], lbl_8019BF60[], lbl_80408CFC;
static inline f32 aiLocatorParamFloat(struct aiparamproc_s *processor, char *param)
{
    char *cursor;
    s32 i;
    if (param != NULL) {
        if (processor != NULL) {
            cursor = NuStrIStr(param, str_ScriptToken_ParamLower);
            if (cursor != NULL) {
                i = NuAToI(cursor);
                if ((u32)i < 4) return processor->script->params[i].value;
            } else if (processor->script != NULL) {
                for (i = 0; i < 4; i++) {
                    if (processor->script->params[i].name != NULL &&
                        strcasecmp(processor->script->params[i].name, param) == 0) {
                        return processor->script->params[i].value;
                    }
                }
            }
        }
        return fn_800077E8(param);
    }
    return lbl_8019BF58[0];
}
s32 Action_GoToLocator(void* sys, struct aiparamproc_s* processor, u8* packet, char** params, s32 count, s32 firstTime, f32 elapsed)
{
    struct aimovevec_s difference;
    char filename[64];
    f32 min = lbl_8019BF58[0];
    s32 personal = 0, indexed = 0, random = 0;
    f32 max = min;
    s32 i;
    u8* object;
    u8* locator;
    if (!packet || !(object = *(u8**)(packet + 0x50)) || !*(void**)object || !*(void**)(packet + 0x10c) || !*(void**)(packet + 0x110)) goto complete;
    if (firstTime) {
        *(void**)(packet + 0xb4) = *(void**)(packet + 0xfc);
        if (count) for (i = 0; i < count; i++) {
            char* found;
            if (lbl_80408D2C && lbl_80408D2C(params[i],packet + 0xb0)) continue;
            if (NuStrIStr(params[i],lbl_8019BEEC) || NuStrIStr(params[i],lbl_8019BF24)) {
                i++;
                if (i < count) {
                    if (indexed && *(s8*)(*(u8**)(packet + 0x50) + 0x284) != -1)
                        sprintf(filename,lbl_8019BF10,params[i],*(s8*)(*(u8**)(packet + 0x50) + 0x284));
                    else if (personal && *(void**)(*(u8**)(packet + 0x50) + 0x64))
                        sprintf(filename,lbl_8019BF18,params[i],*(s32*)(*(u8**)(*(u8**)(packet + 0x50) + 0x64) + 0xc));
                    else if (random) { s32 value = NuRand(0); sprintf(filename,lbl_8019BF10,params[i],value % random); }
                    else sprintf(filename,params[i]);
                    *(void**)(packet + 0xb4) = AIPathFindLocator(sys,filename);
                }
                if (NuStrIStr(params[i-1],lbl_8019BF24)) {
                    locator = *(u8**)(packet + 0xb4);
                    if (locator) *(struct aimovevec_s*)(*(u8**)(packet + 0x50) + 0x6c) = *(struct aimovevec_s*)(locator + 0x10);
                    goto complete;
                }
            } else if (NuStrIStr(params[i],lbl_8019BEF4)) personal = 1;
            else if (NuStrIStr(params[i],lbl_8019BF00)) indexed = 1;
            else if ((found = NuStrIStr(params[i],lbl_8019BF08))) { found += 9; random = (s32)aiLocatorParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i],lbl_8019BF30))) { found += 9; processor->actionTimer = aiLocatorParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i],str_ScriptParam_MinTime))) { found += 8; min = aiLocatorParamFloat(processor,found); }
            else if ((found = NuStrIStr(params[i],str_ScriptParam_MaxTime))) { found += 8; max = aiLocatorParamFloat(processor,found); }
            else if (!NuStrICmp(params[i],lbl_8019BF3C)) packet[0xb2] = 1;
            else if (!NuStrICmp(params[i],lbl_8019BF48)) packet[0xb3] = 1;
            else *(f32*)(packet + 0x134) = aiLocatorParamFloat(processor,params[i]);
        }
        if (processor->actionTimer == lbl_8019BF58[0]) {
            if (min != lbl_8019BF58[0] && max != lbl_8019BF58[0]) processor->actionTimer = fn_8000C038() * (max-min) + min;
            else processor->actionTimer = lbl_8019BF5C[0];
        }
        locator = *(u8**)(packet + 0xb4);
        if (!locator) goto complete;
        AIMoveInstruction(packet,locator + 0x10,lbl_8019BF58[0],locator + 0x20,1,*(f32*)(packet + 0x134));
        {
            f32 zero = lbl_8019BF58[0];
            f32 one = lbl_8019BF60[0];
            struct ailocatorpacket_s { u8 pad[0xc0]; struct aimovevec_s facing; };
            struct ailocatorpacket_s* view = (struct ailocatorpacket_s*)packet;
            view->facing.x = zero; view->facing.z = one; view->facing.y = zero;
        }
        NuVecRotateY(packet + 0xc0,packet + 0xc0,*(s32*)(locator + 0x1c));
        NuVecAdd(packet + 0xc0,packet + 0xc0,locator + 0x10);
    } else {
        f32 distance, radius;
        s32 near;
        locator = *(u8**)(packet + 0xb4);
        if (!locator) goto complete;
        near = 0;
        AIMoveInstruction(packet,locator + 0x10,min,locator + 0x20,1,*(f32*)(packet + 0x134));
        if (packet[0xb2] && !(*(u8**)(packet + 0x50))[0x285]) goto running;
        if (packet[0xb3]) distance = fn_8000CDFC(packet + 0x100,locator + 0x10,&difference);
        else distance = fn_8000CD08(packet + 0x100,locator + 0x10,&difference);
        radius = *(f32*)(packet + 0x134) + lbl_80408CFC;
        if (distance < radius * radius) near = 1;
        if (near) {
            f32 zero;
            f32 time;
            *(void**)(packet + 0x19c) = packet + 0xc0;
            zero = lbl_8019BF58[0];
            time = processor->actionTimer;
            if (time > zero) {
                time -= elapsed;
                processor->actionTimer = time;
                if (time < zero) processor->actionTimer = zero;
            } else goto complete;
        }
    }
    goto running;
complete:
    return 1;
running:
    return 0;
}

extern s32 fn_8000ABDC(f32, f32);
extern f32 lbl_8019C628[], lbl_8019C62C[];
f32 Condition_YawToOpponent(void *sys, void *process, u8 *packet)
{
    f32 relative[3];
    u8 *opponent;
    u8 *owner;
    if (packet && (opponent = *(u8 **)(packet + 0x64))) {
        owner = *(u8 **)(packet + 0x50);
        relative[0] = *(f32 *)(opponent + 0x6c) - *(f32 *)(owner + 0x6c);
        relative[2] = *(f32 *)(opponent + 0x74) - *(f32 *)(owner + 0x74);
        NuVecRotateY(relative, relative, -*(u16 *)(owner + 0x27e));
        return (f32)fn_8000ABDC(relative[0], relative[2]) * lbl_8019C628[0];
    }
    return lbl_8019C62C[0];
}

static inline void *aiFindState(char *name, u8 *script)
{
    u8 *entry = fn_800076C4(script + 0x10);
    if (!name) return 0;
    for (; entry; entry = fn_800076CC(script + 0x10, entry))
        if (!NuStrICmp(name, *(char **)(entry + 0x18))) return entry;
    return 0;
}
s32 Action_SetState(void *sys, u8 *process, void *packet, char **params, s32 count, s32 firstTime)
{
    if (firstTime && count)
        *(void **)(process + 0x10) = aiFindState(params[0], *(u8 **)(process + 4));
    return 0;
}

s32 Action_SetMoveRadius(void *sys, struct aiparamproc_s *process, u8 *packet, char **params, s32 count, s32 firstTime)
{
    u8 *owner;
    if (packet && (owner = *(u8 **)(packet + 0x50)) && *(void **)(packet + 0x10c) && *(void **)(packet + 0x110) && firstTime) {
        *(f32 *)(packet + 0x9c) = 2.0f * *(f32 *)(owner + 0xbc);
        if (count && NuStrICmp(params[0], str_ScriptToken_Default))
            *(f32 *)(packet + 0x9c) = aiParamFloat(process, params[0]);
    }
    return 1;
}

extern char lbl_8019BFE0[];
struct aiignorebits_s { u32 pad0:17; u32 value:1; u32 pad18:14; };
s32 Action_SetIgnoreAntinodes(void *sys, void *process, u8 *packet, char **params, s32 count, s32 firstTime)
{
    u8 *object = 0;
    s32 ignore = 1;
    s32 i;
    char *value;
    if (firstTime) {
        if (packet && *(u8 **)(packet + 0x50)) object = *(u8 **)(packet + 0x50);
        for (i = 0; i < count; i++) {
            value = NuStrIStr(params[i], lbl_8019BFE0);
            if (value) {
                value += 10;
                if (lbl_80408DA4) object = lbl_80408DA4(sys, value);
            } else if (NuStrICmp(lbl_8019BFA0, params[0]) == 0) ignore = 0;
        }
        if (object) ((struct aiignorebits_s *)(object + 0x20c))->value = ignore;
    }
    return 1;
}

extern f32 lbl_8019C684[];
static inline f32 aiParamFloatFace(struct aiparamproc_s *processor, char *param)
{
    char *cursor;
    s32 i;
    if (param != NULL) {
        if (processor != NULL) {
            cursor = NuStrIStr(param, str_ScriptToken_ParamLower);
            if (cursor != NULL) {
                i = NuAToI(cursor);
                if ((u32)i < 4) return processor->script->params[i].value;
            } else if (processor->script != NULL) {
                for (i = 0; i < 4; i++) {
                    if (processor->script->params[i].name != NULL &&
                        strcasecmp(processor->script->params[i].name, param) == 0) {
                        return processor->script->params[i].value;
                    }
                }
            }
        }
        return fn_800077E8(param);
    }
    return lbl_8019C684[0];
}
s32 Action_FacePlayer(u8 *sys, struct aiparamproc_s *process, u8 *packet, char **params, s32 count, s32 firstTime, f32 elapsed)
{
    u8 *player;
    f32 timer, zero;
    if (!packet) return 1;
    if (firstTime && count) process->actionTimer = aiParamFloatFace(process, params[0]);
    player = *(u8 **)(sys + 0x1380);
    if (player) *(void **)(packet + 0x19c) = player + 0x6c;
    timer = process->actionTimer;
    zero = lbl_8019C684[0];
    if (timer > zero) {
        timer -= elapsed;
        process->actionTimer = timer;
        if (timer <= zero) {
            process->actionTimer = zero;
            return 1;
        }
    }
    return 0;
}

struct aiturnbits_s { u32 pad0:16; u32 value:1; u32 pad17:15; };
s32 NuAngAdd(s32, s32);
s32 NuAngSub(s32, s32);
extern f32 lbl_80408D54;
extern f32 lbl_80408D58;


void AISysCheckAntinode_Circle(void *object, void *region, void *relative, f32 radius)
{
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f12;
    f32 temp_f13;
    f32 temp_f13_2;
    f32 temp_f1;
    f32 temp_f1_2;
    s32 startAngle;
    s32 endAngle;
    s32 angle;
    s32 var_r0;
    s32 maxTurn;
    s32 turn;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;

    temp_f0 = *(f32 *)((u8 *)relative + 8);
    temp_f13 = *(f32 *)((u8 *)relative + 0);
    if (((temp_f13 * temp_f13) + (temp_f0 * temp_f0)) < (radius * radius)) {
        temp_f1 = *(f32 *)((u8 *)object + 0x6c) - *(f32 *)((u8 *)region + 0);
        *(f32 *)((u8 *)relative + 0) = temp_f1;
        temp_f13_2 = *(f32 *)((u8 *)region + 8);
        temp_f0_2 = *(f32 *)((u8 *)object + 0x74) - temp_f13_2;
        *(f32 *)((u8 *)relative + 8) = temp_f0_2;
        startAngle = fn_8000ABDC(temp_f1, temp_f0_2);
        temp_f1_2 = (*(f32 *)((s8 *)((*(void **)((s8 *)(object) + (4)))) + (0x80))) - *(f32 *)((u8 *)region + 0);
        *(f32 *)((u8 *)relative + 0) = temp_f1_2;
        temp_f0_3 = (*(f32 *)((s8 *)((*(void **)((s8 *)(object) + (4)))) + (0x88))) - *(f32 *)((u8 *)region + 8);
        *(f32 *)((u8 *)relative + 8) = temp_f0_3;
        angle = fn_8000ABDC(temp_f1_2, temp_f0_3);
        turn = NuAngSub(angle, startAngle);
        temp_r11 = (*(void **)((s8 *)(object) + (4)));
        temp_f12 = *(f32 *)((u8 *)temp_r11 + 0x140);
        if (temp_f12 > 0.0f) {
            if ((((s32) (*(s32 *)((u8 *)temp_r11 + 0x118) & 0xC000) == 0x4000) || (*(f32 *)((u8 *)object + 0x1e8) > 1.0f)) && (temp_f12 < lbl_80408D54)) {
                ((struct aiturnbits_s *)((u8 *)temp_r11 + 0x1a0))->value = !((struct aiturnbits_s *)((u8 *)temp_r11 + 0x1a0))->value;
                (*(f32 *)((s8 *)((*(void **)((s8 *)(object) + (4)))) + (0x140))) = (f32) (lbl_80408D58 + lbl_80408D54);
            }
            if ((*(s32 *)((s8 *)((*(void **)((s8 *)(object) + (4)))) + (0x1A0))) & 0x8000) {
                if (turn < 0) {
                    turn += 0x10000;
                }
            } else if (turn > 0) {
                turn += 0xFFFF0000;
            }
        } else {
            if (turn > 0) {
                var_r0 = *(s32 *)((u8 *)temp_r11 + 0x1a0) | 0x8000;
            } else {
                var_r0 = *(s32 *)((u8 *)temp_r11 + 0x1a0) & 0xFFFF7FFF;
            }
            *(s32 *)((u8 *)temp_r11 + 0x1a0) = var_r0;
        }
        if (!(*(u8 *)((u8 *)region + 0x49) & 2)) {
            temp_r9 = (*(void **)((s8 *)(object) + (4)));
            if (*(f32 *)((u8 *)temp_r9 + 0x140) < lbl_80408D54) {
                *(f32 *)((u8 *)temp_r9 + 0x140) = (f32) lbl_80408D54;
            }
        }
        maxTurn = (s32) ((lbl_80408CFC / radius) * 10430.378f);
        if ((turn > maxTurn) || (maxTurn = -maxTurn, ((turn < maxTurn) != 0))) {
            turn = maxTurn;
        }
        endAngle = NuAngAdd(startAngle, turn);
        *(f32 *)((u8 *)relative + 0) = (f32) 0.0f;
        *(f32 *)((u8 *)relative + 4) = (f32) 0.0f;
        *(f32 *)((u8 *)relative + 8) = radius;
        NuVecRotateY(relative, relative, endAngle);
        (*(f32 *)((s8 *)((*(void **)((s8 *)(object) + (4)))) + (0x90))) = (f32) (*(f32 *)((u8 *)relative + 0) + *(f32 *)((u8 *)region + 0));
        temp_r9_2 = (*(void **)((s8 *)(object) + (4)));
        *(f32 *)((u8 *)temp_r9_2 + 0x94) = (f32) *(f32 *)((u8 *)temp_r9_2 + 0x84);
        (*(f32 *)((s8 *)((*(void **)((s8 *)(object) + (4)))) + (0x98))) = (f32) (*(f32 *)((u8 *)relative + 8) + *(f32 *)((u8 *)region + 8));
    }
}

struct aipointertable_s { u8 pad0[0xc]; void *entries[16]; };

void fn_800868B4(void *arg0, void *arg1)
{
    f32 temp_f0;
    f32 temp_f7;
    s32 temp_r9;
    s32 var_r7;
    u32 temp_r10;
    u8 temp_r11;
    u8 temp_r9_2;

    if ((arg0 != NULL) && (arg1 != NULL)) {
        temp_r11 = (*(u8 *)((s8 *)(arg0) + (7)));
        if (temp_r11 <= 0xFU) {
            temp_r10 = (*(u8 *)((s8 *)(arg0) + (8)));
            temp_r9 = temp_r11 + 1;
            var_r7 = temp_r9 / (s32) temp_r10;
            if (temp_r9 != (s32) (var_r7 * temp_r10)) {
                var_r7 += 1;
            }
            if (var_r7 <= 4) {
                if ((s32) temp_r11 == 0) {
                    temp_f7 = (*(f32 *)((s8 *)(arg0) + (0x128)));
                    (*(void **)((s8 *)(arg0) + (0))) = arg1;
                    if (temp_f7 > 0.0f) {
                        temp_f0 = temp_f7 * ((f32) (f64) temp_r10 - 1.0f) * 0.5f;
                        (*(f32 *)((s8 *)(arg0) + (0x124))) = temp_f0;
                        (*(s16 *)((s8 *)(arg0) + (4))) = (s16) (((*(f32 *)((s8 *)(arg0) + (0x130))) / (temp_f0 * 6e+01f)) * 10430.378f);
                    } else {
                        (*(f32 *)((s8 *)(arg0) + (0x124))) = (f32) 0.0f;
                        (*(s16 *)((s8 *)(arg0) + (4))) = 0x64;
                    }
                }
                (*(s8 *)((s8 *)(arg0) + (6))) = (s8) var_r7;
                (*(void **)((s8 *)((*(void **)((s8 *)(arg1) + (4)))) + (0xF0))) = arg0;
                (*(u8 *)((s8 *)((*(void **)((s8 *)(arg1) + (4)))) + (0xF4))) = (u8) (*(u8 *)((s8 *)(arg0) + (7)));
                (*(s8 *)((s8 *)((*(void **)((s8 *)(arg1) + (4)))) + (0xF6))) = (s8) ((u8) (*(u8 *)((s8 *)(arg0) + (7))) / (u8) (*(u8 *)((s8 *)(arg0) + (8))));
                (*(s8 *)((s8 *)((*(void **)((s8 *)(arg1) + (4)))) + (0xF5))) = (s8) ((*(u8 *)((s8 *)(arg0) + (7))) % (*(u8 *)((s8 *)(arg0) + (8))));
                ((struct aipointertable_s *)arg0)->entries[(*(u8 *)((u8 *)arg0 + 7))++] = arg1;
            }
        }
    }
}

struct aioriginobject_s {
    u8 pad0[0x6c];
    struct aimovevec_s position;
    u8 pad78[0x190];
    u32 flags208;
};
struct aiorigincreature_s {
    u8 pad0[0x30];
    struct aimovevec_s position;
    u8 pad3c[0x6c];
};
struct aioriginsys_s {
    u8 pad0[0x14];
    struct aiorigincreature_s *creatures;
};
struct aioriginpacket_s {
    u8 pad0[0x50];
    struct aioriginobject_s *owner;
    u8 pad54[0x52];
    u8 index;
};

s32 Action_ResetToOrigin(struct aioriginsys_s *sys, void *process, struct aioriginpacket_s *packet)
{
    struct aioriginobject_s *owner = packet->owner;
    if ((owner->flags208 & 0x400) && packet->index != 0xff) {
        struct aiorigincreature_s *creature = sys->creatures + packet->index;
        owner->position = creature->position;
    }
    return 1;
}

extern f32 lbl_8019C68C[];

s32 Action_FollowPath(void *sys, void *process, void *packet, char **params, s32 count, s32 firstTime)
{
    s32 *owner;
    char **param;
    s32 remaining;

    if ((packet == NULL) || (owner = (*(s32 **)((s8 *)(packet) + (0x50))), ((owner == NULL) != 0)) || ((s32) *owner == 0)) {
        return 1;
    }
    if (!*(void **)((u8 *)packet + 0x10c) || !*(void **)((u8 *)packet + 0x110)) return 0;
    {
        if (firstTime != 0) {
            (*(s32 *)((s8 *)(packet) + (0x13C))) = 0;
            if (count > 0) {
                param = params;
                remaining = count;
                do {
                    if (lbl_80408D2C != NULL) {
                        lbl_80408D2C(*param, packet + 0xB0);
                    }
                    param += 1;
                    remaining -= 1;
                } while (remaining != 0);
            }
        }
        AIMoveInstruction(packet, 0, lbl_8019C68C[0], 0, 4, (*(f32 *)((s8 *)(packet) + (0x134))));
    }
    return 0;
}

f32 Condition_EitherPlayerInTriggerArea(u8 *sys, void *process, u8 *packet, char *params, u8 *inputArea)
{
    u8 *area = 0;
    u8 *player;

    if (sys) {
        if (inputArea) area = inputArea;
        else if (packet) area = *(u8 **)(packet + 0xf8);
        if (area) {
            player = *(u8 **)(sys + 0x1380);
            if (player && (*(u64 *)(area + 0x30) & __ashldi3(1ULL, player[0x291])))
                return lbl_8019C5E0[0];
            if (*(u8 **)(sys + 0x1384) && (*(u64 *)(area + 0x30) & __ashldi3(1ULL, (*(u8 **)(sys + 0x1384))[0x291])))
                return lbl_8019C5E0[0];
        }
    }
    return lbl_8019C5E4[0];
}

f32 NuCosApprox(s32);
f32 NuSinApprox(s32);
void NuVecScale(void *, void *, f32);

void AISysCheckAntinode_Ellipse(void *arg0, f32 *arg1, f32 *arg2, f32 farg0)
{
    f32 boundary[3];
    f32 tangent[3];
    f32 outer[3];
    f32 sp38[3];
    f32 sp48[3];
    f32 sp58[3];
    f32 *temp_r3;
    f32 *temp_r3_2;
    f32 temp_f0;
    f32 x1, x2;
    f32 temp_f0_2;
    f32 temp_f12;
    f32 temp_f13;
    f32 temp_f13_2;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    f32 temp_f1_5;
    f32 temp_f1_6;
    f32 temp_f1_7;
    f32 temp_f1_8;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f31;
    s32 temp_r0;
    s32 angle;
    s32 temp_r0_2;
    s32 temp_r27;

    s32 temp_r9_2;
    s32 var_r0;
    s32 var_r31;
    void *temp_r11;
    void *temp_r9;

    temp_f0 = (*(f32 *)((s8 *)(arg2) + (8)));
    temp_f13 = (*(f32 *)((s8 *)(arg2) + (0)));
    temp_f29 = (temp_f13 * temp_f13) + (temp_f0 * temp_f0);
    if (temp_f29 < (farg0 * farg0)) {
        temp_f13_2 = (*(f32 *)((s8 *)(arg1) + (0x44)));
        temp_f30 = 0.0f;
        temp_f31 = temp_f13_2 / (*(f32 *)((s8 *)(arg1) + (0x40)));
        (*(f32 *)((s8 *)(arg2) + (4))) = temp_f30;
        NuVecRotateY(sp38, arg2, -(*(s32 *)((s8 *)(arg1) + (0x38))));
        temp_r27 = fn_8000ABDC(sp38[0] * temp_f31, sp38[2]);
        temp_f1 = NuSinApprox(temp_r27);
        outer[1] = temp_f30;
        temp_f1_2 = temp_f1 * (*(f32 *)((s8 *)(arg1) + (0x40)));
        outer[0] = temp_f1_2;
        temp_f1_3 = NuCosApprox(temp_r27) * (*(f32 *)((s8 *)(arg1) + (0x44)));
        outer[2] = temp_f1_3;
        NuVecNorm(arg2, &outer[0]);
        NuVecScale(arg2, arg2, (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x9C))));
        NuVecAdd(&outer[0], &outer[0], arg2);
        if (temp_f29 < ((outer[0] * outer[0]) + (outer[2] * outer[2]))) {
            x1 = (*(f32 *)((s8 *)(arg0) + (0x6C))) - (*(f32 *)((s8 *)(arg1) + (0)));
            (*(f32 *)((s8 *)(arg2) + (4))) = temp_f30;
            (*(f32 *)((s8 *)(arg2) + (0))) = x1;
            (*(f32 *)((s8 *)(arg2) + (8))) = (f32) ((*(f32 *)((s8 *)(arg0) + (0x74))) - (*(f32 *)((s8 *)(arg1) + (8))));
            NuVecRotateY(sp48, arg2, -(*(s32 *)((s8 *)(arg1) + (0x38))));
            temp_r27 = fn_8000ABDC(sp48[0] * temp_f31, sp48[2]);
            temp_f1_4 = NuSinApprox(temp_r27);
            boundary[1] = temp_f30;
            temp_f1_5 = temp_f1_4 * (*(f32 *)((s8 *)(arg1) + (0x40)));
            boundary[0] = temp_f1_5;
            temp_f1_6 = NuCosApprox(temp_r27) * (*(f32 *)((s8 *)(arg1) + (0x44)));
            boundary[2] = temp_f1_6;
            NuVecNorm(arg2, &boundary[0]);
            NuVecScale(arg2, arg2, (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x9C))));
            NuVecAdd(&boundary[0], &boundary[0], arg2);
            x2 = (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x80))) - (*(f32 *)((s8 *)(arg1) + (0)));
            (*(f32 *)((s8 *)(arg2) + (4))) = temp_f30;
            (*(f32 *)((s8 *)(arg2) + (0))) = x2;
            (*(f32 *)((s8 *)(arg2) + (8))) = (f32) ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x88))) - (*(f32 *)((s8 *)(arg1) + (8))));
            NuVecRotateY(sp58, arg2, -(*(s32 *)((s8 *)(arg1) + (0x38))));
            angle = fn_8000ABDC(sp58[0] * temp_f31, sp58[2]);
            var_r31 = NuAngSub(angle, temp_r27);
            temp_r11 = (*(void **)((s8 *)(arg0) + (4)));
            temp_f12 = (*(f32 *)((s8 *)(temp_r11) + (0x140)));
            if (temp_f12 > temp_f30) {
                if ((((s32) ((*(s32 *)((s8 *)(temp_r11) + (0x118))) & 0xC000) == 0x4000) || ((*(f32 *)((s8 *)(arg0) + (0x1E8))) > 1.0f)) && (temp_f12 < lbl_80408D54)) {
                    temp_r0 = (*(s32 *)((s8 *)(temp_r11) + (0x1A0)));
                    ((struct aiturnbits_s *)((u8 *)temp_r11 + 0x1a0))->value = !((struct aiturnbits_s *)((u8 *)temp_r11 + 0x1a0))->value;
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x140))) = (f32) (lbl_80408D58 + lbl_80408D54);
                }
                if ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x1A0))) & 0x8000) {
                    if (var_r31 < 0) {
                        var_r31 += 0x10000;
                    }
                } else if (var_r31 > 0) {
                    var_r31 += 0xFFFF0000;
                }
            } else {
                if (var_r31 > 0) {
                    var_r0 = (*(s32 *)((s8 *)(temp_r11) + (0x1A0))) | 0x8000;
                } else {
                    var_r0 = (*(s32 *)((s8 *)(temp_r11) + (0x1A0))) & 0xFFFF7FFF;
                }
                (*(s32 *)((s8 *)(temp_r11) + (0x1A0))) = var_r0;
            }
            if (!((*(u8 *)((s8 *)(arg1) + (0x49))) & 2)) {
                temp_r9 = (*(void **)((s8 *)(arg0) + (4)));
                if ((*(f32 *)((s8 *)(temp_r9) + (0x140))) < lbl_80408D54) {
                    (*(f32 *)((s8 *)(temp_r9) + (0x140))) = (f32) lbl_80408D54;
                }
            }
            temp_f1_7 = NuCosApprox(temp_r27) * (*(f32 *)((s8 *)(arg1) + (0x40)));
            tangent[0] = temp_f1_7;
            temp_f1_8 = -NuSinApprox(temp_r27) * (*(f32 *)((s8 *)(arg1) + (0x44)));
            tangent[2] = temp_f1_8;
            if (var_r31 < 0) {
                tangent[2] = -temp_f1_8;
                tangent[0] = -tangent[0];
            }
            temp_r9_2 = var_r31 >> 0x1F;
            temp_r0_2 = (temp_r9_2 ^ var_r31) - temp_r9_2;
            if (temp_r0_2 <= 0x71B) {
                temp_f0_2 = (f32) (f64) temp_r0_2 / 1.82e+03f;
                tangent[0] *= temp_f0_2;
                tangent[2] *= temp_f0_2;
            }
            (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x90))) = (f32) (boundary[0] + tangent[0]);
            (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x94))) = (f32) 0.0f;
            (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x98))) = (f32) (boundary[2] + tangent[2]);
            temp_r3 = (*(void **)((s8 *)(arg0) + (4))) + 0x90;
            NuVecRotateY(temp_r3, temp_r3, (*(s32 *)((s8 *)(arg1) + (0x38))));
            temp_r3_2 = (*(void **)((s8 *)(arg0) + (4))) + 0x90;
            NuVecAdd(temp_r3_2, temp_r3_2, arg1);
        }
    }
}

f32 fn_8000ED40(f32);
extern f32 lbl_8019C818[];

void AISysCheckAntinode_Rectangle(void *arg0, void *arg1, f32 *arg2, f32 farg0)
{
    f32 tangent[3];
    f32 sp18[3];
    f32 sp28[3];
    f32 *temp_r3_2;
    f32 temp_f0;
    f32 x;
    f32 sinAngle;
    f32 cosAngle;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f31;
    s32 temp_r0;
    s32 angle;
    s32 temp_r0_2;
    s32 temp_r28;
    s32 temp_r31;
    s32 temp_r9_5;
    s32 var_r0;
    s32 var_r27;
    void *temp_r11;
    void *temp_r11_4;
    void *temp_r11_5;
    void *temp_r3_3;
    void *temp_r9;

    temp_f0 = (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x90))) - (*(f32 *)((s8 *)(arg1) + (0)));
    (*(f32 *)((s8 *)(arg2) + (0))) = temp_f0;
    if (!(temp_f0 > farg0)) {
        temp_f12 = -farg0;
        if (!(temp_f0 < temp_f12)) {
            temp_f0_2 = (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x98))) - (*(f32 *)((s8 *)(arg1) + (8)));
            (*(f32 *)((s8 *)(arg2) + (8))) = temp_f0_2;
            if (!(temp_f0_2 > farg0) && !(temp_f0_2 < temp_f12) && !((*(f32 *)((s8 *)(arg0) + (0x18C))) > (*(f32 *)((s8 *)(arg1) + (0x18)))) && !((*(f32 *)((s8 *)(arg1) + (0x10))) > (*(f32 *)((s8 *)(arg0) + (0x198))))) {
                NuVecRotateY(arg2, arg2, -(*(s32 *)((s8 *)(arg1) + (0x38))));
                if ((fn_8000ED40((*(f32 *)((s8 *)(arg2) + (0)))) < ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x9C))) + (*(f32 *)((s8 *)(arg1) + (0x40))))) && (fn_8000ED40((*(f32 *)((s8 *)(arg2) + (8)))) < ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x9C))) + (*(f32 *)((s8 *)(arg1) + (0x44)))))) {
                    temp_f31 = 0.0f;
                    (*(f32 *)((s8 *)(arg2) + (0))) = (*(f32 *)((s8 *)(arg0) + (0x6C))) - (*(f32 *)((s8 *)(arg1) + (0)));
                    (*(f32 *)((s8 *)(arg2) + (4))) = temp_f31;
                    (*(f32 *)((s8 *)(arg2) + (8))) = (f32) ((*(f32 *)((s8 *)(arg0) + (0x74))) - (*(f32 *)((s8 *)(arg1) + (8))));
                    NuVecRotateY(sp18, arg2, -(*(s32 *)((s8 *)(arg1) + (0x38))));
                    temp_r28 = fn_8000ABDC(sp18[0], sp18[2]);
                    x = (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x80))) - (*(f32 *)((s8 *)(arg1) + (0)));
                    (*(f32 *)((s8 *)(arg2) + (4))) = temp_f31;
                    (*(f32 *)((s8 *)(arg2) + (0))) = x;
                    (*(f32 *)((s8 *)(arg2) + (8))) = (f32) ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x88))) - (*(f32 *)((s8 *)(arg1) + (8))));
                    NuVecRotateY(sp28, arg2, -(*(s32 *)((s8 *)(arg1) + (0x38))));
                    angle = fn_8000ABDC(sp28[0], sp28[2]);
                    var_r27 = NuAngSub(angle, temp_r28);
                    temp_r11 = (*(void **)((s8 *)(arg0) + (4)));
                    temp_f12_2 = (*(f32 *)((s8 *)(temp_r11) + (0x140)));
                    if (temp_f12_2 > temp_f31) {
                        if ((((s32) ((*(s32 *)((s8 *)(temp_r11) + (0x118))) & 0xC000) == 0x4000) || ((*(f32 *)((s8 *)(arg0) + (0x1E8))) > 1.0f)) && (temp_f12_2 < lbl_80408D54)) {
                            temp_r0 = (*(s32 *)((s8 *)(temp_r11) + (0x1A0)));
                            ((struct aiturnbits_s *)((u8 *)temp_r11 + 0x1a0))->value = !((struct aiturnbits_s *)((u8 *)temp_r11 + 0x1a0))->value;
                            (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x140))) = (f32) (lbl_80408D58 + lbl_80408D54);
                        }
                        if ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x1A0))) & 0x8000) {
                            if (var_r27 < 0) {
                                var_r27 += 0x10000;
                            }
                        } else if (var_r27 > 0) {
                            var_r27 += 0xFFFF0000;
                        }
                    } else {
                        if (var_r27 > 0) {
                            var_r0 = (*(s32 *)((s8 *)(temp_r11) + (0x1A0))) | 0x8000;
                        } else {
                            var_r0 = (*(s32 *)((s8 *)(temp_r11) + (0x1A0))) & 0xFFFF7FFF;
                        }
                        (*(s32 *)((s8 *)(temp_r11) + (0x1A0))) = var_r0;
                    }
                    if (!((*(u8 *)((s8 *)(arg1) + (0x49))) & 2)) {
                        temp_r9 = (*(void **)((s8 *)(arg0) + (4)));
                        if ((*(f32 *)((s8 *)(temp_r9) + (0x140))) < lbl_80408D54) {
                            (*(f32 *)((s8 *)(temp_r9) + (0x140))) = (f32) lbl_80408D54;
                        }
                    }
                    temp_r31 = fn_8000ABDC((*(f32 *)((s8 *)(arg1) + (0x40))), (*(f32 *)((s8 *)(arg1) + (0x44))));
                    temp_r28 = NuAngAdd(temp_r28, 0);
                    if (temp_r28 > (s32)(0x8000U - temp_r31)) {
                        tangent[0] = -1.0f;
                        tangent[2] = 0.0f;
                        sinAngle = NuSinApprox(temp_r28);
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x90) = (*(f32 *)((u8 *)arg1 + 0x40) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c)) * sinAngle;
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x98) = -(*(f32 *)((u8 *)arg1 + 0x44) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c));
                    } else if (temp_r28 > temp_r31) {
                        tangent[0] = 0.0f;
                        tangent[2] = -1.0f;
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x90) = *(f32 *)((u8 *)arg1 + 0x40) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c);
                        cosAngle = NuCosApprox(temp_r28);
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x98) = (*(f32 *)((u8 *)arg1 + 0x44) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c)) * cosAngle;
                    } else if (temp_r28 > -temp_r31) {
                        tangent[0] = 1.0f;
                        tangent[2] = 0.0f;
                        sinAngle = NuSinApprox(temp_r28);
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x90) = (*(f32 *)((u8 *)arg1 + 0x40) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c)) * sinAngle;
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x98) = *(f32 *)((u8 *)arg1 + 0x44) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c);
                    } else if (temp_r28 > temp_r31 - 0x8000) {
                        tangent[0] = 0.0f;
                        tangent[2] = 1.0f;
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x90) = -(*(f32 *)((u8 *)arg1 + 0x40) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c));
                        cosAngle = NuCosApprox(temp_r28);
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x98) = (*(f32 *)((u8 *)arg1 + 0x44) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c)) * cosAngle;
                    } else {
                        tangent[0] = -1.0f;
                        tangent[2] = 0.0f;
                        sinAngle = NuSinApprox(temp_r28);
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x90) = (*(f32 *)((u8 *)arg1 + 0x40) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c)) * sinAngle;
                        *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x98) = -(*(f32 *)((u8 *)arg1 + 0x44) + *(f32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x9c));
                    }
                    if (var_r27 < 0) {
                        tangent[0] = -tangent[0];
                        tangent[2] = -tangent[2];
                    }
                    temp_r9_5 = var_r27 >> 0x1F;
                    temp_r0_2 = (temp_r9_5 ^ var_r27) - temp_r9_5;
                    if (temp_r0_2 <= 0x71B) {
                        temp_f0_3 = (f32) (f64) temp_r0_2 / lbl_8019C818[0];
                        tangent[0] *= temp_f0_3;
                        tangent[2] *= temp_f0_3;
                    }
                    temp_r11_4 = (*(void **)((s8 *)(arg0) + (4)));
                    (*(f32 *)((s8 *)(temp_r11_4) + (0x90))) = (f32) ((*(f32 *)((s8 *)(temp_r11_4) + (0x90))) + tangent[0]);
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (4)))) + (0x94))) = (f32) 0.0f;
                    temp_r11_5 = (*(void **)((s8 *)(arg0) + (4)));
                    (*(f32 *)((s8 *)(temp_r11_5) + (0x98))) = (f32) ((*(f32 *)((s8 *)(temp_r11_5) + (0x98))) + tangent[2]);
                    temp_r3_2 = (*(void **)((s8 *)(arg0) + (4))) + 0x90;
                    NuVecRotateY(temp_r3_2, temp_r3_2, (*(s32 *)((s8 *)(arg1) + (0x38))));
                    temp_r3_3 = (*(void **)((s8 *)(arg0) + (4))) + 0x90;
                    NuVecAdd(temp_r3_3, temp_r3_3, arg1);
                }
            }
        }
    }
}
