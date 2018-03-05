#include "xxtea.h"

#ifndef __AVR__
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
#else

#endif


#define DELTA 0x9e3779b9

#define MX ((((z>>5)^(y<<2)) + ((y>>3)^(z<<4))) ^ ((sum^y) + (g_xxtea_key[(p&3)^e] ^ z)))


void xxtea_crypt(uint32_t *v, int8_t an /*, uint32_t const key[4]*/)
{
    /* n=2 -> 32 rounds
     * n=4 -> 19 rounds
     * n=8 -> 13 rounds
     */
    uint32_t y, z, sum;
    uint8_t p, rounds, e;

    if (an > 1) {
        // Coding Part
        rounds = 6 + 52/an;
        sum = 0;
        z = v[an-1];
        do {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p=0; p<an-1; p++) {
                y = v[p+1];
                z = v[p] += MX;
            }
            y = v[0];
            z = v[an-1] += MX;
        } while (--rounds);
    } // End of Encoding
    else if (an < -1) {
        // Decoding Part
        an = (uint8_t)(-an);
        rounds = 6 + 52/an;
        sum = rounds*DELTA;
        y = v[0];
        do {
            e = (sum >> 2) & 3;
            for (p=an-1; p>0; p--) {
                z = v[p-1];
                y = v[p] -= MX;
            }
            z = v[an-1];
            y = v[0] -= MX;
            sum -= DELTA;
        } while (--rounds);
    } // End of Decoding
}


// global g_xxtea_txt must be filled with data beforehand
void xxtea_fixed_encrypt(uint8_t pad_after)
{
    // add PKCS#7 padding
    for(uint8_t i=pad_after; i<XXTEA_DATA_LEN; ++i)
        g_xxtea_txt[i]= XXTEA_DATA_LEN-pad_after;
    xxtea_crypt((uint32_t*) g_xxtea_txt, XXTEA_DATA_LEN/4);
}

uint8_t xxtea_fixed_decrypt()
{
    xxtea_crypt((uint32_t*) g_xxtea_txt, -XXTEA_DATA_LEN/4);

    // remove padding
    uint8_t pad_len = g_xxtea_txt[XXTEA_DATA_LEN-1];
    if(pad_len>0 && pad_len<XXTEA_DATA_LEN) {
        for(int i = 0; i<pad_len; ++i) {
            if( g_xxtea_txt[XXTEA_DATA_LEN-1-i] != pad_len) {
                pad_len=0;
                break;
            }
        }
    } else {
        // no padding
        pad_len=0;
    }

    g_xxtea_txt[XXTEA_DATA_LEN-pad_len]= '\0';
    return XXTEA_DATA_LEN-pad_len;
}


#ifndef __AVR__
void printhex( const char * label, uint8_t * data, uint8_t len)
{
    printf("\n%-8s : ", label);
    for(uint8_t i=0; i<len; ++i)
        printf("%02x", data[i]);

}


// uint32_t g_xxtea_key[] = { 0x01234567, 0x89ABCDEF, 0x01234567, 0x89ABCDEF }; // 128 bit key

int main(int argc, char **argv)
{
    char * txt = "3333000033330000";
    uint8_t txtlen=strlen(txt);

    printhex( "ORIG: ", (uint8_t*)txt, txtlen);
    printf("  l=%d",txtlen);

    memcpy(g_xxtea_txt, txt, txtlen);

    xxtea_fixed_encrypt(txtlen);
    printhex( "ENC  : ", (uint8_t*)g_xxtea_txt, txtlen);
    uint8_t readlen = xxtea_fixed_decrypt();
    printhex( "DEC  : ", (uint8_t*)g_xxtea_txt, txtlen);
    printf(" rl=%d",readlen);



    // realloc(&txt, txtlen + (4-txtlen%4));
    if(0!=txtlen%4) {
        return -1;
    }

    //uint32_t del[4]={0x00, 0xffff, 0x00,0xfff};
    //char del[16]={ 1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//   unsigned char del[16]={ 1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,33,33};
//    strcpy(del, "abcd");

    /*
    char del[20];
    memset(del, 0, 20);
    del[1]='a';
    */
    /*
    char * del2= "abcd000033330000";
    uint8_t del[16];
    memcpy(del, del2, 16);
    */
    uint8_t del[15];
    strncpy(del, "ffffDDDD3333DDDD", 15);


    printhex( "ORIG: ", del, 4*4);
    xxtea_crypt((uint32_t*) del, 4);
    printhex( "ENC : ", del, 4*4);
    xxtea_crypt((uint32_t*) del, -4);
    printhex( "DEC : ", (uint8_t*)del, 4*4);
    xxtea_crypt((uint32_t*) &del[4], 3);
    printhex( "ENC : ", (uint8_t*)del, 4*4);
    xxtea_crypt((uint32_t*) &del[4], -3);
    printhex( "DEC : ", (uint8_t*)del, 4*4);

    printf("\n|%s|", del);
    //xxtea_crypt((uint32_t*) txt, (txtlen)/4);
    //printhex( "ENC : ", (uint8_t*)txt, txtlen);
    //xxtea_crypt((uint32_t*) txt, -((txtlen)/4));
    //printhex( "DECR: ", (uint8_t*)txt, txtlen);


    return 0;
}

#endif
