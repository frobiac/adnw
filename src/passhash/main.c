#include "passhash.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

int main (int argc, char * argv[])
{
    if( argc != 6) {
        fprintf(stderr, "\nNeed 5 args: \n   len type secret key tag\n\n");
        return(2) ;
    }

    uint8_t len    = atoi(argv[1]);
    uint8_t type   = atoi(argv[2]);

    char * secret  = argv[3];
    char * key     = argv[4]; // gui key in twik
    char * tag     = argv[5]; // private_key in twik config

    char password[len+1];
    uint8_t ret = passHash(password, len, type, secret, key, tag);

    if(ret==0)
        fprintf(stdout, "%s\n", password);
    else
        fprintf(stdout, "ERROR creating pw: %d\n", ret);

    return 0;
}

