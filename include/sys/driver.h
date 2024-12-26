/** -- Lunix driver header --
 * -> Sun Nov 17 11:11
 *
 * This header contains a definition of driver_t struct,
 * and declarations of functions for installing and uninstalling that driver.
 */
#pragma once
#include <stdint.h>

/** Structure for driver information
 */
struct driver_t {
    /* Name of a driver. Limited to 24 characters,
     * which is just decided to be that much (can change) */
    char name[24];
    /* Type of a driver, which is one in driver type set all defined below */
    uint16_t type;
    /* Every driver oftern needs to be initialized before use.
     * Will be invoked on every installation. So it would be better to be valid.  */
    void (*init)();
    /* Driver functions that will be set as target redirect functions.
     * ex. When calling ata_disk_read(), func0() will be called instead.
     * When installing a driver, global function address will be set as the driver's */
    void *func0;
    void *func1;
    void *func2;
    void *func3;
    void *func4;
    void *func5;
    void *func6;
    void *func7;
};
// Type definition of driver_t struct
typedef struct driver_t driver_t;

/* Driver type set.
 * All definitions start with a prefix 'DT' */
enum driver_type_enum {
    DT_INVALID,
    // Disk/CD-ROM/Floppy/... type driver types
    DT_ATA_PIO,
    DT_ATA_IDE,
    DT_ATA_AHCI,
    DT_ATAPI,
    DT_CDDVD,
    DT_FLOPPY,
    DT_DISK_RESERVED0,
    DT_DISK_RESERVED1,
    // Keyboard driver types
    DT_KEYBOARD_PS2,
    DT_KEYBOARD_STD,
    // Mouse driver types
    DT_STD_MOUSE,
    // Graphics driver types
    DT_VIDEO_VGA,
    DT_VIDEO_INTEL,
    DT_VIDEO_MDV,
    DT_VIDEO_RESERVED1,
    DT_VIDEO_RESERVED2,
    DT_VIDEO_RESERVED3,
    // Filesystem driver types
    DT_FS_LXFS,
    DT_FS_ISO9660,
    DT_FS_FAT12,
    DT_FS_FAT16,
    DT_FS_FAT32,
    DT_FS_EXFAT,
    DT_FS_NTFS,
    DT_FS_EXT,
    DT_FS_EXT2,
    DT_FS_EXT3,
    DT_FS_EXT4,
    DT_FS_XFS,
    DT_FS_ZFS,

    // leave for last
    DT_LAST
};

/* All driver slots enum 
 * Definitions start with prefix 'DS' */
enum driver_slots_enum {
    // Drive job drivers
    DS_DISK0,
    DS_DISK1,
    DS_CDDVD,
    DS_FLOPPY,
    // Keyboard driver slots (2 slots for max. of 2 keyboards at the same time)
    DS_KEYBOARD0,
    DS_KEYBOARD1,
    // Mouse driver slots (same as keyboard)
    DS_MOUSE0,
    DS_MOUSE1,
    // Video graphics driver slot
    DS_VIDEO,
    // Middle-degree video driver slot
    DS_MDVIDEO,
    // Filesystem driver slots for a max. of 4 filesystems open at once
    DS_FILESYSTEM0,
    DS_FILESYSTEM1,
    DS_FILESYSTEM2,
    DS_FILESYSTEM3,

    // leave for last
    DS_LAST
};
#define DS_INVALID  0xFFFFFFF
/* Driver slots enum will be used as slots for drivers.
 * Example: If a driver has a driver type DT_FLOPPY, when installing it would be loaded
 *          to driver slot with index DT_FLOPPY and it will be set as the main driver for managing
 *          with floppies.
 *          If an another driver with type DT_FLOPPY installs, then the driver for floppies that was
 *          installed before will be uninstalled and thus replaced with a new one.
 */

// Type definition of function array
typedef void(* FUNCARRAY)();

/** Function for initializing the global driver slots array.
 * Will be set all to zero (0). */
void    init_driver_array(void);

/** Function for installing a driver to a slot specified in its driver type
 * Returns 0 if successful, otherwise error code */
int     driver_install(const struct driver_t*);

/** Function for uninstalling a driver that was before installed */
/* It is as simple as:
 *   - invoke driver.uninstall();
 *   - free the slot used by driver
 */
void    driver_uninstall(const struct driver_t*);

/** Function for uninstalling a driver without access to main drivers list (which only kernel has)
 * Checks if has enough access to do so, thats why the function name is driver_req_uninstall (short for driver request uninstall).
 * @param id - the slot where driver is installed
 */
int     driver_req_uninstall(int id);

/** Function for deciding which driver type belongs to which slot
 */
int     driver_get_slot(uint16_t type);

/** Function for returning driver type name
 */
const char* driver_get_type_name(uint16_t type);

/** Function for creating a list of installed drivers.
 * Returns driver list buffer.
 * @param driver_list_size - size of driver list buffer.
 * Note that this list is create via allocation (kmalloc) and should be freed
 */
driver_t    *driver_create_list_installed_drivers(int *driver_list_size);

/** Function for freeing a list of installed drivers.
 */
void        driver_delete_list_installed_drivers(void*);







