/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2016 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#if   (COLS <= 8)
    typedef uint8_t  column_size_t;
#elif (COLS <= 16)
    typedef uint16_t column_size_t;
#elif (COLS <= 32)
    typedef uint32_t column_size_t;
#else
    #error "More than 32 columns are not supported"
#endif

/** @file matrix.h
 *
 *  This is a description of the hardware setup used in the BlueCube.
 *  Uses Teensy-2.0 .
 *
 */

#include "config.h"

// this must be called once before matrix_scan.
static inline column_size_t read_col(void)
{
    uint8_t resF = PINF;

    // flipped teensy 2.0: F 01  4567 for columns
#ifdef BLUECUBE
    return (( resF & 0b11) | ((resF & 0b11110000)>>2 ));
#endif

#ifdef REDTILT
    return (( resF & 0b11) | ((resF & 0b11110000)>>2 ));
#endif

#ifdef HYPERNANO
    return (
               ((resF & 0b00000001) << 5) |
               ((resF & 0b00000010) << 3) |
               ((resF & 0b00010000) >> 1) |
               ((resF & 0b00100000) >> 3) |
               ((resF & 0b01000000) >> 5) |
               ((resF & 0b10000000) >> 7)
           );
#endif

#ifdef HYPERMICRO
    // Columns 0..11:
    // D6 D4 D7 B4 B5 B6 D5 B3 B2 F4 F1 F0
    uint8_t resD = PIND;
    uint8_t resB = PINB;
    uint8_t resE = PINE;

    uint16_t res = (
                       (( resE & (1<<6)) >> 6) |
                       (( resD & (1<<4)) >> 3) |
                       (( resD & (1<<7)) >> 5) |
                       (( resB & (1<<4)) >> 1) |
                       (( resB & (1<<5)) >> 1) |
                       (( resB & (1<<6)) >> 1) |
                       (( resD & (1<<5)) << 1) |
                       (( resB & (1<<3)) << 4) |
                       (( resB & (1<<2)) << 6) |
                       (( resF & (1<<4)) << 5) |
                       (( resF & (1<<1)) << 9) |
                       (( resF & (1<<0)) << 11)
                   );
    return res;
#endif

}

static inline void unselect_rows(void)
{
#ifdef BLUECUBE
    DDRD  &= 0b00001011;
    PORTD &= 0b00001011;
    DDRB  &= 0b10001111;
    PORTB &= 0b10001111;
#endif

#ifdef REDTILT
    DDRD  &= 0b01110000;
    PORTD &= 0b01110000;
    DDRB  &= 0b10001111;
    PORTB &= 0b10001111;
#endif

#ifdef HYPERNANO
    DDRD  &= 0b00000000;
    PORTD &= 0b00000000;
#endif

#ifdef HYPERMICRO
    DDRB  &= 0b01111100;
    PORTB &= 0b01111100;
    DDRF  &= 0b01111111;
    PORTF &= 0b01111111;
#endif
}


static inline void activate(uint8_t row)
{
    unselect_rows();

#ifdef BLUECUBE
    // swap upper and lower ports to have left half first in matrix
    (row<4) ? (row+=4) : (row-=4);

    // B6 B5 B4 D7
    // D6 D4 D2 D5
    switch(row) {
        case 0: DDRB |= (1<<6); break;
        case 1: DDRB |= (1<<5); break;
        case 2: DDRB |= (1<<4); break;
        case 3: DDRD |= (1<<7); break;
        case 4: DDRD |= (1<<6); break;
        case 5: DDRD |= (1<<4); break;
        case 6: DDRD |= (1<<2); break;
        case 7: DDRD |= (1<<5); break;
    }
#endif

#ifdef REDTILT
    switch(row) {
        case 7: DDRB |= (1<<6); break;
        case 4: DDRB |= (1<<5); break;
        case 5: DDRB |= (1<<4); break;
        case 6: DDRD |= (1<<7); break;
        case 3: DDRD |= (1<<0); break;
        case 2: DDRD |= (1<<1); break;
        case 1: DDRD |= (1<<2); break;
        case 0: DDRD |= (1<<3); break;

    }
#endif

#ifdef HYPERNANO
    // Row 7 on pin 0
    DDRD |= (1<<(7-row));
#endif

#ifdef HYPERMICRO
    // Rows 0..3 = B0 B1 B7 F7
    switch(row) {
        case 3: DDRB  |= (1<<0); break;
        case 2: DDRB  |= (1<<1); break;
        case 1: DDRB  |= (1<<7); break;
        case 0: DDRF  |= (1<<7); break;
    }
#endif
}

static inline void init_cols(void)
{
#ifdef HYPERMICRO
    // Columns 0..11:
    // D6 D4 D7 B4 B5 B6 D5 B3 B2 F4 F1 F0
    // but had to change D6 -> E6 due to diode polarity used (cathode on round pad)

    DDRE  &= (0b10111111);
    PORTE |= (0b01000000);
    DDRD  &= (0b01001111);
    PORTD |= (0b10110000);
    DDRB  &= (0b10000011);
    PORTB |= (0b01111100);
    DDRF  &= (0b11101100);
    PORTF |= (0b00010011);
#else
    // teensy 2.0: 2&3 unused, F 01  4567
    /* Columns are inputs */
    DDRF  &= ((1<<2) | (1<<3)); // 192 or upper 2 bits
    /* Enable pull-up resistors on inputs */
    PORTF |= (0b11110011);
#endif
}

#endif
