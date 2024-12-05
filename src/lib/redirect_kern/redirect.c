#include <lib/lxstd.h>
#include <stdint.h>
#include <util.h> // asif(void*, void*)

#define FDEC(type, name, ...) type (*name)(__VA_ARGS__)

FDEC(void,      printf,     const char*, ...);
FDEC(void,      kfree,      void*);
/*
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
FDEC(void,      printf,     const char*, ...);
*/

// synchronize function (syscall) //
// !! warning !! //
// this function uses intel syntax. to compile this code
// use gcc option -masm=intel
//
// void sync0() { ...  (* defined in redirasm.s *)

#define smac(fn,fid) sync0((void**)&fn, fid)

void synca(){
    smac(printf, 0);
    smac(kfree, 1);
    // i tako nastavi sa redjanjem
}


