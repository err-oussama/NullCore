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

## Page Table Entry (PTE) 32-bit

A **PTE** is a single 32-bit value inside a page table that maps one virtual page to one physical frame. It holds the **Physical Frame Address in its upper 20 bits** and the **Flags in its lower 12 bits**.
There are 1024 PTEs in one page table, each covering 4KB — so one page table covers 4 MB total.


## Page Directory Entry (PDE) 32-bit:

A **PDE** is a single 32-bit value inside the page directory that **points to a page table**. It has the same bit structure as a PTE but bit 7 means page size not PAT. There are 1024 PDEs in the page directory, each pointing to a page table that covers 4MB — so the full page directory covers 4GB total. 


## Entry bit structure


|*Bits* |*Name* |                           *Role*                              |                   *1*             |               *0*             |
|-------|-------|---------------------------------------------------------------|-----------------------------------|-------------------------------|
|0      |P      |Is VP mapped to PP                                             |Mapped                             |Not Mapped                     |
|1      |R/W    |Enable/Disable write in page                                   |Read&Write                         |Read-only                      |
|2      |U/S    |Determine Access Privilege                                     |User Mode                          |Kernel Mode                    |
|3      |PWT    |Determine Cache Policy                                         |Through                            |Back                           |
|4      |PCD    |Enable/Disable cache for page                                  |Disable                            |Enabled                        |
|5      |A      |Determine if the page accessed                                 |Accessed                           |Not Accessed                   |
|6      |D      |Determine if the page written in it                            |Modified                           |Not Modified                   |
|7      |PAT-PS |PAT:extends cache control / PS: page size (PDE)                |PTE:use PAT entry / PDE:4MB page   |PTE: ignore PAT / PDE: 4KB page|
|8      |G      |Is the Page Address will evicted from TLB upon content switch  |Remian                             |Evicted                        |
|9-11|  |AVL    |Ignored by CPU, free for kernel to use however it wants        |
|12-31  |FA     |Physical Frame Address                                         |


## Virtual Address 

```
22-31   →   10 bits — Page Directory index 
12-21   →   10 bits — Page Table index 
00-11   →   12 bits — offset 
```

### Page Directory index (31-22)
This is the index into the page directory. The MMU take these 10 bits and uses them to pick which PDE to read from the page directory. Since it is 10 bits it can index 1024 entries — exactly the size of the page directory.

```
tells the MMU   →   which Page Table to use
range           →   0 - 1023 
covers          →   4MB per index (each page table covers 4MB)
```


### Page Table index (21-12)

This is the index into the page table that the PDT pointed to. The MMU takes these 10 bits and uses them to pick which PTE to read. Since it is 10 bits it can index 1024 entries — exactly the size of one page table.

```
tells the MMU   →   which frame to use 
range           →   0 - 1023 
covers          →   4KB per index (each PTE covers one frame)
```

### Offset (12-0)
This is the byte offset within the physical frame. Once the MMU has the physical frame address from the PTE it adds this offset to get the exact byte being accessed. Since it is 12 bits it can address 4096 bytes — exactly the size of one frame.

```
tells the MMU   →   which exact byte within the frame
range           →   0 - 4095
covers          →   every byte within the 4KB frame
```






