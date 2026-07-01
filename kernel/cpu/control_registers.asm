global write_cr3
global read_cr3

global write_cr0
global read_cr0

global read_cr2

section .text

write_cr3:
	mov eax, [esp + 4]
	mov cr3, eax 
	ret 



read_cr3:
	mov eax, cr3 
	ret


write_cr0:
	mov eax, [esp + 4]
	mov cr0, eax 
	ret 



read_cr0:
	mov eax, cr0 
	ret


read_cr2:
	mov eax, cr2;
	ret
