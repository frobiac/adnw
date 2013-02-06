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
win  alt  shft ctrl  | -  mod3 mod4  altgr
     esc  spac tab        back enter del

maltron branch(es) cleanup
--------------------------
* reduce .data usage to enable code on Teensy 2.0 (ExtraKey, Layouts...)
* secondUse (successor of MKT) investigation
* credit K.W. in future merge
* modular config for different HW: Pins, Cmd-keys, IDs
