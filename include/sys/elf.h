/** -- Lunix ELF executable file format implementation --
 * -> Tue Nov 12 19:25 FIX ME
 * 
 * This header contains declarations of Lunix ELF format implementation...
 * 
 * More about ELF format can be found from the glorious INTERNET
 */
#pragma once
#include <stdint.h>
// Lunix ELF implementation uses its own file system for reading files
#include <fs/lxfs.h>

/** ELF program header entry struct
 */
struct ELF_PROGRAM_HEADER_ENTRY {
    uint32_t    p_type;
    // offset in file
    uint32_t    p_offset;
    uint32_t    p_vaddr;
    uint32_t    p_paddr;
    uint32_t    p_filesz;
    uint32_t    p_memsz;
    uint32_t    p_flags;
    uint32_t    p_align;
}__attribute__((packed));
// Type definition of ELF_PROGRAM_HEADER_ENTRY
typedef struct ELF_PROGRAM_HEADER_ENTRY elf_ph_entry;

/** ELF segment data struct, which is a simplified version of an ELF program header.
 * It goes thru calculations, so when executing a program it's useful...
 */
struct ELF_SEGMENT_DATA {
    uint32_t    s_type;
    uint32_t    s_offset;
    uint32_t    s_foffset;
    uint32_t    s_memsz;
};
// Type definition of ELF_SEGMENT_DATA
typedef struct ELF_SEGMENT_DATA elf_segment;

/** ELF program data struct
 */
struct ELF_PROGRAM_DATA {
    uint32_t    memoff;
    uint32_t    memsz;
    uint32_t    entry;
    uint32_t    p_vaddr;
    uint32_t    high_addr;
    uint32_t    s_num;      // number of segments
    elf_segment s_data[1];  // it could be more than one segment, so use kmalloc
}__attribute__((packed));
// Type definition of ELF_PROGRAM_DATA
typedef struct ELF_PROGRAM_DATA elf_prog_data;

/**
 * ELF program loader. Loads ELF program from given raw file bytes
 */
int elf_load_raw(char* bytes, elf_prog_data* dest);

/**
 * ELF program loader. Simply loads program header from file into buffer
 */
int elf_load_file(LXFS_DIR* curdir, const char* fname, elf_prog_data* dest);

/**
 * Get number of segments in program header
 * @param bytes - raw bytes of file
 */
uint32_t elf_get_num_segments(char* bytes);







