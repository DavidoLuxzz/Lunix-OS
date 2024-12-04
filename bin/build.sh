# run from parent directory of "bin"
export PATH="/Users/luka/i686-elf-tools/Toolchain/i686-elf/bin:$PATH"
export CFLAGS="-m32 -I$PWD/../include/lib -fno-stack-protector -fno-builtin"
export GCC=i686-elf-gcc
export LIBS="$PWD/../lib"

echo "Compiling bin/ programs"

$GCC $CFLAGS -c ../src/bin/program.c -o bin_program.o

# linking process: use or not use libkern library
# i686-elf-ld -Ttext 0x0 -L$LIBS -lredir -m elf_i386 bin_program.o -e main -o program 

i686-elf-ld -Ttext 0x0 -m elf_i386 bin_program.o $LIBS/libredir.a -e main -o program 

cat program _zeroes > CD_ROM
