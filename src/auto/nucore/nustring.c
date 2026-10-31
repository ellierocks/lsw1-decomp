#include "types.h"

struct nulnode_s {
    struct nulnode_s* next;
    struct nulnode_s* prev;
};

struct nulist_s {
    struct nulnode_s* first;
    struct nulnode_s* last;
};

extern s32 lbl_80407AAC;

void* memset(void* dst, s32 v, u32 n);

s32 NuStrLen(char* s)
{
    s32 n = 0;
    while (*s != 0) {
        n++;
        s++;
    }
    return n;
}

s32 NuStrCpy(char* dst, char* src)
{
    s32 n = 0;
    if (src == 0) {
        *dst = 0;
    } else {
        do {
            *dst++ = *src;
            n++;
        } while (*src++ != 0);
    }
    return n;
}

void NuStrCat(char* dst, char* src)
{
    while (*dst != 0) {
        dst++;
    }
    if (src == 0) {
        return;
    }
    do {
        *dst++ = *src;
    } while (*src++ != 0);
}

void fn_80007594(s32 v)
{
    lbl_80407AAC = v;
}

void fn_80007610(struct nulist_s* list, struct nulnode_s* node)
{
    node->next = 0;
    node->prev = list->last;
    if (list->last != 0) {
        list->last->next = node;
    }
    list->last = node;
    if (list->first == 0) {
        list->first = node;
    }
}

void fn_80007648(struct nulist_s* list, struct nulnode_s* node)
{
    node->next = list->first;
    node->prev = 0;
    if (list->first != 0) {
        list->first->prev = node;
    }
    if (list->last == 0) {
        list->last = node;
    }
    list->first = node;
}

void fn_80007680(struct nulist_s* list, struct nulnode_s* node)
{
    if (node->next != 0) {
        node->next->prev = node->prev;
    } else {
        list->last = node->prev;
    }
    if (node->prev != 0) {
        node->prev->next = node->next;
    } else {
        list->first = node->next;
    }
}

u8 NuToUpper(u8 c)
{
    if (c >= 'a' && c <= 'z') {
        c -= 0x20;
    } else if (c > 0xdf) {
        c -= 0x20;
    }
    return c;
}

u8 NuToLower(u8 c)
{
    if (c >= 'A' && c <= 'Z') {
        c += 0x20;
    } else if (c > 0xbf && c < 0xe0) {
        c += 0x20;
    }
    return c;
}

s32 NuStrNCpy(char* dst, char* src, s32 n)
{
    s32 count = 0;
    if (src == 0) {
        *dst = 0;
    } else {
        do {
            n--;
            if (n < 1) {
                *dst = 0;
                count++;
                break;
            }
            *dst = *src;
            dst++;
            count++;
        } while (*src++ != 0);
    }
    return count;
}

s32 NuStrCmp(char* a, char* b)
{
    s8 ca;
    s8 cb;
    if (a == 0) {
        return -1;
    }
    if (b == 0) {
        return 1;
    }
    do {
        ca = *a;
        cb = *b;
        if (ca > cb) {
            return 1;
        }
        if (ca < cb) {
            return -1;
        }
        a++;
        b++;
    } while (ca != 0 && cb != 0);
    return 0;
}

void fn_8000759C(u8* dst, u8* src, s32 size)
{
    if (dst < src) {
        while (size != 0) {
            size--;
            *dst = *src;
            dst++;
            src++;
        }
    } else {
        dst += size;
        src += size;
        while (size != 0) {
            size--;
            dst--;
            src--;
            *dst = *src;
        }
    }
}

void fn_800075EC(void* p, s32 v, s32 n)
{
    memset(p, v, n << 4);
}

s32 NuAToI(char* s)
{
    s32 value = 0;
    s32 sign = 0;
    s8 c = *s;
    s++;
    if (c == '-') {
        sign = -1;
        c = *s;
        s++;
    }
    while (c >= '0' && c <= '9') {
        value = (value << 3) + (value << 1);
        value = value + c - 0x30;
        c = *s;
        s++;
    }
    if (sign != 0) {
        return value * sign;
    }
    return value;
}

