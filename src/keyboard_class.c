/**

  Copyright 2010, frobiac |At| goolemail /d0t\ com

  This project is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This project is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this project.  If not, see  <http://www.gnu.org/licenses/>.

*/

#include <string.h>
#include <limits.h>
#include <util/delay.h>

#include "Keyboard.h"
#include "keyboard_class.h"

#include "hhstdio.h"

#include "keymap.h"

#include "matrix.h"


void init_active_keys(void);
uint8_t lastKeyCode;
struct
{
    uint8_t       row_data[ROWS];
    uint8_t       prev_row_data[ROWS];
} kb;

struct AnalogData {
    int8_t x;
    int8_t y;
    uint8_t mods;
    uint8_t layer;
} ;

struct AnalogData analogData;

int8_t k2idx(struct Key k) {
    return(k.row*COLS + k.col);
}
uint8_t rc2idx(uint8_t r, uint8_t c) {
    return(r*COLS + c);
}


/// used for MODE-key press/release cycle detection
enum { MKT_RESET, MKT_MODEKEYFIRST, MKT_TOOMANYKEYS, MKT_YES , MKT_INIT };
/// @todo Make this configurable or time based (clock independant!)
#define MKT_TIMEOUT 120

uint16_t    mkt_timer;
uint8_t     mkt_state;
struct Key  mkt_key;

/** Debounce code based on code from Peter Dannegger [danni/At/specs/d0t/de]
 *  described in German at bottom of page
 *      http://www.mikrocontroller.net/articles/Entprellung
 *  and discussed at
 *      http://www.mikrocontroller.net/topic/48465
 *  Array extensions inspired by Pult.zip
 *      http://www.mikrocontroller.net/topic/48465#1489399
 */

/// debounce variables
volatile uint8_t kb_state[ROWS];	// debounced and inverted key state: bit = 1: key pressed
volatile uint8_t kb_press[ROWS];  // key press detect
volatile uint8_t kb_release[ROWS];  // key release detect
volatile uint8_t kb_rpt[ROWS];    // key long press and repeat

static uint8_t ct0[ROWS], ct1[ROWS];
static int16_t rpt[ROWS];

void Keyboard__init()
{
    for (uint8_t row = 0; row < ROWS; ++row){
        kb.row_data[row]=0;
        ct0[row]=0xFF;
        ct1[row]=0xFF;
    }

    g_mouse_mode=0;
    g_mouse_keys = 0;
    mkt_timer=MKT_TIMEOUT;

    stdio_init();
    init_cols();
}

void analogDataAcquire(void) {
    /// @todo: Hardcoded mouse layer
    g_mouse_keys = 0;
    analogData.layer=analogData.mods=0;

    if(g_mouse_mode) {
        if(kb.row_data[5] & (1<<1))
            g_mouse_keys = 0x01;
        if(kb.row_data[5] & (1<<2))
            g_mouse_keys = 0x04;
        if(kb.row_data[5] & (1<<3))
            g_mouse_keys = 0x02;
        if(kb.row_data[4] & (1<<1))
            g_mouse_keys = 0x08;
    }
    else{
        int16_t dx=0, dy=0;
        getXY(&dx, &dy);

        analogData.x = dx;
        analogData.y = dy;

        if(dx>1)
            analogData.layer = 3;
        else if(dx<-1)
            analogData.layer = 2;

        if(dy>1)
            analogData.mods = 0x01 ; // SHIFT
        else if(dy<-1)
            analogData.mods = 0x02 ; // CTRL

        //printf("\nMouse: %d/%d %d %d",analogData.mods,analogData.layer , dx, dy);
    }
}

uint8_t Keyboard__get_report(USB_KeyboardReport_Data_t *report_data)
{
    //testMKT();
    if(mkt_timer++ > MKT_TIMEOUT)
        mkt_state = MKT_RESET;
    if(activeKeys.keycnt==0){
        if(mkt_state == MKT_YES){
            report_data->KeyCode[0] = ModeKeyMatrix[mkt_key.row][mkt_key.col].hid;
            report_data->Modifier = 0;
            mkt_state = MKT_INIT;
            return sizeof(USB_KeyboardReport_Data_t);
        }
        mkt_state = MKT_INIT;
    }

    clearActiveKeys();

    scan_matrix();


    if ( keysChanged() )
    {
        copyCurrentKeys();
        printCurrentKeys();
    }


    init_active_keys();

    analogDataAcquire();

    return fillReport(report_data);
}



