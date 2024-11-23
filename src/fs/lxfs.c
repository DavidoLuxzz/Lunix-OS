#include <fs/lxfs.h>
#include <util.h>
#include <sys/drivers/disk_ata.h>
#include <math.h>
#include <sys/kmalloc.h>
#include <stdio.h>
#include <string.h>
#include <cpu/cpu.h>
#include <unistd.h>

DISK_PARTITION _dskpart;

uint8_t* __lxfs_fs_bitmap; // 1 bit = 4 sectora
uint32_t __lxfs_fs_4s;

uint8_t* __lxfs_data_bitmap; // 1 bit = 1 block (2048 sectora)
uint32_t __lxfs_data_blocks;
uint32_t __lxfs_data_start_lba;

bool __lxfs_fs_log = false;

void _lxfs_scan_dir(uint32_t lba){
    LXFS_ENTRY entries[LXFS_MAX_NODES+2]; // +2 for the dir
    if (__lxfs_fs_log) printf("LBA: %u\n", lba);
    lxfs_get_all_entries(lba, entries); // zeza
    uint32_t dsize=entries[0].size; // '.'(this).size
    uint32_t found=0;
    if (dsize) {
        for (int i=2; i<LXFS_MAX_NODES; i++){
            LXFS_ENTRY e = entries[i];
            if (e.flags & LXFS_VALID){ // valid dir
                uint8_t type = (e.type_and_perms>>12) & 0xFF; 
                if (__lxfs_fs_log) printf("LXFS: found node '%s' (%s)\n", e.name, _lxfs_get_type_name(type));
                if (type == LXFS_FOLDER){
                    if (e.lba<4) continue;
                    uint32_t bitmap_lba = e.lba/4;
                    uint32_t byte_off = bitmap_lba/8;
                    uint8_t inbyte_off = bitmap_lba - byte_off;
                    *(__lxfs_fs_bitmap+byte_off) |= (0x80>>inbyte_off); // set bit to 1
                    // recurse next dir
                    _lxfs_scan_dir(e.lba);
                    if (++found >= dsize) return;
                } else if (type == LXFS_FILE){
                    if (e.lba<__lxfs_data_start_lba) continue;
                    uint32_t bitmap_lba = (e.lba-__lxfs_data_start_lba)/LXFS_BLOCK_SIZE;
                    uint32_t byte_off = bitmap_lba/8;
                    uint8_t inbyte_off = bitmap_lba - byte_off;
                    *(__lxfs_fs_bitmap+byte_off) |= (0x80>>inbyte_off);
                    if (++found >= dsize) return;
                }
            }
        }
    }
}

void lxfs_load_filesystem(){
    uint32_t free_space = _dskpart.part.space;
    __lxfs_fs_4s = free_space/4 / 512; // 512ti deo od cele particije ide za foldere
    __lxfs_data_start_lba = __lxfs_fs_4s*4; // * 4 sectors per table
    __lxfs_fs_bitmap = (uint8_t*) kmalloc(__lxfs_fs_4s/8); // 32 kB
    memset(__lxfs_fs_bitmap, 0, __lxfs_fs_4s);
    *(__lxfs_fs_bitmap) |= 0x80; // root
    _lxfs_scan_dir(0); // scan root dir
    free_space -= (__lxfs_fs_4s*4);
    __lxfs_data_blocks = free_space/LXFS_BLOCK_SIZE; // blocks
    __lxfs_data_bitmap = (uint8_t*) kmalloc(__lxfs_data_blocks/8);
}

void lxfs_list_bitmap(){
    for (int i=0; i<4; i++)
        printf("%p\n", *(__lxfs_fs_bitmap+i)&0xFF);
}

void lxfs_use_partition(DISK_PARTITION *dpart){
    memcpy(&_dskpart, dpart, sizeof(DISK_PARTITION));
}

void _lxfs_get_entry(DISK_PARTITION *dpart, uint32_t parent_lba, uint8_t id, LXFS_ENTRY *e){
    uint32_t sector = dpart->part.lba + parent_lba + (id/8);
    LXFS_ENTRY entries[8]; // 8 entries in 512 byte sector
    _ata_disk_read(dpart->drive, sector, 1, (uint16_t*)entries);
    uint8_t id_in_sect = (id%8) * sizeof(LXFS_ENTRY);
    char* bytes = (char*)entries + id_in_sect;
    memcpy(e, bytes, sizeof(LXFS_ENTRY));
}

