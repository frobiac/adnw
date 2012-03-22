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

*/

/** \file
 *
 *  Header file for Keyboard.c.
 */

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

#include "Descriptors.h"

#include <LUFA/Version.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/USB/Class/HID.h>


/* Macros: */
/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
#define LEDMASK_USB_NOTREADY      LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

/* Function Prototypes: */
void SetupHardware(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_UnhandledControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, uint8_t* const ReportID,
        const uint8_t ReportType, void* ReportData, uint16_t* ReportSize);
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, const uint8_t ReportID,
        const uint8_t ReportType, const void* ReportData, const uint16_t ReportSize);




uint8_t     g_mouse_keys;
int8_t      g_mouse_modifier;
uint16_t    g_mouse_mode_sum[4];
uint8_t     g_tp_counter;
uint32_t    g_mouse_mode;

volatile uint32_t idle_count;

#endif
