#include <os/install.h>
#include <sys/drivers/disk_h.h> // ide_atapi_read(drv,lba,no_sects,0,buffer)
#include <sys/libkern.h> // disk_write(lba, no_sects, buffer)
#include <stdio.h>
#include <unistd.h>
#include <sys/kmalloc.h>
#include <util.h>
#include <fs/iso9660.h>

extern uint32_t _kernel_end;

void __os_iso_vol_desc(uint8_t drive){
    uint16_t _r_buffer[1024];
    memset(_r_buffer, 0, 2048);
    ide_atapi_read(drive, 0x10, 1, 0, _r_buffer);
    
    printf("type: %s\n", iso_get_vol_desc_type_str(_r_buffer[0]&0xFF));
    char* _buf = (char*)_r_buffer;
    printf("identifier: ");
    write(_buf+1, 5);
    printf("\nVolume Desc. v: 0x%x\n", *(_buf+6));
    
    printf("volume identifier: ");
    write(_buf+40, 32);
    printf("\n");
    
    uint32_t _size = *((uint32_t*) (_buf+80));
    printf("size (sectors): %u\n", _size);

    uint32_t _path_table_lba = *((uint32_t*) (_buf+140));
    uint32_t _pt_size = *((uint32_t*) (_buf+132));
    printf("path table at: %u sector, with size: %u bytes\n", _path_table_lba, _pt_size);
}

uint32_t __os_iso_get_vol_size(uint8_t drive){
    uint16_t _rbuf[1024];
    ide_atapi_read(drive, 0x10, 1, 0, _rbuf);
    char* _buf = (char*) _rbuf;
    uint32_t _size = *((uint32_t*) (_buf+80));
    return _size;
}

uint32_t __os_iso_get_pt_addr(uint8_t drive){
    uint16_t _r_buffer[1024];
    ide_atapi_read(drive, 0x10, 1, 0, _r_buffer);
    char* _ptbuf = (char*)_r_buffer + 140;
    uint32_t ret = *((uint32_t*) _ptbuf);
    return ret;
}
uint32_t __os_iso_get_root_addr(uint8_t drive){
    return 156;
}

void __os_iso_path_table(uint8_t drive, uint32_t addr, uint16_t offset){
    uint16_t _r_buffer[1024];
    ide_atapi_read(drive, addr, 1, 0, _r_buffer);
    ISO_PATH_TABLE* pt = (ISO_PATH_TABLE*)((char*)_r_buffer + offset);

    uint8_t dir_len = pt->name_len;
    printf("DIR EN NAME LEN: %u\n", dir_len);

    char* dir_name = pt->name;
    printf("DIR EN NAME: ");
    write(dir_name, dir_len);
    printf("\n");

    uint32_t lba = pt->dir_lba;
    printf("DIR LBA: 0x%x (%u)",lba, lba);

    uint32_t next_entry = 8 + offset + dir_len;
    next_entry += (next_entry & 0x0000001);
    printf("\nnext entry offset: 0x%x (%u)\n", next_entry, next_entry);
    
}
/*
void __os_iso_dir_table(uint8_t drive, uint32_t addr, uint16_t offset){
    uint16_t _rbuf[1024];
    ide_atapi_read(drive, addr, 1, 0, _rbuf);
    char* _buf = (char*)_rbuf + offset;

    uint8_t len = _buf[32];
    printf("isoDIR NLEN: %u %u\n", len, _buf[33]);
    printf("isoDIR NAME: ");
    write(_buf, 34);
    printf("\n");
}
*/

uint8_t __os_iso_dir_name(uint8_t drive, uint32_t lba, uint16_t offset){
    uint16_t _rbuf[1024];
    ide_atapi_read(drive, lba, 1, 0, _rbuf);
    char* _buf = (char*)_rbuf + offset;

    printf("DIR RECORD LEN: %u\nDIR NAME LEN: %u\nDIR NAME: ", _buf[0]&0xFF, _buf[32]&0xFF);
    uint8_t len = _buf[32];
    write(_buf+33, len);
    printf("\n");

    return len;
}

