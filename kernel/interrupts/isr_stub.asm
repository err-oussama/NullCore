global isr_dummy
global isr_timer_handler:
global isr_syscall_handler

global isr_page_fault_handler

; CPU exception
extern divide_error_handler
extern debug_handler
extern non_maskable_interrupt_handler
extern break_point_handler
extern overflow_handler
extern bound_range_exceeded_handler
extern invalid_opcode_handler
extern device_not_avalibale_handler
extern double_fault_handler
extern coprocessor_segment_overrun_handler
extern invalid_TSS_handler
extern segment_not_present_handler
extern stack_segment_fault_handler
extern general_protection_fault_handler
extern page_fault_handler
extern x87_floating_point_exception_handler
extern alignment_check_handler
extern machine_check_handler
extern SIMD_floating_point_exception_handler
extern virtualization_exception_handler
extern control_protection_exception_handler
extern hypervisor_injection_exception_handler
extern VMM_communication_exception_handler
extern security_exception_handler

extern kprint_hex
extern kprint_cha

; TIME
extern timer_handler

; KEYBAORD
extern keyboard_handler


; SYSTEM CALL 
extern syscall_dispatch


section .text 

%macro ISR_STUB 1

global isr_%1

isr_%1:
	pusha
	call %1
	popa
	iret

%endmacro

isr_syscall_handler:
	pusha
	call syscall_dispatch 
	popa
	iret

isr_timer_handler:
	pusha
	push esp 
	call timer_handler
	mov esp, eax
	popa
	mov eax, [esp + 4]
	and eax, 0x3
	cmp eax, 0x3
	jne .kernel_return 
	mov eax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
.kernel_return:
	iret


isr_page_fault_handler:
	pusha
	mov eax, [esp + 32]
	push eax 
	call page_fault_handler
	add esp, 4
	popa 
	add esp, 4
	iret

ISR_STUB divide_error_handler
ISR_STUB debug_handler
ISR_STUB non_maskable_interrupt_handler
ISR_STUB break_point_handler
ISR_STUB overflow_handler
ISR_STUB bound_range_exceeded_handler
ISR_STUB invalid_opcode_handler
ISR_STUB device_not_avalibale_handler
ISR_STUB double_fault_handler
ISR_STUB coprocessor_segment_overrun_handler
ISR_STUB invalid_TSS_handler
ISR_STUB segment_not_present_handler
ISR_STUB stack_segment_fault_handler
ISR_STUB general_protection_fault_handler
ISR_STUB x87_floating_point_exception_handler
ISR_STUB alignment_check_handler
ISR_STUB machine_check_handler
ISR_STUB SIMD_floating_point_exception_handler
ISR_STUB virtualization_exception_handler
ISR_STUB control_protection_exception_handler
ISR_STUB hypervisor_injection_exception_handler
ISR_STUB VMM_communication_exception_handler
ISR_STUB security_exception_handler

;
;ISR_STUB timer_handler
ISR_STUB keyboard_handler


isr_dummy:
	iret
