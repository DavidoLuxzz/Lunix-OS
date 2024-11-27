#include <user/user.h>
#include <fs/lxfs.h>
#include <util.h>
#include <stdio.h>
#include <sys/libkern.h> // for disk functions
#include <string.h>
#include <math.h>
#include <unistd.h>

// Local variables //

user_t __all_users[MAX_USERS];

const user_t __root = {0, 0, 0, "root"};
const user_t __usr_null = {-1, -1, -1, USER_NAME_NULL};

user_t __login = __usr_null; // current user (initialize to null)

uint32_t    __users_file_lba;

// Functions //

int init_users(){
    if (init_users_file()){
        memset(__all_users, 0xFF, sizeof(__all_users)); // set all users to {-1,-1,-1} (null)
        __all_users[0] = __root;
    } else {
        char bytes[512];
        disk_read(__users_file_lba, 1, bytes);
        memcpy(__all_users, bytes, sizeof(__all_users));
        __all_users[0] = __root;
    }
    return 0;
}

// note if you change this function be aware that it is used in usr_add()
int usr_set(int index, user_t user){
    // check if index valid (includes root check because index <= 0)
    if (index<=0 || index>=MAX_USERS){
        logerr("USERS: user index invalid"); return 6; }
    // check if user at index exists
    if (__all_users[index].uid >= 0) {
        logerr("USERS: user already exists"); return 6; }
    __all_users[index] = user;
    return 0;
}

int usr_add(user_t user){
    for (int i=1; i<MAX_USERS; ++i){
        if (!usr_set(i, user)) return 0;
    }
    return 1;
}

// this is a kernel root or admin only function
int usr_remove(int index){
    // still user 0 (root) cannot be removed
    if (index<=0 || index>=MAX_USERS){
        logerr("USERS: user index invalid"); return 6; }
    __all_users[index] = __usr_null;
    return 0;
}

void _usr_login(int index, user_t *user){
    *user = __all_users[index];
}

int usr_login(const char* name){
    for (int uid=0; uid<MAX_USERS; uid++){
        user_t u = __all_users[uid];
        if (u.uid<0 || u.uid != uid) continue;
        //if (strnequ(name, u.name, umin(strlen(u.name),16))){
        if (strcmd(name, u.name)){
            __login = u;
            return uid;
        }
    }
    return -1;
}

void usr_get_current(user_t *u){
    *u = __login;
}

void usr_print_name(){
    write(__login.name, umin(strlen(__login.name), 16));
}

// usr_xxxx() functions only affect the memory (aren't saved in .users file)
// so there should be a function for saving users data
int save_users_file(){
    char bytes[512];
    memcpy(bytes, __all_users, sizeof(__all_users));
    disk_write(__users_file_lba, 1, bytes);
}



int init_users_file(){
    LXFS_ENTRY* en = lxfs_open_file(lxfs_get_root_dir(), ".users");
    if(!en){ // doesn't exist
        log("USERS: user file doesn't exists");
        if (lxfs_create_new_file(lxfs_get_root_dir(), ".users", 0000, LXFS_HIDDEN, 512)){
            logerr("USERS: error creating users file");
            return 9;
        }
        en = lxfs_open_file(lxfs_get_root_dir(), ".users");
        __users_file_lba = lxfs_get_data_lba(en->lba);
        log("USERS: successfully created users file");
        return 1; // non-zero code because it's a new file and no data is contained there
    }
    __users_file_lba = lxfs_get_data_lba(en->lba);
    log("USERS: successfully loaded users file");
    lxfs_close_file(en);
    return 0;
}

