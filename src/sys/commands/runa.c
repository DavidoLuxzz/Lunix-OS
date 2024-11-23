#include <sys/commands.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>

__CMD_FUNC(cmd_runa) {
    // running program files from disk to certain address
    if (argc < 2) {
        printf("usage: runa [execname] [address (hex)]\n"); return 0;
    }
    char fname[tokens[1]-tokens[0]];
    strncpy(fname, command+tokens[0], tokens[1]-tokens[0]-1);
    uint32_t addr = stox(command+tokens[1]);
    __execa(fname, addr);
    return 0;
}
