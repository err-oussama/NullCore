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


## Key Concepts

**Interrupt Vector**: A small integer(0-255 on X86) that uniquely identifies a specific interrupt source.


**Entry/Descriptor**: Each slot in the table typically stores a pointer to an *ISR*, plus control bits(gate type, privilege level, segment selector on X86).


**Triggering**: When an interrupt fires (hardware signal, software *INT* instrucion or CPU exception), the processor uses the vector number as an index into the table, loads the handler, and transfers execution to it.


**Interrupt Service Routine(ISR)**: A function executed by the CPU in response to an interrupt. It handles the event (reading a keystroke, acknowledging a timer) and then returns control back to whatever was running before.

## Architecture Examples

|   *Architecture*      |               *Name*            |         *Location*           |
|-----------------------|---------------------------------|------------------------------|
|X86 Real Mode          |Interrupt Vector Table (IVT)     |Fixed at physical address 0x00|
|X86 Protected/Long Mode|Interrupt Description Table (IDT)|Pointed by IDTR register      |
|ARM                    |Exception Vector Table           |Fixed or pointed by VBAR      |
|RISC-V                 |Trap Vector Table                |Pointed by mtvee CSR          |


## X86 Interrupt Tables 


|       *Mode*      |           *Table Name*         |*Element Size*| *Element Name*           | 
|-------------------|--------------------------------|--------------|--------------------------|
|Real (16bit)       |Interrupt Vector Table (IVT)    |4 Bytes       |Interrupt Vector          |
|Protected (32-bit) |Interrupt Descriptor Table (IDT)|8 Bytes       |Gate Description          |
|Long (64)          |Interrupt Descriptor Table (IDT)|16 Bytes      |Interrupt Gate / IDT Entry|


## Interrupt Description Table 


### Interrupt Vector - Real Mode (16-bit)

|*Byte* |*Name* |                   *Role*                              |
|-------|-------|-------------------------------------------------------|
|0-1    |offset |Pointer to the ISR location whithin the segment        |
|2-3    |segment|Base address of the memory segment containing the ISR  |

***Physical address = segment * 16 + offset***





