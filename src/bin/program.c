#include <lxstd.h>

int main(){
    *(char*)0xb8224 = '*';
    sync0();
    printf("Hello from program.c\n");
    printf("2 = %b\n3 = %b\n-2 = %b\n-3 = %b\n", 2, 3, -2, -3);
    return 0;
}
