#pragma once
#include <stdint.h>

#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_LIGHT_BROWN 14
#define COLOR_WHITE 15

#define width 80
#define height 25

#define COLOR_DEFAULT 0xF700

// vga graphics cursor
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void set_cursor_pos(int x, int y);
void update_cursor();
bool cursor_enabled();
// text cursor
void goto_line(uint32_t);
void goto_column(uint32_t);
void move_cursor(uint32_t x, uint32_t y); // text cursor not the visible blinking cursor
void set_vga_position(uint32_t pos);
uint32_t get_vga_position(void);

void set_text_color(uint16_t);
void set_background_color(uint16_t);
void set_color(uint16_t);
void _putchar(char c); // raw char ('\n' isn't a new line)
int putchar(char c);
void deleteLast();
void print(const char* s);
void scrollUp();
void newLine();
void clear_screen();
void clear_color(uint8_t color);
