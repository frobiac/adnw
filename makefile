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

# Acknowledged USB ID limitations and restrictions
KB_USB_ID = ""


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
	$(SRCDIR)/external/jump_bootloader.c  \
	$(SRCDIR)/external/i2cmaster/twimaster.c \
	$(SRCDIR)/global_config.c      \
	$(SRCDIR)/external/mem-check.c 		   \
	$(SRCDIR)/b64.c   \
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

ifneq (,$(findstring frobiac,$(KB_USB_ID)))
CC_FLAGS += -DFROBIAC_USB_ID
endif
ifneq (,$(findstring LUFA,$(KB_USB_ID)))
CC_FLAGS += -DLUFA_USB_ID
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
		$(SRCDIR)/external/avr-cryptolib/sha1-asm.S   \
		$(SRCDIR)/external/avr-cryptolib/hmac-sha1.c  \

SRC += $(SRCDIR)/xor.c
SRC += $(SRCDIR)/tabularecta.c

ifneq (,$(findstring DEBUG_OUTPUT,$(CC_FLAGS)))
	SRC += \
		   $(SRCDIR)/print.c \
		   $(SRCDIR)/external/xprintf.S \

endif

ifneq (,$(findstring ANALOGSTICK,$(CC_FLAGS)))
	SRC += $(SRCDIR)/analog.c
endif

ifneq (,$(findstring PS2MOUSE,$(CC_FLAGS)))
	PS2_MOUSE_ENABLE = yes # PS/2 mouse(TrackPoint) support
	# Define variant used per board above
	#PS2_USE_BUSYWAIT = yes # uses primitive reference code
	#PS2_USE_INT      = yes # uses external interrupt for falling edge of PS/2 clock pin
	#PS2_USE_USART    = yes # uses hardware USART engine for PS/2 signal receive(recommended)

	TMK_PS2_DIR = ./src/external/tmk_ps2
	-include $(TMK_PS2_DIR)/tmk_ps2.mk
	CC_FLAGS += $(OPT_DEFS)
	SRC += $(SRCDIR)/ps2mouse.c
	SRC += $(SRCDIR)/trackpoint.c
endif

FW_VERSION := $(shell git describe --tags --always --long --dirty="-D")-$(shell git log --pretty=format:%cd --date=short -n1)
ifeq ('',$(FW_VERSION))
FW_VERSION := unknown_version-$(shell date +%Y%m%d)
endif
CC_FLAGS    += -DFW_VERSION=\"$(FW_VERSION)\"

# generate a sample uuid for passhash secret
HMAC_TAG_BASE := $(shell tr -dc 'a-f0-9' </dev/urandom | head -c 40 | sed 's/\(..\)/\\\\x\1/g')
# Random xorshift init 32
XOR_RND_INIT := $(shell tr -dc 'a-f0-9' </dev/urandom | head -c 8 )
CC_FLAGS += -DXOR_RND_INIT=0x$(XOR_RND_INIT)

# Default target
all: lufacheck configtest private_data_check submodule

# Should check length of given data...
private_data_check:
ifneq ("$(wildcard $(SRCDIR)/_private_data.h)","")
	@echo "*** Private data definition found ";
else
	@echo "*** PRIVATE DATA $(SRCDIR)/_private_data.h NOT found.";
	@echo "*** Creating default one. Please edit template to your needs (file is ignored in git)";
	@sed 's/^const char \* HMAC_TAG_BASE.*/const char * HMAC_TAG_BASE = \"$(HMAC_TAG_BASE)\";/' $(SRCDIR)/_private_data_template.h > $(SRCDIR)/_private_data.h ;
	@chmod 600 $(SRCDIR)/_private_data.h ;
	@false;
endif


# check that LUFA is there
lufacheck:
	@if test -d LUFA/LUFA ; then \
		echo "*** LUFA found.";\
	else \
		echo "*** ERROR: LUFA/LUFA missing - see README for install instructions.\n***        Try to checkout LUFA source with\n***        git submodule init && git submodule update\n\n"; false;\
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

ifneq (,$(findstring USB_ID,$(CC_FLAGS)))
	@echo "*** USB_ID set to $(KB_USB_ID)" ;
else
	@echo "*** USB_ID unset, use KB_USB_ID=xxx or define yours in src/config.h";
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
