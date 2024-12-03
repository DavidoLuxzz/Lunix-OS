BITS 32

MBOOT_MEM_INFO EQU 1 << 1
MBOOT_USE_GFX EQU 0

MBOOT_MAGIC    equ 0x1BADB002
MBOOT_FLAGS    equ MBOOT_MEM_INFO
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)

; multiboot data should have its own section
; so QEMU could find data needed to run with -kernel option
    section .multiboot
    ALIGN 4
    DD MBOOT_MAGIC
    DD MBOOT_FLAGS
    DD MBOOT_CHECKSUM
    DD 0, 0, 0, 0, 0

    DD 0 ;0 = linear graphics
    DD 1280 ;width
    DD 960 ;height
    DD 32 ;depth

    section .text
    global start
    extern kmain

start:
    CLI
    ; set up stack
    MOV ebp, stack_top
    MOV esp, ebp
    ; start kernel
    PUSH ebx
    PUSH eax
    CALL kmain
    HLT

NoLongMode:
HaltKernel:
    MOV  byte [0xB8000], '!' 
    CLI
    HLT
    JMP HaltKernel

    section .bss
stack_bottom:
    RESQ 8192   ; 64 kB
stack_top:
