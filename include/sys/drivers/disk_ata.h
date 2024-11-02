#pragma once
#include <stdint.h>

void ata_disk_read(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer);
uint8_t ata_disk_write(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer);

void ata_disk_read2(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer);