void lxfs_get_entry(uint32_t parent_lba, uint8_t id, LXFS_ENTRY *e){  
    uint32_t sector = _dskpart.part.lba + parent_lba + (id/8);
    LXFS_ENTRY entries[8]; // 8 entries in 512 byte sector
    _ata_disk_read(_dskpart.drive, sector, 1, (uint16_t*)entries);
    uint8_t id_in_sect = (id%8) * sizeof(LXFS_ENTRY);
    char* bytes = (char*)entries + id_in_sect;
    memcpy(e, bytes, sizeof(LXFS_ENTRY));
}

void lxfs_set_entry(uint32_t parent_lba, uint8_t id, LXFS_ENTRY *e){
    uint32_t sector = _dskpart.part.lba + parent_lba + (id/8);

    LXFS_ENTRY entries[8];
    _ata_disk_read(_dskpart.drive, sector, 1, (uint16_t*)entries);

    uint8_t id_in_sect = (id%8);
    memcpy(&entries[id_in_sect], e, sizeof(LXFS_ENTRY));

    _ata_disk_write(_dskpart.drive, sector, 1, (uint16_t*)entries);
}

void lxfs_get_all_entries(uint32_t parent_lba, LXFS_ENTRY *entries){
    uint32_t sector = _dskpart.part.lba + parent_lba;
    _ata_disk_read(_dskpart.drive, sector, 4, (uint16_t*)entries);
}

uint32_t _lxfs_find_free_space(){
    for (uint32_t i=0; i<__lxfs_fs_4s; i++){
        uint8_t c = *(__lxfs_fs_bitmap+i);
        for (uint32_t j=0; j<8; j++){
            uint32_t bit = getbit(c,7-j);
            if (!bit){
                return 8*i + j;
            }
        }
    }
    return -1;
}

bool __lxfs_data_block_free(uint8_t* c, uint8_t j){
    return getbit(*c, 7-j);
}
// return block address
uint32_t _lxfs_find_free_data_space(uint32_t count){
    if (count>8) return -1;
    for (uint32_t i=0; i<__lxfs_data_blocks; i++){
        uint8_t c = *(__lxfs_data_bitmap+i);
        bool found=0;
        uint32_t x=0;
        for (uint32_t j=0; j<8; j++){
            uint32_t bit=0;
            for (uint32_t d=0; d<count; d++) bit |= getbit(c,7-(j+d));
            if (!bit){
                return 8*i + x;
            } else x=j+1;
        }
    }
    return -1;
}

void _lxfs_occupy_data_space(uint32_t block){
    uint32_t byte_off = block / 8;
    uint32_t inbyte_off = block - byte_off;
    *(__lxfs_data_bitmap+byte_off) |= (0x80>>inbyte_off);
}
void _lxfs_free_data_space(uint32_t block){
    uint32_t byte_off = block / 8;
    uint32_t inbyte_off = block - byte_off;
    uint8_t m = 0xFF;
    m ^= (0x80>>inbyte_off);
    *(__lxfs_data_bitmap+byte_off) &= m;
}

void _lxfs_occupy_space(uint32_t _4sector){
    uint32_t byte_off = _4sector / 8;
    uint32_t inbyte_off = _4sector - byte_off;
    *(__lxfs_fs_bitmap+byte_off) |= (0x80>>inbyte_off);
}
void _lxfs_free_space(uint32_t _4sector){
    uint32_t byte_off = _4sector / 8;
    uint32_t inbyte_off = _4sector - byte_off;
    uint8_t m = 0xFF;
    m ^= (0x80>>inbyte_off);
    *(__lxfs_fs_bitmap+byte_off) &= m;
}
int lxfs_create_new_dir(LXFS_DIR *currentDir,const char* name,uint16_t perms,uint8_t flags){
    uint32_t parent_lba = currentDir->entry.lba;
    // is there room for new entry
    LXFS_ENTRY all[LXFS_MAX_NODES+2];
    lxfs_get_all_entries(parent_lba,all);
    if (all[0].size >= LXFS_MAX_NODES) return LXFS_ERR_FULL;
    uint32_t index,found=0;
    bool _fnd=false;
    for (index=2; index < LXFS_MAX_NODES; index++){
        LXFS_ENTRY e = all[index];
        if (e.flags & LXFS_VALID) {
            if (++found > all[0].size) break;
            continue;
        }
        break;
    }
    printf("mkdir: at index: %u\n", index);
    // create entry for dir
    LXFS_ENTRY de = {
        "name will be copied here", // name
        LXFS_VALID | flags, // flags
        (LXFS_FOLDER<<12) | perms, // type and permissions
        0, // size
        0, // will be calculated (LBA)
    };
    char dname[53];
    memset(dname, 0, 53);
    strncpy(dname,name,min(strlen(name),52));
    memcpy(de.name, dname, 53); // apply dir name
    // find lba for new dir
    uint32_t lba = _lxfs_find_free_space();
    // occupy that lba
    _lxfs_occupy_space(lba);
    de.lba = lba*4; // 1 folder contains 4 sectors of entries
    printf("mkdir: at lba: %u\n",de.lba);
    // write entry to disk
    lxfs_set_entry(parent_lba, index, &de);
    // create and write LXFS_DIR
    LXFS_DIR dir;
    memcpy(&dir.entry, &de, 64);
    dir.parent_lba = parent_lba;
    lxfs_set_dir(lba*4, &dir);
    // update directory size
    currentDir->entry.size++;
    lxfs_set_dir(parent_lba, currentDir);
    return LXFS_SUCCESS;
}


