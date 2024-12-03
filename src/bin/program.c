#include <lxstd.h>

extern const char* __test_text;

int main(int* asd){
    *(char*)0xb8224 = *__test_text;
    sync0(printf);
    asm volatile ("int $0x80");
    printf("a nigga");
    *asd = 69;
    return 0;
}
