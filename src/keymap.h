#ifndef KEYMAP_H
#define KEYMAP_H 1

#include <stdint.h>
#include "hid_usage.h"

#define LAYERS 4
#define ROWS   8
#define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected

#define _ANALOG _no

// *************************************************************
// *    Matrix of physical keyboard to key mapping
// *************************************************************

struct keycode {
    uint8_t  ch;	///< could be useful for dumping the layout
    uint8_t  hid;	///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;	///< HID modifiers , will be interpreted by OS keyboard layout!
} ;

/* AdNW (Ulf)

   kuÜ,ä vgclfß
   hieao dtrnsz
   xyö.q bpwmj

   kuj,  vgclf
   hieao dtrns
   xy .q bpwmz

 */

/*
   ----- Thumb & MKT ----
   M   W 2 C A  3 1 A    ♫ 
   ⌧ ⇤  ␣ ⌫  ⌦ 

   ----- M0 Default -----
   q u o b p   k g l m f 
   a i e t c   h d n r s
   j x w v   z y , . -

   ----- M1 / Shift -----
   Q U O B P   K G L M F 
   A I E T C   H D N R S
   J X W V   Z Y ; : _

   ----- M2 / special ---
   @ _ [ ] #   ! < > = & 
   \ / { } *   ? ( ) + -
   $ | ~ `   ^ % " '

   ----- M3 / MoveNum ---
   ⇞ ⌫ ⇡ ⌦ ⇟     7 8 9 + 
   ⇱ �  ⇣ ⇢ ⇲     4 5 6 -
   ↲ ⎀ ⇥     0 1 2 3

   ----- Mouse ----------
   S
   1 2 3


   ----- Compose --------
   ß ü ö       F1.....F6 
   Ä           F7....F12


   ----------------------
 */
//static const uint8_t PROGMEM Keymap[][ROWS][COLS] = {


uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);

uint8_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer);


static const struct keycode /*PROGMEM*/ ModeKeyMatrix[ROWS][COLS] =
{
    // Left half
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,	 _ESC,  	_TAB,  _SPACE },

    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _BSPACE, _ENTER,  _no,	 _no,   	_no,     _no }
};

/// @todo PROGMEM

//#define _THUMB_ROW_LEFT		_MOUSE,		_L_GUI,		_R_ALT,		_L_ALT,		_L_SHIFT,		_L_CTRL
//#define _THUMB_ROW_RIGHT	_ANALOG,	_MOD_2,		_MOD_3,		_no,		_no,		_COMPOSE
#define _THUMB_ROW_LEFT		_L_GUI,		_R_ALT,		_MOD_1,		_L_CTRL , _L_ALT,		_L_SHIFT
#define _THUMB_ROW_RIGHT	_MOD_2,		_MOD_3,		_COMPOSE,	_R_CTRL,  _no, _no

/** Depending on the TWO_COL_PINKY setting one of the outer columns is dropped in 
 *  getKeyCode() !!!

 kuj,  vgclf
 hieao dtrns
 xy .q bpwmz

 */ 
static const struct keycode /*PROGMEM*/ KeyMatrix[LAYERS+2][ROWS][COLS] =
{    // normal layer
    {
        { _ESC, _k,	_u,	_q,	_PERIOD,_j	},
        { _TAB, _h,	_i,	_e,	_a,	_o 	},
        { _no,  _x,	_y,	_MINUS,	_COMMA,	_DQUOTE	},
        { _THUMB_ROW_LEFT },

        { _v,	_g,	_c,	_l,	_f,     _BSPACE },
        { _d,	_t,	_r,	_n,	_s,	_ENTER	},
        { _b,	_p,	_w,	_m,	_z,	_no   	},
        { _THUMB_ROW_RIGHT}
    },

    // MOD1 layer (shifted)
    {
        { _ESC, _K,         _U,         _Q,         _COLON,     _J},
        { _TAB, _H,         _I,         _E,         _A,         _O},
        { _no,  _X,         _Y,         _USCORE,    _SCOLON,    _SQUOTE},
        { _THUMB_ROW_LEFT },

        { _V,     _G,         _C,         _L,         _F,         _BSPACE },
        { _D,     _T,         _R,         _N,         _S,         _ENTER  },  
        { _B,     _P,         _W,         _M,         _Z,         _no     },  
        { _THUMB_ROW_RIGHT}
    },	

    // MOD2 layer (special chars)
    {
        { _no, _AT,      _no,		_L_BRACKET, _R_BRACKET, _HASH },
        { _no, _BSLASH,  _SLASH,	_L_BRACE,   _R_BRACE,   _ASTERIX},
        { _no, _no,      _DOLLAR,	_BAR,	    _TILDE,	_GRAVE },
        { _THUMB_ROW_LEFT },

        { _EXCLAM,	_LESS,      _GREATER,	_EQUAL,		_AMPERSAND,	_no	},
        { _QUESTION,_L_PAREN,	_R_PAREN, 	_PLUS,		_MINUS,		_no	},
        { _CARET,	_PERCENT, 	_DQUOTE,    _SQUOTE,	_no,		_no	},
        { _THUMB_ROW_RIGHT}
    },

    // MOD3 layer (movement controls and numbers)
    {
        { _no, _PGUP,	_BSPACE,	_UP,		_DEL ,		_PGDN},
        { _no, _HOME,	_LEFT,		_DOWN,		_RIGHT,		_END },
        { _no, _ESC,	_TAB,		_INS,		_ENTER,		_no },
        { _THUMB_ROW_LEFT },

        { _EXCLAM,	_7,			_8,			_9,     _PLUS,		_no },
        { _EQUAL,	_4,			_5,			_6,     _MINUS,		_no	},
        { _0,		_1,			_2,			_3,     _no,		_no	},
        { _THUMB_ROW_RIGHT}
    },

    //MOUSE MODE
    /// @todo These should not be shifted, maybe factor out entirely!
    {
        { _no, _SSHARP, _U_UML,         _O_UML, _A_UML, _no},
        { _no, _MS_BTN_1, _MS_BTN_2, _MS_BTN_3, _MS_SCROLL, _no},
        { _no, _no, _no, _no, _no, _no},
        { _no, _no, _no, _no, _MS_SCROLL, _MS_BTN_1 },

        { _no, _no, _no, _no, _no, _no },
        { _no, _no, _no, _no, _no, _no },
        { _no, _no, _no, _no, _no, _no },
        { _MS_BTN_2,      _MS_BTN_3,     _no,    _no ,_MOUSE, _MOUSE}
    },

    // COMPOSE
    {
        { _no, _SSHARP,	_U_UML,	_no,	_no,	_no},
        { _no, _no,     _no,	_no,	_A_UML,	_O_UML},
        { _no, _no,   	_no,	_no,	_no,	_no},
        { _THUMB_ROW_LEFT },

        { _F1,		_F2,		_F3,		_F4,		_F5,	_F6		},
        { _F7,		_F8,		_F9,		_F10,		_F11,	_F12 	},
        { _no,		_no,		_no,		_no,		_no,	_no		},
        { _THUMB_ROW_RIGHT}
    }
}; // end of matrix[][][]


