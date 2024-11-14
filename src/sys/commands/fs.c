#include <sys/commands.h>
#include <fs/lxfs.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <util.h>

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
                write(e.name, umax(strlen(e.name),53)); putc(0xa);
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
