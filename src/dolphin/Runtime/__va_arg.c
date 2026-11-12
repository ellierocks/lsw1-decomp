/* CodeWarrior runtime va_arg helper, retained in the SN link at 0x801531E8.
 * MSSB (CC0), commit 407dbcbe70f8be5d167d0b1f0a229f38e0359019,
 * src/Dolphin/Runtime/__va_arg.c, with the later type-4 (vector) case that
 * LSW1's body tests first.
 */
typedef struct __va_list_struct {
    signed char gpr;
    signed char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list[1];

void* __va_arg(__va_list v_list, unsigned char type)
{
    char* addr;
    signed char* reg = &(v_list->gpr);
    int g_reg = v_list->gpr;
    int maxsize = 8;
    int size = 4;
    int increment = 1;
    int even = 0;
    int fpr_offset = 0;
    int regsize = 4;

    if (type == 4) {
        addr = (char*)(((unsigned long)(v_list->input_arg_area) + 15) & ~15);
        v_list->input_arg_area = addr + 16;
        return addr;
    }
    if (type == 3) {
        reg = &(v_list->fpr);
        g_reg = v_list->fpr;
        size = 8;
        fpr_offset = 32;
        regsize = 8;
    }
    if (type == 2) {
        size = 8;
        maxsize--;
        if (g_reg & 1)
            even = 1;
        increment = 2;
    }
    if (g_reg < maxsize) {
        g_reg += even;
        addr = v_list->reg_save_area + fpr_offset + (g_reg * regsize);
        *reg = g_reg + increment;
    } else {
        *reg = 8;
        addr = v_list->input_arg_area;
        addr = (char*)(((unsigned long)(addr) + ((size)-1)) & ~((size)-1));
        v_list->input_arg_area = addr + size;
    }
    if (type == 0)
        addr = *((char**)addr);

    return addr;
}
