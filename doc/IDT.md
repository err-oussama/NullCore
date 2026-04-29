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

### Gate Description - Protected Mode (32-bit) 

|*Bytes*|   *Name*          | *Role*                                                                       |   
|-------|-------------------|------------------------------------------------------------------------------|
|0-1    |Offset Low         |Lower 16 bits of the ISR address                                              |
|2-3    |Selector           |Segment selector that indexes into the GDT to find code segment containing ISR|
|4      |Reserved/Zero      |Always set to 0, reserved for future usage, unused                            |
|5      |Type & Attributes  |Gate type, Descriptor privilege(DPL), present bit                             |
|6-7    |Offset High        |Upper 16 bits of the ISR address                                              |


### Interrupt Gate/ IDT entry - Long Mode (64-bit)

|*Bytes*|       *Name*        |                              *Role*                                          |
|-------|---------------------|------------------------------------------------------------------------------|
|0-1    |Low offset           |Lower 16 bits of the ISR address                                              |
|2-3    |Selector             |Segment selector that indexes into the GDT to find code segment containing ISR|
|4      |Interrupt Stack Table|Index into the ISR to select which stack to use when the interrupt fires      |
|5      |Type & Attributes    |Gate Type, Descriptor Privilege Level(DPL), present bit                       |
|6-7    |Middle Offset        |Middle 16 bits of the ISR address (bits 16-31)                                |
|8-11   |High offset          |Upper 32 bits of the ISR address (bits 32-63)                                 |
|12-15  |Reserved/Zero        |Reserved for future use, must be zero                                         |


#### Selector 

|*Bits* |           *Name*            |                         *Role*                          |*Bit Value*|
|-------|-----------------------------|---------------------------------------------------------|-----------|
|0-1    |Request Privilege Level (RPL)|Defines the privilege of the requested segment           | Ring 0-3  |
|2      |Table indecator (TI)         |Indecates which table to lookup the segment descriptor in|0=GDT/1=LDI|
|3-15   |Index                        |Index of the sgment descriptor inside the GDT/LDT        |Index value|


#### Type & Attribute 


|*Bits* |           *Name*            |                         *Role*                                |                     *Bit Value*                       |
|-------|-----------------------------|---------------------------------------------------------------|-------------------------------------------------------|
|0-3    |Type                         |Define the type of gate                                        |0-16                                                   |
|4      |System Bit (S)               |Identifies system structures                                   |0=System Descriptor/1=(code,data) segment              |
|5-6    |Description Privilege Level  |Define the minimum privilege level required to access this gate|Rin 0-3                                                |
|7      |Present                      |Marks whether this descriptor is valid and active              |1=Active/0=Inactive, raise General Protection Fault(GP)|



## Interrupt Descriptor Type 

### Interrupt Gate 
**Interrupt Gate** is the gate type used for **hardware IRQs and CPU exceptions** that must not be interrupted while being handled.

**What makes it special -- one thing**:

When the CPU enters an interrupt gate handler, it **automatically clears the IF (Interrupt Flage) flag in EFLAGS** -- meaning all maskable hardware interrupts are disabled for the duration of your handler. When you `iret`, the CPU restores EFLAGS and IF goes back to what it was.

In one line: Interrupt gate = jump to ISR + *automatically disable interrupts* for the duration, so your handler runs atomically without being interrupted by another IRQ.


### Trap Gate 
**Trap Gate** is identical to interrupt gate in almost every way -- **one single difference**: it does **not clear the IF flag** -- interrupts remain eneabled while your handler runs.

**When do you use it**:
The classic case is **system call `int 0x80`** -- you want user space to trigger the handler, but you also want to timer IRQ to still fire during a long syscall so the scheduler can still preempt. If you used an interrupt gate for syscalls, the entire system would freeze during every syscall.

**In one line**: Trap gate = exactly like interrupt gate but **Interrupt stay enabled** -- used it when your handler is safe to be interrupted


### Call Gate 

**Call Gate** is fundamentally different from interrupt and trap gates -- it is **not itriggered by an interrupt at all**. It is triggered by an explicit `call` or `jmp` instruction from code.

**What it is for**:
It exists purely for **controlled privilege transitions** -- user space (ring 3) calling into kernel space(ring 0) in a deliberate, explicit way, Think of it as a **defined entry point** the kernel exposes to user space.

**In one line**: Call gate = a **GDT-defined entry point** that lets lower privilege code explicitly `call` into higher privilege code -- correct but obsolete, replaced by `sysenter`/`syscall`



### Task Gate 
**Task Gate** is the heaviest gate type -- instead of just jumping to a handler, it triggers a **complete hardware task swith**.


