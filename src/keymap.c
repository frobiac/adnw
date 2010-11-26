#include "keymap.h"

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer)
{
	uint8_t c;
	if( row == 3 || row == 7 )
    	c = col;
	else if( TWO_COL_PINKY )
    		c = (row > 3) ? col+1 : col;
		else    
    		c = (row > 3) ? col   : col+1;
	return KeyMatrix[layer][row][c].mods;
}

uint8_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer)
{
	uint8_t c;
	if( row == 3 || row == 7 ) // Thumb row stays
		c = col;
	else if( TWO_COL_PINKY )
    		c = (row > 3) ? col+1: col;
		else    
    		c = (row > 3) ? col  : col+1;

	return KeyMatrix[layer][row][c].hid;
}



