#pragma once
#include <stdint.h>

#define TERMINAL_MAX_BUFFER_LENGTH 1024
#define TERMINAL_MAX_CMD_HISTORY 1

void __term_flush_stdin(void);
void __term_stdin_append(char);
bool __term_is_stdin_flushed();
char* const __term_get_stdin();
void __term_add_to_history();
char* const __term_get_and_update_stdin();

void __term_load_str_cmd();
void __term_save_str_cmd();

char* __term_get_previous_cmd();

bool __term_check_keycode(uint32_t);


