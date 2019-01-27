/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2019 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#include <util/atomic.h>

#include "config.h"
#include "matrix.h"
#include "keyboard_class.h"

#ifdef BLACKBOWL
#include "mcp23018.h"

// these are the initially tested hardwired addresses that should work
// 0x46 -> 3 on chip=left side, 0x4E -> 7 on chip (VCC) = right side
static uint8_t i2c_adr[2] = {0x46, 0x4E};
static bool i2c_left = 0;

// this must be called once before matrix_scan.
static inline column_size_t read_col(void)
{
    return mcp23018_read_rows();
}


// '0' for used columns
static inline void unselect_rows(void)
{
    mcp23018_all_cols_highz();
}


static inline void activate(uint8_t row)
{
    unselect_rows();

    if(row<4) {
        if(i2c_left) {
            i2c_left=!i2c_left;
            mcp23018_init_addr(i2c_adr[i2c_left]);
        }
        mcp23018_col_low(3-row);
    } else {
        if(!i2c_left) {
            i2c_left=!i2c_left;
            mcp23018_init_addr(i2c_adr[i2c_left]);
        }
        mcp23018_col_low(7-row);
    }
}

/**
 * Only called once during keyboard initialization.
 */
bool init_cols(void)
{
    twi_init();
    _delay_ms(500);

    // detect both attached MCP23018 which should be on addresses +3 and +7
    if( mcp23018_init_addr(i2c_adr[0]) && mcp23018_init_addr(i2c_adr[1]) ) {
        return true;
    }
    _delay_ms(500);

    i2c_adr[0] = 0xFF;
    i2c_adr[1] = 0xFF;
    uint8_t retries = 5;
    do {
        uint8_t adr;
        for (adr=0x40; adr<0x50; adr+=2) {
            if(adr==i2c_adr[0])
                continue; // first adress was already found.
            if(mcp23018_init_addr(adr)) {
                i2c_adr[i2c_left] = adr;
                i2c_left = !i2c_left;
            }
            _delay_ms(50);
        }
        _delay_ms(250);
    } while (i2c_adr[1] == 0xFF && --retries>0 );

    if(i2c_adr[1] == 0xFF) // did not find second one
        return false;

    return true;
}


/*********************************************
 * END OF BLACKBOWL I2C MATRIX SCANNING
 *********************************************/
#else
/*********************************************
 * NON I2C SCANNING
 *********************************************/

// this must be called once before matrix_scan.
inline column_size_t read_col(void)
{
    uint8_t resF __attribute__((unused)) = PINF;

    // flipped teensy 2.0: F 01  4567 for columns
#ifdef BLUECUBE
    return (( resF & 0b11) | ((resF & 0b11110000)>>2 ));
#endif

#ifdef REDTILT
    return (( resF & 0b11) | ((resF & 0b11110000)>>2 ));
#endif

#ifdef BLACKFLAT
    // Only 5 columns on F_01456
    return (( resF & 0b11) | ((resF & 0b01110000)>>2 ));
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


// '0' for used columns
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

#ifdef BLACKFLAT
    // C_67
    DDRC  &= 0b00111111;
    PORTC &= 0b00111111;
    // D_25
    DDRD  &= 0b11011011;
    PORTD &= 0b11011011;
    // F_7
    DDRF  &= 0b01111111;
    PORTF &= 0b01111111;
    // B_456
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

#ifdef BLACKFLAT
    // swap upper and lower ports to have left half first in matrix
//    (row<4) ? (row+=4) : (row-=4);

    // B6 B5 B4 D7
    // D6 D4 D2 D5
    switch(row) {
        case 0: DDRB |= (1<<6); break;
        case 1: DDRB |= (1<<5); break;
        case 2: DDRB |= (1<<4); break;
        case 3: DDRF |= (1<<7); break;
        case 4: DDRD |= (1<<2); break;
        case 5: DDRC |= (1<<6); break;
        case 6: DDRC |= (1<<7); break;
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

/**
 * Only called once during keyboard initialization.
 */
bool init_cols(void)
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
#elif defined(BLACKFLAT) // only 5 columns -> F7 unused
    // teensy 2.0: 2&3 unused, F 01  456
    /* Columns are inputs */
    DDRF  &= ((1<<2) | (1<<3) | (1<<7));
    /* Enable pull-up resistors on inputs */
    PORTF |= (0b01110011);

#else // REDTILT, BLUECUBE, ???
    // teensy 2.0: 2&3 unused, F 01  4567
    /* Columns are inputs */
    DDRF  &= ((1<<2) | (1<<3)); // 192 or upper 2 bits
    /* Enable pull-up resistors on inputs */
    PORTF |= (0b11110011);
#endif

    return true;
}
#endif // non-I2C




/// debounce variables
volatile column_size_t kb_state[ROWS];    // debounced and inverted key state: bit = 1: key pressed
volatile column_size_t kb_press[ROWS];    // key press detect
volatile column_size_t kb_release[ROWS];  // key release detect
volatile column_size_t kb_rpt[ROWS];      // key long press and repeat

static column_size_t ct0[ROWS], ct1[ROWS];
static int32_t rpt[ROWS];

#define ALL_COLS_MASK ((1<<COLS)-1)  // 0x63 or all lower 6 bits
#define REPEAT_MASK    ALL_COLS_MASK // repeat: key0 = 0x3F = 63
#define REPEAT_START   31            // 61 = 1000ms
#define REPEAT_NEXT    15


column_size_t get_kb_release( column_size_t key_mask, uint8_t col)
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        key_mask &= kb_release[col];                      // read key(s)
        kb_release[col] ^= key_mask;                      // clear key(s)
    }
    return key_mask;
}

