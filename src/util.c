#include <util.h>

void memset(void *dest, char val, uint32_t count){
    char *temp = (char*) dest;
    for (; count != 0; count --){
        *temp++ = val;
    }

}

void memcpy(void *dest, const void* src, uint32_t countb){
    char *dtmp = (char*) dest;
    char *stmp = (char*) src;
    for (; countb != 0; countb --){
        *dtmp++ = *stmp++;
    }
}

uint8_t getbit(uint32_t num, uint32_t bit){
    uint32_t m = 0x00000001<<bit;
    return num&m;
}

/*
void outb(uint16_t Port, uint8_t Value){
    asm volatile ("outb %1, %0" : : "dN" (Port), "a" (Value));
}

char inb(uint16_t port){
    char rv;
    asm volatile("inb %1, %0": "=a"(rv):"dN"(port));
    return rv;
}
*/

void outb(uint16_t port, unsigned char value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    unsigned char value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outw(uint16_t port, unsigned short value) {
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    unsigned short value;
    asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void insl(uint16_t __port, void *__buf, unsigned long __n) {
	asm volatile("cld; rep; insl"
			: "+D"(__buf), "+c"(__n)
			: "d"(__port));
}

void outsl(uint16_t __port, const void *__buf, unsigned long __n) {
	asm volatile("cld; rep; outsl"
			: "+S"(__buf), "+c"(__n)
			: "d"(__port));
}


void insw(uint16_t __port, void *__buf, unsigned long __n) {
	asm volatile("cld; rep; insw"
			: "+D"(__buf), "+c"(__n)
			: "d"(__port));
}

void outsw(uint16_t __port, const void *__buf, unsigned long __n) {
	asm volatile("cld; rep; outsw"
			: "+S"(__buf), "+c"(__n)
			: "d"(__port));
}

#include <stdio.h>
void logerr(const char* msg){
    printf("[ ^4ERRN^_ ] %s\n", msg);
}
void logwarn(const char* msg){
    printf("[ ^eWARN^_ ] %s\n", msg);
}
void log(const char* msg){
    printf("[ ^fINFO^_ ] %s\n", msg);
}
