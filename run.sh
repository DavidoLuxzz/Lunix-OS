qemu-system-i386 -m 512M -drive file=../LunixOS_drive,format=raw -drive file=bin/CD_ROM,format=raw -kernel Lunix/boot/kernel -boot order=d -machine pc
# ne znam zasta ce boot order=d ali kako god
# RAM: 512MB
# Za Storage ce koristiti fajl <drive> koji se nalazi pored Lunix.iso
# x86 32-bit
