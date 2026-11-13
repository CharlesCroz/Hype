#ifndef __PROT_SVC
#define __PROT_SVC

#include "syscall_ID.h"

typedef int (* syscall_callback)(void *, void *);

int handle_syscall(int SYSCALL_ID, void *p1, void *p2);

int __syscall0(int SYSCALL_ID);
int __syscall1(int SYSCALL_ID, void *p1);
int __syscall2(int SYSCALL_ID, void *p1, void *p2);

#endif
