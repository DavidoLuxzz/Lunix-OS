int nigga = 12;

int main(){
    *(char*)0xb8124 = '*';
    nigga += 2;
    nigga = nigga - 8;
    nigga = 9;
    nigga ^= nigga;
    return nigga;
}
