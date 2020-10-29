# Hey Emacs, this is a -*- makefile -*-

# Define version number
MAJOR = 0
MINOR = 7
PATCHLEVEL = 2
FIX = 2

# Forces bootloader version to 0, comment out or leave empty for release
#PRERELEASE =

#----------------------------------------------------------------------------
# WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
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
#
#
# Extensively modified for sd2iec by Ingo Korb
# Modified for flexible src dir option by Jim Brain
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
# make debug = Start either simulavr or avarice as specified for debugging,
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean all".
#----------------------------------------------------------------------------

# Read configuration file
ifdef CONFIG
 CONFIGSUFFIX = $(CONFIG:config%=%)
else
 CONFIG = config
 CONFIGSUFFIX =
endif

# Enable verbose compilation with "make V=1"
ifdef V
 Q :=
 E := @:
else
 Q := @
 E := @echo
endif

# Include the configuration file
include $(CONFIG)

# Set MCU name and length of binary for bootloader
# WARNING: Fuse settings not tested!
MCU := $(CONFIG_MCU)
ifeq ($(MCU),atmega16)
	BINARY_LENGTH = 0x3c00
	EFUSE = 0xff
	HFUSE = 0xd2
	LFUSE = 0xfc
else ifeq ($(MCU),atmega8)
	BINARY_LENGTH = 0x1c00
  HFUSE = 0xd1
  LFUSE = 0xe4
else ifeq ($(MCU),atmega88)
  BINARY_LENGTH = 0x1c00
  EFUSE = 0xf9
  HFUSE = 0xdc
  LFUSE = 0xe2
else ifeq ($(MCU),atmega168)
  BINARY_LENGTH = 0x3c00
  EFUSE = 0xf9
  HFUSE = 0xdc
  LFUSE = 0xe2
else ifeq ($(MCU),atmega162)
  BINARY_LENGTH = 0x3c00
  EFUSE = 0xff
  HFUSE = 0xd2
  LFUSE = 0xfc
else ifeq ($(MCU),atmega32)
  BINARY_LENGTH = 0x7800
  EFUSE = 0xff
  HFUSE = 0xd2
  LFUSE = 0xfc
else ifeq ($(MCU),atmega128)
  BINARY_LENGTH = 0x1f000
  EFUSE = 0xff
  HFUSE = 0xd2
  LFUSE = 0xfc
else ifeq ($(MCU),atmega1281)
  BINARY_LENGTH = 0x1f000
  EFUSE = 0xff
  HFUSE = 0xd2
  LFUSE = 0xfc
else ifeq ($(MCU),atmega2561)
  BINARY_LENGTH = 0x3f000
  EFUSE = 0xfd
  HFUSE = 0x93
  LFUSE = 0xef
else ifeq ($(MCU),atmega644)
  BINARY_LENGTH = 0xf000
  EFUSE = 0xfd
  HFUSE = 0x91
  LFUSE = 0xef
else ifeq ($(MCU),atmega644p)
  BINARY_LENGTH = 0xf000
  EFUSE = 0xfd
  HFUSE = 0x91
  LFUSE = 0xef
else ifeq ($(MCU),attiny2313)
  BINARY_LENGTH = 0x7800
  EFUSE = 0xff
  HFUSE = 0x9f
  LFUSE = 0xff
else
.PHONY: nochip
nochip:
	@echo '=============================================================='
	@echo 'No known target chip specified.'
	@echo
	@echo 'Please edit the Makefile.'
	@exit 1
endif

# Directory for all source files
SRCDIR := src

# Directory for all generated files
OBJDIR := obj-$(CONFIG_MCU:atmega%=m%)$(CONFIGSUFFIX)

# Output format. (can be srec, ihex, binary)
FORMAT = ihex


# Target file name (without extension).
TARGET = c=key

