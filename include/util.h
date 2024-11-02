#pragma once
#include <stdint.h>

void memset(void *dest, char val, uint32_t count);
void memcpy(void *dest, void* src, uint32_t countb);

uint8_t getbit(uint32_t, uint32_t);
void outb(uint16_t Port, uint8_t Value);
uint8_t inb(uint16_t port);
void outw(uint16_t, uint16_t);
uint16_t inw(uint16_t);
void insw(uint16_t __port, void *__buf, unsigned long __n);
void outsw(uint16_t __port, const void *__buf, unsigned long __n);
void insl(uint16_t __port, void *__buf, unsigned long __n);
void outsl(uint16_t __port, const void *__buf, unsigned long __n);

#define CEIL_DIV(a,b) (((a + b) - 1)/b)

struct InterruptRegisters{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};
