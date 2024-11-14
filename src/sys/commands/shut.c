#include <sys/commands.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

__CMD_FUNC(cmd_shut) {
    if (argc < 1) {
        printf("Choose shutdown method:\n1: todo\n2: QEMU\n$ ");
        char ans[10];
        nread(ans,1);
        switch(ans[0]&0xFF - '0'){
            case 1: goto __todo_shutdown_label;
            case 2: goto __qemu_shutdown_label;
            default: goto __invalid_shutdown_label;
        }
    } else if (strcmd(command+tokens[0], "--todo")){
__todo_shutdown_label:
        printf("Do you know what TO DO means?? Try 'exit' command instead\n");
    } else if (strcmd(command+tokens[0], "--qemu")){
__qemu_shutdown_label:
        printf("Sending shutdown signal... (QEMU)\n");
        outw(0x604, 0x2000);
    } else {
__invalid_shutdown_label:
        printf("Unknown shutdown option: %s\n",command+tokens[0]);
    }
    return 0;
}

