#pragma once
#include <stdint.h>

#define MEM_MAX_NUM_BLOCKS 0x400000
#define MEM_BITMAP_UINT32_COUNT 131072 // MEM_MAX_NUM_BLOCKS/32
#define MEM_BITMAP_BYTE_COUNT 524288

int init_kmalloc(uint32_t);
void kdump_arrays(int);

void *kmalloc(uint32_t);
int kfree(void*);
