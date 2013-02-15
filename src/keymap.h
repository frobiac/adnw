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

#include <avr/eeprom.h>
#include <stdint.h>
#include <stdbool.h>
#include "hid_usage.h"
#include "config.h"

#define PINKYDROP 0  ///<  drop pinky column by one for more ergonomic layout


#define GEOGRAPHICAL_AREAS 3 // QWERTZ(de), QWERTY(gb), QWERTY(us)


typedef struct {
    uint16_t  hid;   ///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;  ///< HID modifiers , will be interpreted by OS keyboard layout!
} keycode;


typedef struct {
    InterimsKeycode ikc; // internal usage code, used for layout definition
    keycode kc[GEOGRAPHICAL_AREAS];
} local_keycode;

// gespeichert im EEPROM. ca. 100.000 Schreibzugriffe.
typedef enum
{
    QWERTZ_DE,
    KOY,
    Malt90,
    QWERTY_GB
} Layout;

EEMEM extern Layout alternateLayoutNr; // der Wert des aktiven Layouts; wird persistent gespeichert
typedef enum
{
    DE,
    GB,
    US
} GeoArea;
EEMEM extern GeoArea alternateGeoArea;  // der Wert des aktiven geografischen Bereichs; wird persistent gespeichert

typedef enum
{
    PC,
    Mac
} MacOrPC;
EEMEM extern MacOrPC altMacOrPC;  // der Wert des aktiven Rechnertyps; wird persistent gespeichert

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);
uint16_t getKeyCode (uint8_t row, uint8_t col, uint8_t layer);
uint8_t getKeyChar (uint8_t row, uint8_t col, uint8_t layer);
keycode getKeyStruct_Ic(InterimsKeycode ikc);
void    printLayout(uint8_t l);

#define _MACRO _no
#ifdef MALTRON_KW
  // Menu and Fn pressed simultaneouslye
  #define CMD_MODE() ((rowData[6] & (1<<4)) && (rowData[6] & (1<<3)) ) 

  #define LAYERS   3
  #define LAYOUTS  4
  #define ROWS     8  // F,Nr,3xChars,Subs,Thumb, Middle
  #define COLS    12  ///< cols of both keyboard halves are "stacked" in layout and physically connected
#endif


#ifdef BLUECUBE
  #define CMD_MODE() ((rowData[2] & (1<<0)) && (rowData[6] & (1<<5)) ) 

  #define LAYERS 5
  #define ROWS   8
  #define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected

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
//         0  5
// 2 3 4 5 1  4 0 1 2 3 4
//

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
  { t72,t73,t74,t75,t71,t70 } \
}
#endif

#ifdef HYPERNANO
  #define CMD_MODE() ( (rowData[3] & (1<<1)) && (rowData[7] & (1<<5)) ) 
 
  #define LAYERS 5
  #define ROWS   8
  #define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected

/*  Row/col matrix: (0-based)
 *  unused: 11,21,51,61
 *  innermost thumb buttons: 31,41,71,81 
 *  mapped to match BlueCube without 31 & 75 using the keycode from the outer lower pinky
 *  
 *     01 02 03 04 05 --  -- 41 42 43 44 45
 *    11 12 13 14 15 --    -- 51 52 53 54 55
 *   21 22 23 24 25 20      60 61 62 63 64 65
 *    31 32 33 34 35 30    70 71 72 73 74 75
 */

//_X, SHFT L_ALT LCTRL, SPACE, GUI, _MACRO,  MOD_3, MOD_2,  L_SHIFT, MOD_1,  AltGr, _A, _B

#define KEYMAP( no, \
  no00,k01,k02,k03,k04,k05,        k40,k41,k42,k43,k44,no45, \
  no10,k11,k12,k13,k14,k15,        k50,k51,k52,k53,k54,no55, \
  no20,k21,k22,k23,k24,k25,        k60,k61,k62,k63,k64,no65, \
/*                              M3 M2  SHF  M1  AGr A  B  */ \
   t30,t31,t32,t33,t34,t35,t20,t65,t70,t71,t72,t73,t74,t75   \
) { \
  {  no, k01,k02,k03,k04,k05 }, \
  {  no, k11,k12,k13,k14,k15 }, \
  { t20, k21,k22,k23,k24,k25 }, \
  { t35, /*t30*/no20,t31,t32,t33,t34 }, \
\
  {  no, k40, k41,k42,k43,k44 }, \
  {  no, k50, k51,k52,k53,k54 }, \
  { t65/*no00 ESC*/, k60, k61,k62,k63,k64 }, \
  { t70, t71, t72,t73,t74,/*t75*/no65 } \
}
#endif


/**
 * Mouse keycodes that temporarily override normal keycodes during trackpoint usage.
 * Mouse buttons can be mapped to good locations this way.
 * This is not to be confused with the mousekey implementation, which is activated by switching to its layer.
 *
 */
static const uint8_t MouseUsage[ROWS][COLS] =
  /*KEYMAP(*/{ 0,
 HID_ESC,  0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0 ,
 0, HID_MOUSEBTN_1, HID_MOUSEBTN_3, HID_MOUSEBTN_2, HID_MOUSEBTN_4, 0 , 0, 0, 0, 0, 0, 0 ,
 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0 ,
 0, 0, HID_ESC, HID_TAB,0, HID_MOUSEBTN_1,0, /* | */ HID_MOUSEBTN_3, HID_MOUSEBTN_2, HID_BACKSPACE, HID_ENTER, 0, 0, 0
};


#ifdef OLD_STRUCTURES

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
 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0 ,
 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0 ,
 0, 0, HID_ESC, HID_TAB,0, 0 /*is real SPACE*/,0, /* | */ 0, HID_ESC, HID_BACKSPACE, HID_ENTER, 0, 0, 0
);


