#include "types.h"

extern s32 lbl_80409970;
extern s32 lbl_8040A758;
extern u8* lbl_8040A75C;
extern s32 lbl_8040A764;
extern f32 lbl_801ABE30[];
extern f32 lbl_801ABE34[];
extern f32 lbl_801ABE38[];
extern f32 lbl_801ABE74[];
extern f32 lbl_801ABE78[];
extern f32 lbl_801ABE7C[];
extern f32 lbl_801ABE3C[];
extern f32 lbl_801ABE40[];
extern f32 lbl_801ABE44[];
extern f32 lbl_801ABE48[];
extern f32 lbl_801ABE4C[];
extern f32 lbl_801ABE50[];
extern f32 lbl_801ABE54[];
extern f32 lbl_801ABE58[];
extern f32 lbl_801ABE5C[];
extern f32 lbl_801ABE60[];
extern f32 lbl_801ABE64[];
extern f32 lbl_801ABE68[];
extern f32 lbl_801ABE6C[];
extern f32 lbl_801ABE70[];
extern s32 lbl_8040A768;
extern s32 lbl_8040B250;

void fn_8011AA1C(void)
{
    *(s32*)(lbl_8040B250 + 4) = (*(s32*)(lbl_8040B250 + 4) | 0x10);
}

void fn_8011AC0C(f32 x)
{
    *(f32*)(lbl_8040B250 + 104) = x;
}

void fn_8011AC18(f32 x)
{
    *(f32*)(lbl_8040B250 + 108) = x;
}

void fn_8011AC24(f32 x)
{
    *(f32*)(lbl_8040B250 + 112) = x;
}

void fn_8011ADB8(void)
{
    *(s32*)(lbl_8040B250 + 4) = (*(s32*)(lbl_8040B250 + 4) | 0x20);
}

void fn_8011ADCC(void)
{
    *(s32*)(lbl_8040B250 + 4) = (*(s32*)(lbl_8040B250 + 4) | 0x20000);
}

void fn_8011ADE0(void)
{
    *(s32*)(lbl_8040B250 + 4) = (*(s32*)(lbl_8040B250 + 4) | 0x200);
}

void fn_8011AF2C(f32 x)
{
    *(f32*)(lbl_8040B250 + 152) = x;
}

void fn_8011AF38(s32 a)
{
    *(u8*)(lbl_8040B250 + 156) = a;
}

void fn_8011AF44(s32 a)
{
    *(u8*)(lbl_8040B250 + 157) = a;
}

void fn_8011AF50(s32 a)
{
    *(u8*)(lbl_8040B250 + 158) = a;
}

void fn_8011AF5C(s32 a)
{
    *(u8*)(lbl_8040B250 + 159) = a;
}

void fn_8011AF68(s32 a)
{
    *(u8*)(lbl_8040B250 + 160) = a;
}

void fn_8011AF74(s32 a)
{
    *(u8*)(lbl_8040B250 + 161) = a;
}

void fn_8011AF80(s32 a)
{
    *(u8*)(lbl_8040B250 + 162) = a;
}

void fn_8011AF8C(s32 a)
{
    *(u8*)(lbl_8040B250 + 163) = a;
}

void fn_8011AF98(f32 x)
{
    *(f32*)(lbl_8040B250 + 164) = x;
}

void fn_8011AFA4(void)
{
    *(s32*)(lbl_8040B250 + 4) = (*(s32*)(lbl_8040B250 + 4) | 0x8000);
}

void fn_8011AFB8(void)
{
    *(s32*)(lbl_8040B250 + 4) = (*(s32*)(lbl_8040B250 + 4) | 0x10000);
}

void fn_8011AFCC(void)
{
    *(s32*)(lbl_8040B250 + 4) = (*(s32*)(lbl_8040B250 + 4) | 0x40000);
}

void fn_8011AFE0(f32 x)
{
    *(f32*)(lbl_8040A758 + 40) = x;
}

void fn_8011B020(f32 x)
{
    *(f32*)(lbl_8040A758 + 48) = x;
}

void fn_8011B02C(f32 x)
{
    *(f32*)(lbl_8040A758 + 52) = x;
}

void fn_8011B038(f32 x)
{
    *(f32*)(lbl_8040A758 + 56) = x;
}

