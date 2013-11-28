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

#include "macro.h"
#include "Keyboard.h"

uint8_t curMacro  = MACROCOUNT;
uint8_t sendEmpty = 0;    // empty report needed to send the same character twice in a row

uint8_t idx=0, recidx=0;


bool macromode=false;
uint8_t g_macrorecord=0;

uint8_t hidmacro[MACROLEN];

inline bool macroRecording(void)         { return(g_macrorecord != 0); };
inline void setMacroRecording( uint8_t id ) { g_macrorecord=id; recidx=0; };

inline bool macroMode(void)         { return(macromode != 0); };
inline void setMacroMode( bool on ) { macromode=on; };

#ifdef DEBUG_OUTPUT
/**
 * Only ASCII, no umlauts or other specials, and additional modifier need to be checked!
 *
 * hid,modifier,ascii
 * with valid modifiers = shift and altgr.
 *
 */
char hid2asciicode(uint8_t hid, uint8_t mod){
    uint8_t i;
    for(i=0; i<128; ++i){
        if( ascii2hid[i][0] == hid){
            // either no modifier is needed and neither shift or altgr are pressed
            // or the required modifier is pressed
            if((ascii2hid[i][1]==0 && !(mod && (SHIFT|ALTGR))) || (ascii2hid[i][1] & mod)){
                //printf("MATCH=%c; ",ascii2hid[i][2] );
                return(ascii2hid[i][2]);
            }
        }
    }
    return '0';
}
#endif


/** Record key into macro:
 *    Up to MACROLEN keys may be stored, but any combination of modifiers takes one additional slot.
 *    @todo: Dynamic length, macro idx
 *
 *    Ctrl+Enter terminates macro entry
 *    Alt+Enter  inserts pause
 */
void macro_key(uint8_t mod, uint8_t hid)
{
  // printf("\nMC(%d) : %d:%d =%c   ",recidx, mod, hid, hid2asciicode(hid,mod) );
  // int i; for(i=0; i<MACROLEN; ++i) printf("%d ", hidmacro[i]);
  if(!macroRecording())
      return;

  // Ctrl+Enter ends macro recording
  if((hid == HID_ENTER && mod == CTRL) || recidx==MACROLEN-1){
    hidmacro[recidx]=0;
    uint8_t written=updateEEMacroHID(hidmacro, g_macrorecord-1);
    printf("\nWrote %d/%d", written,recidx);
    recidx=0;

/*
#ifdef DEBUG_OUTPUT
    printf("\n");
    uint8_t i; 
    for(i=0; i<MACROLEN; ++i) {
        uint8_t c=hidmacro[i];
        uint8_t m=0;
        if(c==0){
            i=MACROLEN;
            break;
        }
        if(c&0x80){
            m=(c&0x7f);
            if(c& SHIFT) printf("S");
            if(c& CTRL) printf("C");
            if(c& ALT) printf("A");
            if(c& ALTGR) printf("ag");
            if(c& GUI) printf("G");
            if(++i<MACROLEN){
                c=hidmacro[i];
                printf("(%d=%c) ", c, hid2asciicode(c,m));
            }
        } else {
            printf("%d=%c ", c, hid2asciicode(c,m));
        }
    }
#endif
*/
    setMacroRecording(0);
    return;
  }

  if(recidx<MACROLEN){
    if(mod != 0){
        //printf("\n%02x->%02x", hidmacro[recidx],mod+0x80);
        hidmacro[recidx]=(mod+0x80);
        ++recidx;
    }
  }
  if(recidx<MACROLEN){
    if(hid != 0){
        //printf("\n%02x->%02x", hidmacro[recidx],hid);
        hidmacro[recidx]=hid;
        ++recidx;
    }
  }
}


/**
 * Ends current macro
 */
void endMacro()
{
    macromode = false;
    idx  = 0;
    curMacro=MACROCOUNT;
}

/**
 * Set currently active macro
 */
bool activateMacro(uint8_t id)
{
    if(!macromode)
        return false;
    // de-bounce not working in current macro detection, so return if already active
    if(id==curMacro)
        return true;
    if(id<MACROCOUNT) {
        readEEMacroHID(hidmacro,id);
        idx=0;
        curMacro=id;
        return true;
    } else {
        setMacroMode(false);
        return false;
    }
}


/**
 *  Fills report with the next key in the currently activated macro.
 *  To be called until no more characters are available.
 *  @return false if macro is done or errors prevent further processing.
 */
bool getMacroReport(USB_KeyboardReport_Data_t *report)
{
    uint8_t c;
    if(!macromode)
        return false;
    if(curMacro>=MACROCOUNT)
        return false;

    sendEmpty = sendEmpty ? 0 : 1;
    if( sendEmpty) {
        c=0;
        memset(&report->KeyCode[0], 0, 6);
        return true;
    }

    if( idx < MACROLEN ) {
        uint8_t mod = 0;

        c=hidmacro[idx];
        if( c == 0 ) {
            memset(&report->KeyCode[0], 0, 6);
            endMacro();
            return false;
        }

        // if > 127, it is a modifier
        if(c & 0x80 ) {
            mod=(c & 0x7F);
            c=0;
            sendEmpty = sendEmpty ? 0 : 1;
            idx++;
            if( idx < MACROLEN ) {
                c=hidmacro[idx];
            }
            if( mod==ALT && c==HID_ENTER) {
                _delay_ms(500);
                c=0;mod=0;
            }
            
        }
        
        report->KeyCode[0] = c;
        report->Modifier = mod;
        memset(&report->KeyCode[1], 0, 5);

        idx++;
        return true;
    } else {
        endMacro();
        return false;
    }
    return false;
}



/**
 * Reads the macro at given index from eeprom into macro and returns its length.
 * Caller needs to make sure there in enough space allocated!
 */
uint8_t readEEMacroHID(uint8_t * macro, uint8_t idx)
{
    eeprom_busy_wait();
    uint8_t len=eeprom_read_byte (( const void *) EE_ADDR_MACRO(idx) );

    if(len>MACROLEN)
        len=MACROLEN;

    eeprom_busy_wait();
    eeprom_read_block (( void *) macro , ( const void *) (EE_ADDR_MACRO(idx)+1) , len);

    macro[len]=0;;
    // printf("\nEE read #%d @%d len=%d: ", idx, EE_ADDR_MACRO(idx), len);
    // uint8_t i; for(i=0;i<len;++i) printf(" %02x", macro[i]);

    return len;
}


/**
 * Writes the macro to eeprom at given index and returns length of written string.
 * As eeprom update functions are used, no unnecessary writes are performed.
 */
uint8_t updateEEMacroHID(const uint8_t * macro, uint8_t idx)
{
    uint8_t len=0;
    while(macro[len] != 0 && len < MACROLEN) { 
        len++ ;
    }
    //printf("\nEE write #%d @%d len=%d: ", idx, EE_ADDR_MACRO(idx), len);
    //uint8_t i; for(i=0;i<len;++i) printf(" %02x", macro[i]);

    eeprom_busy_wait();
    eeprom_update_byte ((void *) EE_ADDR_MACRO(idx) , len );
    eeprom_busy_wait();
    eeprom_update_block (( const void *) macro , (void *) (EE_ADDR_MACRO(idx)+1) , len );

    return len;
}

