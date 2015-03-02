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

uint8_t idx=0; // position in current macro, both read & write


bool macromode=false;

/// 255 means no recording is going on.
#define INVALID_MACRO_ID 255
/// Holds index of macro while recording, 0<=idx<MACROCOUNT.
uint8_t g_macrorecord=INVALID_MACRO_ID;

uint8_t hidmacro[MACROLEN];

inline bool macroRecording(void)         { return(g_macrorecord != INVALID_MACRO_ID); };
inline void disableMacroRecording(void) { g_macrorecord=INVALID_MACRO_ID; };

inline bool macroMode(void)         { return(macromode != 0); };
inline void setMacroMode( bool on ) { macromode=on; };

/**
 * Return ascii char of given hid code and modifier combination.
 *
 * Only ASCII printable characters - no umlauts or other specials like € or °.
 *
 * @arg hid HID code to map to ascii
 * @arg mod modifier necessary [SHIFT|ALTGR]
 *
 * @ret printable ascii char if found, or '\0' on unprintable characters
 *
 * @todo backslash does it work?
 * @todo handle ESC, BS and the like?
 */
char hid2asciicode(uint8_t hid, uint8_t mod)
{
    if(hid==0)
        return('\0');

    uint8_t i;
    for(i=127; i>0; --i) {
        if( ascii2hid[i][0] == hid) {
            // either no modifier is needed and neither shift or altgr are pressed
            // or the required modifier is pressed
            if((ascii2hid[i][1]==0 && !(mod && (SHIFT|ALTGR))) || (ascii2hid[i][1] & mod)) {
                return((char)(i));
            }
        }
    }
    return ('\0');
}

bool setMacroRecording( uint8_t id )
{
    if(id<MACROCOUNT) {
        g_macrorecord=id;
        idx=0;
        return true;
    }
    g_macrorecord = INVALID_MACRO_ID;
    return false;
};

/** Record key into macro:
 *    Up to MACROLEN keys may be stored, but any combination of modifiers takes one additional slot.
 *    @todo: Dynamic length, macro idx
 *
 *    Ctrl+Enter terminates macro entry
 *    Ctrl+Esc   aborts
 *    Alt+Enter  inserts pause
 */
void macro_key(uint8_t hid, uint8_t mod)
{
    // printf("\nMC(%d) : %d:%d =%c   ",idx, mod, hid, hid2asciicode(hid,mod) );
    // int i; for(i=0; i<MACROLEN; ++i) printf("%d ", hidmacro[i]);
    if(!macroRecording())
        return;

    if(hid == HID_ESC && mod == CTRL) {
        disableMacroRecording();
        printf("\nabort");
        return;
    }
    // Ctrl+Enter ends macro recording
    if((hid == HID_ENTER && mod == CTRL) || idx==MACROLEN-1) {
        hidmacro[idx]=0;
        uint8_t written=updateEEMacroHID(hidmacro, g_macrorecord);
        printf("\nWrote %d/%d", written,idx);
        idx=0;

        disableMacroRecording();
        return;
    }

    if(idx<MACROLEN) {
        if(mod != 0) {
            //printf("\n%02x->%02x", hidmacro[idx],mod+0x80);
            hidmacro[idx]=(mod+0x80);
            ++idx;
        }
    }
    if(idx<MACROLEN) {
        if(hid != 0) {
            //printf("\n%02x->%02x", hidmacro[idx],hid);
            hidmacro[idx]=hid;
            ++idx;
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
                _delay_ms(400);
                c=0; mod=0;
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
 * Caller needs to make sure there is enough space allocated!
 */
uint8_t readEEMacroHID(uint8_t * macro, uint8_t idx)
{
    eeprom_busy_wait();
    uint8_t len=eeprom_read_byte (( const void *) EE_ADDR_MACRO(idx) );

    if(len>MACROLEN)
        len=MACROLEN;

    eeprom_busy_wait();
    eeprom_read_block (( void *) macro, ( const void *) (EE_ADDR_MACRO(idx)+1), len);

    macro[len]=0;
    // printf("\nEE read #%d @%d len=%d: ", idx, EE_ADDR_MACRO(idx), len);
    // uint8_t i; for(i=0;i<len;++i) printf(" %02x", macro[i]);

    return len;
}


/**
 * Writes the macro to eeprom at given index and returns length of written string.
 * As eeprom update functions are used, no unnecessary writes are performed.
 *
 * @param macro array of hid/modifier codes to store, '0' signals end of macro
 * @param idx   index of macro to store
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
    eeprom_update_byte ((void *) EE_ADDR_MACRO(idx), len );
    eeprom_busy_wait();
    eeprom_update_block (( const void *) macro, (void *) (EE_ADDR_MACRO(idx)+1), len );

    return len;
}

