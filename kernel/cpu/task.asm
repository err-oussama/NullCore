global save_context
global load_context
global switch_context


section .text





save_context:
	push eax; save 'eax' in stack so i can use it as a base
	mov eax, [ebp + 8]; store context ptr in eax
	mov [eax + 4], ebx
	mov [eax + 8], ecx
	mov [eax + 12], edx
	mov [eax + 16], edi 
	mov [eax + 20], esi
	mov [eax + 24], ebp 
	;mov [eax + 32], eflags
	;mov bx, cs
	;mov [eax + 38], bx
	;mov bx, ds
	;mov [eax + 40], bx
	;mov bx, es
	;mov [eax + 42], bx
	;mov bx, fs
	;mov [eax + 44], bx
	;mov bx, gs
	;mov [eax + 46], bx
	;mov bx, ss
	;mov [eax + 48], bx
	;mov edx, eax 
	pop eax 
	mov edx, eax
	mov eax, [edx]
	;mov [eax + 28], esp
	;mov [edx + 36],eax 

	ret

load_context:
	mov eax, [ebp + 4]; store the context address in `eax`
	mov ecx, [eax + 4]
	mov ecx, [eax + 8]
	mov edx, [eax + 12]
	mov edi, [eax + 16]
	mov esi, [eax + 20]

	ret 

switch_context:
	ret



