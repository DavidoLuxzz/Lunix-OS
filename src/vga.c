#include <vga.h>
#include <util.h>

uint16_t column = 0;
uint16_t line = 0;
uint16_t* const vga = (uint16_t* const) 0x000B8000;
const uint16_t __defaultColor = (COLOR_LIGHT_GREY << 8) | (COLOR_BLACK << 12);
uint16_t currentColor = __defaultColor;
bool __cursor_enabled = false;

void clear_screen(){
    line = 0;
    column = 0;
    currentColor = __defaultColor;

    for (uint16_t y = 0; y < height; y++){
        for (uint16_t x = 0; x < width; x++){
            vga[y * width + x] = ' ' | __defaultColor;
        }
    }
}

void clear_color(uint8_t color){
    line = 0;
    column = 0;
    for (uint16_t y=0; y<height; y++){
        for (uint16_t x=0; x<width; x++){
            vga[y*width + x] = color << 8;
        }
    }
}

void newLine(){
    if (line < height - 1){
        line++;
        column = 0;
    }else{
        scrollUp();
        column = 0;
    }
}

void scrollUp(){
    for (uint16_t y = 0; y < height; y++){
        for (uint16_t x = 0; x < width; x++){
            vga[(y-1) * width + x] = vga[y*width+x];
        }
    }

    for (uint16_t x = 0; x < width; x++){
        vga[(height-1) * width + x] = ' ' | currentColor;
    }
}

void deleteLast(){
    if (column == 0 && line != 0){
        line--;
        column = width;
    }
    vga[line * width + (--column)] = 0 | currentColor;
}

void __vga_print(const char* s){
    int _x = 0;
    while(s[_x]){
        switch(s[_x]){
            case '\n':
                newLine();
                break;
            case '\r':
                column = 0;
                break;
            case '\b':
                deleteLast();
                break;
            case '\t':
                if (column == width){
                    newLine();
                }
                uint16_t tabLen = 4 - (column % 4);
                while (tabLen != 0){
                    vga[line * width + (column++)] = ' ' | currentColor;
                    tabLen--;
                }
                break;
            default:
                _putchar(s[_x]);
                break;
        }
        _x++;
    }
}

int putchar(char c){
    int new_char_count = 0;
    switch(c){
        case '\n':
            newLine();
            break;
        case '\r':
            column = 0;
            break;
        case '\b':
            deleteLast();
            new_char_count--;
            break;
        case '\t':
            if (column == width){
                newLine();
            }
            uint16_t tabLen = 4 - (column % 4);
            new_char_count += tabLen;
            while (tabLen != 0){
                vga[line * width + (column++)] = ' ' | currentColor;
                tabLen--;
            }
            break;
        case '\0':
            // do nothing
            break;
        default:
            _putchar(c);
            new_char_count++;
            break;
    }
    set_cursor_pos(column, line);
    return new_char_count;
}

void _putchar(char c){
    if (column == width){
        newLine();
    }
    vga[line*width + (column++)] = c | currentColor;
}

void goto_column (uint32_t n){ column = n; }
void goto_line   (uint32_t n){ line   = n; }
void move_cursor(uint32_t x, uint32_t y){
    column = x;
    line = y;
}
void set_vga_position(uint32_t pos){
    column = pos%width;
    line = (pos-column)/width;
}
uint32_t get_vga_position(void){
    return line*width + column;
}


void set_text_color(uint16_t _col){
    // (COLOR8_LIGHT_GREY << 8) | (COLOR8_BLACK << 12);
    uint16_t bg_col = currentColor & 0xF000;
    currentColor = ((_col&0xF) << 8) | (bg_col << 12);
}

void set_background_color(uint16_t _col){
    uint16_t txt_col = currentColor & 0x0F00;
    currentColor = (txt_col << 8) | ((_col&0xF) << 12);
}

void set_color(uint16_t color){
    currentColor = (color << 8);
}


void enable_cursor(uint8_t cursor_start, uint8_t cursor_end){
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);

    __cursor_enabled = true;
}

void disable_cursor(){
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
    __cursor_enabled = false;
}

void set_cursor_pos(int x, int y){
	uint16_t pos = y * width + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void update_cursor(){
    set_cursor_pos(column, line);
}

bool cursor_enabled(){
    return __cursor_enabled;
}
