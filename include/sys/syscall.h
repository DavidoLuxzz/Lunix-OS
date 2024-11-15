/** -- LunixOS system calls for 32-bit x86 architecture --
 * -> Sun Nov 3 21:55
 *  
 * Arguments are passed by registers in order:
 *  1 - edi,
 *  2 - esi,
 *  3 - ebx,
 *  4 - edx,
 *  5 - ecx
 * 
 * The return value is contained in EAX register.
 */

#pragma once
#include <stdint.h>
#include <util.h>

#define __NUM_SYSCALLS  32
void init_system_calls(void);
void __exec_sys_call(struct InterruptRegisters*);

#define SCDEC(argc, type, name, ...) static const int __SYSCALL_##name##_ARGC = argc; \
                                        type __##name (__VA_ARGS__);
#define SCDEF(type, name, ...) type __##name (__VA_ARGS__)

/* Creates new process */
//   argc  fn_type  fn_name     parameters
SCDEC(0,    void,   nosys,      void)
SCDEC(1,    bool,   exec,    const char*)
/* Write system call:
 *  When calling write syscall from a process, it firstly converts the relative address passed as the string parameter.
 *  Note: that a relative address is the address that an ELF linker would put some value. The linker often uses 0x8048000 as the begin of the program virtual address.
 *  As Lunix does not use virtual addresses (paging), that relative address should be converted into absolute address.
 * So it does that and only then it does its job
 */
SCDEC(2,    void,   write,      const char*, size_t)





