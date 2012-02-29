#!/bin/bash

MCU=atmega32u4
echo "*** make clean" 
make clean > log &&
echo "*** make " &&
make >> log &&
echo "*** Now flashing $MCU..." &&
#teensy_loader_cli -mmcu=at90usb1286 -w -v  build/firmware.hex &&
teensy_loader_cli -mmcu=$MCU -w -v  firmware.hex &&

sudo -S  /usr/sbin/hid_listen