void fn_8011B708(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x4);
}

void fn_8011B71C(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x8);
}

void fn_8011B730(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x10);
}

void fn_8011B744(void)
{
    *(s32*)(lbl_8040A758 + 4) = ((*(s32*)(lbl_8040A758 + 4) | 0x80000) | 0x10);
}

void fn_8011B75C(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x30);
}

void fn_8011B770(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x50);
}

void fn_8011B784(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x80);
}

void fn_8011B798(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x200000);
}

void fn_8011B7AC(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x400000);
}

void fn_8011B7C0(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x1000000);
}

void fn_8011B7D4(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x800000);
}

void fn_8011B7E8(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x1000);
}

void fn_8011B7FC(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x100);
}

void fn_8011B810(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x200);
}

void fn_8011B824(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x20000);
}

void fn_8011B838(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x40000);
}

void fn_8011B84C(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x400);
}

void fn_8011B860(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x800);
}

void fn_8011B874(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x2000);
}

void fn_8011B888(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x4000);
}

void fn_8011B89C(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x8000);
}

void fn_8011B8B0(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x10000);
}

void fn_8011B8C4(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x100000);
}

void fn_8011B8D8(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x2000000);
}

void fn_8011B8EC(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x4000000);
}

void fn_8011B900(void)
{
    *(s32*)(lbl_8040A758 + 4) = (*(s32*)(lbl_8040A758 + 4) | 0x8000000);
}

void fn_8011BE44(f32 x)
{
    *(f32*)(lbl_8040A768 + 12) = x;
}

void fn_8011BE50(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x20);
}

void fn_8011BE80(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x40);
}

void fn_8011BE94(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x8);
}

void fn_8011BEA8(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x4);
}

void fn_8011BF44(s32 a)
{
    *(u8*)(lbl_8040A768 + 8) = a;
}

void fn_8011BF50(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x80);
}

void fn_8011BF64(void)
{
    *(s32*)(lbl_8040A768 + 60) = ((*(s32*)(lbl_8040A768 + 60) | 0x80) ^ 0x80);
}

void fn_8011BF7C(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x100);
}

void fn_8011BF90(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x200);
}

void fn_8011BFA4(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x1000);
}

void fn_8011BFB8(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x4000);
}

void fn_8011BFCC(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x400);
}

void fn_8011BFE0(void)
{
    *(s32*)(lbl_8040A768 + 60) = (*(s32*)(lbl_8040A768 + 60) | 0x2000);
}

void fn_8011F118(void)
{
    lbl_80409970 = 0;
}

void fn_8011A9FC(void)
{
    volatile s32* p = (volatile s32*)(lbl_8040B250 + 4);
    *p |= 0x10;
    *p ^= 0x10;
}

void fn_8011B318(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 16) = x;
    }
}

void fn_8011B330(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 20) = x;
    }
}

void fn_8011B348(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 24) = x;
    }
}

void fn_8011B360(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 28) = x;
    }
}

void fn_8011B398(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 36) = x;
    }
}

void fn_8011B3B0(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 40) = x;
    }
}

void fn_8011B3C8(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 44) = x;
    }
}

void fn_8011B3E0(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 52) = x;
    }
}

void fn_8011B3F8(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 64) = x;
    }
}

void fn_8011B410(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 76) = x;
    }
}

void fn_8011B5D0(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 84) = x;
    }
}

void fn_8011B5E8(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 88) = x;
    }
}

void fn_8011B600(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 92) = x;
    }
}

void fn_8011BA2C(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 96) = x;
    }
}

void fn_8011BBBC(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 132) = x;
    }
}

void fn_8011BBD4(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 136) = x;
    }
}

void fn_8011BBEC(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 140) = x;
    }
}

void fn_8011BC04(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 144) = x;
    }
}

void fn_8011B914(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 0;
    }
}

void fn_8011B930(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 1;
    }
}

void fn_8011B94C(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 2;
    }
}

void fn_8011B968(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 3;
    }
}

void fn_8011B984(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 4;
    }
}

void fn_8011B9A0(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 5;
    }
}

void fn_8011B9BC(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 6;
    }
}

void fn_8011B9D8(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 7;
    }
}

