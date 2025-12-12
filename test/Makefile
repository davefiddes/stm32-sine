OUT_DIR  = obj
CC		= gcc
CPP	= g++
LD		= g++
CP		= cp
CFLAGS    = -std=c99 -Wall -Wextra -Werror -fsanitize=undefined,address -ggdb -DSTM32F1 -I../include -I../libopeninv/include -I../libopencm3/include
CXXFLAGS    = -std=c++14 -Wall -Wextra -Werror -fsanitize=undefined,address -ggdb -DSTM32F1 -DCTRL_FOC=1 -DCTRL_SINE=0 -DCTRL=CTRL_FOC -I../include -I../libopeninv/include -I../libopencm3/include
LDFLAGS     = -g -fsanitize=undefined,address
BINARY		= test_sine
OBJSL = test_main.o fu.o test_fu.o test_fp.o test_vcu.o my_fp.o my_string.o params.o vehiclecontrol.o GD31xxOI.o \
			  test_throttle.o throttle.o sine_core.o temp_meas.o stub_canhardware.o \
			  stub_libopencm3.o test_throttle_tempderate.o test_oiltempcontroller.o oiltempcontroller.o

OBJS     = $(patsubst %.o,obj/%.o, $(OBJSL))
DEPENDS  = $(patsubst %.o,obj/%.d, $(OBJSL))
vpath %.c ../src/ ../libopeninv/src
vpath %.cpp ../src/ ../libopeninv/src

# Extract the current state of this git repository
# to embed it into the firmware.
GIT_VERSION = $(shell git describe --tags --always --dirty)
GIT_HASH = $(shell echo -n "0x" && git rev-parse --short=8 HEAD)
GIT_DIRTY = $(shell test -n "`git status --porcelain`" && echo "1" || echo "0")
GIT_DEFINES = -DGIT_VERSION=$(GIT_VERSION) -DGIT_HASH=$(GIT_HASH) -DGIT_DIRTY=$(GIT_DIRTY)
CFLAGS += $(GIT_DEFINES)
CXXFLAGS += $(GIT_DEFINES)

all: $(BINARY)

$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) -o $(BINARY) $(OBJS)

-include $(DEPENDS)

$(OUT_DIR)/%.o: %.cpp Makefile
	@mkdir -p ${OUT_DIR}
	$(CPP) $(CXXFLAGS) -MMD -MP -o $@ -c $<

$(OUT_DIR)/%.o: %.c Makefile
	@mkdir -p ${OUT_DIR}
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

.PHONY: clean

clean:
	rm -rf ${OUT_DIR}
	rm -f $(BINARY)
