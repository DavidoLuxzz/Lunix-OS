#include <sys/commands.h>
#include <stdio.h>
#include <util.h>
#include <user/user.h>
#include <string.h>
#include <math.h>

__CMD_FUNC(cmd_user) {
    if (argc < 1){
        usr_print_name(); putc(0xa); return 0;
    }
    if (strcmd(command+tokens[0], "help")){
        printf("usage: user (add/remove/list) ...\n"); return 0;
    }
    if (strcmd(command+tokens[0], "add")){
        if (argc < 3) printf("usage: user add [name] [aA/mM/gG] (privlege_level)\n");
        else {
            uint8_t plevel, utype;
            switch(*(command+tokens[2])){
                case 'a':
                case 'A':
                    utype = USER_ADMIN; break;
                case 'm':
                case 'M':
                    utype = USER_MEMBER; break;
                case 'g':
                case 'G':
                    utype = USER_GUEST; break;
                default:
                    utype = USER_INVALID; break;
            }
            if (argc > 3) plevel = stoi(command+tokens[3])&0xFF;
            else plevel = utype;
            char username[16]; memset(username, 0, 16);
            memcpy(username, command+tokens[1], umin(strdlen(command+tokens[1],' '),16));
            int code = usr_add(username, utype, plevel);
            if (code==2) logerr("user already exists");
            else if (code) logerr("error adding new user");
            save_users_file();
        }
    } else if (strcmd(command+tokens[0], "remove")){
        if (argc < 2) printf("usage: user remove [user_id]\n");
        else {
            int uid = stoi(command+tokens[1]);
            usr_remove(uid);
            save_users_file();
        }
    } else if (strcmd(command+tokens[0], "list")){
        usr_print_all();
    }
    return 0;
}

