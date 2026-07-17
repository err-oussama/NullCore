global syscall


syscall:
	mov ebx, [esp + 8]
	mov eax, [esp + 4]
	int 0x80
	ret
