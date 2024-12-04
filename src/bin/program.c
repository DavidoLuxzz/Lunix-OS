#include <lxstd.h>

int main(){
    *(char*)0xb8224 = '*';
    sync0();
    printf("Hello from program.c\n");
    return 0;
}
