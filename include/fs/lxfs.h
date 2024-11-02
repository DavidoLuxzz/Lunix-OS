/** -- LunixOS default File System :: LxFS --
 * -> Tue 22 Oct 2024
 * This type of file system is created exclusivly for LunixOS
 * and it is the default Lunix'es file system.
 *
 * File system starts at offset 0x0 with node entries for root directory ('/').
 * A node can be a file, folder, volume or anything that can contain data on disk.
 * Each entry has its own name, type and data Logical Block Address (LBA).
 * If a node is a file, then its LBA address points to the raw data of the file.
 * If a node is a folder, then its LBA points to the starting sector where its all child entries are stored.
 * Files have reserved constant disk space, so its disk usage cannot grow.
 * 
 * Rules:
 *  Root directory has full entry name set to all zeroes.
 *  A folder can contain a maximum of 30 nodes + directory descrtiptor (128 bytes) = 2048 bytes.
 */

#pragma once
#include <stdint.h>
#include <disk/partition.h>

/* Definition of node entry struct
 * sizeof(struct LXFS_ENTRY) = 64 bytes */
struct LXFS_ENTRY {
    /* Name of a node. Uses 53 characters to make struct size 64 bytes */
    char name[53];
    /* Node flags (defined below) */
    uint8_t flags;
    /* Upper 4 bits: Type of a node (defined below) */
    /* Lower 9 bits: Node permissions (rwx rwx rwx) */
    uint16_t type_and_perms;
    /* If folder, size should be set to number of child entries where '.' and '..' doesn't count.
     * If file, size should be set to reserved file size.
     * Directories other than '.' should see this as unused.
     * The only dir entry that will show corrent dir size is '.' (it's just a rule) */
    uint32_t size;
    /* Logical Block Address offset of a node (address of contents stored in a node) */
    uint32_t lba;
}__attribute__((packed));
// Type definition
typedef struct LXFS_ENTRY LXFS_ENTRY;

/* Definition of directory struct
 * sizeof(struct LXFS_DIR) = 128 bytes (should be sized about two entries) */
struct LXFS_DIR {
    /* Entry descriptor */
    LXFS_ENTRY entry;
    /* Parent dir LBA */
    uint32_t parent_lba;
    /* Unused bits to make it 128 bytes */
    char unused[60];
}__attribute__((packed));
// Type definition
typedef struct LXFS_DIR LXFS_DIR;
// DEFINITIONS DEFAULT TO LXFS ENTRY
/* Node types */
#define LXFS_FILE       0 // File type node
#define LXFS_FOLDER     1 // Folder type node
#define LXFS_VOLUME     2 // Volume type node

/** Get type string
 * @return type in string
 */
static const char* _lxfs_get_type_name(uint8_t type){
    static const char* names[] = {"FILE","FOLDER","VOLUME"};
    return names[type];
}

/** Get directory name
 * @return directory name
 */
static const char* lxfs_get_dir_name(LXFS_DIR *dir){
    if (dir->entry.lba == 0) return "/";
    return dir->entry.name;
}

/* Node permissions */
#define LXFS_PERM_R     0b100 // Read node permission
#define LXFS_PERM_W     0b010 // Write node permission
#define LXFS_PERM_X     0b001 // Execute node permission

/* Node flags */
#define LXFS_VALID      0x01
#define LXFS_HIDDEN     0x02
// ...

/* Max entries in a directory */
#define LXFS_MAX_NODES  30

/* Error codes */
#define LXFS_SUCCESS    0  // Success
#define LXFS_ERR_FULL   1  // No space (full) error
#define LXFS_ERR_NOT_FOUND  2  // Not found error
#define LXFS_ERR_EXISTS     3  // Node already exists error
#define LXFS_ERR_INVL       4  // Invalid (something) error
#define LXFS_ERR_TYPE       5  // Type error (ex. rmdir a file)

/* Block(cluster) size (for files) in sectors */
#define LXFS_BLOCK_SIZE     1024

/* ### Functions ### */
/**
 * Makes given disk partition the target disk partition
 */
void lxfs_use_partition(DISK_PARTITION *disk_part_ptr);

/**
 * Gets requested entry in a given disk and partition.
 */
void _lxfs_get_entry(DISK_PARTITION *dskpart_ptr, uint32_t parent_lba, uint8_t id, LXFS_ENTRY *entry);

/**
 * Gets requested entry 
 */
void lxfs_get_entry(uint32_t parent_lba, uint8_t id, LXFS_ENTRY *entry);

/**
 * Stores requested entry to disk
 */
void lxfs_set_entry(uint32_t parent_lba, uint8_t id, LXFS_ENTRY *entry);

/**
 * Gets all entries possible from given LBA
 */
void lxfs_get_all_entries(uint32_t parent_lba, LXFS_ENTRY *entries);

/**
 * Load filesystem block bitmap
 */
void lxfs_load_filesystem(void);

/**
 * Creates new empty directory
 */
int lxfs_create_new_dir(LXFS_DIR *current, const char* name, uint16_t perms, uint8_t flags);

/**
 * Find index of entry with given entry lba
 */
uint32_t lxfs_find_entry_id_with_lba(uint32_t parent_lba, uint32_t req_lba);

/**
 * Write directory to disk
 */
void lxfs_set_dir(uint32_t parent_lba, LXFS_DIR* dir);

/**
 * Gets requested directory
 */
void lxfs_get_dir(uint32_t parent_lba, LXFS_DIR* ret);

/**
 * List free-space bitmap
 */
void lxfs_list_bitmap(void);

/**
 * Remove subdirectory in a directory with given name
 */
int lxfs_remove_dir(LXFS_DIR *currentDir, const char* name);

/**
 * Create new file
 */
int lxfs_create_new_file(LXFS_DIR *currentDir, const char* name, uint16_t perms, uint8_t flags, uint32_t filesize);

/**
 * Invalidate entry
 */
int _lxfs_invl_entry(LXFS_DIR *currentDir, int index);

/**
 * Remove subdirectory in a directory with given entry index
 */
int _lxfs_remove_dir(LXFS_DIR *current, int index);

/**
 * Remove file in a directory with given entry index
 */
int _lxfs_remove_file(LXFS_DIR *cd, int index);

/**
 * Remove file in a directory with given name
 */
int lxfs_remove_file(LXFS_DIR *current, const char* filename);