static const keycode KeyMatrix[LAYERS][ROWS][COLS] PROGMEM = 
{
  // normal layer
 KEYMAP( _no,
         _MOD_3,   _k, _u, _q,     _PERIOD, _j,      _v, _g, _c, _l, _f, _BSPACE ,
         _TAB,   _h, _i, _e,     _a,      _o,      _d, _t, _r, _n, _s, _ENTER  ,
         _DQUOTE,_x, _y, _MINUS, _COMMA,  _SLASH,  _b, _p, _w, _m, _z, _MOD_MOUSEKEY,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,  _B  
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
  
  // MOD3 Fx layer
  KEYMAP( _no,
    _no, _no, _no, _no, _a_UML, _o_UML ,   _F1,      _F2,        _F3,        _F4,        _F5,    _F6     ,
    _no, _no, _no, _no, _no,    _no,       _F7,      _F8,        _F9,        _F10,       _F11,   _F12    ,
    _no, _no, _no, _no, _no,    _no,       _no,      _no,        _no,        _no,        _no,    _no     ,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 
    ),

  // MOD_MOUSE layer
  KEYMAP( _no,
    _no, _no, _MS_W_U,  _MS_U,  _MS_W_D,  _no,  _a_UML, _o_UML ,  _F1,      _F2,        _F3,        _F4,
    _no, _no, _MS_L, _MS_D, _MS_R, _no,  _X,     _MS_BTN1, _MS_BTN3, _MS_BTN2, _no, _no,
    _no, _no, _MS_W_L, _no,      _MS_W_R,  _no,  _no,    _MS_ACC0, _MS_ACC1, _MS_ACC2, _no, _no,
     _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B
    ),
/*  
  // MOD4 BU-TECK
  KEYMAP( _no,
    _ESC,   _b, _u, _q, _PERIOD,_x  , _p,   _c, _l, _m, _f, _BSPACE ,
    _TAB,   _h, _i, _e, _a, _o  , _d,   _t, _r, _n, _s, _ENTER  ,
    _DQUOTE,_k, _y, _MINUS, _COMMA, _SLASH , _j,   _g, _w, _v, _z, _no  ,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,     _B 
  )
*/
}; // end of matrix[][][]
#endif
    
static const uint16_t secondaryModifierUsageMatrix[LAYOUTS][ROWS][COLS] = {
    {   // LAYOUT 1: QWERTZ de
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { HID_TAB,   HID_SPACE, HID_BACKSPACE,0,     0,       /**/ HID_ENTER, HID_SPACE, HID_DELETE,0,         0,         0,         0 }, // nur 5 Tasten belegt pro Seite!!
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 }
    },
    {   // LAYOUT 2: KO.,Y de
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { HID_ESC,   0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { HID_TAB,   HID_SPACE, HID_BACKSPACE,0,     0,       /**/ HID_ENTER, HID_SPACE, HID_DELETE,0,         0,         0,         0 }, // nur 5 Tasten belegt pro Seite!!
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 }
    },
    {   // LAYOUT 3: Malt90 en
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { HID_E,HID_SPACE,HID_BACKSPACE,      0,     0,       /**/ HID_ENTER, HID_SPACE, HID_DELETE,0,         0,         0,         0 }, // nur 5 Tasten belegt pro Seite!!
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 }
    },
    {   // LAYOUT 4: QWERTY en
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 },
        { HID_TAB,HID_SPACE,HID_BACKSPACE,    0,     0,       /**/ HID_ENTER, HID_SPACE, HID_DELETE,0,         0,         0,         0 }, // nur 5 Tasten belegt pro Seite!!
        { 0,         0,         0,            0,     0,    0, /**/ 0,         0,         0,         0,         0,         0 }
    }
};


// on BlueCube, the innermost thumb-buttons are on outermost columns in matrix!
//         0  5
// 2 3 4 5 1  4 0 1 2 3
//
#define _ik_MACRO _ik_no

#define _ik_THUMB_ROW_LEFT_QWERTZ  _ik_L_CTRL,   _ik_L_SHIFT,            _ik_L_ALT,                \
                                _ik_MOD_2/*FN*/, _ik_APP
#define _ik_THUMB_ROW_RIGHT_QWERTZ _ik_R_CTRL,           _ik_R_SHIFT,    _ik_L_ALT,                 \
                                _ik_ALTGR /*_R_ALT*/, _ik_R_GUI,                              _ik_no,            _ik_no
#define _ik_THUMB_ROW_LEFT_NEO  _ik_MOD_1,   _ik_L_SHIFT,            _ik_MOD_2,                \
                                _ik_MOD_2/*FN*/, _ik_APP
#define _ik_THUMB_ROW_RIGHT_NEO _ik_MOD_1,           _ik_R_SHIFT,    _ik_MOD_2,                 \
                                _ik_ALTGR /*_R_ALT*/, _ik_R_GUI,                              _ik_no,            _ik_no

// #define _ik_THUMB_ROW_RIGHT _ik_L_SHIFT,      _ik_MOD_1,   _ik_R_ALT,  _ik_MOD_3,  _ik_MOD_2, _ik_MOD_3
#define _ik_EXTRA_KEYS  _ik_MAIL, _ik_CALCULATOR, _ik_MY_COMPUTER, _ik_WWW_SEARCH,_ik_F5,_ik_F6,  /**/ _ik_MEDIA_PREV_TRACK,_ik_MEDIA_PLAY_PAUSE,_ik_MEDIA_NEXT_TRACK,_ik_AUDIO_VOL_DOWN,_ik_AUDIO_VOL_UP,_ik_MEDIA_SELECT
// von unten nach oben
#define _ik_MIDDLE_COLUMN  _ik_INS, _ik_SYSTEM_POWER, _ik_AUDIO_MUTE, _ik_no, _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no

