/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2018 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
#include <avr/pgmspace.h>
#include "keymap.h"
#include "global_config.h"
/**
 *  Mappings of row and columns of the switch matrix to the selected layout matrix.
 *  Currently, the two halves of the keyboard are stacked so that
 *  rows 0-3 correspond to the left half and
 *  rows 4-7 to the right hand side.
 *
 *  All indices are zero-based.
 */
static keycode getKeyStruct(uint8_t row, uint8_t col, uint8_t layer);

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer)
{
    return getKeyStruct(row, col, layer).mods;
}

uint8_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer)
{
    return getKeyStruct(row, col, layer).hid;
}

keycode getKeyStruct(uint8_t row, uint8_t col, uint8_t layer)
{
#ifdef ALTERNATE_LAYER
    // use alternate layout: @todo: convert to altenateLayout ?!
    if( g_cfg.fw.alt_layer && layer==0)
        layer=ALT_ALPHA_LAYER;
#endif

    keycode kc;

    memcpy_P(&kc, &KeyMatrix[layer][row][col], sizeof(kc));

    return kc;
}

