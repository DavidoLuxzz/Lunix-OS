#include <sys/driver.h>
#include <util.h>
#include <stdio.h>
#include <sys/libkern.h>

// all global drivers info stored in memory
driver_t __DRIVERS[DS_LAST];

FUNCARRAY *__ALL_DRIVER_FUNCTIONS[DS_LAST*8];

void adf_set(int slot, int index, void* fn){
    __ALL_DRIVER_FUNCTIONS[slot*8 + index] = (FUNCARRAY*) &fn;
}

void init_driver_array(void){
    memset(__DRIVERS, 0, sizeof(__DRIVERS));
    memset(__ALL_DRIVER_FUNCTIONS, 0, sizeof(__ALL_DRIVER_FUNCTIONS));
    // DISK0
    adf_set(DS_DISK0, 0, disk_change_drive);
    adf_set(DS_DISK0, 1, disk_read);
    adf_set(DS_DISK0, 2, disk_write);
    // DISK1
    adf_set(DS_DISK1, 0, sdisk_change_drive);
    adf_set(DS_DISK1, 1, sdisk_read);
    adf_set(DS_DISK1, 2, sdisk_write);
    // CDDVD
    adf_set(DS_CDDVD, 0, cdrom_change_drive);
    adf_set(DS_CDDVD, 1, cdrom_read);
    adf_set(DS_CDDVD, 2, cdrom_write);
}

int driver_install(const driver_t *driver){
    printf("[ ^6DRV^_ ] Installing driver %s\n", driver->name);
    switch (driver->type){
        default:
            disk_change_drive = (FUNCARRAY) driver->func0;
            disk_read = (FUNCARRAY) driver->func1;
            disk_write = (FUNCARRAY) driver->func2;
            break;
    }
    driver->init();
}

void driver_uninstall(const driver_t *driver){
    printf("[ ^6DRV^_ ] Uninstalling driver %s\n", driver->name);
}

