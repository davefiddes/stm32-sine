# stm32-sine M3_DU

[![Build status](../../actions/workflows/CI-build.yml/badge.svg)](../../actions/workflows/CI-build.yml)

Firmware for Damien Maguire's Tesla Model 3 [inverter replacement project](https://github.com/damienmaguire/Tesla-Model-3-Drive-Unit). This firmware is a maintained fork of Johannes Huebner's [stm32-sine](https://github.com/jsphuebner/stm32-sine) firmware.

Though not a prime focus of the project it should run on any revision of the "Huebner" hardware <https://github.com/jsphuebner/inverter-hardware> as well as any derivatives such as the Open Source Tesla controller <https://github.com/damienmaguire>.

## Features

Over the original stm32-sine firmware this fork adds:

* Support for the M3_DU inverter PCB
* Configuration and fault monitoring of the M3_DU gate drivers
* Tesla Model 3 oil pump control
* Robust configuration over CAN using [OpenInverter CAN Tool](https://github.com/davefiddes/openinverter-can-tool/) or [esp32-web-interface](https://github.com/jsphuebner/esp32-web-interface/tree/can-backend)

## Binary releases

Binary releases can be found on the [releases page](../../releases).

## Further reading

Development is documented in this forum thread: <https://openinverter.org/forum/viewtopic.php?t=575>

Progress of the hardware and system integration is documented on Damien Maguire's YouTube channel: <https://www.youtube.com/@Evbmw>

A comprehensive guide to the Huebner inverter system can be found here: <https://openinverter.org/docs>

## Compiling

Building the firmware requires Linux and an `arm-none-eabi` toolchain.:

On Debian/Ubuntu install the toolchain by running:

```terminal
sudo apt-get install build-essential git gcc-arm-none-eabi
```

On Fedora:

```terminal
sudo dnf group install c-development development-tools
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib
```

The only dependency is [libopencm3](https://github.com/libopencm3/libopencm3) which is maintained as a git submodule. You can download and build this by running:

```terminal
make get-deps
```

Now you can build the FOC firmware for rear drive units by running:

```terminal
make CONTROL=FOC
```

or SINE firmware for front drive units by running:

```terminal
make CONTROL=SINE
```

And upload it to your board using a JTAG/SWD adapter using [openocd](http://openocd.org/):

```terminal
make flash
```

or with OpenInverter CAN Tool:

```terminal
oic upgrade stm32_foc.bin
```
