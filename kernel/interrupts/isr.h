#ifndef ISR_H

#define ISR_H

// Interrupt service routine caller
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

//
void divide_error_handler();
void debug_handler();
void non_maskable_interrupt_handler();
void break_point_handler();
void overflow_handler();
void bound_range_exceeded_handler();
void invalid_opcode_handler();
void device_not_avalibale_handler();
void double_fault_handler();
void coprocessor_segment_overrun_handler();
void invalid_TSS_handler();
void segment_not_present_handler();
void stack_segment_fault_handler();
void general_protection_fault_handler();
void page_fault_handler();
void x87_floating_point_exception_handler();
void alignment_check_handler();
void machine_check_handler();
void SIMD_floating_point_exception_handler();
void virtualization_exception_handler();
void control_protection_exception_handler();
void hypervisor_injection_exception_handler();
void VMM_communication_exception_handler();
void security_exception_handler();
// pic
void timer_handler();
void keyboard_handler();

void isr_dummy();
#endif
