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

  Mouse desciptors added by frobiac, 2010

  --------------------------------------------------------------------
*/

/** \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"
#include "Board/Identifiers.h"


/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 *
 *  This descriptor describes the mouse HID interface's report structure.
 */

#ifdef OLD_MOUSE
USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport[] =

{
	0x05, 0x01,          /* Usage Page (Generic Desktop)             */
	0x09, 0x02,          /* Usage (Mouse)                            */
	0xA1, 0x01,          /* Collection (Application)                 */
	0x09, 0x01,          /*   Usage (Pointer)                        */
	0xA1, 0x00,          /*   Collection (Application)               */
	0x95, 0x03,          /*     Report Count (3)                     */
	0x75, 0x01,          /*     Report Size (1)                      */
	0x05, 0x09,          /*     Usage Page (Button)                  */
	0x19, 0x01,          /*     Usage Minimum (Button 1)             */
	0x29, 0x03,          /*     Usage Maximum (Button 3)             */
	0x15, 0x00,          /*     Logical Minimum (0)                  */
	0x25, 0x01,          /*     Logical Maximum (1)                  */
	0x81, 0x02,          /*     Input (Data, Variable, Absolute)     */
	0x95, 0x01,          /*     Report Count (1)                     */
	0x75, 0x05,          /*     Report Size (5)                      */
	0x81, 0x01,          /*     Input (Constant)                     */
	0x75, 0x08,          /*     Report Size (8)                      */
	// 0x95, 0x02,          /*     Report Count (2)             */
	0x95, 0x04,          /*     Report Count (2 + WHEELs)             */
	0x05, 0x01,          /*     Usage Page (Generic Desktop Control) */
	0x09, 0x30,          /*     Usage X                              */
	0x09, 0x31,          /*     Usage Y                              */
	0x09, 0x38,		   // Vertical wheel
	0x05, 0x0c,        // Horizontal Wheel  USAGE_PAGE (Consumer Devices)
	0x0a, 0x38, 0x02,  //         USAGE (AC Pan)
	0x15, 0x81,          /*     Logical Minimum (-127)               */
	0x25, 0x7F,          /*     Logical Maximum (127)                */
	0x81, 0x06,          /*     Input (Data, Variable, Relative)     */
	0xC0,                /*   End Collection                         */
	0xC0                 /* End Collection                           */
};

#endif
#ifdef NEW_MOUSE

//
// Wheel Mouse - simplified version - 5 button, vertical and horizontal wheel
//
// Input report - 5 bytes
//
//     Byte | D7      D6      D5      D4      D3      D2      D1      D0
//    ------+---------------------------------------------------------------------
//      0   |  0       0       0    Forward  Back    Middle  Right   Left (Buttons)
//      1   |                             X
//      2   |                             Y
//      3   |                       Vertical Wheel
//      4   |                    Horizontal (Tilt) Wheel
//
// Feature report - 1 byte
//
//     Byte | D7      D6      D5      D4   |  D3      D2  |   D1      D0
//    ------+------------------------------+--------------+----------------
//      0   |  0       0       0       0   |  Horizontal  |    Vertical
//                                             (Resolution multiplier)
//
// Reference
//    Wheel.docx in "Enhanced Wheel Support in Windows Vista" on MS WHDC
//    http://www.microsoft.com/whdc/device/input/wheel.mspx
//

USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport[] =
        //const BYTE HID_ReportDescriptor[] = {
{
    0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,        // USAGE (Mouse)
    0xa1, 0x01,        // COLLECTION (Application)
    0x09, 0x02,        //   USAGE (Mouse)
    0xa1, 0x02,        //   COLLECTION (Logical)
    0x09, 0x01,        //     USAGE (Pointer)
    0xa1, 0x00,        //     COLLECTION (Physical)
                       // ------------------------------  Buttons
    0x05, 0x09,        //       USAGE_PAGE (Button)
    0x19, 0x01,        //       USAGE_MINIMUM (Button 1)
    0x29, 0x05,        //       USAGE_MAXIMUM (Button 5)
    0x15, 0x00,        //       LOGICAL_MINIMUM (0)
    0x25, 0x01,        //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //       REPORT_SIZE (1)
    0x95, 0x05,        //       REPORT_COUNT (5)
    0x81, 0x02,        //       INPUT (Data,Var,Abs)
                       // ------------------------------  Padding
    0x75, 0x03,        //       REPORT_SIZE (3)
    0x95, 0x01,        //       REPORT_COUNT (1)
    0x81, 0x03,        //       INPUT (Cnst,Var,Abs)
                       // ------------------------------  X,Y position
    0x05, 0x01,        //       USAGE_PAGE (Generic Desktop)
    0x09, 0x30,        //       USAGE (X)
    0x09, 0x31,        //       USAGE (Y)
    0x15, 0x81,        //       LOGICAL_MINIMUM (-127)
    0x25, 0x7f,        //       LOGICAL_MAXIMUM (127)
    0x75, 0x08,        //       REPORT_SIZE (8)
    0x95, 0x02,        //       REPORT_COUNT (2)
    0x81, 0x06,        //       INPUT (Data,Var,Rel)
    0xa1, 0x02,        //       COLLECTION (Logical)
                       // ------------------------------  Vertical wheel res multiplier
    0x09, 0x48,        //         USAGE (Resolution Multiplier)
    0x15, 0x00,        //         LOGICAL_MINIMUM (0)
    0x25, 0x01,        //         LOGICAL_MAXIMUM (1)
    0x35, 0x01,        //         PHYSICAL_MINIMUM (1)
    0x45, 0x04,        //         PHYSICAL_MAXIMUM (4)
    0x75, 0x02,        //         REPORT_SIZE (2)
    0x95, 0x01,        //         REPORT_COUNT (1)
    0xa4,              //         PUSH
    0xb1, 0x02,        //         FEATURE (Data,Var,Abs)
                       // ------------------------------  Vertical wheel
    0x09, 0x38,        //         USAGE (Wheel)
    0x15, 0x81,        //         LOGICAL_MINIMUM (-127)
    0x25, 0x7f,        //         LOGICAL_MAXIMUM (127)
    0x35, 0x00,        //         PHYSICAL_MINIMUM (0)        - reset physical
    0x45, 0x00,        //         PHYSICAL_MAXIMUM (0)
    0x75, 0x08,        //         REPORT_SIZE (8)
    0x81, 0x06,        //         INPUT (Data,Var,Rel)
    0xc0,              //       END_COLLECTION
    0xa1, 0x02,        //       COLLECTION (Logical)
                       // ------------------------------  Horizontal wheel res multiplier
    0x09, 0x48,        //         USAGE (Resolution Multiplier)
    0xb4,              //         POP
    0xb1, 0x02,        //         FEATURE (Data,Var,Abs)
                       // ------------------------------  Padding for Feature report
    0x35, 0x00,        //         PHYSICAL_MINIMUM (0)        - reset physical
    0x45, 0x00,        //         PHYSICAL_MAXIMUM (0)
    0x75, 0x04,        //         REPORT_SIZE (4)
    0xb1, 0x03,        //         FEATURE (Cnst,Var,Abs)
                       // ------------------------------  Horizontal wheel
    0x05, 0x0c,        //         USAGE_PAGE (Consumer Devices)
    0x0a, 0x38, 0x02,  //         USAGE (AC Pan)
    0x15, 0x81,        //         LOGICAL_MINIMUM (-127)
    0x25, 0x7f,        //         LOGICAL_MAXIMUM (127)
    0x75, 0x08,        //         REPORT_SIZE (8)
    0x81, 0x06,        //         INPUT (Data,Var,Rel)
    0xc0,              //       END_COLLECTION
    0xc0,              //     END_COLLECTION
    0xc0,              //   END_COLLECTION
    0xc0               // END_COLLECTION
};
#endif


/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 */
USB_Descriptor_HIDReport_Datatype_t PROGMEM KeyboardReport[] =
{
	0x05, 0x01,          /* Usage Page (Generic Desktop)                    */
	0x09, 0x06,          /* Usage (Keyboard)                                */
	0xa1, 0x01,          /* Collection (Application)                        */

	0x75, 0x01,          /*   Report Size (1)                               */
	0x95, 0x08,          /*   Report Count (8)                              */
	0x05, 0x07,          /*   Usage Page (Keyboard)                         */
	0x19, 0xe0,          /*   Usage Minimum (Keyboard LeftControl)          */
	0x29, 0xe7,          /*   Usage Maximum (Keyboard Right GUI)            */
	0x15, 0x00,          /*   Logical Minimum (0)                           */
	0x25, 0x01,          /*   Logical Maximum (1)                           */
	0x81, 0x02,          /*   Input (Data, Variable, Absolute)              */

	0x95, 0x01,          /*   Report Count (1)                              */
	0x75, 0x08,          /*   Report Size (8)                               */
	0x81, 0x03,          /*   Input (Const, Variable, Absolute)             */

	0x95, 0x05,          /*   Report Count (5)                              */
	0x75, 0x01,          /*   Report Size (1)                               */
	0x05, 0x08,          /*   Usage Page (LEDs)                             */
	0x19, 0x01,          /*   Usage Minimum (Num Lock)                      */
	0x29, 0x05,          /*   Usage Maximum (Kana)                          */
	0x91, 0x02,          /*   Output (Data, Variable, Absolute)             */

	0x95, 0x01,          /*   Report Count (1)                              */
	0x75, 0x03,          /*   Report Size (3)                               */
	0x91, 0x03,          /*   Output (Const, Variable, Absolute)            */

	0x95, 0x06,          /*   Report Count (6)                              */
	0x75, 0x08,          /*   Report Size (8)                               */
	0x15, 0x00,          /*   Logical Minimum (0)                           */
	0x25, 0x65,          /*   Logical Maximum (101)                         */
	0x05, 0x07,          /*   Usage Page (Keyboard)                         */
	0x19, 0x00,          /*   Usage Minimum (Reserved (no event indicated)) */
	0x29, 0x65,          /*   Usage Maximum (Keyboard Application)          */
	0x81, 0x00,          /*   Input (Data, Array, Absolute)                 */

	0xc0                 /* End Collection                                  */
};