PROGMEM static const InterimsKeycode KeyMatrix[LAYOUTS][LAYERS][ROWS][COLS] = {
    {   // LAYOUT 1: QWERTZ de
        // normal layer
        {
            { _ik_F1,       _ik_F2,       _ik_F3,       _ik_F4,       _ik_F5,       _ik_F6,        /**/ _ik_F7,       _ik_F8,       _ik_F9,       _ik_F10,      _ik_F11,      _ik_F12    },
            { _ik_CARET,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_6,        _ik_7,        _ik_8,        _ik_9,        _ik_0,        _ik_SSHARP },
            { _ik_SQUOTE,   _ik_q,        _ik_w,        _ik_e,        _ik_r,        _ik_t,         /**/ _ik_z,        _ik_u,        _ik_i,        _ik_o,        _ik_p,        _ik_u_UML  },
            { _ik_ESC,      _ik_a,        _ik_s,        _ik_d,        _ik_f,        _ik_g,         /**/ _ik_h,        _ik_j,        _ik_k,        _ik_l,        _ik_o_UML,    _ik_a_UML  },
            { _ik_LESS,     _ik_y,        _ik_x,        _ik_c,        _ik_v,        _ik_b,         /**/ _ik_n,        _ik_m,        _ik_COMMA,    _ik_PERIOD,   _ik_MINUS,    _ik_PLUS   },
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,       _ik_LEFT,     _ik_RIGHT,    _ik_DOWN,     _ik_UP,       _ik_HASH   },
            {                                           _ik_THUMB_ROW_LEFT_QWERTZ , /**/ _ik_THUMB_ROW_RIGHT_QWERTZ                                        },
            {                                                            _ik_MIDDLE_COLUMN                                                              }
        },
        // dummy MOD3 layer
        {
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    }
        },
        // FN called MOD4 layer -  (movement controls and numbers, media control, layout switch, power, applications)
        {
            {                                                                _ik_EXTRA_KEYS                                                              },
            { _ik_CARET,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_POUND,     _ik_7,        _ik_TAB,      _ik_SLASH,    _ik_ASTERIX,  _ik_SSHARP },
            { _ik_no,       _ik_PGUP,     _ik_BSPACE,   _ik_UP,       _ik_DEL ,     _ik_PGDN,      /**/ _ik_EXCLAMI,   _ik_7,        _ik_8,        _ik_9,        _ik_PLUS,     _ik_MINUS  },
            { _ik_ESC,      _ik_HOME,     _ik_LEFT,     _ik_DOWN,     _ik_RIGHT,    _ik_END,       /**/ _ik_QUESTIONI, _ik_4,        _ik_5,        _ik_6,        _ik_COMMA,    _ik_PERIOD },
            { _ik_no,       _ik_ESC,      _ik_TAB,      _ik_INS,      _ik_ENTER,    _ik_UNDO,      /**/ _ik_REDO,      _ik_1,        _ik_2,        _ik_3,        _ik_no,       _ik_no     },
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,        _ik_LEFT,     _ik_0,        _ik_DOWN,     _ik_UP,       _ik_HASH   },
            {                                           _ik_THUMB_ROW_LEFT_QWERTZ , /**/ _ik_THUMB_ROW_RIGHT_QWERTZ                                         },
            {                                                            _ik_MIDDLE_COLUMN                                                               }
        }
    },
    {   // LAYOUT 2: KO.,Y de
        // KO.,Y (meins)
        {
            { _ik_F1,       _ik_F2,       _ik_F3,       _ik_F4,       _ik_F5,       _ik_F6,        /**/ _ik_F7,       _ik_F8,       _ik_F9,       _ik_F10,      _ik_F11,      _ik_F12    },
            { _ik_CARET,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_6,        _ik_7,        _ik_8,        _ik_9,        _ik_0,        _ik_MINUS  },
            { _ik_TAB,      _ik_k,        _ik_o,        _ik_PERIOD,   _ik_COMMA,    _ik_y,         /**/ _ik_p,        _ik_c,        _ik_l,        _ik_m,        _ik_b,        _ik_x      },
            { _ik_L_ALT,    _ik_h,        _ik_a,        _ik_e,        _ik_i,        _ik_u,         /**/ _ik_d,        _ik_t,        _ik_r,        _ik_n,        _ik_s,        _ik_SSHARP },
            { _ik_L_CTRL,   _ik_z,        _ik_q,        _ik_a_UML,    _ik_u_UML,    _ik_o_UML,     /**/ _ik_j,        _ik_g,        _ik_w,        _ik_v,        _ik_f,        _ik_R_CTRL },
            { _ik_GRAVE,    _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,       _ik_LEFT,     _ik_RIGHT,    _ik_DOWN,     _ik_UP,       _ik_DEGUE  },
            {                                           _ik_THUMB_ROW_LEFT_NEO ,    /**/ _ik_THUMB_ROW_RIGHT_NEO                                           },
            {                                                            _ik_MIDDLE_COLUMN                                                              }
        },
        //                                                                      //  BU-TECK
        //        { _ik_ESC,      _ik_b,        _ik_u,        _ik_q,        _ik_PERIOD,   _ik_x,         /**/ _ik_p,        _ik_c,        _ik_l,        _ik_m,        _ik_f,        _ik_BSPACE },
        //        { _ik_TAB,      _ik_h,        _ik_i,        _ik_e,        _ik_a,        _ik_o,         /**/ _ik_d,        _ik_t,        _ik_r,        _ik_n,        _ik_s,        _ik_ENTER  },
        //        { _ik_DQUOTE,   _ik_k,        _ik_y,        _ik_MINUS,    _ik_COMMA,    _ik_SLASH,     /**/ _ik_j,        _ik_g,        _ik_w,        _ik_v,        _ik_z,        _ik_no  },
        // MOD3 layer (special char)
        {
            { _ik_F1,       _ik_F2,       _ik_F3,       _ik_F4,       _ik_F5,       _ik_F6,        /**/ _ik_F7,       _ik_F8,       _ik_F9,       _ik_F10,      _ik_F11,      _ik_F12    },
            { _ik_CARET,    _ik_1,        _ik_2,        _ik_3,        _ik_SR_QUOTM, _ik_SL_QUOTM,  /**/ _ik_6,        _ik_7,        _ik_8,        _ik_9,        _ik_0,         _ik_MINUS },
            { _ik_no,       _ik_DOT3,      _ik_USCORE,  _ik_L_BRACKET, _ik_R_BRACKET,_ik_CARET,     /**/ _ik_EXCLAM,   _ik_LESS,     _ik_GREATER,  _ik_EQUAL,    _ik_AMPERSAND,_ik_no     },
            { _ik_ESC,      _ik_BSLASH,   _ik_SLASH,    _ik_L_BRACE,  _ik_R_BRACE,  _ik_ASTERIX,   /**/ _ik_QUESTION, _ik_L_PAREN,  _ik_R_PAREN,  _ik_MINUS,    _ik_COLON,    _ik_AT     },
            { _ik_no,       _ik_HASH,     _ik_DOLLAR,   _ik_PIPE,     _ik_TILDE,    _ik_GRAVE,     /**/ _ik_PLUS,     _ik_PERCENT,  _ik_DQUOTE,   _ik_SQUOTE,   _ik_SCOLON,   _ik_R_CTRL },
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,       _ik_LEFT,     _ik_RIGHT,    _ik_DOWN,     _ik_UP,       _ik_no     },
            {                                           _ik_THUMB_ROW_LEFT_NEO ,    /**/ _ik_THUMB_ROW_RIGHT_NEO                                           },
            {                                                            _ik_MIDDLE_COLUMN                                                              }
        },
        // MOD4 layer (movement controls and numbers)
        {
            {                                                                _ik_EXTRA_KEYS                                                             },
            { _ik_CARET,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_POUND,    _ik_7,        _ik_TAB,      _ik_SLASH,    _ik_ASTERIX,  _ik_MINUS  },
            { _ik_no,       _ik_PGUP,     _ik_BSPACE,   _ik_UP,       _ik_DEL ,     _ik_PGDN,      /**/ _ik_EXCLAMI,  _ik_7,        _ik_8,        _ik_9,        _ik_PLUS,     _ik_MINUS  },
            { _ik_ESC,      _ik_HOME,     _ik_LEFT,     _ik_DOWN,     _ik_RIGHT,    _ik_END,       /**/ _ik_QUESTIONI,_ik_4,        _ik_5,        _ik_6,        _ik_COMMA,    _ik_PERIOD },
            { _ik_no,       _ik_ESC,      _ik_TAB,      _ik_INS,      _ik_ENTER,    _ik_UNDO,      /**/ _ik_REDO,     _ik_1,        _ik_2,        _ik_3,        _ik_no,       _ik_no     },
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,       _ik_LEFT,     _ik_0,        _ik_DOWN,     _ik_UP,       _ik_HASH   },
            {                                           _ik_THUMB_ROW_LEFT_NEO ,    /**/ _ik_THUMB_ROW_RIGHT_NEO                                           },
            {                                                            _ik_MIDDLE_COLUMN                                                              }
        }
    },
    {   // LAYOUT 3: Malt90en
        // normal layer
        {
            { _ik_F1,       _ik_F2,       _ik_F3,       _ik_F4,       _ik_F5,       _ik_F6,        /**/ _ik_F7,       _ik_F8,       _ik_F9,       _ik_F10,      _ik_F11,      _ik_F12    },
            { _ik_GRAVE,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_6,        _ik_7,        _ik_8,        _ik_9,        _ik_0,        _ik_ESC     },
            { _ik_EQUAL,    _ik_q,        _ik_p,        _ik_y,        _ik_c,        _ik_b,         /**/ _ik_v,        _ik_m,        _ik_u,        _ik_z,        _ik_l,        _ik_L_BRACKET},
            { _ik_TAB,      _ik_a,        _ik_n,        _ik_i,        _ik_s,        _ik_f,         /**/ _ik_d,        _ik_t,        _ik_h,        _ik_o,        _ik_r,        _ik_SQUOTE },
            { _ik_BSLASH,   _ik_COMMA,    _ik_PERIOD,   _ik_j,        _ik_g,        _ik_SLASH,     /**/ _ik_SCOLON,   _ik_w,        _ik_k,        _ik_MINUS,    _ik_x,        _ik_R_BRACKET},
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,       _ik_LEFT,     _ik_RIGHT,    _ik_DOWN,     _ik_UP,       _ik_HASH   },
            {                                           _ik_THUMB_ROW_LEFT_QWERTZ , /**/ _ik_THUMB_ROW_RIGHT_QWERTZ                                        },
            {                                                            _ik_MIDDLE_COLUMN                                                              }
        },
        // dummy MOD3 layer
        {
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    }
        },
        // FN called MOD4 layer -  (movement controls and numbers, media control, layout switch, power, applications)
        {
            {                                                                _ik_EXTRA_KEYS                                                              },
            { _ik_CARET,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_POUND,     _ik_7,        _ik_TAB,      _ik_SLASH,    _ik_ASTERIX,  _ik_no     },
            { _ik_no,       _ik_PGUP,     _ik_BSPACE,   _ik_UP,       _ik_DEL ,     _ik_PGDN,      /**/ _ik_EXCLAMI,   _ik_7,        _ik_8,        _ik_9,        _ik_PLUS,     _ik_MINUS  },
            { _ik_TAB,      _ik_HOME,     _ik_LEFT,     _ik_DOWN,     _ik_RIGHT,    _ik_END,       /**/ _ik_QUESTIONI, _ik_4,        _ik_5,        _ik_6,        _ik_COMMA,    _ik_PERIOD },
            { _ik_no,       _ik_ESC,      _ik_TAB,      _ik_INS,      _ik_ENTER,    _ik_UNDO,      /**/ _ik_REDO,      _ik_1,        _ik_2,        _ik_3,        _ik_no,       _ik_no     },
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,        _ik_LEFT,     _ik_0,        _ik_DOWN,     _ik_UP,       _ik_HASH   },
            {                                           _ik_THUMB_ROW_LEFT_QWERTZ , /**/ _ik_THUMB_ROW_RIGHT_QWERTZ                                         },
            {                                                            _ik_MIDDLE_COLUMN                                                               }
        }
    },
    {   // LAYOUT 4: QWERTY en
        // normal layer
        {
            { _ik_F1,       _ik_F2,       _ik_F3,       _ik_F4,       _ik_F5,       _ik_F6,        /**/ _ik_F7,       _ik_F8,       _ik_F9,       _ik_F10,      _ik_F11,      _ik_F12    },
            { _ik_GRAVE,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_6,        _ik_7,        _ik_8,        _ik_9,        _ik_0,        _ik_MINUS  },
            { _ik_EQUAL,    _ik_q,        _ik_w,        _ik_e,        _ik_r,        _ik_t,         /**/ _ik_y,        _ik_u,        _ik_i,        _ik_o,        _ik_p,        _ik_L_BRACKET},
            { _ik_ESC,      _ik_a,        _ik_s,        _ik_d,        _ik_f,        _ik_g,         /**/ _ik_h,        _ik_j,        _ik_k,        _ik_l,        _ik_SCOLON,   _ik_SQUOTE },
            { _ik_BSLASH,   _ik_z,        _ik_x,        _ik_c,        _ik_v,        _ik_b,         /**/ _ik_n,        _ik_m,        _ik_COMMA,    _ik_PERIOD,   _ik_SLASH,    _ik_R_BRACKET},
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,       _ik_LEFT,     _ik_RIGHT,    _ik_DOWN,     _ik_UP,       _ik_HASH   },
            {                                           _ik_THUMB_ROW_LEFT_QWERTZ , /**/ _ik_THUMB_ROW_RIGHT_QWERTZ                                        },
            {                                                            _ik_MIDDLE_COLUMN                                                              }
        },
        // dummy MOD3 layer
        {
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    },
            { _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,       _ik_no,        /**/ _ik_no,       _ik_no,       _ik_no,       _ik_no,      _ik_no,      _ik_no    }
        },
        // FN called MOD4 layer -  (movement controls and numbers, media control, layout switch, power, applications)
        {
            {                                                                _ik_EXTRA_KEYS                                                                },
            { _ik_CARET,    _ik_1,        _ik_2,        _ik_3,        _ik_4,        _ik_5,         /**/ _ik_POUND,        _ik_7,       _ik_TAB,      _ik_SLASH,    _ik_ASTERIX,  _ik_no     },
            { _ik_no,       _ik_PGUP,     _ik_BSPACE,   _ik_UP,       _ik_DEL ,     _ik_PGDN,      /**/ _ik_EXCLAMI,      _ik_7,       _ik_8,        _ik_9,        _ik_PLUS,     _ik_MINUS  },
            { _ik_ESC,      _ik_HOME,     _ik_LEFT,     _ik_DOWN,     _ik_RIGHT,    _ik_END,       /**/ _ik_QUESTIONI,   _ik_4,        _ik_5,        _ik_6,        _ik_COMMA,    _ik_PERIOD },
            { _ik_no,       _ik_ESC,      _ik_TAB,      _ik_INS,      _ik_ENTER,    _ik_UNDO,      /**/ _ik_REDO,        _ik_1,        _ik_2,        _ik_3,        _ik_no,       _ik_no     },
            { _ik_L_CTRL,   _ik_L_SHIFT,  _ik_PRTSC,    _ik_PGDN,     _ik_PGUP,     _ik_no,        /**/ _ik_no,          _ik_LEFT,     _ik_0,        _ik_DOWN,     _ik_UP,       _ik_HASH   },
            {                                           _ik_THUMB_ROW_LEFT_QWERTZ , /**/ _ik_THUMB_ROW_RIGHT_QWERTZ                                           },
            {                                                            _ik_MIDDLE_COLUMN                                                                 }
        }
    }
}; // end of layouts


