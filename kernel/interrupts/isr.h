#ifndef ISR_H
#define ISR_H

// Interrupt service routine caller

// exception
void isr_divide_error_handler();
void isr_debug_handler();
void isr_non_maskable_interrupt_handler();
void isr_break_point_handler();
void isr_overflow_handler();
void isr_bound_range_exceeded_handler();
void isr_invalid_opcode_handler();
void isr_device_not_avalibale_handler();
void isr_double_fault_handler();
void isr_coprocessor_segment_overrun_handler();
void isr_invalid_TSS_handler();
void isr_segment_not_present_handler();
void isr_stack_segment_fault_handler();
void isr_general_protection_fault_handler();
void isr_page_fault_handler();
void isr_x87_floating_point_exception_handler();
void isr_alignment_check_handler();
void isr_machine_check_handler();
void isr_SIMD_floating_point_exception_handler();
void isr_virtualization_exception_handler();
void isr_control_protection_exception_handler();
void isr_hypervisor_injection_exception_handler();
void isr_VMM_communication_exception_handler();
void isr_security_exception_handler();

// pic
void isr_timer_handler();
void isr_keyboard_handler();

void isr_dummy();
#endif
