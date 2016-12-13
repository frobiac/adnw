#include <util/twi.h>
#include <stdbool.h>
#include <stddef.h> // size_t
// #include "../dbg.h"
#include "mcp23018.h"
#include <util/delay.h>
#include <math.h>

#undef TWI_DEBUG

#ifdef print
    #undef print
#endif
#ifdef printf
    #undef printf
#endif

#define print(...) ((void)0)
#define printf(...) ((void)0)

/* [1] ATMega32u4 datasheet  http://www.atmel.com/images/7766s.pdf*/
/* [2] Atmel page on ATMega32u4 http://www.atmel.com/devices/atmega32u4.aspx */
/* [3] ATMega32u4 manual http://www.atmel.com/Images/doc7766.pdf */

static void mcp23018_stopcond(void); // just to be used internally in this file
static bool mcp23018_startcond(void); // just to be used internally in this file
static bool mcp23018_repstartcond(void); // just to be used internally in this file
static bool mcp23018_sendbyte(uint8_t data);
static bool mcp23018_readbyte_nack(uint8_t *data);

static bool _twi_read(uint8_t *data);

static uint8_t  TWI_MCP23018_CONTROLBYTEWRITE;
static uint8_t  TWI_MCP23018_CONTROLBYTEREAD;

void twi_init(void)
{
    TWSR = 0; //prescaler 0
    TWBR = (F_CPU / F_SCL) / 2; // TWI @ 100Khz or TWI @ 400Khz
}

bool twi_start(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* START condition. See [3] See Section 20.7 page 234 */
    loop_until_bit_is_set(TWCR, TWINT); /* hardware sets TWINT when status is avaible in TWSR*/
    switch(TW_STATUS) {
        case TW_START:
            return true;
        case TW_REP_START: //repeated start
            print("TWI/IC2 got a REPEATED START when we were expecting START\n");
            return false;
        default:
            print("TWI/IC2 START condition failed\n");
            return false;
    }
    return false;
}

bool twi_repstart(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* START condition. See [3] See Section 20.7 page 234 */
    loop_until_bit_is_set(TWCR, TWINT); /* hardware sets TWINT when status is avaible in TWSR*/
    switch(TW_STATUS) {
        case TW_START:
            print("got a START when we were trying a REPEATED START\n");
            return false;
        case TW_REP_START: //repeated start
            return true;
        default:
            print("TWI/IC2 START condition failed\n");
            return false;
    }
    return false;
}

#ifdef USE_TWI_STOP

void twi_stop(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* STOP condition */
    loop_until_bit_is_clear(TWCR, TWSTO);
    return;
}
#endif

bool twi_send(uint8_t data)
{
    TWDR = data; /* TWDR hold the data to be transmitted */
    TWCR = _BV(TWINT) | _BV(TWEN); /* clear TWINT to initiate transmision */
    loop_until_bit_is_set(TWCR, TWINT); /* hardware sets TWINT when status is avaible in TWSR*/
    switch(TW_STATUS) {
        case TW_MT_SLA_ACK:  //SLA+W transmitted, ACK received
        case TW_MT_DATA_ACK: //data transmitted, ACK received
        case TW_MR_SLA_ACK:  //SLA+R transmitted, ACK received
            return true;
        default:
            printf("TWI/I2C: twi_send failed (%x)\n", TW_STATUS);
            return false;
    }
    return false;
}

bool twi_read_nack(uint8_t *data)
{
    TWCR = _BV(TWINT) | _BV(TWEN); /* no TWEA mean NACK */
    return _twi_read(data);
}

bool twi_read_ack(uint8_t *data)
{
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); /* TWEA to send ACK*/
    return _twi_read(data);
}

bool _twi_read(uint8_t *data)
{
    /* TWCR must be already set by the caller */
    loop_until_bit_is_set(TWCR, TWINT); /* hardware sets TWINT when status is avaible in TWSR*/
    switch(TW_STATUS) {
        case TW_MR_DATA_ACK: //SLA+W transmitted, ACK received
        case TW_MR_DATA_NACK: //SLA+W transmitted, ACK received
            *data = TWDR;
            return true;
        default:
            goto fail;
    }
    return true;
fail:
    twi_print_error("twi_read failed");
    return false;
}

void twi_print_error(const char *data)
{
    char *errorCode __attribute__((unused)); // nothing in debug
    switch(TW_STATUS) {
        case TW_MR_DATA_NACK: errorCode = "TW_MR_DATA_NACK"; break;
        case TW_BUS_ERROR: errorCode = "TW_BUS_ERROR"; break;
        case TW_MT_ARB_LOST: errorCode = "TW_MT_ARB_LOST / TW_MR_ARB_LOST"; break;
        default:
            errorCode = "unknown";
    }
    printf("%s (%s)", data, errorCode);
}

