#include "kprint.h"
#include "gic.h"
#include "tic.h"
#include "syscall.h"
#include "uart.h"
#include "time.h"
#include "partitions_configuration.h"
#include "elf.h"
#include "drivers.h"

#define REG_LED *((volatile int *) 0x40000000)

void key(void *arg){
    _kprintf("Key %d pressed\n\r", (int) arg);
    REG_LED = REG_LED ^ (1 << (int) arg);
}


struct scheduler_status {
    unsigned int current_ms;
    unsigned int current_slot;
} scheduler_status;

void scheduler(void *param){
    struct scheduler_status *sched_status = (struct scheduler_status *) param;

    sched_status->current_ms += 1;
    if(sched_status->current_ms == hype_scheduling[sched_status->current_slot].to_ms){
        sched_status->current_slot++;
        if(sched_status->current_slot == hype_scheduling_count){
            sched_status->current_slot = 0;
            sched_status->current_ms   = 0;
        }
        // _kprintf("\033[13;25H%s   ", hype_partitions[hype_scheduling[sched_status->current_slot].partition_id].name);
    }
}


void print_elf_header(const struct elf_header *header, int is_short){
    _kprintf("ELF file located at 0x%p\n\r",                     (unsigned int) header);
    if(!is_short){
        _kprintf("e_ident[0:3] magic [0x%x]%c%c%c\n\r",           (int) header->e_ident[0], header->e_ident[1],
                 header->e_ident[2], header->e_ident[3]);
        _kprintf("e_ident[4] class   0x%x (1:32b, 2:64b)\n\r", (int) header->e_ident[4]);
        _kprintf("e_type             0x%x\n\r",                      (int) header->e_type);
        _kprintf("e_machine          0x%x\n\r",                      (int) header->e_machine);
        _kprintf("e_entry            0x%p\n\r",                      (int) header->e_entry);
        _kprintf("Program e_phoff    0x%p\n\r",                      (int) header->e_phoff);
        _kprintf("Section e_shoff    0x%p\n\r",                      (int) header->e_shoff);
        _kprintf("e_shstrndx         0x%p\n\r",                      (int) header->e_shstrndx);
    }


    const struct elf_pheader *pheaders = (const struct elf_pheader *) ((unsigned int) header
                                                                       + (unsigned int) header->e_phoff);

    for(int i = 0 ; i < header->e_phnum; i++){
        _kprintf("Program header entry %i\n\r", i);
        _kprintf("\tp_type   0x%p",             pheaders[i].p_type);
        _kprintf("  p_offset 0x%p\n\r",         pheaders[i].p_offset);
        _kprintf("\tp_vaddr  0x%p",             pheaders[i].p_vaddr);
        _kprintf("  p_paddr  0x%p\n\r",         pheaders[i].p_paddr);
        _kprintf("\tp_filesz 0x%p",             pheaders[i].p_filesz);
        _kprintf("  p_memsz  0x%p\n\r",         pheaders[i].p_memsz);
        _kprintf("\tp_flags  0x%p",             pheaders[i].p_flags);
        _kprintf("  p_align  0x%p\n\r",         pheaders[i].p_align);
    }

    if(!is_short){
        const struct elf_sheader *sheaders = (const struct elf_sheader *) ((unsigned int) header
                                                                           + (unsigned int) header->e_shoff);
        const char *section_names = (char *) ((uint32_t) header + (uint32_t) sheaders[header->e_shstrndx].sh_offset);
        _kprintf("Section names at 0x%p\r\n", sheaders[header->e_shstrndx].sh_offset);
    
        for(int i = 0 ; i < header->e_shnum; i++){
            _kprintf("Section header %i : %s",
                     i, &section_names[sheaders[i].sh_name]);
            _kprintf("\tsh_type 0x%x, sh_size 0x%x, sh_offset 0x%x\n\r",
                     sheaders[i].sh_type, sheaders[i].sh_size, sheaders[i].sh_offset);
        }
    }
} /* print_elf_header */


void compare_elf(struct elf_header *header, const uint8_t *ref){
    const struct elf_pheader *pheaders = (const struct elf_pheader *) ((unsigned int) header
                                                                       + (unsigned int) header->e_phoff);

    const struct elf_pheader *prog = &pheaders[1];

    int was_same        = 1;
    int diff_count      = 0;
    int first_diff      = 0;
    const char *segment = (const char *) ((uint32_t) header + prog->p_offset);

    _kprintf("Comparing 0x%x bytes at 0x%p\r\n", prog->p_filesz, ref);
    for(int i = 0; i < prog->p_filesz; i++){
        if(segment[i] != ref[i]){
            diff_count++;
        }
        if(segment[i] != ref[i] && was_same){
            was_same   = 0;
            first_diff = i;
        } else if(segment[i] == ref[i] && !was_same){
            _kprintf("At 0x%p, diff of %d bytes\r\n", first_diff, diff_count);
            was_same   = 1;
            diff_count = 0;
        }
    }
    if(!was_same){
        _kprintf("At 0x%p, diff of %d bytes\r\n", first_diff, diff_count);
    }

    _kprintf("Checking 0x%x zero bytes at 0x%p\r\n", (prog->p_memsz - prog->p_filesz), (void *) prog->p_filesz);
    for(int i = prog->p_filesz; i < prog->p_memsz; i++){
        if(0 != ref[i]){
            diff_count++;
        }
        if(0 != ref[i] && was_same){
            was_same   = 0;
            first_diff = i;
        } else if(0 == ref[i] && !was_same){
            _kprintf("At 0x%p, %d non-zero bytes\r\n", first_diff, diff_count);
            was_same   = 1;
            diff_count = 0;
        }
    }
    if(!was_same){
        _kprintf("At 0x%p, %d non-zero bytes\r\n", first_diff, diff_count);
    }
} /* compare_elf */


