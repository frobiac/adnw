/*
    Copyright 2010-2013 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#ifndef TRACKPOINT_H
#define TRACKPOINT_H

#include "ps2mouse.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

//reset trackpoimt
#define RSTPORT B
#define RBIT    3


bool    tp_send_read_ack( uint8_t val );
void    tp_reset( void );
void    tp_init( void );
void    tp_id( void );
uint8_t tp_read_config(void);


#endif // TRACKPOINT_H
