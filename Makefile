##
## This file is part of the libopenstm32 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

OUT_DIR  = obj
PREFIX		?= arm-none-eabi
CONTROL     ?= SINE
CONTROLLC   := $(shell echo $(CONTROL) | tr A-Z a-z)
BINARY		= stm32_$(CONTROLLC)
SIZE  = $(PREFIX)-size
CC		= $(PREFIX)-gcc
CPP	= $(PREFIX)-g++
LD		= $(PREFIX)-gcc
OBJCOPY		= $(PREFIX)-objcopy
OBJDUMP		= $(PREFIX)-objdump
MKDIR_P     = mkdir -p
TERMINAL_DEBUG ?= 0
CFLAGS_COMMON = -Os -Wall -Wextra -Iinclude/ -Ilibopeninv/include -Ilibopencm3/include \
				 -fno-common -fno-builtin -pedantic  -DT_DEBUG=$(TERMINAL_DEBUG) \
				 -DCONTROL=CTRL_$(CONTROL) -DCTRL_SINE=0 -DCTRL_FOC=1
CFLAGS		= $(CFLAGS_COMMON) -std=gnu99 \
				 -DSTM32F1 -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections
CPPFLAGS    = $(CFLAGS_COMMON) -std=c++11 \
				-ffunction-sections -fdata-sections -fno-builtin -fno-rtti -fno-exceptions -fno-unwind-tables \
				 -DSTM32F1 -mcpu=cortex-m3 -mthumb
LDSCRIPT	= stm32_sine.ld
LDFLAGS  = -Llibopencm3/lib -T$(LDSCRIPT) -march=armv7 -nostartfiles -Wl,--gc-sections,-Map,linker.map,--no-warn-rwx-segments
OBJSL		= stm32_sine.o hwinit.o stm32scheduler.o params.o terminal.o terminal_prj.o \
           my_string.o digio.o sine_core.o my_fp.o fu.o inc_encoder.o printf.o anain.o \
           temp_meas.o param_save.o throttle.o errormessage.o stm32_can.o pwmgeneration.o \
           picontroller.o terminalcommands.o vehiclecontrol.o

ifeq ($(CONTROL), SINE)
	OBJSL += pwmgeneration-sine.o
endif
ifeq ($(CONTROL), FOC)
	OBJSL += pwmgeneration-foc.o foc.o
endif

OBJS     = $(patsubst %.o,obj/%.o, $(OBJSL))
vpath %.c src/ libopeninv/src
vpath %.cpp src/ libopeninv/src

OPENOCD_BASE	= /usr
OPENOCD		= $(OPENOCD_BASE)/bin/openocd
OPENOCD_SCRIPTS	= $(OPENOCD_BASE)/share/openocd/scripts
OPENOCD_FLASHER	= $(OPENOCD_SCRIPTS)/interface/parport.cfg
OPENOCD_BOARD	= $(OPENOCD_SCRIPTS)/board/olimex_stm32_h103.cfg

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
NULL := 2>/dev/null
endif

all: directories images genparamdb
Debug:images
Release: images
cleanDebug:clean
images: $(BINARY)
	@printf "  OBJCOPY $(BINARY).bin\n"
	$(Q)$(OBJCOPY) -Obinary $(BINARY) $(BINARY).bin
	@printf "  OBJCOPY $(BINARY).hex\n"
	$(Q)$(OBJCOPY) -Oihex $(BINARY) $(BINARY).hex
	$(Q)$(SIZE) $(BINARY)

directories: ${OUT_DIR}

${OUT_DIR}:
	$(Q)${MKDIR_P} ${OUT_DIR}

$(BINARY): $(OBJS) $(LDSCRIPT)
	@printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) $(LDFLAGS) -o $(BINARY) $(OBJS) -lopencm3_stm32f1

$(OUT_DIR)/%.o: %.c Makefile
	@printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

$(OUT_DIR)/%.o: %.cpp Makefile
	@printf "  CPP     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CPP) $(CPPFLAGS) -o $@ -c $<

genparamdb: utils/genparamdb/genparamdb.cpp include/param_prj.h
	$(Q)g++ $(CFLAGS_COMMON) -DNDEBUG \
	utils/genparamdb/genparamdb.cpp -o genparamdb

clean:
	@printf "  CLEAN   ${OUT_DIR}\n"
	$(Q)rm -rf ${OUT_DIR}
	@printf "  CLEAN   $(BINARY)\n"
	$(Q)rm -f $(BINARY)
	@printf "  CLEAN   $(BINARY).bin\n"
	$(Q)rm -f $(BINARY).bin
	@printf "  CLEAN   $(BINARY).hex\n"
	$(Q)rm -f $(BINARY).hex
	@printf "  CLEAN   $(BINARY).srec\n"
	$(Q)rm -f $(BINARY).srec
	@printf "  CLEAN   $(BINARY).list\n"
	$(Q)rm -f $(BINARY).list
	@printf "  CLEAN   genparamdb\n"
	$(Q)rm -f genparamdb

flash: images
	@printf "  FLASH   $(BINARY).bin\n"
	@# IMPORTANT: Don't use "resume", only "reset" will work correctly!
	$(Q)$(OPENOCD) -s $(OPENOCD_SCRIPTS) \
		       -f $(OPENOCD_FLASHER) \
		       -f $(OPENOCD_BOARD) \
		       -c "init" -c "reset halt" \
		       -c "flash write_image erase $(BINARY).hex" \
		       -c "reset" \
		       -c "shutdown" $(NULL)

.PHONY: directories images clean

get-deps:
	@printf "  GIT SUBMODULE\n"
	$(Q)git submodule update --init
	@printf "  MAKE libopencm3\n"
	$(Q)${MAKE} -C libopencm3

Test:
	cd test && $(MAKE)
cleanTest:
	cd test && $(MAKE) clean
