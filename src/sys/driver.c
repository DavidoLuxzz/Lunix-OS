#include <sys/driver.h>
#include <util.h>
#include <stdio.h>

// all global drivers info stored in memory
driver_t __DRIVERS[DS_LAST];

void **__ALL_DRIVER_FUNCTIONS[DS_LAST];

void init_driver_array(void){
    memset(__DRIVERS, 0, sizeof(__DRIVERS));
    memset(__ALL_DRIVER_FUNCTIONS, 0, sizeof(__ALL_DRIVER_FUNCTIONS));
}

int driver_install(driver_t *driver){
    printf("[ ^6DRV^_ ] Installing driver %s\n", driver->name);
    
}


void driver_uninstall(driver_t *driver){
    
}

