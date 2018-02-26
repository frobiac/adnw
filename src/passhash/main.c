/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2015-2018 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "passhash.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main (int argc, char * argv[])
{
    if( argc != 6) {
        fprintf(stderr, "\nNeed 5 args: \n   len type private_key master_pw tag\n\n");
        return(2) ;
    }

    if(strlen(argv[4]) > PH_PW_LEN) {
        fprintf(stderr, "\nERROR: PW len %d>%d for\"%s\"", strlen(argv[4]), PH_PW_LEN, argv[4] );
        return 3;
    }

    if(strlen(argv[3]) >= PH_INPUT_LEN) {
        fprintf(stderr, "\nERROR: PK len %d>%d for\"%s\"", strlen(argv[3]), PH_INPUT_LEN, argv[3] );
        return 4;
    }

    fprintf(stderr, "\n------");
    char ph_result[PH_INPUT_LEN];
    memset(ph_result, 0, PH_INPUT_LEN);
    snprintf(ph_result, PH_INPUT_LEN, "%s", argv[3]);

#ifdef NO_PER_CHARACTER_MODE
    uint8_t len    = atoi(argv[1]);
    uint8_t type   = atoi(argv[2]);

    char master_pw[PH_PW_LEN+1];
    memset(master_pw, 0, PH_PW_LEN+1);
    snprintf(master_pw, PH_PW_LEN+1, "%s", argv[4]);

    char * tag     = argv[5]; // private_master_pw in twik config

    uint8_t ret = passHash(ph_result, master_pw, tag, len, type);
    if(ret==0)
        fprintf(stdout, "%s\n", ph_result);
    else
        fprintf(stdout, "\nERROR %d creating pw: %s", ret, (ret==3?"Invalid length [4-26]" : (ret==4 ? "Invalid type [1-3]" : "Unknown error" ) ) );

#else
    // assemble data from command line
    ph_setPrivateKey(ph_result);

    char input[PH_INPUT_LEN+PH_PW_LEN+1];
    strcpy(input, argv[4]);     // master pw
    int len=strlen(input); input[len]='\n'; input[len+1]=0;
    strcat(input, argv[5]);
    strcat(input, " ");
    strcat(input, argv[1]);
    strcat(input, " ");
    strcat(input, argv[2]);

    len=strlen(input); input[len]='\n'; input[len+1]=0;

    // fprintf(stderr, "\nWILL SEND %s|", input);
    //ph_reset();

    for(uint8_t i=0; i<strlen(input); ++i) {
        char c=input[i];
        uint8_t ph_state;
        {
            /// more or less same code as in subcommand
            ph_state = ph_parse(c);

            if( PH_READING == ph_state ) // not yet done reading, so stay in command mode and wait for next char.
                continue;

            // getting here means either
            // 1) password has been set (again)
            // 2) or all data was entered, so passhash has been already printed
            // 3) password had been cleared
            if( PH_PW_ENTERED != ph_state && PH_DONE != ph_state)
                fprintf(stderr, "\nERROR %d creating pw", ph_state);

            // could check state here for errors ...
            // if( PH_PW_CLEARED == ph_state || PH_PW_ENTERED == ph_state )
        }
    }
#endif

    return 0;
}

