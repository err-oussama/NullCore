global tss_load
global deascalate


tss_load:
	mov eax, [esp + 4]
	ltr ax
	ret



deascalate:
    mov eax, [esp + 4]  ; eax = user EIP
    mov ebx, [esp + 8]  ; ebx = user ESP

    mov ecx, 0x23
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx          ; Don't forget ES, FS, GS!

    push 0x23           ; User SS
    push ebx            ; User ESP
    
    pushf
    pop ecx
    or ecx, 0x200
    push ecx            ; User EFLAGS
    
    push 0x1B           ; User CS
    push eax            ; User EIP

    iret


SECTION .user_test exec read 

global syscall

syscall:
	mov eax, 0
	int 0x80
	ret





