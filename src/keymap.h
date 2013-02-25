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
    uint8_t  hid;   ///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;  ///< HID modifiers , will be interpreted by OS keyboard layout!
    char     ch;
} keycode;

bool g_alternateLayer; ///< toggle using an alternate layout for layer 1

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
uint8_t getKeyCode (uint8_t row, uint8_t col, uint8_t layer);
uint8_t getKeyChar (uint8_t row, uint8_t col, uint8_t layer);
void    printLayout(uint8_t l);

/// position of an alternate layout in matrix below
#define ALT_ALPHA_LAYER 5

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

  #define LAYERS 6
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
 
  #define LAYERS 6
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
  KEYMAP( 0, /* not used code */
 HID_ESC,  0, 0, 0, 0, 0,                                                     0, 0, 0, 0, 0, 0 ,
       0, HID_MOUSEBTN_1, HID_MOUSEBTN_3, HID_MOUSEBTN_2, HID_MOUSEBTN_4, 0,  0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0, 0, 0 ,                                                     0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0 ,0, HID_MOUSEBTN_1,HID_MOUSEBTN_3, /* | */ 0, HID_MOUSEBTN_2 , 0, 0, 0, 0, 0
);


/**
 * MKT keycode if single key and mousekey location is stored in here.
 * In contrast to the keymatrix below, only actual HID-usage codes are used, as
 * there should be no need for modifiers with MKTs.
 * The mouse button locations are put here as there is no overlap whith other keys,
 * but strictly not the right place here.
 */
static const uint8_t SecondaryUsage[ROWS][COLS] =
  KEYMAP( 0, /* not used code */
 HID_ESC, 0, 0, 0, 0, 0,                                  0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0, 0, 0,                                  0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0, 0, 0,                                  0, 0, 0, 0, 0, 0 ,
 0, 0, HID_ESC, HID_TAB,0, 0 /*is real SPACE*/,0, /* | */ 0, HID_ESC, HID_BACKSPACE, HID_ENTER, 0, 0, 0
);


static const keycode KeyMatrix[LAYERS][ROWS][COLS] PROGMEM = 
{
  // normal layer
 KEYMAP( _no,
         _MOD_3, _k, _u, _q,     _PERIOD, _j,      _v, _g, _c, _l, _f, _BSPACE ,
         _TAB,   _h, _i, _e,     _a,      _o,      _d, _t, _r, _n, _s, _ENTER  ,
         _DQUOTE,_x, _y, _MINUS, _COMMA,  _SLASH,  _b, _p, _w, _m, _z, _MOD_MOUSEKEY,
         _X, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,  _B
  ),
    
  // MOD1 layer (special char)
  KEYMAP( _no, 
    _no, _AT,      _DEGREE,   _L_BRACKET, _R_BRACKET, _HASH   , _EXCLAM,  _LESS,      _GREATER,   _EQUAL,     _AMPERSAND, _no ,
    _no, _BSLASH,  _SLASH,    _L_BRACE,   _R_BRACE,   _ASTERIX, _QUESTION,_L_PAREN,   _R_PAREN,   _PLUS,      _MINUS,     _no ,
    _no, _no,      _DOLLAR,   _PIPE,      _TILDE,     _GRAVE ,  _CARET,   _PERCENT,   _DQUOTE,    _SQUOTE,    _no,        _no ,
    _no, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _no, _no
  ),
    
  // MOD2 layer (movement controls and numbers)
  KEYMAP( _no,
    _no, _PGUP,   _BSPACE,    _UP,        _DEL ,      _PGDN, _SSHARP,_7,         _8,         _9,     _a_UML, _A_UML ,
    _no, _HOME,   _LEFT,      _DOWN,      _RIGHT,     _END , _PERIOD,_4,         _5,         _6,     _o_UML, _O_UML ,
    _no, _ESC,    _TAB,       _INS,       _ENTER,     _no ,  _0,     _1,         _2,         _3,     _u_UML, _U_UML ,
  _no, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,    _R_ALT, _no, _no
    ),
  
  // MOD3 Fx layer
  KEYMAP( _no,
    _no, _no, _no, _u_UML, _A_UML, _O_UML ,_F1,      _F2,        _F3,        _F4,        _F5,    _F6     ,
    _no, _no, _no, _U_UML, _a_UML, _o_UML, _F7,      _F8,        _F9,        _F10,       _F11,   _F12    ,
    _no, _no, _no, _no,    _no,    _no,    _no,      _no,        _no,        _no,        _no,    _no     ,
    _no, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _no, _no 
    ),

  // MOD_MOUSE layer
  KEYMAP( _no,
    _no, _no, _MS_W_U, _MS_U, _MS_W_D, _no,  _no,    _no,      _no,      _no,      _no, _no,
    _no, _no, _MS_L,   _MS_D, _MS_R,   _no,  _no,    _MS_BTN1, _MS_BTN3, _MS_BTN2, _no, _no,
    _no, _no, _MS_W_L, _no,   _MS_W_R, _no,  _no,    _MS_ACC0, _MS_ACC1, _MS_ACC2, _no, _no,
    _no, _L_SHIFT, _L_ALT ,  _L_CTRL, _SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _no,     _no
    ),

  // MOD5 BU-TECK
  // needs to be layout # ALT_ALPHA_LAYER
  KEYMAP( _no,
    _ESC,   _k, _u, _q,     _PERIOD,_x  ,   _p,  _c, _l, _m, _f, _BSPACE ,
    _TAB,   _h, _i, _e,     _a,     _o  ,   _d,  _t, _r, _n, _s, _ENTER  ,
    _DQUOTE,_b, _y, _MINUS,_COMMA,_SLASH,   _j,  _g, _w, _v, _z, _no  ,
    _no,_L_SHIFT,_L_ALT,_L_CTRL,_SPACE, _L_GUI, _MACRO,   _MOD_3, _MOD_2,  _L_SHIFT, _MOD_1,  _R_ALT, _A,  _B  
  )

}; // end of matrix[][][]
    
#endif

