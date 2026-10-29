#include "numath/nurand.h"

static u32 fseed = 0;
static u32 unk_80407B44 = 0x1365;
static struct nunrand_s global_rand = { 1 };
// Retail .sdata for this unit continues for 24 bytes after global_rand with no
// direct references; they are carried verbatim so the linked layout is exact.
static u32 unk_80407B4C = 0;
static u32 unk_80407B50 = 0;
static u32 unk_80407B54 = 0;
static u32 unk_80407B58 = 0;
static u32 unk_80407B5C = 0;
static u32 unk_80407B60 = 1;

// The retail symbol table still names these by address, and other units
// reference those names, so export them as aliases until symbols.txt is renamed.
asm(".globl lbl_80407B40\n\t.set lbl_80407B40, fseed\n\t.globl lbl_80407B48\n\t.set lbl_80407B48, global_rand");

u32 NuRandInt(void)
{
    fseed = fseed * 0x19660d + 0x3c6ef35f;
    return fseed;
}

u32 NuRandIntPtr(u32* seed)
{
    u32 x = *seed * 0x19660d + 0x3c6ef35f;
    *seed = x;
    return x;
}

void NuRandSeed(u32 seed)
{
    fseed = seed;
}

void NuRandSetSeed(struct nunrand_s* nrand, s32 seed)
{
    if (nrand == 0) {
        nrand = &global_rand;
    }
    nrand->idum = seed;
}

s32 NuRand(struct nunrand_s* nrand)
{
    s32 k, tmp;
    if (nrand != 0) {
        if (nrand->idum == 0) {
            nrand->idum = 1;
        }
    } else {
        nrand = &global_rand;
    }
    tmp = nrand->idum ^ 0x75bd924;
    k = tmp / 127773;
    nrand->idum = (tmp - k * 127773) * 16807 - k * 2836;
    if (nrand->idum < 0) {
        nrand->idum += 0x7fffffff;
    }
    nrand->idum ^= 0x75bd924;
    return nrand->idum;
}
