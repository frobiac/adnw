/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2019 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

/**
HYPER modifier Mod3
===================
To activate a 5th modifier under linux _GUI_R is internally handled as a regular
hid modifier, including secondary usage. But in final report assembly it is translated into
regular key International_1 with xev decimal code 97 / hid 0x87 that may then be mapped
to Mod3 as follows:

Hyper.xmodmap:
--------------
    clear Mod4
    add Mod4 = Super_L Super_R
    clear Mod3
    ! keycode 97 is HID_INTERNATIONAL_1
    keycode 97 = Hyper_L
    add Mod3 = Hyper_L Hyper_R
*/

#pragma once

#include <avr/eeprom.h>
#include <stdint.h>
#include <stdbool.h>
#include "hid_usage.h"
#include "config.h"

typedef struct {
    uint8_t  hid;   ///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;  ///< HID modifiers , will be interpreted by OS keyboard layout!
    //char     ch;
} keycode;


uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);
uint8_t getKeyCode (uint8_t row, uint8_t col, uint8_t layer);
//uint8_t getKeyChar (uint8_t row, uint8_t col, uint8_t layer);

// void    printLayout(uint8_t l);

#ifdef ALTERNATE_LAYER
    /// position of an alternate layout in matrix below
    #define ALT_ALPHA_LAYER 5
#endif

#define _MACRO _no
#define LAYERS 6

// *INDENT-OFF*

#ifdef BLUECUBE

/*
            ┌───┐                             ┌───┐
        ┌───┤03 ├───┬───┐             ┌───┬───┤42 ├───┐
        │02 ├───┤04 │05 │             │40 │41 ├───┤43 │
┌───┬───┼───┤13 ├───┼───┤             ├───┼───┤52 ├───┼───┬───┐
│00 │01 │12 ├───┤14 │15 │             │50 │51 ├───┤53 │44 │45 │
├───┼───┼───┤23 ├───┼───┤             ├───┼───┤62 ├───┼───┼───┤
│10 │11 │22 ├───┤24 │25 ├───┐     ┌───┤60 │61 ├───┤63 │54 │55 │
├───┼───┼───┼───┼───┼───┤30!│     │75!├───┼───┼───┼───┼───┼───┤
│20 │21 │32 │33 │34 │35 ├───┤     ├───┤70 │71 │72 │73 │64 │65 │
└───┴───┴───┴───┴───┴───┤31!│     │74!├───┴───┴───┴───┴───┴───┘
                        └───┘     └───┘
*/

#define KEYMAP( no, \
   Uesc,   k,   u,   q, dot,   j,       p,   c,   l,   m,   f, Ubsp,  \
   Utab,   h,   i,   e,   a,   o,       d,   t,   r,   n,   s, Uret,  \
   Ucap,   x,   y, das, com, sla,       b,   g,   w,   v,   z, Umou,  \
   Usft, ALT, MAC, GUI, CTL, SPC,      M2, SHF,  M1, AGR,  M3, Unone  \
) { \
/*           0    1    2    3    4    5 */ \
/* 0 */  {  no,   k,   u,   q, dot,   j  }, \
/* 1 */  {  no,   h,   i,   e,   a,   o  }, \
/* 2 */  {  no,   x,   y, das, com, sla  }, \
/* 3 */  { SPC, CTL,Usft, ALT, MAC, GUI  }, \
/* 4 */  {   p,   c,   l,   m,   f,  no  }, \
/* 5 */  {   d,   t,   r,   n,   s,  no  }, \
/* 6 */  {   b,   g,   w,   v,   z,  no  }, \
/* 7 */  { SHF,  M1, AGR, Uno,  M2,  M3  }  \
}
#endif


#ifdef REDTILT

/*
    RedTilt: outermost "thumb" keys are in topmost row
    Ring finger thumbkeys (32/73) missing in matrix!

    ┌───┬───┐   ┌───┬───┬───┐    ┌───┬───┬───┐   ┌───┬───┐
    │30!│31!├───┤03 │04 │05 │    │40 │41 │42 ├───┤74!│75!│
    ├───┼───┤02 ├───┼───┼───┤    ├───┼───┼───┤43 ├───┼───┤
    │00 │01 ├───┤13 │14 │15 │    │50 │51 │52 ├───┤44 │45 │
    ├───┼───┤12 ├───┼───┼───┤    ├───┼───┼───┤53 ├───┼───┤
    │10 │11 ├───┤23 │24 │25 │    │60 │61 │62 ├───┤54 │55 │
    ├───┼───┤22 ├───┼───┼───┤    ├───┼───┼───┤63 ├───┼───┤
    │20 │21 ├───┤33 │34 │35 │    │70 │71 │72 ├───┤64 │65 │
    └───┴───┘   └───┴───┴───┘    └───┴───┴───┘   └───┴───┘

*/

