#include "types.h"

extern void OSReport(const char* fmt, ...);
extern char lbl_8018E2B4[];

void fn_800099D4(char* fmt)
{
    OSReport(fmt);
}

void fn_800099F8(s32 arg)
{
    OSReport(lbl_8018E2B4, arg);
}

s32 DVDClose(void *);
s32 OSCreateThread(s32 *, s32 *, s32, s32, s32, s32, s32);
s32 OSGetCurrentThread();
s32 OSInitSemaphore(s32 *, s32);
s32 OSResumeThread(s32 *);
s32 OSSignalSemaphore(s32 *);
s32 OSWaitSemaphore(s32 *);
s32 VIWaitForRetrace();
s32 fn_80038210();
extern s32 fn_8000A724[];
extern s32 lbl_8018E3A0[];
extern s32 lbl_80207AC0[];
extern s32 lbl_80207DC0[];
extern s32 lbl_80207DCC[];
extern s32 lbl_80209DF0[];
extern s32 lbl_80407AF0;
extern s32 lbl_80407B04;
extern s32 lbl_80407B08;
extern void *lbl_80407B0C;
extern s32 lbl_80407F70;
extern s32 lbl_8040A2DC;

s32 fn_80009C00(s32 arg0)
{
  s32 temp_r9;
  s32 *new_var;
  void *temp_r30;
  temp_r30 = (arg0 * 0x4C) + ((s8 *) lbl_80207AC0);
  if (((s32) lbl_80407B08) == 0)
  {
    new_var = lbl_80207DCC;
    temp_r9 = ((s32) (((s8 *) new_var) + 0x1F)) & 0xFFFFFFE0;
    lbl_8040A2DC = temp_r9;
    OSCreateThread(lbl_80209DF0, fn_8000A724, 0, temp_r9 + 0x1FFF, 0x2000, 0x14, 0);
    lbl_80407B08 = 1;
  }
  if (((s32) lbl_80407B04) == 0)
  {
    OSInitSemaphore(lbl_80207DC0, 1);
    lbl_80407B04 = 1;
  }
  OSWaitSemaphore(lbl_80207DC0);
  if (OSGetCurrentThread() == ((s32) lbl_80407F70))
  {
    lbl_80407B0C = temp_r30;
    OSResumeThread(lbl_80209DF0);
    if (((void *) lbl_80407B0C) != ((void *) 0))
    {
      do
      {
        OSReport(lbl_8018E3A0);
        fn_80038210();
        VIWaitForRetrace();
      }
      while (((void *) lbl_80407B0C) != ((void *) 0));
    }
  }
  else
  {
    DVDClose(temp_r30);
  }
  *((s32 *) (((s8 *) temp_r30) + 0x44)) = 1;
  if (((s32) lbl_80407AF0) == arg0)
  {
    lbl_80407AF0 = -1;
  }
  OSSignalSemaphore(lbl_80207DC0);
  return 0;
}
