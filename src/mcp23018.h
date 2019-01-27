/*
   This file is part of the AdNW keyboard firmware.

   Copyright 2010-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

   Created with the helpful code and advice from
   http://rubenlaguna.com/blog/2014/01/16/mcp23018-i2c-communication-advice/

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


#pragma once

#include <stdbool.h>
#include "external/i2cmaster/i2cmaster.h"

void twi_init(void);

void mcp23018_write_register(uint8_t reg, uint8_t val);
bool mcp23018_read_register(uint8_t reg, uint8_t *data);
void mcp23018_init_unused(void);
void mcp23018_all_cols_highz(void);
void mcp23018_col_low(uint8_t n);
uint8_t mcp23018_read_rows(void);

///< write address of I2C slaves, base = 0x40 for MCP23018 + 3bit hw add << 1 + 1 if reading
static uint8_t DevMCP23018;

// some unused but kept for completeness, and useful on boards wired differently
#define TGPVO  0.5
#define TGPIV  0.450
#define IODIRA 0x00
#define IODIRB 0x01
#define IOCONA 0x0A
#define IOCONB 0x0B
#define IOCON  IOCONA
#define GPPUA  0x0C
#define GPPUB  0x0D
#define GPIOA  0x12
#define GPIOB  0x13
#define OLATA  0x14
#define OLATB  0x15

//#define GPAROWS 0xFF
#define GPBROWS 0x1F

//bring all GPIO pins to high impedance (inputs without pull-up resistors)  this is the default
void mcp23018_init_unused()
{
    mcp23018_write_register(IOCON, 0x00);
    mcp23018_write_register(GPPUA, 0x00);
    mcp23018_write_register(GPPUB, 0x00);
    mcp23018_write_register(IODIRA,0xFF);
    mcp23018_write_register(IODIRB,0xFF);
}

void mcp23018_write_register(uint8_t reg, uint8_t val)
{
    i2c_start_wait(DevMCP23018+I2C_WRITE); // set device address and write mode
    i2c_write(reg);                        // write address
    i2c_write(val);                        // write value
    i2c_stop();                            // set stop conditon = release bus
}

bool mcp23018_read_register(uint8_t reg, uint8_t *data)
{

    i2c_start_wait(DevMCP23018+I2C_WRITE); // set device address and write mode
    i2c_write(reg);                        // write address
    i2c_rep_start(DevMCP23018+I2C_READ);   // set device address and read mode
    *data = i2c_readNak();                 // read one byte
    i2c_stop();
    return true;
}


void twi_init()
{
    i2c_init();
}

bool mcp23018_init_addr(uint8_t addr)
{
    if(addr!=DevMCP23018) {
        DevMCP23018=addr;
        uint8_t ret = i2c_start(DevMCP23018+I2C_WRITE);
        if ( ret ) {
            // failed to issue start condition, possibly no device found
            i2c_stop();
            return false;
        }
    }
    return true;
}

void mcp23018_all_cols_highz()
{
    mcp23018_write_register(GPPUA,0x00); // disable pullups
    mcp23018_write_register(IODIRA,0xFF);
    _delay_us(TGPVO);
}

void mcp23018_col_low(uint8_t n)
{
    mcp23018_write_register(GPPUA,0x00);      // disable pullup
    mcp23018_write_register(OLATA,0x00);      // output low
    //set one of the GPAn pins as output and the rest as inputs
    mcp23018_write_register(IODIRA, ~_BV(n)); // IODIR 0 = output / 1 = input
    _delay_us(TGPVO);                         // tGPOV 500ns
}


uint8_t mcp23018_read_rows()
{
    mcp23018_write_register(IODIRB, 0xFF);   //IODIR 1=input 0=output
    mcp23018_write_register(GPPUB, GPBROWS); // Pullups of GPIOB only for used rows
    _delay_us(TGPIV);

    uint8_t data;
    mcp23018_read_register(GPIOB, &data); // and read from GPIOB
    return data;
}



