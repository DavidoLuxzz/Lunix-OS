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

__CMD_NOAR_FUNC(cmd_clear) {
    clear_screen();
    return 0;
}

__CMD_FUNC(cmd_vol) {
    if (argc < 1){
        printf("^4error:^_ no enough arguments\n");
        printf("vol:\n  info (drive)\n  pt [drive] (offset) (lba)\n  dir [drive] [lba] (offset)\n");
    } else if (strcmd(command+tokens[0], "info")){
        if (argc < 2){
            ide_dump_drives();
        } else {
            uint32_t drv = stoi(command+tokens[1]);
            __os_iso_vol_desc(drv);
        }
    } else if (strcmd(command+tokens[0], "pt")){
        if (argc < 2){
            printf("No drive selected\n");
        } else {
            uint32_t drv = stoi(command+tokens[1]);
            uint32_t offset=0, lba;
            if (argc >= 3) {
                offset = stoi(command+tokens[2]);
                if (argc < 4) lba = __os_iso_get_pt_addr(drv);
                else lba = stoi(command+tokens[3]);
            } else lba = __os_iso_get_pt_addr(drv);
            printf("LBA: %u\n", lba);
            __os_iso_path_table(drv, lba, offset);
        }
    } else if (strcmd(command+tokens[0], "dir")){
        if (argc < 3){
            printf("No drive selected\nNo logical block selected\n");
        } else {
            uint16_t offset;
            if (argc < 4) offset = 0;
            else offset = stoi(command+tokens[3]) & 0xFFFF;
            uint32_t drv = stoi(command+tokens[1]);
            uint32_t lba = stoi(command+tokens[2]);
            __os_iso_dir_name(drv, lba, offset);
        }
    } else {
        printf("^4error:^_ unknown option: ");
        write(command+tokens[0], strdlen(command+tokens[0], ' '));
        newLine();
    }
    return 0;
}

__CMD_FUNC (cmd_part) {
    if (argc < 1){
        PARTITION part;
        _get_partition(0, 0, &part);
        printf("PARTITION INFO:\n");
        printf("- Name: "); write(part.name, 18); newLine();
        printf("- Magic: %p", part.magic);
        printf("- Magic ASCII: "); write((char*)&part.magic, 4); newLine();
    } else if (strcmd(command+tokens[0], "set")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1]);
            if (!id) {
                printf("Access denied editing Partition 0\n");
                return 6;
            }
            printf("Setting partition (id=%u)\n", id);
            PARTITION previous;
            _get_partition(0,id-1,&previous);
            // check if exists
            printf("id after prev part: %u\n", id);
            PARTITION part;
            _get_partition(0,id,&part);
            printf("id after new part: %u\n", id);
            if (_is_partition_valid(&part)){
                printf("Partition exists at index %u\nDo you still want to edit? [y/n]");
                char _tmp;
                nread(&_tmp, 1);
                if (_tmp != 'y') return 0;
            }
            printf("id after part valid check: %u\n", id);
            char _tmp[20];
            _tmp[19] = 0;
            printf("af: %u\n", id);
            printf("Enter Partition Name: "); nread(_tmp, 18);
            printf("nr: %u\n", id);
            strncpyzx(part.name,_tmp,18);