#ifdef USE_TWI_STOP
void mcp23018_stopcond(void)
{
#ifdef TWI_DEBUG
    print("mcp23018 stopcond\n");
#endif
    twi_stop();
    _delay_us(TBUF);
}
#endif

static bool mcp23018_startcond(void)
{
#ifdef TWI_DEBUG
    print("mcp23018_startcond\n");
#endif
    if(twi_start()) {
        return true;
    }
    return false;
}

static bool mcp23018_repstartcond(void)
{
#ifdef TWI_DEBUG
    print("mcp23018_repstartcond\n");
#endif
    if(twi_repstart()) {
        return true;
    }
    return false;
}

/*
 * Sends a byte over TWI/I2C but taking into account
 * the waiting  times that MCP23018 imposes
 */
static bool mcp23018_sendbyte(uint8_t data)
{
#ifdef TWI_DEBUG
    print("mcp23018_sendbyte\n");
#endif
    bool result = twi_send(data);
    //_delay_us(THDDAT);
    return result;
}

static bool mcp23018_readbyte_nack(uint8_t *data)
{
#ifdef TWI_DEBUG
    print("mcp23018_readbyte\n");
#endif
    bool result = twi_read_nack(data);
    return result;
}

bool mcp23018_write_register(uint8_t reg, uint8_t data)
{
#ifdef TWI_DEBUG
    print("mcp23018_write_register\n");
#endif
    if (!mcp23018_startcond()) return false; // if START fails, no stopcond
    if (!mcp23018_sendbyte(TWI_MCP23018_CONTROLBYTEWRITE)) goto fail; // to address the mcp23018
    if (!mcp23018_sendbyte(reg)) goto fail; // first we send the register bytes
    if (!mcp23018_sendbyte(data)) goto fail;  // then the value we want to store in that register
#ifdef USE_TWI_STOP
    mcp23018_stopcond();
#endif
#ifdef TWI_DEBUG
    mcp23018_check_reg(reg,data);
#endif
    return true;
fail:
    printf("MCP23018: error trying to write register (%x) with (%x)\n", reg, data);
#ifdef USE_TWI_STOP
    mcp23018_stopcond();
#endif
    return false;
}

bool mcp23018_read_register(uint8_t reg, uint8_t *data)
{
#ifdef TWI_DEBUG
    print("mcp23018_read_register\n");
#endif
    if (!mcp23018_startcond()) return false; // if START fails, without stop cond
    if (!mcp23018_sendbyte(TWI_MCP23018_CONTROLBYTEWRITE)) goto fail; // to address the mcp23018
    if (!mcp23018_sendbyte(reg)) goto fail; // first we tell which register we want to read
    if (!mcp23018_repstartcond()) return false;   // then we change the mode to READ
    if (!mcp23018_sendbyte(TWI_MCP23018_CONTROLBYTEREAD)) goto fail;
    if (!mcp23018_readbyte_nack(data)) goto fail; // last byte read = NACK
#ifdef USE_TWI_STOP
    mcp23018_stopcond();
#endif
    return true;
fail:
    printf("MCP23018: error trying to read register (%x)\n", reg);
#ifdef USE_TWI_STOP
    mcp23018_stopcond();
#endif
    return false;
}

bool mcp23018_init_addr(uint8_t addr )
{
    // addr is 0x40 for MCP23018 + 3 hw address bits << 1 plus 0x01 for read
    TWI_MCP23018_CONTROLBYTEWRITE = addr;
    TWI_MCP23018_CONTROLBYTEREAD  = addr+1; // 0b1000001 | (addr<<1);
    return mcp23018_init();
}

bool mcp23018_init()
{


#ifdef TWI_DEBUG
    print("mcp23018 init\n");
#endif
    //bring all GPIO pins to high impedance (inputs without pull-up resistors)  this is the default
    if (!mcp23018_write_register(IOCON,0x00)) goto fail;
    if (!mcp23018_write_register(GPPUA, 0x00)) goto fail; //GPA0-A7 no pull up
    if (!mcp23018_write_register(GPPUB, 0x00)) goto fail; //GPA0-A7 no pull up
    if (!mcp23018_write_register(IODIRA,0xFF)) goto fail; // IODIR 0 = output | 1 = input
    if (!mcp23018_write_register(IODIRB,0xFF)) goto fail; // IODIR 0 = output | 1 = input
    return true;
fail:
    print("mcp23018_init failed\n");
    return false;
}

