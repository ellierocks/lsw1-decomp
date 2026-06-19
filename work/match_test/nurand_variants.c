typedef signed int s32;
typedef unsigned int u32;
struct nunrand_s { s32 idum; };
static u32 fseed;
static struct nunrand_s global_rand;

// NuRandInt variant: explicit local to keep value in register
u32 NuRandInt_v1(void) {
    u32 x = fseed * 0x19660d + 0x3c6ef35f;
    fseed = x;
    return x;
}

// NuRandIntPtr: explicit local avoids reload
u32 NuRandIntPtr_v1(u32* seed) {
    u32 x = *seed * 0x19660d + 0x3c6ef35f;
    *seed = x;
    return x;
}

// NuRandSetSeed: force signed comparison with cmpwi (not cmplwi)?
void NuRandSetSeed_signed(struct nunrand_s* nrand, s32 seed) {
    if ((s32)nrand == 0) nrand = &global_rand;
    nrand->idum = seed;
}
