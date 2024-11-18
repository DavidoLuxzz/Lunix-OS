export PATH="/Users/luka/i686-elf-tools/Toolchain/i686-elf/bin:$PATH"
export CFLAGS="-m32 -I$PWD/include -fno-stack-protector -fno-builtin -c"
export GCC=i686-elf-gcc

# gcc
$GCC $CFLAGS src/kernel.c -o kernel.o
$GCC $CFLAGS src/vga.c -o vga.o
$GCC $CFLAGS src/gdt/gdt.c -o gdt.o
$GCC $CFLAGS src/util.c -o util.o
$GCC $CFLAGS src/interrupts/idt.c -o idt.o
$GCC $CFLAGS src/stdlib/stdio.c -o stdio.o
$GCC $CFLAGS src/keyboard.c -o keyboard.o
$GCC $CFLAGS src/timer.c -o timer.o
$GCC $CFLAGS src/kmalloc/kmalloc.c -o kmalloc.o
$GCC $CFLAGS src/stdlib/unistd.c -o unistd.o
$GCC $CFLAGS src/terminal.c -o terminal.o
$GCC $CFLAGS src/stdlib/string.c -o string.o
$GCC $CFLAGS src/sys/commands.c -o commands.o
$GCC $CFLAGS src/sys/drivers/disk_ata.c -o disk_ata.o
$GCC $CFLAGS src/sys/drivers/cd_atapi.c -o atapi.o
$GCC $CFLAGS src/fs/lxfs.c -o lxfs.o
$GCC $CFLAGS src/sys/drivers/disk_ide.c -o disk_ide.o
$GCC $CFLAGS src/inst.c -o os_install.o
$GCC $CFLAGS src/disk/partition.c -o partio.o
$GCC $CFLAGS src/sys/syscall.c -o syscall.o
$GCC $CFLAGS src/sys/elf.c -o elf.o
$GCC $CFLAGS src/sys/_process.c -o _proc.o
$GCC $CFLAGS src/sys/driver.c -o _driver.o

# compile system commands
echo "compiling system commands"
$GCC $CFLAGS src/sys/commands/*.c 

# space between gcc and nasm compilation
echo

# nasm
echo "nasm boot.s"
nasm -f elf32 src/boot.s -o boot.o
echo "nasm gdt.s"
nasm -f elf32 src/gdt/gdt.s -o gdts.o
echo "nasm idt.s"
nasm -f elf32 src/interrupts/idt.s -o idts.o
echo "nasm core_fn.s"
nasm -f elf32 src/stdlib/core_fn.s -o core_asm.o
echo "nasm cpu.s"
nasm -f elf32 src/cpu/cpu.s -o cpu_asm.o

# linking
echo "\nLinking...\n"
#i686-elf-ld --no-warn-rwx-segments -m elf_i386 -T linker.ld -o kernel boot.o kernel.o vga.o gdt.o gdts.o util.o idt.o idts.o stdio.o keyboard.o timer.o kmalloc.o unistd.o core_asm.o terminal.o string.o commands.o disk_ata.o atapi.o lxfs.o disk_ide.o os_install.o cpu_asm.o partio.o syscall.o elf.o _proc.o 
i686-elf-ld --no-warn-rwx-segments -m elf_i386 -T linker.ld -o kernel *.o


#clean (rm *.o)
rm *.o

# make ISO
echo "Moving kernel file into Lunix/boot/kernel"
mv kernel Lunix/boot/kernel
exit
echo "Making ISO\n"
grub-mkrescue -o Lunix.iso Lunix
