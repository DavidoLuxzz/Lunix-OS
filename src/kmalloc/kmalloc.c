#include <sys/kmalloc.h>
// #include <string.h> // izbrisi i koristi util.h
#include <util.h>
#include <stdio.h>

// extern uint32_t _kernel_end; // kernel end address will be used as start memory address
uint32_t _mem_start;

uint32_t _mem_blk_sizes[MEM_MAX_NUM_BLOCKS];
uint32_t _mem_bitmap[MEM_BITMAP_UINT32_COUNT];

int init_kmalloc(uint32_t _start){
    memset((uint8_t*)_mem_bitmap, 0x00, MEM_BITMAP_BYTE_COUNT);
    memset((uint8_t*)_mem_blk_sizes, 0x00, MEM_MAX_NUM_BLOCKS*4);

    _mem_start = _start;

    return 0;
}
/*
void kdump_arrays(int blocks){
    for (int i=0; i<blocks; i++){
        printf("%u,", _mem_blk_sizes[i]);
    }
    putc("\n");
    int _bi = blocks/32 + 1;
    for (int i=0; i<_bi; i++){
        uint32_t m = 0x00000001;
        for (int j=0; j<32; j++){
            printf("%u", (_mem_bitmap[i]&m)>0);

            m = m<<1;
        }
    }
    putc("\n");
}
*/
void *kmalloc(uint32_t size){ // size in bytes ofc
    uint32_t index=0;
    void* ptr=(void*)_mem_start;

    for (; index<MEM_MAX_NUM_BLOCKS; index++){
        // looks for block with enough space
        // block with memory space size 0 means its not yet allocated, thus needs to be
        if (_mem_blk_sizes[index]==0 || _mem_blk_sizes[index]>=size){ // has enough space
            // checks if the coresponding bit is 0 (free)
            uint32_t u32id = index/32;
            uint32_t bit = index%32;
            if (!getbit(_mem_bitmap[u32id], bit)){ // bit is 0 (free)
                // mark space as used
                _mem_bitmap[u32id] |= (0x00000001<<bit); // set bit to 1 (used)
                // if block size was 0 set it to <size>
                if (_mem_blk_sizes[index]==0){
                    _mem_blk_sizes[index] = size;
                }

                return ptr;
            }
        }
        ptr += _mem_blk_sizes[index];
    }
    print("kmalloc: error: cannot find enough space for allocation\n");
    return NULL;
}

int kfree(void* _ptr){
    // find which index it is
    uint32_t index=0;
    void* iptr=(void*)_mem_start;

    for (; index<MEM_MAX_NUM_BLOCKS; index++){
        if (iptr == _ptr){
            uint32_t u32id = index/32;
            uint32_t bit = index%32;
            uint32_t bitval = getbit(_mem_bitmap[u32id], bit);
            if (bitval){
                _mem_bitmap[u32id] ^= bitval;  // mark space as free
            } else {
                print("kfree: warning: given pointer was already free\n");
            }
            return 0;
        }

        iptr += _mem_blk_sizes[index];
    }
    print("kfree: error: cannot find block index of given pointer \n");
    return -1;
}






