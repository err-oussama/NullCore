# Task State Segment 

## Definition

the **Task State Segment (TSS)** is an architectural memory structred defined by the *x86 Instruction Set Architecture (ISA)* that encapsulates the complete execution context of a task. It serves as the hardware-backed state container for processor registers, memory management state, and privilege-level transition parameters.



## Why Does it exist 

When the CPU transitions from a lower privilege level to a higher one (ring 3 → ring 0) on an interrupt or syscall, it needs a **safe kernel stack** to push the interrupted task's state onto. It cannot use the stack because user space controls it — a malicious program could point `esp` anywhere and trick the kernel into writing sensitive data there.


The TSS solves this by giving the CPU a **fixed, trusted location** to look up the kernel stack pointer. Before jumping to the interrupt handler, the CPU reads `esp0` and `ss0` from the TSS and switchs to that stack automatically — no software involvement needed.



## Resource 


[TSS](https://wiki.osdev.org/Task_State_Segment)