column_size_t get_kb_press( column_size_t key_mask, uint8_t col )
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        key_mask &= kb_press[col];                      // read key(s)
        kb_press[col] ^= key_mask;                      // clear key(s)
    }
    return key_mask;
}

column_size_t get_kb_rpt( column_size_t key_mask, uint8_t col )
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        key_mask &= kb_rpt[col];                        // read key(s)
        kb_rpt[col] ^= key_mask;                        // clear key(s)
    }
    return key_mask;
}

/** The real hardware access take place here.
 *  Each of the rows is individually activated and the resulting column value read.
 *  Should more than 8 channels be needed, this can easily be extended to 16/32bit.
 *  By means of a neat routine found on http://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/
 *
 */
void scan_matrix(void)
{
    column_size_t i, data;

    for (uint8_t row = 0; row < ROWS; ++row) {
        activate(row);

        // Insert NOPs for synchronization
#ifndef HAS_I2C
        _delay_us(20);
#endif
        // Place data on all column pins for active row
        // into a single 8/16/32 bit value.
        data = read_col();
        /// @see top comment for source of debounce magic
        // Needs to be adjusted for more than 8 columns
        i = kb_state[row] ^ (~data);                    // key changed ?
        ct0[row] = ~( ct0[row] & i );                   // reset or count ct0
        ct1[row] = ct0[row] ^ (ct1[row] & i);           // reset or count ct1
        i &= ct0[row] & ct1[row];                       // count until roll over ?
        kb_state[row] ^= i;                             // then toggle debounced state

        kb_press  [row] |=  kb_state[row] & i;          // 0->1: key press detect
        kb_release[row] |= ~kb_state[row] & i;          // 1->0: key release detect

        if( (kb_state[row] & REPEAT_MASK) == 0 ) {      // check repeat function
            rpt[row] = idle_count + REPEAT_START;       // start delay
        }
        if(  rpt[row] <= idle_count ) {
            rpt[row] = idle_count + REPEAT_NEXT;        // repeat delay
            kb_rpt[row] |= kb_state[row] & REPEAT_MASK;
        }

        // Now evaluate results
        column_size_t p,r,h;
        p=get_kb_press  (ALL_COLS_MASK, row);
        h=get_kb_rpt    (ALL_COLS_MASK, row);
        r=get_kb_release(ALL_COLS_MASK, row);

        if(p|h|r)
            keyChange(row, p,h,r);
    }
}

