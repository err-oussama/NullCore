global lidtr
global sidtr


section .text

lidtr:
	mov eax, [esp + 4]
	lidt [eax]
	ret


sidtr:
	mov eax, [esp + 4]
	sidt [eax]
	ret

