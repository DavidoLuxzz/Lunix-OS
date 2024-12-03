    section .text    
    global  sync0
sync0:
    push    ebp
    mov     ebp,    esp
    
    ; syscall no.
    mov     eax,    4 ; sync0 system call number
    ; ebx = param 1 (address of the list)
    mov     ebx,    [esp+8]
    int     0x80

    pop     ebp
    ret
