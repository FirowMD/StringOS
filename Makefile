BOOT=bootsect
KERNEL=kernel
GDB_SCR=script.gdb
SCR_CONTENT=																\
	target remote |															\
	qemu-system-i386 -fda $(BOOT).bin -fdb $(KERNEL).bin -S -gdb stdio\n	\
	file $(KERNEL).o\n														\
	file $(BOOT).o

all:
	as --32 -g -o $(BOOT).o $(BOOT).asm
	ld -Ttext 0x7c00 --oformat binary -m elf_i386 -o $(BOOT).bin $(BOOT).o
	gcc -g3 -fpermissive -fno-pie -ffreestanding -m32 -o $(KERNEL).o -c $(KERNEL).cpp
	ld --oformat binary -Ttext 0x10000 -o $(KERNEL).bin --entry=KernelStart -m elf_i386 $(KERNEL).o
	qemu-system-i386 -fda $(BOOT).bin -fdb $(KERNEL).bin

debug:
	echo "$(SCR_CONTENT)" | tee $(GDB_SCR)
	gdb -q -x $(GDB_SCR)

clean:
	rm -r *.o
	rm -r *.bin