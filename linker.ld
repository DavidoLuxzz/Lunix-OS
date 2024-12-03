OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
 {
    . = 0x1000000;
    /* .text section contains .multiboot and .text */
    .text : {
        /* thus .multiboot section set as first .text section,
         * so that QEMU could find it */
        *(.multiboot)
        *(.text)
    }
    .data : {*(.data)}
    .bss  : {*(.bss)}

    _kernel_end = ALIGN(4K);
 }
