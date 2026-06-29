global tss_load


tss_load:
	mov eax, [esp + 4]
	ltr ax
	ret
