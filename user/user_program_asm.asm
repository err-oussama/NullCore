global elf_syscall

elf_syscall:
	mov eax, 0
	int 0x80
	ret 
