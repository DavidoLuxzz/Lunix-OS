#include <sys/elf.h>
#include <sys/drivers/disk_ata.h>
#include <util.h>
#include <math.h>
#include <string.h>
#include <sys/kmalloc.h>
#include <stdio.h>
#include <unistd.h>

int elf_get_program_header(LXFS_DIR* curdir, const char* fname, elf_ph_entry* dest){
    LXFS_ENTRY* fe = lxfs_open_file(curdir, fname);

    uint32_t lba  = lxfs_get_data_lba(fe->lba); // absolute LBA
    uint32_t size = fe->size; // sectors

    lxfs_close_file(fe);
    if (!size) { logerr("file is invalid size"); return 6; }
    
    uint32_t he_size = min(4096, size*512);
    char* bytes = kmalloc(he_size);
    if (!bytes) { logerr("no memory space left"); return 6; }

    ata_disk_read(0, lba, size, bytes);
    // verify ELF
    if (!(*bytes&0xFF==0x7F || strnequ(bytes+1, "ELF", 3))) {
        logerr("not an ELF format"); return 6;
    }
    // if gud find program header
    uint32_t e_phoff = *(uint32_t*)(bytes+0x1C);
    memcpy(dest, bytes+e_phoff, sizeof(elf_ph_entry));
}





