#ifndef MACRO_H
#define MACRO_H 1

#include <stdint.h>
#include <stdbool.h>

#include "keymap.h"


uint8_t g_macro_mode;

bool activateMacro(uint8_t id);
bool getMacroChar(struct keycode *kc);

//uint8_t fillMacroReport(USB_KeyboardReport_Data_t *report_data);



#endif
