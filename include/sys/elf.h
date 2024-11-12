/** -- Lunix ELF executable file format implementation --
 * -> Tue Nov 12 19:25 FIX ME
 * 
 * This header contains declarations of Lunix ELF format implementation...
 * 
 * More about ELF format can be found from the glorious INTERNET
 */
#pragma once
#include <stdint.h>

/** ELF program header entry struct
 */
struct ELF_PROGRAM_H {
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
// Type definition of ELF_PROGRAM_H
typedef struct ELF_PROGRAM_H ELF_PROGRAM_H;


