#include <sys/driver.h>
#include <util.h>
#include <stdio.h>
#include <sys/libkern.h>

// all global drivers info stored in memory
driver_t __DRIVERS[DS_LAST];

void init_driver_array(void){
    memset(__DRIVERS, 0, sizeof(__DRIVERS));
}

int driver_install(const driver_t *driver){
    printf("[ ^6DRV^_ ] Installing driver %s\n", driver->name);
    switch (driver->type){
        // DS_DISK0
        case DT_ATA_PIO:
        case DT_ATA_IDE:
        case DT_DISK_RESERVED0:
            asif(disk_change_drive, driver->func0);
            asif(disk_read, driver->func1);
            asif(disk_write, driver->func2);
            break;
        // DS_DISK1
        case DT_ATAPI:
        case DT_ATA_AHCI:
        case DT_DISK_RESERVED1:
            asif(sdisk_change_drive, driver->func0);
            asif(sdisk_read, driver->func1);
            asif(sdisk_write, driver->func2);
            break;
        // DS_CDDVD
        case DT_CDDVD:
            asif(cdrom_change_drive, driver->func0);
            asif(cdrom_read, driver->func1);
            break;
        // DS_FLOPPY
        case DT_FLOPPY:
            asif(flp_change_drive, driver->func0);
            asif(flp_read, driver->func1);
            asif(flp_write, driver->func2);
            break;
        // DS_KEYBOARD0
        case DT_KEYBOARD_PS2:
        case DT_KEYBOARD_STD:
            asif(keyboard_function, driver->func0);
            asif(keyboard_get_key, driver->func1);
            break;


        default:
            printf("[ ^6DRV^4 ERR^_ ] Invalid driver type %u\n", driver->type&0xFFFF);
            return 6;
    }
    driver->init();
    return 0;
}

void driver_uninstall(const driver_t *driver){
    printf("[ ^6DRV^_ ] Uninstalling driver %s\n", driver->name);
}

