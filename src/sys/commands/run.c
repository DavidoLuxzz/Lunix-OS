#include <sys/commands.h>
#include <sys/syscall.h>
#include <stdio.h>

__CMD_FUNC(cmd_run) {
    // running program files from disk
    if (argc < 1) {
        printf("usage: run [execname]\n"); return 0;
    }
    __newproc(command+tokens[0]);
    return 0;
}