uint8_t fillReport(USB_KeyboardReport_Data_t *report_data)
{
    uint8_t idx=0;

    for(uint8_t i=0; i < activeKeys.keycnt; ++i)
    {
        struct Key k=activeKeys.keys[i];
        if(k.normalKey && idx < 6) {
           report_data->KeyCode[idx]=getKeyCode(k.row, k.col, getActiveLayer());
           idx++;
        }
        if(idx==6) {
            printf("\nError: more than 6 keys!");
            break;
        }

    }
    report_data->Modifier=getActiveModifiers()|getActiveKeyCodeModifier();
    return sizeof(USB_KeyboardReport_Data_t);
}


/// @todo Fix timeouts and generalize routine:
///       Currently, thumbs are excluded from repeat handling and debounce due to double usage
#define REPEAT_MASK     0x3F       // repeat: key0
#define REPEAT_START   1000
#define REPEAT_NEXT     500

uint8_t get_kb_release( uint8_t key_mask, uint8_t col)
{
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        key_mask &= kb_release[col];                      // read key(s)
        kb_release[col] ^= key_mask;                      // clear key(s)
    }
    return key_mask;
}

uint8_t get_kb_press( uint8_t key_mask, uint8_t col )
{
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        key_mask &= kb_press[col];                      // read key(s)
        kb_press[col] ^= key_mask;                      // clear key(s)
    }
	return key_mask;
}
uint8_t get_kb_rpt( uint8_t key_mask, uint8_t col )
{
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        key_mask &= kb_rpt[col];                        // read key(s)
        kb_rpt[col] ^= key_mask;                        // clear key(s)
    }
    return key_mask;
}

uint8_t get_kb_short( uint8_t key_mask, uint8_t col )
{
  uint8_t i;
  ATOMIC_BLOCK(ATOMIC_FORCEON)
          i = get_kb_press( ~kb_state[col] & key_mask, col );
  return i;
}
 
uint8_t get_kb_long( uint8_t key_mask, uint8_t col )
{
  return get_kb_press( get_kb_rpt( key_mask,col  ), col);
}

uint8_t get_kb_long_r( uint8_t key_mask, uint8_t col )
{
  return get_kb_press( get_kb_rpt( kb_press[col]& key_mask,col  ), col);
}

uint8_t get_kb_long_l( uint8_t key_mask, uint8_t col )
{
  return get_kb_rpt( ~kb_press[col]& key_mask,col  );
}

void scan_matrix(void)
{
	uint8_t i, data;

    for (uint8_t row = 0; row < ROWS; ++row)
    {
        activate(row);

        // Insert NOPs for synchronization
        _delay_us(20);

        // Place data on all column pins for active row
        // into a single 32 bit value.
        data = read_col();

        /// @see top comment for source of debounce magic
        // Needs to be adjusted for more than 8 columns
        i = kb_state[row] ^ (~data);                     // key changed ?
        ct0[row] = ~( ct0[row] & i );                   // reset or count ct0
        ct1[row] = ct0[row] ^ (ct1[row] & i);           // reset or count ct1
        i &= ct0[row] & ct1[row];                       // count until roll over ?
        kb_state[row] ^= i;                             // then toggle debounced state

        kb_press  [row] |=  kb_state[row] & i;             // 0->1: key press detect
        kb_release[row] |= ~kb_state[row] & i;             // 1->0: key press detect

        if( (kb_state[row] & REPEAT_MASK) == 0 )        // check repeat function
            rpt[row] = REPEAT_START;                    // start delay
        if(  --rpt[row] == 0 )
        {
            rpt[row] = REPEAT_NEXT;                     // repeat delay
            kb_rpt[row] |= kb_state[row] & REPEAT_MASK;
        }

        // Now evaluate results
        uint8_t p,r,h;
        p=get_kb_press  (REPEAT_MASK, row);
        h=get_kb_rpt    (REPEAT_MASK, row);
        r=get_kb_release(REPEAT_MASK, row);

        kb.row_data[row] = ((kb.row_data[row]|(p|h)) & ~r);

        // permanent layer toggles go here!
        if(row==3 && (p & 0x01))
            g_mouse_mode = !g_mouse_mode;

/*
        if(p!=0) {
            //printf("\n(%d,%d) PRESS  ",row,p);
            kb.row_data[row] |= p;
        }
        if(r!=0) {
            //printf("\n(%d,%d) RELEASE",row,r);
            kb.row_data[row] &= ~r;
        }
        if(h!=0) {
            //printf("\n(%d,%d) HOLD   ",row,h);
            kb.row_data[row] |= h;
        }
        if( !( p==0 && r==0 && h==0)) {
            ;//printf("  phr= %2d %2d %2d",p,h,r );
        }
        */
    }

}

