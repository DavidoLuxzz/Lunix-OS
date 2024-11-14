; CREATE ELF FILE EXECUTABLE LOADER         - jesam
; FIX RELATIVE ADDRESSES                    - jesam
; poenta OSa je da ga pravis za sebe, pa i nije toliko bitno da li ces sjebati OS
    section .text
    global _start
_start:
    call    _testf
    ; write syscall
    mov     eax,    0x2
    mov     ebx,    msg
    mov     ecx,    13
    int 0x80
    ret

    section .data
msg:    db  "hello niggas", 10, 0
ciger:  db  "mjau mjau ma mjau", 10, 0

    section .nigger
    global  _testf
_testf:
    mov     byte [0xb8124], '*' 
    mov     eax,    0x2
    mov     ebx,    ciger
    mov     ecx,    18
    int     0x80
    ret


