#pragma once
#include <stdint.h>
#include <cpuid.h>

extern uint32_t __cpu_check(void);
extern void __dump_stack(void);

/* DEPRECATED: You should use the <cpuid.h> header that comes with GCC instead. */

enum cpuid_requests {
  CPUID_GETVENDORSTRING,
  CPUID_GETFEATURES,
  CPUID_GETTLB,
  CPUID_GETSERIAL,
  
  CPUID_INTELEXTENDED=0x80000000,
  CPUID_INTELFEATURES,
  CPUID_INTELBRANDSTRING,
  CPUID_INTELBRANDSTRINGMORE,
  CPUID_INTELBRANDSTRINGEND,
};
/** issue a single request to CPUID. Fits 'intel features', for instance
 *  note that even if only "eax" and "edx" are of interest, other registers
 *  will be modified by the operation, so we need to tell the compiler about it.
 */
static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
    asm volatile("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
}
    
/** issue a complete request, storing general registers output as a string
 */
static inline int cpuid_string(int code, uint32_t where[3]) {
    int ret;
    asm volatile("cpuid":"=a"(ret),"=b"(*(where+0)),
                 "=c"(*(where+1)),"=d"(*(where+2)):"a"(code));
    return ret;
}















