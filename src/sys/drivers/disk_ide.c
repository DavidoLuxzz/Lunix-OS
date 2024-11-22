#include <sys/drivers/disk_h.h>
#include <stdio.h>
#include <util.h>
#include <interrupts/idt.h>

struct IDEChannelRegisters channels[2];
struct ide_device ide_devices[4];
unsigned char ide_buf[2048] = {0};
volatile unsigned static char ide_irq_invoked = 0;
unsigned static char atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned char ide_read(unsigned char channel, unsigned char reg) {
   unsigned char result;
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   if (reg < 0x08)
      result = inb(channels[channel].base + reg - 0x00);
   else if (reg < 0x0C)
      result = inb(channels[channel].base  + reg - 0x06);
   else if (reg < 0x0E)
      result = inb(channels[channel].ctrl  + reg - 0x0A);
   else if (reg < 0x16)
      result = inb(channels[channel].bmide + reg - 0x0E);
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
   return result;
}

void ide_write(unsigned char channel, unsigned char reg, unsigned char data) {
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   if (reg < 0x08)
      outb(channels[channel].base  + reg - 0x00, data);
   else if (reg < 0x0C)
      outb(channels[channel].base  + reg - 0x06, data);
   else if (reg < 0x0E)
      outb(channels[channel].ctrl  + reg - 0x0A, data);
   else if (reg < 0x16)
      outb(channels[channel].bmide + reg - 0x0E, data);
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

unsigned char ide_polling(unsigned char channel, unsigned int advanced_check) {

   // (I) Delay 400 nanosecond for BSY to be set:
   // -------------------------------------------------
   for(int i = 0; i < 4; i++)
      ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns; loop four times.

   // (II) Wait for BSY to be cleared:
   // -------------------------------------------------
   while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
      ; // Wait for BSY to be zero.

   if (advanced_check) {
      unsigned char state = ide_read(channel, ATA_REG_STATUS); // Read Status Register.

      // (III) Check For Errors:
      // -------------------------------------------------
      if (state & ATA_SR_ERR)
         return 2; // Error.

      // (IV) Check If Device fault:
      // -------------------------------------------------
      if (state & ATA_SR_DF)
         return 1; // Device Fault.

      // (V) Check DRQ:
      // -------------------------------------------------
      // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
      if ((state & ATA_SR_DRQ) == 0)
         return 3; // DRQ should be set

   }

   return 0; // No Error.

}

void ide_read_buffer(unsigned char channel, unsigned char reg, void* buffer,
                     unsigned int quads) {
   /* WARNING: This code contains a serious bug. The inline assembly trashes ES and
    *           ESP for all of the code the compiler generates between the inline
    *           assembly blocks.
    */
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   asm("pushw %es; movw %ds, %ax; movw %ax, %es");
   if (reg < 0x08)
      insl(channels[channel].base  + reg - 0x00, buffer, quads);
   else if (reg < 0x0C)
      insl(channels[channel].base  + reg - 0x06, buffer, quads);
   else if (reg < 0x0E)
      insl(channels[channel].ctrl  + reg - 0x0A, buffer, quads);
   else if (reg < 0x16)
      insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
   asm("popw %es;");
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}


unsigned char ide_print_error(unsigned int drive, unsigned char err) {
    print("IDE: ");
    if (err == 0){
        print("- No Error\n");
        return err;
    }

    if (err == 1) {print("- Device Fault\n     "); err = 19;}
    else if (err == 2) {
        unsigned char st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
        if (st & ATA_ER_AMNF)   {print("- No Address Mark Found\n     ");   err = 7;}
        if (st & ATA_ER_TK0NF)   {print("- No Media or Media Error\n     ");   err = 3;}
        if (st & ATA_ER_ABRT)   {print("- Command Aborted\n     ");      err = 20;}
        if (st & ATA_ER_MCR)   {print("- No Media or Media Error\n     ");   err = 3;}
        if (st & ATA_ER_IDNF)   {print("- ID mark not Found\n     ");      err = 21;}
        if (st & ATA_ER_MC)   {print("- No Media or Media Error\n     ");   err = 3;}
        if (st & ATA_ER_UNC)   {print("- Uncorrectable Data Error\n     ");   err = 22;}
        if (st & ATA_ER_BBK)   {print("- Bad Sectors\n     ");       err = 13;}
    } else  if (err == 3)           {print("- Reads Nothing\n     "); err = 23;}
    else  if (err == 4)  {print("- Write Protected\n     "); err = 8;}

    print("- [");
    unsigned char _ch = ide_devices[drive].channel;
    if (_ch) printf("%u: Secondary ", _ch);
    else printf("%u: Primary ", _ch);
    print("] ");
    print(ide_devices[drive].model);
    putc('\n');

    return err;
}

void ide_initialize(void) {
   uint8_t BAR0,BAR1,BAR2,BAR3,BAR4;
   BAR0=BAR1=BAR2=BAR3=BAR4 = 0;
   int j, k, count = 0;

   // 1- Detect I/O Ports which interface IDE Controller:
   channels[ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
   channels[ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
   channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
   channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
   channels[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
   channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

   // 2- Disable IRQs:
   ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
   ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

   // 3- Detect ATA-ATAPI Devices:
   for (unsigned short i = 0; i < 2; i++)
      for (unsigned short j = 0; j < 2; j++) {

         unsigned char err = 0, type = IDE_ATA, status;
         ide_devices[count].reserved = 0; // Assuming that no drive here.

         // (I) Select Drive:
         ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
         ide_pseudo_sleep(1); // Wait 1ms for drive select to work.

         // (II) Send ATA Identify Command:
         ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
         ide_pseudo_sleep(1); // This function should be implemented in your OS. which waits for 1 ms.
                   // it is based on System Timer Device Driver.

         // (III) Polling:
         if (ide_read(i, ATA_REG_STATUS) == 0) continue; // If Status = 0, No Device.

         while(1) {
            status = ide_read(i, ATA_REG_STATUS);
            if ((status & ATA_SR_ERR)) {err = 1; break;} // If Err, Device is not ATA.
            if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break; // Everything is right.
         }

         // (IV) Probe for ATAPI Devices:

         if (err != 0) {
            unsigned char cl = ide_read(i, ATA_REG_LBA1);
            unsigned char ch = ide_read(i, ATA_REG_LBA2);

            if (cl == 0x14 && ch == 0xEB)
               type = IDE_ATAPI;
            else if (cl == 0x69 && ch == 0x96)
               type = IDE_ATAPI;
            else
               continue; // Unknown Type (may not be a device).

            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
            ide_pseudo_sleep(1);
         }

         // (V) Read Identification Space of the Device:
         ide_read_buffer(i, ATA_REG_DATA, ide_buf, 128);

         // (VI) Read Device Parameters:
         ide_devices[count].reserved     = 1;
         ide_devices[count].type         = type;
         ide_devices[count].channel      = i;
         ide_devices[count].drive        = j;
         ide_devices[count].signature    = *((unsigned short *)(ide_buf + ATA_IDENT_DEVICETYPE));
         ide_devices[count].capabilities = *((unsigned short *)(ide_buf + ATA_IDENT_CAPABILITIES));
         ide_devices[count].commandSets  = *((unsigned int *)(ide_buf + ATA_IDENT_COMMANDSETS));

         // (VII) Get Size:
         if (ide_devices[count].commandSets & (1 << 26))
            // Device uses 48-Bit Addressing:
            ide_devices[count].size   = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
         else
            // Device uses CHS or 28-bit Addressing:
            ide_devices[count].size   = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA));

         // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
         for(k = 0; k < 40; k += 2) {
            ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
            ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];}
         ide_devices[count].model[40] = 0; // Terminate String.

         count++;
      }
    print("Installing IDE driver interrupt handler (IRQ 14, 15)\n");
    ide_install_handler();
}

void ide_dump_drives(){
    for (int i = 0; i < 4; i++) {
        if (ide_devices[i].reserved == 1) {
            // $n: $MODEL ($TYPE) size: $SIZE
            
            unsigned short _t = ide_devices[i].type;
            char* _type;
            /* Drive types:
             *  0: ATA
             *  1: ATAPI
             */
            if (_t) _type = "ATAPI";
            else _type = "ATA";
            unsigned int _s = ide_devices[i].size * 512;
            unsigned int _dsz, _ti;
            const char* _szt[4] = {"B","kB","MB","GB"};
            if (_s < 1024) {
                _dsz = _s;
                _ti = 0;
            } else if (_s < 1024*1024) {
                _dsz = _s / 1024;
                _ti = 1;
            } else {
                _dsz = _s / 1024 / 1024;
                _ti = 2;
            }
            //printf(") [%u", _dsz);
            //print(_szt[_ti]);
            //print("]\n");
            printf("%u: %s (%s) [%u%s]\n", i, ide_devices[i].model, _type, _dsz, _szt[_ti]);
        }
    }
}

// FIX ME
void ide_pseudo_sleep(unsigned long int millis){
	for (int i = 0; i < CEIL_DIV(millis, 1000); ++i){
		for (int j = 0; j < 10000; ++j) 1234/4321;
	}
}

struct ide_device* ide_get_device(int i){
    return &ide_devices[i];
}






void ide_wait_irq() {
    while (!ide_irq_invoked)
        ;
    ide_irq_invoked = 0;
}

void ide_irq(struct InterruptRegisters *regs) {
    ide_irq_invoked = 1;
}

uint8_t __ide_drive = 0;

void ide_change_drive(unsigned char drive){
    __ide_drive = drive;
}

unsigned char _ide_atapi_read(uint32_t lba, uint8_t sects, void* buf){
    return ide_atapi_read(__ide_drive, lba, sects, 0, buf);
}

unsigned char ide_atapi_read(unsigned char drive, unsigned int lba, unsigned char numsects,
          unsigned short selector, void* _buf) {
    unsigned int   channel  = ide_devices[drive].channel;
    unsigned int   slavebit = ide_devices[drive].drive;
    unsigned int   bus      = channels[channel].base;
    unsigned int   words    = 1024; // Sector Size. ATAPI drives have a sector size of 2048 bytes.
    unsigned int   buf = (uint32_t)_buf;
    unsigned char  err;
    int i;

    // Enable IRQs:
    ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x0);

    // (I): Setup SCSI Packet:
    // ------------------------------------------------------------------
    atapi_packet[ 0] = ATAPI_CMD_READ;
    atapi_packet[ 1] = 0x0;
    atapi_packet[ 2] = (lba >> 24) & 0xFF;
    atapi_packet[ 3] = (lba >> 16) & 0xFF;
    atapi_packet[ 4] = (lba >> 8) & 0xFF;
    atapi_packet[ 5] = (lba >> 0) & 0xFF;
    atapi_packet[ 6] = 0x0;
    atapi_packet[ 7] = 0x0;
    atapi_packet[ 8] = 0x0;
    atapi_packet[ 9] = numsects;
    atapi_packet[10] = 0x0;
    atapi_packet[11] = 0x0;

    // (II): Select the drive:
    // ------------------------------------------------------------------
    ide_write(channel, ATA_REG_HDDEVSEL, slavebit << 4);

    // (III): Delay 400 nanoseconds for select to complete:
    // ------------------------------------------------------------------
    for(int i = 0; i < 4; i++)
        ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns.

    // (IV): Inform the Controller that we use PIO mode:
    // ------------------------------------------------------------------
    ide_write(channel, ATA_REG_FEATURES, 0);         // PIO mode.

    // (V): Tell the Controller the size of buffer:
    // ------------------------------------------------------------------
    ide_write(channel, ATA_REG_LBA1, (words * 2) & 0xFF);   // Lower Byte of Sector Size.
    ide_write(channel, ATA_REG_LBA2, (words * 2) >> 8);   // Upper Byte of Sector 

    // (VI): Send the Packet Command:
    // ------------------------------------------------------------------
    ide_write(channel, ATA_REG_COMMAND, ATA_CMD_PACKET);      // Send the Command.

    // (VII): Waiting for the driver to finish or return an error code:
    // ------------------------------------------------------------------
    if (err = ide_polling(channel, 1)) {
        ide_print_error(drive, err);
        return err;         // Polling and return if error.
    }

    // (VIII): Sending the packet data:
    // ------------------------------------------------------------------
    asm("rep   outsw" : : "c"(6), "d"(bus), "S"(atapi_packet));   // Send Packet Data

    // (IX): Receiving Data:
    // ------------------------------------------------------------------
    for (i = 0; i < numsects; i++) {
        ide_wait_irq();                  // Wait for an IRQ.
        if (err = ide_polling(channel, 1))
            return err;      // Polling and return if error.
        asm("pushw %es");
        asm("mov %%ax, %%es"::"a"(selector));
        asm("rep insw"::"c"(words), "d"(bus), "D"(buf));// Receive Data.
        asm("popw %es");
        buf += (words * 2);
    }

    // (X): Waiting for an IRQ:
    // ------------------------------------------------------------------
    ide_wait_irq();

    // (XI): Waiting for BSY & DRQ to clear:
    // ------------------------------------------------------------------
    while (ide_read(channel, ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ))
        ;

    return 0; // Easy, ... Isn't it?
}

void ide_install_handler(){
    irq_install_handler (14, ide_irq); // Primary ATA Hard Disk
    irq_install_handler (15, ide_irq); // Secondary ATA Hard Disk
}



// ### DRIVER DEFINITION ###
void __ide_nodef(void){};

const driver_t __OSDEV_IDE_DRIVER_ide = {
    "Default IDE driver 1/2",
    DT_ATA_IDE,
    &ide_initialize,
    &ide_change_drive,
    NULL
};


const driver_t __OSDEV_IDE_DRIVER_ide_atapi = {
    "Default IDE driver 2/2",
    DT_CDDVD,
    &__ide_nodef,
    &ide_change_drive,
    &_ide_atapi_read
};

const driver_t *ide_get_driver0(void){
    return &__OSDEV_IDE_DRIVER_ide;
}
const driver_t *ide_get_driver1(void){
    return &__OSDEV_IDE_DRIVER_ide_atapi;
}

