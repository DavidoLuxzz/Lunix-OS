# run from parent directory of "bin"
export PATH="/Users/luka/i686-elf-tools/Toolchain/i686-elf/bin:$PATH"
export CFLAGS="-m32 -I$PWD/include -fno-stack-protector -fno-builtin"
export GCC=i686-elf-gcc

echo "Compiling bin/ programs"
nasm -f bin src/bin/test.s -o bin/test


