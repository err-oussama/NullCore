global elf_syscall
global get_flags


get_flags:
	pushf
	pop eax
	ret

elf_syscall:
	mov eax, [esp + 4]
	int 0x80
	ret 