/// @see Modifier in hid_usage.h
#define L_CTL (1<<0)
#define L_SHF (1<<1)
#define L_ALT (1<<2)
#define L_GUI (1<<3)
#define R_CTL (1<<4)
#define R_GUI (1<<7)


PROGMEM static const local_keycode localizedMacDifferences[] = {
    // Index                  DE                          GB                          US
    {_ik_UNDO              , {{HID_Y,        R_GUI      },{HID_Z,        L_GUI      },{HID_Z,        L_GUI      }}  },
    {_ik_REDO              , {{HID_Y,    R_GUI | L_SHF  },{HID_Z,    L_GUI | L_SHF  },{HID_Z,    L_GUI | L_SHF  }}  },
    {_ik_BSLASH            , {{0    , 0                 },{HID_NON_US_1,        0   },{HID_NON_US_1,     0      }}  },
    {_ik_HASH              , {{0,                0      },{HID_3,       L_ALT       },{0,0                      }}  },
    {_ik_PIPE              , {{HID_GRAVE,        ALTGR  },{HID_GRAVE,        L_SHF  },{HID_GRAVE,        L_SHF  }}  },
    {_ik_DEGUE             , {{0    , 0                 },{HID_E,       L_ALT       },{HID_E,       L_ALT       }}  },
    {_ik_GRAVE             , {{0    , 0                 },{HID_GRAVE,    L_ALT      },{HID_GRAVE,    L_ALT      }}  },
    {_ik_GREATER           , {{HID_GRAVE,        L_SHF  },{0    , 0                 },{0    , 0                 }}  },
    {_ik_LESS              , {{HID_GRAVE,        0      },{0    , 0                 },{0    , 0                 }}  },
    {_ik_DEGREE            , {{HID_NON_US_2,     L_SHF  },{0    , 0                 },{0    , 0                 }}  },
    {_ik_CARET             , {{HID_NON_US_2,     0      },{0    , 0                 },{0    , 0                 }}  },
    {_ik_SYSTEM_POWER      , {{HID_MAC_POWER,    0      },{HID_MAC_POWER,    0      },{HID_MAC_POWER,    0      }}  }
};
#define MAC_DIFFS_COUNT sizeof(localizedMacDifferences)/sizeof(local_keycode)

