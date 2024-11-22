# run from parent directory of "bin"
export PATH="/Users/luka/i686-elf-tools/Toolchain/i686-elf/bin:$PATH"
export CFLAGS="-m32 -I$PWD/include -fno-stack-protector -fno-builtin -maddress-mode=short"
export GCC=i686-elf-gcc

echo "Compiling standard library"
nasm -f elf32 ../src/sys/libkern/std.s -o libkern_std.o

echo "Compiling bin/ programs"
nasm -f elf32 ../src/bin/test.s -o bin_test.o

$GCC $CFLAGS -c ../src/bin/program.c -o bin_program.o

# linking process: use or not use libkern library
i686-elf-ld -m elf_i386 -Ttext 0x0 -e main -o program bin_program.o libkern_std.o


