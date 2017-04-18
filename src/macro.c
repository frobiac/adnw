/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#include "global_config.h"

#define MACRO_ID_INVALID 255     // 255 means no recording is going on.
#define MACRO_INVALID    255     // can be used by anyone here 
#define MACRO_COMPLETE   254     // outstring has been written to completely and can be printed

uint8_t curMacro  = MACROCOUNT;
uint8_t sendEmpty = 0;    // empty report needed to send the same character twice in a row

bool macromode=false;
/// Holds index of macro while recording, 0<=idx<MACROCOUNT.
uint8_t g_macrorecord=MACRO_ID_INVALID;

/// cur offset in outHidCodes, both read & write for macro
uint8_t outOffs = MACRO_INVALID;

/// Buffer for EEPROM macros and strings to print out
uint8_t outHidCodes[MACRO_MAX_LEN+1];


inline bool macroRecording(void)         { return(g_macrorecord != MACRO_ID_INVALID); };
inline void disableMacroRecording(void) { g_macrorecord=MACRO_ID_INVALID; setCommandMode(false); outOffs=MACRO_INVALID; };
inline void setMacroMode( bool on ) { macromode=on; };

bool appendHidCode(uint8_t hid);
bool clearHIDCodes(void);


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
    if(id<MACROCOUNT && clearHIDCodes() ) {
        g_macrorecord=id;
        return true;
    }
    g_macrorecord = MACRO_ID_INVALID;
    return false;
};

/**
 * @brief appendHidCode Append given hid code / modifier to buffer
 * @param hid HID code or (modifier|0x80) or 0 to end.
 * @return true if successful
 */
bool appendHidCode(uint8_t hid)
{
    if(outOffs<MACRO_MAX_LEN) {
        outHidCodes[outOffs] = hid;
        ++outOffs;
        return true;
    }
    return false;
}


bool clearHIDCodes()
{
    if(outOffs != MACRO_INVALID)
        return false;

    memset(outHidCodes,0,MACRO_MAX_LEN+1);
    outOffs=0;
    return true;
}


/** Record key into macro:
 *    Up to MACRO_MAX_LEN keys may be stored, but any combination of modifiers takes one additional slot.
 *    @todo: Dynamic length, macro idx
 *
 *    Ctrl+Enter terminates macro entry
 *    Ctrl+Esc   aborts
 *    Alt+Enter  inserts pause
 */
void macro_key(uint8_t hid, uint8_t mod)
{
    if(hid == HID_ESC && mod == CTRL) {
        disableMacroRecording();
        xprintf("\nabort");
        return;
    }
    // Ctrl+Enter ends macro recording
    if(hid == HID_ENTER && mod == CTRL) {
        if( appendHidCode(0) ) {
            uint8_t written __attribute__((unused));
            written=updateEEMacroHID(outHidCodes, g_macrorecord);
            xprintf("\nWrote %d/%d", written, outOffs);
        }

        disableMacroRecording();
        return;
    }

    if(mod != 0)
        appendHidCode(mod|0x80);
    if(hid != 0)
        appendHidCode(hid);

}

/// shortcut to put macro directly in print buffer
uint8_t printMacro(uint8_t idx)
{
    uint8_t ret=0;
    if(outOffs==MACRO_INVALID) {
        ret=readEEMacroHID(outHidCodes, idx);
        outOffs=MACRO_COMPLETE;
    }
    return ret;
}

/**
 * Reads the macro at given index from eeprom into macro and returns its length.
 * Caller needs to make sure there is enough space allocated!
 */
uint8_t readEEMacroHID(uint8_t * macro, uint8_t idx)
{
    eeprom_busy_wait();
    uint8_t len=eeprom_read_byte (( const void *) EE_ADDR_MACRO(idx) );

    if(len>MACRO_MAX_LEN)
        len=MACRO_MAX_LEN;

    eeprom_busy_wait();
    eeprom_read_block (( void *) macro, ( const void *) (EE_ADDR_MACRO(idx)+1), len);

    macro[len]=0;

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
    while(macro[len] != 0 && len < MACRO_MAX_LEN) {
        len++ ;
    }

    eeprom_busy_wait();
    eeprom_update_byte ((void *) EE_ADDR_MACRO(idx), len );
    eeprom_busy_wait();
    eeprom_update_block (( const void *) macro, (void *) (EE_ADDR_MACRO(idx)+1), len );

    return len;
}


/**
 * Will copy given string to output buffer.
 * It is then delivered by the call to printOutStr() in keyboards main loop.
 */
uint8_t setOutputString(char * str)
{
    if (! clearHIDCodes() )
        return 0;

    uint8_t i, ch, hid, mod;
    // convert each char to HID codes
    for(i=0; str[i] != '\0'; ++i) {
        ch=str[i];
        hid=ascii2hid[ch][0];
        mod=ascii2hid[ch][1];

        if(mod != 0)
            appendHidCode(mod|0x80);
        if(hid != 0)
            appendHidCode(hid);
    }

    outOffs = MACRO_COMPLETE;
    return 1;
};

/**
 * @brief setOutputHIDCodes Will copy given HID code sequence to output buffer.
 *
 *
 * Modifier bitmasks must be encoded with the highest bit set (|0x80) and will be applied
 * to the following HID code. This enables output of control sequences and non-ascii
 * characters like '€' or '°' in printOutstr().
 *
 * @param hidcodes sequence of modifiers and hidcodes, terminated with '0'
 * @return  0 on success
 *
 * @todo Pass length
 */
uint8_t setOutputHIDCodes(uint8_t * hidcodes)
{
    if (! clearHIDCodes() )
        return 0;

    memcpy(outHidCodes, hidcodes, MACRO_MAX_LEN);

    outOffs = MACRO_COMPLETE;
    return 1;
}



/**
 * Fills report with one characters from outHidCodes[] until all done.
 *
 * @todo : could send up to 6 if no modifiers, but not worth the extra checks.
 *
 */
uint8_t printOutstr(USB_KeyboardReport_Data_t * report)
{
    // do _NOT_ mess with report unless we need to!
    if(outOffs!=MACRO_COMPLETE)
        return 0;

    static uint8_t readOffs = 0;

    if( sendEmpty) {
        memset(&report->KeyCode[0], 0, 6);
        sendEmpty = sendEmpty ? 0 : 1;
        return 1;
    }

    uint8_t mod=0;
    if( readOffs<MACRO_MAX_LEN) {
        uint8_t c = outHidCodes[readOffs++];
        if(c == 0)
            goto all_printed;

        // can handle extra modifiers here like in getMacroReport()
        // if > 127, it is a modifier
        if( (c&0x80) && readOffs<MACRO_MAX_LEN) {
            mod=(c&0x7F);
            c = outHidCodes[readOffs++];
            // assert c != mod here?
            if( mod==ALT && c==HID_ENTER) {
                _delay_ms(400);
                c=0; mod=0;
            }
        }
        // now fill in one character
        /// @todo Is this necessary - purge zeroReport() ???
        memset(&report->KeyCode[0],0,6);
        report->KeyCode[0]=c;
        report->Modifier  =mod;
        sendEmpty = sendEmpty ? 0 : 1;
        return 1;
    }

all_printed:
    // getting here means we're done, so invalidate for next run
    outOffs=MACRO_INVALID;
    readOffs=0;
    return 0;
}

