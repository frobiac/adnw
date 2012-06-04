#
#             LUFA Library
#     Copyright (C) Dean Camera, 2012.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

MCU = atmega32u4
#MCU = at90usb1286
ARCH         = AVR8
BOARD        = TEENSY
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = nts
OBJDIR		 = build
SRCDIR		 = .
# List C source files here. (C dependencies are automatically generated.)
SRC =   $(LUFA_SRC_USB) \
    $(LUFA_SRC_USBCLASS) \
    $(SRCDIR)/Keyboard.c         \
    $(SRCDIR)/dbg.c              \
    $(SRCDIR)/Descriptors.c      \
    $(SRCDIR)/keyboard_class.c   \
    $(SRCDIR)/hhstdio.c          \
    $(SRCDIR)/keymap.c       \
    $(SRCDIR)/analog.c       \
    $(SRCDIR)/macro.c       \
    $(SRCDIR)/ps2mouse.c       \
    $(SRCDIR)/jump_bootloader.c 
#   $(LUFA_SRC)

LUFA_PATH    = ../LUFA/LUFA/
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =

LUFA_OPTS += -DUSE_HHSTDIO -DNEW_MOUSE  -DPS2MOUSE -DMOUSE_HAS_SCROLL_WHEELS


# Default target
all:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa.core.in
include $(LUFA_PATH)/Build/lufa.sources.in
include $(LUFA_PATH)/Build/lufa.build.in
include $(LUFA_PATH)/Build/lufa.doxygen.in
include $(LUFA_PATH)/Build/lufa.dfu.in
include $(LUFA_PATH)/Build/lufa.avrdude.in
