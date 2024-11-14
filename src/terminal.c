#include <terminal.h>
#include <vga.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <keyboard.h>

char _stdin[TERMINAL_MAX_BUFFER_LENGTH];
char _stdout[TERMINAL_MAX_BUFFER_LENGTH];

char _str_cmd[TERMINAL_MAX_BUFFER_LENGTH];
uint32_t _str_len;
char _tmp_cmd[TERMINAL_MAX_BUFFER_LENGTH];
uint32_t _tmp_len;
bool _showing_his;

uint32_t _term_stdin_current_len;
bool _term_stdin_flushed = false;

bool ___term_ctrl_on = false;

void __term_stdin_append(char c){
    if (_term_stdin_flushed) {
        _term_stdin_flushed = false;
        _term_stdin_current_len = 0;
    }
    if (_term_stdin_current_len >= TERMINAL_MAX_BUFFER_LENGTH){
        _term_stdin_current_len = TERMINAL_MAX_BUFFER_LENGTH;
        __term_flush_stdin();
        return;
    }
    switch(c){
        case 0:
        case '\n':
            __term_flush_stdin();
            newLine();
            break;
        case '\b':
            if (_term_stdin_current_len) {
                _term_stdin_current_len--;
                deleteLast();
                update_cursor();
            }
            break;
        case '\t':
            int ncc = putchar('\t');
            for (int i=0; i<ncc; i++){
                _stdin[_term_stdin_current_len++] = ' ';
            }
            break;
        default:
            _stdin[_term_stdin_current_len++] = c;
            putchar(c);
            _showing_his = false;
            break;
    }
    *(char*) 0xb8000 = _term_stdin_current_len + '0';
}

void __term_flush_stdin(void){
    _stdin[_term_stdin_current_len] = 0;
    _term_stdin_flushed = true;
    if(_term_stdin_current_len) __term_save_str_cmd(_stdin);
    _showing_his = false;
    _tmp_len = 0;
}

bool __term_is_stdin_flushed(){ return _term_stdin_flushed; }

char* const __term_get_stdin(){
    _term_stdin_flushed = false;
    _term_stdin_current_len = 0;
    return _stdin;
}
void __term_save_tmp(){
    strncpy(_tmp_cmd, _stdin, TERMINAL_MAX_BUFFER_LENGTH);
    _tmp_len = _term_stdin_current_len;
    _tmp_cmd[_term_stdin_current_len] = 0;
}
void __term_load_tmp(){
    strncpy(_stdin, _tmp_cmd, TERMINAL_MAX_BUFFER_LENGTH);
    _term_stdin_current_len = _tmp_len;
}
/**
 * Returns current stdin value and adds it to command history
 * stdin gets flushed and after that it won't be avaiable for read.
 */
char* const __term_get_and_update_stdin(){
    if (_term_stdin_current_len)
        __term_save_str_cmd();
    return __term_get_stdin();
}

bool __term_check_keycode(uint32_t keycode, char press){
    switch (keycode){
        case K_UP:
            if (!_showing_his){
                //printf("\nlen: %u %x\n", _term_stdin_current_len, _stdin[1]);
                __term_save_tmp();
                 
                // display new stdin
                for (uint32_t i=0; i<_term_stdin_current_len; i++) putchar('\b');
                
                __term_load_str_cmd();
                print(_stdin);
                _showing_his = true;

                *(char*) 0xb8000 = _term_stdin_current_len + '0';
            }
            return 1;
        case K_DOWN:
            printu(_tmp_len);
            if (_showing_his){
                _showing_his = false;
                
                // display old stdin
                for (uint32_t i=0; i<_term_stdin_current_len+1; i++) putchar('\b');
                
                __term_save_str_cmd();
                __term_load_tmp();
                print(_stdin);

                *(char*) 0xb8000 = _term_stdin_current_len + '0';
            }
            return 1;
        case K_CTRL:
            ___term_ctrl_on = !press;
            return 1;
        case 46: // C
            if (___term_ctrl_on) for(;;);
            return 0;
        case 0xE0:
            return 1;
        default:
            return 0;
    }
    return 0;
}

// arrow keys
void __term_load_str_cmd(){
    strncpy(_stdin, _str_cmd, TERMINAL_MAX_BUFFER_LENGTH);
    _term_stdin_current_len = _str_len;
}

void __term_save_str_cmd(){
    strncpy(_str_cmd, _stdin, TERMINAL_MAX_BUFFER_LENGTH);
    _str_len = _term_stdin_current_len;
}

void __term_nullize_stdin(void){
    memset(_stdin, 0, TERMINAL_MAX_BUFFER_LENGTH);
}

