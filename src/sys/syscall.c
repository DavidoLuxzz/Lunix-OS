#include <sys/syscall.h>
#include <stdio.h>
#include <sys/process.h>
#include <string.h>
#include <unistd.h>

void *__syscall_functions[__NUM_SYSCALLS];
uint8_t __syscall_argc[__NUM_SYSCALLS];

void __nosys(){}

void init_system_calls(){
    // first set all syscall functions to zero
    memset(__syscall_functions, 0, __NUM_SYSCALLS*sizeof(int*));
    // then initialize them
    __syscall_functions[0] = __nosys;
    __syscall_functions[1] = __execve;
    __syscall_functions[2] = __write;
    // later initialize syscall functions arguments count
    memset(__syscall_argc, 0, __NUM_SYSCALLS); // don't remove (set all to zero)
    // init argc-s
    // __nosys argc already at 0
    __syscall_argc[1] = __SYSCALL_execve_ARGC;
    __syscall_argc[2] = __SYSCALL_write_ARGC;
}

void __exec_sys_call(struct InterruptRegisters* regs){
    uint32_t eax = regs->eax;
/*
    printf("EAX code = %p\n", eax);
    printf("EDX = %p\nEBX = %p\nEBP = %p\nESP = %p\nEIP = %p\n", regs->edx, regs->ebx, regs->ebp, regs->esp, regs->eip);
*/
    void (*fn)();
    fn = __syscall_functions[eax];
    if (eax>=__NUM_SYSCALLS || !fn) {
        printf("invalid system call! (EIP = %p)\n", regs->eip);
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
#include <fs/lxfs.h>
#include <sys/elf.h>
extern LXFS_DIR currentDir;
SCDEF(bool, execve, const char* fname) {
    LXFS_ENTRY* e = lxfs_open_file(&currentDir, fname);
    
    uint32_t lba = lxfs_get_data_lba(e->lba);
    uint32_t size = e->size;

    lxfs_close_file(e);
    if (!size) {
        printf("error: size=0\n");
        return 6;
    }
    // ELF stuff
    char* bytes = kmalloc(size * 512);
    if (!bytes) { logerr("kmalloc error"); return 6; }
    ata_disk_read(0, lba,size, bytes);
    uint32_t s_num = elf_get_num_segments(bytes);
    elf_prog_data *pd = (elf_prog_data*) kmalloc(sizeof(elf_prog_data) + (s_num)*sizeof(elf_segment));
    if (elf_load_raw(bytes, pd)){
        kfree(bytes); return 1;
    }

    // create space for new program
    char* program_memory = kmalloc(pd->memsz);
    // now for each segment load it into memory
    for (int i=0; i<pd->s_num; i++){
        elf_segment seg = pd->s_data[i];
        // printf("Loaded segment from foff=%p to off=%p with memsz=0x%x\n",seg.s_foffset, seg.s_offset, seg.s_memsz);
        memcpy(program_memory+seg.s_offset, bytes+seg.s_foffset, seg.s_memsz);
    }
    kfree(bytes);
    kfree(pd);
    // ELF program data is now loaded
    // next create a process
    process_t newp;
    newp.stat = P_ST_RUNNING;
    memset(newp.name, 0, 23);
    strcpy(newp.name, fname);
    newp.addr = (void*)program_memory;
    newp.p_vaddr = pd->p_vaddr;
    
    proc_set_current(&newp);
    // then load a program into memory
    // it doesn't affect the workflow if you load the program before creating a process
    void (*_where)() = (void*) (program_memory+pd->entry);
    _where();

    kfree(program_memory);
    return 0;
}
SCDEF(void, write, const char* _str, size_t len) {
    uint32_t memfix = (uint32_t)proc_get_current()->addr - proc_get_current()->p_vaddr;
    // printf("_str: %p\nmemfix: %u\n", _str, memfix);
    const char* str = _str + memfix;
    // printf("write called!\nstr: %p\nlen=%u\n", str, len);
    // printf("first value = %x (%c)\n", *str&0xFF, *str&0xFF);
    write(str, len);
}





