# NullCore

A minimal x86 32-bit kernel built from scratch for educational purposes.

---

## Roadmap

### Bootstrapping
- [x] Kernel entry point
- [x] Stack initialization
- [x] Direct VGA text output
- [x] Basic printing utilities

### CPU Control
- [x] Global Descriptor Table (GDT)
- [x] Interrupt Descriptor Table (IDT)
- [x] ISR stubs (all 32 CPU exceptions)

### Interrupt System
- [x] PIC remapping
- [x] Hardware IRQ handling
- [x] Keyboard input

### Memory Management
- [x] Physical memory map (parse multiboot)
- [x] Physical frame allocator
- [x] Paging (virtual memory)
- [x] Kernel heap (kmalloc/kfree)

### Execution Model
- [x] Timer (PIT)
- [x] Task switching primitives
- [x] Basic scheduler
- [x] System call interface

### User Space
- [x] Address space control
- [x] TSS (ring 3 → ring 0 stack switch)
- [x] ELF loader
- [x] First user process

### Filesystem
- [ ] ATA/IDE disk driver
- [ ] Filesystem implementation
- [ ] sys_read, sys_write (file-backed)

### Shell
- [ ] fork / exec
- [ ] Basic shell (load and run programs from disk)

### Networking
- [ ] PCI enumeration
- [ ] NIC driver
- [ ] TCP/IP stack

---

## Project Structure


```
boot/        # Boot and low-level entry (assembly)
kernel/      # Core kernel code (C + ASM)
user/        # User space programs
docs/        # Technical documentation (per subsystem)
linker.ld    # Memory layout definition
Makefile     # Build system
```

---

## Build

**Requirements:** `nasm`, `gcc` (32-bit), `ld`, `make`, `qemu-system-x86_64`

```
make      # build
make run  # run in QEMU
```

---

## Notes
* Targets x86 32-bit protected mode
* No external libraries
* Tested under QEMU
