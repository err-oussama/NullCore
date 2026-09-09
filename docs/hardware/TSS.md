# Task State Segment (x86 32-bit Protected Mode)

## Definition
The **Task State Segment (TSS)** is a fixed-format structure that the x86 CPU reads directly during privilege-level transitions. Its core job is to tell the CPU which kernel stack to switch to whenever code running in ring 3 triggers an interrupt or syscall into ring 0. The CPU reads `ss0` and `esp0` from the TSS and switchs to that stack automatically, before the kernel ever runs a single instruction. The kernel's only real resposibitiy is keeping `esp0` updated to point to the current task's kernel stack on every task switch. The TSS is registerd through a descriptor in the GDT, and the `ltr` instruction loads its selector into the `Task Register` so the CPU knows where to find it. Everythings else in the TSS struct (general registers, segment, `cr3`, `previous_task_link`) is a leftover from hardware task switching and is unused in modern software-scheduled kernels.

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


## I/O Permission Bitmap (IOPB)

A bitmap stored at the end of the TSS that controls which hardware I/O ports a task is allowed to access directly.
It is an array of bits whre each bit corresponds to one of the 65536 possible I/O ports 
```
0 = allowed  
1 = blocked, CPU raise General Protection Fault
```

## What's Actually Used (Legacy vs Modern)

Out of the entire 104-byte TSS structure, only a small handful of fields are ever touched by a modern software-scheduled kernel. Everything else exists purely because the struct's layout is fixed by the architecture, it has to be defined correctly even though most of it is never read or written.

### Actually Used 

|Field        | Purpose                                                              |
|-------------|----------------------------------------------------------------------|
|`esp0`       |kernel stack pointer; loaded by the CPU on every ring 3 → 0 transition|
|`ss0`        |kernel stack segment; always `0x10`, loaded alongside `esp0`          |
|`iobp_offset`|Set beyond the struct's size to deny all ring 3 I/O port access       |


### The Legacy

Every other field is the TSS, `previous_task_link`, `cr3`, `eip`, `eflags`, all the gerneral purpose registers, the segment selectors, `ldt_selector`, `esp1/ss1`, `esp2`/`ss2`, only matters for hardware task switching, which this kernel does not use. They can be left zeroed; nothing in a software-scheduled kernel ever reads or relies on them.


### What matters?

I only need to set three fields when initializing TSS: `ss0 = 0x10`, `esp0= initial kernel stack`, `iopb_offset = sizeof(tss_t)`. Everything else can be zeroed and forgotten, it has not effect on a software-scheduled, no matter what value it holds.


## GDT descriptor Setup 


The TSS needs an entry in the GDT, just like any other segment, but it's registered as **system descriptor**, not a code/data segment, so it carries a different type field.

For a 32bit TSS, the descriptor's `S` bit is set to `0` (marking it as a system descriptor rather than code/data), and its `type` field is set to `0x9`, meaning `Available 32-bit TSS`. Combined with present bit, this typically gives a type byte of `0x89`.

The descriptor's base field points to the address of the TSS struct in memory, and its limit field covers the size of the struct, `sizeof(tss) - 1`. If an `I/O Permission Bitmap` is appended after the struct, the limit needs to extend far enough to cover that too; if `iobp_offset` is simply set beyond the struct's size to deny all I/O, the limit only needs to cover the base struct.


Once this descriptor is in place at some index in the GDT, its selector, loaded into the **Task Register** using `ltr`.


## Loading the TSS (`ltr`)

After the TSS descriptor exists in the GDT, the CPU needs to be told which one to actually use. This is done once, at boot which the `ltr` **Load Task Register** instruction:
```
ltr ax; ax = TSS selector 
```

The selector works exactly like any other GDT selector, index × 8, with RPL bits as usual. Once loaded, the **Task Register (TR)** holds this selector permanently, and the CPU uses it to locate the active TSS every time a privilege-level transition occurs.

`ltr` is a privileged instruction, in can only be executed at ring 0, and is typically called once during kernel initialization, right after the GDT is setup and before interrupts are enabled.


## Updating ESP0 on task Switch 

Every time the scheduler switches to a different task, `esp0` in the TSS must be updated to point to that task's own kernel stack. The CPU reads `esp0` directly from the TSS whenever a privilege-level transition occurs, it has no other way to know where a task's kernel stack lives.

If `esp0` is left pointing at the previous task's kernel stack after a switch, the next time the new task triggers an interrupt or syscall, the CPU will push its interrupt frame onto the *wrong* stack, silently corrupting whatever the old task had stored there.

So this update belongs in the same place the scheduler already swaps `esp` for user-mode stack, right after picking the next task to run, before resuming it:

**switch to next task**:
1. load next task's user-mode esp (as alreay done).
2. set tss.esp0 = next task's kernel stack pointer.

This is the once piece of ongoing, pre-switch maintenance the TSS requires.
Everything else about the TSS (the GDT descriptor, the `ltr` load) is set up once, at boot, and never touched again.


## Ring 3 → Ring 0 Transition Flow

This is the complete sequence of what the CPU does, automatically, the moment ring 3 code triggers an interrupt or syscall:


1. Ring 3 code is running, `CS` points to the user code segment, `SS:ESP` point to the user stack.
2. An interrupt or `int N` fires. The CPU looks up the corresponding `IDT` entr.
3. The CPU checks the `IDT` entry's `DPL` against the current `CPL`, for software interrupts this determines whetever  ring 3 is even allowed to trigger this vector.
4. Privilege is rising (ring 3 → ring 0), so the CPU reads `esp0` and `ss0` from the **TSS** (found via the `Task Register`).
5. The CPU switches the stack to `SS0:ESP0`, the kernel stack of this task.
6. On this new kernel stack, the CPU pushes the interrupted task's `SS`, `ESP`, `EFLAGS`, `CS`, `EIP`, and for exceptions with an error code, the error code as well.
7. `CS` is loaded with the krnel code segment from the `IDT` entry, `CPL` is now `0`.
8. Excecution jumps to the interrupt handler.
9. The handler runs entirely in ring 0, on the kernel stack found via `esp0`.
10. The handler executes `iret`. The CPU pops `EIP`, `CS`, `EFLAGS`, and since the popped `CS` indicates ring 3, it also pops `SS`, and `ESP`, restoring the original user stack.
11. Execution resumes in ring 3, exactly where it left off, on the original user stack.


The TSS only participates in one step of this entire flow, step 4 and 5 supplying the kernel stack pointer. Everything else is the CPU's normal interrupt machinery, unrelated to the TSS itself.







## Resource

[TSS](https://wiki.osdev.org/Task_State_Segment)
