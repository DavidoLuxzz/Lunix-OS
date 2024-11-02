#pragma once
#include <stdint.h>

void putc(char c);
void puts(const char* str);
void printb32(uint32_t);
void printx32(uint32_t);
void printptr(void*);
void printu(uint32_t);
void __putval(char, int*);

void printf(const char*, ...);

void read(char *dest);
void nread(char* dest, size_t len);

extern void print(const char*);
extern void x86_div64_32(uint64_t, uint32_t, uint64_t*, uint32_t*);
