#include <stdio.h>
#include <vga.h>
#include <util.h>
#include <unistd.h>
#include <terminal.h>
#include <string.h>

void putc(char c){
    putchar(c);
}

void puts(const char* _s){
    register const char* s = _s;
    while (*s){
        putc(*s);
        s++;
    }
    s = NULL;
}

void printb32(uint32_t n){
    char nstr[32];
    memset(nstr, '#', 32);
    for (uint32_t i=0; i<32; i++){
        uint32_t bit = getbit(n, 31-i);
        if (bit) nstr[i] = '1';
        else nstr[i] = '0';
    }
    // todo write(3) from unistd.h
    write(nstr, 32);
}

const char* HEXADECIMAL_CHARS = "0123456789abcdef";

void printx32(uint32_t n){
    // hexadecimal
    uint32_t len=0;
    char nstr[8];
    memset(nstr, '#', 8); // debug
    uint32_t in = n; // instance
    for (uint32_t i=0; i<8; i++){
        uint32_t qtr = (in>>(i*4)) & 0x0000000F; // get down quarter by shifting number
        if (qtr) len=i;
        nstr[7-i] = HEXADECIMAL_CHARS[qtr];
    }
    ++len;
    write(nstr+8-len, len);
}
void printptr(void* n){
    // hexadecimal
    write("0x", 2);
    uint32_t len=0;
    char nstr[8];
    memset(nstr, '#', 8); // debug
    uint32_t in = (uint32_t)n; // instance
    for (uint32_t i=0; i<8; i++){
        uint32_t qtr = (in>>(i*4)) & 0x0000000F; // get down quarter by shifting number
        if (qtr) len=i;
        nstr[7-i] = HEXADECIMAL_CHARS[qtr];
    }
    ++len;
    write(nstr+8-len, len);
}

void printu(uint32_t n){
    // decimal
    uint32_t len=0, ndig=1;
    char nstr[10];
    memset(nstr, '#', 10);
    for (uint32_t i=0; i<10; i++){
        char digit = (n/ndig)%10;
        nstr[9-i] = digit+'0';
        if (digit) len=i;
        ndig*=10;
    }
    ++len;
    write(nstr+10-len, len);
}

#include <sys/core_asm.h>
void printf(const char* _fmt, ...){
    int* argp = (int*) &_fmt;
    argp++;
    const char* fmt = _fmt;
    while (*fmt){
        switch(*fmt){
            case '%': // argument format
                __putval(*(++fmt), argp);
                ++argp;
                if (*fmt == 0) return;
                break;
            case '^': // color format
                      // NOTE that color format doesn't pop an argument,
                      // it just sets the color specified after the '^' (ex. "^0")
                uint16_t _col_chr = *(++fmt);
                if (_col_chr == '_'){
                    __vga_set_text_color(7);
                    break;
                }
                uint16_t _col;
                bool _found_col = false;
                for (_col=0; _col<16; _col++){
                    if (HEXADECIMAL_CHARS[_col] == _col_chr){
                        __vga_set_text_color(_col);
                        _found_col = true;
                        break;
                    }
                }
                if (!_found_col) putchar(_col_chr);
                break;
            default:
                putchar(*fmt);
                break;
        }
        fmt++;
    }
}

void __putval(char fmt, int* ptr){
    uint32_t val = *ptr;
    switch(fmt){
        case 'z':
        case 'u': // unsigned int
            printu(val);
            break;
        case 'X': // todo uppercase hexadecimal
        case 'x': // hex unsigned int
            printx32(val);
            break;
        case 'd': // signed int
            printu(val); // todo printi
            break;
        case 'c':
            putchar(val);
            break;
        case 'b':
            printb32(val);
            break;
        case 'p':
            printptr((void*)val);
            break;
        case 's':
            print((const char*)val);
            break;
        default: // when the format is invalid
            break;
    }
}

void read(char *dest){
    while (!__term_is_stdin_flushed()); // wait until stdin is flushed
    strncpy(dest, __term_get_and_update_stdin(), TERMINAL_MAX_BUFFER_LENGTH);
}

void nread(char *dest, size_t len){
    while (!__term_is_stdin_flushed());
    strncpy(dest, __term_get_and_update_stdin(), len);
}




