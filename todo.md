**UNSORTED TODO LIST**
==================

LUFA 
----
* test existance in makefile 


Documentation
-------------
* license check on older files (PS2,hhkb)
* code
* pinout for different controllers
* case files and pictures


Trackpoint
----------
* initialization not really thought out
* document most important registers ( see trackpoint.[ch] from kernel)
* configuration editable at runtime + eeprom store
* tartup even if TP is disabled


Refactor
--------
* english / german host layout
 

Layout
------
* BU-TECK better?
* implement QWERTY for adopters


Git
---
* hooks (beautify, sanitize)
* document workflow


Macro
-----
* configurable (saving to eeprom) while running or separate configtool only flashing eeprom content?
		
		
Commandmode
-----------
* debug mode to configure features while running (PinkyDrop, ThumbShift)


BUGS
----
* Ctrl stuck
* revert multimodekey


Coding style
------------ 
* astyle --style=linux --convert-tabs --lineend=linux --indent=spaces=4  -o -O -K src/\*.[ch]


DONE
----
* move mod4 to shifted mod3 Not very efficient, double usage of modifiers with standard keys is better


Ideas
-----
- copy paste on mouse middle btn on Win possible?
- prntscreen...
- debug output of layout
- openmoko touchpad?
- multiple layouts -> fixed based layer toggle


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
