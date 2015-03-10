/*
                   The HumbleHacker Keyboard Project
                 Copyright © 2008-2010, David Whetstone
              david DOT whetstone AT humblehacker DOT com

  This file is a part of the HumbleHacker Keyboard Firmware project.

  The HumbleHacker Keyboard Project is free software: you can
  redistribute it and/or modify it under the terms of the GNU General
  Public License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  The HumbleHacker Keyboard Project is distributed in the
  hope that it will be useful, but WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with The HumbleHacker Keyboard Firmware project.  If not, see
  <http://www.gnu.org/licenses/>.

  --------------------------------------------------------------------

  This code is based on the keyboard demonstration application by
  Denver Gingerich.

  Copyright 2008  Denver Gingerich (denver [at] ossguy [dot] com)

  --------------------------------------------------------------------

  Gingerich's keyboard demonstration application is based on the MyUSB
  Mouse demonstration application, written by Dean Camera.

             LUFA Library
     Copyright (C) Dean Camera, 2010.

  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
  --------------------------------------------------------------------

  Mouse handling added by frobiac, 2010

  --------------------------------------------------------------------

*/

/** \file
 *
 *  Main source file for the Keyboard demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */

#include <assert.h>

#include "Keyboard.h"
#include "keyboard_class.h"
#include "dbg.h"

#include "hid_usage.h"

#ifdef PS2MOUSE
    #include "trackpoint.h"
#endif

#include "mousekey.h"
#ifdef ANALOGSTICK
    #include "analog.h"
#endif

#include "macro.h"
#include "command.h"

/** Buffer to hold the previously generated Mouse HID report, for comparison purposes inside the HID class driver. */
uint8_t PrevMouseHIDReportBuffer[sizeof(USB_MouseReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the mouse HID
 *  interface within the device.
 */
USB_ClassInfo_HID_Device_t Mouse_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = 2,

        .ReportINEndpoint             =
        {
            .Address              = MOUSE_IN_EPADDR,
            .Size                 = HID_EPSIZE,
            .Banks                = 1,
        },

        .PrevReportINBuffer           = PrevMouseHIDReportBuffer,
        .PrevReportINBufferSize       = sizeof(PrevMouseHIDReportBuffer),
    },
};


/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = 0,

        .ReportINEndpoint             =
        {
            .Address              = KEYBOARD_IN_EPADDR,
            .Size                 = KEYBOARD_EPSIZE,
            .Banks                = 1,
        },

        .PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
        .PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
    },
};

uint8_t PrevDBGHIDReportBuffer[sizeof(USB_DBGReport_Data_t)];

USB_ClassInfo_HID_Device_t DBG_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = 1,

        .ReportINEndpoint             =
        {
            .Address              = DBG_IN_EPADDR,
            .Size                 = DBG_EPSIZE,
            .Banks                = 1,
        },

        .PrevReportINBuffer           = PrevDBGHIDReportBuffer,
        .PrevReportINBufferSize       = sizeof(PrevDBGHIDReportBuffer),
    },
};


//uint8_t g_num_lock, g_caps_lock, g_scrl_lock;

void led(uint8_t n)
{
    DDRD |= (1 << 6);
    if(n==0)
        PORTD &= ~(1 << 6);
    else if(n==1)
        PORTD |= (1 << 6);
    return;
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
static void __attribute__ ((noreturn)) main_loop(void)
{
    SetupHardware();

    sei();

    for (;;) {
        if (USB_DeviceState != DEVICE_STATE_Suspended) {
            HID_Device_USBTask(&Keyboard_HID_Interface);
            HID_Device_USBTask(&DBG_HID_Interface);
#ifdef PS2MOUSE
            if( g_mouse_enabled )
                HID_Device_USBTask(&Mouse_HID_Interface);
#endif
        } else if (USB_Device_RemoteWakeupEnabled ) {
            USB_CLK_Unfreeze();
            USB_Device_SendRemoteWakeup();
        }
        USB_USBTask();
    }
}

int main(void)
{
    main_loop();
    return 0;
}


/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
{
    /* Disable clock division */
    //clock_prescale_set(clock_div_2);
    // set for 16 MHz clock
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00
    CPU_PRESCALE(CPU_16MHz);

    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Hardware Initialization */

#ifdef ANALOGSTICK
    Analog_Init();
#endif

    LEDs_Init();
    USB_Init();
    USB_PLL_On();
    while (!USB_PLL_IsReady());

    /* Task init */
    initKeyboard();

    // g_num_lock = g_caps_lock = g_scrl_lock = 0;

    // set up timer
    TCCR0A = 0x00;
    TCCR0B = 0x05;
    TIMSK0 = (1<<TOIE0);

#ifdef PS2MOUSE
    if( ps2_init_mouse() )
        printf("\nTP initialized");
    else
        printf("\nTP init FAILED!");
#endif

    printf("\nAdNW : %s", FW_VERSION);

#if defined(BOOTLOADER_TEST)
    uint8_t bootloader = eeprom_read_byte(&ee_bootloader);
    if (bootloader == 0xff) { // eeprom has been reset
        eeprom_write_byte(&ee_bootloader, FALSE);
    } else if (bootloader == TRUE) {
        __asm__("jmp 0xF000");
    }
#endif
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_READY);

    if (!(HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface)))
        LEDs_SetAllLEDs(LEDMASK_USB_ERROR);

    if (!(HID_Device_ConfigureEndpoints(&DBG_HID_Interface)))
        LEDs_SetAllLEDs(LEDMASK_USB_ERROR);

    if (!(HID_Device_ConfigureEndpoints(&Mouse_HID_Interface)))
        LEDs_SetAllLEDs(LEDMASK_USB_ERROR);

    USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Unhandled Control Request event. */
