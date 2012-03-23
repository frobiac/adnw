#include "macro.h"

/// Increasing either too much leads to keyboard not registering at all!
#define MACROLEN    20
#define MACROCOUNT  3

struct keycode macros[MACROCOUNT][MACROLEN] =
{
{MacroKilled},
{MacroKilled},
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
"MacroKilled"
};

bool macroMode(void) {
    return(macromode != 0);
}

void setMacroMode( bool on ) {
    macromode=on;
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
    if(id<MACROCOUNT){
        idx=0;
        curMacro=id;
        return true;
    } else {
        setMacroMode(false);
        return false;
    }
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
        macromode = 0;
        idx  = 0;
        curMacro=MACROCOUNT;
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
        //kc = _L;
        kc->hid=0; kc->mods=0;
        return true;
    }

    if( idx < sizeof(macros[curMacro])/sizeof(struct keycode) ) {
        *kc=macros[curMacro][idx];
        idx++;
        return true;
    } else {
        macromode = 0;
        idx  = 0;
        curMacro=MACROCOUNT;
    }

    return false;
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
