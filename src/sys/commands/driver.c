#include <sys/commands.h>
#include <sys/driver.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

__CMD_FUNC(cmd_driver){
    if (argc < 1){
__print_usage:
        printf("usage: driver [help/list/uninstall] ...\n");
        return 0;
    }
    if (strcmd(command+tokens[0], "help")){
        goto __print_usage;
    } else if (strcmd(command+tokens[0], "list")){
        int dlsize;
        driver_t *drivers = driver_create_list_installed_drivers(&dlsize);
        
        printf("Installed drivers (%u)\n", dlsize);
        for (int i=0; i<dlsize; i++){
            int type = drivers[i].type;
            printf(" - %s ==== (%s) (slot %u)\n", drivers[i].name, driver_get_type_name(type), driver_get_slot(type));
        }

        driver_delete_list_installed_drivers(drivers);
    } else if (strcmd(command+tokens[0], "uninstall")){
        if (argc < 2) logerr("driver slot index missing::no enough arguments");
        else {
            int slot = stoi(command+tokens[1]);
            driver_req_uninstall(slot);
        }
    }
    return 0;
}
