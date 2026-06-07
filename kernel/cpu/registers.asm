global switch_esp

global get_esp



get_esp:
	mov eax, esp 
	add eax, 4
	ret

switch_esp:
	mov esp, [esp + 4]
	popa
	iret
	

