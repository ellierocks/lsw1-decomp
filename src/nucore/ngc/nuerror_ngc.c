#include "types.h"

extern void* lbl_8040A2C8;
extern void* lbl_8040A2CC;
extern void* lbl_8040A2D0;

void* fn_80009564();
void* fn_8000965C();
void* fn_80009748();
void* fn_80009858();

void* fn_80009948(void* a, void* b)
{
    lbl_8040A2C8 = a;
    lbl_8040A2CC = b;
    return fn_80009564;
}

void* fn_8000995C(void* a, void* b)
{
    lbl_8040A2C8 = a;
    lbl_8040A2CC = b;
    return fn_8000965C;
}

void* fn_80009970(void* a, void* b)
{
    lbl_8040A2C8 = a;
    lbl_8040A2CC = b;
    return fn_80009748;
}

void* fn_80009984(void* a, void* b)
{
    lbl_8040A2C8 = a;
    lbl_8040A2CC = b;
    return fn_80009858;
}

void fn_80009998(void* a)
{
    lbl_8040A2D0 = a;
}

extern char lbl_8018E27C[];

void OSPanic(const char* file, int line, const char* msg, ...);

void fn_800099A0(const char* msg)
{
    OSPanic(lbl_8018E27C, 21, msg);
}
