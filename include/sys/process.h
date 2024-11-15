/** -- Lunix Process definition header --
 * -> Thu Nov 14 15:32
 *
 * This header contains definitions and declarations that describe a process.
 */
#pragma once
#include <stdint.h>

#define P_ST_FINISHED   0x0
#define P_ST_RUNNING    0x1

/** Lunix process header containing information of a process
 */
struct process_t {
    /* State of this process */
    uint8_t     stat;
    /* Name of this process */
    char        name[23];
    /* Physical address where is this process currently located */
    void*       addr;
    /* The virtual address where's process expected to be.
     * Used by ELF file formats */
    uint32_t    p_vaddr;
    /* The delta address of this process
     * physical address = virtual address + delta address
     */
    uint32_t    daddr; // daddr = addr - p_vaddr
};
// Type definition of process_t
typedef struct process_t process_t;
// Memory size of struct process_t is 32 bytes (sizeof(struct process_t) = 32)

/**
 * Redefines a current process to a new one. (use when switching tasks)
 */
void proc_set_current(void *process);

/**
 * Returns a pointer to a current process. Don't change the data in a returned process ptr
 */
struct process_t *proc_get_current(void);

