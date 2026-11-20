MEMORY
{
   RAM_allocated : ORIGIN = 0, LENGTH = /*%%MEM_LEN%%*/
}

ENTRY(main)

SECTIONS
{
    .text : {
        *(.text)
    } > RAM_allocated
    .data : {
        *(.text)
    } > RAM_allocated
    .rodata : {
        *(.text)
    } > RAM_allocated
    .bss : {
        *(.text)
    } > RAM_allocated
}
