# Compiler / Assembler / Linker
CC      := gcc
ASM     := nasm
LD      := ld

# Flags
CFLAGS 	:= -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -c
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T linker.ld

# Files
ASM_SRC := $(shell find . -name "*.asm" ! -name "user_program_asm.asm")
ASM_OBJ := $(ASM_SRC:.asm=.o)

C_SRC   := $(shell find . -name "*.c" ! -name "user_program.c")
C_OBJ   := $(C_SRC:.c=.o)


# Header 
INCL   	:= 	-I./kernel/interrupts/ \
						-I./kernel/drivers/ \
						-I./kernel/include/ \
						-I./kernel/process/ \
						-I./kernel/memory/ \
						-I./kernel/klibc/ \
						-I./kernel/boot/ \
						-I./kernel/core/ \
						-I./kernel/cpu/ \
						-I./kernel/elf/ \

TARGET  := kernel.bin

# kernel/elf/initrd.o: kernel/elf/initrd.asm user_program
# 	$(ASM) $(ASFLAGS) $< -o $@

user/user_program_asm.o: user/user_program_asm.asm 
	$(ASM) $(ASFLAGS) $< -o $@


user_program: user/user_program_asm.o user/user_program.c
	$(CC) -m32 -nostdlib -nostartfiles -static -no-pie -fno-pic -fno-plt \
	      -T user/user.ld -o $@ $^



# Default target
all: disk.img user_program $(TARGET)

# Link everything
$(TARGET): $(ASM_OBJ) $(C_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^
	rm -f $(ASM_OBJ) $(C_OBJ)

# Compile C sources
kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) $(INCL)  $< -o $@

kernel/%.o: kernel/%.asm 
	$(ASM) $(ASFLAGS) $< -o $@


disk.img:
	qemu-img create -f raw disk.img 32M

# Running
run: all
	qemu-system-i386 -kernel $(TARGET) \
		-drive file=disk.img,format=raw,index=0,media=disk \
		-netdev user,id=net0 \
		-device rtl8139,netdev=net0,mac=52:54:00:12:34:56 \
		-object filter-dump,id=f1,netdev=net0,file=capture.pcap

# Clean
clean:
	rm -f $(ASM_OBJ) $(C_OBJ) user_program user/user_program_asm.o 
fclean: clean
	rm -f $(TARGET) user_program disk.img


.PHONY: clean fclean all run 
