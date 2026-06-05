global switch_esp



switch_esp:
	mov esp, [esp + 4]
	iret
	

