/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


#include <stdint.h>
#include "hid_usage.h"
#include "extra.h"
#include "print.h"

/** Suspend with System Control 0x82 = 130 works under linux.
 *  WakeUp with any key.
 *
 */
uint8_t getExtraReport(USB_ExtraReport_Data_t *ExtraReport)
{
//    if(g_extra_data==0)
//        return 0;

    ExtraReport->report_id = REPORT_ID_CONSUMER;

    if(g_extra_data >= 0x81 || g_extra_data <= 0xB7 )
        ExtraReport->report_id = REPORT_ID_SYSTEM;

    ExtraReport->usage     = g_extra_data;

    g_extra_data = 0;

    return sizeof(USB_ExtraReport_Data_t);
}

/*
// http://www.freebsddiary.org/APC/usb_hid_usages.php
// 1 Generic Desktop
// =================
    ...
    0x80    System Control
    0x81    System Power Down
    0x82    System Sleep
    0x83    System Wake Up
    0x84    System Context Menu
    0x85    System Main Menu
    0x86    System App Menu
    0x87    System Menu Help
    0x88    System Menu Exit
    0x89    System Menu Select
    0x8A    System Menu Right
    0x8B    System Menu Left
    0x8C    System Menu Up
    0x8D    System Menu Down
    0x90    D-pad Up
    0x91    D-pad Down
    0x92    D-pad Right
    0x93    D-pad Left


// 12 Consumer
// ===========

    0x00    Unassigned
    0x01    Consumer Control
    0x02    Numeric Key Pad
    0x03    Programmable Buttons
    0x20    +10
    0x21    +100
    0x22    AM/PM
    0x30    Power
    0x31    Reset
    0x32    Sleep
    0x33    Sleep After
    0x34    Sleep Mode
    0x35    Illumination
    0x36    Function Buttons
    0x40    Menu
    0x41    Menu  Pick
    0x42    Menu Up
    0x43    Menu Down
    0x44    Menu Left
    0x45    Menu Right
    0x46    Menu Escape
    0x47    Menu Value Increase
    0x48    Menu Value Decrease
    0x60    Data On Screen
    0x61    Closed Caption
    0x62    Closed Caption Select
    0x63    VCR/TV
    0x64    Broadcast Mode
    0x65    Snapshot
    0x66    Still
    0x80    Selection
    0x81    Assign Selection
    0x82    Mode Step
    0x83    Recall Last
    0x84    Enter Channel
    0x85    Order Movie
    0x86    Channel
    0x87    Media Selection
    0x88    Media Select Computer
    0x89    Media Select TV
    0x8A    Media Select WWW
    0x8B    Media Select DVD
    0x8C    Media Select Telephone
    0x8D    Media Select Program Guide
    0x8E    Media Select Video Phone
    0x8F    Media Select Games
    0x90    Media Select Messages
    0x91    Media Select CD
    0x92    Media Select VCR
    0x93    Media Select Tuner
    0x94    Quit
    0x95    Help
    0x96    Media Select Tape
    0x97    Media Select Cable
    0x98    Media Select Satellite
    0x99    Media Select Security
    0x9A    Media Select Home
    0x9B    Media Select Call
    0x9C    Channel Increment
    0x9D    Channel Decrement
    0x9E    Media Select SAP
    0xA0    VCR Plus
    0xA1    Once
    0xA2    Daily
    0xA3    Weekly
    0xA4    Monthly
    0xB0    Play
    0xB1    Pause
    0xB2    Record
    0xB3    Fast Forward
    0xB4    Rewind
    0xB5    Scan Next Track
    0xB6    Scan Previous Track
    0xB7    Stop
    0xB8    Eject
    0xB9    Random Play
    0xBA    Select DisC
    0xBB    Enter Disc
    0xBC    Repeat
    0xBD    Tracking
    0xBE    Track Normal
    0xBF    Slow Tracking
    0xC0    Frame Forward
    0xC1    Frame Back
    0xC2    Mark
    0xC3    Clear Mark
    0xC4    Repeat From Mark
    0xC5    Return To Mark
    0xC6    Search Mark Forward
    0xC7    Search Mark Backwards
    0xC8    Counter Reset
    0xC9    Show Counter
    0xCA    Tracking Increment
    0xCB    Tracking Decrement
    0xE0    Volume
    0xE1    Balance
    0xE2    Mute
    0xE3    Bass
    0xE4    Treble
    0xE5    Bass Boost
    0xE6    Surround Mode
    0xE7    Loudness
    0xE8    MPX
    0xE9    Volume Up
    0xEA    Volume Down
    0xF0    Speed Select
    0xF1    Playback Speed
    0xF2    Standard Play
    0xF3    Long Play
    0xF4    Extended Play
    0xF5    Slow
    0x100   Fan Enable
    0x101   Fan Speed
    0x102   Light
    0x103   Light Illumination Level
    0x104   Climate Control Enable
    0x105   Room Temperature
    0x106   Security Enable
    0x107   Fire Alarm
    0x108   Police Alarm
    0x150   Balance Right
    0x151   Balance Left
    0x152   Bass Increment
    0x153   Bass Decrement
    0x154   Treble Increment
    0x155   Treble Decrement
    0x160   Speaker System
    0x161   Channel Left
    0x162   Channel Right
    0x163   Channel Center
    0x164   Channel Front
    0x165   Channel Center Front
    0x166   Channel Side
    0x167   Channel Surround
    0x168   Channel Low Frequency Enhancement
    0x169   Channel Top
    0x16A   Channel Unknown
    0x170   Sub-channel
    0x171   Sub-channel Increment
    0x172   Sub-channel Decrement
    0x173   Alternate Audio Increment
    0x174   Alternate Audio Decrement
    0x180   Application Launch Buttons
    0x181   AL Launch Button Configuration Tool
    0x182   AL Programmable Button Configuration
    0x183   AL Consumer Control Configuration
    0x184   AL Word Processor
    0x185   AL Text Editor
    0x186   AL Spreadsheet
    0x187   AL Graphics Editor
    0x188   AL Presentation App
    0x189   AL Database App
    0x18A   AL Email Reader
    0x18B   AL Newsreader
    0x18C   AL Voicemail
    0x18D   AL Contacts/Address Book
    0x18E   AL Calendar/Schedule
    0x18F   AL Task/Project Manager
    0x190   AL Log/Journal/Timecard
    0x191   AL Checkbook/Finance
    0x192   AL Calculator
    0x193   AL A/V Capture/Playback
    0x194   AL Local Machine Browser
    0x195   AL LAN/WAN Browser
    0x196   AL Internet Browser
    0x197   AL Remote Networking/ISP Connect
    0x198   AL Network Conference
    0x199   AL Network Chat
    0x19A   AL Telephony/Dialer
    0x19B   AL Logon
    0x19C   AL Logoff
    0x19D   AL Logon/Logoff
    0x19E   AL Terminal Lock/Screensaver
    0x19F   AL Control Panel
    0x1A0   AL Command Line Processor/Run
    0x1A1   AL Process/Task Manager
    0x1A2   AL Select Tast/Application
    0x1A3   AL Next Task/Application
    0x1A4   AL Previous Task/Application
    0x1A5   AL Preemptive Halt Task/Application
    0x200   Generic GUI Application Controls
    0x201   AC New
    0x202   AC Open
    0x203   AC Close
    0x204   AC Exit
    0x205   AC Maximize
    0x206   AC Minimize
    0x207   AC Save
    0x208   AC Print
    0x209   AC Properties
    0x21A   AC Undo
    0x21B   AC Copy
    0x21C   AC Cut
    0x21D   AC Paste
    0x21E   AC Select All
    0x21F   AC Find
    0x220   AC Find and Replace
    0x221   AC Search
    0x222   AC Go To
    0x223   AC Home
    0x224   AC Back
    0x225   AC Forward
    0x226   AC Stop
    0x227   AC Refresh
    0x228   AC Previous Link
    0x229   AC Next Link
    0x22A   AC Bookmarks
    0x22B   AC History
    0x22C   AC Subscriptions
    0x22D   AC Zoom In
    0x22E   AC Zoom Out
    0x22F   AC Zoom
    0x230   AC Full Screen View
    0x231   AC Normal View
    0x232   AC View Toggle
    0x233   AC Scroll Up
    0x234   AC Scroll Down
    0x235   AC Scroll
    0x236   AC Pan Left
    0x237   AC Pan Right
    0x238   AC Pan
    0x239   AC New Window
    0x23A   AC Tile Horizontally
    0x23B   AC Tile Vertically
    0x23C   AC Format
*/

