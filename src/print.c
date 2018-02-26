/* Copyright 2012,2013 Jun Wako <wakojun@gmail.com> */
/* Very basic print functions, intended to be used with usb_debug_only.c
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "LUFA/LUFA/Drivers/USB/USB.h"
#include <stdint.h>
#include "print.h"

#ifdef DEBUG_OUTPUT

static bool console_flush;
int8_t sendchar(uint8_t c);

void InitDebugPrint()
{
    xdev_out(sendchar);
}

/*******************************************************************************
 * Console
 ******************************************************************************/
void Console_Task(void)
{
    static uint8_t count;
    if (++count % 50)
        return;
    count = 0;

    if (!console_flush)
        return;

    //Console_Task();
    console_flush = false;

    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    uint8_t ep = Endpoint_GetCurrentEndpoint();

    /* IN packet */
    Endpoint_SelectEndpoint(DBG_IN_EPNUM);
    if (!Endpoint_IsEnabled() || !Endpoint_IsConfigured()) {
        Endpoint_SelectEndpoint(ep);
        return;
    }

    // fill empty bank
    while (Endpoint_IsReadWriteAllowed())
        Endpoint_Write_8(0);

    // flash senchar packet
    if (Endpoint_IsINReady()) {
        Endpoint_ClearIN();
    }

    Endpoint_SelectEndpoint(ep);
}

#define CONSOLE_FLUSH_SET(b)   do { \
        uint8_t sreg = SREG; cli(); console_flush = b; SREG = sreg; \
    } while (0)


/*******************************************************************************
 * sendchar
 ******************************************************************************/
#define SEND_TIMEOUT 5
int8_t sendchar(uint8_t c)
{
    // Not wait once timeouted.
    // Because sendchar() is called so many times, waiting each call causes big lag.
    static bool timeouted = false;

    // prevents Console_Task() from running during sendchar() runs.
    // or char will be lost. These two function is mutually exclusive.
    CONSOLE_FLUSH_SET(false);

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return -1;

    uint8_t ep = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(DBG_IN_EPNUM);
    if (!Endpoint_IsEnabled() || !Endpoint_IsConfigured()) {
        goto ERROR_EXIT;
    }

    if (timeouted && !Endpoint_IsReadWriteAllowed()) {
        goto ERROR_EXIT;
    }

    timeouted = false;

    uint8_t timeout = SEND_TIMEOUT;
    while (!Endpoint_IsReadWriteAllowed()) {
        if (USB_DeviceState != DEVICE_STATE_Configured) {
            goto ERROR_EXIT;
        }
        if (Endpoint_IsStalled()) {
            goto ERROR_EXIT;
        }
        if (!(timeout--)) {
            timeouted = true;
            goto ERROR_EXIT;
        }
        _delay_ms(1);
    }

    Endpoint_Write_8(c);

    // send when bank is full
    if (!Endpoint_IsReadWriteAllowed()) {
        while (!(Endpoint_IsINReady()));
        Endpoint_ClearIN();
    } else {
        CONSOLE_FLUSH_SET(true);
    }

    Endpoint_SelectEndpoint(ep);
    return 0;
ERROR_EXIT:
    Endpoint_SelectEndpoint(ep);
    return -1;
}

uint8_t DBG__get_report(USB_DBGReport_Data_t *report_data)
{
    /// @todo Remove DBGReport altogether if no debug
    /*
    if (!stdout_is_empty()) {
        char ch;
        int index = 0;
        while(index < DBG_EPSIZE && (ch = stdout_popchar())) {
            report_data->data[index++] = ch;
        }
    }
    */

    return sizeof(USB_DBGReport_Data_t);
}
#endif
