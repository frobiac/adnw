#!/bin/bash
#
# call with extra arg to test ph verification
#   '-DPH_EXPECTED_4_DIGITS_FOR_A=4616'
#

rm ./xor

FW_VERSION=$(git describe --tags --always)-$(git log --pretty=format:%cd --date=short -n1)-$(git describe --tags --always --all | sed s:heads/::| tr " " "_")

# using -O0 fails, need to check
FLAGS="-g -O -W -std=c99 -DFW_VERSION=\"${FW_VERSION}\""
CC=gcc

mkdir .build 2>/dev/null
(
  rm xor
  cd .build

  $CC $FLAGS -c ../../helpers.c &&
  $CC $FLAGS -c ../xor.c  &&
  $CC $FLAGS helpers.o xor.o -o ../xor
)
