#include <sys/syscall.h>
#include <util.h>
#include <stdio.h>

int *__syscall_functions[__NUM_SYSCALLS];

void init_system_calls(){
    // first set all syscall functions to zero
    memset(__syscall_functions, 0, __NUM_SYSCALLS*sizeof(int*));

    printf("newproc id: %u\n", __SYSCALL_newproc_ID);
}

SCDEF(bool, newproc, void) {
    printf("creating new process\n");

    return 0;
}



