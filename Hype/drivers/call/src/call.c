#include "call.h"

int __syscall0(int SYSCALL_ID){
    return __syscall2(SYSCALL_ID, 0, 0);
}


int __syscall1(int SYSCALL_ID, void *p1){
    return __syscall2(SYSCALL_ID, p1, 0);
}


int __syscall2(int SYSCALL_ID, void *p1, void *p2){
    int res;

    __asm("svc #0");
    __asm("mov %0, r0" : "=r" (res));
    return res;
};
