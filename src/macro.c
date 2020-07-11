/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2020 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
 * @TODO:
 *   - clear up convoluted mess of string assembly in command.c
 *   - interface for dynamic lengths of different macros
 *   - usability: signal remaining free slots, used characters, ...
 */


#include "macro.h"
#include "helpers.h"
#include "Keyboard.h"
#include "ascii2hid.h"

#include "global_config.h"

#define MACRO_ID_INVALID 255     // 255 means no recording is going on.
#define MACRO_INVALID    255     // can be used by anyone here
#define MACRO_COMPLETE   254     // outstring has been written to completely and can be printed

static uint8_t sendEmpty;    // empty report needed to send the same character twice in a row

/// Holds index of macro while recording, 0<=idx<MACROCOUNT.
static uint8_t g_macrorecord=MACRO_ID_INVALID;

/// cur offset in outHidCodes, both read & write for macro
static uint8_t outOffs = MACRO_INVALID;

/// Buffer for EEPROM macros and strings to print out
static uint8_t outHidCodes[MACRO_MAX_LEN+1];

static inline void disableMacroRecording(void) { g_macrorecord=MACRO_ID_INVALID; setCommandMode(false); outOffs=MACRO_INVALID; };

bool appendHidCode(uint8_t hid);
bool clearHIDCodes(void);
uint8_t findMacroId(char macro_char);
uint8_t findFreeMacroId(void);
void print_used_macro_chars(void);



/**
 * Find macro id for given character.
 *
 * @return id, or MACRO_ID_INVALID if not found.
 */
uint8_t findMacroId(char macro_char)
{
    uint8_t offs;
    for(offs=0; offs<MACROCOUNT; ++offs) {
        if(macro_char == eeprom_read_byte (( const void *) (EE_ADDR_MACRO_MAP + offs)) ) {
            return offs;
        }
    }
    // not found
    return MACRO_ID_INVALID;
}

/**
 * Return index of first non-alnum macro character in map,
 * or MACRO_ID_INVALID if all slots are in use.
 */
uint8_t findFreeMacroId()
{
    char macro_char;
    uint8_t offs;
    for(offs=0; offs<MACROCOUNT; ++offs) {
        macro_char = eeprom_read_byte (( const void *) (EE_ADDR_MACRO_MAP + offs));
        if(! isalnum(macro_char)) {
            return offs;
        }
    }
    // not found
    return MACRO_ID_INVALID;
}

void print_used_macro_chars()
{
    uint8_t offs;
    char macro_char __attribute__((unused));
    xprintf("\nM:");
    for(offs=0; offs<MACROCOUNT; ++offs) {
        macro_char = eeprom_read_byte (( const void *) (EE_ADDR_MACRO_MAP + offs));
        xprintf("%c ", isalnum(macro_char) ? macro_char : '-');
    }
}

/**
 * Update macro for character c, or create a new one in free slot.
 * Free slot is indicated with map entry of index = MACRO_INVALID.
 *
 */
bool setMacroRecording(char macro_char, uint8_t hid, uint8_t mod)
{
    if(g_macrorecord >= MACROCOUNT) { // first call, new macro
        if(!isalnum(macro_char)) // do not store what cannot be retrieved :-)
            goto err;

        uint8_t offs = MACRO_ID_INVALID;

        offs = findMacroId(macro_char); // first check if macro_char is already in use
        if(offs == MACRO_ID_INVALID)
            offs = findFreeMacroId(); // find empty spot

        if(offs != MACRO_ID_INVALID && clearHIDCodes()) {
            g_macrorecord=offs;
            outHidCodes[0]=macro_char;
            outOffs=1;
            return true;
        } else { // no free slot found
            print_used_macro_chars();
        }
    } else { // next macro character
        macro_key(hid,mod);
        return true;
    }

err:
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
    if(hid == HID_ESC && mod == HID_MOD_MASK(MOD_L_CTRL)) {
        disableMacroRecording();
        return;
    }
    // Ctrl+Enter ends macro recording
    if(hid == HID_ENTER && mod == HID_MOD_MASK(MOD_L_CTRL)) {
        if( appendHidCode(0) ) {
            uint8_t written __attribute__((unused));
            written=updateEEMacroHID(outHidCodes, g_macrorecord);
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
uint8_t printMacro(char macro_char)
{
    uint8_t ret=0;
    if(outOffs==MACRO_INVALID) { // Ready to read, not in use
        uint8_t macro_idx = MACRO_ID_INVALID;
        macro_idx=findMacroId(macro_char);
        if(macro_idx != MACRO_INVALID) {
            ret=readEEMacroHID(outHidCodes, macro_idx);
            outOffs=MACRO_COMPLETE;
        } else {
            print_used_macro_chars();
        }
    }
    return ret;
}

/**
 * Reads the macro at given index from eeprom into macro and returns its length.
 * Caller needs to make sure there is enough space allocated!
 */
uint8_t readEEMacroHID(uint8_t * macro, uint8_t idx)
{
    if(idx>=MACROCOUNT)
        return 0;

    eeprom_busy_wait();
    uint8_t len=eeprom_read_byte (( const void *) EE_ADDR_MACRO(idx) );

    if(len>MACRO_MAX_LEN)
        len=MACRO_MAX_LEN;

    eeprom_busy_wait();
    eeprom_read_block (( void *) macro, ( const void *) (EE_ADDR_MACRO(idx)+1), len);

    macro[len]=0;

    // xor with unlock code
    decrypt(macro,len);

    return len;
}


/**
 * Writes the macro to eeprom at given index and returns length of written string.
 * If macro[0] is 0 then clear its access character from map to free it.
 *
 * As eeprom update functions are used, no unnecessary writes are performed.
 *
 * @param macro array of hid/modifier codes to store, '0' signals end of macro
 *              first character is macro selector character
 * @param idx   index of macro to store
 */
uint8_t updateEEMacroHID(uint8_t * macro, uint8_t idx)
{
    if(idx>=MACROCOUNT)
        return 0;

    if(macro[1] == 0) { // clear macro - [0] contains selector character.
        eeprom_busy_wait();
        eeprom_update_byte ((void *) (EE_ADDR_MACRO_MAP + idx), MACRO_ID_INVALID);
        return 0;
    }

    uint8_t len=0;
    while(macro[len] != 0 && len < MACRO_MAX_LEN) {
        ++len;
    }
    // xor with unlock code, macro[0] contains selector character so ignore that one
    encrypt(&macro[1], len-1);

    eeprom_busy_wait();
    eeprom_update_byte ((void *) (EE_ADDR_MACRO_MAP + idx), macro[0]);

    eeprom_busy_wait();
    eeprom_update_byte ((void *) EE_ADDR_MACRO(idx), len-1 );

    eeprom_busy_wait();
    eeprom_update_block (( const void *) (&macro[1]), (void *) (EE_ADDR_MACRO(idx)+1), len-1 );

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
        ascii2hid(ch, &hid, &mod);
        //hid=ascii2hid[ch][0];
        //mod=HID_MOD_MASK(ascii2hid[ch][1]);

        if(mod != 0)
            appendHidCode(mod|0x80);
        if(hid != 0)
            appendHidCode(hid);
    }

    outOffs = MACRO_COMPLETE;
    return 1;
};


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
            if( mod==HID_MOD_MASK(MOD_L_ALT) && c==HID_ENTER) {
                _delay_ms(400);
                c=0; mod=0;
            }
        }
        // @TODO Should check complete macro before it is entered into buffer.
        if(c > HID_F24)
            goto all_printed;

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


