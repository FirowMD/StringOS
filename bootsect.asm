.att_syntax

.code16
.global _start

_start:
	# Text mode
	movb $0x00, %ah
	movb $0x03, %al
	int $0x10

	# Get mode ("bm" or "std")
	call set_stringos_mode

	# Write result of set_stringos_mode() to {mode}
	movw $mode, %bx
	movb %al, 0(%bx)
	
# Allocates memory on drive
load_drive:
	# 1st Head
	movb $0x02, %ah # Function "read"
	movb $0x7f, %al # Number of sectors to read (0x01 .. 0x80)
	movb $0x01, %cl # First available sector (remember that bootloader on floopy1)
	movb $0x00, %dh # Head number
	movb $0x00, %ch # Cylinder
	movb $0x01, %dl # Disk number, floppy2
	movw $0x1000, %bx
	movw %bx, %es
	xorw %bx, %bx
	int $0x13

set_protected_mode:
	cli
	lgdt gdt_info
	inb $0x92, %al
	orb $2, %al
	outb %al, $0x92
	movl %cr0, %eax
	orb $1, %al
	movl %eax, %cr0
	ljmp $0x8, $protected_mode

.code32
protected_mode:

start_kernel:
	# Selectors loading
	movw $0x10, %ax
	movw %ax, %es
	movw %ax, %ds
	movw %ax, %ss

	# Mode for kernel
	movl $mode, %ebx
	movl 0(%ebx), %eax
	movl $0xbf1c, %ebx
	movl %eax, 0(%ebx)

	# Start kernel
	call 0x10000

# # # # # # FUNCTIONS # # # # # #
.code16
# Reads a byte to %al
get_char:
	movb $0x00, %ah # Key read function
	int $0x16
	movb $0x0E, %ah # Output
	int $0x10
	ret

# Returns stringos mode in %al ("bm" (0) or "std" (1))
set_stringos_mode:
	pusha
	movw $last_chars_buf, %bx
	movw %bx, %di
	addw $0x02, %di

set_stringos_mode_loop:
	call get_char
	call update_last_chars
	movb %al, 2(%bx) # Move new byte to 3rd position

	# Check for matching to "std" or "bm"
	call is_std
	cmpb $0x00, %al
	je set_stringos_mode_std

	call is_bm
	cmpb $0x00, %al
	je set_stringos_mode_bm

	jmp set_stringos_mode_loop

set_stringos_mode_std:
	popa
	movb $0x00, %al
	ret

set_stringos_mode_bm:
	popa
	movb $0x01, %al
	ret

# Updates history of pressed keys, {last_chars_buf}
# Moves items like this: [ 's', 't', 'r', 0 ] => [ 't', 'r', 'r', 0 ]
update_last_chars:
	pusha
	movb 2(%bx), %al
	movb 1(%bx), %ah
	movb %al, 1(%bx)
	movb %ah, 0(%bx)
	popa
	ret

# Sets %al as 0, if {last_chars_buf} stores "std"
is_std:
	pusha
	cld # Clear D (direction flag)
	movw $0x04, %cx
	movw $str_std, %si
	movw $last_chars_buf, %di
	repe cmpsb
	jcxz is_std_true
	popa
	movb $0xFF, %al
	ret
is_std_true:
	popa
	movb $0x00, %al
	ret

# Sets %al as 0, if {last_chars_buf} stores "bm"
is_bm:
	pusha
	cld # Clear D (direction flag)
	movw $0x03, %cx
	movw $str_bm, %si
	movw $last_chars_buf, %di
	incw %di # We need to check only 2 bytes
	repe cmpsb
	jcxz is_bm_true
	popa
	movb $0xFF, %al
	ret
is_bm_true:
	popa
	movb $0x00, %al
	ret

# Prints string with '\n\r'
puts:
	pusha
	movb $0x0e, %ah
puts_loop:
	movb 0(%bx), %al
	test %al, %al
	jz end_puts
	int $0x10
	incw %bx
	jmp puts_loop
end_puts:
	movb $0x0a, %al # Line feed
	int $0x10
	movb $0x0d, %al
	int $0x10
	popa
	ret

# # # # # # D A T A # # # # # #
str_std:
	.asciz "std"

str_bm:
	.asciz "bm"

last_chars_buf:
	.byte 0x00, 0x00, 0x00, 0x00 # Keeps 3 last chars entered

mode:
	.byte 0x00, 0x00, 0x00, 0x00

gdt:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
	.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
	.word gdt_info - gdt
	.word gdt, 0

# Unreachable code
unreachable:
	.zero (510 - (. - _start))
	.byte 0x55, 0xAA
