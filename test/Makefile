CC		= gcc
CPP	= g++
LD		= g++
CP		= cp
CFLAGS    = -std=c99 -ggdb -DSTM32F1 -I../include -I../libopeninv/include -I../libopencm3/include
CPPFLAGS    = -ggdb -DSTM32F1 -DCTRL_FOC=1 -DCTRL_SINE=0 -DCTRL=CTRL_FOC -I../include -I../libopeninv/include -I../libopencm3/include
LDFLAGS     = -g
BINARY		= test_sine
OBJS		= test_main.o fu.o test_fu.o test_fp.o test_vcu.o my_fp.o my_string.o params.o vehiclecontrol.o GD31xxOI.o \
			  test_throttle.o throttle.o sine_core.o temp_meas.o stub_canhardware.o \
			  stub_libopencm3.o
VPATH = ../src ../libopeninv/src

# Extract the current state of this git repository
# to embed it into the firmware.
GIT_VERSION = $(shell git describe --tags --always --dirty)
GIT_HASH = $(shell echo -n "0x" && git rev-parse --short=8 HEAD)
GIT_DIRTY = $(shell test -n "`git status --porcelain`" && echo "1" || echo "0")
CPPFLAGS += -DGIT_VERSION=$(GIT_VERSION) -DGIT_HASH=$(GIT_HASH) -DGIT_DIRTY=$(GIT_DIRTY)

all: $(BINARY)

$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) -o $(BINARY) $(OBJS)

%.o: ../%.cpp
	$(CPP) $(CPPFLAGS) -o $@ -c $<

%.o: ../%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(BINARY)
