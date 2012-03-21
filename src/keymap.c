#include "keymap.h"

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer)
{
    return KeyMatrix[layer][row][col].mods;
}

uint8_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer)
{
    return KeyMatrix[layer][row][col].hid;
}



