#include <sys/syscall.h>
#include <stdio.h>

void *__syscall_functions[__NUM_SYSCALLS];
uint8_t __syscall_argc[__NUM_SYSCALLS];

void __nosys(){}

void init_system_calls(){
    // first set all syscall functions to zero
    memset(__syscall_functions, 0, __NUM_SYSCALLS*sizeof(int*));
    // then initialize them
    __syscall_functions[0] = __nosys;
    __syscall_functions[1] = __newproc;
    __syscall_functions[2] = __write;
    // later initialize syscall functions arguments count
    memset(__syscall_argc, 0, __NUM_SYSCALLS); // don't remove (set all to zero)
    // init argc-s
    // __nosys argc already at 0
    __syscall_argc[1] = __SYSCALL_newproc_ARGC;
    __syscall_argc[2] = __SYSCALL_write_ARGC;
}

void __exec_sys_call(struct InterruptRegisters* regs){
    uint32_t eax = regs->eax;
    printf("EAX code = %p\n", eax);
    printf("EDX = %p\nEBX = %p\nEBP = %p\nESP = %p\nEIP = %p\n", regs->edx, regs->ebx, regs->ebp, regs->esp, regs->eip);
    void (*fn)();
    fn = __syscall_functions[eax];
    if (eax>=__NUM_SYSCALLS || !fn) {
        printf("invalid system call!\n");
        return;
    }
    // execution
    // uint8_t argc = __syscall_argc[eax];
    /*
    if (!argc){
        printf("Executing system call with no arguments\n");
        fn();
        return;
    }
    */
    uint32_t a1 = regs->ebx;
    uint32_t a2 = regs->ecx;
    uint32_t a3 = regs->edx;
    uint32_t a4 = regs->edi;
    uint32_t a5 = regs->esi;
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

#include <sys/drivers/disk_h.h>
#include <sys/drivers/disk_ata.h>
#include <sys/kmalloc.h>
SCDEF(bool, newproc, uint32_t lba) {
    printf("creating new process lba=%u\n", lba);
    void (*_where)() = kmalloc(1024);
    ata_disk_read(0, lba, 2, _where);
    asm volatile ("mov %0, %%edx" : "+r"(_where));
    uint32_t _ebx;
    asm volatile ("mov %%edx, %0" : "=r"(_ebx));
    printf("running from %p edx=%p\n", _where, _ebx);
    _where();
    kfree(_where);
    return 0;
}
#include <unistd.h>
SCDEF(void, write, const char* str, size_t len) {
    write(str, len);
}







