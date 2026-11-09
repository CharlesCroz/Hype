
#include "kprint.h"


void handle_svc(int SYSCALL_ID){
    _kprint("SYSCALL_ID:");
    _kprint_i32dec(SYSCALL_ID);
    _kprint("\n\r");

    return;
};
