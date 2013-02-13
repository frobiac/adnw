**UNSORTED TODO LIST**
==================

LUFA
----
* test existance in makefile
* inquire about upstream horizontal scroll support

Documentation
-------------
* license check on older files (hhkb)
* code
* pinout for different controllers
* case files and pictures


Trackpoint
----------
* initialization not really thought out
* document most important registers ( see trackpoint.[ch] from kernel)
* configuration editable at runtime + eeprom store
* startup even if TP is disabled

HW
--
* PS/2 pull-ups needed
* LED signalling

Refactor
--------
* english / german host layout
* maybe use KEYMAP macro like tmk?
* check licenses

Layout
------
* BU-TECK better?
* implement QWERTY for adopters
* usage page for system keys
* copy/paste

Git
---
* hooks (beautify, sanitize)
* document workflow


Macro
-----
* configurable (saving to eeprom) while running or separate configtool only flashing eeprom content


Commandmode
-----------
* cleanup layer switching between different layouts
* read data like tp settings
* hierarchical command structure

BUGS
----
* Ctrl stuck
* revert multimodekey


Coding style
------------
* astyle --style=linux --convert-tabs --lineend=linux --indent=spaces=4  -o -O -S src/\*.[ch]



Ideas
-----
- copy paste on mouse middle btn on Win possible?
- prntscreen...
- openmoko touchpad?
- analog input for 2-wire matrix-scan

Problems with layout
--------------------
- Ctrl-Tab needs separate Tab
- Ctrl-Alt-Fx
- Ctrl-Alt-Del
- ctrl-alt       vmware
- (ctrl)shift + direction
- ctrl-mouseclick


Thumbkey double usage
---------------------
- outmost column only on hypernano
- make lower center keys mousebuttons for TP
- ESC to M3/ or mouse to pinky
- Macro to pinky
                               _____         _____
                              |MACRO|       |MOUSE|
                              |     |       |     |
                              |     |       | ESC |
 _____________________________|_____|       |_____|______________________________
|     |     | ALT |CTRL |     | WIN |       | M3  | M1  | M2  |AltGr|     |      |
|     |     |     |     |     |     |       |     |     |     |     |     |      |
|     |     |     |     |SPACE| TAB |       |     | <-| |ENTER|     |     |      |
 -----------------------------------         ------------------------------------

Unsorted
--------
SecondaryUsage repeat (backspace)
Testcases/mode
Use center keys as mouse btns

maltron branch(es) cleanup
--------------------------
* credit K.W. in future merge
