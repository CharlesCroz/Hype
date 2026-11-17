MEMORY
{
   RAM_allocated : ORIGIN = 0x200000, LENGTH = 0x100000
}

ENTRY(main)

SECTIONS
{
    .text : {
        *(.text)
    } > RAM_allocated
}
