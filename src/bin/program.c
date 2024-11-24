#include <stdio.h>
#include <vga.h>

int main(){
    *(char*)0xb8224 = '*';
    move_cursor(0, 20);
    printf("a nigga");
    return 0;
}
