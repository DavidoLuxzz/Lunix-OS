#include <sys/drivers/atapi.h>
#include <stdio.h>
#include <util.h>

void ata_io_wait(const uint8_t p) {
	inb(p + ATAPI_CONTROL + ATAPI_ALTERNATE_STATUS);
	inb(p + ATAPI_CONTROL + ATAPI_ALTERNATE_STATUS);
	inb(p + ATAPI_CONTROL + ATAPI_ALTERNATE_STATUS);
	inb(p + ATAPI_CONTROL + ATAPI_ALTERNATE_STATUS);
}

// Reads sectors starting from lba to buffer
int atapi_read_cdrom(uint8_t drv, uint32_t lba, uint32_t sectors, uint16_t *buffer) {
    uint16_t port = 0x01F0;
    // The command
	volatile uint8_t read_cmd[12] = {0xA8, 0,
	                                 (lba >> 0x18) & 0xFF, (lba >> 0x10) & 0xFF, (lba >> 0x08) & 0xFF,
	                                 (lba >> 0x00) & 0xFF,
	                                 (sectors >> 0x18) & 0xFF, (sectors >> 0x10) & 0xFF, (sectors >> 0x08) & 0xFF,
	                                 (sectors >> 0x00) & 0xFF,
	                                 0, 0};

	outb(port + ATAPI_DRIVE_SELECT, 0xA0 & (drv << 4)); // Drive select
	ata_io_wait(port);
	outb(port + ATAPI_ERROR_R, 0x00);
	outb(port + ATAPI_LBA_MID, 2048 & 0xFF);
	outb(port + ATAPI_LBA_HIGH, 2048 >> 8);
	outb(port + ATAPI_COMMAND_REGISTER, 0xA0); // Packet command
	ata_io_wait(port); // I think we might need this delay, not sure, so keep this

        // Wait for status
	while (1) {
		uint8_t status = inb(port + ATAPI_COMMAND_REGISTER);
        if (status & 0x01)
			return 1;
		if (!(status & 0x80) && (status & 0x08))
			break;
		ata_io_wait(port);
	}

        // Send command
	outsw(port + ATAPI_DATA, (uint16_t *) read_cmd, 6);

        // Read words
	for (uint32_t i = 0; i < sectors; i++) {
                // Wait until ready
		while (1) {
			uint8_t status = inb(port + ATAPI_COMMAND_REGISTER);
			if (status & 0x01)
				return 2;
			if (!(status & 0x80) && (status & 0x08))
				break;
		}

		int size = inb(port + ATAPI_LBA_HIGH) << 8
		           | inb(port + ATAPI_LBA_MID); // Get the size of transfer

		insw(port + ATAPI_DATA, (uint16_t *) ((uint8_t *) buffer + i * 0x800), size / 2); // Read it
	}

	return 0;
}
