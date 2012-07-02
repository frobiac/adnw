#ifndef MATRIX_H
#define  MATRIX_H 1

#include <avr/io.h>


#define ALL_COLS_MASK ((1<<COLS)-1)  // 0x63 or all lower 6 bits

uint8_t read_col(bool swap);
void unselect_rows(void);
void activate(uint8_t row);


// this must be called once before matrix_scan.
uint8_t read_col(bool swap)
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

void unselect_rows(void)
{
    DDRD  &= 0b00001011;
    PORTD &= 0b00001011;
    DDRB  &= 0b10001111;
    PORTB &= 0b10001111;
}

void activate(uint8_t row)
{
    unselect_rows();
    // swap upper and lower ports to have left half first in matrix
    (row<4) ? (row+=4) : (row-=4);

    // B6 B5 B4 D7
    // D6 D4 D2 D5
    switch(row) {
    case 0:
        DDRB |= (1<<6);
        break;
    case 1:
        DDRB |= (1<<5);
        break;
    case 2:
        DDRB |= (1<<4);
        break;
    case 3:
        DDRD |= (1<<7);
        break;
    case 4:
        DDRD |= (1<<6);
        break;
    case 5:
        DDRD |= (1<<4);
        break;
    case 6:
        DDRD |= (1<<2);
        break;
    case 7:
        DDRD |= (1<<5);
        break;

    }
    return;
}
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
