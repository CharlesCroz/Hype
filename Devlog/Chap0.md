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

Todo : Faire le tri dans les sources pour extraire "Juste ce qu'il faut". Et Mettre en place la compilation hors de Vitis.
Todo : comprendre comment est générée la table de MMU, et surement d'autres bricoles utiles !
