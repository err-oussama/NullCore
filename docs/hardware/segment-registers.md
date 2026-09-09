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


The lowest  bits of a selector, set once when the selector constant is build, not recalculated dynamically. RPL only matters at the moment a selector is loaded into a segment register (`mov ds, ax`, far jump, far call); once loaded, it plays no further role in subsequent uses of that register.



**The OSDev convention**: always set `RPL` to match the privilege level the selector is meant for.

```
kernel CS → RPL = 0
user CS → RPL = 3
```

This makes `RPL` always equeal CPL in practice, so the load-time check collapses:

`max(CPL, RPL) <= DPL     →     max(CPL, CPL) <= PDL    →   CPL <= DPL`

**What `RPL` is actually for**: privilege code can set RPL higher than its onw CPL to make a check stricter than `CPL <= DPL`, simulating "would a less-privilege caller be allowed to use this selector?" This matters in segmentation-based designs, where different selectors point to genuinely different memory regions.

**In a flat, paging-protected kernel** like NullCore and most of moder kernels, this scenario never comes up, every selector points to the same flat address space, and the real security boundary is the page table's `User/Supervisor` bit, not the selector used.




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

Every instruction that touches memory triggers at least one privilege check, sometimes tow or three, depending on how many distinct memory operands it has.
Each check is independent and uses whichever segment register actually applies to that specific access.


### Step I: Fetch Check (always happens)

Before the CPU can even fetch an instruction's bytes, it check:
`CPL <= DPL of the segment CS curretnly points to`

This check is, in practice, always satisified once `CS` is loaded, it's an automatic consistency guarantee rather then something that can fial mid-stream, since `CPL` is set equal to that segment's `DPL` the moment `CS` was last loaded.



### Step II: Data/Stack Access check (only if the instruction touches memory)


For every memory operand the instruction has, the CPU checks:

`CPL <= DPL of the segment register that operand routes through`.

Which segment register applies depends on the operand, not the instruction:

```
- push eax/ pop eax     → Uses SS (always, for any stack operation)
- mor eax, [ebp + 4]    → Uses SS
- mov eax, [ebx]        → Uses DS (default)
- mov eax, [gs:0xF10]   → Uses GD (explicit segment prefix overrides DS)
```

### Worked Example: `pop [eax]`


This instruction has two separate memory operands, so three total check occur:

1. Fetch check              : `CPL <= DPL(CS)`
2. Stack read(the pop)      : `CPL <= DPL(SS)`
3. Memory write(to \[eax\]) : `CPL <= DPL(DS)`



### Why No RPL Term Appears Here.

The `max(CPL, RPL) <= DPL` formula only applies at the moment a segment register is **loaded** with a new selector. Once loaded, ordinary use of that register, any later instruction reading or writing through it only checks `CPL <= DPL` with no RPL involved at all.

## Why Segmentation Still Matters Under Paging

Paging and segmentation solve two completely different problems, and one did not replace the other, paging only took over the job segmentation used to do for **addressing**.



**Paging** answers: 

- *Where is this byte of memory, physically*?
- *Is this PAGE accessible from ring 3*?


**Segmentation** answers: 
- *What ring am I currently running at*?
- *Am I allowed to fetch/execute this code*?
- *Am I allowed to touch this data, through this specific segment, right now*?




### What Paging Took Over 


In a flat memory model , base=0, limit=max on every segment, segmentation no longer contributes anything to address translation. `CS.base + EIP` is just `EIP`.
The segment registers stop describing distanct memory regions entirely.



### What Paging Never Did 

Paging's only privilege-related feature is a single bit per page: The `User/Supervisor` bit, which says *Can ring 3 touch this page at all*. That's coarse and binary, it says nothing about: 

- What ring is curretnly executing right now (`CPL`).
- Whetever a specific instruction is even allowed to be fetched.
- Where the correct kernel stack lives when crossing from ring 3 to ring 0.
- Whether a far jump/call/interrupt is permitted to chenge privilege level.

None of this was ever paging's job, and none of it was reassigned elsewhere, it still belongs entirely to segmentation.


### The two systems run in parallel

Evey single memory access goes through **both** checks, independently, for different reasons:


1. Segmentation check: is this access allowed given the current privilege level and the segment it routes through?

2. Paging check: Where physically is this address, and does the page itself permit this ring  to touch it?




















