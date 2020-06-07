##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
## Copyright (C) 2013 Frantisek Burian <BuFran@seznam.cz>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##
## Copied from:
## https://github.com/libopencm3/libopencm3-examples/blob/master/examples/rules.mk

## FDSoftware, pa no olvidarme como flashear el stm32 stm32flash -v -w archivo.bin /dev/ttyUSB0 
BINARY = openefi

## TARGET SPECIFIC
LIBNAME		= opencm3_stm32f1
DEFS		+= -DSTM32F1

FP_FLAGS	?= -msoft-float
ARCH_FLAGS	= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

OOCD		?= openocd
OOCD_INTERFACE	?= stlink-v2
OOCD_TARGET	?= stm32f1x
OOCD_BOARD ?= target/stm32f1x.cfg

## /


# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q		:= @
NULL		:= 2>/dev/null
endif

###############################################################################
# Executables

PREFIX		?= arm-none-eabi

AS		:= $(PREFIX)-as
CC		:= $(PREFIX)-gcc
CXX		:= $(PREFIX)-g++
LD		:= $(PREFIX)-gcc
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump
GDB		:= $(PREFIX)-gdb
STFLASH		= $(shell which st-flash)
STYLECHECK	:= /checkpatch.pl
STYLECHECKFLAGS	:= --no-tree -f --terse --mailback
STYLECHECKFILES	:= $(shell find . -name '*.[ch]')
OPT		:= -Os
CSTD		?= -std=c99


###############################################################################
# Source files