s32 NuStrNCmp(char* a, char* b, s32 n)
{
    s8 ca;
    s8 cb;
    if (a == 0) {
        return -1;
    }
    if (b == 0) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        char* t = a;
        n = 0;
        while (*t != 0) {
            n++;
            t++;
        }
    } else if (n == -2) {
        char* t = b;
        n = 0;
        while (*t != 0) {
            n++;
            t++;
        }
    }
    do {
        ca = *a;
        cb = *b;
        if (ca > cb) {
            return 1;
        }
        if (ca < cb) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (ca != 0 && cb != 0 && n != 0);
    return 0;
}

void NuStrUpr(char* dst, char* src)
{
    for (; *src != 0; src++) {
        *dst = NuToUpper(*src);
        dst++;
    }
    *dst = *src;
}

s32 NuStrICmp(char* a, char* b)
{
    s8 ca;
    s8 cb;
    if (a == 0) {
        return -1;
    }
    if (b == 0) {
        return 1;
    }
    do {
        ca = NuToUpper(*a);
        cb = NuToUpper(*b);
        if (ca > cb) {
            return 1;
        }
        if (ca < cb) {
            return -1;
        }
        a++;
        b++;
    } while (ca != 0 && cb != 0);
    return 0;
}

char* NuStrIStr(char* str, char* sub)
{
    char* sc;
    char* bc;
    while (*str != 0) {
        sc = str;
        bc = sub;
        while (*bc != 0) {
            if (*sc == 0) {
                break;
            }
            if (NuToUpper(*sc) != NuToUpper(*bc)) {
                break;
            }
            sc++;
            bc++;
        }
        if (*bc == 0) {
            return str;
        }
        str++;
    }
    return 0;
}

void NuUnicodeToAscii(char* dst, u16* src)
{
    s32 pos = 0;
    *dst = 0;
    if (src == 0) {
        return;
    }
    if (dst == 0) {
        return;
    }
    for (; src[pos] != 0; pos++) {
        if ((src[pos] & 0xff00) != 0) {
            switch (src[pos]) {
                case 0x2018:
                    dst[pos] = 0x91;
                    break;
                case 0x2019:
                    dst[pos] = 0x92;
                    break;
                case 0x2013:
                    dst[pos] = 0x96;
                    break;
                case 0x2026:
                    dst[pos] = 0x85;
                    break;
                case 0x2122:
                    dst[pos] = 0x99;
                    break;
                default:
                    dst[pos] = '?';
            }
        } else {
            dst[pos] = src[pos];
        }
    }
    dst[pos] = 0;
}

char* NuStrStr(char* str, char* sub)
{
    char* sc;
    char* bc;
    while (*str != 0) {
        sc = str;
        bc = sub;
        while (*bc != 0) {
            if (*sc == 0) {
                break;
            }
            if (*sc != *bc) {
                break;
            }
            sc++;
            bc++;
        }
        if (*bc == 0) {
            return str;
        }
        str++;
    }
    return 0;
}

s32 fn_800076C4(s32* p)
{
    return *p;
}

s32 fn_800076CC(s32* a, s32* b)
{
    if (b != 0) {
        return *b;
    }
    return *a;
}

s32 fn_800076E4(char* a, char* b, s32 n)
{
    s8 ca;
    s8 cb;
    s32 len;
    if (a == 0) {
        return -1;
    }
    if (b == 0) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        char* t = a;
        len = 0;
        while (*t != 0) {
            len++;
            t++;
        }
        n = len;
    } else if (n == -2) {
        char* t = b;
        len = 0;
        while (*t != 0) {
            len++;
            t++;
        }
        n = len;
    }
    do {
        ca = NuToUpper(*a);
        cb = NuToUpper(*b);
        if (ca > cb) {
            return 1;
        }
        if (ca < cb) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (ca != 0 && cb != 0 && n != 0);
    return 0;
}