#define KEYMAP( no, \
   Uesc,   k,   u,   q, dot,   j,       p,   c,   l,   m,   f, Ubsp,  \
   Utab,   h,   i,   e,   a,   o,       d,   t,   r,   n,   s, Uret,  \
   Ucap,   x,   y, das, com, sla,       b,   g,   w,   v,   z, Umou,  \
   Usft, ALT, MAC, GUI, CTL, SPC,      M2, SHF,  M1, AGR,  M3, Unone  \
) { \
/*           0    1    2    3    4    5 */ \
/* 0 */  {  no,   k,   u,   q, dot,   j  }, \
/* 1 */  {  no,   h,   i,   e,   a,   o  }, \
/* 2 */  {  no,   x,   y, das, com, sla  }, \
/* 3 */  {  no, ALT, MAC,  GUI, CTL, SPC }, \
/* 4 */  {   p,   c,   l,   m,   f,  no  }, \
/* 5 */  {   d,   t,   r,   n,   s,  no  }, \
/* 6 */  {   b,   g,   w,   v,   z,  Umou}, \
/* 7 */  {  M2,  SHF,  M1,  no,  M3, AGR }  \
}
#endif

#if defined(BLACKFLAT) || defined(BLACKBOWL)

/*
    BlackFlat: RedTilt without outer pinky columns, with 4th button on pinky bottom row
    and hooked up to former bluecube teensy with limited working pins
    BlackBowl: Identical layout-wise, but with I2C for both halves.

    ┌───┐   ┌───┬───┬───┐    ┌───┬───┬───┐RST┌───┐
    │30 ├───┤02 │03 │04 │    │40 │41 │42 ├───┤74 │
    ├───┤01 ├───┼───┼───┤    ├───┼───┼───┤43 ├───┤
    │00 ├───┤12 │13 │14 │    │50 │51 │52 ├───┤44 │
    ├───┤11 ├───┼───┼───┤    ├───┼───┼───┤53 ├───┤
    │10 ├───┤22 │23 │24 │    │60 │61 │62 ├───┤54 │
    ├───┤21 ├───┼───┼───┤    ├───┼───┼───┤63 ├───┤
    │20 ├───┤32 │33 │34 │    │70 │71 │72 ├───┤64 │
    └───┘31 └───┴───┴───┘    └───┴───┴───┘73 └───┘

*/
/* Thumb rows column assignment:
        |0    | |    0|
        |     | |     |
        |     | |     |
        | 4312| |1234 |
*/
#define KEYMAP( no, \
   _u0,   k,   u,   q, dot,   j,       p,   c,   l,   m,   f, _u4,  \
   _u1,   h,   i,   e,   a,   o,       d,   t,   r,   n,   s, _u5,  \
   _u2,   x,   y, das, com, sla,       b,   g,   w,   v,   z, _u6,  \
   _u3, ALT, MAC, GUI, CTL, SPC,      M2, SHF,  M1, AGR,  M3, _u7   \
) { \
/*           0    1    2    3    4   */ \
/* 0 */  {   k,   u,   q,   dot, j   }, \
/* 1 */  {   h,   i,   e,   a,   o   }, \
/* 2 */  {   x,   y,   das, com, sla }, \
/* 3 */  {  ALT, MAC,  GUI, CTL, SPC }, \
/* 4 */  {   p,   c,   l,   m,   f   }, \
/* 5 */  {   d,   t,   r,   n,   s   }, \
/* 6 */  {   b,   g,   w,   v,   z   }, \
/* 7 */  {  M2,  SHF,  M1,  AGR, M3 }  \
}

#endif // BLACKFLAT


#ifdef HYPERNANO

/*  Row/col matrix: (0-based)
 *  unused in layout: 00,10,20, 45,55,65
 *  but used in matrix]  20/65 for innermost keys on row 3
 *
    ┌─────┬───┬───┬───┬───┐───────┌───┬───┬───┬───┬─────┐
    │  01 │02 │03 │04 │05 │       │41 │42 │43 │44 │45   │
    ├────┬┴──┬┴──┬┴──┬┴──┬┘   _   └┬──┴┬──┴┬──┴┬──┴┬────┤
    │ 11 │12 │13 │14 │15 │   (_)   │51 │52 │53 │54 │55  │
    ├───┬┴──┬┴──┬┴──┬┴──┬┴──┐   ┌──┴┬──┴┬──┴┬──┴┬──┴┬───┤
    │21 │22 │23 │24 │25 │20!│   │60!│61 │62 │63 │64 │65 │
    ├───┴┬──┴┬──┴┬──┴┬──┴┬──┴┐ ┌┴──┬┴──┬┴──┬┴──┬┴──┬┴───┤
    │ 31 │32 │33 │34 │35 │30!│ │70!│71 │72 │73 │74 │75  │
    └────┴───┴───┴───┴───┴───┘─└───┴───┴───┴───┴───┴────┘
 */

