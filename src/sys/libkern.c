// Declarations of all libkern.h functions
#include <stdint.h>

// ### DRIVE MANAGING ###
// DISK0 
void    (*disk_change_drive)(uint8_t target_drive);                                           
int     (*disk_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                     
int     (*disk_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                    
// DISK1 
void    (*sdisk_change_drive)(uint8_t target_drive);                                          
int     (*sdisk_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                    
int     (*sdisk_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                   
// CDDVD
void    (*cdrom_change_drive)(uint8_t target_drive);                                          
int     (*cdrom_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                    
int     (*cdrom_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                   
// Floppy
void    (*flp_change_drive)(uint8_t target_drive);                                            
int     (*flp_read)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                      
int     (*flp_write)(uint32_t lba, uint32_t num_sectors, void* dest_buf);                     
    
    
// ### KEYBOARD MANAGING ###                                                                  
// KEYBOARD0
void    (*keyboard_function)(int fnID, int action);
void    (*keyboard_get_key)(uint16_t keycode);