PROGMEM static const local_keycode localizationMatrix[_ik_NUMBERoF]   = {
    // Index                  DE                          GB                          US
    {_ik_no                , {{0    , 0                 },{0    , 0                 },{0    , 0                 }}  },
    {_ik_1                 , {{HID_1, 0                 },{HID_1, 0                 },{HID_1, 0                 }}  },
    {_ik_EXCLAM            , {{HID_1, L_SHF             },{HID_1, L_SHF             },{HID_1, L_SHF             }}  },
    {_ik_EXCLAMI           , {{HID_1, ALTGR             },{HID_1, ALTGR             },{HID_1, ALTGR             }}  },
    {_ik_2                 , {{HID_2, 0                 },{HID_2, 0                 },{HID_2, 0                 }}  },
    {_ik_3                 , {{HID_3, 0                 },{HID_3, 0                 },{HID_3, 0                 }}  },
    {_ik_4                 , {{HID_4, 0                 },{HID_4, 0                 },{HID_4, 0                 }}  },
    {_ik_DOLLAR            , {{HID_4, L_SHF             },{HID_4, L_SHF             },{HID_4, L_SHF             }}  },
    {_ik_5                 , {{HID_5, 0                 },{HID_5, 0                 },{HID_5, 0                 }}  },
    {_ik_PERCENT           , {{HID_5, L_SHF             },{HID_5, L_SHF             },{HID_5, L_SHF             }}  },
    {_ik_6                 , {{HID_6, 0                 },{HID_6, 0                 },{HID_6, 0                 }}  },
    {_ik_7                 , {{HID_7, 0                 },{HID_7, 0                 },{HID_7, 0                 }}  },
    {_ik_8                 , {{HID_8, 0                 },{HID_8, 0                 },{HID_8, 0                 }}  },
    {_ik_9                 , {{HID_9, 0                 },{HID_9, 0                 },{HID_9, 0                 }}  },
    {_ik_0                 , {{HID_0, 0                 },{HID_0, 0                 },{HID_0, 0                 }}  },
    {_ik_q                 , {{HID_Q, 0                 },{HID_Q, 0                 },{HID_Q, 0                 }}  },
    {_ik_Q                 , {{HID_Q, L_SHF             },{HID_Q, L_SHF             },{HID_Q, L_SHF             }}  },
    {_ik_w                 , {{HID_W, 0                 },{HID_W, 0                 },{HID_W, 0                 }}  },
    {_ik_W                 , {{HID_W, L_SHF             },{HID_W, L_SHF             },{HID_W, L_SHF             }}  },
    {_ik_e                 , {{HID_E, 0                 },{HID_E, 0                 },{HID_E, 0                 }}  },
    {_ik_E                 , {{HID_E, L_SHF             },{HID_E, L_SHF             },{HID_E, L_SHF             }}  },
    {_ik_r                 , {{HID_R, 0                 },{HID_R, 0                 },{HID_R, 0                 }}  },
    {_ik_R                 , {{HID_R, L_SHF             },{HID_R, L_SHF             },{HID_R, L_SHF             }}  },
    {_ik_t                 , {{HID_T, 0                 },{HID_T, 0                 },{HID_T, 0                 }}  },
    {_ik_T                 , {{HID_T, L_SHF             },{HID_T, L_SHF             },{HID_T, L_SHF             }}  },
    {_ik_u                 , {{HID_U, 0                 },{HID_U, 0                 },{HID_U, 0                 }}  },
    {_ik_U                 , {{HID_U, L_SHF             },{HID_U, L_SHF             },{HID_U, L_SHF             }}  },
    {_ik_i                 , {{HID_I, 0                 },{HID_I, 0                 },{HID_I, 0                 }}  },
    {_ik_I                 , {{HID_I, L_SHF             },{HID_I, L_SHF             },{HID_I, L_SHF             }}  },
    {_ik_o                 , {{HID_O, 0                 },{HID_O, 0                 },{HID_O, 0                 }}  },
    {_ik_O                 , {{HID_O, L_SHF             },{HID_O, L_SHF             },{HID_O, L_SHF             }}  },
    {_ik_p                 , {{HID_P, 0                 },{HID_P, 0                 },{HID_P, 0                 }}  },
    {_ik_P                 , {{HID_P, L_SHF             },{HID_P, L_SHF             },{HID_P, L_SHF             }}  },
    {_ik_a                 , {{HID_A, 0                 },{HID_A, 0                 },{HID_A, 0                 }}  },
    {_ik_A                 , {{HID_A, L_SHF             },{HID_A, L_SHF             },{HID_A, L_SHF             }}  },
    {_ik_s                 , {{HID_S, 0                 },{HID_S, 0                 },{HID_S, 0                 }}  },
    {_ik_S                 , {{HID_S, L_SHF             },{HID_S, L_SHF             },{HID_S, L_SHF             }}  },
    {_ik_d                 , {{HID_D, 0                 },{HID_D, 0                 },{HID_D, 0                 }}  },
    {_ik_D                 , {{HID_D, L_SHF             },{HID_D, L_SHF             },{HID_D, L_SHF             }}  },
    {_ik_f                 , {{HID_F, 0                 },{HID_F, 0                 },{HID_F, 0                 }}  },
    {_ik_F                 , {{HID_F, L_SHF             },{HID_F, L_SHF             },{HID_F, L_SHF             }}  },
    {_ik_g                 , {{HID_G, 0                 },{HID_G, 0                 },{HID_G, 0                 }}  },
    {_ik_G                 , {{HID_G, L_SHF             },{HID_G, L_SHF             },{HID_G, L_SHF             }}  },
    {_ik_h                 , {{HID_H, 0                 },{HID_H, 0                 },{HID_H, 0                 }}  },
    {_ik_H                 , {{HID_H, L_SHF             },{HID_H, L_SHF             },{HID_H, L_SHF             }}  },
    {_ik_j                 , {{HID_J, 0                 },{HID_J, 0                 },{HID_J, 0                 }}  },
    {_ik_J                 , {{HID_J, L_SHF             },{HID_J, L_SHF             },{HID_J, L_SHF             }}  },
    {_ik_k                 , {{HID_K, 0                 },{HID_K, 0                 },{HID_K, 0                 }}  },
    {_ik_K                 , {{HID_K, L_SHF             },{HID_K, L_SHF             },{HID_K, L_SHF             }}  },
    {_ik_l                 , {{HID_L, 0                 },{HID_L, 0                 },{HID_L, 0                 }}  },
    {_ik_L                 , {{HID_L, L_SHF             },{HID_L, L_SHF             },{HID_L, L_SHF             }}  },
    {_ik_x                 , {{HID_X, 0                 },{HID_X, 0                 },{HID_X, 0                 }}  },
    {_ik_X                 , {{HID_X, L_SHF             },{HID_X, L_SHF             },{HID_X, L_SHF             }}  },
    {_ik_c                 , {{HID_C, 0                 },{HID_C, 0                 },{HID_C, 0                 }}  },
    {_ik_C                 , {{HID_C, L_SHF             },{HID_C, L_SHF             },{HID_C, L_SHF             }}  },
    {_ik_v                 , {{HID_V, 0                 },{HID_V, 0                 },{HID_V, 0                 }}  },
    {_ik_V                 , {{HID_V, L_SHF             },{HID_V, L_SHF             },{HID_V, L_SHF             }}  },
    {_ik_b                 , {{HID_B, 0                 },{HID_B, 0                 },{HID_B, 0                 }}  },
    {_ik_B                 , {{HID_B, L_SHF             },{HID_B, L_SHF             },{HID_B, L_SHF             }}  },
    {_ik_n                 , {{HID_N, 0                 },{HID_N, 0                 },{HID_N, 0                 }}  },
    {_ik_N                 , {{HID_N, L_SHF             },{HID_N, L_SHF             },{HID_N, L_SHF             }}  },
    {_ik_m                 , {{HID_M, 0                 },{HID_M, 0                 },{HID_M, 0                 }}  },
    {_ik_M                 , {{HID_M, L_SHF             },{HID_M, L_SHF             },{HID_M, L_SHF             }}  },
    {_ik_CAPS              , {{HID_CAPS_LOCK,0          },{HID_CAPS_LOCK,0          },{HID_CAPS_LOCK,0          }}  },
    {_ik_APP               , {{HID_APPLICATION, 0       },{HID_APPLICATION, 0       },{HID_APPLICATION, 0       }}  },
    {_ik_BSPACE            , {{HID_BACKSPACE,0          },{HID_BACKSPACE,0          },{HID_BACKSPACE,0          }}  },
    {_ik_TAB               , {{HID_TAB,      0          },{HID_TAB,      0          },{HID_TAB,      0          }}  },
    {_ik_noN_US_1          , {{HID_NON_US_1, 0          },{HID_NON_US_1, 0          },{HID_NON_US_1, 0          }}  },
    {_ik_noN_US_2          , {{HID_NON_US_2, 0          },{HID_NON_US_2, 0          },{HID_NON_US_2, 0          }}  },
    {_ik_ENTER             , {{HID_ENTER,    0          },{HID_ENTER,    0          },{HID_ENTER,    0          }}  },
    {_ik_COMMA             , {{HID_COMMA,    0          },{HID_COMMA,    0          },{HID_COMMA,    0          }}  },
    {_ik_PERIOD            , {{HID_PERIOD,   0          },{HID_PERIOD,   0          },{HID_PERIOD,   0          }}  },
    {_ik_DOT3              , {{HID_PERIOD,   ALTGR      },{HID_PERIOD,   ALTGR      },{HID_PERIOD,   ALTGR      }}  },
    {_ik_SPACE             , {{HID_SPACE,    0          },{HID_SPACE,    0          },{HID_SPACE,    0          }}  },
    {_ik_L_SHIFT           , {{HID_L_SHIFT,  MOD_L_SHIFT},{HID_L_SHIFT,  MOD_L_SHIFT},{HID_L_SHIFT,  MOD_L_SHIFT}}  },
    {_ik_R_SHIFT           , {{HID_R_SHIFT,  MOD_R_SHIFT},{HID_R_SHIFT,  MOD_R_SHIFT},{HID_R_SHIFT,  MOD_R_SHIFT}}  },
    {_ik_L_CTRL            , {{HID_L_CONTROL,MOD_L_CTRL },{HID_L_CONTROL,MOD_L_CTRL },{HID_L_CONTROL,MOD_L_CTRL }}  },
    {_ik_R_CTRL            , {{HID_R_CONTROL,MOD_R_CTRL },{HID_R_CONTROL,MOD_R_CTRL },{HID_R_CONTROL,MOD_R_CTRL }}  },
    {_ik_L_ALT             , {{HID_L_ALT,    MOD_L_ALT  },{HID_L_ALT,    MOD_L_ALT  },{HID_L_ALT,    MOD_L_ALT  }}  },
    {_ik_R_ALT/*_ik_ALTGR*/, {{HID_R_ALT,    MOD_R_ALT  },{HID_R_ALT,    MOD_R_ALT  },{HID_R_ALT,    MOD_R_ALT  }}  },
    {_ik_L_GUI             , {{HID_L_GUI,    MOD_L_GUI  },{HID_L_GUI,    MOD_L_GUI  },{HID_L_GUI,    MOD_L_GUI  }}  },
    {_ik_R_GUI             , {{HID_R_GUI,    MOD_R_GUI  },{HID_R_GUI,    MOD_R_GUI  },{HID_R_GUI,    MOD_R_GUI  }}  },
    {_ik_MOD_0             , {{HID_NO_KEY,   MOD_LAYER_0},{HID_NO_KEY,   MOD_LAYER_0},{HID_NO_KEY,   MOD_LAYER_0}}  },
    {_ik_MOD_1             , {{HID_NO_KEY,   MOD_LAYER_1},{HID_NO_KEY,   MOD_LAYER_1},{HID_NO_KEY,   MOD_LAYER_1}}  },
    {_ik_MOD_2             , {{HID_NO_KEY,   MOD_LAYER_2},{HID_NO_KEY,   MOD_LAYER_2},{HID_NO_KEY,   MOD_LAYER_2}}  },
    {_ik_MOD_3             , {{HID_NO_KEY,   MOD_LAYER_3},{HID_NO_KEY,   MOD_LAYER_3},{HID_NO_KEY,   MOD_LAYER_3}}  },
//    {_ik_MOUSE             , {{HID_NO_KEY,   MOD_MOUSE  },{HID_NO_KEY,   MOD_MOUSE  },{HID_NO_KEY,   MOD_MOUSE  }}  },
    {_ik_COMPOSE           , {{HID_NO_KEY,   MOD_COMPOSE},{HID_NO_KEY,   MOD_COMPOSE},{HID_NO_KEY,   MOD_COMPOSE}}  },
//    {_ik_MS_SCROLL         , {{ MS_SCROLL,   0,         },{ MS_SCROLL,   0,         },{ MS_SCROLL,   0,         }}  },
//    {_ik_MS_BTN_1          , {{ MS_BTN_1,    0,         },{ MS_BTN_1,    0,         },{ MS_BTN_1,    0,         }}  },
//    {_ik_MS_BTN_2          , {{ MS_BTN_2,    0,         },{ MS_BTN_2,    0,         },{ MS_BTN_2,    0,         }}  },
//    {_ik_MS_BTN_3          , {{ MS_BTN_3,    0,         },{ MS_BTN_3,    0,         },{ MS_BTN_3,    0,         }}  },
    {_ik_INS               , {{HID_INSERT,   0          },{HID_INSERT,   0          },{HID_INSERT,   0          }}  },
    {_ik_DEL               , {{HID_DELETE,   0          },{HID_DELETE,   0          },{HID_DELETE,   0          }}  },
    {_ik_LEFT              , {{HID_LEFT,     0          },{HID_LEFT,     0          },{HID_LEFT,     0          }}  },
    {_ik_HOME              , {{HID_HOME,     0          },{HID_HOME,     0          },{HID_HOME,     0          }}  },
    {_ik_END               , {{HID_END,      0          },{HID_END,      0          },{HID_END,      0          }}  },
    {_ik_UP                , {{HID_UP,       0          },{HID_UP,       0          },{HID_UP,       0          }}  },
    {_ik_DOWN              , {{HID_DOWN,     0          },{HID_DOWN,     0          },{HID_DOWN,     0          }}  },
    {_ik_PGUP              , {{HID_PAGE_UP,  0          },{HID_PAGE_UP,  0          },{HID_PAGE_UP,  0          }}  },
    {_ik_PGDN              , {{HID_PAGE_DOWN,0          },{HID_PAGE_DOWN,0          },{HID_PAGE_DOWN,0          }}  },
    {_ik_RIGHT             , {{HID_RIGHT,    0          },{HID_RIGHT,    0          },{HID_RIGHT,    0          }}  },
    {_ik_NUMLOCK           , {{HID_NUM_LOCK,0           },{HID_NUM_LOCK,0           },{HID_NUM_LOCK,0           }}  },
    {_ik_PRTSC             , {{HID_PRINTSCREEN,0        },{HID_PRINTSCREEN,0        },{HID_PRINTSCREEN,0        }}  },
    {_ik_ESC               , {{HID_ESC, 0               },{HID_ESC, 0               },{HID_ESC, 0               }}  },
    {_ik_F1                , {{HID_F1,  0               },{HID_F1,  0               },{HID_F1,  0               }}  },
    {_ik_F2                , {{HID_F2,  0               },{HID_F2,  0               },{HID_F2,  0               }}  },
    {_ik_F3                , {{HID_F3,  0               },{HID_F3,  0               },{HID_F3,  0               }}  },
    {_ik_F4                , {{HID_F4,  0               },{HID_F4,  0               },{HID_F4,  0               }}  },
    {_ik_F5                , {{HID_F5,  0               },{HID_F5,  0               },{HID_F5,  0               }}  },
    {_ik_F6                , {{HID_F6,  0               },{HID_F6,  0               },{HID_F6,  0               }}  },
    {_ik_F7                , {{HID_F7,  0               },{HID_F7,  0               },{HID_F7,  0               }}  },
    {_ik_F8                , {{HID_F8,  0               },{HID_F8,  0               },{HID_F8,  0               }}  },
    {_ik_F9                , {{HID_F9,  0               },{HID_F9,  0               },{HID_F9,  0               }}  },
    {_ik_F10               , {{HID_F10, 0               },{HID_F10, 0               },{HID_F10, 0               }}  },
    {_ik_F11               , {{HID_F11, 0               },{HID_F11, 0               },{HID_F11, 0               }}  },
    {_ik_F12               , {{HID_F12, 0               },{HID_F12, 0               },{HID_F12, 0               }}  },
    {_ik_SYSTEM_POWER      , {{HID_SYSTEM_POWER_DOWN,0        },{HID_SYSTEM_POWER_DOWN,0        },{HID_SYSTEM_POWER_DOWN,0        }}  },
    {_ik_SYSTEM_SLEEP      , {{HID_SYSTEM_SLEEP,0        },{HID_SYSTEM_SLEEP,0        },{HID_SYSTEM_SLEEP,0        }}  },
    {_ik_SYSTEM_WAKE       , {{HID_SYSTEM_WAKE_UP,0         },{HID_SYSTEM_WAKE_UP,0         },{HID_SYSTEM_WAKE_UP,0         }}  },
    {_ik_AUDIO_MUTE        , {{HID_AUDIO_MUTE,0          },{HID_AUDIO_MUTE,0          },{HID_AUDIO_MUTE,0          }}  },
    {_ik_AUDIO_VOL_UP      , {{HID_AUDIO_VOL_UP,0        },{HID_AUDIO_VOL_UP,0        },{HID_AUDIO_VOL_UP,0        }}  },
    {_ik_AUDIO_VOL_DOWN    , {{HID_AUDIO_VOL_DOWN,0      },{HID_AUDIO_VOL_DOWN,0      },{HID_AUDIO_VOL_DOWN,0      }}  },
    {_ik_MEDIA_NEXT_TRACK  , {{HID_TRANSPORT_NEXT_TRACK,0    },{HID_TRANSPORT_NEXT_TRACK,0    },{HID_TRANSPORT_NEXT_TRACK,0    }}  },
    {_ik_MEDIA_PREV_TRACK  , {{HID_TRANSPORT_PREV_TRACK,0    },{HID_TRANSPORT_PREV_TRACK,0    },{HID_TRANSPORT_PREV_TRACK,0    }}  },
    {_ik_MEDIA_STOP        , {{HID_TRANSPORT_STOP,0          },{HID_TRANSPORT_STOP,0          },{HID_TRANSPORT_STOP,0          }}  },
    {_ik_MEDIA_PLAY_PAUSE  , {{HID_TRANSPORT_PLAY_PAUSE,0    },{HID_TRANSPORT_PLAY_PAUSE,0    },{HID_TRANSPORT_PLAY_PAUSE,0    }}  },
    {_ik_MEDIA_SELECT      , {{HID_AL_CC_CONFIG,0        },{HID_AL_CC_CONFIG,0        },{HID_AL_CC_CONFIG,0        }}  },
    {_ik_MAIL              , {{HID_AL_EMAIL,0                },{HID_AL_EMAIL,0                },{HID_AL_EMAIL,0                }}  },
    {_ik_CALCULATOR        , {{HID_AL_CALCULATOR,0          },{HID_AL_CALCULATOR,0          },{HID_AL_CALCULATOR,0          }}  },
    {_ik_MY_COMPUTER       , {{HID_AL_LOCAL_BROWSER,0         },{HID_AL_LOCAL_BROWSER,0         },{HID_AL_LOCAL_BROWSER,0         }}  },
    {_ik_WWW_SEARCH        , {{HID_AC_SEARCH,0          },{HID_AC_SEARCH,0          },{HID_AC_SEARCH,0          }}  },
    {_ik_WWW_HOME          , {{HID_AC_HOME,0            },{HID_AC_HOME,0            },{HID_AC_HOME,0            }}  },
    {_ik_WWW_BACK          , {{HID_AC_BACK,0            },{HID_AC_BACK,0            },{HID_AC_BACK,0            }}  },
    {_ik_WWW_FORWARD       , {{HID_AC_FORWARD,0         },{HID_AC_FORWARD,0         },{HID_AC_FORWARD,0         }}  },
    {_ik_WWW_STOP          , {{HID_AC_STOP,0            },{HID_AC_STOP,0            },{HID_AC_STOP,0            }}  },
    {_ik_WWW_REFRESH       , {{HID_AC_REFRESH,0         },{HID_AC_REFRESH,0         },{HID_AC_REFRESH,0         }}  },
    {_ik_WWW_FAVORITES     , {{HID_AC_BOOKMARKS,0       },{HID_AC_BOOKMARKS,0       },{HID_AC_BOOKMARKS,0       }}  },
    {_ik_UNDO              , {{HID_Y,        R_CTL      },{HID_Z,        R_CTL      },{HID_Z,        R_CTL      }}  },
    {_ik_REDO              , {{HID_Z,        R_CTL      },{HID_Y,    R_CTL          },{HID_Y,    R_CTL          }}  },
    {_ik_BSLASH            , {{HID_MINUS,        ALTGR  },{HID_NON_US_2,        0   },{HID_BSLASH,    0         }}  },
    {_ik_PIPE              , {{HID_NON_US_2,     ALTGR  },{HID_NON_US_2,     L_SHF  },{HID_BSLASH,       L_SHF  }}  },
    {_ik_GRAVE             , {{HID_EQUAL,        L_SHF  },{HID_GRAVE,        0      },{HID_GRAVE,    0          }}  },
    {_ik_DEGUE             , {{HID_EQUAL,               },{HID_GRAVE,        L_SHF  },{HID_NON_US_2,     L_SHF  }}  },
    {_ik_POUND             , {{HID_4,L_SHF |L_ALT       },{HID_3,L_SHF              },{HID_3,L_SHF              }}  },
    {_ik_SL_QUOTM          , {{HID_B,L_SHF |L_ALT |L_GUI},{HID_3,L_SHF |L_ALT       },{HID_3,L_SHF |L_ALT       }}  },
    {_ik_SR_QUOTM          , {{HID_N,L_SHF |L_ALT |L_GUI},{HID_4,L_SHF |L_ALT       },{HID_4,L_SHF |L_ALT       }}  },
    {_ik_AMPERSAND         , {{HID_6,            L_SHF  },{HID_7,         L_SHF     },{HID_7,         L_SHF     }}  },
    {_ik_ASTERIX           , {{HID_R_BRACKET,    L_SHF  },{HID_8,         L_SHF     },{HID_8,         L_SHF     }}  },
    {_ik_AT                , {{HID_Q,            ALTGR  },{HID_2,         L_SHF     },{HID_2,         L_SHF     }}  },
    {_ik_CARET             , {{HID_GRAVE,        0      },{HID_6,         L_SHF     },{HID_6,         L_SHF     }}  },
    {_ik_COLON             , {{HID_PERIOD,       L_SHF  },{HID_SEMICOLON, L_SHF     },{HID_SEMICOLON, L_SHF     }}  },
    {_ik_DQUOTE            , {{HID_2,            L_SHF  },{HID_QUOTE,     L_SHF     },{HID_QUOTE,     L_SHF     }}  },
    {_ik_EQUAL             , {{HID_0,            L_SHF  },{HID_EQUAL,     0,        },{HID_EQUAL,     0,        }}  },
    {_ik_GREATER           , {{HID_NON_US_2,     L_SHF  },{HID_PERIOD,    L_SHF     },{HID_PERIOD,    L_SHF     }}  },
    {_ik_HASH              , {{HID_BSLASH,       0      },{HID_NON_US_1,    0       },{HID_3,    L_SHF          }}  },
    {_ik_L_BRACE           , {{HID_7,            ALTGR  },{HID_L_BRACKET, L_SHF     },{HID_L_BRACKET, L_SHF     }}  },
    {_ik_L_BRACKET         , {{HID_8,            ALTGR  },{HID_L_BRACKET, 0         },{HID_L_BRACKET, 0         }}  },
    {_ik_LESS              , {{HID_NON_US_2,     0      },{HID_COMMA ,    L_SHF     },{HID_COMMA ,    L_SHF     }}  },
    {_ik_L_PAREN           , {{HID_8,            L_SHF  },{HID_9,         L_SHF     },{HID_9,         L_SHF     }}  },
    {_ik_MINUS             , {{HID_SLASH,        0      },{HID_MINUS,     0         },{HID_MINUS,     0         }}  },
    {_ik_PLUS              , {{HID_R_BRACKET,    0      },{HID_EQUAL,     L_SHF     },{HID_EQUAL,     L_SHF     }}  },
    {_ik_QUESTION          , {{HID_MINUS,        L_SHF  },{HID_SLASH,     L_SHF     },{HID_SLASH,     L_SHF     }}  },
    {_ik_QUESTIONI         , {{HID_MINUS, L_ALT | L_CTL },{HID_SLASH,L_ALT | L_SHF  },{HID_SLASH,L_ALT | L_SHF  }}  },
    {_ik_SQUOTE            , {{HID_NON_US_1,     L_SHF  },{HID_QUOTE,     0         },{HID_QUOTE,     0         }}  },
    {_ik_R_BRACE           , {{HID_0,            ALTGR  },{HID_R_BRACKET, L_SHF     },{HID_R_BRACKET, L_SHF     }}  },
    {_ik_R_BRACKET         , {{HID_9,            ALTGR  },{HID_R_BRACKET, 0         },{HID_R_BRACKET, 0         }}  },
    {_ik_R_PAREN           , {{HID_9,            L_SHF  },{HID_0,         L_SHF     },{HID_0,         L_SHF     }}  },
    {_ik_SCOLON            , {{HID_COMMA,        L_SHF  },{HID_SEMICOLON, 0         },{HID_SEMICOLON, 0         }}  },
    {_ik_SLASH             , {{HID_7,            L_SHF  },{HID_SLASH,     0,        },{HID_SLASH,     0,        }}  },
    {_ik_USCORE            , {{HID_SLASH,        L_SHF  },{HID_MINUS,     L_SHF     },{HID_MINUS,     L_SHF     }}  },
    {_ik_y                 , {{HID_Z,         0         },{HID_Y,         0         },{HID_Y,         0         }}  },
    {_ik_Y                 , {{HID_Z,         L_SHF     },{HID_Y,         L_SHF     },{HID_Y,         L_SHF     }}  },
    {_ik_z                 , {{HID_Y,         0         },{HID_Z,         0         },{HID_Z,         0         }}  },
    {_ik_Z                 , {{HID_Y,         L_SHF     },{HID_Z,         L_SHF     },{HID_Z,         L_SHF     }}  },
    {_ik_DEGREE            , {{HID_GRAVE,        L_SHF  },{0    , 0                 },{0    , 0                 }}  },
    {_ik_TILDE             , {{HID_R_BRACKET,    ALTGR  },{HID_NON_US_1,   L_SHF    },{HID_GRAVE,   L_SHF       }}  },
    {_ik_SSHARP            , {{HID_MINUS,        0      },{0    , 0                 },{0    , 0                 }}  },
    {_ik_a_UML             , {{HID_QUOTE,        0      },{0    , 0                 },{0    , 0                 }}  },
    {_ik_A_UML             , {{HID_QUOTE,        L_SHF  },{0    , 0                 },{0    , 0                 }}  },
    {_ik_O_UML             , {{HID_SEMICOLON,    L_SHF  },{0    , 0                 },{0    , 0                 }}  },
    {_ik_o_UML             , {{HID_SEMICOLON,    0      },{0    , 0                 },{0    , 0                 }}  },
    {_ik_U_UML             , {{HID_L_BRACKET,    L_SHF  },{0    , 0                 },{0    , 0                 }}  },
    {_ik_u_UML             , {{HID_L_BRACKET,    0      },{0    , 0                 },{0    , 0                 }}  }
};


#endif