#define KEYMAP( no, \
   Uesc,   k,   u,   q, dot,   j,       p,   c,   l,   m,   f, Ubsp,  \
   Utab,   h,   i,   e,   a,   o,       d,   t,   r,   n,   s, Uret,  \
   Ucap,   x,   y, das, com, sla,       b,   g,   w,   v,   z, Umou,  \
   Usft, ALT, MAC, GUI, CTL, SPC,      M2, SHF,  M1, AGR,  M3, Unone  \
) { \
/*           0    1    2    3    4    5 */ \
/* 0 */  {  no,   k,   u,   q, dot,   j  }, \
/* 1 */  {  no,   h,   i,   e,   a,   o  }, \
/* 2 */  { ALT,   x,   y, das, com, sla  }, \
/* 3 */  {  no,Usft, MAC, GUI, CTL, SPC  }, \
/* 4 */  {  no,   p,   c,   l,   m,   f  }, \
/* 5 */  {  no,   d,   t,   r,   n,   s  }, \
/* 6 */  {  no,   b,   g,   w,   v,   z  }, \
/* 7 */  {  Unone, M2, SHF,  M1, AGR, M3 }, \
}
#endif

#ifdef HYPERMICRO

/*  Row/col matrix: (0-based)
 *  unused in layout: 29
 *

    ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
    │30 │31 │32 │33 │34 │35 │36 │37 │38 │39 │310│311│
    ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┤
    │20   │21 │22 │23 │24 │25 │26 │27 │28 │210│  211│
    ├───┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬───┤
    │10 │11 │12 │13 │14 │15 │16 │17 │18 │19 │110│111│
    ├───┼───┴┬──┴───┼───┴┬──┴──┬┴───┼───┴──┬┴───┼───┤
    │00 │ 01 │  02  │ 03 │ 05  │ 08 │  09  │010 │011│
    └───┴────┴──────┴────┴─────┴────┴──────┴────┴───┘
                      ┌───┬───┬───┐
                      └───┴───┴───┘
                       04  06  07

 */
#define KEYMAP( no, \
   Uesc,   k,   u,   q, dot,   j,       p,   c,   l,   m,   f, Ubsp,  \
   Utab,   h,   i,   e,   a,   o,       d,   t,   r,   n,   s, Uret,  \
   Ucap,   x,   y, das, com, sla,       b,   g,   w,   v,   z, Umou,  \
   Usft, ALT, MAC, GUI, CTL, SPC,      M2, SHF,  M1, AGR,  M3, Unone  \
) { \
/*           0    1    2    3    4    5       6    7    8    9   10   11   */ \
/* 3 */  {Uesc,   k,   u,   q, dot,   j,      p,   c,   l,   m,   f,  Ubsp }, \
/* 2 */  {   h,   i,   e,   a,   o,Utab,      d,   t,   r,   no,  n,  s   }, \
/* 1 */  {   x,   y, das, com, sla, SPC,     M3,   b,   g,   w,   v,  z   }, \
/* 0 */  { ALT, CTL, GUI, MAC,Ucap,  AGR,  Usft,Umou,  M2, SHF,  M1, Uret }  \
}
#endif

#define _DU_TAB_CTRL { HID_TAB,       TAPPING_MASK | MOD_L_CTRL  }
#define _DU_SPC_ALT  { HID_SPACE,     TAPPING_MASK | MOD_L_ALT   }
#define _DU_ESC_MOD2 { HID_ESC,       TAPPING_MASK | MOD_LAYER_2 }
#define _DU_BSP_SHFT { HID_BACKSPACE, TAPPING_MASK | MOD_L_SHIFT }
#define _DU_RET_MOD1 { HID_ENTER,     TAPPING_MASK | MOD_LAYER_1 }


