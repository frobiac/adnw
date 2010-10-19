#ifndef MATRIX_H
#define  MATRIX_H 1

#include <avr/io.h>


#define ALL_COLS_MASK ( ~((1<<7)|(1<<6)))

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
	DDRB  &= ~((1<<6) | (1<<7));
	PORTB |= ~((1<<6) | (1<<7));
	/*
  DDRB &= ~(1 << PB0);
  DDRB &= ~(1 << PB1);
  DDRB &= ~(1 << PB2);
  DDRB &= ~(1 << PB3);
  DDRB &= ~(1 << PB4);
  DDRB &= ~(1 << PB5);
  DDRB &= ~(1 << PB6);

  /* Enable pull-up resistors on inputs */
  /*PORTB |= (1 << PB0);
  PORTB |= (1 << PB1);
  PORTB |= (1 << PB2);
  PORTB |= (1 << PB3);
  PORTB |= (1 << PB4);
  PORTB |= (1 << PB5);
  PORTB |= (1 << PB6);
  */
}
#endif