int _lxfs_invl_entry(LXFS_DIR *currentDir, int index){
    // if the given index is invalid
    if (index<2 || index>=LXFS_MAX_NODES) return LXFS_ERR_INVL;
    LXFS_ENTRY entry;
    lxfs_get_entry(currentDir->entry.lba, index, &entry);
    entry.flags = 0; // invalidate entry
    memset(entry.name, 0, 2);
    lxfs_set_entry(currentDir->entry.lba, index, &entry);
    return LXFS_SUCCESS;
}

int _lxfs_remove_dir(LXFS_DIR *cd, int index){
    LXFS_ENTRY e;
    lxfs_get_entry(cd->entry.lba, index, &e);
    if ((e.type_and_perms>>12 & 0x0F)!=LXFS_FOLDER) return LXFS_ERR_TYPE;
    _lxfs_free_space(e.lba/4);
    _lxfs_invl_entry(cd, index);
    cd->entry.size--;
    lxfs_set_dir(cd->entry.lba, cd);
    return LXFS_SUCCESS;
}

int lxfs_remove_dir(LXFS_DIR *currentDir, const char* dirname){
    uint32_t valids=0;
    LXFS_ENTRY es[LXFS_MAX_NODES+2];
    // first two entries are skipped (LXFS_DIR)
    lxfs_get_all_entries(currentDir->entry.lba, es);
    uint32_t dirsize = es[0].size; // + 2 for . and ..
    if (!dirsize) return LXFS_ERR_NOT_FOUND;
    for (uint32_t i=2; i<LXFS_MAX_NODES; i++){
        LXFS_ENTRY e = es[i];
        if (!(e.flags & LXFS_VALID)) continue;
       
        if (strcmd(e.name, dirname)){
            return _lxfs_remove_dir(currentDir, i);
        }

        if (++valids >= dirsize) break;
    }
    return LXFS_ERR_NOT_FOUND;
}

int _lxfs_remove_file(LXFS_DIR *cd, int index){
    LXFS_ENTRY e;
    lxfs_get_entry(cd->entry.lba, index, &e);
    uint8_t type = (e.type_and_perms>>12) & 0x0F;
    if ((e.type_and_perms>>12 & 0x0F)!=LXFS_FILE) return LXFS_ERR_TYPE;
    _lxfs_free_data_space(e.lba/LXFS_BLOCK_SIZE);
    _lxfs_invl_entry(cd, index);
    cd->entry.size--;
    lxfs_set_dir(cd->entry.lba, cd);
    return LXFS_SUCCESS;
}

int lxfs_remove_file(LXFS_DIR *cd, const char* filename){
    uint32_t valids=0;
    LXFS_ENTRY es[LXFS_MAX_NODES+2];
    lxfs_get_all_entries(cd->entry.lba, es);
    uint32_t dirsize = es[0].size;
    if (!(dirsize | cd->entry.size)) return LXFS_ERR_NOT_FOUND;
    for (uint32_t i=2; i<LXFS_MAX_NODES; i++){
        LXFS_ENTRY e = es[i];
        if (!(e.flags & LXFS_VALID)) continue;

        if (strcmd(e.name, filename)){
            return _lxfs_remove_file(cd, i);
        }
        if (++valids >= dirsize) break;
    }
    return LXFS_ERR_NOT_FOUND;
}

uint32_t lxfs_find_entry_id_with_lba(uint32_t parent_lba, uint32_t req_lba){
    uint32_t valids=0;
    LXFS_ENTRY es[LXFS_MAX_NODES+2];
    lxfs_get_all_entries(parent_lba, es);
    uint32_t dirsize = es[0].size + 2; // + 2 for . and ..
    for (uint32_t i=0; i<LXFS_MAX_NODES; i++){
        LXFS_ENTRY e = es[i];
        if (!(e.flags & LXFS_VALID)) continue;
        if (e.lba == req_lba) return i;
        if (++valids >= dirsize) break;
    }
    return -1;
}

