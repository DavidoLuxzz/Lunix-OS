    section .text    
    extern  printf
    global  sync0
sync0:
    push    ebp
    mov     ebp,    esp
    
    ; syscall no.
    mov     eax,    4 ; sync0 system call number
    ; ebx = param 1 (address of the list)
    mov     ebx,    printf
    int     0x80

    pop     ebp
    ret
    
    global  write
write:
    push    ebp
    mov     ebp,    esp
    ; syscall no.
    mov     eax,    2
    ; ebx = address of buffer
    mov     ebx,    printf
    ; ecx = length of buffer
    mov     ecx,    8
    int     0x80

    pop     ebp
    ret
