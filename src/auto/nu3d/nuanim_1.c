#include "types.h"

extern f32 lbl_8040A318;
extern f32 fn_8002DE38(void);

void fn_8001662C(f32 a)
{
    lbl_8040A318 = a * fn_8002DE38();
}
