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

/**
 * ELF program header loader. Simply loads program header from file into buffer
 */
int elf_get_program_header(LXFS_DIR* curdir, const char* fname, elf_ph_entry* dest);
