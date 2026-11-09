# Chapitre 3 - Hyp-0, Part 2

How serious am I for this project? I'm Elf-serious! I know these days it may refer to [Kokiris](https://en.wikipedia.org/wiki/The_Legend_of_Zelda:_Ocarina_of_Time_(2026_video_game)), but it's not my kind of elfs... I'm talking [Executable and Linkable Format
](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)!

## Why ELF?

I could just compile functions, decorate them a bit at kernel compile time, and have the kernel start the functions... But where's the fun in that?

An elf file has some critically important metadata to validate and setup the execution of a program, including but not limited to processor architecture, endianness, float handling, and so on. And then, of course, the layout of the application.

## Elf header

Wikipedia describes the structure of an elf header, and [linux man pages](https://man7.org/linux/man-pages/man5/elf.5.html) shows some C struct example. From this...

```C
#define EI_NIDENT 16

struct elf_header {
    uint8_t     e_ident[EI_NIDENT];
    uint16_t    e_type;
    uint16_t    e_machine;
    uint32_t    e_version;
    uint32_t    e_entry;
    uint32_t    e_phoff;
    uint32_t    e_shoff;
    uint32_t    e_flags;
    uint16_t    e_ehsize;
    uint16_t    e_phentsize;
    uint16_t    e_phnum;
    uint16_t    e_shentsize;
    uint16_t    e_shnum;
    uint16_t    e_shstrndx;
};
```

The fields `e_ident`, `e_type`, `e_machine`, `e_version`, `e_ehsize`, `e_phentsize`, `e_shentsize`, `e_shstrndx` can be used for compatibility checks, since their values are known in advance.

The other fields describe sections that will contain executable code, read/write data and read-only data. The sections also contain addressing information, e.g "This chunk of code, located here in the elf file, should be loaded at that memory address, optionally zero-padded, and has these rwx (Read, Write, eXec) permissions".

So... Let's try a dummy application that... runs and... interracts with the world? Oh no... So first of all, I need a driver. Let's start with... uart_send?

## Interracting with the world

I will do this poorly. It's Hyp-0, it's prototype. I want it to work, I'll make it work efficiently and beautyfully later. And race conditions. [Mutex](https://en.wikipedia.org/wiki/Mutual_exclusion) is a for another day.


What I have in mind, is something like this:
1. Process X:
    1. Wants to print `n_char` characters stored at address `text`
    1. Stores `SEND_UART_ID` in r0, `n_char` in r1, `text` in r2
    1. Raises a software interrupt
1. Kernel compares r0 to `SEND_UART_ID`. If equal, call the drivers handler
1. The handler:
    1. Fills its SW buffer with the `n_char` characters of `text`
    1. Fills the HW UART_TX buffer until HW buffer full or SW buffer empty.
    1. If the SW buffer is empty, done
    1. Else, configure the HW to raise an interrupt when HW send buffer is empty, upon which it repeats 3.2

Seems simple enough, let's see if it works (I'm pretty sure the 3.1 step will be problematic, due to MMU)!

Oh, by the way, let's have a drivers directory tree. It's going to be clean!
