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
#include <sys/syscall.h>
#include <sys/elf.h>

#define KERNEL_BOOT_START   0x00100000  // grub loaded kernel destination
#define KERNEL_MEMORY_START 0x00500000  // = ~1 GB for kernel and bios   (1,25gb)
                                        // rest is for apps (from $KERNEL_MEMORY_START)
extern uint32_t _kernel_end;

bool __global_exit;

LXFS_DIR currentDir; // the LBA of a dir

extern void init_system_calls(void);

void __dump_multiboot_info(struct multiboot_info* bootInfo);
void __transfer_iso_from_drive(uint16_t, uint16_t);
void __execute_full(const char*);
void kmain(uint32_t magic, struct multiboot_info* bootInfo);
void kmain(uint32_t magic, struct multiboot_info* bootInfo){
    printf("Loading ^9LUNIX OS^7...\nWelcome\n");
    initGdt(KERNEL_MEMORY_START);
    printf("[ ^2OK^7 ] GDT initialized\n");
    initIdt();
    printf("[ ^2OK^7 ] IDT initialized\n");
    initTimer();
    printf("[ ^2OK^7 ] Timer interrupt initialized\n");
    initKeyboard();
    printf("[ ^2OK^7 ] PS2 Keyboard initialized\n");
    init_kmalloc(KERNEL_MEMORY_START);
    printf("[ ^2OK^7 ] Memory allocation initialized\n");
    ide_initialize(0,0,0,0,0);
    printf("[ ^2OK^7 ] IDE/ATAPI driver initialized\n"); 
    
    init_system_calls();

    if (__cpu_check()){
        char _cpu_vend_raw[12];
        cpuid_string(0, (uint32_t*)_cpu_vend_raw);
        
        char* _cv = _cpu_vend_raw;

        char _cpu_vend[13];
        memset(_cpu_vend, '#', 12);
        memcpy(_cpu_vend, _cv+0, 4);
        memcpy(_cpu_vend+8, _cv+4, 4);
        memcpy(_cpu_vend+4, _cv+8, 3);
        _cpu_vend[7] = _cv[11];

        _cpu_vend[12] = 0;

        printf("CPU Vendor: %s\n", _cpu_vend);
    }
    newLine();
  
    DISK_PARTITION dpart;
    PARTITION defpart;
    _get_partition(0,1,&defpart);
    dpart.drive = 0;
    memcpy(&dpart.part, &defpart, PARTITION_STRUCT_SIZE);
    lxfs_use_partition(&dpart);    
    lxfs_get_dir(0,&currentDir);
    lxfs_load_filesystem();
    
    
    uint32_t test_iptr = 0x0;
    test_iptr = _getip();
    printf("EIP: %p\n", test_iptr);

    printf("printf at: %p\n", printf);

    k_setKeyboardMode(K_MODE_TERMINAL);
    print("Keyboard mode set to: K_MODE_TERMINAL\n");
  
    // --- RESET SCREEN --- //
    // clear_screen();
    print("LunixOS 0.3.2\n\n");
    
    // __dump_multiboot_info(bootInfo);
    
    char command[TERMINAL_MAX_BUFFER_LENGTH];
    __global_exit = 0;
    while (!__global_exit){
        if (kGetKey(K_ESC)) break;
        printf("^croot^7@lunixos ^f%s^7 > ", lxfs_get_dir_name(&currentDir));
        // todo
        read(command);
        __execute_full(command);
    }
    k_setKeyboardMode(K_MODE_DISABLED);
    print("Halting...\n");
    for(;;);
}

uint8_t currentPartition=1;