#include <vga.h>
void __os_iso_list_dir(uint8_t drive, uint32_t lba){
    uint16_t _rbuf[1024];
    ide_atapi_read(drive, lba, 1, 0, _rbuf);
    char* _buf = (char*)_rbuf;
    uint16_t offset=0;

    do {
        uint8_t len = _buf[32];
        char* name = _buf+33;
        if (!(*name)) printf(".");
        else if (*name == '\1' && len==1) printf("..");
        else write(name, len);

        uint32_t ext_lba = *((uint32_t*)(_buf+2));
        goto_column(50);
        printf("| lba: %u, offset: %u\n", ext_lba, offset);
        
        offset += (*_buf)&0xFF;
        _buf += (*_buf)&0xFF;
        if ((uint32_t)_buf - (uint32_t)(char*)_rbuf >= 2048) {
            printf("EOB\n");
            return;
        }
    } while (*_buf);
}


void __os_iso_dir_info(uint8_t drive, uint32_t lba, uint16_t offset){
    uint16_t _rbuf[1024];
    ide_atapi_read(drive, lba, 1, 0, _rbuf);
    ISO_DIRECTORY* dir = (ISO_DIRECTORY*)((char*)_rbuf + offset);

    printf("DIR RECORD LEN: %u\nDIR NAME LEN: %u\n", dir->size, dir->name_len);
    printf("Data size: %u\nData LBA: %u\n", dir->data_size, dir->data_lba);
    
    printf("DIR NAME: ");
    if (!dir->name[0] && dir->name_len==1) printf(".");
    else if (dir->name[0]==1 && dir->name_len==1) printf("..");
    else write(dir->name, dir->name_len);
    printf("\n");
}









void __os_install_to_disk(uint8_t dest_drive, uint8_t drive, uint16_t drv_type, uint32_t lba){
    struct ide_device* dst_dev = ide_get_device(dest_drive);
    struct ide_device* src_dev = ide_get_device(drive);
    if (!(dst_dev->reserved && src_dev->reserved)){
        printf("^4error:^_ one or more drives doesn't exist\n");
        printf("drv%u->resv: %u, drv%u->resv: %u\n", dest_drive, dst_dev->reserved, drive, src_dev->reserved);
        return;
    }
    char* _s_drv_type;
    uint32_t sectors;
    if (drv_type) {
        _s_drv_type = "ATAPI";
        sectors = __os_iso_get_vol_size(drive);
    } else _s_drv_type = "ATA";
    printf("Installing OS from drive %u (%s) to drive from drive %u\n", drive, _s_drv_type, dest_drive);
    // Installation
    printf("^fNOTE that currently [install] copies all from ISO file to disk^_\n");
    uint32_t bytes = _kernel_end-0x100000;
    printf("copying whole iso area... %x bytes\n", bytes);
    printf("are you sure you wanna continue installation [y/n]? ");
    char* ans = kmalloc(1100);
    read(ans);
    if (ans[0] != 'y') return;
    kfree(ans);
    uint32_t sect_size;
    if (!drv_type) {
        sectors = bytes/512;    // ATA sector
        sect_size = 512;
    } else {
        sect_size = 2048;
    }
    uint16_t _cbuf[1024];
    uint16_t* _buf = (uint16_t*) _cbuf;
    for (uint32_t sect=0; sect<sectors; sect++){
        printu(sect);
        print("copying sector\n");
        uint8_t werr=0;
        
        if (ide_atapi_read(drive, sect, 1, 0, _buf)) {
            printf("ide read error\n");
            return;
        }
        disk_write(dest_drive, lba + 4*sect, 1, _buf);
        disk_write(dest_drive, lba + 4*sect+1, 1, _buf + (256));
        disk_write(dest_drive, lba + 4*sect+2, 1, _buf + (512));
        disk_write(dest_drive, lba + 4*sect+3, 1, _buf + (768));
        
        printf("done loop\n");
    }
    printf("INSTALLATION COMPLETE!\n");
    
    return;
}



