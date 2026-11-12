#include <stddef.h>
extern char lbl_801AFFD4[];
extern int integerOutputCount __asm__("lbl_80409D34+0x280");
extern char *integerOutputPtr __asm__("lbl_80409D34+0x284");

void fn_80156314(char *str)
{
    int end = 0;
    int start;
    char temp;
    while (str[end])
        end++;
    start = 0;
    end--;
    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int fn_801562B8(char *buf, int len)
{
    if (len > 0) {
        len--;
        if ((signed char)buf[len] > '4') {
            do {
                buf[len] = '0';
                len--;
            } while (len > 0 && buf[len] == '9');
            if (buf[len] == '9')
                return 0;
            buf[len]++;
        }
    }
    return 1;
}

extern int _write(int, const void *, unsigned int);
extern char lbl_802F3604[];
extern int outputCount __asm__("lbl_80409D34+0x278");
extern char *outputPtr __asm__("lbl_80409D34+0x27c");
int fn_801568C4(int fd, const char *data, unsigned int len, int flush)
{
    unsigned int i;
    int result;
    if (flush == 1) {
        result = _write(fd, lbl_802F3604, outputCount);
        outputCount = 0;
        outputPtr = lbl_802F3604;
        return result;
    }
    for (i = 0; i < len; i++) {
        *outputPtr = data[i];
        outputPtr++;
        outputCount++;
        if (outputCount > 127) {
            result = _write(fd, lbl_802F3604, outputCount);
            outputPtr = lbl_802F3604;
            outputCount = 0;
            if (result == 0)
                return 0;
        }
    }
    return i;
}

extern char lbl_801E1FE8[];
void fn_80156360(unsigned int value, char *buf, int base)
{
    int neg = value >> 31;
    int i;
    char *digits;
    char digit;
    if (neg && base == 10)
        value = -(int)value;
    digits = lbl_801E1FE8 + 0x24;
    i = 0;
    do {
        digit = digits[value % base];
        value /= base;
        buf[i] = digit;
        i++;
    } while (value);
    if (neg)
        buf[i++] = '-';
    buf[i] = 0;
    fn_80156314(buf);
}

#include <stdio.h>
#include <stdarg.h>
extern int _vfprintf_r(struct _reent *, FILE *, const char *, va_list);
extern int _vfiprintf_r();
int vfprintf(FILE *fp, const char *fmt, va_list args)
{
    const char *p = fmt;
    if (*p) {
        do {
            if (*p == '%' && p[1]) {
                ++p;
                while ((signed char)*p <= '@' && p[1])
                    ++p;
                switch ((signed char)*p) {
                case 'E': case 'G': case 'L': case 'e': case 'f': case 'g':
                    return _vfprintf_r(fp->_data, fp, fmt, args);
                }
            }
        } while (*++p);
    }
    return _vfiprintf_r(fp->_data, fp, fmt, args);
}

extern char lbl_802F3684[];
#include <string.h>
#include <math.h>
extern char cvtBuffer[] __asm__("lbl_802F3480+0xe0");
char *fn_801563DC(int requested, signed char format, int trim, char *sign, double value)
{
    union { double d; unsigned int word[2]; } bits;
    unsigned int exponent, mantissa, low, negative;
    int precision, finalPrecision;
    int n;
    int decpt;
    int i, j, digit;
    char tmp;
    char *buf;
    double epsilon;
    double integer;
    bits.d = value;
    low = bits.word[1];
    mantissa = bits.word[0] & 0xfffff;
    exponent = bits.word[0] >> 20;
    negative = exponent & 0x800;
    exponent &= 0x7ff;
    if (exponent == 0x7ff) {
        if (!mantissa && !low) {
            if (negative)
                strcpy(cvtBuffer, "-Inf");
            else
                strcpy(cvtBuffer, "Inf");
        } else {
            strcpy(cvtBuffer, "NaN");
        }
        return cvtBuffer;
    }
    precision = requested;
    n = 0;
    if (value < 0.0) {
        if (sign) {
            *sign = '-';
            buf = cvtBuffer;
        } else {
            cvtBuffer[0] = '-';
            buf = cvtBuffer + 1;
        }
        value = -value;
    } else {
        buf = cvtBuffer;
    }
    if (value >= 1.0) {
        integer = floor(value);
        value -= integer;
        do {
            if (n > 162)
                break;
            digit = fmod(integer, 10.0);
            buf[n++] = digit + '0';
            integer -= digit;
            integer /= 10.0;
        } while (integer >= 1.0);
        for (i = 0, j = n - 1; i < j; i++, j--) {
            tmp = buf[i];
            buf[i] = buf[j];
            buf[j] = tmp;
        }
    }
    decpt = n;
    if (decpt == 0) {
        if (format != 'f') {
            if (value != 0.0) {
                value *= 10.0;
                if (value < 1.0) {
                    do {
                        value *= 10.0;
                        decpt--;
                        if (!(value < 1.0))
                            break;
                    } while (decpt > -1021);
                }
                decpt--;
            }
            if (value >= 1.0) {
                precision++;
                value /= 10.0;
            }
        } else {
            buf[n++] = '0';
        }
    }
    if (decpt < 0)
        finalPrecision = precision;
    else if (format == 'f')
        finalPrecision = precision + (decpt < 1 ? 1 : decpt);
    else
        finalPrecision = precision + 1;
    epsilon = 5.551115123125783e-17;
    while (1) {
        value *= 10.0;
        epsilon *= 10.0;
        digit = value;
        value -= digit;
        if (n < finalPrecision && value >= epsilon && value <= 1.0 - epsilon)
            buf[n++] = digit + '0';
        else
            break;
    }
    if (value >= 0.5)
        digit++;
    buf[n++] = digit + '0';
    while (n <= finalPrecision)
        buf[n++] = '0';
    if (!fn_801562B8(buf, finalPrecision + 1)) {
        buf[0] = '1';
        decpt++;
        finalPrecision++;
    }
    if (requested) {
        if (format == 'f') {
            j = 1;
            if (decpt > 0) j = decpt;
        } else {
            j = 1;
        }
        n = finalPrecision;
        if (n > j) {
            char *p = n + buf;
            do {
                n--;
                *p = p[-1];
                p--;
            } while (n > j);
        }
        buf[j] = '.';
    } else {
        finalPrecision--;
    }
    if (trim) {
        while (finalPrecision && buf[finalPrecision] == '0')
            finalPrecision--;
        if (buf[finalPrecision] == '.')
            finalPrecision--;
    }
    if (format != 'f') {
        buf[++finalPrecision] = format;
        if (decpt >= 0) {
            buf[++finalPrecision] = '+';
            if (decpt)
                decpt--;
            if (decpt <= 9)
                buf[++finalPrecision] = '0';
        } else {
            buf[++finalPrecision] = '-';
            if (decpt > -10)
                buf[++finalPrecision] = '0';
            decpt = -decpt;
        }
        finalPrecision++;
        fn_80156360(decpt, buf + finalPrecision, 10);
    } else {
        buf[finalPrecision + 1] = 0;
    }
    return cvtBuffer;
}

#define FLOATING_POINT
#define BUF 348
#define OUTPUT fn_801568C4
#define _VFPRINTF_R _vfprintf_r
#include "sn_vfprintf_core.h"
#undef FLOATING_POINT
#undef BUF
#undef OUTPUT
#undef _VFPRINTF_R

int fn_80157EB4(int fd, const char *data, unsigned int len, int flush)
{
    unsigned int i;
    int result;
    if (flush == 1) {
        result = _write(fd, lbl_802F3684, integerOutputCount);
        integerOutputCount = 0;
        integerOutputPtr = lbl_802F3684;
        return result;
    }
    for (i = 0; i < len; i++) {
        *integerOutputPtr = data[i];
        integerOutputPtr++;
        integerOutputCount++;
        if (integerOutputCount > 127) {
            result = _write(fd, lbl_802F3684, integerOutputCount);
            integerOutputPtr = lbl_802F3684;
            integerOutputCount = 0;
            if (result == 0)
                return 0;
        }
    }
    return i;
}

#define BUF 40
#define OUTPUT fn_80157EB4
#define _VFPRINTF_R _vfiprintf_r
#include "sn_vfprintf_core.h"
