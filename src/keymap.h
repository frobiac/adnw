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

#define LAYERS 4
#define ROWS   8
#define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected

#define PINKYDROP 0  ///<  drop pinky column by one for more ergonomic layout

#define _ANALOG _no


// *************************************************************
// *    Matrix of physical keyboard to key mapping
// *
// * @todo  PROGMEM and configuration
// *        Shift != Mod1, see "/'
// *************************************************************

struct keycode {
    uint8_t  hid;   ///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;  ///< HID modifiers , will be interpreted by OS keyboard layout!
} ;

/*
bu-TECK-DE
b u . , ü   p c l m f x
h i e a o   d t r n s
k y ö ä q   j g w v z

bu-TECK-DE SFR
b u . ,     p c l m f x
h i e a o   d t r n s
k y     q   j g w v z


AdNW-SFR
k u q . j   v g c l f
h i e a o   d t r n s
x y - . "   b p w m z
*/

/*
   ----- Thumb & MKT ----
   M   W 2 C A  3 1 A    â«
   ? ?  ? ?  ?

   ----- M0 Default AdNW (Ulf) -----
   k u q . j   v g c l f
   h i e a o   d t r n s
   x y - . "   b p w m z

   ----- M2 / special ---
   @ _ [ ] #   ! < > = &
   \ / { } *   ? ( ) + -
     $ | ~ `   ^ % " '

   ----- M3 / MoveNum ---
    ? ? ? ? ?     7 8 9 +
    ? ? ? ? ?     4 5 6 -
      ? ? ?     0 1 2 3

   ----- Mouse ----------
   S
   1 2 3


   ----- Compose --------
    F1.....F6
    F7....F12


   ----------------------
 */


uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);
uint8_t getKeyCode (uint8_t row, uint8_t col, uint8_t layer);


static const struct keycode /*PROGMEM*/ ModeKeyMatrix[ROWS][COLS] = {
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
#define _THUMB_ROW_RIGHT    _MOD_1,      _MOD_2,   _R_ALT,  _COMPOSE, _MOD_3, _H


static const struct keycode KeyMatrix[LAYERS+2][ROWS][COLS] PROGMEM = {
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

    // MOD1 layer (shifted)
    {
        { _ESC,   _K,         _U,         _Q,         _COLON,     _J},
        { _TAB,   _H,         _I,         _E,         _A,         _O},
        { _no,_X,         _Y,         _USCORE,    _SCOLON,    _BSLASH /** @todo Does not work with regular Shift as 2nd layer*/},
        { _THUMB_ROW_LEFT },

        { _V,     _G,         _C,         _L,         _F,         _DEL },
        { _D,     _T,         _R,         _N,         _S,         _ENTER  },
        { _B,     _P,         _W,         _M,         _Z,         _no  },
        { _THUMB_ROW_RIGHT}
    },

    // MOD2 layer (special char)
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

    // MOD3 layer (movement controls and numbers)
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

    // MOUSE MODE
    /// @todo These should not be shifted, maybe factor out entirely!
    {
        { _no, _CUT,     _COPY,    _PASTE,      _a_UML,     _o_UML },
        { _no, _MS_BTN_1, _MS_BTN_2,_MS_BTN_3, _MS_SCROLL, _no},
        { _no, _no, _no, _no, _no, _no},
        { _no, _no, _no, _L_CTRL, _L_ALT, _L_SHIFT },

        { _no, _no, _no, _no, _no, _no },
        { _no, _no, _no, _no, _no, _no },
        { _no, _no, _no, _no, _no, _no },
        { _no, _no, _no, _no, _no, _no },
//        { _MS_BTN_2,      _MS_BTN_3,     _no,    _no ,_MOUSE, _MOUSE}
    },

    // COMPOSE or MOD2 + MOD3
    {
        { _no, _CUT,     _COPY,    _PASTE,    _no,    _no },
        { _no, _SSHARP, _no,    _no,    _a_UML, _o_UML},
        { _no, _no,     _no,    _no,    _no,    _no},
        { _THUMB_ROW_LEFT },

        { _F1,      _F2,        _F3,        _F4,        _F5,    _F6     },
        { _F7,      _F8,        _F9,        _F10,       _F11,   _F12    },
        { _no,      _no,        _no,        _no,        _no,    _no     },
        { _THUMB_ROW_RIGHT}
    }
}; // end of matrix[][][]

#endif