void EVENT_USB_Device_ControlRequest(void)
{
    HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
    HID_Device_ProcessControlRequest(&DBG_HID_Interface);
    HID_Device_ProcessControlRequest(&Mouse_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
    HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
    HID_Device_MillisecondElapsed(&DBG_HID_Interface);
    HID_Device_MillisecondElapsed(&Mouse_HID_Interface);
}


/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID  Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in] ReportType  Type of the report to create, either REPORT_ITEM_TYPE_In or REPORT_ITEM_TYPE_Feature
 *  \param[out] ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out] ReportSize  Number of bytes written in the report (or zero if no report is to be sent
 *
 *  \return Boolean true to force the sending of the report, false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, uint8_t* const ReportID,
        const uint8_t ReportType, void* ReportData, uint16_t* ReportSize)
{
    if (HIDInterfaceInfo == &Keyboard_HID_Interface) {
        USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;
        *ReportSize = sizeof(USB_KeyboardReport_Data_t);
        getKeyboardReport(KeyboardReport) ;
        // clear report in command mode to disable echoing of selected commands.
        if( handleCommand(KeyboardReport->KeyCode[0], KeyboardReport->Modifier) ) {
            memset(&KeyboardReport->KeyCode[0], 0, 6 );
            KeyboardReport->Modifier=0;
        }
        return false;
    }
#ifdef DEBUG_OUTPUT
    else if (HIDInterfaceInfo == &DBG_HID_Interface) {
        USB_DBGReport_Data_t* DBGReport = (USB_DBGReport_Data_t*)ReportData;
        *ReportSize = DBG__get_report(DBGReport);
    }
#endif

    else if (HIDInterfaceInfo == &Mouse_HID_Interface) {
        USB_MouseReport_Data_t* MouseReport = (USB_MouseReport_Data_t*)ReportData;

        /// @todo : is this needed?
        // mouse through key emulation only if PS/2 mouse report is "empty"
#ifdef PS2MOUSE
        *ReportSize = getMouseReport(MouseReport);
        if( *ReportSize == 0 || ( MouseReport->X == 0 && MouseReport->Y == 0 &&
#ifdef MOUSE_HAS_SCROLL_WHEELS
                                  MouseReport->V == 0 && MouseReport->H == 0 &&
#endif
                                  MouseReport->Button == 0 ) ) {
#endif
            *ReportSize = getMouseKeyReport(MouseReport);
            if( *ReportSize == 0 ) {
                MouseReport->X = 0; MouseReport->Y = 0;
#ifdef MOUSE_HAS_SCROLL_WHEELS
                MouseReport->V = 0; MouseReport->H = 0;
#endif
                MouseReport->Button = 0;
            }
#ifdef PS2MOUSE
        }
#endif
        // force sending so mouse stops when empty
        return true;

    }

    return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, const uint8_t ReportID,
        const uint8_t ReportType, const void* ReportData, const uint16_t ReportSize)
{
    if (HIDInterfaceInfo == &Keyboard_HID_Interface) {
        uint8_t* LEDReport = (uint8_t*)ReportData;
        hostLEDChange(*LEDReport);
    }
}
