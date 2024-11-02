#pragma once
#include <stdint.h>

#define ATAPI_DATA 0
#define ATAPI_ERROR_R 1
#define ATAPI_SECTOR_COUNT 2
#define ATAPI_LBA_LOW 3
#define ATAPI_LBA_MID 4
#define ATAPI_LBA_HIGH 5
#define ATAPI_DRIVE_SELECT 6
#define ATAPI_COMMAND_REGISTER 7

// Control register defines
#define ATAPI_CONTROL 0x206

#define ATAPI_ALTERNATE_STATUS 0

void ata_io_wait(const uint8_t p);

int atapi_read_cdrom(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer);