# List C source files here. (C dependencies are automatically generated.)
SRC  = switches.c 
SRC += uart.c
SRC += kb.c
SRC += main.c
SRC += ps2.c
SRC += eeprom.c
SRC += scanner64.c
SRC += led.c
SRC += ps2_kb.c
SRC += poll64.c

# Sample mechanism to add files to SRC line
#ifeq ($(CONFIG_VARIABLE),4)
#  SRC += file.c
#else
#  SRC += file2.c
#endif

# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC =


# Optimization level, can be [0, 1, 2, 3, s].
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
# Use s -mcall-prologues when you really need size...
#OPT = 2
OPT = s

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS =

# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS = -DF_CPU=$(CONFIG_MCU_FREQ)UL

# Calculate bootloader version
ifdef PRERELEASE
BOOT_VERSION := 0
else
BOOT_VERSION := 0x$(MAJOR)$(MINOR)$(PATCHLEVEL)$(FIX)
endif

# Create a version number define
ifdef PATCHLEVEL
ifdef FIX
PROGRAMVERSION := $(MAJOR).$(MINOR).$(PATCHLEVEL).$(FIX)
else
PROGRAMVERSION := $(MAJOR).$(MINOR).$(PATCHLEVEL)
BOOT_VERSION := $(BOOT_VERSION)0
endif
else
PROGRAMVERSION := $(MAJOR).$(MINOR)
BOOT_VERSION := $(BOOT_VERSION)00
endif

ifdef PRERELEASE
PROGRAMVERSION := $(PROGRAMVERSION)$(PRERELEASE)
endif

LONGVERSION := -$(CONFIG_MCU:atmega%=m%)$(CONFIGSUFFIX)

CDEFS += -DVERSION=\"$(PROGRAMVERSION)\" -DLONGVERSION=\"$(LONGVERSION)\"

# Place -I options here
CINCS =


# Define programs and commands.
# CC must be defined here to generate the correct CFLAGS
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
COPY = cp
WINSHELL = cmd
CRCGEN := scripts/crcgen-avr.pl
AWK = gawk
#CRCGEN = crcgen-new


#---------------- Compiler Options ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
# Add this if you want all warnings output as errors.
CFLAGS += -Werror
# Use this only on small uCs (program size-wise) and only as a last resort
#CFLAGS += -mshort-calls
#CFLAGS += -fno-unit-at-a-time
CFLAGS += -Wundef
CFLAGS += -Wextra
CFLAGS += -Wunreachable-code
CFLAGS += -Wshadow
#CFLAGS += -Winline
CFLAGS += -Wsign-compare
CFLAGS += -Wstrict-prototypes 
# Add this if you want all warnings output as errors.
#CFLAGS += -Werror
#CFLAGS += -Wa,-adhlns=$(OBJDIR)/$(<:.c=.lst)
CFLAGS += -I$(OBJDIR) -Isrc
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
# CFLAGS += -funsigned-char
# CFLAGS += -funsigned-bitfields
# CFLAGS += -fpack-struct
# CFLAGS += -fshort-enums
#CFLAGS += -fno-unit-at-a-time
# CFLAGS += -Wundef
# CFLAGS += -Wextra
# CFLAGS += -Wunreachable-code
# CFLAGS += -Wshadow
#CFLAGS += -Winline
# CFLAGS += -Wsign-compare
#CFLAGS += -mtiny-stack
#CFLAGS += -mno-interrupts
CFLAGS += -mcall-prologues
CFLAGS += -ffreestanding
CFLAGS += -fno-tree-scev-cprop
CFLAGS += -fno-optimize-sibling-calls
CFLAGS += -fno-tree-switch-conversion
CFLAGS += -maccumulate-args
CFLAGS += -mstrict-X
CFLAGS += -flto
CFLAGS += -fno-inline-small-functions
#CFLAGS += -finline-limit=3 
CFLAGS += -fno-move-loop-invariants
CFLAGS += -fno-split-wide-types

