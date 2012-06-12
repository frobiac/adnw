#include "macro.h"
//#include "hhstdio.h"
#include "Keyboard.h"

/// Increasing either too much leads to keyboard not registering at all!
#define MACROLEN    20
#define MACROCOUNT  6

// end all macros with "_no" as dynamic length detection does not work yet
struct keycode macros[MACROCOUNT][MACROLEN] = {
{MacroKilled},
{MacroKilled},
{MacroKilled},
    { _h, _a, _l, _l, _o , _no},
    { _SPACE,_L_CTRL, _c, _no },
/MacroKilled }
{MacroKilled }
};


uint8_t curMacro  = MACROCOUNT;
uint8_t sendEmpty = 0;    // empty report needed to send the same character twice in a row
uint8_t idx=0;


bool macromode=false;

/**
 * Only standard ascii characters work: No umlauts!
 */
static const  char  macrosC[][MACROLEN]  =  {
    /*   "^X!\"X$%&/()=?`",
    "{[]}\\+*~#'",
    "<>|,;.:-_",
    "@´`",
    */
"MacroKilled",
"MacroKilled",
"MacroKilled",
    { (L_CTL|L_ALT)+0x80, 127, '\0' /*CTRL_ALT_DEL*/},
    { 'a' },
{MacroKilled }
};

bool macroMode(void)
{
    return(macromode != 0);
}

void setMacroMode( bool on )
{
    macromode=on;
}

void endMacro()
{
    macromode = 0;
    idx  = 0;
    curMacro=MACROCOUNT;
}

/**
 * set current active macro
 */
bool activateMacro(uint8_t id)
{
    if(!macromode)
        return false;
    // de-bounce not working in current macro detection
    if(id==curMacro)
        return true;
    if(id<MACROCOUNT) {
        idx=0;
        curMacro=id;
        return true;
    } else {
        setMacroMode(false);
        return false;
    }
}

bool getMacroReport(USB_KeyboardReport_Data_t *report)
{
    char c;
    if(!macromode)
        return false;
    if(curMacro>=MACROCOUNT)
        return false;

    sendEmpty = sendEmpty ? 0 : 1;
    if( sendEmpty) {
        c='\0';
        memset(&report->KeyCode[0], 0, 6);
        //sendEmpty = 0;
        return true;
    }

    if( idx < sizeof(macrosC[curMacro])/sizeof(char) ) {
        uint8_t mod = 0;

        c=macrosC[curMacro][idx];

        // if > 127, it is a modifier
        if(c & 0x80 ){
            mod=(c & 0x7F);
            c=0;
            sendEmpty = sendEmpty ? 0 : 1;
        }

        memcpy_P(report, &ascii_table[(uint8_t)c], sizeof(USB_KeyboardReport_Data_t));
        report->Modifier |= mod;
        // set remaining 5 bytes to 0
        memset(&report->KeyCode[1], 0, 5);

        //if(report->Modifier != 0)
        //    sendEmpty = 1;

        idx++;
        return true;
    } else {
        endMacro();
        return false;
    }

    return false;
}

bool getMacroCharacter(char *c)
{
    if(!macromode)
        return false;
    if(curMacro>=MACROCOUNT)
        return false;

    sendEmpty = sendEmpty ? 0 : 1;
    if( sendEmpty) {
        *c='\0';
        return true;
    }

    if( idx < sizeof(macrosC[curMacro])/sizeof(char) ) {
        *c=macrosC[curMacro][idx];
        idx++;
        return true;
    } else {
        endMacro();
        return false;
    }

    return false;
}

bool getMacroChar(struct keycode *kc)
{
    if(!macromode)
        return false;
    if(curMacro>=MACROCOUNT)
        return false;

    sendEmpty = sendEmpty ? 0 : 1;
    if( sendEmpty) {
        kc->hid=0;
        kc->mods=0;
        return true;
    }
    // check if we're at the end
    if(macros[curMacro][idx].hid == 0 ){//&& macros[curMacro][idx].mods == NONE ){
        endMacro();
        return false;
    }

        while( macros[curMacro][idx].hid >= HID_L_CONTROL &&
            macros[curMacro][idx].hid <= HID_R_GUI ) {
            // nasty difference to general modifier usage (shift & altgr)
            kc->mods |= 1 << (macros[curMacro][idx].mods - MOD_BEGIN);
            idx++;
        }
        kc->hid = macros[curMacro][idx].hid;
        kc->mods |= macros[curMacro][idx].mods;
        //printf("\nMACRO %d [%d]: %c=%d + %d" ,curMacro, idx, macros[curMacro][idx].ch, kc->hid,  kc->mods);
        idx++;

        if(macros[curMacro][idx].hid == 0 ){//&& macros[curMacro][idx].mods == NONE ){
            endMacro();
        }

        return true;
}



/** Goes through given macro and sends it letter by letter.
 *  We could try to be smart and fill the report with up to 6 letters,
 *  but keeping track fo modifier changes and duplicates makes this over-complicated.
 */
/*
uint8_t fillMacroReport(USB_KeyboardReport_Data_t *report_data)
{
    if(!g_macro_mode)
        return 0;
    if(sendEmpty++%2)
        return sizeof(USB_KeyboardReport_Data_t);

    if( idx < sizeof(macro[curMacro)/sizeof(struct keycode) ) {
        report_data->KeyCode[0]=macro[curMacro][idx].hid;
        report_data->Modifier=macro[curMacro][idx].mods;
        idx++;
        return sizeof(USB_KeyboardReport_Data_t);
    } else {
        g_macro_mode = 0;
        idx  = 0;
    }

    return 0;
}
*/
