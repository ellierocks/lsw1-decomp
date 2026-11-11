/* Constructor loop adapted from MSSB (CC0), commit
 * 407dbcbe70f8be5d167d0b1f0a229f38e0359019,
 * src/Dolphin/os/__ppc_eabi_init.cpp. This is not the donor's fragment registry.
 */
typedef void (*Initializer)(void);
extern Initializer lbl_801FD050[];
static void __init_cpp(void);

void fn_801638A0(void)
{
    __init_cpp();
}

static void __init_cpp(void)
{
    Initializer *constructor;
    for (constructor = lbl_801FD050; *constructor; constructor++) {
        (*constructor)();
    }
}
