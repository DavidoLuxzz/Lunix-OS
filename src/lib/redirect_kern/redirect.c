#include <stdint.h>
#include <util.h> // asif(void*, void*)

#define FDEC(type, name, ...) type (*name)(__VA_ARGS__)

FDEC(void,      printf,     const char*, ...);


const char* __test_text = "*Ovaj tekst je iz libredir.a";

// synchronize function (syscall) //
// !! warning !! //
// this function uses intel syntax. to compile this code
// use gcc option -masm=intel
//
// void sync0() { ...  (* defined in redirasm.s *)


