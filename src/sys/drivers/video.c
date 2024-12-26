#include <sys/drivers/video.h>
#include <vga.h>
#include <util.h>

const uint16_t __lunix_video_defaultColor = (COLOR_LIGHT_GREY << 8) | (COLOR_BLACK << 12);

int __lunix_video_width, __lunix_video_height, __lunix_video_offset;
int __lunix_video_column, __lunix_video_line;

int* __lunix_video[5];

void __lunix_video_clear_screen(){
    __lunix_video_line = 0;
    __lunix_video_column = 0;
    uint16_t last_col = __vga_get_color();
    __vga_set_color(__lunix_video_defaultColor);

    for (uint16_t y = 0; y < __lunix_video_height; y++){
        for (uint16_t x = 0; x < __lunix_video_width; x++){
            __vga_setchar(0, x, y);
        }
    }
    __vga_set_color(last_col);
}

void __lunix_video_newLine(){
    if (__lunix_video_line < __lunix_video_height - 1){
        __lunix_video_line++;
        __lunix_video_column = 0;
    }else{
        __lunix_video_scrollUp();
        __lunix_video_column = 0;
    }
}

void __lunix_video_scrollUp(){
    for (uint16_t y = 0; y < __lunix_video_height; y++){
        for (uint16_t x = 0; x < __lunix_video_width; x++){
            __vga_setchar(__vga_getchar(x, y), x, y-1);
        }
    }

    for (uint16_t x = 0; x < __lunix_video_width; x++){
        __vga_setchar(0, x, __lunix_video_height-1);
    }
}

void __lunix_video_deleteLast(){
    if (__lunix_video_column == 0 && __lunix_video_line != 0){
        __lunix_video_line--;
        __lunix_video_column = __lunix_video_width;
    }
    __vga_setchar(0, (--__lunix_video_column), __lunix_video_line);
}

void __lunix_video_init(){
    __lunix_video[0] = &__lunix_video_width;
    __lunix_video[1] = &__lunix_video_height;
    __lunix_video[2] = &__lunix_video_column;
    __lunix_video[3] = &__lunix_video_line;
    __lunix_video[4] = &__lunix_video_offset;
    __lunix_video_width = __lunix_video_width;
    __lunix_video_height = __lunix_video_height;
    __lunix_video_offset = 0;
    __lunix_video_column=__lunix_video_line = 0;
}

void __lunix_video_chval(int id, int val){
    if (id >= 0 && id < 5) *__lunix_video[id] = val;
}

int __lunix_putchar(char c){
    int new_char_count = 0;
    switch(c){
        case '\n':
            __lunix_video_newLine();
            break;
        case '\r':
            __lunix_video_column = 0;
            break;
        case '\b':
            __lunix_video_deleteLast();
            new_char_count--;
            break;
        case '\0':
            // do nothing
            break;
        default:
            _putchar(c);
            new_char_count++;
            break;
    }
    set_cursor_pos(__lunix_video_column, __lunix_video_line);
    return new_char_count;
}

const driver_t *lunix_video_get_driver(){
    static const driver_t driver = {"Lunix mid-video driver", DT_VIDEO_MDV, __lunix_video_init, __lunix_video_chval, __lunix_putchar};
    return &driver;
}