void fn_8011B9F4(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 8;
    }
}

void fn_8011BA10(void)
{
    if (lbl_8040A764 != 0) {
        lbl_8040A75C[229] = 9;
    }
}

void fn_8011B044(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[224] = a;
    }
}

void fn_8011B06C(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[204] = a;
    }
}

void fn_8011B094(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[205] = a;
    }
}

void fn_8011B0BC(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[206] = a;
    }
}

void fn_8011B0E4(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[207] = a;
    }
}

void fn_8011B10C(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[208] = a;
    }
}

void fn_8011B134(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[209] = a;
    }
}

void fn_8011B15C(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[220] = a;
    }
}

void fn_8011B1A4(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[221] = a;
    }
}

void fn_8011B1CC(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[223] = a;
    }
}

void fn_8011B1F4(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[222] = a;
    }
}

void fn_8011B21C(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[201] = a;
    }
}

void fn_8011B244(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[202] = a;
    }
}

void fn_8011B26C(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[203] = a;
    }
}

void fn_8011B618(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[218] = a;
    }
}

void fn_8011B640(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[219] = a;
    }
}

void fn_8011B668(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[225] = a;
    }
}

void fn_8011B690(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[226] = a;
    }
}

void fn_8011B6B8(s32 a)
{
    if (lbl_8040A764 != 0 && a >= 0 && a <= 15) {
        lbl_8040A75C[227] = a;
    }
}

void fn_8011B184(s32 a)
{
    if (lbl_8040A764 != 0 && a > 0) {
        lbl_8040A75C[195] = a;
    }
}

void fn_8011B378(f32 x)
{
    if (lbl_8040A764 != 0) {
        u8* p = lbl_8040A75C;
        *(f32*)(p + 32) = x;
        *(f32*)(p + 40) = x;
        *(f32*)(p + 44) = x;
    }
}

void fn_8011B2C8(f32 x)
{
    if (lbl_8040A764 != 0 && x <= *(f32*)(lbl_8040A75C + 12)) {
        *(f32*)(lbl_8040A75C + 8) = x;
    }
}

void fn_8011B2F0(f32 x)
{
    if (lbl_8040A764 != 0 && x >= *(f32*)(lbl_8040A75C + 8)) {
        *(f32*)(lbl_8040A75C + 12) = x;
    }
}

void fn_8011B574(f32 x)
{
    if (lbl_8040A764 != 0 && x > lbl_801ABE3C[0]) {
        *(f32*)(lbl_8040A75C + 104) = x;
    }
}

void fn_8011B6E0(f32 x)
{
    if (lbl_8040A764 != 0 && x > lbl_801ABE40[0]) {
        *(f32*)(lbl_8040A75C + 152) = x;
    }
}

void fn_8011BA44(f32 x)
{
    if (lbl_8040A764 != 0 && x > lbl_801ABE44[0]) {
        *(f32*)(lbl_8040A75C + 148) = x;
    }
}

void fn_8011BAA0(f32 x)
{
    if (lbl_8040A764 != 0) {
        if (x < lbl_801ABE48[0]) {
            x = lbl_801ABE48[0];
        }
        *(f32*)(lbl_8040A75C + 100) = x;
    }
}

void fn_8011BACC(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 112) = x * lbl_801ABE4C[0] / lbl_801ABE50[0];
    }
}

void fn_8011BAFC(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 116) = x * lbl_801ABE54[0] / lbl_801ABE58[0];
    }
}

void fn_8011BB2C(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 120) = x * lbl_801ABE5C[0] / lbl_801ABE60[0];
    }
}

void fn_8011BB5C(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 124) = x * lbl_801ABE64[0] / lbl_801ABE68[0];
    }
}

void fn_8011BB8C(f32 x)
{
    if (lbl_8040A764 != 0) {
        *(f32*)(lbl_8040A75C + 128) = x * lbl_801ABE6C[0] / lbl_801ABE70[0];
    }
}

void fn_8011B59C(s32 a)
{
    if (lbl_8040A764 != 0) {
        if (a > 255) {
            a = 255;
        } else if (a < 0) {
            a = 0;
        }
        lbl_8040A75C[194] = a;
    }
}

