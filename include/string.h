#pragma once
#include <stdint.h>

void strcpy(char* dest, const char* src);
void strncpy(char* dest, const char* src, size_t len);
void strncpyzx(char* dest, const char* src, size_t len);

size_t strlen(const char*);
size_t strdlen(const char*, char);
bool strnequ(const char*, const char*, size_t);
bool strdequ(const char*, const char*, char);
bool strcmd(const char*, const char*);

uint32_t stoi(const char*);
uint32_t stox(const char*);
uint32_t strsect(const char* s, uint32_t sect_size);

size_t strtok(const char*, char, uint16_t*);


