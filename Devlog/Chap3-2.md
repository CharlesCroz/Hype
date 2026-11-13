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

```
kernel/drivers/
├── export
│   ├── includes
│   └── lib
├── gpio
├── time
└── uart
    ├── include
    ├── obj
    └── src
```

And the config file evolved a bit :

```yaml
drivers:
  headers:
    - uart.h
    # - time.h
    # - gpio.h
  callbacks:
    - name: uart_send
      id: 0
    # - name : uart_receive
    #   id: 1 
    # - name : get_time
    #   id: 2 
    # - name : btn_read
    #   id: 3 
```

And it generates thing like these...
```c
#include "syscall.h"
#include "uart.h"


const int (*__syscall_callbacks[])(void *, void *) = {
    (syscall_callback) uart_send,

};
const int __syscall_callbacks_count = 1;
```
And...
```c
#ifndef HYPE_SYSCALL_ID_PROT
#define HYPE_SYSCALL_ID_PROT

#define SYSCALL_UART_SEND 0

#endif
```

These functions are registered into a nicely decorated handler :
```c
__attribute__ ((target("general-regs-only"), interrupt("SWI"))) 
int handle_syscall(int SYSCALL_ID, void *p1, void *p2){
    int is_defined;
    
    is_defined = (SYSCALL_ID < __syscall_callbacks_count) && (__syscall_callbacks[SYSCALL_ID] != 0);

    if(is_defined){
        _kprintf("SYSCALL_ID:%d\n\r", SYSCALL_ID);
        return __syscall_callbacks[SYSCALL_ID](p1, p2);
    }else{
        _kprintf("SYSCALL_ID:%d-undef\n\r", SYSCALL_ID);
    }
    
    return -1;
};
```

And how do you call the handler you may ask ? Well, simply (depending on your callback count of parameters) :
```c
int __syscall0(int SYSCALL_ID);
int __syscall1(int SYSCALL_ID, void *p1);
int __syscall2(int SYSCALL_ID, void *p1, void *p2);
```

For instance :
```c
    const char hello[] = "Hello\n\r";
    __syscall2(SYSCALL_UART_SEND, (void *)hello, (void *)sizeof(hello));
```

So, from this `__syscall2` call, we climb up to the handler calling `__syscall_callbacks[0]`, aka `uart_send` with the arguments, casted as `void *` for genericity. Anyway, the arguments will be passed as registers `r0` and `r1`, as per arm function call convention. And then, from the drivers, we have this :
```c
int uart_send(char *text, unsigned int n_char);
```
Bim, profit !

Some makefiles still misbehave a bit and don't take into account some recently compiled artifacts... make clean and make again does the trick !