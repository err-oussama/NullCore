# Segment Registers

- `CS`: ***Code Segment***
- `SS`: ***Stack Segment***
- `DS`: ***Data Segment***
- `ES`: ***Extra Segment***
- `FS`: ***F Segment***
- `GS`: ***G Segment***






These registers i categorize them into three functional groups:


1. `CS` →   **Governs FETCH privilege**
            *Am I allowed to even read/execute this insruction?*
            Always checked, every single instruction, no exceptions.

2. `SS` →   **Governs STACK ACCESS privilege**
            *Am I allowed to read/write the stack?*
            Checked whenever an instruction touchs the stack:
            push/pop, call/ret, pusha/popa, int/iret, 
            or any explicit \[esp + NUMBER\],  \[ebp + NUMBER\] memory operand.


3. `DS`, `ES`, `FS`, `GS`   ->  **Governs GENERAL DATA ACCESS privilege**
            *Am I allowed to read/write this memory?*
            Checked whenever an instruction touches memory that is Not a Stack operation - DS by default, or ES/FS/GS if explicity selected via a segment prifix.






