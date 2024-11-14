#include <stdint.h>
#include <util.h>
#include <interrupts/idt.h>
#include <stdio.h>
#include <keyboard.h>
#include <vga.h>
#include <terminal.h>

bool capsOn;
bool capsLock;
bool altOn;

uint8_t keyboard_mode = K_MODE_TYPE;

const uint32_t UNKNOWN = 0xFFFFFFFF;
const uint32_t ESC = 0xFFFFFFFF - 1;
const uint32_t CTRL = 0xFFFFFFFF - 2;
const uint32_t LSHFT = 0xFFFFFFFF - 3;
const uint32_t RSHFT = 0xFFFFFFFF - 4;
const uint32_t ALT = 0xFFFFFFFF - 5;
const uint32_t F1 = 0xFFFFFFFF - 6;
const uint32_t F2 = 0xFFFFFFFF - 7;
const uint32_t F3 = 0xFFFFFFFF - 8;
const uint32_t F4 = 0xFFFFFFFF - 9;
const uint32_t F5 = 0xFFFFFFFF - 10;
const uint32_t F6 = 0xFFFFFFFF - 11;
const uint32_t F7 = 0xFFFFFFFF - 12;
const uint32_t F8 = 0xFFFFFFFF - 13;
const uint32_t F9 = 0xFFFFFFFF - 14;
const uint32_t F10 = 0xFFFFFFFF - 15;
const uint32_t F11 = 0xFFFFFFFF - 16;
const uint32_t F12 = 0xFFFFFFFF - 17;
const uint32_t SCRLCK = 0xFFFFFFFF - 18;
const uint32_t HOME = 0xFFFFFFFF - 19;
const uint32_t UP = 0xFFFFFFFF - 20;
const uint32_t LEFT = 0xFFFFFFFF - 21;
const uint32_t RIGHT = 0xFFFFFFFF - 22;
const uint32_t DOWN = 0xFFFFFFFF - 23;
const uint32_t PGUP = 0xFFFFFFFF - 24;
const uint32_t PGDOWN = 0xFFFFFFFF - 25;
const uint32_t END = 0xFFFFFFFF - 26;
const uint32_t INS = 0xFFFFFFFF - 27;
const uint32_t DEL = 0xFFFFFFFF - 28;
const uint32_t CAPS = 0xFFFFFFFF - 29;
const uint32_t NONE = 0xFFFFFFFF - 30;
const uint32_t ALTGR = 0xFFFFFFFF - 31;
const uint32_t NUMLCK = 0xFFFFFFFF - 32;


const uint32_t lowercase[128] = {
UNKNOWN,ESC,'1','2','3','4','5','6','7','8',
'9','0','+',UNKNOWN,'\b','\t','q','w','e','r',
't','y','u','i','o','p','[',']','\n',CTRL,
'a','s','d','f','g','h','j','k','l',':',
'\'','|',LSHFT,'\\','z','x','c','v','b','n','m',',',
'.','-',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',LEFT,UNKNOWN,RIGHT,
'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

const uint32_t uppercase[128] = {
    UNKNOWN,ESC,'!','\"','#','$','%','&','/','(',')','=','?','`','\b','\t','Q','W','E','R',
'T','Y','U','I','O','P','{','}','\n',CTRL,'A','S','D','F','G','H','J','K','L',':','"','~',LSHFT,'|','Z','X','C',
'V','B','N','M','<','>','?',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',
LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

const uint32_t altcase[128] = {
    UNKNOWN,ESC,UNKNOWN,'@',UNKNOWN,'$',UNKNOWN,UNKNOWN,'|','[',']',UNKNOWN,UNKNOWN,UNKNOWN,'\b','\t','q','w','e','r',
't','y','u','i','o','p',UNKNOWN,'~','\n',CTRL,'a','s','d','f','g','h','j','k','l',UNKNOWN,UNKNOWN,UNKNOWN,LSHFT,UNKNOWN,'Z','X','C',
'V','B','N','M',',','.','-',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',
LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

bool __KEYS[256];  // real-time keys
bool __KEYLOCKS[256];

void keyboardHandler(struct InterruptRegisters *regs){
    uint32_t keycode = inb(0x60);
    char scanCode = keycode & 0x7F; //What key is pressed
    char press = inb(0x60) & 0x80; //Press down, or released
    
    if (keyboard_mode == K_MODE_TYPE){ // free type
        _keyboard_putc(scanCode, press, 0);
    } else if (keyboard_mode == K_MODE_TERMINAL){ // type to stdin, also to screen
        if (!__term_check_keycode(keycode, press))
            _keyboard_putc(scanCode, press, 1);
    }
    __KEYS[scanCode] = press==0;
}

void _keyboard_putc(char scanCode, int8_t press, bool _trm){
    switch(scanCode){
        case 28: // enter
            if (press == 0) {
                if (_trm) __term_stdin_append('\n');
                else newLine();
            }
            break;
        case 56: // alt
            if (press == 0){
                altOn = true;
            } else altOn = false;
            break;
        case 1:
        case 29:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 87:
        case 88:
            break;
        case 42:
            //shift key
            if (press == 0){
                capsOn = true;
            }else{
                capsOn = false;
            }
            break;
        case 58:
            if (!capsLock && press == 0){
                capsLock = true;
            }else if (capsLock && press == 0){
                capsLock = false;
            }
            break;
        default:
            if (press == 0){
                if (altOn){
                    if (_trm) __term_stdin_append(altcase[scanCode]);
                    else putc(altcase[scanCode]);
                } else if (capsOn || capsLock){
                    if (_trm) __term_stdin_append(uppercase[scanCode]);
                    else putc(uppercase[scanCode]);
                } else {
                    if (_trm) __term_stdin_append(lowercase[scanCode]);
                    else putc(lowercase[scanCode]);
                }
            }
            break;
    }
}

bool kGetKey(uint8_t key){
    int gk = __KEYS[key];
    if (kIsKeyLocked(key)){
        __KEYLOCKS[key] = gk;
        return 0;
    }
    return gk;
}
void kLockKey(uint8_t key){
    __KEYLOCKS[key] = 1;
}
void kUnlockKey(uint8_t key){
    __KEYLOCKS[key] = 0;
}
bool kIsKeyLocked(uint8_t key){
    return __KEYLOCKS[key];
}

void initKeyboard(){
    capsOn = false;
    capsLock = false;
    altOn = false;
    memset(__KEYS, 0, 256);
    memset(__KEYLOCKS, 0, 256);
    irq_install_handler(1,&keyboardHandler);
}

void k_setKeyboardMode(uint8_t mode){
    keyboard_mode = mode;
    if (mode==K_MODE_TYPE || mode==K_MODE_TERMINAL){
        if (!cursor_enabled()) enable_cursor(0, 0xF);
    } else {
        if (cursor_enabled()) disable_cursor();
    }
}

