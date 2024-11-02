/** -- LunixOS system partitioning header --
 * -> Sat 19 Oct 2024
 * LunixOS does NOT use the standard partition tables (ex. GPT).
 * It uses a custom partitioning named as Lunix Partition Table (aka. LPT).
 *
 * First non-user-modifiable partition is Boot Partition with an index 0 (Part. 0)
 *
 * Boot Partition also contains the whole partition table, which is located at hex address 0x200 or LBA #1
 * LBA #0 (0x0) is used as boot sector containing the built-in bootloader (mainly GRUB).
 * The Boot Partition is simplified a copy of an .ISO file created by grub-mkrescue, so it is using the ISO9660 filesystem.
 *
 * Boot Partition layout:
 * LBA# |
 *   0  .------------------------.
 *      |       Boot sector      |
 *   1  |------------------------|
 *      |        LPT Table       |
 *      |          . . .         |
 *      |          . . .         |
 *  16  |------------------------|
 *      | ISO Primary Vol. Desc. |
 *  17  |------------------------|
 *      |    Other ISO data...   |
 * END  '------------------------'
 *
 */

#pragma once
#include <stdint.h>

/** Definition of Partition Record struct
 *  sizeof(struct PARTITION) = 32 bytes */
struct PARTITION {
    /* Name of a partition. Uses 18 characters to make partition size 32 bytes. */
    char name[18];
    /* Type of a partition (defined below)*/
    uint8_t type;
    /* Filesystem ID of a partition (defined below) */
    uint8_t fsID;
    /* Magic number which makes partition valid, or not (defined below) */
    uint32_t magic;
    /* Disk usage space used by partition in sectors
     * (which means that minimum partition size is size of a sector on disk, usualy 512 bytes) */
    uint32_t space;
    /* Logical Block Address of partition (address of contents stored in a partition) */
    uint32_t lba;
}__attribute__((packed));
// Type definition (use [PARTITION p] instead of [struct PARTITION p])
typedef struct PARTITION PARTITION;

/** Definition of Partition on specific drive struct
 *  sizeof(struct DISK_PARTITION) = 1 + sizeof(struct PARTITION) bytes */
struct DISK_PARTITION {
    /* Drive which contains partition */
    uint8_t drive;
    /* Partition information */
    PARTITION part;
}__attribute__((packed));
// Type definition if struct DISK_PARTITION
typedef struct DISK_PARTITION DISK_PARTITION;

// DEFINITIONS DEFAULT TO PARTITION
/* File systems */
// TODO napraviti da drajveri mogu dodavati svoje fajl sisteme
#define FILESYSTEM_ISO9660  0 // ISO9660 file system. Used by CD-ROMs and Boot Partition
#define FILESYSTEM_LxFS     1 // Lunix File System. Default Lunix filesystem
#define FILESYSTEM_FAT32    2 // FAT file system. Not usable in Lunix unless a propriate driver is made

static const char* _get_partition_fs_name(uint8_t fs){
    static const char* names[] = {"ISO9660","LxFS","FAT32"};
    return names[fs];
}

/* Partition types */
#define PARTYPE_BOOT    0 // Boot partition. Only one Boot Partition should exists.
                          // All system code + bootloader and boot content data stored in a partition this type
#define PARTYPE_USER    1 // Normal partition. All user files are stored in a partition with this type
#define PARTYPE_KERN    2 // Kernel only partition. Will lead to removal if not needed

/* Magic (validator) number */
#define PARTITION_MAGIC 0x54444943 // 32-bit int number, translated to ascii it is "TDIC"

/* Lunix Partition Table start sector */
#define LPT_SECTOR      0x1

/* LPT max partitions */
#define MAX_PARTITIONS  0xF // 16 - 1

/* Partition byte size */
#define PARTITION_STRUCT_SIZE   32 // bytes


/* ### Functions ### */
/**
 * Gets partition info data from drive and writes it to given pointer
 */
void _get_partition(uint8_t drive, uint16_t id, struct PARTITION* partition_ptr);

/**
 * Checks if the given partition is valid.
 * @return true if valid (partition.magic == PARTITION_MAGIC)
 */
bool _is_partition_valid(struct PARTITION* partition_ptr);

/**
 * Stores given partition on disk
 */
void _set_partition(uint8_t drive, uint16_t partition_id, struct PARTITION* partition_ptr);

/**
 * Partitioning with a Graphics User Interface
 */
int _gui_partition(uint8_t drv);