cmd_part_disk_size_loop:
            printf("Enter Partition Disk space(K|M|G): "); nread(_tmp, 10);
            part.space = strsect(_tmp, 512);
            if (!part.space){
                printf("Partition disk space cannot be less than 512 bytes!\n");
                goto cmd_part_disk_size_loop;
            }
            printf("Enter Partition Filesystem ID: "); nread(_tmp, 5);
            part.fsID = stoi(_tmp);
            part.type = PARTYPE_USER;
            part.lba = previous.lba + previous.space;
            part.magic = PARTITION_MAGIC;
            printf("id cherck: %u", id);
            _set_partition(0,id,&part);
        }
    } else if (strcmd(command+tokens[0], "info")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1]);
            PARTITION part;
            _get_partition(0, id, &part);
            printf("PARTITION %u INFO:\n", id&0xFFFF);
            printf("- Name: "); write(part.name, 18); newLine();
            printf("- Type: %u\n", part.type);
            printf("- File system: %s\n", _get_partition_fs_name(part.fsID));
            printf("- Magic: %p\n", part.magic);
            printf("- Magic ASCII: "); write((char*)&part.magic, 4); newLine();
            printf("- Disk space: %u (%u kB)\n", part.space, (part.space/2));
            printf("- Block address: %u\n", part.lba);
        }
    } else if (strcmd(command+tokens[0], "fixboot")){
        bool qemu_without_cdrom = false;
        if (argc > 1) {
            if (strcmd(command+tokens[1], "qemu-without-cdrom")) qemu_without_cdrom = true;
        }
        uint32_t drive = 1; // source drive
        uint32_t sectors = 1400;
        if (!qemu_without_cdrom) sectors = __os_iso_get_vol_size(drive);
        else printf("(qemu-without-cdrom): creating boot partition with disk space of 1400 sectors\n");
        if (sectors < 500)
            printf("^eWARN:^_ CD-ROM ISO Volume size not corrent. If you are running from QEMU, you are propably running with -kernel without -cdrom option.\n");
        PARTITION part = {
            "BOOT PARTITION",
            PARTYPE_BOOT,
            FILESYSTEM_ISO9660,
            PARTITION_MAGIC,
            5600,       // 1400 CD-ROM sectora = 5600 HD sectora (1400*2048 / 512)
            0x00000000
        };
        part.space = sectors * 4;
        _set_partition(0,0,&part);
    } else if (strcmd(command+tokens[0], "rm")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1]);
            uint8_t zero[PARTITION_STRUCT_SIZE];
            memset(zero, 0, PARTITION_STRUCT_SIZE);
            _set_partition(0,id,(PARTITION*)zero);
        }
    } else if (strcmd(command+tokens[0], "invl")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1]);
            if (!id){
                printf("Access denied (BOOT PARTITION)\n");
                return 6;
            }
            PARTITION part;
            _get_partition(0,id,&part);
            memcpy((char*)&part.magic, "INVL", 4);
            _set_partition(0,id,&part);
        }
    }
    return 0;
}

__CMD_FUNC (cmd_fs) {
    // LxFS file system command
    if (argc < 1) return 0;
    if (strcmd(command+tokens[0], "root")){ // broken, no support
        // List all entries in root directory (LBA=0)
        LXFS_ENTRY root; // '.'
        lxfs_get_entry(0,0, &root); // parent lba 0 (root), 0 za . (1 za ..)
        uint32_t dsize = root.size + 2; // bez . i .. + ta dva
        uint32_t found=0;
        for (uint32_t i=0; i<8; i++){
            LXFS_ENTRY e;
            lxfs_get_entry(0,i, &e);

            if (!(e.flags & LXFS_VALID)) continue;

            printf("%s\t", _lxfs_get_type_name((e.type_and_perms>>12) & 0x0F));
            if (e.name[0]==0) printf(".\n");
            else if (e.name[0]==1 && e.name[1]==0) printf("..\n");
            else {
                write(e.name, umax(strlen(e.name),53)); newLine();
            }
            if (++found >= dsize) return 0;
        }
    } else if (strcmd(command+tokens[0], "test")){ // force creates root directory if something messed up
        // add home folder in root directory
        LXFS_DIR root = {
            {
                "/",
                LXFS_VALID | LXFS_HIDDEN,
                (LXFS_FOLDER<<12) | 0777,
                0,
                0
            },
            0,
            "\0"
        };
        lxfs_set_dir(0,&root);
    } else if (strcmd(command+tokens[0], "bmap")){
        lxfs_list_bitmap();
    } else if (strcmd(command+tokens[0], "clear")){
        LXFS_ENTRY es[LXFS_MAX_NODES+2];
        lxfs_get_all_entries(currentDir.entry.lba, es);
        for (uint32_t i=2; i<LXFS_MAX_NODES; i++){
            if (es[i].flags & LXFS_VALID){
                LXFS_ENTRY e = es[i];
                switch ((e.type_and_perms>>12) & 0x0F){
                    case LXFS_FOLDER:
                        // remove whole dir
                        _lxfs_remove_dir(&currentDir, i);
                        break;
                    case LXFS_FILE:
                        // remove file entry
                        _lxfs_invl_entry(&currentDir, i);
                        break;
                    default:
                        printf("Unknown node type\n");
                        break;
                }
            }
        }
        currentDir.entry.size = 0;
        lxfs_set_dir(currentDir.entry.lba, &currentDir);
    } else if (strcmd(command+tokens[0], "size")){
        printf("current dir size: 0x%x\n", currentDir.entry.size);
    }
    return 0;
}

