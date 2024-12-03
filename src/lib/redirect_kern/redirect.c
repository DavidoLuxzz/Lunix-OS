#include <stdint.h>
#include <util.h> // asif(void*, void*)

#define FDEC(type, name, ...) type (*name)(__VA_ARGS__)

FDEC(void,      printf,     const char*, ...);