#include <sys/commands.h>
void __execute_full(const char* command){
    uint16_t tokens[32];
    size_t argc = strtok(command, ' ', tokens);

    if (strdlen(command, ' ')<1) return;

    if (strcmd(command, "help")){
        printf("Built-in commands:\n");
        printf("- clear                          # Cleares screen\n");
        printf("- exit                           # Halt system (TODO)\n");
        printf("- disk                           # Displays all detected disks\n");
        printf("- install                        # Installs OS from ISO to disk\n");
        printf("                                 ^4! Don't use on real hardware^_\n");
        printf("- dsktest (atapi_drive)          # Tests CD-ROM read with IDE ATAPI\n");
        printf("- part [info|set|fixboot] (id)   # Main partitioning command\n");
        printf("- fs [root|test]                 # LxFS filesystem command\n");
        printf("- mkdir [name]                   # Create new directory\n");
        printf("- rmdir [name]                   # Remove directory\n");
        printf("- ls                             # List directory\n");
        printf("- cd [name]                      # Change current directory\n");
        printf("- help                           # List built-in commands\n");
        return;
    } else if (strcmd(command, "clear")){
        cmd_clear();
        return;
    } else if(strcmd(command, "exit")){
        // TODO
        __global_exit = true;
        return;
    } else if (strcmd(command, "disk")){
        ide_dump_drives();
        return;
    } else if (strcmd(command, "install")){
        uint32_t where = 0; // resiti zbog partition (ali prvi sektor mora biti za boot)
        __os_install_to_disk(0,1,IDE_ATAPI, where);
        *(char*) 0xb8002 = '?';
        return;
    } else if (strcmd(command, "dsktest")){
        if (argc < 1){
            printf("^4error:^_ no enough arguments\n");
            return;
        }
        uint32_t drv = stoi(command+tokens[0]);
        printf("[ ^fINFO^_ ] Reading from Drive %u:\n", drv);
        uint16_t _r_buffer[1024];
        memset(_r_buffer, 0, 2048);
        uint8_t err = ide_atapi_read(drv, 24, 1, 0, _r_buffer);
        for (int i=0; i<4; i++){
            printf("0x%x ", _r_buffer[i]);
        }
        newLine();
        ide_print_error(drv, err);
        return;
    } else if (strcmd(command, "vol")){
        cmd_vol(command, argc, tokens);
        return;
    } else if (strcmd(command, "ils")){
        if (!argc) {
            printf("No LBA given\n");
            return;
        }
        uint32_t drv = 1; //set
        uint32_t lba = stoi(command+tokens[0]);
        __os_iso_list_dir(drv, lba);
        return;
    } else if (strcmd(command, "part")){
        cmd_part(command, argc, tokens);
        return;
    } else if (strcmd(command, "fs")){
        cmd_fs(command, argc, tokens);
        return;
    } else if (strcmd(command, "ls")){
        cmd_ls(command, argc, tokens);
        return;
    } else if (strcmd(command, "cd")){
        cmd_cd(command, argc, tokens);
        return;
    } else if (strcmd(command, "mkdir")){
        cmd_mkdir(command, argc, tokens);
        return;
    } else if (strcmd(command, "lba")){
        printf("%u\n",currentDir.entry.lba);
        return;
    } else if (strcmd(command, "rmdir")){
        cmd_rmdir(command, argc, tokens);
        return;
    } else if (strcmd(command, "new")){
        if (argc < 1) {
            printf("new: no file name given\n"); return;
        }
        int err = lxfs_create_new_file(&currentDir, command+tokens[0], 0777, 0, 512);
        if (err) printf("error: %u\n", err);
        return;
    } else if (strcmd(command, "rm")){
        cmd_rm(command, argc, tokens);
        return;
    } else if (strcmd(command, "finfo")){
        cmd_finfo(command, argc, tokens);
        return;
    } else if (strcmd(command, "led")){
        cmd_led(command, argc, tokens);
        return;
    } else if (strcmd(command, "run")){
        cmd_run(command, argc, tokens);
        return;
    } else if (strcmd(command, "shut")){
        cmd_shut(command, argc, tokens);
        return;
    } else if (strcmd(command, "elf")){
        if (argc < 1) logerr("no input file");
        else {
            elf_prog_data pd;
            elf_load_file(&currentDir, command+tokens[0], &pd);
            printf("-- PROGRAM DATA: --\n");
            printf("File offset: %p\nMemory size: %p\n", pd.memoff, pd.memsz);
            printf("Entry point address: %p\n", pd.entry);
        }
        return;
    } else if (strcmd(command, "cpbin")){
        char tmp[12];
        printf("Destination drive will be assumed to be 0\nEnter source drive: ");
        nread(tmp, 10);
        uint8_t sdrv = stoi(tmp)&0xFF;
        printf("How many sectors will be copied: ");
        nread(tmp, 10);
        uint32_t sects = stoi(tmp);
        char* buf = (char*)kmalloc(2048);
        ata_disk_read(sdrv, 0x0,sects, buf);
        
        write(buf, 4);

        ata_disk_write(0, 9696,sects, buf);

        kfree(buf);
        return;
    }
    print("unknown command: "); write(command, strdlen(command, ' '));
    newLine();
}

void __dump_multiboot_info(struct multiboot_info* bi){
    printf("flags 0b%b\n", bi->flags);
    printf("mem_lower 0x%x\n", bi->mem_lower);
    printf("mem_upper 0x%x\n", bi->mem_upper);
    printf("boot_device 0x%x\n", bi->boot_device);

    print((const char*)bi->cmdline);
    printf("mods_count %u\n", bi->mods_count);
    printf("mods_addr 0x%x\n", bi->mods_addr);
}

