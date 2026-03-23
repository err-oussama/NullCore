# x86 Kernel (From Scratch)

A minimal kernel built from the ground up to understand how a system actually boots, executes, and interacts with hardware at the lowest level.

This is not a framework, not a toy abstraction, and not based on any existing OS code.  
The goal is to control execution directly: from CPU state to memory layout.

---

## Objectives

- Enter protected/long mode
- Set up a valid execution environment (GDT, stack, paging)
- Write directly to hardware (VGA text buffer)
- Handle interrupts (IDT, PIC/APIC)
- Build toward basic scheduling and system calls

---

## Current State

- [ ] Kernel entry point
- [ ] Stack initialization
- [ ] VGA text output
- [ ] GDT setup
- [ ] IDT / interrupt handling
- [ ] Paging (virtual memory)
- [ ] Timer interrupts
- [ ] Basic scheduler

---

## Build

Requirements:
- `nasm`
- `gcc` (cross-compiler recommended)
- `ld`
- `make`
- `qemu-system-x86_64`

Build the kernel:

```bash
make