__CMD_FUNC(cmd_ls) {
    // List directory (".", ".." uvek prvo)
    uint32_t valids=0;
    LXFS_ENTRY es[LXFS_MAX_NODES+2];
    // first two entries are combined a LXFS_DIR struct, so they are skipped
    lxfs_get_all_entries(currentDir.entry.lba, es);
    uint32_t dirsize = es[0].size; // + 2 for . and ..
    if (!dirsize) return 0;
    for (uint32_t i=2; i<LXFS_MAX_NODES; i++){
        LXFS_ENTRY e = es[i];
        if (!(e.flags & LXFS_VALID)) continue;
        
        printf("%s %s\n", _lxfs_get_type_name(e.type_and_perms>>12), e.name);

        if (++valids >= dirsize) return 0;
    }
    return 0;
}

__CMD_FUNC(cmd_cd) {
    // change directory
    if (argc < 1) return 0;
    if (strcmd(command+tokens[0], ".")) return 0;
    if (strcmd(command+tokens[0], "..")) {
        LXFS_DIR target_dir;
        lxfs_get_dir(currentDir.parent_lba, &target_dir);
        memcpy(&currentDir, &target_dir, 128);
        return 0;
    }
    uint32_t valids=0;
    LXFS_ENTRY es[LXFS_MAX_NODES+2];
    // first two entries are skipped (LXFS_DIR)
    lxfs_get_all_entries(currentDir.entry.lba, es);
    uint32_t dirsize = es[0].size; // + 2 for . and ..
    if (!dirsize) return 0;
    for (uint32_t i=2; i<LXFS_MAX_NODES; i++){
        LXFS_ENTRY e = es[i];
        if (!(e.flags & LXFS_VALID)) continue;
       
        if (strcmd(e.name, command+tokens[0])){
            uint8_t type = (e.type_and_perms>>12)&0x0F;
            if (type != LXFS_FOLDER){
                printf("cd: not a directory: %s\n", e.name);
                return 0;
            }
            LXFS_DIR target_dir;
            lxfs_get_dir(e.lba, &target_dir);
            memcpy(&currentDir, &target_dir, 128);
            return 0;
        }

        if (++valids >= dirsize) break;
    }
    printf("cd: no such file or directory: %s\n", command+tokens[0]);
    return 0;
}

__CMD_FUNC(cmd_mkdir) {
    if (argc < 1) {
        printf("No directory name given\n");
        return 0;
    }
    int err = lxfs_create_new_dir(&currentDir, command+tokens[0], 0333, 0);
    if (err) printf("error creating directory %u\n", err);
    return err;  
}

__CMD_FUNC(cmd_rmdir) {
    // remove directory
    if (argc < 1) return 0;
    int err = lxfs_remove_dir(&currentDir, command+tokens[0]);
    if (!err) return 0;
    switch(err){
        case LXFS_ERR_NOT_FOUND:
            printf("rmdir: no such file or directory: %s\n", command+tokens[0]); return 0;
        case LXFS_ERR_TYPE:
            printf("rmdir: not a directory: %s\n", command+tokens[0]); return 0;
        default:
            printf("rmdir: error removing directory\n"); return 0;
    }
    return 0;
}

__CMD_FUNC(cmd_rm) {
    // remove file
    // file is removed by just its entry being invalidated (removed)
    // so...
    if (argc < 1) {
        printf("rm: no arguments\n"); return 0;
    }
    int err = lxfs_remove_file(&currentDir, command+tokens[0]);
    if (!err) return 0;
    switch(err){
        case LXFS_ERR_NOT_FOUND:
            printf("rm: no such file or directory: %s\n", command+tokens[0]); return 0;
        case LXFS_ERR_TYPE:
            printf("rm: not a file: %s\n", command+tokens[0]); return 0;
        default:
            printf("rm: error removing file\n"); return 0;
    }
    return 0;
}

__CMD_FUNC(cmd_finfo){
    // displays file info
    if (argc < 1){
        printf("no file entry index given\n"); return 0;
    }
    uint32_t i = stoi(command+tokens[0]);
    LXFS_ENTRY e;
    lxfs_get_entry(currentDir.entry.lba, i, &e);
    printf("Name: %s\n", e.name);
    printf("Flags: %x\n", e.flags);
    printf("Size: %u\n", e.size);
    printf("LBA: %u\n", e.lba);

    return 0;
}

