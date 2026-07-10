# Compiler / Assembler / Linker
CC      := gcc
ASM     := nasm
LD      := ld

# Flags
CFLAGS 	:= -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -c
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386 -T linker.ld

# Files
ASM_SRC := $(shell find . -name "*.asm")
ASM_OBJ := $(ASM_SRC:.asm=.o)

C_SRC   := $(shell find . -name "*.c")
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

user_program_asm.o: user_program_asm.asm 
	$(ASM) $(ASFLAGS) $< -o $@

user_program: user_program_asm.o user_program.c 
	$(CC) -m32 -nostdlib -nostartfiles -static -no-pie -Ttext=0x400000 -o $@ $^




# Default target
all: user_program $(TARGET)

# Link everything
$(TARGET): $(ASM_OBJ) $(C_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^
	rm -f $(ASM_OBJ) $(C_OBJ)

# Compile C sources
kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) $(INCL)  $< -o $@

kernel/%.o: kernel/%.asm 
	$(ASM) $(ASFLAGS) $< -o $@



# Running
run: all
	qemu-system-x86_64 -kernel $(TARGET)

# Clean
clean:
	rm -f $(ASM_OBJ) $(C_OBJ) 
fclean: clean
	rm -f $(TARGET) user_program


.PHONY: clean fclean all run 
