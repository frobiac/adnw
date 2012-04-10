#include "keymap.h"

/**
 *  Mappings of row and columns of the switch matrix to the selected layout matrix.
 *  Currently, the two halves of the keyboard are stacked so that
 *  rows 0-3 correspond to the left half and
 *  rows 4-7 to the right hand side.
 *
 *  All indices are zero-based.
 */
uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer)
{
    if( PINKYDROP ) {
        if( (row<4 && row > 0 && col==1) || (row > 4 && col == 4))
            return KeyMatrix[layer][row-1][col].mods;
    } else
        return KeyMatrix[layer][row][col].mods;
}

uint8_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer)
{
    if(PINKYDROP) {
        if( (row<4 && row > 0 && col==1) || (row > 4 && col == 4))
            return KeyMatrix[layer][row-1][col].hid;
    } else
        return KeyMatrix[layer][row][col].hid;
}



