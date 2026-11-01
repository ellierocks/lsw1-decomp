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
