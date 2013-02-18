**UNSORTED TODO LIST**
==================

LUFA
----
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


Coding style
------------
* astyle --style=linux --convert-tabs --lineend=linux --indent=spaces=4  -o -O -S src/\*.[ch]
* astyle -o -O -S src/\*.[ch]

Ideas
-----
- copy paste on mouse middle btn on Win possible?
- prntscreen...
- openmoko touchpad?
- analog input for 2-wire matrix-scan


Thumbkey double usage
---------------------
- outmost column only on hypernano
- make lower center keys mousebuttons for TP
                               _____         _____
                              |     |       | M3  |
                              |     |       |     |
  |- HN:only                  |     |       |     |                         |- HN only, uses pinky key
 _V___________________________|_____|       |_____|_________________________V____
|     |     | ALT |CTRL |     | WIN |       | M2  | M0  | M1  |AltGr|     |MOUSE |
|     |     |     |     |     |     |       |     |     |     |     |     |      |
|     |     |     |     |SPACE| TAB |       | ESC | <-| |ENTER|     |     |      |
 -----------------------------------         ------------------------------------

Unsorted
--------
SecondaryUsage repeat (backspace)
Testcases/mode
Pinkydrop again ?!

maltron 
-------
* credit K.W. in future merge
