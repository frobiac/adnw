PASSHASH
========

Private key is limited to 26 chars
Master password is limited to XXX chars

Private Key
-----------
Stored permanently. Options:
- #define in extra header       : Unsafe, must not be checked into git
- #define on make command line  : Better, keep out of history (ignorespace) and logs
    1) export HISTCONTROL=$HISTCONTROL:ignorespace and call " make ..." with leading space
    2) history -d $(history 1)
    3) clear log
- environment variable          : same as make cli.
- stored in eeprom              : Best, but has to be entered at least once.


Master Password
---------------
Stored until poweroff or manual clear.
Has to be entered on first-time passhash usage.
