global elf_syscall
global get_flags
global write


get_flags:
	pushf
	pop eax
	ret

elf_syscall:
	mov eax, [esp + 4]
	int 0x80
	ret 


write:
	mov ebx, [esp + 4]
	mov eax, 1
	int 0x80
	ret
