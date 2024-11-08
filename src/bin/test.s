    bits 32    ; start with this or NASM will compile it as 16-bit code
    section .text
    global _start
_start:
    ; write syscall
    mov     eax,    0x2
    mov     ebx,    msg
    add     ebx,    edx
    mov     ecx,    13
    int 0x80
    ret

    section .data
msg:    db  "hello niggas", 10, 0
