
global lgdtr;
global sgdtr;

lgdtr:
	mov eax, [esp + 4];
	lgdt [eax];
	ret
sgdtr:
	mov eax, [esp + 4]
	sgdt [eax];
	ret;
