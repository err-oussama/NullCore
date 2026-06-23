# Segment Registers: Roles and Privilege Checking

## Overview

Six segment registers exist on x86, each holding a selector that indexes into the `GDT`.
In a flat memory model they no longer contribute to addresss translation, paging handles that, but they remain fully active for privilege checking on every instruction.


- `CS`: ***Code Segment***  → defines the current privilege level (CPL) and gates instruction fetch.
- `SS`: ***Stack Segment*** → gates stack operations (push/pop, call/ret, interrupt frames).
- `DS`: ***Data Segment***  → default segment for general memory access.
- `ES`: ***Extra Segment*** → alternate data segment, used when explicity selected.
- `FS`: ***F Segment***     → alternate data segment, commonly used for thread-local/per-CPU data.
- `GS`: ***G Segment***     → alternate data segment, commonly used for pre-CPU data in modern kernels.




## Functional Groups: CS, SS and Data Segments


### Code Segment (`CS`):  **Governs FETCH privilege**

- *Am I allowed to even read/execute this insruction?*
- Always checked, on every single instruction, no exceptions.
- Defines the current privilege level (CPL) for everything else on this list.

### Stack Segment (`SS`): **Governs STACK ACCESS privilege**

- *Am I allowed to read/write the stack?*
- Checked whenever an instruction touchs the stack: `push`/`pop`, `call`/`ret`, `pusha`/`popa`, `int`/`iret`,or any explicit `[esp + NUMBER]`,  `[ebp + NUMBER]` memory operand.
- `esp`/`ebp` used as a base register defaults to `SS`, never `DS`, even outside of `push`/`pop`.



### Data Segments (`DS`, `ES`, `FS`, `GS`): **Governs GENERAL DATA ACCESS privilege**

- *Am I allowed to read/write this memory?*
- Checked whenever an instruction touches memory that is Not a Stack operation.
- `DS` is the default for general memory access; `ES`/`FS`/`GS` only apply when explicity selected via a segment prifix (e.g, `mov eax [gs:0x10]`), and replace `DS` for that one access.
- Only one segment register applies per memory operand, they are never combined for a single access.

## CPL, RPL and DPL

Three privilege values participate in every segment-related check, and it's easy to confuse them since all three are just 2-bit numbers (0-3) representing a `ring`



### CPL (Current Privilege Level)

The privilege level that CPU is currently executing at.
There is only one CPL at any moment, and it always comes from the lowest 2 bits of the **current `CS` selector**, there is no separate CPL register.

### RPL (Requested Privilege Level)

The lowest 2 bits of a selector that is **about to loaded** into a segment register.
RPL only matters at that moment a selector is loaded (`mov ds, ax`, far jump, far call), once loaded, RPL plays no further role in subsequent uses of that register.
It exists so privileged code can deliberately request a selector be treated as if it came from a lower privilege level, without actually being at that level. 



### DPL (Descriptor Privilege Level)

The privilege level baked into a GDT descriptor itself, set once when the descriptor is created.
This is the value every check is ultimately compared against, it answers *What privilege level is required to use this segment/gate/TSS at all?*.


### How they Combine 

```
Loading a selector into a segment register:
    max(CPL, RPL) <= DPL    → allowed, otherwise #GP

Using an already-loaded segment register (no RPL involved):
    CPL <= DPL of whatever that register curretnly holds
```


## How Privilege Checking works Per Access 


## Why Segmentation Still Matters Under Paging









