#ifndef MATRIX_H
#define  MATRIX_H 1

#include <avr/io.h>


#define ALL_COLS_MASK ((1<<COLS)-1)  // 0x63 or all lower 6 bits

uint8_t read_col(void);
void unselect_rows(void);
void activate(uint8_t row);


// this must be called once before matrix_scan.
uint8_t read_col(void)
{
    return (PINB & ~((1<<6) | (1<<7)));
}

void unselect_rows(void) {
    DDRC  = 0x00;
    PORTC = 0x00;
}

void activate(uint8_t row)
{
    unselect_rows();
    // swap upper and lower ports to have left half first in matrix
    (row<4) ? (row+=4) : (row-=4);
    DDRC = (1<<row);
    return;
}
static inline void init_cols(void)
{

    /* Columns are inputs */
    DDRB  &= ((1<<6) | (1<<7)); // 192 or upper 2 bits

    /* Enable pull-up resistors on inputs */
    PORTB |= ((1<<6)-1);        // 63 or lower 6 bits

}
#endif
