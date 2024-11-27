#include <sys/commands.h>
#include <stdio.h>
#include <util.h>

__CMD_FUNC(cmd_fsz){
    if (argc < 1){
        logerr("no file name given"); return 0;
    }
    LXFS_ENTRY* e = lxfs_open_file(&currentDir, command+tokens[0]);
    printf("File: %s\n", e->name);
    printf("Type: %u\n", (e->type_and_perms>>12)&0xF);
    printf("LBA: %u\n", e->lba);
    printf("Size: %u (%u bytes)\n", e->size, e->size*512);
    lxfs_close_file(e);
    return 0;
}

