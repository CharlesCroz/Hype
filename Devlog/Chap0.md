# Chapitre 0 - Hello World

Premier objectif : les outils, et un premier Hello World **utilisable** à partir des outils Xilinx. En particulier le GCC "qui va bien" pour pouvoir booter en JTAG ou sur carte SD.

## Projet Vivado

Zybo_GPIO_led_sws_INT_btn.xsa : Dans la PL, GPIO pour les 4 leds et les 4 switchs à l'adresse 0x4000_0000, et les 4 boutons sur le contrôlleur d'interruption [64-61].

## Projet Vitis

Creation du projet avec l'Application Hello World sur le CPU0 à partir du XSA. Compilée et testée avec succès sur Vitis.

A partir de la console Vitis, extraction des commandes tcl à exécuter pour correctement programmer la carte.
Les filtres ont été modifiés pour anonymiser notamment mes cartes (numéro de série du JTAG dans les commandes) et utiliser des chemins relatifs pour les fichiers.
Commande utile dans **xsdb** : `targets -target-properties` (pour comprendre les filtres).

## Détour MMU

Après avoir fait joujou avec **objdump** sur le Kernel.elf généré et un fsbl.elf, j'ai vu que par défaut le linker script envoie le code Kernel dans la DDR (0x10_0000) et le fsbl dans l'OCM (On Chip Memory, 0x0). Première modif de *lscript.ld* réussie.

Mumuse avec **xsdb**, notament les commandes `mr`, `mw` (Memory Read/Write), `rrd`, `rwr` (Register ReaD/WRite), et l'assembleur `mcr` (Move to Coprocessor from Register) et `mrc` (Move to Register from Coprocessor), compilé en ligne pour avoir la valeur en hexa de l'instruction.

Petit outil python de décodage : `python3 ./Tools/decoder.py --help`

Et le *main.c* qui va avec, pour transférer les registres du CP15 (qui gère notamment la mémoire) vers un registres, puis printf la valeur
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

    init_platform();
    print("Hello World\n\r");

    __asm ("mrc p15, 0, %[result], c2, c0, 0" : [result] "=r" (reg) :);
    print_reg("TTBR0", reg);
    
    __asm ("mrc p15, 0, %[result], c2, c0, 1" : [result] "=r" (reg) :);
    print_reg("TTBR1", reg);

    __asm ("mrc p15, 0, %[result], c2, c0, 2" : [result] "=r" (reg) :);
    print_reg("TTBCR", reg);

    cleanup_platform();
    return 0;
}
```

La translation table L1 est à l'addresse 0x4000 (cf. le resultat en console xsdb de `dow .[...]/Kernel.elf`) et mappe les 4Go d'adresse virtuelle vers les mêmes adresses physiques, avec des permissions différentes.

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

Par exemple `00015DE6` (première entrée de la table @0x4000) vs `40000C02` (@0x5000) vs `C0000000` (@0x7000) :
- `00015DE6` : Range `0x0000_0000-0x0010_0000` -> **DDR/OCM** -> S/TEX/AP/Domain/B bits are set
- `40000C02` : Range `0x4000_0000-0x4010_0000` -> **FPGA** -> S/TEX/AP/Domain/B bits are unset
- `C0000000` : Range `0xC000_0000-0xC000_0000` -> **Not in system-level address map** -> Fault

## Projet Vitis (suite)

La sortie du makefile "Application" de Vitis montre qu'il y a trois étapes à ce stade : `helloworld.c -> helloworld.o`, `platform.c -> platform.o` et la création du `Kernel.elf`

Pour le premier, le log donne : 

`arm-none-eabi-gcc -Wall -O2 -c -fmessage-length=0 -MT"src/helloworld.o" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I/[...]/Hype/HW/Vitis/Zybo_GPIO_led_sws_INT_btn/export/Zybo_GPIO_led_sws_INT_btn/sw/Zybo_GPIO_led_sws_INT_btn/standalone_ps7_cortexa9_0/bspinclude/include -MMD -MP -MF"src/helloworld.d" -MT"src/helloworld.o" -o "src/helloworld.o" "../src/helloworld.c"`

Les options :
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

Bon, les -M, c'est flou, mais surement utile, en attendant, le *src/helloworld.d* : 
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

Du header, on avance, mais on cherche encore le code... La compil' du `platform.c` c'est la meme chose. Le Kernel.elf par contre :

`arm-none-eabi-gcc -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -Wl,-build-id=none -specs=Xilinx.spec -Wl,-T -Wl,../src/lscript.ld -L/[...]/Hype/HW/Vitis/Zybo_GPIO_led_sws_INT_btn/export/Zybo_GPIO_led_sws_INT_btn/sw/Zybo_GPIO_led_sws_INT_btn/standalone_ps7_cortexa9_0/bsplib/lib -o "Kernel.elf" ./src/helloworld.o ./src/platform.o   -Wl,--start-group,-lxil,-lgcc,-lc,--end-group`

Les nouvelles options :

- `-Wl,xxx` : Linker options (`man arm-none-eabi-ld`)
- `-Wl,-build-id=none` : *Request the creation of a ".note.gnu.build-id" ELF note section or a ".buildid" COFF section. [...] Passing "none" for style disables the setting from any "--build-id" options earlier on the command line.*
- `-specs` : *Process  file  after  the  compiler  reads  in  the  standard  specs file, in order to override the defaults which the gcc driver program uses when determining what switches to pass to cc1, cc1plus, as, ld, etc.  More than one -specs=file can be specified on the command line, and they are processed in order, from left to right.*
- `-Wl,-T` + `-Wl,../src/lscript.ld ` : *Use  **scriptfile**  as  the  linker script.*
- `-Lxxx` : FINALLY ! `.o` and `.d` 
- `-Wl,--start-group,-lxil,-lgcc,-lc,--end-group` : *The archives should be a list of archive files.  They may be either explicit file names, or -l options. The specified archives are searched repeatedly until no new undefined references are created.  Normally, an archive is searched only once in the order that it is specified on the command line.  If a symbol in that  archive  is needed  to resolve an undefined symbol referred to by an object in an archive that appears later on the command line, the linker would not be able to resolve that reference.  By grouping the archives, they will all be searched repeatedly until all possible references are resolved. Using this option has a significant performance cost.  It is best to use it only when there are unavoidable circular references between two or more archives.*

Bon.

On a des ingrédients un peu en vrac, avec beaucoup de liens absolus.... Mais on commence à savoir où est quoi.

Todo : Faire le tri dans les sources pour extraire "Juste ce qu'il faut". Et Mettre en place la compilation hors de Vitis.
Todo : comprendre comment est générée la table de MMU, et surement d'autres bricoles utiles !
