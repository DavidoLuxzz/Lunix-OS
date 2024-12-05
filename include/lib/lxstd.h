/** -- Lunix standard redirect library header --
 * -> Tue Dec 3 20:29
 *
 */
#pragma once
// do not include system headers because they
// won't be avaiable be 3rd part programs

// syscalls
void sync0(void**, unsigned long int);
//void write(void*, unsigned long int);
void write(void);

void synca(void);
#define __EXTERN_FUNC__(type, name, ...) extern type (*name)(__VA_ARGS__)

__EXTERN_FUNC__(void, printf, const char*, ...);
__EXTERN_FUNC__(void, kfree,  void*);


