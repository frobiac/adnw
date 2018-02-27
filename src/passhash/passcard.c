#include "passcard.h"
#include "b64.h"
#include "hmac-sha1.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __AVR__
    #include "../print.h"
#endif


//const char GITBLOB[]="$Id$";

void genPWs(char r, char c, char * res, uint8_t n, uint8_t dir_raw)
{
    // xprintf("genPW:%c %c %d %d\n",r,c,n,dir_raw);
    read_pattern_t dir;
    dir.raw=dir_raw;
    int h = dir.h > 1 ? -1 : dir.h;
    int v = dir.v > 1 ? -1 : dir.v;

    for(uint8_t i=0; i<n; ++i) {
        hmac_line((r-'a'+v*i*(dir.v_skip+1)+13)%13);
        res[i]=g_hmac_b64[(26+c-'a'+h*i*(dir.h_skip+1))%26];
        //xprintf("%c",res[i]);
    }
    res[n]='\0';

#ifndef __AVR__
    fprintf(stderr, "\n(%cx%c) #%d in %02x : ", r, c, n, dir);
    if(h>0) fprintf(stderr," >%d",  dir.h_skip);
    if(h<0) fprintf(stderr," <%d",  dir.h_skip);
    if(v>0) fprintf(stderr," v%d",  dir.v_skip);
    if(v<0) fprintf(stderr," ^%d",  dir.v_skip);
    fprintf(stderr," |");
#endif
}

void hmac_line(uint8_t idx)
{
    g_tag = "tag"; ///@TODO remove
    memset(g_hmac_b64, 0, 27);
    snprintf(g_hmac_b64, 27, "%s", g_tag);
    uint8_t len = strlen(g_tag);
    if(len>=27) len=26;
    g_hmac_b64[len] =  idx+'a';

#ifdef HMAC
#include "sha1.h"
    char sha1hash[SHA1_HASH_BITS/8];
    memset(sha1hash, 0, SHA1_HASH_BITS/8);
    sha1((void*)(&sha1hash), g_hmac_b64, 8*strlen(g_hmac_b64));
    b64enc((unsigned char*)sha1hash, SHA1_HASH_BITS/8, g_hmac_b64, sizeof(g_hmac_b64));
#else
    char sha1hash[HMAC_SHA1_BYTES];
    memset(sha1hash, 0, HMAC_SHA1_BYTES);
    hmac_sha1((void*)(&sha1hash), KEY, 8*strlen(KEY), g_hmac_b64, 8*strlen(g_hmac_b64));
    b64enc((unsigned char*)sha1hash, HMAC_SHA1_BYTES, g_hmac_b64, sizeof(g_hmac_b64));
#endif
    g_hmac_b64[26] = '\0'; // remove trailing '='
}

#ifndef __AVR__
// last character of msg is incremented from 'a'
void tabula_recta()
{
    printf("\ntag=%s", g_tag);
    for(int i=0; i<13; ++i) {
        printf("\n%c: ", i+'a');
        hmac_line(i);
        printf("%s", g_hmac_b64);
    }
}

#include <stdlib.h>
int main (int argc, char * argv[])
{
    if(argc==1) {
        runTests();
        return(0);
    }
    if( argc >= 3) {
        KEY = argv[2];
        g_tag = argv[1];
        if(argc==3) {
            tabula_recta();
            return(0);
        } else if(argc==7) {

            char r=argv[3][0];
            char c=argv[4][0];
            uint8_t n=atoi(argv[5]);
            char res[n];
            uint8_t dir_raw=atoi(argv[6]);
            genPWs(r,c,res, n, dir_raw);
            printf("%s", res);
            return(0);
        }
    }

    fprintf(stderr, "\nNeed 2 or 6 args: tag key [r c n dir]\n");
    return(-1);
}

void runTests()
{
    char *key="key";
    char *tag="tag";

    KEY=key;
    g_tag=tag;
    tabula_recta();
    fprintf(stdout, "\n");

    verifyPWs('a','a',4, 0x00, "cccc");
    verifyPWs('a','a',4, 0x01, "cdma");
    //  verifyPWs('a','a',4, 0x06, "FB7si");
    fprintf(stdout, "\nTests done.\n");

}

void verifyPWs(char r, char c, uint8_t n, uint8_t dir_raw, char * expect)
{
    printf(".");
    char res[n];
    genPWs(r,c,res, n,dir_raw);
    if( strcmp(res,expect)) {
        printf("\nERR: %s != %s", res, expect);
    }
}
#endif

