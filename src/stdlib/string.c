#include <string.h>

void strcpy(char* dest, const char* src){
    int i;
    for (i=0; src[i]; i++){
        dest[i] = src[i];
    }
    dest[i] = 0;
}

void strncpy(char* dest, const char* src, size_t len){
    uint32_t i;
    for (i=0; i<len; i++){
        dest[i] = src[i];
    }
    dest[i] = 0;
}

void strncpyzx(char* dest, const char* src, size_t len){
    uint32_t i;
    char v=1;
    for (i=0; i<len; i++){
        if (v) v = (src[i]!=0);
        dest[i] = src[i]*v;
    }
}

size_t strlen(const char* s){
    size_t ret=0;
    while (s[++ret]);
    return ret;
}
size_t strdlen(const char* s, char d){
    size_t ret=0;
    while (s[ret] && s[ret]^d) ret++;
    return ret;
}
bool strdequ(const char* s1, const char* s2, char d){
    size_t len = strdlen(s1, d);
    return strnequ(s1, s2, len);
}
bool strcmd(const char* full, const char* s){ // command == s?
    size_t len = strdlen(full, ' ');
    size_t req_len = strlen(s);
    if (len != req_len) return false;
    return strnequ(full, s, len);
}

bool strnequ(const char* s1, const char* s2, size_t len){
    for (size_t i=0; i<len; i++){
        if (s1[i] != s2[i]) return false;
    }
    return true;
}
size_t strtok(const char* s, char d, uint16_t *tokens){
    size_t ntokens=0;
    for (uint16_t i=0; i<strlen(s); i++){
        if (s[i]==d){
            ++i;
            if (s[i]!=d && s[i]!=0){
                tokens[ntokens++]=i;
            } else --i;
        }
    }
    return ntokens;
}
#include <stdio.h>
uint32_t stoi(const char* s){
    int i, _10=1;
    uint32_t res=0;
    for (i=strdlen(s, ' ')-1; i>=0; i--){
        res += (s[i]-'0') * _10;
        _10*=10;
    }
    return res;
}
uint32_t stox(const char* s){
    int i;
    uint32_t res=0, bs=0;
    for (i=strdlen(s, ' ')-1; i>=0; i--){
        char c = s[i];
        if (c>='a') res += ((c-'a'+0xa)<<bs);
        else if (c>='A') res += ((c-'A'+0xA)<<bs);
        else if (c>='0') res += ((c-'0'+0x0)<<bs);
        bs+=4;
    }
    return res;
}

uint32_t strsect(const char* s, uint32_t sect_size){
    int i, _10=1;
    uint32_t res=0;
    uint32_t mul=1;
    for (i=strdlen(s, ' ')-1; i>=0; i--){
        switch(s[i]){
            case 'K':
                mul = 0x400;
                break;
            case 'M':
                mul = 0x100000;
                break;
            case 'G':
                mul = 0x40000000;
                break;
            default:
                res += (s[i]-'0') * _10;
                _10*=10;
                break;
        }
    }
strsect_end_calc:
    if (sect_size > mul)
        res = res*mul / sect_size;
    else res = res * (mul/sect_size);
    return res;
}




