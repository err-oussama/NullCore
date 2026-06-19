# Task State Segment (x86 32-bit Protected Mode)

## Definition
The **Task State Segment (TSS)** is a fixed-format structure that the x86 CPU reads directly during privilege-level transitions. Its core job is to tell the CPU which kernel stack to switch to whenever code running in ring 3 triggers an interrupt or syscall into ring 0. The CPU reads `ss0` and `esp0` from the TSS and switchs to that stack automatically, before the kernel ever runs a single instruction. The kernel's only real resposibitiy is keeping `esp0` updated to point to the current task's kernel stack on every task switch. The TSS ir registerd through a descriptor in the GDT, and the `ltr` instruction loads its selector into the `Task Register` so the CPU knows where to find it. Everythings else in the TSS struct (general registers, segment, `cr3`, `previous_task_link`) is a leftover from hardware task switching and is unused in modern software-scheduled kernels.



## Why it exist 

When the CPU transitions from a lower privilege level to a higher one (ring 3 → ring 0) on an interrupt or syscall, it needs a **safe kernel stack** to push the interrupted task's state onto. It cannot use the stack because user space controls it — a malicious program could point `esp` anywhere and trick the kernel into writing sensitive data there.


The TSS solves this by giving the CPU a **fixed, trusted location** to look up the kernel stack pointer. Before jumping to the interrupt handler, the CPU reads `esp0` and `ss0` from the TSS and switchs to that stack automatically — no software involvement needed.






## TSS Struct Layout

|Byte   |       Name        |                       Role                            |
|-------|-------------------|-------------------------------------------------------|
|0-1    |Previous Task Link |Selector of the previous TSS, used for task chaining   |
|2-3    |Reserved           |-                                                      |
|4-7    |ESP0               |Stack Pointer for Ring 0                               |
|8-11   |SS0                |Stack Segment for Ring 0                               |
|12-15  |ESP1               |Stack Pointer for Ring 1                               |
|16-19  |SS1                |Stack Segment for Ring 1                               |
|20-23  |ESP2               |Stack Segment for Ring 2                               |
|24-27  |SS2                |Stack Segment for Ring 2                               |
|28-31  |CR3                |Page Directory Base Address                            |
|32-35  |EIP                |Instruction Pointer                                    |
|36-39  |EFLAGS             |CPU flags                                              |
|40-43  |EAX                |General Purpose Register                               |
|44-47  |ECX                |General Purpose Register                               |
|48-51  |EDX                |General Purpose Register                               |
|52-55  |EBX                |General Purpose Register                               |
|56-59  |ESP                |General Purpose Register                               |
|60-63  |EBP                |General Purpose Register                               |
|64-67  |ESI                |General Purpose Register                               |
|68-71  |EDI                |General Purpose Register                               |
|72-75  |ES                 |Extra Segment selector                                 |
|76-79  |CS                 |Code Segment selector                                  |
|80-83  |SS                 |Stack Segment selector                                 |
|84-87  |DS                 |Data Segment selector                                  |
|88-91  |FS                 |F Segment selector                                     |
|92-95  |GS                 |G Segment selector                                     |
|96-99  |LDT Selector       |Selector pointing to the LDT for this task             |
|102-103|IOPB               |Offset of the I/O Permission Bitmap                    |
|104+   |IOPB               |I/O Permission Bitmap                                  |


## What's Actually Used (Legacy vs Modern)

## GDT Descriptor Setup

## Loading the TSS (`ltr`)

## Updating ESP0 on task Switch 

## I/O Permission Bitmap (IOPB)

A bitmap stored at the end of the TSS that controls which hardware I/O ports a task is allowed to access directly.
It is an array of bits whre each bit corresponds to one of the 65536 possible I/O ports 
```
0 = allowed  
1 = blocked, CPU raise General Protection Fault
```

## Ring 3 → Ring 0 Transition Flow


## Resource

[TSS](https://wiki.osdev.org/Task_State_Segment)
