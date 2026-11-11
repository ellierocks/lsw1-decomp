/* Independent ProDG candidate. MSSB (CC0), commit
 * 407dbcbe70f8be5d167d0b1f0a229f38e0359019, Runtime/runtime.c was
 * reviewed; its saturating asm conversion does not implement this SN bridge.
 */
unsigned int __cvt_fp2unsigned(double value)
{
    return (unsigned int)value;
}
