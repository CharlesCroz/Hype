#ifndef HYPE_CALL
#define HYPE_CALL

int __syscall0(int SYSCALL_ID);
int __syscall1(int SYSCALL_ID, void *p1);
int __syscall2(int SYSCALL_ID, void *p1, void *p2);

#endif
