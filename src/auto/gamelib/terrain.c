#include "types.h"

extern s32 lbl_804083B0;
extern s32 lbl_8040A604;
extern s32 lbl_8040A608;

void HVQM4PlayerExBufvSetBusyFlag(u8* a, s32 b)
{
    *(s32*)(a + 20) = b;
}

void fn_8005A320(s32 a, u8* b, s32 c)
{
    lbl_804083B0 = a;
    lbl_8040A604 = b;
    *(s32*)b = 0;
    lbl_8040A608 = c;
}

void HVQM4PlayerEx1stFrameFunc(u8* a, s32 b, s32 c)
{
    *(s32*)(a + 988) = b;
    *(s32*)(a + 992) = c;
}

s32 HVQM4PlayerExCheckExecute(u8* a)
{
    return *(s32*)(a + 928);
}

s32 HVQM4PlayerExGetMicroSecPerFrame(u8* a)
{
    return *(s32*)(a + 40);
}

s32 HVQM4PlayerExGetAudioSamples(u8* a)
{
    return *(s32*)(a + 68);
}

s32 fn_8004ACE0(u8* a)
{
    return *(s32*)(a + 16);
}

s32 fn_8004ADAC(u8* a)
{
    return *(s32*)(a + 12);
}

s32 fn_8004ADB4(u8* a)
{
    return *(s32*)(a + 8);
}

s32 HVQM4PlayerExControlExit(u8* a)
{
    *(s32*)(a + 956) = 1;
    *(s32*)(a + 944) = 1;
    return 1;
}

s32 fn_8004BCB0(u8* a)
{
    return *(s32*)(a + 84);
}
