	.file	"partition.c"
	.intel_syntax noprefix
	.text
	.type	_get_partition_fs_name, @function
_get_partition_fs_name:
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	mov	eax, DWORD PTR [ebp+8]
	mov	BYTE PTR [ebp-4], al
	movzx	eax, BYTE PTR [ebp-4]
	mov	eax, DWORD PTR names.0[0+eax*4]
	leave
	ret
	.size	_get_partition_fs_name, .-_get_partition_fs_name
	.globl	_get_partition
	.type	_get_partition, @function
_get_partition:
	push	ebp
	mov	ebp, esp
	sub	esp, 552
	mov	edx, DWORD PTR [ebp+8]
	mov	eax, DWORD PTR [ebp+12]
	mov	BYTE PTR [ebp-540], dl
	mov	WORD PTR [ebp-544], ax
	movzx	eax, BYTE PTR [ebp-540]
	lea	edx, [ebp-524]
	push	edx
	push	1
	push	1
	push	eax
	call	ata_disk_read
	add	esp, 16
	movzx	eax, WORD PTR [ebp-544]
	sal	eax, 5
	mov	edx, eax
	lea	eax, [ebp-524]
	add	eax, edx
	mov	DWORD PTR [ebp-12], eax
	sub	esp, 4
	push	32
	push	DWORD PTR [ebp-12]
	push	DWORD PTR [ebp+16]
	call	memcpy
	add	esp, 16
	nop
	leave
	ret
	.size	_get_partition, .-_get_partition
	.globl	_is_partition_valid
	.type	_is_partition_valid, @function
_is_partition_valid:
	push	ebp
	mov	ebp, esp
	mov	eax, DWORD PTR [ebp+8]
	mov	eax, DWORD PTR [eax+20]
	cmp	eax, 1413761347
	sete	al
	pop	ebp
	ret
	.size	_is_partition_valid, .-_is_partition_valid
	.section	.rodata
.LC0:
	.string	"part id: %u\n"
	.text
	.globl	_set_partition
	.type	_set_partition, @function
_set_partition:
	push	ebp
	mov	ebp, esp
	sub	esp, 552
	mov	edx, DWORD PTR [ebp+8]
	mov	eax, DWORD PTR [ebp+12]
	mov	BYTE PTR [ebp-540], dl
	mov	WORD PTR [ebp-544], ax
	movzx	eax, WORD PTR [ebp-544]
	sub	esp, 8
	push	eax
	push	OFFSET FLAT:.LC0
	call	printf
	add	esp, 16
	movzx	eax, BYTE PTR [ebp-540]
	lea	edx, [ebp-524]
	push	edx
	push	1
	push	1
	push	eax
	call	ata_disk_read
	add	esp, 16
	lea	eax, [ebp-524]
	mov	DWORD PTR [ebp-12], eax
	movzx	eax, WORD PTR [ebp-544]
	sal	eax, 5
	mov	edx, eax
	mov	eax, DWORD PTR [ebp-12]
	add	eax, edx
	sub	esp, 4
	push	32
	push	DWORD PTR [ebp+16]
	push	eax
	call	memcpy
	add	esp, 16
	movzx	eax, BYTE PTR [ebp-540]
	push	DWORD PTR [ebp-12]
	push	1
	push	1
	push	eax
	call	ata_disk_write
	add	esp, 16
	nop
	leave
	ret
	.size	_set_partition, .-_set_partition
	.globl	_gui_partition
	.type	_gui_partition, @function
_gui_partition:
	push	ebp
	mov	ebp, esp
	sub	esp, 520
	mov	eax, DWORD PTR [ebp+8]
	mov	BYTE PTR [ebp-508], al
	call	_getip
	mov	DWORD PTR [ebp-12], eax
	movzx	eax, BYTE PTR [ebp-508]
	lea	edx, [ebp-492]
	push	edx
	push	1
	push	1
	push	eax
	call	ata_disk_read
	add	esp, 16
	sub	esp, 12
	push	128
	call	clear_color
	add	esp, 16
	call	clear_screen
	mov	eax, 0
	leave
	ret
	.size	_gui_partition, .-_gui_partition
	.section	.rodata
.LC1:
	.string	"ISO9660"
.LC2:
	.string	"LxFS"
.LC3:
	.string	"FAT32"
	.data
	.align 4
	.type	names.0, @object
	.size	names.0, 12
names.0:
	.long	.LC1
	.long	.LC2
	.long	.LC3
	.ident	"GCC: (GNU) 14.2.0"
