#ifndef KEYMAP_H
#define KEYMAP_H 1

#include <stdint.h>
#include "hid_usage.h"

void printMatrixAllLayers(void);
void printMatrix(void);


#define LAYERS 4
#define ROWS   8
#define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected

// *************************************************************
// *    Matrix of physical keyboard to key mapping
// *************************************************************

struct keycode {
    uint8_t  ch;	///< could be useful ofr dumping the layout
    uint8_t  hid;	///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;	///< HID modifiers , will be interpreted by OS keyboard layout!
} ;


//static const uint8_t PROGMEM Keymap[][ROWS][COLS] = {

static const struct keycode /*PROGMEM*/ ModeKeyMatrix[ROWS][COLS] =
{
    // Left half
    {   _NO,     _NO,     _NO,     _NO,     _NO,     _NO     },
    {   _NO,     _NO,     _NO,     _NO,     _NO,     _NO     },
    {   _NO,     _NO,     _NO,     _NO,     _NO,     _NO     },
    {   _NO,     _NO,     _ESC,     _SPACE,     _TAB,     _NO     },

    {   _NO,     _NO,     _NO,     _NO,     _NO,     _NO     },
    {   _NO,     _NO,     _NO,     _NO,     _NO,     _NO     },
    {   _NO,     _NO,     _NO,     _NO,     _NO,     _NO     },
    {   _NO,     _ENTER,     _BSPACE,     _NO,     _NO,     _NO     },
};

/// @todo PROGMEM
///			lalt = gui, lgui = ctrl, 
static const struct keycode /*PROGMEM*/ KeyMatrix[LAYERS+2][ROWS][COLS] =
{
    // normal layer
    {
        // Left half
        {   _q,     _u,     _o,     _b,     _p, _NO },
        {   _a,     _i,     _e,     _t,     _c, _NO },
        {   _NO,    _j,     _x,     _w,     _v, _TAB },
        {   _MOD_MOUSE, _L_GUI, _L_ALT, _MOD_3,  _L_CTRL, _L_ALT },
        // Right half
        {   _MOD_1, _k,     _g,     _l,     _m,     _f },
        {   _MOD_2, _h,     _d,     _n,     _r,     _s },
        {   _L_SHIFT, _z,     _y,     _COMMA, _PERIOD,_MINUS },
        {   _NO,    _MOD_2, _L_SHIFT/*_MOD_1*/, _R_ALT, _R_CTRL,  _MOD_COMPOSE }
    },

    // MOD1 layer (shifted)
    {// Left half
        {   _Q,  _U, _O, _B, _P, _NO },
        {   _A,  _I, _E, _T, _C, _NO },
        {   _NO, _J, _X, _W, _V, _NO },
        {   _MOD_MOUSE, _L_GUI, _L_ALT, _MOD_3,  _L_CTRL, _NO },
        // Right half
        {   _NO, _K, _G, _L, _M, _F },
        {   _NO, _H, _D, _N, _R, _S },
        {   _NO, _Z, _Y, _SCOLON, _COLON, _USCORE },
        {   _NO, _MOD_2, _L_SHIFT, _R_ALT, _NO,   _MOD_COMPOSE }
    },



    // MOD2 layer (special chars)
    {
        // Left half
        { _AT,      _NO,    _L_BRACKET, _R_BRACKET, _HASH ,     _NO},
        { _BSLASH,  _SLASH,	_L_BRACE,   _R_BRACE,   _ASTERIX ,  _NO},
        { _NO,      _DOLLAR,_BAR,		_TILDE,		_GRAVE ,    _NO},
        { _NO,      _NO,    _BSPACE,    _TAB,       _SPACE ,    _NO},
        // Right half
        { _NO, _EXCLAM, _LESS,       _GREATER,	_EQUAL, _AMPERSAND },
        { _NO, _QUESTION,_L_PAREN,  _R_PAREN, 	_PLUS,  _MINUS },
        { _NO, _CARET,  _PERCENT, 	_DQUOTE,    _SQUOTE,_NO  },
        { _NO, _ENTER, 	_ESC,       _DEL,   	_NO,    _NO,    }
    },
    // MOD3 layer (movement controls and numbers)
    {
        // Left half
        {    _BSPACE,   _PGUP,  _UP,   	_PGDN,	_DEL ,  _NO},
        {    _HOME,   	_LEFT,  _DOWN,  _RIGHT, _END ,  _NO},
        {    _ESC,  	_TAB, 	_INS,   _ENTER, _NO ,   _NO},
        {    _NO,   	_NO,    _NO,    _NO,    _NO ,   _NO},
        // Right half
        {   _NO,    _EXCLAM,_7,     _8,     _9,     _PLUS  },
        {   _NO,    _EQUAL, _4,     _5,     _6,     _MINUS },
        {   _NO,    _0,     _1,     _2,     _3,     _NO,  },
        {   _NO,    _NO,    _NO,    _NO,   _NO,     _NO}
    },
//MOUSE MODE
    {
    {   _q,     _u,     _o,     _b,     _p, _NO },
    {   _a,     _i,     _e,     _t,     _c, _NO },
    {   _NO,    _j,     _x,     _w,     _v, _NO },
    {   _MOD_MOUSE, _L_GUI, _L_ALT, _MOD_3,  _L_CTRL, _TAB },

    {   _NO,	_MS_SCROLL,	_NO,		_NO,		_NO,	_NO  },
    {   _NO,	_MS_BTN_1,	_MS_BTN_2,	_MS_BTN_3,	_NO,	_NO },
    {   _NO,    _NO,		_NO,		_NO,		_NO,	_NO},
    {   _NO,	_MOD_2,		_L_SHIFT,	_R_ALT,		_NO,	_MOD_COMPOSE }
},
// COMPOSE
{
        { _SSHARP,	_U_UML, _O_UML, _NO,	_NO,  _NO},
        { _A_UML,	_NO,    _NO,    _NO,    _NO ,   _NO},
        { _NO,   	_NO,    _NO,	_NO,    _NO ,   _NO},
        { _NO,   	_NO,    _NO,	_NO,    _NO ,   _NO},

        { _F1,		_F2,	_F3,	_F4,	_F5,	_F6  },
        { _F7,		_F8,	_F9,    _F10,	_F11,	_F12 },
        { _NO,		_NO,    _NO,    _NO,	_NO,	_NO  },
        { _NO,		_NO,    _NO,    _NO,	_NO,	_NO  }
    }
}; // end of matrix[][][]


#endif
