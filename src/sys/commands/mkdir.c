#include <sys/commands.h>
#include <fs/lxfs.h>
#include <stdio.h>

__CMD_FUNC(cmd_mkdir) {
    if (argc < 1) {
        printf("No directory name given\n");
        return 0;
    }
    int err = lxfs_create_new_dir(&currentDir, command+tokens[0], 0333, 0);
    if (err) printf("error creating directory %u\n", err);
    return err;  
}
