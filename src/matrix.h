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


// this must be called once before matrix_scan.
static inline uint8_t read_col(void)
{
#if defined(__AVR_ATmega32U4__)
    uint8_t res = PINF;
    // flipped teensy 2.0: F 01  4567
    return (( res & 0b11) | ((res & 0b11110000)>>2 ));
#elif defined(__AVR_AT90USB1286__)             // Teensy++ 2.0
    // teensy++ 2.0 n // PINF & 0x11
    return (PINF & ~((1<<6) | (1<<7)));
#endif
}

static inline void unselect_rows(void)
{
#if defined(__AVR_ATmega32U4__)
    DDRD  &= 0b00001011;
    PORTD &= 0b00001011;
    DDRB  &= 0b10001111;
    PORTB &= 0b10001111;
#elif defined(__AVR_AT90USB1286__)             // Teensy++ 2.0
#error "PINCONFIG for unselect_rows NOT defined for __AVR_AT90USB1286__ setup"
#endif
}

static inline void activate(uint8_t row)
{
#if defined(__AVR_ATmega32U4__)
    unselect_rows();
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
    return;

#elif defined(__AVR_AT90USB1286__)             // Teensy++ 2.0
#error "PINCONFIG for activate(row) NOT defined for __AVR_AT90USB1286__ setup"
#endif
}

/// @todo Why static inline only here ?
static inline void init_cols(void)
{
#if defined(__AVR_ATmega32U4__)
    // teensy 2.0: 2&3 unused, F 01  4567
    /* Columns are inputs */
    DDRF  &= ((1<<2) | (1<<3)); // 192 or upper 2 bits
    /* Enable pull-up resistors on inputs */
    PORTF |= (0b11110011);

#elif defined(__AVR_AT90USB1286__)
    // teensy++ 2.0 : 6&7 unused
    DDRF &= ((1<<6) | (1<<7));
    PORTF |= (0b00111111);
    return;
#endif

}
#endif
