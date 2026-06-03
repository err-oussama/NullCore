global save_context
global load_context
global switch_context
global get_esp


section .text





save_context:
	push eax
	mov eax, [esp + 8]
	pop dword[eax]
	mov [eax + 4], ebx
	mov [eax + 8], ecx
	mov [eax + 12], edx
	mov [eax + 16], edi 
	mov [eax + 20], esi
	mov [eax + 24], ebp
	ret

load_context:
	mov eax, [esp + 4]
	mov ebx, [eax + 4]
	mov ecx, [eax + 8]
	mov edx, [eax + 12]
	mov edi, [eax + 16]
	mov esi, [eax + 20]

	ret 

switch_context:
	ret



get_esp:
	mov eax, esp 
	ret