# turn these on to keep the functions in the same order as in the source
# this is only useful if you're looking at disassembly
#CFLAGS += -fno-reorder-blocks
#CFLAGS += -fno-reorder-blocks-and-partition
#CFLAGS += -fno-reorder-functions
#CFLAGS += -fno-toplevel-reorder
#CFLAGS += -fno-tree-loop-optimize

ifeq ($(CONFIG_STACK_TRACKING),y)
  CFLAGS += -finstrument-functions
endif


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex 
#       dump that will be displayed for a given single line of source input.
ASFLAGS = -Wa,-adhlns=$(OBJDIR)/$(*F).lst,-gstabs,--listing-cont-lines=100


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB =
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


MATH_LIB = -lm


# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS = 



#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =



#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(OBJDIR)/$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -flto
#LDFLAGS	+= -Wl,--section-start=.text=$(BOOTLOADERSTARTADR)
#LDFLAGS += -T linker_script.x
ifeq ($(CONFIG_LINKER_RELAX),y)
	LDFLAGS += -Wl,-O1,--relax
endif



#---------------- Programming Options (avrdude) ----------------

# Programming hardware: alf avr910 avrisp bascom bsd
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500 stk500v2
#
# Type: avrdude -c ?
# to get a full listing.
# By default, use what is in .avrduderc
#
#AVRDUDE_PROGRAMMER = stk200

# com1 = serial port. Use lpt1 to connect to parallel port.
#AVRDUDE_PORT = lpt1    # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(OBJDIR)/$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(OBJDIR)/$(TARGET).eep

# Allow fuse overrides from the config file
ifdef CONFIG_EFUSE
	EFUSE := $(CONFIG_EFUSE)
endif
ifdef CONFIG_HFUSE
	HFUSE := $(CONFIG_HFUSE)
endif
ifdef CONFIG_LFUSE
	LFUSE := $(CONFIG_LFUSE)
endif

# Calculate command line arguments for fuses
AVRDUDE_WRITE_FUSES :=
ifdef LFUSE
  AVRDUDE_WRITE_FUSES += -U lfuse:w:$(LFUSE):m
endif
ifdef HFUSE
  AVRDUDE_WRITE_FUSES += -U hfuse:w:$(HFUSE):m
endif
ifdef EFUSE
  AVRDUDE_WRITE_FUSES += -U efuse:w:$(EFUSE):m
endif


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

ifdef AVRDUDE_PROGRAMMER
  AVRDUDE_FLAGS = -c $(AVRDUDE_PROGRAMMER)
endif
ifdef AVRDUDE_PORT
  AVRDUDE_FLAGS += -P $(AVRDUDE_PORT)
endif
AVRDUDE_FLAGS += -p $(MCU)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)



#---------------- Debugging Options ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(CONFIG_MCU_FREQ)

# Set the DEBUG_UI to either gdb or insight.
# DEBUG_UI = gdb
DEBUG_UI = insight

# Set the debugging back-end to either avarice, simulavr.
DEBUG_BACKEND = avarice
#DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when
#     avarice is running on a different computer.
DEBUG_HOST = localhost



#============================================================================


# Presume we're in a project directory so name the program like the current
# directory. Set PROGRAM to override.
ifeq ($(TARGET),)
  TARGET := $(notdir $(CURDIR))
endif
 
# Presume the C and asm source files to be located in the subdirectory 'src'.
# Set SRCDIR to override.
ifeq ($(SRCDIR),)
  SRCDIR := src
endif
 
# De-dupe the list of C source files
CSRC := $(patsubst %,$(SRCDIR)/%,$(sort $(SRC)))

# Add subdir to assembler source files
ASMSRC_DIR := $(patsubst %,$(SRCDIR)/%,$(ASMSRC))

# Define all object files.
OBJ := $(patsubst %,$(OBJDIR)/%,$(CSRC:.c=.o) $(ASMSRC_DIR:.S=.o) $(CSRC_DIR:.cpp=.o))

