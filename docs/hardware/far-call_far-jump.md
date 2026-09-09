# Far Call - Far jump 


## What Is a Far Jump/Call

A **Far jump/call** is a `jmp`/`call` instruction that changes both the code segment (`CS`) and the instruction pointer (`EIP`), unlike a normal (near) jump/call  which only changes `EIP` within the current segment.
The target is specified as `selector:offset` - the selector is looked up in the GDT, and the CPU's behavior depands entirely on what type of descriptor that selector resolves to: 
- a normal code segment (ordinary far jump).
- a TSS or task gate (triggers a hareware task switch).
- a call gate(a controlled, privilege-checked call to fixed kernel entry point).

```
call 0x8:0x415653525
jmp 0x8:0x415653525
```

## Valid Targets for Far Jump/Call

|*Descriptor Type*  |*Far Jump (`JMP`)* |*Far Call (`CALL`)*|
|-------------------|-------------------|-------------------|
|Code Segment       |Yes                |Yes                |
|Task Gate          |Yes                |Yes                |
|Available TSS      |Yes                |Yes                |
|Busy TSS           |Yes                |No (`#GP`)         |
|Call Gate          |No (`#GP`)         |Yes                |
|Data/LDT/Int Gates |No (`#GP`)         |No (`#GP`)         |





## Standard Code Segment Transfer

### What Changes 

- `CS`  : Loaded with the target selector.
- `EIP` : Loaded with the offset specified in the instruction.

### Far Call 

It pushes the old `CS:EIP` onto the stack as a return path



## Call Gate: Controlled Privilege Change

### What Changes 

- `CS`  :   Loaded from the **call gate descriptor itself**, not the instruction's selector field directly.

- `EIP` :   Loaded from the **call gate descriptor itself**, **instruction's offset is ignored**


- **If Privilege raise (Ring3 → Ring 0)**:

    - `ESP` : Switched to the new privilege level's stack, read from the TSS (`esp0`)

    - `SS`  : Switched to the new privilege level's stack segment, read from the TSS (`ss0`)




### Far Call 

It pushes the old `CS:EIP` and `SS:ESP` onto the **new stack**, so `ret far` can revers the privilege change later.
    

## Task Gate & TSS: Full Hardware Task Switch
 
### What Changes 
Everything, this is a **Full hardware task switch**, not a lightweight transfer:


- `CS`, `EIP`
- `SS`, `ESP`
- `EAX`, `EBX`, `ECX`, `EDX`, `EDI`, `ESI`, `EBP`
- `EFLAGS`
- `CR3`
- `DS`, `ES`, `FS`, `GS` 
- `LDT`

All the CPU State loaded from the targeted TSS.

Both **Far Jump** and **Far Call** perform an identical full state save into the current task's TSS before loading the new state from the target task's TSS.
The only difference is that **Far Call** additionally writes the calling task's selector into the `previous_task_link` field of the target TSS and sets the `NT` flags in the new task's `EFLAGS`, recording a breadcrumb so a later `iret` can switch back. **Far Jump** does neither, makeing it a clean, one-way switch with no path back.


## Can Far Jump/Call Escalate Privilege?

The answer depends entirely on what the target selector resolves to in the GDT, not the instruction itself.


### Normal Code Segment: Cannot Escalate 

`max(CPL, RPL) <= DPL`

This check can only ever stay the same or de-escalate (jump to a segment with **higher** DPL number, less privileged). It can never succeed if the target's `DPL` is numerically lower then the current `CPL`, so a plain far jump/call to an ordinary code segment can never raise privilege, no matter what selector values are constructed.


### Call Gate: Can Escalate 

A Call gate has it own `DPL`, checked against the **caller's** `CPL`/`RPL`, but the gate's **contents** point to a fixed, kernel-defined ring 0 entry point. This lets ring 3 code legitimately call into ring 0, with the CPU automatically switching `SS:ESP` to the kernel stack (via the TSS) as part of the transition.



### Task Gate & TSS: Can Escalate

A full hardware task switch loads `CS` (and everything else) directly from whatever was saved in the target TSS, with no `max(CPL, RPL) <= DPL` ceiling at all.
The new privilege level is simply whatever the target TSS says it is, including a higher one then caller's current `CPL`.