USB_Descriptor_HIDReport_Datatype_t ConsumerControlReport[] PROGMEM =
{
  0x05, 0x0C,           /* Usage Page (Consumer) */
  0x09, 0x01,           /*   Usage (Consumer Control ??? ) */
  0xA1, 0x01,           /*     Collection (Application) */

  0x85, 0x01,           /*       ReportID (1) */
  0x19, 0x00,           /*       Usage Minimum (0) */
  0x2A, 0x3C, 0x02,     /*       Usage Maximum (572) */
  0x15, 0x00,           /*       Logical Minimum (0) */
  0x26, 0x3C, 0x02,     /*       Logical Maximum (572) */
  0x95, 0x01,           /*       Report Count (1) */
  0x75, 0x10,           /*       Report Size (16) */
  0x81, 0x00,           /*       Input (Data, Array, Absolute) */

  0xC0,                 /*     End Collection (Application */
  0x05, 0x01,           /* Usage Page (Generic Desktop) */
  0x09, 0x80,           /*   Usage (System Control) */
  0xA1, 0x01,           /*     Collection (Application) */

  0x85, 0x02,           /*       ReportID (2) */
  0x19, 0x81,           /*       Usage Minimum (129) */
  0x29, 0x83,           /*       Usage Maximum (131) */
  0x25, 0x01,           /*       Logical Maximum (1) */
  0x75, 0x01,           /*       Report Size (1) */
  0x95, 0x03,           /*       Report Count (3) */
  0x81, 0x02,           /*       Input (Data, Variable, Absolute, No Wrap,
                                        Linear, Preferred State, No Null
                                        Position, Bitfield) */

  0x95, 0x05,           /*       Report Count (5) */
  0x81, 0x01,           /*       Input (Constant, Array, Absolute) */

  0xC0                  /*     End Collection (Application) */
};


