#include <sys/syscall.h>
#include <stdio.h>

void *__syscall_functions[__NUM_SYSCALLS];
uint8_t __syscall_argc[__NUM_SYSCALLS];

void init_system_calls(){
    // first set all syscall functions to zero
    memset(__syscall_functions, 0, __NUM_SYSCALLS*sizeof(int*));
    // then initialize them
    __syscall_functions[0] = __newproc;
    // later initialize syscall functions arguments count
    memset(__syscall_argc, 0, __NUM_SYSCALLS);
    // init argc-s
    __syscall_argc[0] = __SYSCALL_newproc_ARGC;
}

void __exec_sys_call(struct InterruptRegisters* regs){
    uint32_t eax = regs->eax;
    if (eax>=__NUM_SYSCALLS) asm volatile ("int $20");
    void (*fn)();
    fn = __syscall_functions[eax];
    if (!fn) asm volatile ("int $20");
    printf("syscall valid! eax=%u\n", eax);
    // execution
    uint8_t argc = __syscall_argc[eax];
    /*
    if (!argc){
        printf("Executing system call with no arguments\n");
        fn();
        return;
    }
    */
    uint32_t a1 = regs->edi;
    uint32_t a2 = regs->esi;
    uint32_t a3 = regs->edx;
    uint32_t a4 = regs->ebx;
    uint32_t a5 = regs->ecx;
    fn(a1,a2,a3,a4,a5);
    /*
    switch(argc){
        case 1:
            fn(a1);
            return;
        case 2:
            fn(a1, a2);
            return;
        case 3:
            fn(a1, a2, a3);
            return;
        case 4:
            fn(a1, a2, a3, a4);
            return;
        default:
            fn(a1, a2, a3, a4, a5);
            return;
    }
    */
}

SCDEF(bool, newproc, int a) {
    printf("creating new process %u\n", a);

    return 0;
}



