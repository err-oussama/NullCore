extern dived_error_handler
global isr_dummy


section .text 

%macro ISR_STUB 1

global isr_%1

isr_%1:
	pusha
	call %1
	popa
	iret

%endmacro

ISR_STUB dived_error_handler


isr_dummy:
	pusha
	popa
	iret



