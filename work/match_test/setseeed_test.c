typedef signed int s32;
typedef unsigned int u32;

struct nunrand_s { s32 idum; };
static struct nunrand_s global_rand;

// Try 1: nrand == 0
void NuRandSetSeed_v1(struct nunrand_s* nrand, s32 seed) {
    if (nrand == 0) nrand = &global_rand;
    nrand->idum = seed;
}
// Try 2: !nrand
void NuRandSetSeed_v2(struct nunrand_s* nrand, s32 seed) {
    if (!nrand) nrand = &global_rand;
    nrand->idum = seed;
}
// Try 3: nrand = nrand ? nrand : &global_rand
void NuRandSetSeed_v3(struct nunrand_s* nrand, s32 seed) {
    nrand = nrand ? nrand : &global_rand;
    nrand->idum = seed;
}
