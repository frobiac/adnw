#include "b64.h"
#include <string.h>

// #define B64ALGO B64FUNC
// #define B64ALGO B64ARRAY
#define B64ALGO 3

#define B64FUNC  1
#define B64ARRAY 2
#define B64BITF  3


#if B64ALGO == B64FUNC
char b64( uint8_t i )
{
    if(i<26)
        return 'A'+i;
    if(i<52)
        return 'a'+ i-26;
    if(i<62)
        return '0'+ i-52;
    if(i==62)
        return '+';
    if(i==63)
        return '/';

    // should never get here!!!
    return '-';
}

/// encode 3 8-bit binary bytes as 4 '6-bit' characters
void encodeblock( unsigned char *in, unsigned char *out, int len )
{

    out[0] = (unsigned char) b64( (uint8_t)(in[0] >> 2) );
    out[1] = (unsigned char) b64( (uint8_t)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) );
    out[2] = (unsigned char) (len > 1 ? b64( (uint8_t)(((in[1] & 0x0f) << 2) | (((len>2?in[2]:0) & 0xc0) >> 6)) ) : '=');
    out[3] = (unsigned char) (len > 2 ? b64( (uint8_t)(in[2] & 0x3f) ) : '=');
    /*
     b64_t data;
     *data.bin = *in;
     out[0]=b64(data.c.c0);
     out[1]=b64(data.c.c1);
     out[2]=b64(data.c.c2);
     out[3]=b64(data.c.c3);
     */
    /*
    // 50 smaller, but non-standard
    out[0] = b64( (in[0] >> 2) );
    out[1] = b64( (in[1] >> 2) );
    out[2] = b64( (in[2] >> 2) );
    out[3] = b64( ((in[0] & 0x3)<<4) |((in[1] & 0x3)<<2) |((in[2] & 0x3)));
    */
}


#elif B64ALGO == B64ARRAY
static const char b64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/// encode 3 8-bit binary bytes as 4 '6-bit' characters
void encodeblock( unsigned char *in, unsigned char *out, int len )
{
    out[0] = (unsigned char) b64[ (uint8_t)(in[0] >> 2) ];
    out[1] = (unsigned char) b64[ (uint8_t)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
    out[2] = (unsigned char) (len > 1 ? b64[ (uint8_t)(((in[1] & 0x0f) << 2) | (((len>2?in[2]:0) & 0xc0) >> 6)) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ (uint8_t)(in[2] & 0x3f) ] : '=');
}

#elif B64ALGO == B64BITF

typedef union {
    struct {
        // could be 8/16/32/8: Each element must be larger as the fields it is made of
        uint32_t c0:6;
        uint32_t c1:6;
        uint32_t c2:6;
        uint32_t c3:6;
    } ;
    //uint8_t bin[3];
    uint8_t bin[4];
} b64_t;


char b64( uint8_t i )
{
    if(i<26)
        return 'A'+i;
    if(i<52)
        return 'a'+ i-26;
    if(i<62)
        return '0'+ i-52;
    if(i==62)
        return '+';
    if(i==63)
        return '/';

    // should never get here!!!
    return '-';
}
/*
void encodeblock( unsigned char *in, unsigned char *out, int len )
{
#define CORRECTLEN
    b64_t t;

        t.bin[2]=in[0];
#ifdef CORRECTLEN
        t.bin[1]=(len>1?in[1]:0);
        t.bin[0]=(len>2?in[2]:0);
#else
        t.bin[1]=in[1];
        t.bin[0]=in[2];
#endif


        //memcpy(t.bin, &inp[i], 3);
        //t.bin = inp[i] << 16 | inp[i+1] <<8 | inp[i+2];
        out[3]=b64(t.c0);
        out[2]=b64(t.c1);
        out[1]=b64(t.c2);
        out[0]=b64(t.c3);

#ifdef CORRECTLEN
        if(len<3)
            out[3]='=';
        if(len<2)
            out[2]='=';
#endif

}
*/
void b64enc(const unsigned char* data, size_t len, char* result, size_t resultSize)
{
    size_t x;
    b64_t t;
    uint8_t * d=t.bin;

    for(x=0; x+3<=len; x+=3) {
        //t.bin[2]=data[x+0]; t.bin[1]=data[x+1]; t.bin[0]=data[x+2];

        //memcpy(&(t.bin[0]), &data[x], 3);
        memcpy(d, &data[x], 3);
        result[x+3]=b64(t.c0); result[x+2]=b64(t.c1); result[x+1]=b64(t.c2); result[x+0]=b64(t.c3);
    }

    if(len%3==2) {
        t.bin[2]=data[x+0]; t.bin[1]=data[x+1]; t.bin[0]=0;
        result[x+3]=b64(t.c0); result[x+2]=b64(t.c1); result[x+1]=b64(t.c2); result[x+0]='=';
    } else if(len%3==1) {
        t.bin[2]=data[x+0]; t.bin[1]=0; t.bin[0]=0;
        result[x+3]=b64(t.c0); result[x+2]=b64(t.c1); result[x+1]='='; result[x+0]='=';
    }


}


#else

#error B64FUNC invalid/not defined.
//void encodeblock( unsigned char *in, unsigned char *out, int len ){};
//char b64(uint8_t i) { return 'x';};

#endif

#if B64ALGO != B64BITF

/** Encode data of size len to base64 result
 *  Result must be at least len*4.0/3 (rounded up!)
 *
 *  A SHA1 hash of 20 Bytes will thus occupy 27 Bytes (20*4/3=26.666)
 */
void b64enc(const unsigned char* data, size_t len, char* result, size_t resultSize)
{
    size_t x;
    for(x=0; x*3<len; ++x)
        encodeblock( (unsigned char *)(&data[x*3]), (unsigned char *)(&result[x*4]), (len-x*3));
}
#endif

