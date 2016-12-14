#ifndef _MCP23018_H
#define _MCP23018_H
#include <stdbool.h>

/********MCP23018 pinout***/
/* GPIOA-COLS  |GPIOB-ROWS*/
/**************|***********/
/* GPA7 ?      |GPB7 ?    */
/* GPA6 col6   |GPB6 ?    */
/* GPA5 col5   |GPB5 row0 */
/* GPA4 col4   |GPB4 row1 */
/* GPA3 col3   |GPB3 row2 */
/* GPA2 col2   |GPB2 row3 */
/* GPA1 col1   |GPB1 rwo4 */
/* GPA0 col0   |GPB0 row5 */
/**************************/

// [1] http://ww1.microchip.com/downloads/en/DeviceDoc/22103a.pdf

// I2C address A2 A1 A0 are 000 in ergodox
// 0 1 0 0 A2 A1 A0 RW  from [1] section 1.4.2 Figure 1.6
// 0 1 0 0 is always the same for all MCP23018
// A2 A1 A0 are set to 0 by bringin the ADDR pin to low (hard wired)
// R = 1 / W = 0
#define TWI_MCP23018_CONTROLBYTEREAD  0b01000001 // 0 1 0 0 A2 A1 A0 RW  from [1] section 1.4.2 Figure 1.6
#define TWI_MCP23018_CONTROLBYTEWRITE 0b01000000 // 0 1 0 0 A2 A1 A0 RW  from [1] section 1.4.2 Figure 1.6

#define IODIRA 0x00 // From [1] Read Table 1-1 and 1.6.1 I/O DIRECTION REGISTER (page 18)
#define IODIRB 0x01 // From [1] Read Table 1-1 and 1.6.1 I/O DIRECTION REGISTER (page 18)
#define IOCONA 0x0A // From [1] Read Table 1-1 and 1.6.6 CONFIGURATION REGISTER
#define IOCONB 0x0B // From [1] Read Table 1-1 and 1.6.6 CONFIGURATION REGISTER
#define IOCON  IOCONA // From [1] Read Table 1-1 and 1.6.6 CONFIGURATION REGISTER
#define GPPUA  0x0C // From [1] Read Table 1-1 and 1.6.7 pull-up resistor configuration register
#define GPPUB  0x0D // From [1] Read Table 1-1 and 1.6.7 pull-up resistor configuration register
#define GPIOA  0x12 // From [1] Read Table 1-1 and 1.6.10 PORT REGISTER (page 28)
#define GPIOB  0x13 // From [1] Read Table 1-1 and 1.6.10 PORT REGISTER (page 28)
#define OLATA  0x14 // From [1] Read Table 1-1 and 1.6.11 OUTPUT LATCH REGISTER (page 29)
#define OLATB  0x15 // From [1] Read Table 1-1 and 1.6.11 OUTPUT LATCH REGISTER (page 29)


#define TWI100K 100000UL
#define TWI400K 400000UL

#define F_SCL TWI400K
#define USE_TWI_STOP
/* #define TWI_DEBUG */

/* MCP23018 I2C timing */
#define TGPVO  0.5 // 500ns Table 2-2
#define TGPIV  0.450 // 450ns Table 2-2

#if F_SCL==TWI100K
    //table 2-4
    #define TBUF   4.7   // 4.7us @ 100Khz // Bus free time between stop and start
#elif F_SCL==TWI400K
    //table 2-4
    #define TBUF   1.3 // 1.3us @ 400Khz
#else
    #error "F_SCL must be TWI100K or TWI400K"
#endif

static const uint8_t GPBROWS = _BV(0) | _BV(2) | _BV(3) | _BV(4) | _BV(5); // GPPU 1 means pullup enable and 0 means pullup disabled

/* low level TWI */
void twi_init(void);
bool twi_start(void);
bool twi_repstart(void);
void twi_stop(void);
bool twi_send(uint8_t data);
bool twi_read(uint8_t *data);
void twi_print_error(const char *data);

/* mcp23018 specific */
bool mcp23018_write_register(uint8_t reg, uint8_t data);
bool mcp23018_read_register(uint8_t reg, uint8_t *data);
bool mcp23018_init(void);
bool mcp23018_all_cols_highz(void);
bool mcp23018_col_low(uint8_t col);
bool mcp23018_check(void);
bool mcp23018_check_reg(uint8_t reg, const uint8_t data);
uint8_t mcp23018_read_rows(void);

#endif

