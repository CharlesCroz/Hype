# Hype - Sources

## Install

Get yourself `make` and `arm-none-eabi-gcc`, and here we go !

## Build

Turn the config into auto-generated code:

`python3 conftool/process_config.py conftool/configs/platform.yml conftool/configs/sample-conf.yml conftool/templates/ kernel/autogen/`

Turn the auto-generated code into objects:

`make -C kernel/autogen`

Turn the auto-generated objects, and kernel code, into boot.elf:

`make -C kernel/boot`


# Profit

Get tio or screen (but I'm a screen noob) :
- `tio /dev/ttyUSB1` to check outputs.
- `xsdb JTAG_prog.tcl` to program the device !


