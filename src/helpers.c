/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2017-2018 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


#include "helpers.h"

/**
 *  Creates hash from string.
 *
 *  ASCII printable: 33-126 -> 94 chars
 *
 */
uint32_t str2hash(const char * str)
{
    uint8_t i=0;
    uint32_t result = 0;
    do {
        result += str[i]-33;
        result *= 94;
        //printf("\n - s[%d]=%c -> %ld", i, str[i], result);
    } while(str[++i] != '\0' && i<12);
    return result;
}

char mapAscii(uint8_t tmp)
{
    // 256=3*85 + 1 -> 23 specials are beneficial for equal distribution as alnum is 62 chars
    // Ignore special case 255 below, 0 is going to be more likely (4/85 instead of 3/85)
    const char special[24] = "!#$%&()*+,-./:;<=>?@[]_";
    const uint8_t s_special = sizeof(special)-1;

    tmp = tmp % (62+s_special); // make digits more, special less frequent
    if(tmp<10)
        tmp+='0';
    else if(tmp<36)
        tmp+='a'-10;
    else if(tmp<62)
        tmp+='A'-36;
    else if(tmp<62+s_special)
        tmp=special[tmp-62];

    return (char)tmp; //     printf("%c", c);
}

