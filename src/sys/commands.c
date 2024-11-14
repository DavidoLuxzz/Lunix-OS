#include <sys/commands.h>
#include <vga.h>
#include <stdint.h>
#include <math.h>
#include <gdt/gdt.h>
#include <interrupts/idt.h>
#include <timer.h>
#include <sys/kmalloc.h>
#include <keyboard.h>
#include <multiboot.h>
#include <util.h>
#include <stdio.h>
#include <sys/core_asm.h>
#include <terminal.h>
#include <string.h>
#include <unistd.h>
#include <sys/drivers/disk_ata.h>
#include <sys/drivers/atapi.h>
#include <sys/drivers/disk_h.h>
#include <os/install.h>
#include <cpu/cpu.h>
#include <disk/partition.h>
#include <fs/lxfs.h>

extern LXFS_DIR currentDir;


