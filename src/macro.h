#ifndef MACRO_H
#define MACRO_H 1

/** Macro mode
 *  Set g_macro_mode to 1 and activate wanted macro.
 *  Then getMacroChars while possible and assemble a report.
 */

#include <stdint.h>
#include <stdbool.h>

#include "hhstdio.h"
#include "keymap.h"

#include "eeprom.h"

/**
 *  Macros are stored in eeprom.
 *  Content has to be put into a separate file to keep sensitive data
 *  out of normal sources :)
 *  Addresses are therefore fixed here.
*/


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
