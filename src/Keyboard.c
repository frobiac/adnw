/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

    It is based on the KeyboardMouse demo from LUFA.

*/

/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Based on the LUFA KeyboardMouse demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "Keyboard.h"
#include "keyboard_class.h"
#include "matrix.h" //scan_matrix()
#include "print.h"

#include "hid_usage.h"

#ifdef PS2MOUSE
    #include "trackpoint.h"
#endif

#include "mousekey.h"
#ifdef ANALOGSTICK
    #include "analog.h"
#endif

#include "trackpoint.h"
#include "macro.h"
#include "command.h"
/** Buffer to hold the previously generated HID reports, for comparison purposes inside the HID class drivers. */
static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];
static uint8_t PrevMouseHIDReportBuffer[sizeof(USB_WheelMouseReport_Data_t)];
#ifdef DEBUG_OUTPUT
    static uint8_t PrevDBGHIDReportBuffer[sizeof(USB_DBGReport_Data_t)];
#endif

#ifdef EXTRA
    #include "extra.h"
    static uint8_t PrevExtraHIDReportBuffer[sizeof(USB_ExtraReport_Data_t)];
#endif
/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Mouse_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = INTERFACE_ID_Mouse,

        .ReportINEndpoint             =
        {
            .Address              = MOUSE_IN_EPADDR,
            .Size                 = MOUSE_EPSIZE,
            .Banks                = 1,
        },

        .PrevReportINBuffer           = PrevMouseHIDReportBuffer,
        .PrevReportINBufferSize       = sizeof(PrevMouseHIDReportBuffer),
    },
};


USB_ClassInfo_HID_Device_t Keyboard_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = INTERFACE_ID_Keyboard,

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

#ifdef DEBUG_OUTPUT
USB_ClassInfo_HID_Device_t DBG_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = INTERFACE_ID_Debug,

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
#endif

#ifdef EXTRA
USB_ClassInfo_HID_Device_t Extra_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = INTERFACE_ID_Extra,

        .ReportINEndpoint             =
        {
            .Address              = EXTRA_IN_EPADDR,
            .Size                 = EXTRA_EPSIZE,
            .Banks                = 1,
        },

        .PrevReportINBuffer           = PrevExtraHIDReportBuffer,
        .PrevReportINBufferSize       = sizeof(PrevExtraHIDReportBuffer),
    },
};
#endif


void led(uint8_t n)
{
    DDRD |= (1 << 6);
    if(n==0)
        PORTD &= ~(1 << 6);
    else if(n==1)
        PORTD |= (1 << 6);
    return;
}

bool suspend_wakeup_condition(void);
/**
 * Wake up host on 3 or more pressed keys.
 * @todo: Power saving, idle, sleep...
 */
bool suspend_wakeup_condition()
{
    scan_matrix();
    init_active_keys();
    return (activeKeys.keycnt > 2);
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
static void __attribute__ ((noreturn)) main_loop(void)
{
    SetupHardware();

#ifdef DEBUG_OUTPUT
    InitDebugPrint();
#endif

    GlobalInterruptEnable();

    for (;;) {
        while (USB_DeviceState == DEVICE_STATE_Suspended) {
            if (USB_Device_RemoteWakeupEnabled && suspend_wakeup_condition()) {
                USB_CLK_Unfreeze();
                USB_Device_SendRemoteWakeup();
            }
        }
        if (USB_DeviceState != DEVICE_STATE_Suspended) {
            HID_Device_USBTask(&Keyboard_HID_Interface);
#ifdef DEBUG_OUTPUT
            HID_Device_USBTask(&DBG_HID_Interface);
#endif
#ifdef EXTRA
            HID_Device_USBTask(&Extra_HID_Interface);
#endif
#ifdef PS2MOUSE
            if( g_cfg.fw.mouse_enabled )
                HID_Device_USBTask(&Mouse_HID_Interface);
#endif
        }

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
        USB_USBTask();
#endif
    }
}

int main(void)
{
    main_loop();
    return 0;
}

/**
 * BF: One on PD7 is OC4D, can set OCR4D to 0-255
 * BB: RGB on PB5,6,7 = OC1A, OC1B, 0C1C and Timer 1
 *
 * http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/Die_Timer_und_Z%C3%A4hler_des_AVR
 * http://blog.saikoled.com/post/43165849837/secret-konami-cheat-code-to-high-resolution-pwm-on
 */
void initPWM()
{
    // timer 4 not available on teensy++2.0 with at90usb1286
#if defined(HAS_LED) && defined(__AVR_ATmega32U4__)
#   ifdef BLACKFLAT
    DDRD = (1<<7);
    //@TODO what is really required to get PWM on PD7?
    TCCR4A = 1 << COM4A1 | 1 << COM4B1 | 1 << PWM4A | 1 << PWM4B; // Enable outputs A and B in PWM mode
    TCCR4B = 1 << CS43 | 1 << CS40;    // Clock divided by 256 for 244kHz PWM
    TCCR4C = 1 << COM4D1 | 1 << PWM4D; // Enable output D in PWM mode
    // OC4C=0xFF; // Set top to FFFF

#   elif defined BLACKBOWL
    //Set 8-bit fast PWM mode for RGB on B5,6,7
    DDRB |= (1<<7)|(1<<6)|(1<<5);

    //Mode and Prescaler (Phase Correct PWM und Prescaler 1)
    TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1)|(1<<WGM11); // 0xAA
    TCCR1B |= (1<<WGM13)|(1<<CS10); // 0x19
    // 16-Bit Resolution ICR1 |= 0xFFFF;
    // 8-Bit Resolution
    ICR1 |= 0x00FF;

#   endif
#endif
}

