/*
    Copyright 2010-2013 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
#include <stdbool.h>
#include "hid_usage.h"
#include "config.h"

#define LAYERS 5
#define ROWS   8
#define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected

#define MOUSE_LAYER 3   ///< where they mousekeys reside - @todo: fix detection

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

#define _MACRO _no

#ifdef BLUECUBE
/*
 * 00 01 02 03 04 05    40 41 42 43 44 45
 * 10 11 12 13 14 15    50 51 52 53 54 55
 * 20 21 22 23 24 25    60 61 62 63 64 65
 *                30    75 
 *    32 33 34 35 31    74 70 71 72 73
 *
 *
 */

  // on BlueCube, the innermost thumb-buttons are on outermost columns in matrix!
  //         1  6
  // 3 4 5 6 2  5 1 2 3 4
  //#define _THUMB_ROW_LEFT     _MACRO,   _L_GUI,  _L_SHIFT, _L_ALT , _L_CTRL, _SPACE
  //#define _THUMB_ROW_RIGHT    _L_SHIFT, _MOD_1,  _R_ALT,   _MOD_3,  _MOD_2,  _MOD_3
         //_X, _L_SHIFT, _L_ALT , _L_CTRL, _SPACE, _L_GUI, _MACRO,
		//	_Y, _MOD_3,_L_SHIFT, _MOD_1,  _R_ALT, _A, _B
#define KEYMAP( no, \
   k00,k01,k02,k03,k04,k05,        k40,k41,k42,k43,k44,k45, \
   k10,k11,k12,k13,k14,k15,        k50,k51,k52,k53,k54,k55, \
   k20,k21,k22,k23,k24,k25,        k60,k61,k62,k63,k64,k65, \
   no30,t31,t32,t33,t34,t35,t36,t70,t71,t72,t73,t74,t75,no76   \
) { \
  { k00,k01,k02,k03,k04,k05 }, \
  { k10,k11,k12,k13,k14,k15 }, \
  { k20,k21,k22,k23,k24,k25 }, \
  { t36,t35,t31,t32,t33,t34 }, \
  { k40,k41,k42,k43,k44,k45 }, \
  { k50,k51,k52,k53,k54,k55 }, \
  { k60,k61,k62,k63,k64,k65 }, \
  { t72,t73,t74,t75,t70,t71 } \
}
#endif

#ifdef WHITERECT
/*  Row/col matrix: (0-based)
 *  unused: 11,21,51,61
 *  innermost thumb buttons: 31,41,71,81 
 *
 *  
 *     01 02 03 04 05 --  -- 41 42 43 44 45
 *    11 12 13 14 15 --    -- 51 52 53 54 55
 *   21 22 23 24 25 20      60 61 62 63 64 65
 *    31 32 33 34 35 30    70 71 72 73 74 75
 */
  //#define _THUMB_ROW_LEFT     _MACRO,   _L_GUI,  _L_SHIFT, _L_ALT , _L_CTRL, _SPACE
  //#define _THUMB_ROW_RIGHT    _L_SHIFT, _MOD_1,  _R_ALT,   _MOD_3,  _MOD_2,  _MOD_3

#define KEYMAP( no, \
  no00,k01,k02,k03,k04,k05,        k40,k41,k42,k43,k44,no45, \
  no10,k11,k12,k13,k14,k15,        k50,k51,k52,k53,k54,no55, \
  no20,k21,k22,k23,k24,k25,        k60,k61,k62,k63,k64,no65, \
   t30,t31,t32,t33,t34,t35,t20,t65,t70,t71,t72,t73,t74,t75   \
) { \
  {  no, k01,k02,k03,k04,k05 }, \
  {  no, k11,k12,k13,k14,k15 }, \
  { t20, k21,k22,k23,k24,k25 }, \
  { t30, t31,t32,t33,t34,t35 }, \
\
  {  no, k40, k41,k42,k43,k44 }, \
  {  no, k50, k51,k52,k53,k54 }, \
  { no00/*t65*/, k60, k61,k62,k63,k64 }, \
  { t75, t70, t71,t72,t73,t74 } \
}
#endif

// (+) : Only on WHITERECT with 7 keys in thumb-row, these are located at keyboards outside edge.
//		 Both from left to right
#define _THUMB_ROW_LEFT  _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO
#define _THUMB_ROW_RIGHT _Y, _MOD_3,   _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 

