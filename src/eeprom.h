#ifndef _EEPROM_H
#define _EEPROM_H

#include <stdint.h>
#include <avr/eeprom.h>
#include "macro.h"
#include "hhstdio.h"

/// Increasing either too much leads to keyboard not registering at all!
#define MACROLEN    20
#define MACROCOUNT  6

#define EEPROM_DEF 0xFF

#define EE_ADDR_START       100
#define EE_ADDR_MACROS      EE_ADDR_START+100
#define EE_ADDR_MACRO(idx)  (EE_ADDR_MACROS + (idx*MACROLEN))

uint8_t writeMacro(uint8_t macro[MACROLEN], uint8_t idx);
uint8_t readMacro (uint8_t macro[MACROLEN], uint8_t idx);

//void eeprom_example (void);

#endif // _EEPROM_H
