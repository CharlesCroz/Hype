# Hype - Sources

## Install

Get yourself `make` and `arm-none-eabi-gcc`, and here we go !

## Build

Turn the config into auto-generated code:

`make autogen`

Compile everything (autogen, drivers, kernel boot) into boot.elf:

`make`

Compile partitions into partitions.bin:

`make -C kernel/partitions`


# Profit

Get tio or screen (but I'm a screen noob) :
- `tio /dev/ttyUSB1` to check outputs.
- `xsdb JTAG_prog.tcl` to program the device !


