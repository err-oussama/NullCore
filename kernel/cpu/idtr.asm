global lidtr
global sidtr
global activate_interrupt 


section .text

lidtr:
	mov eax, [esp + 4]
	lidt [eax]
	ret


sidtr:
	mov eax, [esp + 4]
	sidt [eax]
	ret

activate_interrupt:
	sti 
	ret 
