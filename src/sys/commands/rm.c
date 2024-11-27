#include <sys/commands.h>
#include <fs/lxfs.h>
#include <stdio.h>


__CMD_FUNC(cmd_rm) {
    // remove file
    // file is removed by just its entry being invalidated (removed)
    // so...
    if (argc < 1) {
        printf("rm: no arguments\n"); return 0;
    }
    int err = lxfs_remove_file(lxfs_get_current_dir(), command+tokens[0]);
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
