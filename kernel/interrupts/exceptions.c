#include "isr.h"
#include "kprint.h"
#include "type.h"
#include <control_registers.h>

void divide_error_handler() {
  kprint_err("Exception: Division Error ");
  while (1)
    ;
}
void debug_handler() {
  kprint_err("Exception: Debug");
  while (1)
    ;
}
void non_maskable_interrupt_handler() {
  kprint_err("Exception: Non Maskable Interrupt");
  while (1)
    ;
}
void break_point_handler() {
  kprint_err("Exception: Break Point");
  while (1)
    ;
}
void overflow_handler() {
  kprint_err("Exception: overflow");
  while (1)
    ;
}
void bound_range_exceeded_handler() {
  kprint_err("Exception: Bound Range Exceeded");
  while (1)
    ;
}
void invalid_opcode_handler() {
  kprint_err("Exception: Invalid OPcode");
  while (1)
    ;
}
void device_not_avalibale_handler() {
  kprint_err("Exception: Device Not Available");
  while (1)
    ;
}
void double_fault_handler() {
  kprint_err("Exception:  Double Fault");
  while (1)
    ;
}
void coprocessor_segment_overrun_handler() {
  kprint_err("Interrupp: Coprecessor Segment Overrun");
  while (1)
    ;
}
void invalid_TSS_handler() {
  kprint_err("Exception: Invalid TSS");
  while (1)
    ;
}
void segment_not_present_handler() {
  kprint_err("Exception: Segment Not Present");
  while (1)
    ;
}
void stack_segment_fault_handler() {
  kprint_err("Exception: Stack Segment Fault");
  while (1)
    ;
}
void general_protection_fault_handler() {
  kprint_err("Exception: General Protection Fault");
  while (1)
    ;
}
void page_fault_handler(uint32 error) {
  kprint_err("Exception: Page Fault\n");
  kprint_str("CR2: 0x");
  kprint_hex(read_cr2());
  kprint_str("\nerror code: 0x");
  kprint_hex(error);
  kprint_str(error & 0x1 ? "\nPresent" : "\nNotPresnet");
  kprint_str(error & (0x1 << 1) ? "\nWrite" : "\nRead");
  kprint_str(error & (0x1 << 2) ? "\nUser mode" : "\nSupervisor mode");
  kprint_str(error & (0x1 << 3) ? "\nReserved-bits" : "\nNoReserved-bits");
  kprint_str(error & (0x1 << 4) ? "\nInstruction fetch" : "\nData access");

  while (1)
    ;
}
void x87_floating_point_exception_handler() {
  kprint_err("Exception: X87 Floating Point Exception");
  while (1)
    ;
}
void alignment_check_handler() {
  kprint_err("Exception: Alignment Check");
  while (1)
    ;
}
void machine_check_handler() {
  kprint_err("Exception: Machine Check");
  while (1)
    ;
}
void SIMD_floating_point_exception_handler() {
  kprint_err("Exception: SIMD Floating Point Exception");
  while (1)
    ;
}
void virtualization_exception_handler() {
  kprint_err("Exception: Virtualization Exception");
  while (1)
    ;
}
void control_protection_exception_handler() {
  kprint_err("Exception: Control Protection Exception");
  while (1)
    ;
}
void hypervisor_injection_exception_handler() {
  kprint_err("Exception: Hypervisor Injection Exception");
  while (1)
    ;
}
void VMM_communication_exception_handler() {
  kprint_err("Exception: VMM Communication Exception");
  while (1)
    ;
}
void security_exception_handler() {
  kprint_err("Exception: Security Exception");
  while (1)
    ;
}