/* GPA7  | GPA6  | GPA5  | GPA4  | GPA3  | GPA2  | GPA1  | GPA0  */
/* highz | highz | highz | highz | highz | highz | highz | highz */
bool mcp23018_all_cols_highz(void)
{
#ifdef TWI_DEBUG
    print("mcp23018_all_cols_highz\n");
#endif
    if (!mcp23018_write_register(GPPUA,0x00)) goto fail;  // GPPU 0 = pullup disabled
    if (!mcp23018_write_register(IODIRA,0xFF)) goto fail; // IODIR 0 = output | 1 = input
    _delay_us(TGPVO); /* tGPOV 500ns */
    return true;
fail:
    print("mcp23018_all_cols_highz failed\n");
    return false;
}



/********MCP23018 pinout***/
/* GPIOA-COLS  |GPIOB-ROWS*/
/**************|***********/
/* GPA7 ?      |GPB7 ?    */
/* GPA6 col6   |GPB6 ?    */
/* GPA5 col5   |GPB5 row0 */
/* GPA4 col4   |GPB4 row1 */
/* GPA3 col3   |GPB3 row2 */
/* GPA2 col2   |GPB2 row3 */
/* GPA1 col1   |GPB1 row4 */
/* GPA0 col0   |GPB0 row5 */
/**************************/
uint8_t mcp23018_read_rows()
{
#ifdef TWI_DEBUG
    print("mcp23018_read_rows\n");
#endif
    /* GPB7   | GPB6   | GPB5 | GPB4 | GPB3 | GPB2 | GPB1 | GPB0 */
    /* high-z | high-z | in-p | in-p | in-p | in-p | in-p | in-p */
    // whole GPB as input (high-z are also inputs)
    if (!mcp23018_write_register(IODIRB, 0xFF)) goto fail;//IODIR 1=input 0=output

    // Pullups of GPIOB only for GPB0-GPB5
    if (!mcp23018_write_register(GPPUB, GPBROWS)) goto fail;

    // and read from GPIOB
    _delay_us(TGPIV); /* tGPIV 450ns Table 2-2 page 36*/
    uint8_t data = 0;
    if (!mcp23018_read_register(GPIOB, &data)) goto fail;

    return (data & GPBROWS); //mask GPB7 and GPB6 out
fail:
    print("mcp23018_read_rows failed\n");
    return 0;
}


/* Bring one of the GPAn pins to ground */
/* GPA7   | GPA6   | GPA5 | GPA4 | GPA3 | GPA2 | GPA1 | GPA0 */
/* high-z | col6   | col5 | col4 | col3 | col2 | col1 | col0 */
bool mcp23018_col_low(uint8_t n)
{
#ifdef TWI_DEBUG
    print("mcp23018_col_low\n");
#endif
    if (n > 8) goto fail;

    //make sure that pullups are properly configured
    if (!mcp23018_write_register(GPPUA,0x00)) goto fail; // GPPU 0 = pullup disabled  1=enabled

    // make sure the output is low
    if (!mcp23018_write_register(OLATA, 0x00)) goto fail; // output latch 0 means ouput low (ground)

    //set one of the GPAn pins as output and the rest as inputs
    if (!mcp23018_write_register(IODIRA, ~_BV(n))) goto fail; // IODIR 0 = output / 1 = input

    _delay_us(TGPVO); /* tGPVO 500ns Table 2-2 page 36 */

    return true;
fail:
    print("mcp23018_setGPAn_low failed\n");
    return false;
}

bool mcp23018_check(void)
{
#ifdef TWI_DEBUG
    print("mcp23018_check\n");
#endif
    for(size_t reg = IODIRA; reg <= OLATB; ++reg) { // all registers 0x00-0x15
        print("mcp23018_check: reg(%x)\n", reg);
        uint8_t data = 0;
        if(!mcp23018_read_register(reg,&data)) goto fail;
        print("mcp23018_check: reg(%x) val(%x)\n", reg, data);
    }
    return true;
fail:
    print("mcp23018_check failed\n");
    return false;
}

#ifdef TWI_DEBUG
bool mcp23018_check_reg(uint8_t reg, const uint8_t expected)
{
    print("mcp23018_check_reg\n");
    uint8_t data = 0;
    if(!mcp23018_read_register(reg,&data)) goto fail;

    if (data == expected) return true;

    // expected != actual
    printf("assert failed reg(%x) exp(%x) act(%x)\n", reg, expected, data);
fail:
    print("mcp23018_check_reg failed\n");
    return false;
}
#endif




