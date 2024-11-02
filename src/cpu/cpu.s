; LunixOS CPU information
; Tue 15 Oct 2024
    section .text
    global  __cpu_check
__cpu_check:
    pushfd                      ;Save EFLAGS
    pushfd                      ;Store EFLAGS
    xor dword [esp],0x00200000  ;Invert the ID bit in stored EFLAGS
    popfd                       ;Load stored EFLAGS (with ID bit inverted)
    pushfd                      ;Store EFLAGS again (ID bit may or may not be inverted)
    pop eax                     ;eax = modified EFLAGS (ID bit may or may not be inverted)
    xor eax,[esp]               ;eax = whichever bits were changed
    popfd                       ;Restore original EFLAGS
    and eax,0x00200000          ;eax = zero if ID bit can't be changed, else non-zero
    ret

    global __dump_stack
    extern printx32
    extern newLine
__dump_stack:
    mov ecx, [ebp]
    push ecx
    call printx32
    call newLine
    mov ecx, [ebp+4]
    push ecx
    call printx32
    call newLine
    mov ecx, [ebp+8]
    push ecx
    call printx32
    call newLine
    ret
