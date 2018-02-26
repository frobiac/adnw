#pragma once

#include <stdint.h>

void xxtea_crypt(uint32_t *v, int8_t n, uint32_t const key[4]);
void xxtea_fixed_encrypt(uint8_t len);
uint8_t xxtea_fixed_decrypt(void);

// Fixed size for txt of length XXTEA_DATA_LEN and key length of 16
#define XXTEA_DATA_LEN 4*5
uint8_t g_xxtea_txt[XXTEA_DATA_LEN+1];
uint8_t g_xxtea_key[16];



// cat /dev/urandom | head -c 16 | xxd -p | sed 's/\(.\{8\}\)/0x\1, /g'
// 0x3116ebd5, 0x954a6e60, 0x38a437de, 0x7316c773

// cat /dev/urandom | head -c 16 | xxd -p | sed 's/\(..\)/\\x\1/g'
/*
    #define HEX "\x00\x01\x39\x38\x37"
    uint8_t dat[8];
    memcpy( dat, HEX, 5);
    for(int i=0; i<5; ++i)
        printf("%02x", dat[i]);
*/


