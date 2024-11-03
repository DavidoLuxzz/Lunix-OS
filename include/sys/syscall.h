/** LunixOS system calls for 32-bit x86 architecture
 * -> Sun Nov 3 21:55
 *  
 * Arguments are passed by registers in order:
 *  1 - edi,
 *  2 - esi,
 *  3 - edx,
 *  4 - ecx,
 *  5 - ebx
 * 
 * The return value is contained in EAX register.
 */

#pragma once
#include <stdint.h>

#define __NUM_SYSCALLS  32
void init_system_calls(void);

#define SCDEC(id, type, name, ...) static const int __SYSCALL_##name##_ID = id; \
                                        type __##name (__VA_ARGS__);

#define SCDEF(type, name, ...) type __##name (__VA_ARGS__)

/* Creates new process */
SCDEC(0,  bool,     newproc,  void)






