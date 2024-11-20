#include <sys/elf.h>
#include <sys/libkern.h>
#include <util.h>
#include <math.h>
#include <string.h>
#include <sys/kmalloc.h>
#include <stdio.h>
#include <unistd.h>

#define ELF_DEBUG_LOG 0

uint32_t elf_get_num_segments(char* bytes){
    return *(uint32_t*)(bytes+0x2C);
}

int elf_load_raw(char* bytes, elf_prog_data* dest){
    // verify ELF
    if (*(uint32_t*)(bytes) != 0x464c457f) { // 7F + "ELF"
        logerr("not an ELF format"); return 6;
    }
    // check if file is executable
    uint16_t e_type = *(uint16_t*)(bytes+0x10);
    if (e_type != 0x0002){
        logerr("not an executable"); return 6;
    }
    // if gud find program header
    uint32_t e_phoff = *(uint32_t*)(bytes+0x1C);
    uint16_t e_phnum = *(uint16_t*)(bytes+0x2C);
    uint32_t e_entry = *(uint32_t*)(bytes+0x18); // entry point in memory where the program starts
    
    elf_ph_entry pes[e_phnum];
    memcpy(pes, bytes+e_phoff, sizeof(elf_ph_entry)*e_phnum);
    // from where to where to copy when executing
    uint32_t fstart = 0xFFFFFFFF;
    uint32_t fend   = 0x0;
    // find "start vaddr"
    uint32_t fvaddr = 0xFFFFFFFF;
    uint32_t fahigh = 0x0;
    for (uint16_t i=0; i<e_phnum; ++i){
        uint32_t flags = pes[i].p_flags;
        if (ELF_DEBUG_LOG) {
            printf("Segment %u:\nType: %u\n", i, pes[i].p_type);
            if (flags & 1) printf("E");
            if (flags & 2) printf("W");
            if (flags & 4) printf("R");
            putc(0xa);
            
            printf("Segment memory space: 0x%x bytes\n", pes[i].p_memsz);
            printf("Segment offset in file: 0x%x bytes\n", pes[i].p_offset);
            printf("Segment planned memory address: %p\n", pes[i].p_vaddr);
        }
        fvaddr = umin(fvaddr, pes[i].p_vaddr);
        fstart = umin(fstart, pes[i].p_offset);
        fend   = umax(fend,   pes[i].p_offset+pes[i].p_filesz);
        fahigh = umax(fahigh, pes[i].p_vaddr +pes[i].p_memsz);
        
        elf_segment seg;
        seg.s_type = pes[i].p_type;
        seg.s_offset = pes[i].p_vaddr;
        seg.s_foffset = pes[i].p_offset;
        seg.s_memsz = pes[i].p_memsz;
        memcpy(&dest->s_data[i], &seg, sizeof(elf_segment));
    }
    dest->p_vaddr = fvaddr;
    dest->memsz   = fahigh - fvaddr;
    dest->entry   = e_entry-fvaddr;
    dest->memoff  = fstart;
    dest->s_num   = e_phnum&0xFFFF;
    for (uint16_t i=0; i<e_phnum; ++i){
        dest->s_data[i].s_offset -= fvaddr;
        if (ELF_DEBUG_LOG) printf("segment s_offset: %p\n", dest->s_data[i].s_offset);
    }
    return 0;
}

int elf_load_file(LXFS_DIR* curdir, const char* fname, elf_prog_data* dest){
    LXFS_ENTRY* fe = lxfs_open_file(curdir, fname);

    uint32_t lba  = lxfs_get_data_lba(fe->lba); // absolute LBA
    uint32_t size = fe->size; // sectors

    lxfs_close_file(fe);
    if (!size) { logerr("file is invalid size"); return 6; }
    
    uint32_t he_size = min(4096, size*512);
    char* bytes = kmalloc(he_size);
    if (!bytes) { logerr("no memory space left"); return 6; }

    disk_read(0, lba, size, bytes);
    
    return elf_load_raw(bytes, dest);
}





