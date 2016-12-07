/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2016 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

typedef struct {
    uint8_t  hid;   ///< HID usage code, will be interpreted by OS keyboard layout!
    uint8_t  mods;  ///< HID modifiers , will be interpreted by OS keyboard layout!
    //char     ch;
} keycode;

EEMEM extern uint8_t ee_pinkyDrop;
EEMEM extern uint8_t ee_alternateLayer;
uint8_t g_alternateLayer; ///< toggle using an alternate layout for layer 1

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
#define ROWS   8
#define COLS   6  ///< cols of both keyboard halves are "stacked" in layout and physically connected

// *INDENT-OFF*

#ifdef BLUECUBE
  #define CMD_MODE() ((rowData[2] & (1<<0)) && (rowData[6] & (1<<5)) )

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
  #define CMD_MODE() ((rowData[2] & (1<<0)) && (rowData[6] & (1<<5)) )

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


#ifdef HYPERNANO
  #define CMD_MODE() ( (rowData[3] & (1<<1)) && (rowData[7] & (1<<5)) ) 

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
/* 2 */  { SPC,   x,   y, das, com, sla  }, \
/* 3 */  { ALT,  no,Usft, MAC, GUI, CTL  }, \
/* 4 */  {  no,   p,   c,   l,   m,   f  }, \
/* 5 */  {  no,   d,   t,   r,   n,   s  }, \
/* 6 */  {  no,   b,   g,   w,   v,   z  }, \
/* 7 */  {  M2, SHF,  M1, AGR,  M3, Unone}  \
}
#endif

#ifdef HYPERMICRO
#undef ROWS
#undef COLS
#define ROWS   4
#define COLS   12
#define CMD_MODE() ( (rowData[3] & (1<<0)) && (rowData[0] & (1<<0)) )

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


/**
 * Mouse keycodes that temporarily override normal keycodes during trackpoint usage.
 * Mouse buttons can be mapped to good locations this way.
 * This is not to be confused with the mousekey implementation, which is activated by switching to its layer.
 */
static const uint8_t MouseUsage[ROWS][COLS] PROGMEM =
  KEYMAP( 0, /* not used code */
       HID_ESC,  0, 0, 0, 0, 0,                                               0, 0, 0, 0, 0, 0 ,
       0, HID_BTN_L, HID_BTN_M, HID_BTN_R, HID_BTN_S, 0,  0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0, 0, 0 ,                                                     0, 0, 0, 0, 0, 0 ,
#ifdef HYPERNANO
       0, 0, 0 ,0, HID_BTN_L,HID_BTN_M, /* | */ 0, HID_BTN_R , 0, 0, 0, 0
#elif defined(REDTILT) || defined(BLACKFLAT) // one handed trackpoint operation with buttons on right thumbs
       0, 0, 0 ,0, 0, 0,         HID_BTN_L, HID_BTN_S, HID_BTN_R, 0, 0, 0
#else
       0, 0, 0, 0, 0 ,            0,0,                                     0, 0, 0, 0, 0
#endif
);



/**
 * MKT keycode if single key and mousekey location is stored in here.
 * In contrast to the keymatrix below, only actual HID-usage codes are used, as
 * there should be no need for modifiers with MKTs.
 * The mouse button locations are put here as there is no overlap whith other keys,
 * but strictly not the right place here.
 */
static const uint8_t SecondaryUsage[ROWS][COLS] PROGMEM =
  KEYMAP( 0, /* not used code */
 HID_ESC, 0, 0, 0, 0, 0,                                  0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0, 0, 0,                                  0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0, 0, 0,                                  0, 0, 0, 0, 0, 0 ,
       0, 0, 0, 0, HID_TAB,0,  /*is real SPACE*/ /*|*/  HID_ESC, HID_BACKSPACE, HID_ENTER, 0, 0, 0
);



