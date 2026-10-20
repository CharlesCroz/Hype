# Chapitre 0 - Hello World

First goal : tools, and a first **usable** Hello world from Xilinx tools. In particular, the correct GCC and the skeleton of a code that boots (JTAG or SD).

## Vivado Project

Zybo_GPIO_led_sws_INT_btn.xsa : Inside the PL (I assume you've played with Vivado and Vitis. If not, consider it's hardware), a GPIO AXI IP controling 4 leds and 4 switchs at address 0x4000_0000, and the 4 buttons connectd to iterrupt controller [64-61].

## Vitis Project

I created an Application Hello wold Project on CPU0 from the Vivado generated XSA. Compiled and successfully programmed to the card using Vitis. Good, next.

From the Vitis console, I extracted tcl commands that were used to program the hardware. I modified the xsdb commands have been modified to "anonymize" the cards (JTAG serial numbers for instance) and relative paths instead of absolute paths have been used in filenames.

Useful **xsdb** command : `targets -target-properties` (to understand what target filters target).

## Detour near MMU

After playing a little with **objdump** on the generated Kernel.elf and a generated fsbl.elf somewhere in the Vitis project, I noticed that the linker script places the Kernel code to DDR (0x10_0000) while the fsbl places it in the OCM (On Chip Memory, 0x0). First time playing with *lscript.ld* linker script was a success !

More fun with **xsdb**, in particular `mr`, `mw` (Memory Read/Write), `rrd`, `rwr` (Register ReaD/WRite), et assembler `mcr` (Move to Coprocessor from Register) and `mrc` (Move to Register from Coprocessor), using an online compiling tool to get the hex value of the instruction from assembler.

Little python tool to decode registers values : `python3 ./Tools/decoder.py --help`

And the new *main.c*, to copy registers values from CP15 to general register for display :
```C
void print_reg(char *name, unsigned int reg){
    char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char buff[11] = {'\0'};

    print(name);
    print(" 0x");

    for(int i = 7; i >= 0; i--){
        buff[7-i] = hex[(reg >> (4*i)) & 0xF];
    }
    buff[8] = '\n';
    buff[9] = '\r';
    buff[10] = '\0';

    print(buff);
}

int main()
{
    unsigned int reg = 0;

    print("Hello World\n\r");

    __asm ("mrc p15, 0, %[result], c2, c0, 0" : [result] "=r" (reg) :);
    print_reg("TTBR0", reg);
    
    __asm ("mrc p15, 0, %[result], c2, c0, 1" : [result] "=r" (reg) :);
    print_reg("TTBR1", reg);

    __asm ("mrc p15, 0, %[result], c2, c0, 2" : [result] "=r" (reg) :);
    print_reg("TTBCR", reg);

    return 0;
}
```

We see from this (and other things) that MMU level 1 tranlsation table is at address 0x4000, cf. for instance the result using **xsdb** of the line `dow .[...]/Kernel.elf`. It maps the 4GB of virtual address to the same physical addresses, but having different permission and cache properties.

```
Downloading Program -- /[...]/Hype/HW/Vitis/Kernel/Release/Kernel.elf                                         
	section, .text: 0x00000000 - 0x000020d7
	section, .init: 0x000020d8 - 0x000020e3
	section, .fini: 0x000020e4 - 0x000020ef
	section, .rodata: 0x000020f0 - 0x00002173
	section, .data: 0x00002178 - 0x000027df
	section, .eh_frame: 0x000027e0 - 0x000027e3
	section, .mmu_tbl: 0x00004000 - 0x00007fff
	section, .init_array: 0x00008000 - 0x00008007
	section, .fini_array: 0x00008008 - 0x0000800b
	section, .bss: 0x0000800c - 0x0000835f
	section, .heap: 0x00008360 - 0x0000a35f
	section, .stack: 0x0000a360 - 0x0000db5f
```

For instance `00015DE6` (first table entry @0x4000) vs `40000C02` (@0x5000) vs `C0000000` (@0x7000) :
- `00015DE6` : Range `0x0000_0000-0x0010_0000` -> **DDR/OCM** -> S/TEX/AP/Domain/B bits are set
- `40000C02` : Range `0x4000_0000-0x4010_0000` -> **FPGA** -> S/TEX/AP/Domain/B bits are unset
- `C0000000` : Range `0xC000_0000-0xC000_0000` -> **Not in system-level address map** -> Fault

## Vitis project (resuming)

The Application makefile output shows three steps : 
1. `helloworld.c -> helloworld.o`
1. `platform.c -> platform.o`
1. `*.o -> Kernel.elf`

The first log goes : 

`arm-none-eabi-gcc -Wall -O2 -c -fmessage-length=0 -MT"src/helloworld.o" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I/[...]/Hype/HW/Vitis/Zybo_GPIO_led_sws_INT_btn/export/Zybo_GPIO_led_sws_INT_btn/sw/Zybo_GPIO_led_sws_INT_btn/standalone_ps7_cortexa9_0/bspinclude/include -MMD -MP -MF"src/helloworld.d" -MT"src/helloworld.o" -o "src/helloworld.o" "../src/helloworld.c"`