static const keycode KeyMatrix[LAYERS][ROWS][COLS] PROGMEM =
{
  // modified BU-TECK is default
  KEYMAP( _no,
    _ESC,    _k, _u, _q,     _PERIOD,_j  ,   _p,  _c, _l, _m, _f, _BSPACE ,
    _TAB,    _h, _i, _e,     _a,     _o  ,   _d,  _t, _r, _n, _s, _ENTER  ,
    _DQUOTE, _x, _y, _MINUS,_COMMA,_SLASH,   _b,  _g, _w, _v, _z, _MOD_MOUSEKEY,
    _L_SHIFT, _L_GUI ,  _MACRO, _R_GUI, _DU_TAB_CTRL, _DU_SPC_ALT,   _DU_ESC_MOD2, _DU_BSP_SHFT, _DU_RET_MOD1, _R_ALT,  _MOD_3, _no
  ),

  // MOD1 layer (special char)
  KEYMAP( _no,
    _no, _AT,      _DEGREE,   _L_BRACKET, _R_BRACKET, _HASH   , _EXCLAM,  _LESS,      _GREATER,   _EQUAL,     _AMPERSAND, _no ,
    _no, _BSLASH,  _EURO  ,   _L_BRACE,   _R_BRACE,   _ASTERIX, _QUESTION,_L_PAREN,   _R_PAREN,   _PLUS,      _no,        _no ,
    _no, _no,      _DOLLAR,   _PIPE,      _TILDE,     _GRAVE ,  _CARET,   _PERCENT,   _DQUOTE,    _SQUOTE,    _CMDMODE,   _no ,
    _L_SHIFT, _L_GUI ,  _MACRO, _R_GUI, _L_CTRL, _L_ALT,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
  ),

  // MOD2 layer (movement controls and numbers)
  KEYMAP( _no,
    _no, _PGUP,   _BSPACE,    _UP,        _DEL ,      _PGDN, _SSHARP,_7,         _8,         _9,     _a_UML, _A_UML ,
    _no, _HOME,   _LEFT,      _DOWN,      _RIGHT,     _END , _PERIOD,_4,         _5,         _6,     _o_UML, _O_UML ,
    _no, _CMDMODE,_EXEC,      _INS,       _MENU ,     _STOP, _0,     _1,         _2,         _3,     _u_UML, _U_UML ,
    _L_SHIFT, _L_GUI ,  _MACRO, _R_GUI, _L_CTRL, _L_ALT,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
    ),

  // MOD3 Fx layer
  KEYMAP( _no,
    _no,      _X11,    _X12,   _X13,   _X14,   _X15,      _F12,   _F7,      _F8,    _F9,    _no,    _no ,
    _no,      _X6,     _X7,    _X8,    _X9,    _X10,      _F11,   _F4,      _F5,    _F6,    _no,    _no ,
    _no,      _X1,     _X2,    _X3,    _F19,    _X5,      _F10,   _F1,      _F2,    _F3,    _no,    _no ,
    _L_SHIFT, _L_GUI , _MACRO, _R_GUI, _L_CTRL, _L_ALT,   _MOD_2, _L_SHIFT, _MOD_1, _R_ALT, _MOD_3, _no
    ),
  // MOD_MOUSE layer
  KEYMAP( _no,
    _no, _no, _MS_W_U, _MS_U, _MS_W_D, _no,  _no,    _no,      _MS_BTN3, _MS_BTN5,      _no, _no,
    _no, _no, _MS_L,   _MS_D, _MS_R,   _no,  _no,    _MS_BTN1, _MS_BTN1, _MS_BTN2, _MS_BTN4, _no,
    _no, _no, _MS_W_L, _no,   _MS_W_R, _no,  _no,    _MS_ACC0, _MS_ACC1, _MS_ACC2, _no, _no,
    _L_SHIFT, _L_GUI ,  _MACRO, _R_GUI, _L_CTRL, _L_ALT,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
    ),

#ifdef ALTERNATE_LAYER
  // AdNW, ALT_ALPHA_LAYER
  KEYMAP( _no,
    _ESC,   _k, _u, _q,     _PERIOD, _j,      _v, _g, _c, _l, _f, _BSPACE ,
    _TAB,   _h, _i, _e,     _a,      _o,      _d, _t, _r, _n, _s, _ENTER  ,
    _DQUOTE,_x, _y, _MINUS, _COMMA,  _SLASH,  _b, _p, _w, _m, _z, _MOD_MOUSEKEY,
    _L_SHIFT, _L_GUI ,  _MACRO, _R_GUI, _L_CTRL, _L_ALT,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
  ),
#endif

}; // end of matrix[][][]

// *INDENT-OFF*

#if   (COLS <= 8)
    typedef uint8_t  column_size_t;
#elif (COLS <= 16)
    typedef uint16_t column_size_t;
#elif (COLS <= 32)
    typedef uint32_t column_size_t;
#else
    #error "More than 32 columns are not supported"
#endif