void fn_8011BA6C(s32 a)
{
    if (lbl_8040A764 != 0) {
        if (a > 255) {
            a = 255;
        } else if (a < 0) {
            a = 0;
        }
        lbl_8040A75C[196] = a;
    }
}

void fn_8011BC54(f32 x)
{
    if (x > *(f32*)(lbl_8040A75C + 176)) {
        *(f32*)(lbl_8040A75C + 176) = x;
    }
}

void fn_8011BE64(void)
{
    if (*(s32*)(lbl_8040A768 + 60) & 32) {
        *(s32*)(lbl_8040A768 + 60) ^= 32;
    }
}

void fn_8011AD58(f32 x)
{
    if (x < lbl_801ABE30[0]) {
        x = lbl_801ABE30[0];
    }
    *(f32*)(lbl_8040B250 + 128) = x;
}

void fn_8011AD78(f32 x)
{
    if (x < lbl_801ABE34[0]) {
        x = lbl_801ABE34[0];
    }
    *(f32*)(lbl_8040B250 + 132) = x;
}

void fn_8011AD98(f32 x)
{
    if (x < lbl_801ABE38[0]) {
        x = lbl_801ABE38[0];
    }
    *(f32*)(lbl_8040B250 + 136) = x;
}

void fn_8011BEE4(f32 x)
{
    if (x < lbl_801ABE74[0]) {
        x = lbl_801ABE74[0];
    }
    *(f32*)(lbl_8040A768 + 20) = x;
}

void fn_8011BF04(f32 x)
{
    if (x < lbl_801ABE78[0]) {
        x = lbl_801ABE78[0];
    }
    *(f32*)(lbl_8040A768 + 24) = x;
}

void fn_8011BF24(f32 x)
{
    if (x < lbl_801ABE7C[0]) {
        x = lbl_801ABE7C[0];
    }
    *(f32*)(lbl_8040A768 + 28) = x;
}

void fn_8011AD30(s32 a)
{
    if (a > 255) {
        a = 255;
    } else if (a < 0) {
        a = 0;
    }
    *(u8*)(lbl_8040B250 + 66) = a;
}

void fn_8011BEBC(s32 a)
{
    if (a > 254) {
        a = 254;
    } else if (a < 0) {
        a = 0;
    }
    *(u8*)(lbl_8040A768 + 17) = a;
}

void fn_8011B494(s32 a)
{
    u8* q = lbl_8040A75C + 197;
    if (lbl_8040A764 != 0) {
        if (a > 255) {
            a = 255;
        } else if (a < 0) {
            a = 0;
        }
        *q = a;
    }
}

void fn_8011B4CC(s32 a)
{
    u8* q = lbl_8040A75C + 198;
    if (lbl_8040A764 != 0) {
        if (a > 255) {
            a = 255;
        } else if (a < 0) {
            a = 0;
        }
        *q = a;
    }
}

void fn_8011B504(s32 a)
{
    u8* q = lbl_8040A75C + 199;
    if (lbl_8040A764 != 0) {
        if (a > 255) {
            a = 255;
        } else if (a < 0) {
            a = 0;
        }
        *q = a;
    }
}

void fn_8011B53C(s32 a)
{
    u8* q = lbl_8040A75C + 200;
    if (lbl_8040A764 != 0) {
        if (a > 255) {
            a = 255;
        } else if (a < 0) {
            a = 0;
        }
        *q = a;
    }
}

void CUT_CreateCharacterInstance(void *arg0, s32 *arg1) {
    if (((s32) (*(u8 *)((s8 *)(arg0) + (0x5F))) != 0xFF) && ((*(u8 *)((s8 *)(arg0) + (0x5C))) & 2)) {
        *arg1 = (*(s32 *)((s8 *)(arg0) + (0x50)));
    }
}

extern void *lbl_8040B254;

void fn_8011AA7C(void) {
    s32 var_ctr;
    s32 var_r8;

    (*(u8 *)((s8 *)(lbl_8040B250) + (0x43))) = 0U;
    if ((s32 *) (*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))) != NULL) {
        var_r8 = 0;
        var_ctr = 0x20;
        do {
            (*(u8 *)((s8 *)((var_r8 + *(*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))))) + (0x68))) = (u8) (*(u8 *)((s8 *)(lbl_8040B250) + (0x43)));
            var_r8 += 0x104;
            var_ctr -= 1;
        } while (var_ctr != 0);
    }
}

