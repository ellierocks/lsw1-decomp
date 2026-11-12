#include <stddef.h>

extern char lbl_801AFFD4[];
extern void OSPanic(const char *, int, const char *, ...);

void *malloc(unsigned int size)
{
    unsigned int caller;
    __asm__ volatile("mflr %0" : "=r" (caller));
    OSPanic(NULL, 0, lbl_801AFFD4, caller - 4);
}

void free(void *ptr)
{
    unsigned int caller;
    __asm__ volatile("mflr %0" : "=r" (caller));
    OSPanic(NULL, 0, lbl_801AFFD4 + 0x80, caller - 4);
}

extern int isspace();
extern int isdigit();
extern int *__errno(void);
extern int exponentLimit __asm__("lbl_80409D34+0x274");
extern float powersOfTen[] __asm__("lbl_801E1FE8");
double strtod(const char *input, char **end)
{
    const char *p;
    const char *last;
    int expNegative = 0;
    int exponent = 0;
    int negative;
    int point;
    int fracExp;
    int count;
    int high;
    int low;
    int c;
    float value;
    float scale;
    float *power;
    p = input;
    while (isspace((unsigned char)*p))
        p++;
    if ((signed char)*p == '-') {
        negative = 1;
        p++;
    } else {
        if ((signed char)*p == '+')
            p++;
        negative = 0;
    }
    point = -1;
    count = 0;
    for (;; count++) {
        c = (signed char)*p;
        if (!isdigit(c)) {
            if (c != '.' || point >= 0)
                break;
            point = count;
        }
        p++;
    }
    last = p;
    p -= count;
    if (point < 0)
        point = count;
    else
        count--;
    if (count > 18) {
        fracExp = point - 18;
        count = 18;
    } else {
        fracExp = point - count;
    }
    if (count == 0) {
        p = input;
        value = 0.0f;
        goto done;
    }
    high = 0;
    while (count > 9) {
        c = (signed char)*p++;
        if (c == '.')
            c = (signed char)*p++;
        high = 10 * high + (c - '0');
        count--;
    }
    low = 0;
    while (count > 0) {
        c = (signed char)*p++;
        if (c == '.')
            c = (signed char)*p++;
        low = 10 * low + (c - '0');
        count--;
    }
    value = 1e9 * (double)high + (double)low;
    p = last;
    if ((signed char)*p == 'E' || (signed char)*p == 'e') {
        p++;
        if ((signed char)*p == '-') {
            expNegative = 1;
            p++;
        } else {
            if ((signed char)*p == '+')
                p++;
            expNegative = 0;
        }
        if (!isdigit((unsigned char)*p)) {
            p = last;
            goto done;
        }
        while (isdigit((unsigned char)*p)) {
            exponent = exponent * 10 + ((signed char)*p++ - '0');
        }
    }
    if (expNegative)
        exponent = fracExp - exponent;
    else
        exponent = exponent + fracExp;
    if (exponent < 0) {
        expNegative = 1;
        exponent = -exponent;
    } else {
        expNegative = 0;
    }
    if (exponent > exponentLimit) {
        exponent = exponentLimit;
        *__errno() = 34;
    }
    scale = 1.0f;
    power = powersOfTen;
    while (exponent) {
        if (exponent & 1)
            scale *= *power;
        power++;
        exponent >>= 1;
    }
    if (expNegative)
        value /= scale;
    else
        value *= scale;
done:
    if (end)
        *end = (char *)p;
    if (negative)
        return -value;
    return value;
}