// @see tmk_core/common/report.h
//
/* Consumer Page(0x0C)
 * following are supported by Windows: http://msdn.microsoft.com/en-us/windows/hardware/gg463372.aspx
 */
#define AUDIO_MUTE              0x00E2
#define AUDIO_VOL_UP            0x00E9
#define AUDIO_VOL_DOWN          0x00EA
#define TRANSPORT_NEXT_TRACK    0x00B5
#define TRANSPORT_PREV_TRACK    0x00B6
#define TRANSPORT_STOP          0x00B7
#define TRANSPORT_STOP_EJECT    0x00CC
#define TRANSPORT_PLAY_PAUSE    0x00CD
/* application launch */
#define APPLAUNCH_CC_CONFIG     0x0183
#define APPLAUNCH_EMAIL         0x018A
#define APPLAUNCH_CALCULATOR    0x0192
#define APPLAUNCH_LOCAL_BROWSER 0x0194
/* application control */
#define APPCONTROL_SEARCH       0x0221
#define APPCONTROL_HOME         0x0223
#define APPCONTROL_BACK         0x0224
#define APPCONTROL_FORWARD      0x0225
#define APPCONTROL_STOP         0x0226
#define APPCONTROL_REFRESH      0x0227
#define APPCONTROL_BOOKMARKS    0x022A
/* supplement for Bluegiga iWRAP HID(not supported by Windows?) */
#define APPLAUNCH_LOCK          0x019E
#define TRANSPORT_RECORD        0x00B2
#define TRANSPORT_FAST_FORWARD  0x00B3
#define TRANSPORT_REWIND        0x00B4
#define TRANSPORT_EJECT         0x00B8
#define APPCONTROL_MINIMIZE     0x0206

/* Generic Desktop Page(0x01) - system power control */
#define SYSTEM_POWER_DOWN       0x0081
#define SYSTEM_SLEEP            0x0082
#define SYSTEM_WAKE_UP          0x0083


