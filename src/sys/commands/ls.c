#include <sys/commands.h>
#include <fs/lxfs.h>
#include <stdio.h>

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
