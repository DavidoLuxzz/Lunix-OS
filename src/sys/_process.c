#include <sys/process.h>
#include <util.h>

process_t ___current_process;

void proc_set_current(void* p){
    memcpy(&___current_process, p, sizeof(process_t));
}

process_t* proc_get_current(void){
    return &___current_process;
}
