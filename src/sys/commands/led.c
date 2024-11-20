#include <sys/commands.h>
#include <stdio.h>
#include <sys/libkern.h>
#include <fs/lxfs.h>
#include <util.h>
#include <vga.h>
#include <string.h>
#include <sys/kmalloc.h>
#include <terminal.h>

__CMD_FUNC(cmd_led){
    // lunix editor
    if (argc < 1){
        printf("led: no arguments\n"); return 0;
    }
    LXFS_ENTRY* e = lxfs_open_file(&currentDir, command+tokens[0]);
    if (!e) {
        printf("Failed to open file: %s\n", command+tokens[0]); return 0;
    }
    uint32_t alba = lxfs_get_data_lba(e->lba);
    printf("Opening file at absolute LBA=%u\n",alba);
    // load file to memory
    char bytes[e->size*512];
    disk_read(0, alba, e->size, bytes);
    // start editor
    printf("Lunix editor (v0.0.1)\n");
    uint32_t view_range, current_byte=0;
    bool running=true;
    printf("Enter viewing range: ");
    char tmp[TERMINAL_MAX_BUFFER_LENGTH+1]; // +1 za svaki slucaj
    tmp[TERMINAL_MAX_BUFFER_LENGTH]=0;
    nread(tmp, 10);
    view_range = stoi(tmp);
    // save current VGA screen to buffer
    uint16_t* __tmp_screen = kmalloc(width*height*2);
    memcpy(__tmp_screen, (void*)0xb8000, width*height*2);
    uint32_t __tmp_vga_pos = get_vga_position();
    // start editing process
    clear_screen();
    while (running){
        move_cursor(0,height-3);
        printf("loc: %u\n$ ", current_byte);
        read(tmp);
        clear_screen();
        if (strcmd(tmp, "q")) running = false;
        else if (strcmd(tmp, "r")){
            // preview file from current_byte to +view_range
            for (uint32_t i=0; i<view_range; i++){
                printf("%x ", bytes[i+current_byte]&0xFF);
            }
            newLine();
        } else if (strcmd(tmp, "rs")){ 
            for (uint32_t i=0; i<view_range; i++){
                char c = bytes[i];
                if (c>0x1F && c<0x7F) putc(c);
                else putc('.');
            }
        } else if (strcmd(tmp, "e")){
            printf("Edit this byte section (use space as delimiter): ");
            read(tmp);
            // tokenize input ' '
            uint16_t tokens[strlen(tmp)/2];
            tokens[0]=0; // da bi kreno od pocetka
            size_t argc = strtok(tmp, ' ', (uint16_t*)tokens+1);
            for (uint32_t i=0; i<argc+1; i++){
                uint8_t x = stox(tmp+tokens[i])&0xFF;
                printf("writing %x to %u\n", x, current_byte+i);
                bytes[current_byte+i] = x;
            }
        } else if (strcmd(tmp, "mv")){
            printf("Move to (byte): ");
            read(tmp);
            current_byte = stoi(tmp);
        } else if (strcmd(tmp, "w")){
            disk_write(0, alba, e->size, bytes);
            printf("Saved file at LBA=%u (\"%s\")\n",alba,command+tokens[0]);
        } else if (strcmd(tmp, "help")){
            printf("Commands:\n  r: preview file from current byte\n  q: quit editor without saving\n  w: save changes\n  e: edit selected section\n  mv: move to byte\n  rs: text preview from current byte\n");
        } 
    }
    // load back saved screen
    memcpy((void*)0xb8000,__tmp_screen,width*height*2);
    kfree(__tmp_screen);
    set_vga_position(__tmp_vga_pos);
    putchar(0);
    // finally, close the file ...
    lxfs_close_file(e);
    return 0;
}