void copyCurrentKeys(void)
{
    for(uint8_t r=0; r<ROWS; ++r)
        kb.prev_row_data[r] = kb.row_data[r];
}

bool keysChanged(void)
{
    for(uint8_t r=0; r<ROWS; ++r)
        if(kb.prev_row_data[r] != kb.row_data[r])
            return true;
    return false;
}

void printCurrentKeys(void)
{
    for(uint8_t r=0; r<ROWS/2; ++r) {
        printf("\n");
        for(uint8_t c=0; c< COLS; ++c){
            if( kb.row_data[r] & (1<<c))
                printf("X");
            else
                printf(".");
        }
        printf("|  |");
        for(uint8_t c=0; c< COLS; ++c){
            if( kb.row_data[r+ROWS/2] & (1<<c))
                printf("X");
            else
                printf(".");
        }
    }
    printf("\n");
}


void clearActiveKeys()
{
    activeKeys.keycnt=0;
}

/// @todo Switch back from mouse layer!
uint8_t getActiveLayer()
{
    uint8_t layer=0;
    for(uint8_t i=0; i < activeKeys.keycnt; ++i)
    {
        struct Key k = activeKeys.keys[i];
        if( isLayerKey(k.row, k.col) )
        {
            if(layer!=0)
                printf("\nWARN: More than one layer key pressed!");
            layer = getModifier(k.row, k.col,0)-MOD_LAYER_0;
            //printf("\nL=%d",layer);
        }
    }
    // no other layer modifier pressed
    if(layer == 0) {
        if( g_mouse_mode)
            return 4; /// @todo  hardcoded layer
        else return analogData.layer;
    }
    return layer;
}


uint8_t getActiveKeyCodeModifier()
{
    for(uint8_t i=0; i < activeKeys.keycnt; ++i)
    {
        struct Key k=activeKeys.keys[i];
        if(k.normalKey)
        {
            return getModifier(k.row, k.col, getActiveLayer());
        }
    }
    return 0;
}

// Assuming there is only one active key
uint8_t getActiveModifiers()
{
    uint8_t modifiers=0;
    for(uint8_t i=0; i < activeKeys.keycnt; ++i)
    {
        struct Key k = activeKeys.keys[i];
        if( isModifierKey(k.row, k.col) )
        {
            modifiers |= (1<<(getModifier(k.row, k.col,0)-MOD_BEGIN));
        }
    }
    modifiers |= analogData.mods;

    return modifiers;
}


/**
  * check whether layer 0 key is a modifier )
  */
bool isModifierKey(uint8_t row, uint8_t col)
{
    if( getModifier(row,col,0) > MOD_BEGIN && getModifier(row,col,0) < MOD_LAYER_0)
        return true;
    return false;
}

/** check whether layer 0 key is a modifier )
  */
bool isLayerKey(uint8_t row, uint8_t col)
{
    if( getModifier(row,col,0) > MOD_LAYER_0 && getModifier(row,col,0) < MOD_LAYER_LAST){
        return true;
    }
    return false;
}

bool isNormalKey(uint8_t row, uint8_t col)
{
    return !(isLayerKey(row,col) || isModifierKey(row,col));
}



/** get modifier
  */
uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer)
{
    return KeyMatrix[layer][row][col].mods;
}

uint8_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer)
{
    return KeyMatrix[layer][row][col].hid;
}

void ActiveKeys_Add(uint8_t row, uint8_t col)
{
    if(activeKeys.keycnt>= MAX_ACTIVE_KEYS){
        printf("ERR: Too many active keys!");
        return;
    }
    struct Key key;
    key.row=row;
    key.col=col;
    key.normalKey = !(isModifierKey(row,col) || isLayerKey(row,col));

    activeKeys.keys[activeKeys.keycnt]=key;
    activeKeys.keycnt++;

    if(activeKeys.keycnt>1)
        mkt_state = MKT_TOOMANYKEYS;
    if(mkt_state == MKT_INIT && activeKeys.keycnt==1 && !key.normalKey){
        mkt_state = MKT_YES;
        mkt_key = key;
        mkt_timer=0;
    }
    if(mkt_state == key.normalKey)
        mkt_state = MKT_RESET;
}

void init_active_keys()
{

    // process row/column data to find the active keys
    for (uint8_t row = 0; row < ROWS; ++row)
    {
        for (uint8_t col = 0; col < COLS; ++col)
        {
            if (kb.row_data[row] & (1UL << col))
            {
                ActiveKeys_Add(row,col);
            }
        }
    }
}

