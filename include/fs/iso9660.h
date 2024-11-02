#pragma once
#include <stdint.h>

// Volume Descriptor Type Codes
#define ISO_BOOT_RECORD         0       // Boot Record
#define ISO_PRIMARY_VOL_DESC    1       // Primary Volume Descriptor
#define ISO_SUPPL_VOL_DESC      2       // Supplementary Volume Descriptor
#define ISO_VOL_PARTITION_DESC  3       // Volume Partition Descriptor
#define ISO_RESERVED            4       // Reserved
#define ISO_TERMINATOR          255     // Volume Descriptor Set Terminator
// Primary Vol.Desc. Type Offsets
#define ISO_DESC_SYS_ID         8       // System Identifier
#define ISO_DESC_VOL_ID         40      // Volume Identifier

// A-Characters (strA)
// "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_!\"%&'()*+,-./:;<=>?"
// D-Characters (strD)
// "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
// ISO string typedefs:
typedef char ISO_STRA[];
typedef char ISO_STRD[];

// ISO path table struct definition:
struct ISO_PATH_TABLE {
    uint8_t name_len;       // 0   1   Length of Directory Identifier
    uint8_t xattr_len;      // 1   1   Extended Attribute Record Length
    uint32_t dir_lba;       // 2   4   Location of Extent (LBA). This is in a different format depending on whether this is the L-Table or M-Table (see explanation above).
    uint16_t parent;        // 6   2   Directory number of parent directory (an index in to the path table). This is the field that limits the table to 65536 records.
    ISO_STRD name;          // 8   (variable)   Directory Identifier (name) in d-characters.
}__attribute__((packed));
typedef struct ISO_PATH_TABLE ISO_PATH_TABLE;

// ISO dir record struct definition:
struct ISO_DIRECTORY {
    uint8_t size;               // Length of Directory Record.
    uint8_t xattr_len;          // Extended Attribute Record length.
    uint32_t data_lba;          // Location of extent (LBA) in both-endian format.
    uint32_t data_lba_big;
    uint32_t data_size;         // Data length (size of extent) in both-endian format.
    uint32_t data_size_big;
    const char date[7];         // Recording date and time.
    uint8_t flags;              // File flags.
    uint8_t il_unit;            // File unit size for files recorded in interleaved mode
    uint8_t il_gap;             // Interleave gap size for files recorded in interleaved mode
    uint16_t vol_seq;           // Volume sequence number
    uint16_t vol_seq_big;
    uint8_t name_len;          // Length of file name.
    ISO_STRD name;              // File name.
}__attribute__((packed));
typedef struct ISO_DIRECTORY ISO_DIRECTORY;

#include <stdio.h>
static char iso_getchar_a(char s){
    static const char* strA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_!\"%&'()*+,-./:;<=>?";
    return strA[s];
}
static char iso_getchar_d(char c){
    static const char* strD = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    return strD[c];
}

static const char* iso_get_vol_desc_type_str(uint8_t a){
    static const char* types[4] = {"Boot Record", "Primary Vol. Desc.", "Suppl. Vol. Desc.",
                                   "Volume Partition Desc"};
    if (a < 4) return types[a];
    if (a < 255) return "Reserved";
    return "VOL.Term";
}

#define ISO_PATH_TABLE_DIR_NAME 8
static const ISO_PATH_TABLE* iso_get_path_table(char* buffer, uint16_t offset){
    ISO_PATH_TABLE* pt = (ISO_PATH_TABLE*)(buffer + offset);
    return pt;
}




