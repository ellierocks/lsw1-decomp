#include <dolphin.h>
#include <dolphin/ax.h>

#include "__ax.h"

static AXSPB __AXStudio ATTRIBUTE_ALIGN(32);

extern s32 __AXSpbAL;
extern s32 __AXSpbAR;
extern s32 __AXSpbAS;
extern s32 __AXSpbAAL;
extern s32 __AXSpbAAR;
extern s32 __AXSpbAAS;
extern s32 __AXSpbABL;
extern s32 __AXSpbABR;
extern s32 __AXSpbABS;

u32 __AXGetStudio(void) {
    return (u32)&__AXStudio;
}



void __AXPrintStudio(void) {
    __AXDepopFade(&__AXSpbAL, (void*)&__AXStudio.dpopLHi, &__AXStudio.dpopLDelta);
    __AXDepopFade(&__AXSpbAR, (void*)&__AXStudio.dpopRHi, &__AXStudio.dpopRDelta);
    __AXDepopFade(&__AXSpbAS, (void*)&__AXStudio.dpopSHi, &__AXStudio.dpopSDelta);
    __AXDepopFade(&__AXSpbAAL, (void*)&__AXStudio.dpopALHi, &__AXStudio.dpopALDelta);
    __AXDepopFade(&__AXSpbAAR, (void*)&__AXStudio.dpopARHi, &__AXStudio.dpopARDelta);
    __AXDepopFade(&__AXSpbAAS, (void*)&__AXStudio.dpopASHi, &__AXStudio.dpopASDelta);
    __AXDepopFade(&__AXSpbABL, (void*)&__AXStudio.dpopBLHi, &__AXStudio.dpopBLDelta);
    __AXDepopFade(&__AXSpbABR, (void*)&__AXStudio.dpopBRHi, &__AXStudio.dpopBRDelta);
    __AXDepopFade(&__AXSpbABS, (void*)&__AXStudio.dpopBSHi, &__AXStudio.dpopBSDelta);
    DCFlushRange(&__AXStudio, sizeof(__AXStudio));
}

void __AXSPBInit(void) {
#ifdef DEBUG
    OSReport("Initializing AXSPB code module\n");
#endif
    __AXSpbAL = 
    __AXSpbAR = 
    __AXSpbAS = 
    __AXSpbAAL = 
    __AXSpbAAR = 
    __AXSpbAAS = 
    __AXSpbABL = 
    __AXSpbABR = 
    __AXSpbABS = 0;
}

void __AXSPBQuit(void) {
#ifdef DEBUG
    OSReport("Shutting down AXSPB code module\n");
#endif
}

void __AXDepopVoice(AXPB* p) {
    __AXSpbAL += p->dpop.aL;
    __AXSpbAAL += p->dpop.aAuxAL;
    __AXSpbABL += p->dpop.aAuxBL;
    __AXSpbAR += p->dpop.aR;
    __AXSpbAAR += p->dpop.aAuxAR;
    __AXSpbABR += p->dpop.aAuxBR;
    __AXSpbAS += p->dpop.aS;
    __AXSpbAAS += p->dpop.aAuxAS;
    __AXSpbABS += p->dpop.aAuxBS;
}