static const keycode KeyMatrix[LAYERS][ROWS][COLS] PROGMEM = 
{
/**
  /// only debug to test matrix
  KEYMAP( _no,
    _Z,    _k, _u, _q,     _PERIOD,_j  ,   _p,  _c, _l, _m, _f, _E,
    _Y,    _h, _i, _e,     _a,     _o  ,   _d,  _t, _r, _n, _s, _D,
    _X,    _x, _y, _MINUS,_COMMA,_SLASH,   _b,  _g, _w, _v, _z, _C,
    _0,    _1, _2, _3,    _4,    _5,       _6,  _7, _8, _9, _A, _B
  ),
*/
  // modified BU-TECK is default
  KEYMAP( _no,
    _ESC,    _k, _u, _q,     _PERIOD,_j  ,   _p,  _c, _l, _m, _f, _BSPACE ,
    _TAB,    _h, _i, _e,     _a,     _o  ,   _d,  _t, _r, _n, _s, _ENTER  ,
    _DQUOTE, _x, _y, _MINUS,_COMMA,_SLASH,   _b,  _g, _w, _v, _z, _MOD_MOUSEKEY,
    _L_SHIFT, _L_ALT ,  _MACRO, _L_GUI, _L_CTRL, _SPACE,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
  ),

  // MOD1 layer (special char)
  KEYMAP( _no, 
    _no, _AT,      _DEGREE,   _L_BRACKET, _R_BRACKET, _HASH   , _EXCLAM,  _LESS,      _GREATER,   _EQUAL,     _AMPERSAND, _no ,
    _no, _BSLASH,  _EURO  ,   _L_BRACE,   _R_BRACE,   _ASTERIX, _QUESTION,_L_PAREN,   _R_PAREN,   _PLUS,      _no,        _no ,
    _no, _no,      _DOLLAR,   _PIPE,      _TILDE,     _GRAVE ,  _CARET,   _PERCENT,   _DQUOTE,    _SQUOTE,    _no,        _no ,
    _L_SHIFT, _L_ALT ,  _MACRO, _L_GUI, _L_CTRL, _SPACE,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
  ),

  // MOD2 layer (movement controls and numbers)
  KEYMAP( _no,
    _no, _PGUP,   _BSPACE,    _UP,        _DEL ,      _PGDN, _SSHARP,_7,         _8,         _9,     _a_UML, _A_UML ,
    _no, _HOME,   _LEFT,      _DOWN,      _RIGHT,     _END , _PERIOD,_4,         _5,         _6,     _o_UML, _O_UML ,
    _no, _no  ,   _no  ,      _INS,       _no   ,     _no ,  _0,     _1,         _2,         _3,     _u_UML, _U_UML ,
    _L_SHIFT, _L_ALT ,  _MACRO, _L_GUI, _L_CTRL, _SPACE,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
    ),

  // MOD3 Fx layer
  KEYMAP( _no,
    _no,      _no,     _no,    _u_UML, _A_UML,  _O_UML,   _F12,   _F7,      _F8,    _F9,    _no,    _no ,
    _no,      _no,     _no,    _U_UML, _a_UML,  _o_UML,   _F11,   _F4,      _F5,    _F6,    _no,    _no ,
    _no,      _no,     _no,    _no,    _no,     _no,      _F10,   _F1,      _F2,    _F3,    _no,    _no ,
    _L_SHIFT, _L_ALT , _MACRO, _L_GUI, _L_CTRL, _SPACE,   _MOD_2, _L_SHIFT, _MOD_1, _R_ALT, _MOD_3, _no
    ),

  // MOD_MOUSE layer
  KEYMAP( _no,
    _no, _no, _MS_W_U, _MS_U, _MS_W_D, _no,  _no,    _no,      _no,      _no,      _no, _no,
    _no, _no, _MS_L,   _MS_D, _MS_R,   _no,  _no,    _MS_BTN1, _MS_BTN3, _MS_BTN2, _no, _no,
    _no, _no, _MS_W_L, _no,   _MS_W_R, _no,  _no,    _MS_ACC0, _MS_ACC1, _MS_ACC2, _no, _no,
    _L_SHIFT, _L_ALT ,  _MACRO, _L_GUI, _L_CTRL, _SPACE,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
    )

#ifdef ALTERNATE_LAYER
,
  // AdNW, ALT_ALPHA_LAYER
  KEYMAP( _no,
    _ESC,   _k, _u, _q,     _PERIOD, _j,      _v, _g, _c, _l, _f, _BSPACE ,
    _TAB,   _h, _i, _e,     _a,      _o,      _d, _t, _r, _n, _s, _ENTER  ,
    _DQUOTE,_x, _y, _MINUS, _COMMA,  _SLASH,  _b, _p, _w, _m, _z, _MOD_MOUSEKEY,
    _L_SHIFT, _L_ALT ,  _MACRO, _L_GUI, _L_CTRL, _SPACE,   _MOD_2, _L_SHIFT,  _MOD_1, _R_ALT,  _MOD_3, _no
  )
#endif

}; // end of matrix[][][]

// *INDENT-OFF*

#endif