void enable_mouse_keys(uint8_t on)
{
    if(on!=g_mouse_keys_enabled) {
#if defined (PS2MOUSE)
        tp_sensitivity(on? g_cfg.tp_config.sens : g_cfg.tp_config.sensL);
#endif
        // @TODO must use some way of activating different modes, keeping track of changes:
        // e.g: Start command mode, change some values there, move mouse and while it
        // is still enabled leave command mode...
        //set_led_mode(LED_MOUSE, on); // -> OCR4D= (on ? 20 : 0);
    }
    g_mouse_keys_enabled = on;
}


/** Configures the board hardware and chip peripherals for keyboard and mouse functionality. */
void SetupHardware()
{
#if (ARCH == ARCH_AVR8)
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);
#endif

    /* Hardware Initialization */
#ifdef ANALOGSTICK
    Analog_Init();
#endif

    LEDs_Init();
    USB_Init();
//    USB_PLL_On();
//    while (!USB_PLL_IsReady());

    // g_num_lock = g_caps_lock = g_scrl_lock = 0;

    // set up timer
    TCCR0A = 0x00;
    TCCR0B = 0x05;

#ifdef HAS_LED
    // @TODO tensy++2.0 at90usb1286 does not have 4th timer?
#  ifdef BLACKFLAT
    PORTC &= ~(1<<7);
    DDRC |= (1<<7);  // D7 is external LED on BF -> output
#  elif defined(BLACKBOWL)
    // BB: RGB on B5,67=OC1A, OC1B, OC1C
    PORTB &= ~((1<<7)|(1<<6)|(1<<5));
    DDRB |= (1<<7)|(1<<6)|(1<<5);  // B5,6,7 is external RGB LED on BF -> output
#  endif
    initPWM();
#endif

    TIMSK0 = (1<<TOIE0);

    /* Task init */
    initKeyboard();

    xprintf("\nAdNW : %s", FW_VERSION);
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
    bool ConfigSuccess = true;

    ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);
    ConfigSuccess &= HID_Device_ConfigureEndpoints(&Mouse_HID_Interface);
#ifdef DEBUG_OUTPUT
    ConfigSuccess &= HID_Device_ConfigureEndpoints(&DBG_HID_Interface);
#endif
#ifdef EXTRA
    ConfigSuccess &= HID_Device_ConfigureEndpoints(&Extra_HID_Interface);
#endif

    USB_Device_EnableSOFEvents();

    LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Unhandled Control Request event. */
void EVENT_USB_Device_ControlRequest(void)
{
    HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
#ifdef DEBUG_OUTPUT
    HID_Device_ProcessControlRequest(&DBG_HID_Interface);
#endif
#ifdef EXTRA
    HID_Device_ProcessControlRequest(&Extra_HID_Interface);
#endif
    HID_Device_ProcessControlRequest(&Mouse_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
    HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
#ifdef DEBUG_OUTPUT
    HID_Device_MillisecondElapsed(&DBG_HID_Interface);
    Console_Task();
#endif
    HID_Device_MillisecondElapsed(&Mouse_HID_Interface);
#ifdef EXTRA
    HID_Device_MillisecondElapsed(&Extra_HID_Interface);
#endif
}


void EVENT_USB_Device_Suspend()
{
}

void EVENT_USB_Device_WakeUp()
{
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
#ifdef EXTRA
    else if (HIDInterfaceInfo == &Extra_HID_Interface) {
        USB_ExtraReport_Data_t* ExtraReport = (USB_ExtraReport_Data_t*)ReportData;
        *ReportSize = sizeof(USB_ExtraReport_Data_t);
        getExtraReport(ExtraReport);
        return false;
    }
#endif

    else if (HIDInterfaceInfo == &Mouse_HID_Interface) {
        USB_WheelMouseReport_Data_t* MouseReport = (USB_WheelMouseReport_Data_t*)ReportData;
        *ReportSize = sizeof(USB_WheelMouseReport_Data_t);

        uint8_t ret = 0;
#ifdef PS2MOUSE
        ret = getMouseReport(MouseReport);
#endif
        if(ret==0)     // mouse through key emulation only if PS/2 mouse report is "empty"
            ret = getMouseKeyReport(MouseReport);

        return true;   // force sending so mouse stops when empty
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
