#include <sys/driver.h>
#include <util.h>
#include <stdio.h>

// all global drivers info stored in memory
driver_t __DRIVERS[DS_LAST];

void init_driver_array(void){
    memset(__DRIVERS, 0, sizeof(__DRIVERS));
}

int driver_install(driver_t *driver){
    
}


void driver_uninstall(driver_t *driver){
    
}

