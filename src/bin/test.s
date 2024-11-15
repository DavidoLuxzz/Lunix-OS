; CREATE ELF FILE EXECUTABLE LOADER         - jesam
; FIX RELATIVE ADDRESSES                    - jesam
; poenta OSa je da ga pravis za sebe, pa i nije toliko bitno da li ces sjebati OS
; no libraries will be supported until a proper system for that doesn't get implemented
    section .text
    global _start
_start:
    ; write syscall
    mov     eax,    0x2
    mov     ebx,    msg
    mov     ecx,    13
    extern  __syscall
    call    __syscall
    ret
    int 0x80
    ret

    section .data
msg:    db  "hello niggas", 10, 0
ciger:  db  "mjau mjau ma mjau", 10, 0

