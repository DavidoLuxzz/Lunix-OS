export PATH="/Users/luka/i686-elf-tools/Toolchain/i686-elf/bin:$PATH"
export CFLAGS="-m32 -I$PWD/include -fno-stack-protector -fno-builtin -c"
export GCC=i686-elf-gcc

# gcc
echo "gcc kernel.c"
$GCC $CFLAGS src/kernel.c -o kernel.o
echo "gcc vga.c"
$GCC $CFLAGS src/vga.c -o vga.o
echo "gcc gdt.c"
$GCC $CFLAGS src/gdt/gdt.c -o gdt.o
echo "gcc util.c"
$GCC $CFLAGS src/util.c -o util.o
echo "gcc idt.c"
$GCC $CFLAGS src/interrupts/idt.c -o idt.o
echo "gcc stdio.c"
$GCC $CFLAGS src/stdlib/stdio.c -o stdio.o
echo "gcc keyboard.c"
$GCC $CFLAGS src/keyboard.c -o keyboard.o
echo "gcc timr.c"
$GCC $CFLAGS src/timer.c -o timer.o
echo "gcc kmalloc/kmalloc.c"
$GCC $CFLAGS src/kmalloc/kmalloc.c -o kmalloc.o
echo "gcc unistd.c"
$GCC $CFLAGS src/stdlib/unistd.c -o unistd.o
echo "gcc terminal.c"
$GCC $CFLAGS src/terminal.c -o terminal.o
echo "gcc string.c"
$GCC $CFLAGS src/stdlib/string.c -o string.o
echo "gcc commands.c"
$GCC $CFLAGS src/sys/commands.c -o commands.o
echo "gcc drivers/disk_ata.c"
$GCC $CFLAGS src/sys/drivers/disk_ata.c -o disk_ata.o
echo "gcc drivers/cd_atapi.c"
$GCC $CFLAGS src/sys/drivers/cd_atapi.c -o atapi.o
echo "gcc fs/lxfs.c"
$GCC $CFLAGS src/fs/lxfs.c -o lxfs.o
echo "gcc drivers/disk_ide.c"
$GCC $CFLAGS src/sys/drivers/disk_ide.c -o disk_ide.o
echo "gcc inst.c"
$GCC $CFLAGS src/inst.c -o os_install.o
echo "gcc disk/partition.c"
$GCC $CFLAGS src/disk/partition.c -o partio.o
echo "gcc syscall.c"
$GCC $CFLAGS src/sys/syscall.c -o syscall.o
echo "gcc sys/elf.c"
$GCC $CFLAGS src/sys/elf.c -o elf.o
echo "gcc sys/_process.c"
$GCC $CFLAGS src/sys/_process.c -o _proc.o
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
