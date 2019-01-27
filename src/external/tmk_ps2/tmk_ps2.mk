PROTOCOL_DIR = .

ifeq (yes,$(strip $(PS2_MOUSE_ENABLE)))
#    SRC += $(PROTOCOL_DIR)/ps2_mouse.c
    OPT_DEFS += -DPS2_MOUSE_ENABLE
    OPT_DEFS += -DMOUSE_ENABLE
endif

ifeq (yes,$(strip $(PS2_USE_BUSYWAIT)))
    SRC += $(PROTOCOL_DIR)/ps2_busywait.c
    SRC += $(PROTOCOL_DIR)/ps2_io_avr.c
    OPT_DEFS += -DPS2_USE_BUSYWAIT
endif

ifeq (yes,$(strip $(PS2_USE_INT)))
    SRC += $(PROTOCOL_DIR)/ps2_interrupt.c
    SRC += $(PROTOCOL_DIR)/ps2_io_avr.c
    OPT_DEFS += -DPS2_USE_INT
endif

ifeq (yes,$(strip $(PS2_USE_USART)))
    SRC += $(PROTOCOL_DIR)/ps2_usart.c
    SRC += $(PROTOCOL_DIR)/ps2_io_avr.c
    OPT_DEFS += -DPS2_USE_USART
endif


# Search Path
VPATH += src/tmk_ps2/
