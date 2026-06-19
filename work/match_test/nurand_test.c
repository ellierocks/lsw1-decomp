typedef long s32;

struct nunrand_s {
    s32 idum;
};

static struct nunrand_s global_rand;

s32 NuRand(struct nunrand_s* nrand)
{
    s32 k, tmp;
    if (nrand == 0) {
        nrand = &global_rand;
    } else if (nrand->idum == 0) {
        nrand->idum = 1;
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
