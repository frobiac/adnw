# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
# WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
#  >> Modified for use with the LUFA project. <<
#
# Released to the Public Domain
#
# Additional material for this makefile was written by:
# Peter Fleury
# Tim Henigan
# Colin O'Flynn
# Reiner Patommel
# Markus Pfaff
# Sander Pool
# Frederik Rouleau
# Carlos Lamas
# Dean Camera
# Opendous Inc.
# Denver Gingerich
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make dfu = Download the hex file to the device, using dfu-programmer (must
#            have dfu-programmer installed).
#
# make flip = Download the hex file to the device, using Atmel FLIP (must
#             have Atmel FLIP installed).
#
# make dfu-ee = Download the eeprom file to the device, using dfu-programmer
#               (must have dfu-programmer installed).
#
# make flip-ee = Download the eeprom file to the device, using Atmel FLIP
#                (must have Atmel FLIP installed).
#
# make doxygen = Generate DoxyGen documentation for the project (must have
#                DoxyGen installed)
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------

#HH_DEFS += -DUSE_HHSTDIO -DNEW_MOUSE -DANALOGSTICK
#HH_DEFS += -DUSE_HHSTDIO -DNEW_MOUSE  -DPS2MOUSE -DMOUSE_HAS_SCROLL_WHEELS -DSWAP_LEFT_RIGHT
#HH_DEFS += -DUSE_HHSTDIO -DNEW_MOUSE  -DPS2MOUSE -DMOUSE_HAS_SCROLL_WHEELS -DSWAP_LEFT_RIGHT


# MCU name, you MUST set this to match the board you are using
# # type "make clean" after changing this, so all files will be rebuilt
# #
# #MCU = at90usb162       # Teensy 1.0
#MCU = atmega32u4        # Teensy 2.0

# #MCU = at90usb646       # Teensy++ 1.0
# #MCU = at90usb1286      # Teensy++ 2.0
#

# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the 
#     processor frequency in Hz. You can then use this symbol in your source code to 
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#
#     This will be an integer division of F_CLOCK below, as it is sourced by
#     F_CLOCK after it has run through any CPU prescalers. Note that this value
#     does not *change* the processor frequency - it should merely be updated to
#     reflect the processor speed set externally so that the code can use accurate
#     software delays.
#F_CPU = 16000000

# Input clock frequency.
#     This will define a symbol, F_CLOCK, in all source code files equal to the 
#     input clock frequency (before any prescaling is performed) in Hz. This value may
#     differ from F_CPU if prescaling is used on the latter, and is required as the
#     raw input clock is fed directly to the PLL sections of the AVR for high speed
#     clock generation for the USB and other AVR subsections. Do NOT tack on a 'UL'
#     at the end, this will be done automatically to create a 32-bit value in your
#     source code.
#
#     If no clock division is performed on the input clock inside the AVR (via the
#     CPU clock adjust registers or the clock division fuses), this will be equal to F_CPU.
#F_CLOCK = $(F_CPU)

# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
#OPT = s
#OPT = 0




#============================================================================
include ./src/makefile
