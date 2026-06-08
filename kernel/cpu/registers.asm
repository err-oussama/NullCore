global switch_esp
global enable_interrupt
global disable_interrupt

global get_esp

enable_interrupt:
	sti
	ret
disable_interrupt:
	cli
	ret


get_esp:
	mov eax, esp 
	add eax, 4
	ret

switch_esp:
	mov esp, [esp + 4]
	popa
	iret
	

