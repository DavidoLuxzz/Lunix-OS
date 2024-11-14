#include <sys/commands.h>
#include <fs/lxfs.h>
#include <stdio.h>

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


