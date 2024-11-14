#include <sys/commands.h>
#include <fs/lxfs.h>
#include <string.h>
#include <util.h>
#include <stdio.h>

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