OBJS		+= src/$(BINARY).o
SOURCES_AS := $(wildcard ./qfplib/*.s)
OBJS_AS := $(SOURCES_AS:.s=.o)

ifeq ($(strip $(OPENCM3_DIR)),)
# user has not specified the library path, so we try to detect it

# where we search for the library
OPENCM3_DIR = ./libopencm3
INCLUDE_DIR = $(OPENCM3_DIR)/include
LIB_DIR     = $(OPENCM3_DIR)/lib

# OpenEFI library
LIB_DIR2     = src/lib
INCLUDE_DIR2 = src/include

ifeq ($(strip $(OPENCM3_DIR)),)
$(warning "$(LIBPATHS)" "$(OPENCM3_DIR)")
$(warning Cannot find libopencm3 library in the standard search paths.)
$(error Please specify it through OPENCM3_DIR variable!)
endif
endif

ifeq ($(V),1)
$(info Using $(OPENCM3_DIR) path to library)
endif

define ERR_DEVICE_LDSCRIPT_CONFLICT
You can either specify DEVICE=blah, and have the LDSCRIPT generated,
or you can provide LDSCRIPT, and ensure CPPFLAGS, LDFLAGS and LDLIBS
all contain the correct values for the target you wish to use.
You cannot provide both!
endef

ifeq ($(strip $(DEVICE)),)
# Old style, assume LDSCRIPT exists
DEFS		+= -I$(OPENCM3_DIR)/include
LDFLAGS		+= -L$(OPENCM3_DIR)/lib
LDFLAGS		+= -L$(LIB_DIR2)
LDFLAGS		+= -L$(OPENCM3_DIR)/lib/stm32/f1
LDLIBS		+= -l$(LIBNAME)
LDSCRIPT	?= $(BINARY).ld
else
# New style, assume device is provided, and we're generating the rest.
ifneq ($(strip $(LDSCRIPT)),)
$(error $(ERR_DEVICE_LDSCRIPT_CONFLICT))
endif
include $(OPENCM3_DIR)/mk/genlink-config.mk
endif

SCRIPT_DIR	= $(OPENCM3_DIR)/scripts

###############################################################################
# C flags

TGT_CFLAGS	+= $(OPT) $(CSTD) -g
TGT_CFLAGS	+= $(ARCH_FLAGS)
TGT_CFLAGS	+= -Wextra -Wshadow -Wimplicit-function-declaration
TGT_CFLAGS	+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
TGT_CFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C++ flags

TGT_CXXFLAGS	+= $(OPT) $(CXXSTD) -g
TGT_CXXFLAGS	+= $(ARCH_FLAGS)
TGT_CXXFLAGS	+= -Wextra -Wshadow -Wredundant-decls  -Weffc++
TGT_CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C & C++ preprocessor common flags

TGT_CPPFLAGS	+= -MD
TGT_CPPFLAGS	+= -Wall -Wundef
TGT_CPPFLAGS    += -I$(INCLUDE_DIR) $(DEFS)
TGT_CPPFLAGS    += -I$(INCLUDE_DIR2)

###############################################################################
# Linker flags

TGT_LDFLAGS		+= --static -nostartfiles
TGT_LDFLAGS     += -L$(LIB_DIR)
TGT_LDFLAGS		+= -L$(LIB_DIR2)
TGT_LDFLAGS		+= -T$(LDSCRIPT)
TGT_LDFLAGS		+= $(ARCH_FLAGS)
TGT_LDFLAGS		+= -Wl,-Map=src/$(*).map
TGT_LDFLAGS		+= -Wl,--gc-sections
ifeq ($(V),99)
TGT_LDFLAGS		+= -Wl,--print-gc-sections
endif

###############################################################################
# Used libraries

LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

###############################################################################
###############################################################################
###############################################################################

.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

all: bin

elf: $(BINARY).elf
bin: $(BINARY).bin
hex: $(BINARY).hex
srec: $(BINARY).srec
list: $(BINARY).list

images: $(BINARY).images
flash: $(BINARY).flash
usb-flash: $(BINARY).usb-flash

# Either verify the user provided LDSCRIPT exists, or generate it.
ifeq ($(strip $(DEVICE)),)
$(LDSCRIPT):
    ifeq (,$(wildcard $(LDSCRIPT)))
        $(error Unable to find specified linker script: $(LDSCRIPT))
    endif
else
include $(OPENCM3_DIR)/mk/genlink-rules.mk
endif

$(OPENCM3_DIR)/Makefile:
	$(Q)git submodule update --init $(OPENCM3_DIR)

$(LIB_DIR)/lib$(LIBNAME).a: $(OPENCM3_DIR)/Makefile
	$(Q)$(MAKE) -C $(OPENCM3_DIR)


# Define a helper macro for debugging make errors online
# you can type "make print-OPENCM3_DIR" and it will show you
# how that ended up being resolved by all of the included
# makefiles.
print-%:
	@echo $*=$($*)

%.images: %.bin %.hex %.srec %.list %.map
	@printf "*** $* images generated ***\n"

%.bin: %.elf
	@mkdir -p bin
	@printf "  OBJCOPY   bin/$(*).bin\n"
	$(Q)$(OBJCOPY) -Obinary bin/$(*).elf bin/$(*).bin

%.hex: %.elf
	@mkdir -p bin
	@printf "  OBJCOPY   bin/$(*).hex\n"
	$(Q)$(OBJCOPY) -Oihex bin/$(*).elf bin/$(*).hex

%.srec: %.elf
	@mkdir -p bin
	@printf "  OBJCOPY   bin/$(*).srec\n"
	$(Q)$(OBJCOPY) -Osrec bin/$(*).elf bin/$(*).srec

%.list: %.elf
	@mkdir -p bin
	@printf "  OBJDUMP bin/$(*).list\n"
	$(Q)$(OBJDUMP) -S bin/$(*).elf > bin/$(*).list

%.elf %.map: $(OBJS_AS) $(OBJS) $(LDSCRIPT) $(LIB_DIR)/lib$(LIBNAME).a
	@mkdir -p bin
	@printf "  LD        bin/$(*).elf\n"
	$(Q)$(LD) $(TGT_LDFLAGS) $(LDFLAGS) $(OBJS_AS) $(OBJS) $(LDLIBS) -o bin/$(*).elf

%.o: %.s %.h
	@printf "  AS        $(*).s\n"
	$(Q)$(AS) -o $(*).o $(*).s

%.o: %.c $(LIB_DIR)/lib$(LIBNAME).a
	@printf "  CC        $(*).c\n"
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

%.o: %.cxx $(LIB_DIR)/lib$(LIBNAME).a
	@printf "  CXX       $(*).cxx\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cxx

%.o: %.cpp $(LIB_DIR)/lib$(LIBNAME).a
	@printf "  CXX       $(*).cpp\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cpp

clean:
	@printf "  CLEAN\n"
	$(Q)$(RM) bin/* src/*.o src/*.d src/*.map generated.* ${OBJS} ${OBJS_AS} ${OBJS:%.o:%.d}

stylecheck: $(STYLECHECKFILES:=.stylecheck)
styleclean: $(STYLECHECKFILES:=.styleclean)

# the cat is due to multithreaded nature - we like to have consistent chunks of text on the output
%.stylecheck: %
	$(Q)$(SCRIPT_DIR)$(STYLECHECK) $(STYLECHECKFLAGS) $* > $*.stylecheck; \
		if [ -s $*.stylecheck ]; then \
			cat $*.stylecheck; \
		else \
			rm -f $*.stylecheck; \
		fi;

%.styleclean:
	$(Q)rm -f $*.stylecheck;


%.stlink-flash: %.bin
	@printf "  FLASH    $<\n"
	$(STFLASH) write $(*).bin 0x8000000

%.usb-flash: %.bin
	@printf "  USB-FLASH $<\n"
	$(Q)./src/usb-flash
ifeq ($(STLINK_PORT),)
ifeq ($(BMP_PORT),)
ifeq ($(OOCD_FILE),)
%.flash: %.elf
	@printf "  FLASH   $<\n"
	(echo "halt; program $(realpath bin/$(*).elf) verify reset" | nc -4 localhost 4444 2>/dev/null) || \
		$(OOCD) -f interface/$(OOCD_INTERFACE).cfg \
		-f target/$(OOCD_TARGET).cfg \
		-c "program bin/$(*).elf verify reset exit" \
		$(NULL)
else
%.flash: %.elf
	@printf "  FLASH   $<\n"
	(echo "halt; program $(realpath bin/$(*).elf) verify reset" | nc -4 localhost 4444 2>/dev/null) || \
		$(OOCD) -f $(OOCD_FILE) \
		-c "program bin/$(*).elf verify reset exit" \
		$(NULL)
endif
else
%.flash: %.elf
	@printf "  GDB   bin/$(*).elf (flash)\n"
	$(GDB) --batch \
		   -ex 'target extended-remote $(BMP_PORT)' \
		   -x $(SCRIPT_DIR)/black_magic_probe_flash.scr \
		   bin/$(*).elf
endif
else
%.flash: %.elf
	@printf "  GDB   bin/$(*).elf (flash)\n"
	$(GDB) --batch \
		   -ex 'target extended-remote $(STLINK_PORT)' \
		   -x $(SCRIPT_DIR)/stlink_flash.scr \
		   bin/$(*).elf
endif

.PHONY: images clean stylecheck styleclean elf bin hex srec list

-include $(OBJS:.o=.d)
