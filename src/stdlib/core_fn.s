    section .text
    global  _pop
_pop:
    pop eax
    ret

    global  _getip
_getip:
    mov eax, [esp]
    ret

    global _fire_syscall
_fire_syscall:
    xor eax, eax
    int 0x80
    ret