void lxfs_get_dir(uint32_t parent_lba, LXFS_DIR *dir){  
    uint32_t sector = _dskpart.part.lba + parent_lba;
    LXFS_DIR dirs[4]; // 4 dir structs in 512 byte sector
    _ata_disk_read(_dskpart.drive, sector, 1, (uint16_t*)dirs);
    memcpy(dir, dirs, 128);
}

void lxfs_set_dir(uint32_t parent_lba, LXFS_DIR *dir){
    uint32_t sector = _dskpart.part.lba + parent_lba;

    LXFS_DIR dirs[4];
    _ata_disk_read(_dskpart.drive, sector, 1, (uint16_t*)dirs);

    memcpy(dirs, dir, 128);

    _ata_disk_write(_dskpart.drive, sector, 1, (uint16_t*)dirs);
}


// create new file
int lxfs_create_new_file(LXFS_DIR *currentDir, const char* name, uint16_t perms, uint8_t flags, uint32_t filesize){
    uint32_t parent_lba = currentDir->entry.lba;
    // is there room for new entry
    LXFS_ENTRY all[LXFS_MAX_NODES+2];
    lxfs_get_all_entries(parent_lba,all);
    if (all[0].size >= LXFS_MAX_NODES) return LXFS_ERR_FULL;
    uint32_t index,found=0;
    bool _fnd=false;
    for (index=2; index < LXFS_MAX_NODES; index++){
        LXFS_ENTRY e = all[index];
        if (e.flags & LXFS_VALID) {
            if (++found > all[0].size) break;
            continue;
        }
        break;
    }
    printf("new: at index: %u\n", index);
    // create entry for file
    LXFS_ENTRY fe = {
        "name will be copied here", // name
        LXFS_VALID | flags, // flags
        (LXFS_FILE<<12) | perms, // type and permissions
        0, // size
        0, // will be calculated (LBA)
    };
    char fname[53];
    memset(fname, 0, 53);
    strncpy(fname,name,min(strlen(name),52));
    memcpy(fe.name, fname, 53); // apply file name
    fe.size = filesize/512;
    if (filesize - (512*fe.size)) fe.size++;
    // find lba for new file
    uint32_t nblocks = fe.size/LXFS_BLOCK_SIZE;
    if (fe.size - (LXFS_BLOCK_SIZE*nblocks)) nblocks++;
    uint32_t lba = _lxfs_find_free_data_space(nblocks);
    // occupy that block
    _lxfs_occupy_data_space(lba);
    fe.lba = lba*LXFS_BLOCK_SIZE; // 1 folder contains 4 sectors of entries
    printf("new: at lba: %u\n",fe.lba);
    // make sure entry is valid
    fe.flags |= LXFS_VALID;
    // write entry to disk
    lxfs_set_entry(parent_lba, index, &fe);
    // update directory size
    currentDir->entry.size++;
    lxfs_set_dir(parent_lba, currentDir);
    
    return LXFS_SUCCESS;
}

uint32_t lxfs_get_data_lba(uint32_t rel_lba){
    return _dskpart.part.lba + __lxfs_data_start_lba + rel_lba;
}



LXFS_ENTRY* lxfs_open_file(LXFS_DIR* cd, const char* fname){
    LXFS_ENTRY* reten = kmalloc(sizeof(LXFS_ENTRY));
    uint32_t valids=0;
    LXFS_ENTRY es[LXFS_MAX_NODES+2];
    // first two entries are skipped (LXFS_DIR)
    lxfs_get_all_entries(cd->entry.lba, es);
    uint32_t dirsize = es[0].size; // + 2 for . and ..
    if (!dirsize) goto __lxfs_open_file___error_label;
    for (uint32_t i=2; i<LXFS_MAX_NODES; i++){
        LXFS_ENTRY e = es[i];
        if (!(e.flags & LXFS_VALID)) continue;
       
        if (strcmd(e.name, fname)){
            memcpy(reten, &e, sizeof(LXFS_ENTRY));
            return reten;
        }

        if (++valids >= dirsize) break;
    }
__lxfs_open_file___error_label:
    logerr("LXFS: open file error");
    kfree(reten);
    return NULL;
}

void lxfs_close_file(LXFS_ENTRY* allocated_file_entry){
    kfree(allocated_file_entry);
}







