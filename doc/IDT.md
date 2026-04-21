# Interrupt Description Table


## Overview

**What it is**: An array of 256 fixes-size entries, indexed by interrupt vector number (0-255).

**What each entry holds**:

- The memory address of an **ISR**(Interrupt Service Routine) -- the handler to execute

- On (32/64)-bit modes architectures: **Privilege level** + **Segment selector** metadata.

**What id does**: Maps every interrupt number -> its corresponding handler routine.

**When an interrupt fires**, the CPU does exactly this:

```
                 interrupt_number
                        │
                        ▼
              IDT[interrupt_number]     <- index into the array 
                        │
                        ▼
              fetch handler address     <- read the entry 
                        │
                        ▼
                   jump to ISR          <- execute the routine 

```
**In one sentence**: The IDT is an 256-entry lookup table the CPU uses to find and jump to the right piece of code whenever an interrupt is triggered.



## Definition

The **Interrupt Description Table (IDT)** -- also called the *Interrupt Vector Table (IVT)* -- is a 256-entry array maintained jointly by the CPU and the OS. Each entry is  indexed by an  *Interrupt Vector Number* (0-255) and holds the memroy address of an ***Interrupt Service Routine (ISR)***, which is the specific handler the CPU must jump to when that interrupt fires. On (32/64)-bit mode architecture, each entry also stores privilege level and segment selector metadata alongside the address. When an interrupt is triggered, the CPU uses its number as an index into this table, fetches the corresponding entry, and immediately transfers execution to the mapped ISR. In essence the IDT is a fixed-size lookup table that gives the processor a direct, unambiguous answer to the question:
*Interrupt N just fired -- where do I go?*


