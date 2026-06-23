#include "numath/nurand.h"

static struct nunrand_s global_rand;
static u32 fseed;

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
