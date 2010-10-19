#include <avr/io.h>

#include "matrix.h"


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
