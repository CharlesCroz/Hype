#include "syscall.h"
/*%%CALLBACK_INCLUDES%%*/


const int (*__syscall_callbacks[])(void *, void *) = {
/*%%CALLBACK_LIST%%*/
};
const int __syscall_callbacks_count = /*%%CALLBACK_COUNT%%*/;
