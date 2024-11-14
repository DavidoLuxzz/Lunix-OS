#include <sys/commands.h>
#include <stdio.h>
#include <sys/drivers/disk_h.h>
#include <os/install.h>
#include <unistd.h>
#include <string.h>

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
        putc(0xa);
    }
    return 0;
}

