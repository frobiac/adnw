/*
    Copyright 2010-2012 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#ifndef KEYMAP_H
#define KEYMAP_H 1

#include <stdint.h>
#include "hid_usage.h"

#define LAYERS 5
#define ROWS   8
#define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected



#define PINKYDROP 0  ///<  drop pinky column by one for more ergonomic layout


/** *********************************************************
 *    Matrix of physical keyboard to key mapping
 *
 * @todo  configuration
 *        could be much more memory efficient by
 *           - not duplicating thumbs everywhere
 *           - only have mode keys on selected keys
 ************************************************************/

typedef struct {
    uint8_t  hid;   ///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;  ///< HID modifiers , will be interpreted by OS keyboard layout!
    char     ch;
} keycode;

bool g_alternateLayer; ///< toggle using an alternate layout for layer 1

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);
uint8_t getKeyCode (uint8_t row, uint8_t col, uint8_t layer);
uint8_t getKeyChar (uint8_t row, uint8_t col, uint8_t layer);
void    printLayout(uint8_t l);


static const keycode /*PROGMEM*/ ModeKeyMatrix[ROWS][COLS] = {
    // Left half
    { _no,     _no,     _no,     _no,       _no,     _no },
    { _no,     _no,     _no,     _no,       _no,     _no },
    { _no,     _no,     _no,     _no,       _no,     _no },
    { _no,     _no,     _no,     _ESC,      _TAB,    _no /*is real SPACE*/ },

    { _no,     _no,     _no,     _no,       _no,     _no },
    { _no,     _no,     _no,     _no,       _no,     _no },
    { _no,     _no,     _no,     _no,       _no,     _no },
    { _BSPACE, _ENTER,  _no,     _no,       _no,     _no }
};

// on BlueCube, the innermost thumb-buttons are on outermost columns in matrix!
//         0  5
// 2 3 4 5 1  4 0 1 2 3
//
#define _MACRO _no
#define _THUMB_ROW_LEFT     _no /*macro*/, _L_GUI,     _L_SHIFT,    _L_ALT , _L_CTRL, _SPACE
#define _THUMB_ROW_RIGHT    _L_SHIFT,      _MOD_1,   _R_ALT,  _MOD_3,  _MOD_2, _MOD_3

static const keycode KeyMatrix[LAYERS][ROWS][COLS] PROGMEM = {
    // normal layer
    {
        { _ESC,   _k, _u, _q, _PERIOD,_j  },
        { _TAB,   _h, _i, _e, _a, _o  },
        { _DQUOTE,_x, _y, _MINUS, _COMMA, _SLASH },
        { _THUMB_ROW_LEFT },

        { _v,   _g, _c, _l, _f, _BSPACE },
        { _d,   _t, _r, _n, _s, _ENTER  },
        { _b,   _p, _w, _m, _z, _no  },
        { _THUMB_ROW_RIGHT}
    },

    // MOD1 layer (special char)
    {
        { _no, _AT,      _DEGREE,   _L_BRACKET, _R_BRACKET, _HASH   },
        { _no, _BSLASH,  _SLASH,    _L_BRACE,   _R_BRACE,   _ASTERIX},
        { _no, _no,      _DOLLAR,   _PIPE,      _TILDE,     _GRAVE },
        { _THUMB_ROW_LEFT },

        { _EXCLAM,  _LESS,      _GREATER,   _EQUAL,     _AMPERSAND, _no },
        { _QUESTION,_L_PAREN,   _R_PAREN,   _PLUS,      _MINUS,     _no },
        { _CARET,   _PERCENT,   _DQUOTE,    _SQUOTE,    _no,        _no },
        { _THUMB_ROW_RIGHT}
    },

    // MOD2 layer (movement controls and numbers)
    {
        { _no, _PGUP,   _BSPACE,    _UP,        _DEL ,      _PGDN},
        { _no, _HOME,   _LEFT,      _DOWN,      _RIGHT,     _END },
        { _no, _ESC,    _TAB,       _INS,       _ENTER,     _no },
        { _THUMB_ROW_LEFT },

        { _SSHARP,_7,         _8,         _9,     _a_UML, _A_UML },
        { _PERIOD,_4,         _5,         _6,     _o_UML, _O_UML },
        { _0,     _1,         _2,         _3,     _u_UML, _U_UML },
        { _THUMB_ROW_RIGHT}
    },

    // MOD3 MOUSE MODE
    /// @todo These should not be shifted, maybe factor out entirely!
    {
        { _no, _no,     _no,    _no,      _a_UML,     _o_UML },
        { _no, _MS_BTN_1, _MS_BTN_2,_MS_BTN_3, _MS_SCROLL, _no},
        { _no, _no, _no, _no, _no, _no},
        { _no, _no, _no, _L_CTRL, _L_ALT, _L_SHIFT },

        { _F1,      _F2,        _F3,        _F4,        _F5,    _F6     },
        { _F7,      _F8,        _F9,        _F10,       _F11,   _F12    },
        { _no,      _no,        _no,        _no,        _no,    _no     },
        { _THUMB_ROW_RIGHT},
    },

    // MOD4 BU-TECK
    {
        { _ESC,   _b, _u, _q, _PERIOD,_x  },
        { _TAB,   _h, _i, _e, _a, _o  },
        { _DQUOTE,_k, _y, _MINUS, _COMMA, _SLASH },
        { _THUMB_ROW_LEFT },

        { _p,   _c, _l, _m, _f, _BSPACE },
        { _d,   _t, _r, _n, _s, _ENTER  },
        { _j,   _g, _w, _v, _z, _no  },
        { _THUMB_ROW_RIGHT}
    }
}; // end of matrix[][][]

#endif
