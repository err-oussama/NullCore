global elf_syscall

elf_syscall:
	mov eax, 0x1
	int 0x80
	ret 
