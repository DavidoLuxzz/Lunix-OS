#include <user/users.h>
#include <fs/lxfs.h>
#include <util.h>

int init_users_file(){
    LXFS_ENTRY* en = lxfs_open_file(lxfs_get_root_dir(), ".users");
    if(!en){ // doesn't exist
        log("USERS: user file doesn't exists");
        if (lxfs_create_new_file(lxfs_get_root_dir(), ".users", 0000, LXFS_HIDDEN, 512)){
            logerr("USERS: error creating users file");
            return 9;
        }
        en = lxfs_open_file(lxfs_get_root_dir(), ".users");
    }
    log("USERS: successfully loaded users file");
    lxfs_close_file(en);
    return 0;
}

