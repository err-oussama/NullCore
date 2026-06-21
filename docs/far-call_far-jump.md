# Far Call - Far jump 

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







## Far Control Transfers: Descriptor-Dependent Behavior


### Standard Code Segment Transfer


### Call Gate: Controlled Privilege Change


### Task Gate: Indirect Task Switching


### TSS: Direct Hardware Task Switch



