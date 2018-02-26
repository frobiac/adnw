#include "xxtea.h"

#ifndef __AVR__
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
#else

#endif

#define DELTA 0x9e3779b9
#define MX ((((z>>5)^(y<<2)) + ((y>>3)^(z<<4))) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

void xxtea_crypt(uint32_t *v, int8_t n, uint32_t const key[4])
{

    uint32_t y, z, sum;
    uint32_t p, rounds, e, an;
    an = n;
    if (n > 1) {
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
    else if (n < -1) {
        // Decoding Part
        an = (uint32_t)-n;
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

#ifndef __AVR__
void printhex( const char * label, uint8_t * data, uint8_t len)
{
    printf("\n%-8s : ", label);
    for(uint8_t i=0; i<len; ++i)
        printf("%02x", data[i]);

}
#endif


// global g_xxtea_txt must be filled with data beforehand
void xxtea_fixed_encrypt(uint8_t len)
{
    // add PKCS#7 padding
    for(uint8_t i=len; i<XXTEA_DATA_LEN; ++i)
        g_xxtea_txt[i]= XXTEA_DATA_LEN-len;

    xxtea_crypt((uint32_t*) g_xxtea_txt, XXTEA_DATA_LEN/4, (const uint32_t*)(g_xxtea_key));
}

uint8_t xxtea_fixed_decrypt()
{
    xxtea_crypt((uint32_t*) g_xxtea_txt, -XXTEA_DATA_LEN/4, (const uint32_t*)(g_xxtea_key));

    // remove padding
    uint8_t pad = g_xxtea_txt[XXTEA_DATA_LEN-1];
    if(pad>0 && pad<XXTEA_DATA_LEN) {
        for(int i = 0; i<pad; ++i) {
            if( g_xxtea_txt[XXTEA_DATA_LEN-1-i] != pad) {
                pad=0;
                break;
            }
        }
    } else {
        // no padding
        pad=0;
    }

    g_xxtea_txt[XXTEA_DATA_LEN-pad]= '\0';
    return XXTEA_DATA_LEN-pad;
}


#ifndef __AVR__
int main(int argc, char **argv)
{

    if(argc==3) {
        char * key = argv[1];
        char * txt = argv[2];
        // bytes to longs
        //memset(g_xxtea_txt, 0, 16);

        memcpy((void*) g_xxtea_txt, txt, strlen(txt));
        memcpy((void*) g_xxtea_key, key, strlen(key));
        xxtea_fixed_encrypt(strlen(txt));
        xxtea_fixed_decrypt();

        return 0;
    }

    uint32_t tvec[] = { 0x633985cf, 0x5e1fe2a0, 0x4a059113, 0xc8894f09, // key
                        0xe31fa38a, 0x30d48e0b,   //txt
                        0x021917c1, 0xe172a0c6
                      }; //exp

    memcpy((void*) g_xxtea_key, (void*)(&tvec[0]), 4*4);
    memcpy((void*) g_xxtea_txt, (void*)(&tvec[4]), 2*4);
    xxtea_fixed_encrypt(2*4);
    xxtea_fixed_decrypt();


    char * key = "Top Sec Key Yes!";
    char * txt = "Text to encrypt - does this work123";

    /// TEST VECTORS:
    // test vector, no padding.
    uint32_t tvector[] = { 0x633985cf, 0x5e1fe2a0, 0x4a059113, 0xc8894f09, // key
                           0xe31fa38a, 0x30d48e0b,   //txt
                           0x021917c1, 0xe172a0c6
                         }; //exp

    uint8_t tlen = (sizeof(tvector)/sizeof(uint32_t) -4 )/2;
    xxtea_crypt(&tvector[4], tlen, &tvector[0]);

    bool ok=true;
    for(int i=0; i<tlen; ++i) {
        if ( tvector[4+i] != tvector[4+i+tlen] ) {
            printf("\nERROR: 0x%08lx != 0x%08lx",  tvector[4+i], tvector[4+i+tlen] );
            ok=false;
        }
    }
    if(ok)
        printf(".");
    printf("\n");


    return 0;
}

#endif