/** Depending on the TWO_COL_PINKY setting one of the outer columns is dropped in 
 *  getKeyCode() !!!
 */ 
static const struct keycode /*PROGMEM*/ KeyMatrixNTSplit[LAYERS+2][ROWS][COLS] =
{    // normal layer
    {
        { _ESC, _q,			_u,			_o,			_b,			_p},
        { _TAB, _a,			_i,			_e,			_t,			_c},
        { _no,  _no,		_j,			_x,			_w,			_v},
        { _THUMB_ROW_LEFT },

        { _k,		_g,			_l,			_m,			_MINUS,		_ESC	},
        { _h,		_d,			_n,			_r,			_s,			_BSPACE	},
        { _z,		_y,			_COMMA,		_PERIOD,	_f,			_ENTER	},
        { _THUMB_ROW_RIGHT}
    },

    // MOD1 layer (shifted)
    {
        { _no, _Q,		_U,			_O,			_B,			_P},
        { _no, _A,		_I,			_E,			_T,			_C},
        { _no,_no,		_J,			_X,			_W,			_V},
        { _THUMB_ROW_LEFT },

        { _K,			_G,		_L,			_M,			_USCORE,	_no,    	},
        { _H,			_D,		_N,			_R,			_S,			_no,    	},
        { _Z,			_Y,		_SCOLON,	_COLON,		_F,			_no,    	},
        { _THUMB_ROW_RIGHT}
    },	

    // MOD2 layer (special chars)
    {
        { _no, _AT,      _no,		_L_BRACKET, _R_BRACKET, _HASH },
        { _no, _BSLASH,  _SLASH,	_L_BRACE,   _R_BRACE,   _ASTERIX},
        { _no, _no,      _DOLLAR,	_BAR,		_TILDE,		_GRAVE },
        { _THUMB_ROW_LEFT },

        { _EXCLAM,	_LESS,      _GREATER,	_EQUAL,		_AMPERSAND,	_no	},
        { _QUESTION,_L_PAREN,	_R_PAREN, 	_PLUS,		_MINUS,		_no	},
        { _CARET,	_PERCENT, 	_DQUOTE,    _SQUOTE,	_no,		_no	},
        { _THUMB_ROW_RIGHT}
    },

    // MOD3 layer (movement controls and numbers)
    {
        { _no, _PGUP,	_BSPACE,	_UP,		_DEL ,		_PGDN},
        { _no, _HOME,	_LEFT,		_DOWN,		_RIGHT,		_END },
        { _no, _ESC,	_TAB,		_INS,		_ENTER,		_no },
        { _THUMB_ROW_LEFT },

        { _EXCLAM,	_7,			_8,			_9,     _PLUS,		_no },
        { _EQUAL,	_4,			_5,			_6,     _MINUS,		_no	},
        { _0,		_1,			_2,			_3,     _no,		_no	},
        { _THUMB_ROW_RIGHT}
    },

    //MOUSE MODE
    /// @todo These should not be shifted, maybe factor out entirely!
    {
        { _no, 		_q,		_u,			_o,			_b,			_p},
        { _no, 		_a,		_i,			_e,			_t,			_c},
        { _no, 		_no,	_j,			_x,			_w,			_v},
        { _THUMB_ROW_LEFT },

        { _MS_SCROLL,	_no,		_no,		_no,	_no,    _no		},
        { _MS_BTN_1,	_MS_BTN_2,	_MS_BTN_3,	_no,	_no,    _no		},
        { _no,		_no,		_no,		_no,	_no,    _no		},
        { _THUMB_ROW_RIGHT}
    },

    // COMPOSE
    {
        { _no, _SSHARP,	_U_UML,		_O_UML,		_no,		_no},
        { _no, _A_UML,	_no,		_no,		_no,		_no},
        { _no, _no,   	_no,		_no,		_no,		_no},
        { _THUMB_ROW_LEFT },

        { _F1,		_F2,		_F3,		_F4,		_F5,	_F6		},
        { _F7,		_F8,		_F9,		_F10,		_F11,	_F12 	},
        { _no,		_no,		_no,		_no,		_no,	_no		},
        { _THUMB_ROW_RIGHT}
    }
}; // end of matrix[][][]


#endif

