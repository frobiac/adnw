#!/bin/bash
#
# Get hid_listen from
# http://www.pjrc.com/teensy/hid_listen_1.01.zip
# and allow user to run via
# username ALL=(ALL) NOPASSWD: /usr/sbin/hid_listen
#
# teensy_loader_cli needs udev rules for user access:
#   www.pjrc.com/teensy/49-teensy.rules


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



mv $HEX.hex $HEX.hex.old 2>/dev/null

rm log log.err 2>/dev/null

if [ ! -z $1 ]; then
	echo "*** make clean"
	make clean >> log
fi
echo "*** make " &&
make >> log 2>log.err || { tail -n 10 log.err; echo "*** BUILD FAILED, see log.err." ; exit 2; }

KB=$(grep "KB_HW defined" log | sed "s/^.*defined as //")

if [ -f $HEX.hex ]; then
  echo "*** Now flashing $KB ($MCU) with $HEX.hex ..." &
  teensy_loader_cli -mmcu=$MCU -w -v $HEX.hex &&
  sudo -S  /usr/sbin/hid_listen
else
  echo "$HEX not found, cannot flash."
fi
