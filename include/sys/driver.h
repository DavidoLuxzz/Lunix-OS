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
    /* Install and uninstall functions all drivers should contain.
     * As the names tell, they are used to install or uninstall a driver */
    void (*install)();
    void (*uninstall)();
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
    DT_KEYBOARD0,   // Primary keyboard driver slot
    DT_KEYBOARD1,   // Secondary keyboard driver slot
    // Mouse driver types
    DT_MOUSE0,      // Primary mouse driver slot
    DT_MOUSE1,      // Secondary mouse driver slot
    // Graphics driver types
    DT_VIDEO_VGA,
    DT_VIDEO_INTEL,
    DT_VIDEO_RESERVED0,
    DT_VIDEO_RESERVED1,
    DT_VIDEO_RESERVED2,
    DT_VIDEO_RESERVED3,

    // leave as the last value
    DT_LAST
};
/* Driver type enum will be used as slots for drivers.
 * Example: If a driver has a driver type DT_FLOPPY, when installing it would be loaded
 *          to driver slot with index DT_FLOPPY and it will be set as the main driver for managing
 *          with floppies.
 *          If an another driver with type DT_FLOPPY installs, then the driver for floppies that was
 *          installed before will be uninstalled and thus replaced with a new one.
 */

/** Function for installing a driver to a slot specified in its driver type
 * Returns 0 if successful, otherwise error code */
int     driver_install(struct driver_t*);

/** Function for uninstalling a driver that was before installed */
/* It is as simple as:
 *   - invoke driver.uninstall();
 *   - free the slot used by driver
 */
void    driver_uninstall(struct driver_t*);



