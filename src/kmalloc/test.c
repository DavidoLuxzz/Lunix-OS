#include <kmalloc.h>
#include <stdio.h>

int _kernel_end = 0xC0000000;

int main(){
    kmallocInit();
    kdump_arrays(4);

    void* ptr = kmalloc(33);
    printf("given pointer: %p\n", ptr);
    kdump_arrays(4);

    void* ptr2 = kmalloc(64);
    printf("given pointer2: %p\n", ptr2);
    kdump_arrays(4);

    kfree(ptr);
    kdump_arrays(4);
    
    void* ptr3 = kmalloc(34);
    printf("given pointer3: %p\n", ptr3);
    kdump_arrays(4);

    void* ptr4 = kmalloc(33);
    printf("given pointer4: %p\n", ptr4);
    kdump_arrays(4);
    
    kfree(ptr);
    kfree(ptr2);
    kfree(ptr3);
    kfree(ptr4);
    
    printf("press enter to exit\n");
    getchar();
    return 0;
}
