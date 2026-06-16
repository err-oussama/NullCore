# Segment 

## Definition
A variable-size region of memory defined by ***Base Address*** (where it starts) and ***Limit*** (how large it is).

Unlike pages which are fixed at 4KB, Segment can be any size. The CPU uses segment descriptors stored in the *GDT* or *LDT* to locate, access, and protect these regions.

***Segmentation*** is the ***predecessor of paging*** -- it was the original mechanism for memory management and protection before paging took-over in modern OS.


## Types

### Codee Segment 

A segment containing executale instructions, the CPU fetches and runs code from it. It can optionally be readable but never writable.


### Data Segment 

A segment containing readable and writable data. Cannot be executed. Includes the stack segment which grows downward.

### System Segment 

Unlike code and data segments which describe regions of memory, a system segment describes a special CPU control structure rather than a memory region. It is identified by the descriptor Type bit(bit 4) in the Access Byte being set to 0, and the gate type field defininf which structure it represents:

- *Task State Segment (TSS)*: Holds the CPU state and stack pointers for each privilege level. Required by the CPU for privilege switching and in 64-bit mode for the Interrupt Stack Table (IST).

- *Local Descriptor Table (LDT)*: A descriptor that point to a Local Descriptor Table, allowing a process to have its own private set of segment descriptors.

- *Call Gate*: A controlled and protected entry point that allows code running at a lower privilege level (rint 3) to safely call code at a higher privilege level (ring 0) without being able to jump to arbitrary kernel addresses.

- *Interrupt Gate*: A gate used in the LDT that transfers execution to an ISR and automatically disable interrupts while the handler runs.


- *Trap Gate*: A gate used in the LDT that transfers to an ISR but keeps interrupts enable, typically used for exceptions and system calls.