With options :
- `-Wall` : All warnings
- `-O2` : *Optimize even more. GCC performs nearly all supported optimizations that do not involve a space-speed tradeoff.*
- `-c` : Not linked (yet)
- `-fmessage-length=0` *Try to format error messages so that they fit on lines of about n characters. If n is zero, then no line-wrapping is done; each error message appears on a single line. This is the default for all front ends.*
- `-MT"src/helloworld.o"` : Weird repeat, see bellow
- `-mcpu=cortex-a9` : *Specify  the  name  of  the  target  processor, optionally suffixed by one or more feature modifiers.  This option has the form -mcpu=cpu{+[no]feature}\*, where the permissible values for cpu are the same as those available for -mtune.  The permissible values for feature are documented in the sub-section on aarch64-feature-modifiers,,-march and -mcpu Feature Modifiers.  Where conflicting feature modifiers are  specified,  the  right-most  feature  is used.*
- `-mfpu=vfpv3` : *Enables support for specific floating-point hardware extensions for ARCv2 cores.* [VFP](https://support.arm.com/documentation/den0013/0400/ARM-Architecture-and-Processors/Architecture-history-and-extensions/VFP?lang=en), [vfpv3](https://support.arm.com/documentation/den0013/0400/Floating-Point/VFP-support-in-GCC)
- `-mfloat-abi=hard` : *Specifies which floating-point ABI to use. **hard** allows generation of floating-point instructions and uses FPU-specific calling conventions.*
- `-Ixxx` : *Contains _profile_timer_hw.h vectors.h xcpu_cortexa9.h xemacps.h xgpio_i.h      xil_cache_vxworks.h  xil_macroback.h xil_... etc.*
- `-MMD` : *Like -MD except mention only user header files, not system header files.*
- `-MP` : *This option instructs CPP to add a phony target for each dependency other than the main file, causing each to depend on nothing. These dummy rules work around errors **make** gives if you remove header files without updating the Makefile to match.*
- `-MF"src/helloworld.d"` : *When used with -M or -MM, specifies a file to write the dependencies to. If no -MF switch is given the preprocessor sends the rules to the same place it would send preprocessed output. When used with the driver options -MD or -MMD, -MF overrides the default dependency output file. If file is -, then the dependencies are written to stdout.*
- `-MT"src/helloworld.o"` : *Change the target of the rule emitted by dependency generation. By default CPP takes the name of the main input file, deletes any directory components and any file suffix such as .c, and appends the platform's usual object suffix. The result is the target.*

Ok... the -Mxxx are a bit vague, yet probably useful, like for instance the content of *src/helloworld.d* : 
```
src/helloworld.o src/helloworld.o: ../src/helloworld.c ../src/platform.h \
 ../src/platform_config.h \
 /[...]/HW/Vitis/Zybo_GPIO_led_sws_INT_btn/export/Zybo_GPIO_led_sws_INT_btn/sw/Zybo_GPIO_led_sws_INT_btn/standalone_ps7_cortexa9_0/bspinclude/include/xil_printf.h \
 /[...]/include/xil_types.h \
 /[...]/include/bspconfig.h \
 /[...]/include/xparameters.h \
 /[...]/include/xparameters_ps.h
../src/platform.h:
../src/platform_config.h:
/[...]/include/xil_printf.h:
/[...]/include/xil_types.h:
/[...]/include/bspconfig.h:
/[...]/include/xparameters.h:
/[...]/include/xparameters_ps.h:
```

It's headers, not going to spend too much time on something that is not code. Compiling `platform.c` is the same. 

But Kernel.elf is different:

`arm-none-eabi-gcc -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -Wl,-build-id=none -specs=Xilinx.spec -Wl,-T -Wl,../src/lscript.ld -L/[...]/Hype/HW/Vitis/Zybo_GPIO_led_sws_INT_btn/export/Zybo_GPIO_led_sws_INT_btn/sw/Zybo_GPIO_led_sws_INT_btn/standalone_ps7_cortexa9_0/bsplib/lib -o "Kernel.elf" ./src/helloworld.o ./src/platform.o   -Wl,--start-group,-lxil,-lgcc,-lc,--end-group`

New options:

- `-Wl,xxx` : Linker options (`man arm-none-eabi-ld`)
- `-Wl,-build-id=none` : *Request the creation of a ".note.gnu.build-id" ELF note section or a ".buildid" COFF section. [...] Passing "none" for style disables the setting from any "--build-id" options earlier on the command line.*
- `-specs` : *Process  file  after  the  compiler  reads  in  the  standard  specs file, in order to override the defaults which the gcc driver program uses when determining what switches to pass to cc1, cc1plus, as, ld, etc.  More than one -specs=file can be specified on the command line, and they are processed in order, from left to right.*
- `-Wl,-T` + `-Wl,../src/lscript.ld ` : *Use  **scriptfile**  as  the  linker script.*
- `-Lxxx` : FINALLY ! `.o` and `.d` 
- `-Wl,--start-group,-lxil,-lgcc,-lc,--end-group` : *The archives should be a list of archive files.  They may be either explicit file names, or -l options. The specified archives are searched repeatedly until no new undefined references are created.  Normally, an archive is searched only once in the order that it is specified on the command line.  If a symbol in that  archive  is needed  to resolve an undefined symbol referred to by an object in an archive that appears later on the command line, the linker would not be able to resolve that reference.  By grouping the archives, they will all be searched repeatedly until all possible references are resolved. Using this option has a significant performance cost.  It is best to use it only when there are unavoidable circular references between two or more archives.*

Well. That's good! I'm glad I took the time to look at this, and found the .o related to all the low level functions specific to the target. Now that's a lot of ingredients, lots of absolute paths. But it's getting a bit clearer ! What is where, when is what used.

## Mon make à moi

So, let's set asside all the .o, .h, lscript.ld, Xilinx.spec and here we go ! Take an old Makefile and... 

Here it is ! [Hype has a folder](../Hype/) ! 

```
===================================
==========   HYPE v0.0   ==========
===================================


Some registers :
> TTBR0 0x0000405b
> TTBR1 0x00010000
> TTBCR 0x00000000


			Bisous
```

Todo : investigate linker warning `warning: obj/boot.elf has a LOAD segment with RWX permissions`
