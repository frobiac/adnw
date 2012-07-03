#ifndef MACRO_H
#define MACRO_H 1

/** Macro mode
 *  Set g_macro_mode to 1 and activate wanted macro.
 *  Then getMacroChars while possible and assemble a report.
 */

#include <stdint.h>
#include <stdbool.h>
#include <avr/eeprom.h>

#include "hhstdio.h"
#include "keymap.h"


#ifdef _private_macros_h_NOT_FOUND
/**
 * _private_macros.h
 * Put private macro declaration in this header file and define LEN and COUNT:
 * - use ascii characters/strings, or the corresponding codes from ascii_table directly
 * - any modifier(s) for the following character should be ORed with 0x80
 */
/// Increasing either too much leads to keyboard not registering at all!
#define MACROCOUNT   3
#define MACROLEN    20

static const char EEmacrosC[MACROCOUNT][MACROLEN]  =  {
    "SampleString!",
    // CTRL+ALT+DEL
    { (L_CTL|L_ALT)+0x80, 127, '\0' },
    // ^c ^t ^v return for firefox
    { (L_CTL)+0x80, c , (L_CTL)+0x80, t, (L_CTL)+0x80, v, '\0' },
};
#endif

#include "_private_macros.h"

/**
 *  Macros are stored in eeprom.
 *  Content has to be put into a separate file to keep sensitive data
 *  out of normal sources :)
 *  Addresses are therefore fixed here.
*/
#define EEPROM_DEF 0xFF

#define EE_ADDR_START       100
#define EE_ADDR_MACROS      (EE_ADDR_START+100)
#define EE_ADDR_MACRO(idx)  (EE_ADDR_MACROS + (idx*MACROLEN))

uint8_t writeMacro(const char    macro[MACROLEN], uint8_t idx);
uint8_t readMacro (uint8_t macro[MACROLEN], uint8_t idx);

bool initMacros(void);
void printMacros(void);

bool macroMode(void);
void setMacroMode(bool on);
void endMacro(void);

bool activateMacro(uint8_t id);
bool getMacroChar(struct keycode *kc);
bool getMacroCharacter(char *c);
bool getMacroReport(USB_KeyboardReport_Data_t *report);


#endif
