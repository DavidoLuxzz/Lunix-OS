#include <sys/commands.h>
#include <user/user.h>
#include <util.h>

__CMD_FUNC(cmd_login){
    if (argc < 1){
        logerr("no username"); return 6;
    }
    if (usr_login(command+tokens[0]) < 0){
        logerr("login error");
    }
    return 0;
}
