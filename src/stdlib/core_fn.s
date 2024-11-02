    section .text
    global  _pop
_pop:
    pop eax
    ret

    global  _getip
_getip:
    mov eax, [esp]
    ret
