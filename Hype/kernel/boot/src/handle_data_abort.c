#include "kprint.h"

void handle_data_abort(){
    _kprint("\033[2J\033[5;5HDataAbortHandler : BOOM");
}