extern void *lbl_8040B254;

void fn_8011AACC(void) {
    s32 var_ctr;
    s32 var_r8;

    (*(u8 *)((s8 *)(lbl_8040B250) + (0x43))) = 1U;
    if ((s32 *) (*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))) != NULL) {
        var_r8 = 0;
        var_ctr = 0x20;
        do {
            (*(u8 *)((s8 *)((var_r8 + *(*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))))) + (0x68))) = (u8) (*(u8 *)((s8 *)(lbl_8040B250) + (0x43)));
            var_r8 += 0x104;
            var_ctr -= 1;
        } while (var_ctr != 0);
    }
}

extern void *lbl_8040B254;

void fn_8011AB1C(void) {
    s32 var_ctr;
    s32 var_r8;

    (*(u8 *)((s8 *)(lbl_8040B250) + (0x43))) = 2U;
    if ((s32 *) (*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))) != NULL) {
        var_r8 = 0;
        var_ctr = 0x20;
        do {
            (*(u8 *)((s8 *)((var_r8 + *(*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))))) + (0x68))) = (u8) (*(u8 *)((s8 *)(lbl_8040B250) + (0x43)));
            var_r8 += 0x104;
            var_ctr -= 1;
        } while (var_ctr != 0);
    }
}

extern void *lbl_8040B254;

void fn_8011AB6C(void) {
    s32 var_ctr;
    s32 var_r8;

    (*(u8 *)((s8 *)(lbl_8040B250) + (0x43))) = 3U;
    if ((s32 *) (*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))) != NULL) {
        var_r8 = 0;
        var_ctr = 0x20;
        do {
            (*(u8 *)((s8 *)((var_r8 + *(*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))))) + (0x68))) = (u8) (*(u8 *)((s8 *)(lbl_8040B250) + (0x43)));
            var_r8 += 0x104;
            var_ctr -= 1;
        } while (var_ctr != 0);
    }
}

extern void *lbl_8040B254;

void fn_8011ABBC(void) {
    s32 var_ctr;
    s32 var_r8;

    (*(u8 *)((s8 *)(lbl_8040B250) + (0x43))) = 4U;
    if ((s32 *) (*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))) != NULL) {
        var_r8 = 0;
        var_ctr = 0x20;
        do {
            (*(u8 *)((s8 *)((var_r8 + *(*(s32 **)((s8 *)(lbl_8040B254) + (0x2DC))))) + (0x68))) = (u8) (*(u8 *)((s8 *)(lbl_8040B250) + (0x43)));
            var_r8 += 0x104;
            var_ctr -= 1;
        } while (var_ctr != 0);
    }
}

extern f32 lbl_801ABE28[];
extern f32 lbl_801ABE2C[];

void fn_8011AD00(f32 farg0) {
    f32 var_f0;
    f32 var_f1;

    var_f1 = farg0;
    var_f0 = lbl_801ABE28[0];
    if ((var_f1 < var_f0) || (var_f0 = lbl_801ABE2C[0], ((var_f1 > var_f0) != 0))) {
        var_f1 = var_f0;
    }
    (*(f32 *)((s8 *)(lbl_8040B250) + (0x7C))) = var_f1;
}

extern s32 lbl_8040A764;

void fn_8011BC6C(s32 arg0) {
    s32 var_r3;

    var_r3 = arg0;
    if ((s32) lbl_8040A764 != 0) {
        if (var_r3 > 0x2710) {
            var_r3 = 0x2710;
        } else if (var_r3 < 0) {
            var_r3 = 0;
        }
        (*(s16 *)((s8 *)(lbl_8040A75C) + (0xB8))) = (s16) ((var_r3 / 10) * 0xA);
    }
}

void fn_8011C13C(s32 arg0) {
    (*(s16 *)((s8 *)(lbl_8040A768) + (0xA))) = (s16) ((arg0 << 0x10) / 360);
}

