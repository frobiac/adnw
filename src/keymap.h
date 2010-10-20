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

/// To shift the used 3x5 keys per side flush with left or right border
#define TWO_COL_PINKY 1
#define _MID( x ) ( TWO_COL_PINKY ) ? (x ",") : "" 

#define _OUT( x )  ( !TWO_COL_PINKY ) ? (x ",") : ""
// #define _OUT(x) !TWO_COL_PINKY ? : " "

//static const uint8_t PROGMEM Keymap[][ROWS][COLS] = {

static const struct keycode /*PROGMEM*/ ModeKeyMatrix[ROWS][COLS] =
{
    // Left half
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _SPACE,    _TAB,    _no },

    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _no,     _no,     _no,     _no,		_no,     _no },
    { _ANALOG, _BSPACE, _ENTER,  _no,		_no,     _no }
};

/// @todo PROGMEM

#define _THUMB_ROW_LEFT		_MOUSE,		_L_GUI,		_R_ALT,		_L_SHIFT,	_L_CTRL,	_L_ALT
#define _THUMB_ROW_RIGHT	_ANALOG,	_L_SHIFT,	_L_CTRL,	_no,		_no,		_COMPOSE



static const struct keycode /*PROGMEM*/ KeyMatrix[LAYERS+2][ROWS][COLS] =
{    // normal layer
{
        { _OUT(_no )	_q,		_u,			_o,			_b,			_p,			_MID(_no )		},
        { _OUT(_no )	_a,		_i,			_e,			_t,			_c,			_MID(_no )		},
        { _OUT(_no )	_no,	_j,			_x,			_w,			_v,			_MID(_no )	    },
        { _THUMB_ROW_LEFT },
        
        { _MID(_no )	_k,		_g,			_l,			_m,			_MINUS,		_ESC	_OUT(_no )	},
        { _MID(_no )	_h,		_d,			_n,			_r,			_s,			_BSPACE	_OUT(_no )	},
        { _MID(_no )	_z,		_y,			_COMMA,		_PERIOD,	_f,			_ENTER	_OUT(_no )	},
        { _THUMB_ROW_RIGHT}
		},

    // MOD1 layer (shifted)
    {
        { _Q,		_U,			_O,			_B,			_P,			_no		},
        { _A,		_I,			_E,			_T,			_C,			_no		},
        { _no,		_J,			_X,			_W,			_V,			_no		},
        { _THUMB_ROW_LEFT },
        
        { _K,			_G,		_L,			_M,			_USCORE,	_no		},
        { _H,			_D,		_N,			_R,			_S,			_no		},
		{ _Z,			_Y,		_SCOLON,	_COLON,		_F,			_no		},
        { _THUMB_ROW_RIGHT}
    },	

    // MOD2 layer (special chars)
    {
        { _AT,      _no,		_L_BRACKET, _R_BRACKET, _HASH ,   _no			},
        { _BSLASH,  _SLASH,		_L_BRACE,   _R_BRACE,   _ASTERIX, _no			},
        { _no,      _DOLLAR,	_BAR,		_TILDE,		_GRAVE ,  _no			},
        { _THUMB_ROW_LEFT },

        { _EXCLAM,	_LESS,      _GREATER,	_EQUAL,		_AMPERSAND,	_no		},
        { _QUESTION,_L_PAREN,	_R_PAREN, 	_PLUS,		_MINUS,		_no		},
        { _CARET,	_PERCENT, 	_DQUOTE,    _SQUOTE,	_no,		_no		},
        { _THUMB_ROW_RIGHT}
    },

    // MOD3 layer (movement controls and numbers)
    {
        { _PGUP,	_BSPACE,	_UP,		_DEL ,		_PGDN,	_no		},
        { _HOME,	_LEFT,		_DOWN,		_RIGHT,		_END ,  _no		},
        { _ESC,		_TAB,		_INS,		_ENTER,		_no ,   _no		},
        { _THUMB_ROW_LEFT },
        
        { _EXCLAM,	_7,			_8,			_9,     _PLUS,		_no		},
        { _EQUAL,	_4,			_5,			_6,     _MINUS,		_no		},
        { _0,		_1,			_2,			_3,     _no,		_no		},
        { _THUMB_ROW_RIGHT}
    },

	//MOUSE MODE
    {
		{ _q,		_u,			_o,			_b,			_p,		_no		},
		{ _a,		_i,			_e,			_t,			_c,		_no		},
		{ _no,		_j,			_x,			_w,			_v,		_no		},
		{ _THUMB_ROW_LEFT },
	
		{ _no,		_MS_SCROLL,	_no,		_no,		_no,	_no		},
		{ _no,		_MS_BTN_1,	_MS_BTN_2,	_MS_BTN_3,	_no,	_no		},
		{ _no,		_no,		_no,		_no,		_no,	_no		},
		{ _THUMB_ROW_RIGHT}
	},

	// COMPOSE
	{
        { _SSHARP,	_U_UML,		_O_UML,		_no,		_no,	_no		},
        { _A_UML,	_no,		_no,		_no,		_no ,	_no		},
        { _no,   	_no,		_no,		_no,		_no ,	_no		},
        { _THUMB_ROW_LEFT },

        { _F1,		_F2,		_F3,		_F4,		_F5,	_F6		},
        { _F7,		_F8,		_F9,		_F10,		_F11,	_F12	},
        { _no,		_no,		_no,		_no,		_no,	_no		},
        { _THUMB_ROW_RIGHT}
    }
}; // end of matrix[][][]


#endif
