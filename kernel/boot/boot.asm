section .multiboot
align 4
	dd 0x1BADB002
	dd 0x0
	dd -(0x1BADB002 + 0x0)

section .text
global _start
extern kmain

_start:
	cli
	mov esp, stack_top
	push ebx
	call kmain

.hang:
	hlt
	jmp .hang

section .bss
resb 8192
stack_top:
