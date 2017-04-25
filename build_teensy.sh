#!/bin/bash
#
# hid_listen
#   https://www.pjrc.com/teensy/hid_listen_1.01.zip
#   and allow user to run via
#   username ALL=(ALL) NOPASSWD: /usr/sbin/hid_listen
#
# teensy_loader_cli
#   https://github.com/PaulStoffregen/teensy_loader_cli
#   needs udev rules for user access:
#   https://www.pjrc.com/teensy/49-teensy.rules
#
# VMWARE config
# usb.generic.allowHID = "TRUE"
# usb.generic.allowLastHID = "TRUE"

MCU=$( grep "^MCU\s*=" makefile | sed "s/.*=\s*//")
HEX=$( grep "^TARGET\s*=" makefile | sed "s/.*=\s*//")


# test for existance of pre-commit hook, and link astyle check.
# always link latest pre-commit and make executable
HOOK=$(git rev-parse --show-toplevel)/.git/hooks/pre-commit
if [ ! -h $HOOK ]; then
	echo "pre-commit hook file found, creating backup."
    mv $HOOK ${HOOK}.orig
fi
ln -sf ../../tools/pre-commit.astyle.git_hook $HOOK
chmod +x $HOOK

OPT=""
if [ $(which usb-devices) ]; then
    CONNECTED_HW=$(usb-devices | grep -A2 "Vendor=1d50 ProdID=6033" | grep Product | sed "s/.*[= ]//" |tr "a-z" "A-Z")

    if [ "x$CONNECTED_HW" = "x" -o $(echo "${CONNECTED_HW}" | wc -l ) -ne 1 ]; then
        echo "Could not detect connected hardware..."
    else
        echo "Detected connected $CONNECTED_HW hardware"
        OPT="KB_HW=$CONNECTED_HW "
    fi
else
    echo "usb-devices binary not found in PATH."
fi


mv $HEX.hex $HEX.hex.old 2>/dev/null

rm log log.err 2>/dev/null

if [ ! -z $1 ]; then
	echo "*** make clean"
	make clean >> log
fi

# @TODO : remove potentially secret arguments from log
echo "*** make $@ $OPT" &&
make $@ $OPT >> log 2>log.err || { tail -n 10 log.err; echo "*** BUILD FAILED, see log.err." ; exit 2; }

KB=$(grep "^\*\*\* HW\s*=" log | sed "s/^.* = \([A-Z]*\) .*/\1/")

if [ -f $HEX.hex ]; then
  if [ $(lspci | grep -i vmware -c ) -gt 0 ]; then
        echo "Not flashing in VM"
        cp $HEX.hex adnw_vm_shared.hex
  else
      echo "*** Now flashing $KB ($MCU) with $HEX.hex ..."
      teensy_loader_cli -mmcu=$MCU -w -v $HEX.hex
      sudo -S  /usr/sbin/hid_listen
  fi
else
  echo "$HEX not found, cannot flash."
fi
