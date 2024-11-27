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
#include <sys/driver.h>
#include <sys/libkern.h>
#include <user/users.h>

#define KERNEL_BOOT_START   0x01000000  // grub loaded kernel destination
#define KERNEL_MEMORY_START 0x01200000  // = ~1 GB for kernel and bios   (1,25gb)
                                        // rest is for apps (from $KERNEL_MEMORY_START)
extern uint32_t _kernel_end;

bool __global_exit;

LXFS_DIR currentDir; // the LBA of a dir

extern void init_system_calls(void);

void _kernel_install_drivers(void);
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
    init_system_calls();
    printf("[ ^2OK^7 ] System call table initialized\n");
    init_driver_array();
    printf("[ ^2OK^7 ] Global driver slots array initialized\n");
    printf("           [ INSTALLING DRIVERS ]\n");
    _kernel_install_drivers();
    putchar(0xa);

    kmem_set_start(KERNEL_MEMORY_START);
    
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
    
    init_users_file();
    printf("[ ^2OK^7 ] /.users file initialized\n");
 
    k_setKeyboardMode(K_MODE_TERMINAL);
  
    // --- RESET SCREEN --- //
    // clear_screen();
    print("LunixOS 0.4.3\n\n");
    
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
    } else if (strcmd(command, "clear")) clear_screen();
    else if (strcmd(command, "disk")) ide_dump_drives();
    else if (strcmd(command, "install")) __os_install_to_disk(0,1,IDE_ATAPI, 0);
    else if (strcmd(command, "vol")) cmd_vol(command, argc, tokens);
    else if (strcmd(command, "ils")){
        if (!argc) {
            printf("No LBA given\n");
            return;
        }
        uint32_t drv = 1; //set
        uint32_t lba = stoi(command+tokens[0]);
        __os_iso_list_dir(drv, lba);
   }else if (strcmd(command, "part")) cmd_part(command, argc, tokens);
    else if (strcmd(command, "fs")) cmd_fs(command, argc, tokens);
    else if (strcmd(command, "ls")) cmd_ls(command, argc, tokens);
    else if (strcmd(command, "cd")) cmd_cd(command, argc, tokens);
    else if (strcmd(command, "mkdir")) cmd_mkdir(command, argc, tokens);
    else if (strcmd(command, "lba")) printf("%u\n",currentDir.entry.lba);
    else if (strcmd(command, "rmdir")) cmd_rmdir(command, argc, tokens);
    else if (strcmd(command, "new")) {
        if (argc < 1) {
            printf("new: no file name given\n"); return;
        }
        printf("Enter static file size in bytes: ");
        char tmp[12];
        memset(tmp, 0, 10);
        nread(tmp, 10);
        uint32_t bytes = stoi(tmp);
        int err = lxfs_create_new_file(&currentDir, command+tokens[0], 0777, 0, bytes);
        if (err) printf("error: %u\n", err);
   }else if (strcmd(command, "rm")) cmd_rm(command, argc, tokens);
    else if (strcmd(command, "led")) {
        printf("Editor currently not in use. Will be when fixed...\ny/N ");
        char tmp[2];
        nread(tmp,1);
        if (*tmp != 'y') return;
        cmd_led(command, argc, tokens);
   }else if (strcmd(command, "run")) cmd_run(command, argc, tokens);
    else if (strcmd(command, "runa")) cmd_runa(command, argc, tokens);
    else if (strcmd(command, "shut")) cmd_shut(command, argc, tokens);
    else if (strcmd(command, "fsz")) cmd_fsz(command, argc, tokens);
    else if (strcmd(command, "elf")){
        if (argc < 1) logerr("no input file");
        else {
            elf_prog_data pd;
            elf_load_file(&currentDir, command+tokens[0], &pd);
            printf("-- PROGRAM DATA: --\n");
            printf("File offset: %p\nMemory size: %p\n", pd.memoff, pd.memsz);
            printf("Entry point address: %p\n", pd.entry);
        }
    } else if (strcmd(command, "cpbin")){
        char tmp[12];
        printf("Destination drive will be assumed to be 0\nEnter source drive: ");
        nread(tmp, 10);
        uint8_t sdrv = stoi(tmp)&0xFF;
        printf("How many sectors will be copied: ");
        nread(tmp, 10);
        uint32_t sects = stoi(tmp);
        char* buf = (char*)kmalloc(sects*512);
        _ata_disk_read(sdrv, 0x0,sects, buf);
        
        write(buf, 4);

        _ata_disk_write(0, 9696,sects, buf);

        kfree(buf);
    }else {
        print("unknown command: "); write(command, strdlen(command, ' '));
        newLine();
    }
}



// ## DRIVER INITIALIZATION ## //
void _kernel_install_drivers(void){
    driver_install(ata_get_driver());
    driver_install(k_keyboard_get_driver());
    driver_install(ide_get_driver0());
    driver_install(ide_get_driver1());
}