/**
 * MKT keycode if single key and mousekey location is stored in here.
 * In contrast to the keymatrix below, only actual HID-usage codes are used, as
 * there should be no need for modifiers with MKTs.
 * The mouse button locations are put here as there is no overlap whith other keys,
 * but strictly not the right place here.
 */
static const uint8_t SecondaryUsage[ROWS][COLS] =
  KEYMAP( 0,
 HID_ESC,  0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0 ,
 0, MS_BTN_1, MS_BTN_2, MS_BTN_3, MS_SCROLL, 0 , 0, 0, 0, 0, 0, 0 ,
 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0 ,
 0, 0, 0, HID_ESC, HID_TAB, 0 /*is real SPACE*/,0,HID_ESC , HID_BACKSPACE, HID_ENTER, 0, 0, 0, 0 
);


static const keycode KeyMatrix[LAYERS][ROWS][COLS] PROGMEM = 
{
  // normal layer
 KEYMAP( _no,
         _MOD_3,   _k, _u, _q,     _PERIOD, _j,      _v, _g, _c, _l, _f, _BSPACE ,
         _TAB,   _h, _i, _e,     _a,      _o,      _d, _t, _r, _n, _s, _ENTER  ,
         _DQUOTE,_x, _y, _MINUS, _COMMA,  _SLASH,  _b, _p, _w, _m, _z, _no  ,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 
// _a,_b,_c,_d,_e,_f,_g, _a,_b,_c,_d,_e,_f,_g
  ),
    
  // MOD1 layer (special char)
  KEYMAP( _no, 
    _no, _AT,      _DEGREE,   _L_BRACKET, _R_BRACKET, _HASH   , _EXCLAM,  _LESS,      _GREATER,   _EQUAL,     _AMPERSAND, _no ,
    _no, _BSLASH,  _SLASH,    _L_BRACE,   _R_BRACE,   _ASTERIX, _QUESTION,_L_PAREN,   _R_PAREN,   _PLUS,      _MINUS,     _no ,
    _no, _no,      _DOLLAR,   _PIPE,      _TILDE,     _GRAVE , _CARET,   _PERCENT,   _DQUOTE,    _SQUOTE,    _no,        _no ,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 
  ),
    
  // MOD2 layer (movement controls and numbers)
  KEYMAP( _no,
    _no, _PGUP,   _BSPACE,    _UP,        _DEL ,      _PGDN, _SSHARP,_7,         _8,         _9,     _a_UML, _A_UML ,
    _no, _HOME,   _LEFT,      _DOWN,      _RIGHT,     _END , _PERIOD,_4,         _5,         _6,     _o_UML, _O_UML ,
    _no, _ESC,    _TAB,       _INS,       _ENTER,     _no , _0,     _1,         _2,         _3,     _u_UML, _U_UML ,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 
  ),

  // MOD3 MOUSE MODE
  /// @todo These should not be shifted, maybe factor out entirely!
  KEYMAP( _no,
    _no, _no, _no, _no, _a_UML, _o_UML ,   _F1,      _F2,        _F3,        _F4,        _F5,    _F6     ,
    _no, _no, _no, _no, _no,    _no,       _F7,      _F8,        _F9,        _F10,       _F11,   _F12    ,
    _no, _no, _no, _no, _no,    _no,       _no,      _no,        _no,        _no,        _no,    _no     ,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 
    ),
    
  // MOD4 BU-TECK
  KEYMAP( _no,
    _ESC,   _b, _u, _q, _PERIOD,_x  , _p,   _c, _l, _m, _f, _BSPACE ,
    _TAB,   _h, _i, _e, _a, _o  , _d,   _t, _r, _n, _s, _ENTER  ,
    _DQUOTE,_k, _y, _MINUS, _COMMA, _SLASH , _j,   _g, _w, _v, _z, _no  ,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 
  )
}; // end of matrix[][][]
    

/*
  #define _THUMB_ROW_LEFT     _MACRO,   _L_GUI,  _L_SHIFT, _L_ALT , _L_CTRL, _SPACE
  #define _THUMB_ROW_RIGHT    _L_SHIFT, _MOD_1,  _R_ALT,   _MOD_3,  _MOD_2,  _MOD_3
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
        { _no, _no, _no, _no, _no, _no},
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
*/

#endif