# Define all listing files.
LST := $(patsubst %,$(OBJDIR)/%,$(CSRC:.c=.lst) $(ASMSRC_DIR:.S=.lst))

# Define the object directories
OBJDIRS := $(sort $(dir $(OBJ)))

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -mmcu=$(MCU) -I$(SRCDIR) $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I$(SRCDIR) -x assembler-with-cpp $(ASFLAGS) $(CDEFS)

# Default target.
all: build

build: elf hex bin eep lss 
	$(E) "  SIZE   $(OBJDIR)/$(TARGET).elf"
	$(Q)$(ELFSIZE)|grep -v debug

elf: $(OBJDIR)/$(TARGET).elf
bin: $(OBJDIR)/$(TARGET).bin
hex: $(OBJDIR)/$(TARGET).hex
eep: $(OBJDIR)/$(TARGET).eep
lss: $(OBJDIR)/$(TARGET).lss
sym: $(OBJDIR)/$(TARGET).sym


# Doxygen output:
doxygen:
	-rm -rf doxyinput
	mkdir doxyinput
	cp $(SRCDIR)/*.h $(SRCDIR)/*.c doxyinput
	srcipts/src2doxy.pl doxyinput/*.h doxyinput/*.c
	doxygen scripts/doxygen.conf

# Display size of file.
HEXSIZE = $(SIZE) --mcu=$(MCU) --target=$(FORMAT) $(OBJDIR)/$(TARGET).hex
ELFSIZE = $(SIZE) -A $(OBJDIR)/$(TARGET).elf
AVRMEM = avr-mem.sh $(TARGET).elf $(MCU)

# Program the device.
program: $(OBJDIR)/$(TARGET).hex $(OBJDIR)/$(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)  $(AVRDUDE_WRITE_EEPROM)

# Set fuses of the device
fuses: $(CONFIG)
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FUSES)

progall: $(OBJDIR)/$(TARGET).hex $(OBJDIR)/$(TARGET).eep $(CONFIG)
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)  $(AVRDUDE_WRITE_EEPROM) $(AVRDUDE_WRITE_FUSES)

# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set
#     a breakpoint at main().
gdb-config:
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif
	@echo break main >> $(GDBINIT_FILE)

debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
	$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause

else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT = $(OBJCOPY) --debugging
COFFCONVERT += --change-section-address .data-0x800000
COFFCONVERT += --change-section-address .bss-0x800000
COFFCONVERT += --change-section-address .noinit-0x800000
COFFCONVERT += --change-section-address .eeprom-0x810000



coff: $(TARGET).elf
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof


# Generate autoconf.h from config
.PRECIOUS : $(OBJDIR) $(OBJDIR)/autoconf.h
$(OBJDIR)/autoconf.h: $(CONFIG) | $(OBJDIR)
	$(E) "  CONF2H $(CONFIG)"
	$(Q)$(AWK) -f scripts/conf2h.awk $(CONFIG) > $(OBJDIR)/autoconf.h
$(OBJDIR):
	$(E) "  MKDIR  $(OBJDIR)"
	-$(Q)mkdir $(OBJDIR)

# Generate macro-only asmconfig.h from autoconf.h
.PRECIOUS: $(OBJDIR)/asmconfig.h
$(OBJDIR)/asmconfig.h: $(CONFFILES) src/config.h | $(OBJDIR)
	$(E) "  CPP    config.h"
	$(Q)$(CC) -E -dM $(ALL_ASFLAGS) src/config.h | grep -v "^#define __" > $@

# Create final output files (.hex, .eep) from ELF output file.
ifeq ($(CONFIG_BOOTLOADER),y)
$(OBJDIR)/%.bin: $(OBJDIR)/%.elf
	$(E) "  BIN    $@"
	$(Q)$(OBJCOPY) -O binary -R .eeprom $< $@
	$(E) "  CRCGEN $@"
	-$(Q)$(CRCGEN) $@ $(BINARY_LENGTH) $(CONFIG_BOOT_DEVID) $(BOOT_VERSION)
	$(E) "  COPY   $(CONFIG_HARDWARE_NAME)-firmware-$(PROGRAMVERSION).bin"
	$(Q)$(COPY) $@ $(OBJDIR)/$(CONFIG_HARDWARE_NAME)-firmware-$(PROGRAMVERSION).bin
else
$(OBJDIR)/%.bin: $(OBJDIR)/%.elf
	$(E) "  BIN    $@"
	$(Q)$(OBJCOPY) -O binary -R .eeprom $< $@
endif

$(OBJDIR)/%.hex: $(OBJDIR)/%.elf
	$(E) "  HEX    $@"
	$(Q)$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

$(OBJDIR)/%.eep: $(OBJDIR)/%.elf
	$(E) "  EEP    $@"
	$(Q)$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0

# Create extended listing file from ELF output file.
$(OBJDIR)/%.lss: $(OBJDIR)/%.elf
	$(E) "  LSS    $<"
	$(Q)$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
$(OBJDIR)/%.sym: $(OBJDIR)/%.elf
	$(E) "  SYM    $<"
	$(E)$(NM) -n $< > $@



# Link: create ELF output file from object files.
.SECONDARY : $(OBJDIR)/$(TARGET).elf
.PRECIOUS : $(OBJ)
$(OBJDIR)/%.elf: $(OBJ) | $(OBJDIR)
	$(E) "  LINK   $@"
	$(Q)$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c $(CONFFILES) | $(OBJDIR)/$(SRCDIR) $(OBJDIR)/autoconf.h
	$(E) "  CC     $<"
	$(Q)$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
$(OBJDIR)/%.s : $(SRCDIR)/%.c $(CONFFILES) | $(OBJDIR)/$(SRCDIR) $(OBJDIR)/autoconf.h
	$(E) "  CC     $<"
	$(Q)$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : $(SRCDIR)/%.S $(OBJDIR)/asmconfig.h $(CONFFILES) | $(OBJDIR) $(OBJDIR)/autoconf.h
	$(E) "  AS     $<"
	$(Q)$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Create preprocessed source for use in sending a bug report.
$(OBJDIR)/%.i : $(SRCDIR)/%.c | $(OBJDIR)/$(SRCDIR) $(OBJDIR)/autoconf.h
	$(E) "  CC     $<"
	$(Q)$(CC) -E -mmcu=$(MCU) -I$(SRCDIR) $(CFLAGS) $< -o $@

# Create the output directories
$(OBJDIR)/$(SRCDIR):
	$(E) "  MKDIR  $(OBJDIRS)"
	-$(Q)mkdir -p $(OBJDIRS)

# Target: clean project.
clean: begin clean_list end

clean_list :
	$(E) "  CLEAN"
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).hex
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).bin
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).eep
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).cof
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).elf
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).map
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).sym
	$(Q)$(REMOVE) $(OBJDIR)/$(TARGET).lss
	$(Q)$(REMOVE) $(OBJ)
	$(Q)$(REMOVE) $(OBJDIR)/autoconf.h
	$(Q)$(REMOVE) $(OBJDIR)/*.bin
	$(Q)$(REMOVE) $(LST)
	$(Q)$(REMOVE) $(SRCDIR)/$(CSRC:.c=.s)
	$(Q)$(REMOVE) $(SRCDIR)$(CSRC:.c=.d)
	$(Q)$(REMOVE) .dep/*
	$(Q)$(REMOVE) -rf codedoc
	$(Q)$(REMOVE) -rf doxyinput
	-$(Q)rmdir --ignore-fail-on-non-empty -p $(OBJDIR)

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program debug gdb-config doxygen