USB_Descriptor_HIDReport_Datatype_t DBGReport[] PROGMEM =
{
  0x06, 0x31, 0xFF,    // Usage Page 0xFF31 (vendor defined)
  0x09, 0x74,          // Usage 0x74
  0xA1, 0x53,          // Collection 0x53
  0x75, 0x08,          // report size = 8 bits
  0x15, 0x00,          // logical minimum = 0
  0x26, 0xFF, 0x00,    // logical maximum = 255
  0x95, DBG_EPSIZE,    // report count
  0x09, 0x75,          // usage
  0x81, 0x02,          // Input (array)
  0xC0                 // end collection
};

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(01.10),
	.Class                  = 0x00,
	.SubClass               = 0x00,
	.Protocol               = 0x00,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = VENDOR_ID,
	.ProductID              = PRODUCT_ID,
	.ReleaseNumber          = RELEASE_NUMBER,

	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
            .TotalInterfaces        = 4,  // +1 due to mouse

			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_REMOTEWAKEUP),

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.HID_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = 0x00,
			.AlternateSetting       = 0x00,

			.TotalEndpoints         = 1,

			.Class                  = 0x03,
			.SubClass               = 0x01,
			.Protocol               = HID_BOOT_KEYBOARD_PROTOCOL,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.HID_KeyboardHID =
		{
			.Header                 = {.Size = sizeof(USB_HID_Descriptor_t), .Type = DTYPE_HID},

			.HIDSpec                = VERSION_BCD(01.11),
			.CountryCode            = 0x00,
			.TotalReportDescriptors = 1,
			.HIDReportType          = DTYPE_Report,
			.HIDReportLength        = sizeof(KeyboardReport)
		},

	.HID_ReportINEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = (ENDPOINT_DESCRIPTOR_DIR_IN | KEYBOARD_EPNUM),
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = KEYBOARD_EPSIZE,
			.PollingIntervalMS      = 0x0A
		},
    .HIDDBG_Interface =
      {
        .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

        .InterfaceNumber        = 0x01,
        .AlternateSetting       = 0x00,

        .TotalEndpoints         = 1,

        .Class                  = 0x03,
        .SubClass               = 0x00,
        .Protocol               = 0x00,

        .InterfaceStrIndex      = NO_DESCRIPTOR
      },

    .HIDDBG_HID =
      {
        .Header                 = {.Size = sizeof(USB_HID_Descriptor_t), .Type = DTYPE_HID},

        .HIDSpec                = VERSION_BCD(01.11),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = DTYPE_Report,
        .HIDReportLength        = sizeof(DBGReport)
      },

    .HIDDBG_ReportINEndpoint =
      {
        .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

        .EndpointAddress        = (ENDPOINT_DESCRIPTOR_DIR_IN | DBG_EPNUM),
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = DBG_EPSIZE,
        .PollingIntervalMS      = 0x0A
      },
    .Programming_Interface =
      {
        .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

        .InterfaceNumber        = 0x02,
        .AlternateSetting       = 0x00,

        .TotalEndpoints         = 1,

        .Class                  = 0xFF, /* Vendor specific */
        .SubClass               = 0x00,
        .Protocol               = 0x00,

        .InterfaceStrIndex      = NO_DESCRIPTOR
      },

    .Programming_ReportINEndpoint =
      {
        .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

        .EndpointAddress        = (ENDPOINT_DESCRIPTOR_DIR_IN | PRG_EPNUM),
        .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = PRG_EPSIZE,
        .PollingIntervalMS      = 0x00 /* ignored for full-speed bulk transfers */
      },

.HID_MouseInterface =
{
		.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

		.InterfaceNumber        = 0x03,
		.AlternateSetting       = 0x00,

		.TotalEndpoints         = 1,

		.Class                  = 0x03,
		.SubClass               = 0x01,
		.Protocol               = HID_NON_BOOT_PROTOCOL, /*HID_BOOT_MOUSE_PROTOCOL,*/

		.InterfaceStrIndex      = NO_DESCRIPTOR
							  },

.HID_MouseHID =
{
		.Header                 = {.Size = sizeof(USB_HID_Descriptor_t), .Type = DTYPE_HID},

		.HIDSpec                = VERSION_BCD(01.11),
		.CountryCode            = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType          = DTYPE_Report,
		.HIDReportLength        = sizeof(MouseReport)
							  },

.HID_MouseReportINEndpoint =
{
		.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
		.EndpointAddress        = (ENDPOINT_DESCRIPTOR_DIR_IN | MOUSE_IN_EPNUM),
		.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize           = HID_EPSIZE,
		.PollingIntervalMS      = 0x0A
},

};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
USB_Descriptor_String_t PROGMEM LanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

	.UnicodeString          = {LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	.Header                 = {.Size = MANUFACTURER_NAME_LEN, .Type = DTYPE_String},

	.UnicodeString          = MANUFACTURER_NAME
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = PRODUCT_NAME_LEN, .Type = DTYPE_String},

	.UnicodeString          = PRODUCT_NAME
};

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	void*    Address = NULL;
	uint16_t Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = (void*)&DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = (void*)&ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = (void*)&LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = (void*)&ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = (void*)&ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
    case DTYPE_HID:
      switch (wIndex)
      {
        case 0:
          Address = (void*)&ConfigurationDescriptor.HID_KeyboardHID;
          Size    = sizeof(USB_HID_Descriptor_t);
          break;
        case 1:
          Address = (void*)&ConfigurationDescriptor.HIDDBG_HID;
          Size    = sizeof(USB_HID_Descriptor_t);
          break;
		case 3:
		  Address = (void*)&ConfigurationDescriptor.HID_MouseHID;
		  Size    = sizeof(USB_HID_Descriptor_t);
		  break;
      }
			break;
    case DTYPE_Report:
      switch (wIndex)
      {
        case 0:
          Address = (void*)&KeyboardReport;
          Size    = sizeof(KeyboardReport);
          break;
        case 1:
          Address = (void*)&DBGReport;
          Size    = sizeof(DBGReport);
          break;
        case 3:
          Address = (void*)&MouseReport;
          Size    = sizeof(MouseReport);
          break;
      }
			break;
	}

	*DescriptorAddress = Address;
	return Size;
}
