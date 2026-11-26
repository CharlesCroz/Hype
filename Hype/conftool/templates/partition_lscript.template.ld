MEMORY
{
   RAM_allocated : ORIGIN = /*%%MEM_ORIG%%*/, LENGTH = /*%%MEM_LEN%%*/
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
