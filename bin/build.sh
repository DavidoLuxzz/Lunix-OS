# run from parent directory of "bin"
export PATH="/Users/luka/i686-elf-tools/Toolchain/i686-elf/bin:$PATH"
export CFLAGS="-m32 -I$PWD/include -fno-stack-protector -fno-builtin"
export GCC=i686-elf-gcc

echo "Compiling standard library"
nasm -f elf32 ../src/sys/libkern/std.s -o libkern_std.o

echo "Compiling bin/ programs"
nasm -f elf32 ../src/bin/test.s -o bin_test.o

# linking process: use or not use libkern library
i686-elf-ld -m elf_i386 -e _start -o test bin_test.o libkern_std.o


