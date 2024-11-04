/** LunixOS system calls for 32-bit x86 architecture
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
SCDEC(1,  bool,     newproc,  int)






