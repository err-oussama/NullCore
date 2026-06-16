# Task Switch


## Overview

**Task switch** is the mechanism that allows multiple tasks to share a single CPU.
The kerenl saves the state of the currently running task, then restores the state of the next task. Because the CPU has no knowledge of "tasks", all of this is done manually using the stack.

This document covers how the `CPU state` is captured using the stack, how `pusha`/`popa` and `iret`



## What is CPU State?

At any moment, a `task's full execution state` is its `register value`:

|   Category        |               Registers               |
|-------------------|---------------------------------------|
|General purpose    |eax, ebx, ecx, edx, esi, edi, ebp, esp |
|Instruction pointer|eip                                    |
|Flags              |eflas                                  |
|Segments           |cs, ds, es, fs, gs, ss                 |

To suspend a task and resume it later, all of these must be saved and restored exactly.



## What The CPU Pushes Automatically on Interrupt

When an interrupt fires (such as the PIT timer), the CPU automatically **pushes three things onto current task's stack** before jumping to the ISR:

```
; Stack after CPU interrupt entry (top = higher address)

[ESP + 8]   EFLAGS  ←   flags register at the time of interrupt 
[ESP + 4]   CS      ←   code segment at the time of interrupt 
[ESP + 0]   EIP     ←   where the interrupted task will resume 
```

This is called **iret frame**. The `iret` instruction at the end of an `ISR` pops these three values back and resumes execution exactly where it was interrupted.


## Saving the Rest with `pusha`

The CPU only saves `eip`, `cs`, and `eflags` automatically. The ISR stub must save the general purpose registers manually. in 32-bit mode this is done with single instruction:


```
isr_stub:
    pusha               ; pushes eax, ecx, edx, ebx, esp, ebp, esi, edi
    ; ...
    call timer_handler
    ; ...
    popa                ; restores all of the above
    iret                ; restores eip, cs, eflags → task resumes
```


`pusha` pushes 8 registers in a defined order, After `pusha`, the full stack layout looks like:



```
[ESP + 40]      EFLAGS      ─┐
[ESP + 36]      CS           |  pushed by CPU on interrupt (evey interrupt)
[ESP + 32]      EIP         ─┘
[ESP + 28]      EAX         ─┐
[ESP + 24]      ECX          |
[ESP + 20]      EDX          | 
[ESP + 16]      EBX          |  pushed by `pusha`
[ESP + 12]      ESP*         |
[ESP + 8]       EBP          |
[ESP + 4]       ESI          |
[ESP + 0]       EDI         ─┘
```

Note: The `esp` value pushed by `pusha` is a snapshot of `esp` before `pusha` run, so it is not useful for restoring — but all other registers are accurate.


At this point the **entire task state is on the stack**. This is the key insight that makes stack-based context switching work.




## Context Switching Using the stack 

Because the full CPU state is on the stack, saving a task's context is as simple as saving its `esp`.

When the task is resumed, loading its `esp` back and running `popa` + `iret` restores everything.


### Full Flow 

PIT interrupt fires 
    └─  CPU pushes `eflgs`, `cs`, `eip` onto task A's(current task) stack 
    └─  ISR stub calls `pusha` → all GPRs  now on task A's stack 
    └─  scheduler decides to switch  
    └─  switch_context saves task A's `esp` into its TCB
    └─  switch_context loads task B's `esp` from its TCB 
    └─  `popa` restores task B's GPRs from its stack 
    └─  `iret` jumps to task B's saved eip 
    └─  task B continues as if nothing happend

