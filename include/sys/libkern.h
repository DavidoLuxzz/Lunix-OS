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
extern void    (*disk_change_drive)(uint8_t target_drive);
extern int     (*disk_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);
extern int     (*disk_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);
// DISK1
extern void    (*sdisk_change_drive)(uint8_t target_drive);
extern int     (*sdisk_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);
extern int     (*sdisk_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);
// CDDVD
extern void    (*cdrom_change_drive)(uint8_t target_drive);
extern int     (*cdrom_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);
extern int     (*cdrom_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);
// Floppy
extern void    (*flp_change_drive)(uint8_t target_drive);
extern int     (*flp_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);
extern int     (*flp_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);


// ### KEYBOARD MANAGING ###
// KEYBOARD0
extern int     (*keyboard_function)(int fnID, int action); // driver depended
extern bool    (*keyboard_get_key)(uint16_t keycode);

// ... mrzi me da nastavim imam i istoriju da ucim tkd
// naravno, nisam ucio .....
