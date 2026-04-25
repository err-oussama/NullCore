global lidtr
global sidtr

section .text

lidtr:
	mov eax, [ebp + 4]
	lidt [eax]
	ret


sidtr:
	mov eax, [ebp + 4]
	sidt [eax]
	ret
