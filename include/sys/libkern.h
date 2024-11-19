/** -- Lunix kernel function declaration header --
 * -> Tue Nov 19 11:52
 *
 * All the system functions declared here.
 * Definitions are mainly implemented in system drivers.
 */
#pragma once
#include <stdint.h>

// ### DRIVE MANAGING ###
// DISK0
void    disk_init(void); // number of params vary depending on a driver
void    disk_change_drive(uint8_t target_drive);
int     disk_read(uint32_t lba, uint32_t num_sectors, void* dest_buf);
int     disk_write(uint32_t lba, uint32_t num_sectors, void* dest_buf);
// DISK1
void    sdisk_init(...); // number of params vary depending on a driver
void    sdisk_change_drive(uint8_t target_drive);
int     sdisk_read(uint32_t lba, uint32_t num_sectors, void* dest_buf);
int     sdisk_write(uint32_t lba, uint32_t num_sectors, void* dest_buf);
// CDDVD
void    cdrom_init(...); // number of params vary depending on a driver
void    cdrom_change_drive(uint8_t target_drive);
int     cdrom_read(uint32_t lba, uint32_t num_sectors, void* dest_buf);
int     cdrom_write(uint32_t lba, uint32_t num_sectors, void* dest_buf);
// Floppy
void    flp_init(...); // number of params vary depending on a driver
void    flp_change_drive(uint8_t target_drive);
int     flp_read(uint32_t lba, uint32_t num_sectors, void* dest_buf);
int     flp_write(uint32_t lba, uint32_t num_sectors, void* dest_buf);


// ### KEYBOARD MANAGING ###
// KEYBOARD0
void    keyboard_init(void);
void    keyboard_get_key(uint16_t keycode);

// ... mrzi me da nastavim imam i istoriju da ucim tkd
