#include <unistd.h>
#include <vga.h>

void write(const char* s, size_t len){
    for (uint32_t i=0; i<len; i++){
        putchar(s[i]);
    }
}