void * load_elf(const struct elf_header *elf){
    const struct elf_pheader *pheaders = (const struct elf_pheader *) ((unsigned int) elf
                                                                       + (unsigned int) elf->e_phoff);

    for(int i = 0; i < elf->e_phnum; i++){
        if(pheaders[i].p_type == PT_LOAD){
            const uint8_t *code_src = (const uint8_t *) ((unsigned int) elf + pheaders[i].p_offset);
            uint8_t *code_dst       = (uint8_t *) (pheaders[i].p_paddr);

            _kprintf("Loading elf prog[%d] %d bytes at %p\n\r", i, pheaders[i].p_filesz, code_dst);

            for(int byte = 0; byte < pheaders[i].p_filesz; byte++){
                code_dst[byte] = code_src[byte];
            }
        }
    }

    return (void *) elf->e_entry;
}

int check_magic(const char *target){
    const char magic[] = "Hyp0Part";
    const char *ptr = magic;

    while(*ptr){
        if(*ptr != *target){
            return 0;
        }
        ptr++;
        target++;
    }
    return 1;
}

int hackjump(void *addr);

int main(){
    __uart_init();
    __time_init();

    _kprintf("\033[2J\033[0;0H"
             "===================================\n\r"
             "==========   HYPE v0.0   ==========\n\r"
             "===================================\n\r");

    _kprintf("Partitions (%i)\n\r", hype_partitions_count);
    for(int i = 0; i < hype_partitions_count; i++){
        _kprintf(" - %s\n\r", hype_partitions[i].name);
    }

    _kprintf("\n\n\r");

    gic_activate_irq(61, 3, key, (void *) 0);
    gic_activate_irq(62, 3, key, (void *) 1);
    gic_activate_irq(63, 3, key, (void *) 2);
    gic_activate_irq(64, 3, key, (void *) 3);


    scheduler_status = (struct scheduler_status) {
        .current_ms   = hype_scheduling[hype_scheduling_count].to_ms - 1,
        .current_slot = hype_scheduling_count - 1
    };
    tic_setup(1, scheduler, &scheduler_status);
    gic_enable();

    /* Wait for UART TX to be empty before we start messing with things */
    while(((*(volatile unsigned int *) 0xE000102C) & (1 << 3)) == 0){
        /* nop */
    }

    struct partition_header *p_header = (struct partition_header *) 0x100000; 

    if(check_magic(p_header->magic)){
        
        _kprintf("Exploring partition binary at 0x%p, containing %i partitions\n\r",
                 p_header, p_header->n_entries);
    
        for(int i = 0; i < p_header->n_entries; i++){
            const char *name = (const char *) ((unsigned int) p_header + p_header->entries[i].name_offset);
            _kprintf("=================\n\r=== Partition[%d] '%s' binary at 0x%p, 0x%x bytes ===\n\r",
                     i, name, p_header->entries[i].elf_offset, p_header->entries[i].elf_size);
            const struct elf_header *elf_header = (const struct elf_header *)((unsigned int) p_header + p_header->entries[i].elf_offset);
            print_elf_header(elf_header, 1);
            _kprintf("=================\n\r\n\r");
        }
    
    
        for(int i = 0 ; i < p_header->n_entries; i++){
            void *entrypoint;
            volatile int retval;
            struct {
                unsigned int sec;
                unsigned int micro_sec;
            } t_start, t_end, t_diff;
            const struct elf_header * elf_header;
            const char *name;

            elf_header = (const struct elf_header *)((unsigned int) p_header + p_header->entries[i].elf_offset);
            name = (const char *) ((unsigned int) p_header + p_header->entries[i].name_offset);
    
            time_get(&t_start.sec, &t_start.micro_sec);
            
            _kprintf("Running partition[%i]:%s\n\r", i, name);
            entrypoint = load_elf(elf_header);
            _kprintf("Ready to jump to code at 0x%p\n\r", entrypoint);
        
            retval = hackjump(entrypoint);
            
            
            time_get(&t_end.sec, &t_end.micro_sec);
            
            t_diff.sec = t_end.sec - t_start.sec;
            if(t_end.micro_sec < t_start.micro_sec){
                t_end.micro_sec += 1000000;
                t_end.sec--;
            }
            t_diff.micro_sec = t_end.micro_sec - t_start.micro_sec;
            
            _kprintf("Done, execution return value is %d\n\r", retval);
            _kprintf("Execution lasted %d s and %d us\n\r", t_diff.sec, t_diff.micro_sec);
        }
    }else{
        _kprintf("Partition Binary not found\n\r");
    }


    return 0;
} /* main */
