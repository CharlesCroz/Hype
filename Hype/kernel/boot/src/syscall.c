#include "kprint.h"
#include "syscall.h"
#include "uart.h"


extern const syscall_callback __syscall_callbacks[];
extern const int __syscall_callbacks_count;


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

int __syscall0(int SYSCALL_ID){
    return __syscall2(SYSCALL_ID, 0, 0);
}

int __syscall1(int SYSCALL_ID, void *p1){
    return __syscall2(SYSCALL_ID, p1, 0);
}

int __syscall2(int SYSCALL_ID, void *p1, void *p2){
    int res;
    __asm("svc #0");
    __asm("mov %0, r0" : "=r"(res));
    return res;
};
