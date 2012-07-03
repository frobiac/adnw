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

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = teensy
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = adnw
SRCDIR       = ./src

# List C source files here. (C dependencies are automatically generated.)
SRC =   $(LUFA_SRC_USB)          \
	$(LUFA_SRC_USBCLASS)         \
	$(SRCDIR)/Keyboard.c         \
	$(SRCDIR)/dbg.c              \
	$(SRCDIR)/Descriptors.c      \
	$(SRCDIR)/keyboard_class.c   \
	$(SRCDIR)/hhstdio.c          \
	$(SRCDIR)/keymap.c           \
	$(SRCDIR)/analog.c           \
	$(SRCDIR)/macro.c            \
	$(SRCDIR)/ps2mouse.c         \
	$(SRCDIR)/trackpoint.c       \
	$(SRCDIR)/jump_bootloader.c


LUFA_PATH    = LUFA/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
CC_FLAGS    += -DUSE_HHSTDIO -DNEW_MOUSE  -DPS2MOUSE -DMOUSE_HAS_SCROLL_WHEELS
LD_FLAGS     =

# Default target
all: version macrocheck


# create some version information from git
version:
	@$(SRCDIR)/version.sh > $(SRCDIR)/version.h


# test macro existance
macrocheck:
	@if test -f $(SRCDIR)/_private_macros.h; then \
	    echo "*** Macro definition found ";  \
	else \
	    echo -e "\n\n\n*** ERROR: $(SRCDIR)/_private_macros.h NOT found. \n*** Consider copying template\n\n"; \
	    false; \
	fi

#
# DOES nothing - LUFA includes are searched before this, so no helpful msg
#
# check that LUFA is there
ifeq ($(wildcard LUFA/LUFA/Build/lufa.core.in),)
	@echo -e "*** ERROR: LUFA/LUFA missing - see README for install instructions"; false;
else
	@echo "*** LUFA found.";
endif


# check whether scrollwheel support is requested and complete
ifneq (,$(findstring MOUSE_HAS_SCROLL_WHEELS,$(CCFLAGS)))
	@echo "*** SCROLLWHEEL not defined"
else
	@echo "*** SCROLLWHEEL defined"; \
	if [ `grep -c "Vertical" LUFA/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h` -eq 1 ] ; then \
	    echo "*** and LUFA seems patched, ok"; \
	else \
	    echo "*** ERROR: but LUFA not patched for it - run "; \
	    echo -e "*** patch -Np1 -i LUFA-scrollwheel.patch \n*** to fix \n" ; \
	    false; \
	fi
endif

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa.core.in
include $(LUFA_PATH)/Build/lufa.sources.in
include $(LUFA_PATH)/Build/lufa.build.in
include $(LUFA_PATH)/Build/lufa.cppcheck.in
include $(LUFA_PATH)/Build/lufa.doxygen.in
include $(LUFA_PATH)/Build/lufa.dfu.in
include $(LUFA_PATH)/Build/lufa.hid.in
include $(LUFA_PATH)/Build/lufa.avrdude.in
include $(LUFA_PATH)/Build/lufa.atprogram.in
