/* SN's CodeWarrior-to-libgcc ABI bridge, reconstructed from GL5E4F branches.
 * MSSB (CC0), commit 407dbcbe70f8be5d167d0b1f0a229f38e0359019,
 * src/Dolphin/Runtime/runtime.c was reviewed: its full arithmetic bodies are
 * absent here. These original tail entries preserve the incoming EABI registers.
 */
extern void __lshrdi3(void);
extern void __divdi3(void);
extern void __ashldi3(void);
extern void __udivdi3(void);
extern void __ashrdi3(void);
extern void __moddi3(void);
extern void __umoddi3(void);

asm void __shr2u(void)
{
    nofralloc
    b __lshrdi3
}

asm void __div2i(void)
{
    nofralloc
    b __divdi3
}

asm void __shl2i(void)
{
    nofralloc
    b __ashldi3
}

asm void __mod2i(void)
{
    nofralloc
    b __moddi3
}

asm void __shr2i(void)
{
    nofralloc
    b __ashrdi3
}

asm void __div2u(void)
{
    nofralloc
    b __udivdi3
}

asm void __mod2u(void)
{
    nofralloc
    b __umoddi3
}
