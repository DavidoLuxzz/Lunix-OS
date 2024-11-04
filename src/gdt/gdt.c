#include <gdt/gdt.h>
#include <util.h>
#include <stdio.h>

extern void gdt_flush(uint32_t);
extern void tss_flush();

struct gdt_entry_struct gdt_entries[6];
struct gdt_ptr_struct gdt_ptr;
struct tss_entry_struct tss_entry;

void initGdt(uint32_t kernel_end){
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    setGdtGate(0,0,0,0,0); //Null segment
    /* Lukin GDT code */
    /* Kernel Code segment:
     *  base ili pocetak je u 0x0
     *  a limit je postavljen u 0x2FF(FFF)
     *  ne broji se limit u bajtovima vec u 1kb "stranicama",
     *  tako da 0x2FF postaje 0x2FFFFF (dodaje se tri hex cifre)
     */
    uint32_t pages = (kernel_end>>12) & 0x000FFFFF;
    printf("GDT: setting kernel code segment limit to %p (%p pages)\n", kernel_end, pages);
    setGdtGate(1,0,pages, 0x9A, 0xCF); //Kernel code segment
    setGdtGate(2,0,0xFFFFFFFF, 0x93, 0xCF); //Kernel data segment
    setGdtGate(3,kernel_end,0xFFFFFFFF, 0x9B, 0xCF); //User code segment
    setGdtGate(4,kernel_end,0xFFFFFFFF, 0x93, 0xCF); //User data segment
    
    /* Old GDT code (made by OliveStem thus can be broken)
    setGdtGate(1,0,0xFFFFFFFF, 0x9A, 0xCF); //Kernel code segment
    setGdtGate(2,0,0xFFFFFFFF, 0x93, 0xCF); //Kernel data segment
    setGdtGate(3,0,0xFFFFFFFF, 0xFB, 0xCF); //User code segment
    setGdtGate(4,0,0xFFFFFFFF, 0xF3, 0xCF); //User data segment
    */
    writeTSS(5,0x10, 0x0);

    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
    
}

void writeTSS(uint32_t num, uint16_t ss0, uint32_t esp0){
    uint32_t base = (uint32_t) &tss_entry;
    uint32_t limit = base + sizeof(tss_entry);

    setGdtGate(num, base, limit, 0xE9, 0x00);
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;

    tss_entry.cs = 0x08 | 0x3;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x3;
}

void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){

    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit = (limit & 0xFFFF);
    gdt_entries[num].flags = (limit >> 16) & 0x0F;
    gdt_entries[num].flags |= (gran & 0xF0);

    gdt_entries[num].access = access;

}
