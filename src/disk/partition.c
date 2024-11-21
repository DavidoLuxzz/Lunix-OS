#include <disk/partition.h>
#include <sys/drivers/disk_ata.h>
#include <util.h>
#include <stdio.h>

void _get_partition(uint8_t drv, uint16_t id, PARTITION* part){
    uint16_t _rbuf[256];
    _ata_disk_read(drv, LPT_SECTOR, 1, _rbuf);
    PARTITION* _pbuf = (PARTITION*) _rbuf + id;
    memcpy(part, _pbuf, PARTITION_STRUCT_SIZE);
}

bool _is_partition_valid(PARTITION* part){
    return part->magic == PARTITION_MAGIC;
}

void _set_partition(uint8_t drv, uint16_t id, PARTITION* part){
    printf("part id: %u\n", id);
    uint16_t _rbuf[256];
    _ata_disk_read(drv, LPT_SECTOR, 1, _rbuf);

    PARTITION* all = (PARTITION*) _rbuf;
    memcpy(all+id, part, PARTITION_STRUCT_SIZE);

    _ata_disk_write(drv, LPT_SECTOR, 1, (uint16_t*)all);
}




#include <vga.h>
#include <unistd.h>
#include <terminal.h>
#include <keyboard.h>
#include <sys/core_asm.h>

int _gui_partition(uint8_t drv){
    uint32_t eip = _getip();
    PARTITION parts[MAX_PARTITIONS];
    _ata_disk_read(drv, LPT_SECTOR, 1, (uint16_t*) parts);

    clear_color(0x80);
    
    // end
    clear_screen();
    
    return 0;
}
