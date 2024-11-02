#pragma once
#include <stdint.h>

void __os_iso_vol_desc(uint8_t drive);
uint32_t __os_iso_get_pt_addr(uint8_t drive);
uint32_t __os_iso_get_root_addr(uint8_t);
uint8_t __os_iso_dir_name(uint8_t drive, uint32_t lba, uint16_t offset);
void __os_iso_list_dir(uint8_t drive, uint32_t lba);
void __os_iso_dir_info(uint8_t drive, uint32_t lba, uint16_t offset);
uint32_t __os_iso_get_vol_size(uint8_t drive);

void __os_iso_path_table(uint8_t drive, uint32_t addr, uint16_t offset);
void __os_iso_dir_table(uint8_t, uint32_t, uint16_t);

/**
 * dst_drv - destination drive where will OS be installed
 * src_drv - source drive from where OS will be installed
 * src_type - (0: ATA, 1: ATAPI), only ATAPI works for now
 * lba - where will installation begin (used only if partitioning in use, otherwise zero)
 */
void __os_install_to_disk(uint8_t dst_drv, uint8_t src_drv, uint16_t src_type, uint32_t lba);























