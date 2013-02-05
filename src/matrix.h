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


#ifndef MATRIX_H
#define  MATRIX_H

/** @file matrix.h
 *
 *  This is a description of the hardware setup used in the Maltron keyboard
 *  Uses Teensy-2.0++ / at90usb1286
 *
 */

#include "config.h"

// this must be called once before matrix_scan.
static inline uint16_t read_col(void)
{
    // 12 cols ordered left to right: PIND:0123457 and PINF:34567
    uint16_t resLow=PIND & 0b10111111;
    uint16_t resHigh=PINF;
    uint16_t resHigh1=(resHigh & 0b11110000)<<4;
    uint16_t resHigh0=(resHigh & 0b00001000)<<3;
    uint16_t result = (resHigh1 | resHigh0 | resLow);
    return result;
}

static inline void unselect_rows(void)
{
    // B:76543210 in use
    // set all row pins as inputs
    DDRB  &= 0b00000000;
    // drive all row pins low
    PORTB &= 0b00000000;
}


static inline void activate(uint8_t row)
{
    unselect_rows();
    // B:76543210 in use
    // set current row pin as output
    if (row<8) {
        DDRB |= (1<<row);
    }
    return;
}

static inline void init_cols(void)
{
    // 12 cols ordered left to right: PIND:0123457 and PINF:34567
    // set all column pins as inputs
    DDRD  &= 0b00000000;
    DDRF  &= 0b00000111;
    /* Enable pull-up resistors on inputs */
    PORTD |= (0b11111111);
    PORTF |= (0b11111000);
    return;
}

#endif
