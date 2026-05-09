# Virtual Memory  

Virtual memory is an abstraction layer the CPU provides between the addresses a program uses and the actual physical RAM addresses. When a program accesses address `0x00F1000` it is not directly touching RAM at that address -- The CPU transparently translates that virtual address to completely different physical address through the page tables before the actual memory access happens.




## Memory Management Unit (MMU)

The **Memory Management Unit (MMU)** is a hardware component inside the CPU that sits between the CPU core and the RAM bus. Every single memory access — instruction fetch, data read, data write — passes through the MMU before reaching physical RAM.

The **MMU job is one thing**:

`Virtual address    →   MMU →   physical address`

It does this translation automatically on every access using the page tables set up in RAM. The CPU core never sees physical addresses — it only works with virtual addresses and the *MMU* handles everything else siltently.



## Page Tables 

A page table is a simple array of 1024 entries that sits in a single 4KB page frame in physical RAM. Each entry in the array maps one virtual page to one physical frame. Since each entry covers 4KB and there are 1024 entries, one page table covers exactly 4MB of virtual address space.

A page table is just a normal array sitting in RAM — nothing special about the memory it occupies. What makes it special is that the MMU knows how to read it.

## Page Table Entry (PTE) in 32-bit

A **PTE** is a 32-bit value split into two parts — the physical frame address and the flags

|*Bits* | *Name* | *Role* |*0*|*1*|
|-------|--------|--------|---|---|
|0      |Present||||
|1      |Read/Write||||
|2      |User/Supervisor||||
|3      |Write through||||
|4      |Cache Disable ||||
|5      |Accessed||||
|6      |Dirty||||
|7      |||||
|8      |Global||||
|9-11|  |AVL            |||
|12-31  |frame address  ||||








