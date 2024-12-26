#include <sys/driver.h>
#include <util.h>
#include <stdio.h>
#include <sys/libkern.h>
#include <sys/kmalloc.h>

// all global drivers info stored in memory
driver_t __DRIVERS[DS_LAST];

void init_driver_array(void){
    memset(__DRIVERS, 0xFF, sizeof(driver_t)*DS_LAST);
}

int driver_install(const driver_t *driver){
    printf("[ ^6DRV^_ ] Installing driver %s\n", driver->name);
    int slot = driver_get_slot(driver->type);
    if (slot >= DS_LAST){
        printf("[ ^6DRV^4 ERR^_ ] Invalid driver slot from type %u\n", driver->type&0xFFFF);
        return 6;
    }
    switch (slot){
        // DS_DISK0
        case DS_DISK0:
            asif(disk_change_drive, driver->func0);
            asif(disk_read, driver->func1);
            asif(disk_write, driver->func2);
            break;
        // DS_DISK1
        case DS_DISK1:
            asif(sdisk_change_drive, driver->func0);
            asif(sdisk_read, driver->func1);
            asif(sdisk_write, driver->func2);
            break;
        // DS_CDDVD
        case DS_CDDVD:
            asif(cdrom_change_drive, driver->func0);
            asif(cdrom_read, driver->func1);
            break;
        // DS_FLOPPY
        case DS_FLOPPY:
            asif(flp_change_drive, driver->func0);
            asif(flp_read, driver->func1);
            asif(flp_write, driver->func2);
            break;
        // DS_KEYBOARD0
        case DS_KEYBOARD0:
            asif(keyboard_function, driver->func0);
            asif(keyboard_get_key, driver->func1);
            break;     
        // invalid
        case DS_INVALID:
        default:
            printf("[ ^6DRV^4 ERR^_ ] Invalid driver type %u (%s)\n", driver->type&0xFFFF, driver->name);
            return 6;
    }
    __DRIVERS[slot] = *driver;
    driver->init();
    return 0;
}

void driver_uninstall(const driver_t *driver){
    printf("[ ^6DRV^_ ] Uninstalling driver %s\n", driver->name);
    int slot = driver_get_slot(driver->type);
    if (slot >= DS_LAST) {
        printf("[ ^6DRV^_ ] Invalid driver. Cannot be uninstalled [%s]\n", driver->name);
        return;
    }
    switch (slot){
        // DS_DISK0
        case DS_DISK0:
            desif(disk_change_drive, driver->func0);
            desif(disk_read, driver->func1);
            desif(disk_write, driver->func2);
            break;
        // DS_DISK1
        case DS_DISK1:
            desif(sdisk_change_drive, driver->func0);
            desif(sdisk_read, driver->func1);
            desif(sdisk_write, driver->func2);
            break;
        // DS_CDDVD
        case DS_CDDVD:
            desif(cdrom_change_drive, driver->func0);
            desif(cdrom_read, driver->func1);
            break;
        // DS_FLOPPY
        case DS_FLOPPY:
            desif(flp_change_drive, driver->func0);
            desif(flp_read, driver->func1);
            desif(flp_write, driver->func2);
            break;
        // DS_KEYBOARD0
        case DS_KEYBOARD0:
            desif(keyboard_function, driver->func0);
            desif(keyboard_get_key, driver->func1);
            break;
            break;
        // invalid
        default:
            break;
    }
    __DRIVERS[slot].type = DT_INVALID;
    __DRIVERS[slot].init = (void(*)()) NULL;
}
// user request uninstall driver
int driver_req_uninstall(int id){
    driver_t *d = &__DRIVERS[id];
    driver_uninstall(d);
    return 0;
}

int driver_get_slot(uint16_t type){
    switch (type){
        // DS_DISK0
        case DT_ATA_PIO:
        case DT_ATA_IDE:
        case DT_DISK_RESERVED0:
            return DS_DISK0;
        // DS_DISK1
        case DT_ATAPI:
        case DT_ATA_AHCI:
        case DT_DISK_RESERVED1:
            return DS_DISK1;
        // DS_CDDVD
        case DT_CDDVD:
            return DS_CDDVD;
        // DS_FLOPPY
        case DT_FLOPPY:
            return DS_FLOPPY;
        // DS_KEYBOARD0
        case DT_KEYBOARD_PS2:
        case DT_KEYBOARD_STD:
            return DS_KEYBOARD0;
        
        case DT_INVALID:
        default:
            return DS_INVALID;
    }
    return DS_INVALID;
}

const char* driver_get_type_name(uint16_t type){
    static const char* names[] = {"DT_INVALID","DT_ATA_PIO","DT_ATA_IDE","DT_ATA_AHCI","DT_ATAPI","DT_CDDVD","DT_FLOPPY","DT_RESV0","DT_RESV1","DT_KEYBOARD_PS2", "DT_KEYBOARD_STD", "DT_STD_MOUSE","DT_VIDEO_VGA","DT_VIDEO_INTEL","DT_VIDEO_MDV","DT_VIDEO_RESV1","DT_VIDEO_RESV2","DT_VIDEO_RESV3","DT_FS_LXFS","DT_FS_ISO9660","DT_FS_FAT12","DT_FS_FAT16","DT_FS_FAT32","DT_FS_EXFAT","DT_FS_NTFS","DT_FS_EXT","DT_FS_EXT2","DT_FS_EXT3","DT_FS_EXT4","DT_FS_XFS","DT_FS_ZFS","null","null","null"};
    return names[type];
}

// more like stuff for 3rd party programs //

driver_t *driver_create_list_installed_drivers(int* drlist_size){
    int drs[DS_LAST]; int drc=0;
    for (int i=0; i<DS_LAST; i++){
        uint16_t type = __DRIVERS[i].type;
        if (type && type < DT_LAST) drs[drc++] = i;
    }
    driver_t *drlist = (driver_t*) kmalloc(sizeof(driver_t)*drc);
    for (int i=0; i<drc; i++){
        drlist[i] = __DRIVERS[drs[i]];
    }
    *drlist_size = drc;
    return drlist;
}

void driver_delete_list_installed_drivers(void* a){
    kfree(a);
}


