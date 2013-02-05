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
#MCU          = at90usb1286 
ARCH         = AVR8
BOARD        = TEENSY2
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
	$(SRCDIR)/command.c          \
	$(SRCDIR)/jump_bootloader.c


LUFA_PATH    = LUFA/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
CC_FLAGS    += -DUSE_HHSTDIO -DNEW_MOUSE  -DPS2MOUSE -DMOUSE_HAS_SCROLL_WHEELS
#CC_FLAGS    += -DMACOS
#CC_FLAGS    += -DQWERTY
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
	    echo -e "\n\n\n*** ERROR: $(SRCDIR)/_private_macros.h NOT found. \n*** Please copy template and edit as wanted:\n\n    cp $(SRCDIR)/_private_macros.h.template $(SRCDIR)/_private_macros.h\n\n"; \
	    false; \
	fi

#
# DOES nothing - LUFA includes are searched before this, so no helpful msg
#
# check that LUFA is there
ifeq ($(wildcard LUFA/LUFA/Build/lufa_core.mk),)
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
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
