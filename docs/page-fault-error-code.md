# Page Fault Error Code 

The Page Fault error code is a 32-bit value that the CPU automatically pushes onto the kernel stack immediately before jumping to the page fault handler.
It provides crucial diagnostic data explaining exactly what triggered the fault.


While it is a full 32 bit value, only the lowest 5 bits are used in 32-bit protected mode (the remaining upper bits are reserved by the architecture and set to 0)





## Error-code bit layout

### BIT 0: P (Present)

- **0 (Clear)**: The page is **not present** in physical memory (the page table entry's present bit is 0).

- **1 (Set)**: The page **is present**, but the fault was caused by a **page-level protection violation** (e.g., privilege mismatch or writing to read-only memory).

### BIT 1: W/R (Write / Read)

- **0 (Clear)**: The memory access that triggered the fault was a **read** operation.
- **1 (Set)**: The memory access that triggered the fault was a **write** operation.


### BIT 2: U/S (User / Supervisor)

- **0 (Clear)**: The execution context was in **Supervisor mode (Ring 0, 1, 2)** when the fault occurred.
- **1 (Set)**: The execution context was in **User mode (Ring 3)** when the fault occurred.


### BIT 3: RSVD (Reserved Bits)

- **0 (Clear)**: No reserved bits wre violated.
- **1 (Set)**: The fault occurred because a page directory or page table entry had one of its **architecture-reserved bits set to 1**. This usually indicates a structural bug in how kernel is formatting its page directory or page table entry structures.


### BIT 4: I/D (Instruction / Data)

- **0 (Clear)**: The fault occurred during a standard **Data access** (reading or writing a memory address/variables)
- **1 (Set)**: The fault occurred during an **Instruction Fetch**. The CPU was explicitly attempting to execute code located on that memory page.


