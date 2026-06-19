typedef unsigned int u32;
static u32 fseed;

// Force two-step load to try to get the load-first pattern
u32 A(void) {
    u32 x;
    x = fseed;
    fseed = x = x * 0x19660d + 0x3c6ef35f;
    return x;
}
// Explicit seed pointer argument version (closer to NuRandIntPtr)
u32 B(void) {
    u32 *p = &fseed;
    u32 x = *p;
    x = x * 0x19660d + 0x3c6ef35f;
    *p = x;
    return x;
}
