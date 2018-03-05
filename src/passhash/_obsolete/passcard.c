#include "passcard.h"
#include "b64.h"
#include "hmac-sha1.h"


#ifdef __AVR__
    #include "../print.h"
#else
    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
#endif


//const char GITBLOB[]="$Id$";

void genPWs(char r, char c, char * res, uint8_t n, uint8_t dir_raw)
{
    read_pattern_t dir;
    dir.raw=dir_raw;
    int h = dir.h > 1 ? -1 : dir.h;
    int v = dir.v > 1 ? -1 : dir.v;

    if(h==0 && v==0) {
        //circle
        hmac_line((26+r-'a'-1)%13);
        res[0]=g_b64[(52+c-'a'+1)%26];
        res[6]=g_b64[(52+c-'a'-1)%26];
        res[7]=g_b64[(52+c-'a'+0)%26];
        hmac_line((26+r-'a'+0)%13);
        res[1]=g_b64[(52+c-'a'+1)%26];
        res[5]=g_b64[(52+c-'a'-1)%26];
        hmac_line((26+r-'a'+1)%13);
        res[2]=g_b64[(52+c-'a'+1)%26];
        res[3]=g_b64[(52+c-'a'-0)%26];
        res[4]=g_b64[(52+c-'a'-1)%26];
    } else {
        // xprintf("genPW:%c %c %d %d\n",r,c,n,dir_raw);
        for(uint8_t i=0; i<n; ++i) {
            hmac_line((r-'a'+v*i*(dir.v_skip+1)+26)%13);
            res[i]=g_b64[(52+c-'a'+h*i*(dir.h_skip+1))%26];

            //uint8_t row=((r-'a'+v*i*(dir.v_skip+1)+26)%13);
            //uint8_t col=(52+c-'a'+h*i*(dir.h_skip+1))%26;
            //fprintf(stderr, "%dx%d,",row, col);
        }
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

    memset(g_b64, 0, 27);
    memcpy(&g_b64[1], g_tag, 27);
    g_b64[0] =  idx+'a';


    char sha1hash[HMAC_SHA1_BYTES];
    memset(sha1hash, 0, HMAC_SHA1_BYTES);
    hmac_sha1((void*)(&sha1hash), g_pw, 8*strlen(g_pw), g_b64, 8*strlen(g_pw));
    b64enc((unsigned char*)sha1hash, HMAC_SHA1_BYTES, g_b64, 27);

    g_b64[26] = '\0'; // remove trailing '='
}





#ifndef __AVR__

// last character of msg is incremented from 'a'
void tabula_recta()
{
    printf("\ntag=%s", g_tag);
    for(int i=0; i<13; ++i) {
        printf("\n%c: ", i+'a');
        hmac_line(i);
        printf("%s", g_b64);
    }
    printf("\n");
}

int main (int argc, char * argv[])
{
    if(argc==1) {
        runTests();
        return(0);
    }
    if( argc >= 3) {
        snprintf(g_pw, 20, "%s", argv[2]);
        snprintf(g_tag, 27, "%s", argv[1]);
        //g_pw = argv[2];
        //g_tag = argv[1];
        if(argc==3) {
            tabula_recta();
            return(0);
        } else if(argc==7) {
            tabula_recta();

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

    snprintf(g_pw, 20, "%s", key);
    snprintf(g_tag, 27, "%s", tag);

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

