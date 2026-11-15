# Chapitre 3 - Hyp-0, Part 3

I think I have some drivers (that I may rework a bit later). But, elf files

## Inspecting some onboard ELF file

In the previous section, I described the elf general header. Well, let's download an lef file to the board, and use Hyp-0 to inspect it.

Now, the clean way to do it will probably be to package someway, and tell the kernel where it is stored. For now... I've chosen to just use `xsdb` to download my `boot.elf` as raw data at address 0x100000 in `JTAG_prog.tcl`

```tcl
# Send actual ELF to memory (using CPU0, I guess)
targets -set -nocase -filter {name =~ "*A9*#0"}
dow ./kernel/boot/obj/boot.elf
configparams force-mem-access 0

dow -data {./kernel/boot/obj/boot.elf} 0x100000

# Starts CPU0
con
```

And just casting that address as apointer to a header struct, I get some display !

```c
void print_elf_header(struct elf_header *header){
    _kprintf("ELF file located at 0x%p\n\r", (unsigned int)header);
    _kprintf("e_ident[0:3] magic [0x%x]%c%c%c\n\r", (int)header->e_ident[0], header->e_ident[1], header->e_ident[2], header->e_ident[3]);
    _kprintf("e_ident[4] class   0x%x (1:32bits, 2:64bits)\n\r", (int)header->e_ident[4]);
    _kprintf("e_type             0x%x\n\r", (int)header->e_type);
    _kprintf("e_machine          0x%x\n\r", (int)header->e_machine);
    _kprintf("Program e_phoff    0x%p\n\r", (int)header->e_phoff);
    _kprintf("Section e_shoff    0x%p\n\r", (int)header->e_shoff);
}

int main(){
    /* ... */
    struct elf_header *header = (struct elf_header *) 0x100000;
    print_elf_header(header);
}
```

And that... works !

![ELF Header](./images/Chap_3_3_ELF0.png)

So, let's check program and sections headers, and voila !

![ELF Program and Section Headers](./images/Chap_3_3_ELF1.png)

## Recipe vs result

Before I try to load a partition elf, I want to compare what I understand from the ELF format with what xsdb does when it runs the command to download the elf file to the card.

Wikipedia entry describes the program header section as follows : _The program header table tells the operating system how to create a process image._ Let's start there. A program section can be of various types, but we just have 0x70000001 and 0x00000001, which are respectively an ARM specific program related to exception handling and a loadable segment. Let's treat them both as Loadable for now.

Program entry 0 tells us that starting offset 0x00009180 in the elf file should be loaded at virtual/physical address 0x00008180, 8 bytes into 8 bytes of memory (no zero-padding).

Program entry 1 tells us that starting offset 0x00001000 in the elf file should be loaded at virtual/physical address 0x00000000, 0x00008190 bytes into 0x000e5d0 bytes of memory (zero-padded).

So, we'll ignore the first entry, and compare the content of program entry 1 to what is present in the ram.

![ELF RAM compare](./images/Chap_3_3_ELF_Compare.png)

So, looks bright! The loaded section is the same as what's in the RAM! I thought they would be some difference, but nope! Good! The rest... I don't know if xsdb zeroed it and it got modified during the execution, or if it comes from startup ram noise. Meh, the first part is actually already a victory! 



