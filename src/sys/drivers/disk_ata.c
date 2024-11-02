#include <sys/drivers/disk_ata.h>
#include <stdio.h>
#include <util.h>

void ata_disk_read(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer){
    for (uint32_t i=0; i < num_sectors; i++) {
        outb(0x1F6, (0xE0 | (drv << 4) | ((sector >> 24) & 0x0F))); // drive | 24-31 LBA bits
        outb(0x1F2, 1); // sector count port - # of sectors to read/write
        outb(0x1F3, sector + i & 0xFF); // sector number port / LBA low bits 0-7
        outb(0x1F4, ((sector >> 8) & 0xFF)); // cylinder low port / LBA mid bits 8-15
        outb(0x1F5, ((sector >> 16) & 0xFF)); // cylinder high port / LBA high bits 16-23
        outb(0x1F7, 0x20); // command port - send read/write command
        
        // poll status port
        while (inb(0x1F7) & (1 << 7)) // wait until BSY bit is clear
            ;

        // read 256 words from data port into memory
        for (uint32_t j = 0; j < 256; j++){
            buffer[j + 256 * i] = inw(0x1F0);
            //printInt(inw(0x1F0));
            //printChar(' ');
        }

        //print("\n[INFO] Sector ");
        //printInt(i);
        //print(", error status ");
        //printInt(inb(0x1F1));

        // 400ns delay - Read alternate status register
        for (uint8_t k = 0; k < 4; k++)
            inb(0x3F6);
    }
}

uint8_t ata_disk_write(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer){
    uint8_t err;
    for (uint32_t i=0; i < num_sectors; i++) {
        outb(0x1F6, (0xE0 | (drv << 4) | ((sector >> 24) & 0x0F))); // head/drive # port - send head/drive #
        outb(0x1F2, 1); // sector count port - # of sectors to read/write
        outb(0x1F3, sector + i & 0xFF); // sector number port / LBA low bits 0-7
        outb(0x1F4, ((sector >> 8) & 0xFF)); // cylinder low port / LBA mid bits 8-15
        outb(0x1F5, ((sector >> 16) & 0xFF)); // cylinder high port / LBA high bits 16-23           
        outb(0x1F7, 0x30); // command port - send read/write command    

        // poll status port
        while (inb(0x1F7) & (1 << 7)) // wait until BSY bit is clear
            ;

        // write 256 words from memory to data port
        for (uint32_t j = 0; j < 256; j++)
            outw(0x1F0, buffer[j + 256 * i]);

        err = inb(0x1f1);
        if (err){
            break;
        }

        // 400ns delay - Read alternate status register
        for (uint8_t k = 0; k < 4; k++)
            inb(0x3F6);
   }
    // send cache flush command after write command is finished
    outb(0x1F7, 0xE7);

    // wait until BSY bit is clear after cache flush
    while (inb(0x1F7) & (1 << 7))
        ;
    return err;
}


// Secondary bus
void ata_disk_read2(uint8_t drv, uint32_t sector, uint32_t num_sectors, uint16_t *buffer){     
    // make sure buffer is clear                                                              
    for (uint32_t j = 0; j < 256 * num_sectors; j++){                                         
        buffer[j] = 0;                                                                        
    }                                                                                         
                                                                                              
    for (uint32_t i=0; i < num_sectors; i++) {                                                
        outb(0x176, (0xE0 | (drv << 4) | ((sector >> 24) & 0x0F))); // drive | 24-31 LBA bits 
        outb(0x172, 1); // sector count port - # of sectors to read/write                     
        outb(0x173, sector + i & 0xFF); // sector number port / LBA low bits 0-7              
        outb(0x174, ((sector >> 8) & 0xFF)); // cylinder low port / LBA mid bits 8-15         
        outb(0x175, ((sector >> 16) & 0xFF)); // cylinder high port / LBA high bits 16-23     
        outb(0x177, 0x20); // command port - send read/write command                          
                                                                                              
        // poll status port                                                                   
        while (inb(0x177) & (1 << 7)) // wait until BSY bit is clear                          
            ;                                                                                 
                                                                                              
        // read 256 words from data port into memory                                          
        for (uint32_t j = 0; j < 256; j++){                                                   
            buffer[j + 256 * i] = inw(0x1F0);                                                 
            //printInt(inw(0x1F0));                                                           
            //printChar(' ');                                                                 
        }                                                                                     
                                                                                              
        //print("\n[INFO] Sector ");                                                          
        //printInt(i);                                                                        
        //print(", error status ");                                                           
        //printInt(inb(0x1F1));                                                               
                                                                                              
        // 400ns delay - Read alternate status register                                       
        for (uint8_t k = 0; k < 4; k++)                                                       
            inb(0x376);                                                                       
    }                                                                                         
} 


