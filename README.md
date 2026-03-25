# x86 Kernel — From First Principles

A minimal x86 kernel built from scratch with the sole purpose of understanding **how a 
machine actually executes code**, from the first instruction to hardware interaction.

This project avoids abstractions and external dependencies. Every component is implemented
manually to expose the real mechanics of:

* CPU execution
* memory layout
* privilege boundaries
* hardware communication

---

## Philosophy

This is not a framework, and not an attempt to recreate an existing OS.

The objective is to **own the execution environment completely**:

* No runtime
* No standard library
* No hidden initialization
* No reliance on existing kernel code

Every step is explicit, controlled, and documented.

---

## Scope

The kernel is developed incrementally, following the actual hardware bring-up sequence 
on x86.

### Core Areas

* Execution environment setup (stack, segments)
* Memory management (segmentation → paging)
* Interrupt handling (exceptions + hardware IRQs)
* Basic device interaction (VGA, keyboard)
* Time and scheduling primitives

---

## Roadmap

### Bootstrapping

* [x] Kernel entry point
* [x] Stack initialization
* [x] Direct VGA text output
* [x] Basic printing utilities

### CPU Control

* [ ] Global Descriptor Table (GDT)
* [ ] Interrupt Descriptor Table (IDT)

### Interrupt System

* [ ] PIC remapping
* [ ] Interrupt Service Routines (ISR)
* [ ] Hardware IRQ handling

### Hardware Interaction

* [ ] Timer (PIT)
* [ ] Keyboard input

### Memory Management

* [ ] Paging (virtual memory)
* [ ] Address space control

### Execution Model

* [ ] Task switching primitives
* [ ] Basic scheduler
* [ ] System call interface

---

## Project Structure

```
boot/        # Boot and low-level entry (assembly)
kernel/      # Core kernel code (C + ASM)
docs/        # Technical documentation (per subsystem)
linker.ld    # Memory layout definition
Makefile     # Build system
```

---

## Build

### Requirements

* `nasm`
* `gcc` (32-bit support or cross-compiler)
* `ld`
* `make`
* `qemu-system-x86_64`

### Build

```bash
make
```

### Run

```bash
make run
```

---

## Execution Model

The kernel is loaded and executed directly without a full bootloader stack (GRUB optional).

At runtime:

1. CPU begins execution at `_start`
2. Stack is initialized manually
3. Control is transferred to `kmain`
4. Kernel interacts directly with hardware (e.g., VGA memory at `0xB8000`)

---

## Documentation

All subsystems are documented under `/docs`.

Each document includes:

* CPU-level behavior
* Memory layout
* Implementation details
* Execution flow
* Known pitfalls

This repository is meant to be both:

* a working kernel
* a long-term technical reference

---

## Notes

* The project targets **x86 (32-bit)** initially
* No external libraries are used
* All behavior is verified through QEMU

---

## Status

Actively developed — focus is currently on **CPU control structures (GDT / IDT)** 
and establishing a reliable interrupt handling foundation.

