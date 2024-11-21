#pragma once
#include <util.h>

#define K_MODE_TYPE 0
#define K_MODE_TERMINAL 1
#define K_MODE_DISABLED 0xFF

// keys set in charset
#define K_ENTER 28
#define K_UNKNOWN 0
#define K_ESC 1
#define K_UP 0x48
#define K_LEFT 0x4B
#define K_RIGHT 0x4D
#define K_DOWN 0x50
#define K_CTRL 0x1D


void initKeyboard();
void _keyboard_putc(char, int8_t, bool);
void keyboardHandler(struct InterruptRegisters *regs);
void kLockKey(unsigned char);
void kUnlockKey(unsigned char);
bool kIsKeyLocked(unsigned char);
bool kGetKey(unsigned char key);
void k_setKeyboardMode(uint8_t);

#include <sys/driver.h>
const driver_t *k_keyboard_get_driver(void);
