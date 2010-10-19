#ifndef MATRIX_H
#define  MATRIX_H 1

#define ALL_COLS_MASK ( ~((1<<7)|(1<<6)))

uint8_t read_col(void);
void unselect_rows(void);
void activate(uint8_t row);

static inline void init_cols(void)
{
  /* Columns are inputs */
  DDRB &= ~(1 << PB0);
  DDRB &= ~(1 << PB1);
  DDRB &= ~(1 << PB2);
  DDRB &= ~(1 << PB3);
  DDRB &= ~(1 << PB4);
  DDRB &= ~(1 << PB5);
  DDRB &= ~(1 << PB6);

  /* Enable pull-up resistors on inputs */
  PORTB |= (1 << PB0);
  PORTB |= (1 << PB1);
  PORTB |= (1 << PB2);
  PORTB |= (1 << PB3);
  PORTB |= (1 << PB4);
  PORTB |= (1 << PB5);
  PORTB |= (1 << PB6);
}
#endif
