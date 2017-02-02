/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2015-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
        fprintf(stderr, "\nNeed 5 args: \n   len type secret key tag\n\n");
        return(2) ;
    }

    uint8_t len    = atoi(argv[1]);
    uint8_t type   = atoi(argv[2]);

    char * secret  = argv[3];
    char * key     = argv[4]; // gui key in twik
    char * tag     = argv[5]; // private_key in twik config

    char password[26+0];
    uint8_t ret = passHash(password, len, type, secret, key, tag);

    if(ret==0)
        fprintf(stdout, "%s\n", password);
    else
        fprintf(stdout, "ERROR %d creating pw: %s\n", ret, (ret==3?"Invalid length [4-26]" : (ret==4 ? "Invalid type [1-3]" : "Unknown error" ) ) );

    return 0;
}

