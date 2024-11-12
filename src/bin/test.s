; CREATE ELF FILE EXECUTABLE LOADER
; FIX RELATIVE ADDRESSES
; poenta OSa je da ga pravis za sebe, pa i nije toliko bitno da li ces sjebati OS
; jedno resenje za REL-ADDR jeste da kernel namesti CS,DS,ESP i EBP registere pre lansiranja procesa
; -> drugo resenje je da kernel preda procesu absolutnu adresu procesa i program ga sacuva u memoriji
;   pa kad program zeli da koristi svoju memoriju moze koristiti sacuvanu adresu kao bazu a relativnu datu adresu kao offset
    bits 32    ; start with this or NASM will compile it as 16-bit code
    section .text
    global _start
_start:
    ; write syscall
    mov     eax,    0x2
    mov     ebx,    msg
    mov     ecx,    13
    int 0x80
    ret

    section .data
msg:    db  "hello niggas", 10, 0
