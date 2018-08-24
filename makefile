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
#BOARD        = MICRO # for LEONARDO ADAFxxx

F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = adnw
SRCDIR       = ./src

# -----------------------------------------------------------
# Keyboard selection below: Override from environment variable
# -----------------------------------------------------------
KB_HW_SUPPORTED = BLUECUBE HYPERNANO REDTILT HYPERMICRO BLACKFLAT BLACKBOWL
KB_HW		 ?= BLACKFLAT

KB_DBG ?= 1
KB_EXT ?= 1


##################################################################
#
# Should not need to change anything below ...
#
##################################################################

# List C source files here. (C dependencies are automatically generated.)
SRC =  $(LUFA_SRC_USB_DEVICE)    \
	$(LUFA_PATH)/Drivers/USB/Class/Device/HIDClassDevice.c \
	$(SRCDIR)/Keyboard.c         \
	$(SRCDIR)/Descriptors.c      \
	$(SRCDIR)/keyboard_class.c   \
	$(SRCDIR)/keymap.c           \
	$(SRCDIR)/matrix.c           \
	$(SRCDIR)/macro.c            \
	$(SRCDIR)/command.c          \
	$(SRCDIR)/ascii2hid.c        \
	$(SRCDIR)/mousekey.c         \
	$(SRCDIR)/jump_bootloader.c  \
	$(SRCDIR)/external/i2cmaster/twimaster.c \
	$(SRCDIR)/global_config.c      \
	$(SRCDIR)/mem-check.c 		   \
	$(SRCDIR)/passhash/b64.c   \
	$(SRCDIR)/helpers.c 		   \

LUFA_PATH    = LUFA/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
CC_FLAGS    += -fdata-sections
CC_FLAGS    += -Werror
LD_FLAGS     =

CC_FLAGS += -D$(KB_HW)

ifeq ($(KB_DBG), 1)
CC_FLAGS += -DDEBUG_OUTPUT
endif

ifeq ($(KB_EXT), 1)
CC_FLAGS += -DEXTRA
SRC += $(SRCDIR)/extra.c
endif

ifneq (,$(findstring REDTILT,$(CC_FLAGS)))
CC_FLAGS    += -DPS2MOUSE
PS2_USE_BUSYWAIT = yes # uses primitive reference code
endif

ifneq (,$(findstring BLACKFLAT,$(CC_FLAGS)))
CC_FLAGS    += -DPS2MOUSE
PS2_USE_BUSYWAIT = yes # uses primitive reference code
endif

ifneq (,$(findstring BLACKBOWL,$(CC_FLAGS)))
CC_FLAGS    += -DPS2MOUSE
PS2_USE_USART = yes # uses primitive reference code
endif

ifneq (,$(findstring HYPERNANO,$(CC_FLAGS)))
CC_FLAGS    += -DPINKYDROP
CC_FLAGS    += -DPS2MOUSE
PS2_USE_BUSYWAIT = yes # uses primitive reference code
endif
	
SRC += \
		$(SRCDIR)/passhash/sha1-asm.S   \
		$(SRCDIR)/passhash/hmac-sha1.c  \

SRC += $(SRCDIR)/passhash/xor.c
SRC += $(SRCDIR)/passhash/tabularecta.c
		

ifneq (,$(findstring DEBUG_OUTPUT,$(CC_FLAGS)))
	SRC += \
		   $(SRCDIR)/print.c \
		   $(SRCDIR)/xprintf.S \

endif

ifneq (,$(findstring ANALOGSTICK,$(CC_FLAGS)))
	SRC += $(SRCDIR)/analog.c
endif

ifneq (,$(findstring PS2MOUSE,$(CC_FLAGS)))
	PS2_MOUSE_ENABLE = yes # PS/2 mouse(TrackPoint) support
	# Define variant used per board above
	#PS2_USE_BUSYWAIT = yes # uses primitive reference code
	#PS2_USE_INT      = yes # uses external interrupt for falling edge of PS/2 clock pin
	#PS2_USE_USART    = yes # uses hardware USART engine for PS/2 signal receive(recomened)

	TMK_PS2_DIR = ./src/tmk_ps2
	-include $(TMK_PS2_DIR)/tmk_ps2.mk
	CC_FLAGS += $(OPT_DEFS)
	SRC += $(SRCDIR)/ps2mouse.c
	SRC += $(SRCDIR)/trackpoint.c
endif

FW_VERSION := $(shell git describe --tags --always)-$(shell git log --pretty=format:%cd --date=short -n1)-$(shell git describe --tags --always --all | sed s:heads/::| tr " " "_")
ifeq ('',$(FW_VERSION))
FW_VERSION := unknown_version-$(shell date +%Y%m%d)
endif
CC_FLAGS    += -DFW_VERSION=\"$(FW_VERSION)\"

# generate a sample uuid for passhash secret
UUID := $(shell cat /proc/sys/kernel/random/uuid)
ifndef UUID
	UUID := $(shell tr -dc 'a-zA-Z0-9' </dev/urandom | tr -d 'lIO' | head -c 26 | sed 's/./-/9; s/./-/14; s/./-/19; s/./-/24')
endif

# Default target
all: lufacheck configtest private_data_check submodule

private_data_check:
ifneq ("$(wildcard $(SRCDIR)/_private_data.h)","")
	@echo "*** Private data definition found ";
else
	@echo "*** PRIVATE DATA $(SRCDIR)/_private_data.h NOT found.";
	@echo "*** Creating default one. Please edit template to your needs (file is ignored in git)";
	@sed "s/^#define PH_PRIVATE_KEY .*/#define PH_PRIVATE_KEY \"$(UUID)\"/" $(SRCDIR)/_private_data_template.h > $(SRCDIR)/_private_data.h ;
	@chmod 600 $(SRCDIR)/_private_data.h ;
	@false;
endif


# check that LUFA is there
lufacheck:
	@if test -d LUFA/LUFA ; then \
		echo "*** LUFA found.";\
	else \
		echo -e "*** ERROR: LUFA/LUFA missing - see README for install instructions.\n***        Try to checkout LUFA source with\n***            git submodule init && git submodule update\n\n"; false;\
	fi

configtest:
# check that KB_HW is defined and valid
ifneq (,$(filter $(KB_HW), $(KB_HW_SUPPORTED)))
	@echo "*** HW  = $(KB_HW) from $(origin KB_HW)"
	@echo "*** DBG = $(KB_DBG) from $(origin KB_DBG)"
else
	$(error *** KB_HW defined as "$(KB_HW)" not valid: not in $(KB_HW_SUPPORTED))
endif


ifneq (,$(findstring DEBUG_OUTPUT,$(CC_FLAGS)))
	@echo "*** DEBUG is defined" ;
else
	@echo "*** DEBUG is NOT defined";
endif



submodule:
	@git submodule update

sizecheck:
	@echo HW=$(KB_HW) DBG=$(KB_DBG)
	@avr-size --mcu=atmega32u4 -B adnw.elf



# Include LUFA build script makefiles
# lines begin with "-" so if not found, the lufacheck above prints message
#-include $(LUFA_PATH)/Build/lufa_core.mk
-include $(LUFA_PATH)/Build/lufa_sources.mk
-include $(LUFA_PATH)/Build/lufa_build.mk