__CMD_FUNC(cmd_led){
    // lunix editor
    if (argc < 1){
        printf("led: no arguments\n"); return 0;
    }
    LXFS_ENTRY* e = lxfs_open_file(&currentDir, command+tokens[0]);
    if (!e) {
        printf("Failed to open file: %s\n", command+tokens[0]); return 0;
    }
    uint32_t alba = lxfs_get_data_lba(e->lba);
    printf("Opening file at absolute LBA=%u\n",alba);
    // load file to memory
    char bytes[e->size*512];
    ata_disk_read(0, alba, e->size, bytes);
    // start editor
    printf("Lunix editor (v0.0.1)\n");
    uint32_t view_range, current_byte=0;
    bool running=true;
    printf("Enter viewing range: ");
    char tmp[TERMINAL_MAX_BUFFER_LENGTH+1]; // +1 za svaki slucaj
    tmp[TERMINAL_MAX_BUFFER_LENGTH]=0;
    nread(tmp, 10);
    view_range = stoi(tmp);
    // save current VGA screen to buffer
    uint16_t* __tmp_screen = kmalloc(width*height*2);
    memcpy(__tmp_screen, (void*)0xb8000, width*height*2);
    uint32_t __tmp_vga_pos = get_vga_position();
    // start editing process
    clear_screen();
    while (running){
        move_cursor(0,height-2);
        printf("$ ");
        read(tmp);
        clear_screen();
        if (strcmd(tmp, "q")) running = false;
        else if (strcmd(tmp, "r")){
            // preview file from current_byte to +view_range
            for (uint32_t i=0; i<view_range; i++){
                printf("%x ", bytes[i+current_byte]&0xFF);
            }
            newLine();
        } else if (strcmd(tmp, "e")){
            printf("Edit this byte section (use space as delimiter): ");
            read(tmp);
            // tokenize input ' '
            uint16_t tokens[strlen(tmp)/2];
            tokens[0]=0; // da bi kreno od pocetka
            size_t argc = strtok(tmp, ' ', (uint16_t*)tokens+1);
            for (uint32_t i=0; i<argc+1; i++){
                uint8_t x = stox(tmp+tokens[i])&0xFF;
                printf("writing %x to %u\n", x, current_byte+i);
                bytes[current_byte+i] = x;
            }
        } else if (strcmd(tmp, "mv")){
            printf("Move to (byte): ");
            read(tmp);
            current_byte = stoi(tmp);
        } else if (strcmd(tmp, "w")){
            ata_disk_write(0, alba, e->size, bytes);
            printf("Saved file at LBA=%u (\"%s\")\n",alba,command+tokens[0]);
        } else if (strcmd(tmp, "help")){
            printf("Commands:\n  r: preview file from current byte\n  q: quit editor without saving\n  w: save changes\n  e: edit selected section\n  mv: move to byte\n");
        } 
    }
    // load back saved screen
    memcpy((void*)0xb8000,__tmp_screen,width*height*2);
    kfree(__tmp_screen);
    set_vga_position(__tmp_vga_pos);
    putchar(0);
    // finally, close the file ...
    lxfs_close_file(e);
    return 0;
}




__CMD_FUNC(cmd_run) {
    // running program files from disk
    uint32_t rlba = 0;
    if (argc < 1) {
        printf("Assuming LBA=%u\n", rlba);
    } else rlba = stoi(command+tokens[0]);
    uint32_t alba = lxfs_get_data_lba(rlba);
    asm volatile ("mov %0, %%edi" : "+r"(alba));
    _fire_syscall();
    return 0;
}



__CMD_FUNC(cmd_shut) {
    if (argc < 1) {
        printf("Choose shutdown method:\n1: todo\n2: QEMU\n$ ");
        char ans[10];
        nread(ans,1);
        switch(ans[0]&0xFF - '0'){
            case 1: goto __todo_shutdown_label;
            case 2: goto __qemu_shutdown_label;
            default: goto __invalid_shutdown_label;
        }
    } else if (strcmd(command+tokens[0], "--todo")){
__todo_shutdown_label:
        printf("Do you know what TO DO means?? Try 'exit' command instead\n");
    } else if (strcmd(command+tokens[0], "--qemu")){
__qemu_shutdown_label:
        printf("Sending shutdown signal... (QEMU)\n");
        outw(0x604, 0x2000);
    } else {
__invalid_shutdown_label:
        printf("Unknown shutdown option: %s\n",command+tokens[0]);
    }
    return 0;
}

