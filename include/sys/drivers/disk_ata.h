#pragma once
#include <stdint.h>
#include <sys/driver.h>

void ata_disk_init(void);
void ata_change_drive(uint8_t);

const driver_t *ata_get_driver(void);

void _ata_disk_read(uint8_t drv, uint32_t sector, uint32_t num_sectors, void *buffer);
uint8_t _ata_disk_write(uint8_t drv, uint32_t sector, uint32_t num_sectors, void *buffer);

void ata_disk_read(uint32_t sector, uint32_t num_sectors, void *buffer);
uint8_t ata_disk_write(uint32_t sector, uint32_t num_sectors, void *buffer);

void ata_disk_read2(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer);