extern f32 lbl_801ACAAC[];
extern f32 lbl_801ACAB0[];
extern u8 lbl_803E028C[];
extern f32 lbl_80408E9C;
extern void *lbl_80409410;
extern f32 lbl_8040993C;
extern s32 lbl_8040AFAC;
extern s32 lbl_8040B08C;

void fn_8011E134(void) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 var_f12;

    if ((((s32) lbl_8040AFAC != 0) && !((*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (4))) & 0x4000)) || (((s32) (*(s32 *)((s8 *)(lbl_80409410) + (0x19C))) != 3) && (((s8) lbl_803E028C[0] == 5) || (((s8) lbl_803E028C[0] == 2) && ((s32) lbl_8040B08C != 0))))) {
        var_f12 = lbl_801ACAAC[0];
    } else {
        var_f12 = lbl_801ACAB0[0];
    }
    if (lbl_8040993C > var_f12) {
        temp_f0 = lbl_8040993C - (2.0f * lbl_80408E9C);
        lbl_8040993C = temp_f0;
        if (temp_f0 < var_f12) {
            lbl_8040993C = var_f12;
        }
    } else if (lbl_8040993C < var_f12) {
        temp_f0_2 = lbl_8040993C + (2.0f * lbl_80408E9C);
        lbl_8040993C = temp_f0_2;
        if (temp_f0_2 > var_f12) {
            lbl_8040993C = var_f12;
        }
    }
}

s32 fn_8009D160(s32 *, s32 *, s32, s32);
void fn_8011F124(void);
extern s32 lbl_804081A8;
extern s32 lbl_80409970;

void fn_8011EB18(void) {
    if (((s32) lbl_804081A8 != 0) && ((s32) lbl_80409970 == 0)) {
        lbl_80409970 = 1;
        fn_8009D160(fn_8011F124, fn_8011F118, 0, 0);
    }
}

s32 NuGCutSceneSysInit(s32 *);
s32 fn_8003F178(s32 *);
s32 fn_8003F180(s32 *);
s32 fn_8003F188(s32 *);
s32 fn_8003F190(s32 *);
s32 fn_8003F198(s32 *);
s32 fn_8003F1A0(s32 *);
extern s32 CUT_FindCharacters[];
extern s32 fn_8011C4B8[];
extern s32 fn_8011D30C[];
extern s32 fn_8011EF88[];
extern s32 fn_8011EFE4[];
extern s32 lbl_801E0CE0[];

void fn_8011E864(void) {
    NuGCutSceneSysInit(lbl_801E0CE0);
    fn_8003F178(fn_8011C4B8);
    fn_8003F188(CUT_FindCharacters);
    fn_8003F198(CUT_CreateCharacterInstance);
    fn_8003F190(fn_8011EF88);
    fn_8003F180(fn_8011EFE4);
    fn_8003F1A0(fn_8011D30C);
}

f32 fn_8000ED40();

void fn_8011BC1C(void)
{
  f32 temp_f1;
  s8 *new_var;
  temp_f1 = fn_8000ED40();
  new_var = (s8 *) lbl_8040A75C;
  *((f32 *) (((s8 *) lbl_8040A75C) + 0xB0)) = temp_f1;
  if ((*((f32 *) (new_var + 0xB4))) < temp_f1)
  {
    *((f32 *) (new_var + 0xB4)) = temp_f1;
  }
}

s32 (*fn_80009970(s32 *, s32))();
s32 fn_8003B5F8();
extern s32 lbl_801ACAD4[];
extern s32 lbl_8040AE04;
extern s32 str_SourceFile_Cut[];

void fn_8011F124(void) {
    if ((s32) lbl_804081A8 != 0) {
        fn_80009970(str_SourceFile_Cut, 0xE0E)(lbl_801ACAD4, lbl_8040AE04);
        fn_8003B5F8();
    }
}

f32 fn_8000ED40();

void fn_8011AFEC(void) {
    f32 temp_f1;

    temp_f1 = fn_8000ED40();
    {
        s8 *lbl_8040A758_p = (s8 *)lbl_8040A758;
        (*(f32 *)(lbl_8040A758_p + (0x30))) = (f32) -temp_f1;
        (*(f32 *)(lbl_8040A758_p + (0x2C))) = temp_f1;
        (*(f32 *)(lbl_8040A758_p + (0x34))) = temp_f1;
    }
}
