global get_cs;
global is64bit;

section .text


get_cs:
	xor eax, eax 
	mov ax, cs
	ret
