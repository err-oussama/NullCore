global switch_esp
global get_esp

global get_flags
global enable_interrupt
global disable_interrupt



enable_interrupt:
	sti
	ret
disable_interrupt:
	cli
	ret


get_flags:
	pushf
	pop eax 
	ret


get_esp:
	mov eax, esp 
	add eax, 4
	ret

switch_esp:
	mov esp, [esp + 4]
	popa
	iret
	

