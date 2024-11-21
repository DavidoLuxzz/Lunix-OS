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
            *(void**)(&disk_change_drive) = driver->func0;
            *(void**)(&disk_read) = driver->func1;
            *(void**)(&disk_write) = driver->func2;
            break;
        // DS_DISK1
        case DT_ATAPI:
        case DT_ATA_AHCI:
        case DT_DISK_RESERVED1:
            *(void**)(&sdisk_change_drive) = driver->func0;
            *(void**)(&sdisk_read) = driver->func1;
            *(void**)(&sdisk_write) = driver->func2;
            break;
        // DS_CDDVD
        case DT_CDDVD:
            *(void**)(&cdrom_change_drive) = driver->func0;
            *(void**)(&cdrom_read) = driver->func1;
            *(void**)(&cdrom_write) = driver->func2;
            break;
        // DS_FLOPPY
        case DT_FLOPPY:
            *(void**)(&cdrom_change_drive) = driver->func0;
            *(void**)(&cdrom_read) = driver->func1;
            *(void**)(&cdrom_write) = driver->func2;
            break;
        // DS_KEYBOARD0
        case DT_KEYBOARD_PS2:
        case DT_KEYBOARD_STD:
            *(void**)(&keyboard_function) = driver->func0;
            *(void**)(&keyboard_get_key) = driver->func1;
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