**What it is for**:
When the CPU hits a task gate it does not just save a few registers and jump -- it saves the **entire CPU state** into the current **TSS (Task State Segment)** and loads a complete new one. Every register, segment, stack pointer -- everything.

**In one line**: Task gate = trigger a **full CPU state swap** via TSS -- correct and safe but extremly heavy, only practically useful today for the double fault handler where you need a guaranteed clean stack.


## Interrupt Vectors

The 256 interrupt vectors are splite into two regions:

***0x00-0x1F (0-31)***: hardwired by  Intel. The CPU decides what triggers these, you cannot reassign them. You can only write the handler, not control  what fires it. 

***0x20-0xFF (32-255)***: completely yours. You decide what triggers them and what the handler does.

### 0x00 - 0x1F

|*Vector*|*Name*|*Type*|           *Description*               |
|--------|------|------|---------------------------------------|
|0x00    |#DE   |Fault |Divide Error                           |
|0x01    |#DB   |F/T   |Debug                                  |
|0x02    |  -   |INT   |Non-Maskable Interrupt (NMI)           |
|0x03    |#BP   |Trap  |Break Point                            |
|0x04    |#OF   |Trap  |Overflow                               |
|0x05    |#BP   |Fault |Bound Range Exceeded                   |
|0x06    |#UP   |Fault |Invalid Opcode                         |
|0x07    |#NM   |Fault |Device Not Available (FPU)             |
|0x08    |#DF   |Abort |Double Fault                           |
|0x09    |  -   |Fault |Coprocessor Segment Overrun (Obsolete) |
|0x0A    |#TS   |Fault |Invalid TSS                            |
|0x0B    |#NP   |Fault |Segment Not Present                    |
|0x0C    |#SS   |Fault |Stack Segment Fault                    |
|0x0D    |#GP   |Fault |General Protection Fault               |
|0x0E    |#PF   |Fault |Page Fault                             |
|0x0F    |  -   |  -   |Reserved                               |
|0x10    |#MF   |Fault |x87 Floating Point Exception           |
|0x11    |#AC   |Fault |Alignment check                        |
|0x12    |#MC   |Abort |Machine Check                          |
|0x13    |#XM   |Fault |SIMD Floating Point Exception          |
|0x14    |#VE   |Fault |Virtualization Exception               |
|0x15    |#CP   |Fault |Control Protection Exception           |
|0x16    |  -   |  -   |Reserved                               |
|0x17    |  -   |  -   |Reserved                               |
|0x18    |  -   |  -   |Reserved                               |
|0x19    |  -   |  -   |Reserved                               |
|0x1A    |  -   |  -   |Reserved                               |
|0x1B    |  -   |  -   |Reserved                               |
|0x1C    |#HV   |INT   |Hypervisor Injection Exception         |
|0x1D    |#VC   |Fault |VMM Communication Exception            |
|0x1E    |#SX   |Fault |Security Exception                     |
|0x1F    |  -   |  -   |Reserved                               |


**Fault**:
-   Reported **before** the instruction that caused it 
-   EIP points to the **faulting instruction** -- `iret` re-executes it 
-   Recoverable -- the handler can fix the cause and retry 



**Trap**:
-   Reported **after** the instruction that caused it 
-   EIP points to the **next instrution**
-   Recoverable -- execution continues normally after handler 


**INT**:
-   Not caused by an instruction fault at all -- comes from **outside** the normal execution flow 
-   NMI comes from hardware completely bypassing the IF flag 
-   Hypervisor injection comes from the VMM injecting an event into the guest
-   No faulting instruction to point to -- EIP just points to whatever was executing



**Abort**:

-   Server unrecoverable error 
-   EIP is **unreliable** -- you cannot trust saved state 
-   Not recoverable -- system is considered dead, no meaningful `iret`



## Interrupt Request (IRQ)

**Interrupt Request (IRQ)** is a signal sent by an external hardware device to the CPU to notify it that something happend and needs attention. The device cannot wait -- it interrupts whatever the CPU is currently doing to get serviced.


The signal goes through the **PIC (Programmable Interrupt Controller)** which prioritizes and forwards it to the CPU as an interrupt vector number. The CPU then pauses execution, jumps to the registered handler, services the device, and resumes where it left off.

```
Examples:
    keyboard    ->  you pressed a key       ->      IRQ1
    timer       ->  another tick elapsed    ->      IRQ0
    hard disk   ->  data finished loading   ->      IRQ14 
    mouse       ->  cursor  moved           ->      IRQ12
```
The key property that defines an IRQ -- it is **Asynchronous**. It has nothing to do with what code is currently running. It can fire at any point, in the middle of any instruction, completely independent of your program's logic.


