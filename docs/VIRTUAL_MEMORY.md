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


## Bits explenation 

### P — Present 

Tells the MMU whether this page exists in physical RAM.
If 0 and the CPU tries to access it #PF fires immediately 

### R/W — Read / Write 

Controls whether the page can be written to. 
If 0 the page is read-only and write attempt fires #PF 
if 1 both reads and write are allowed 

### U/S — User / Supervisor 

Controls which privilege level can access this page 
If 0 only ring 0-2 (kernel) can access it -- any user space (ring-3) access fires #PF
If 1 both kernel and user space can access it. 


### PWT — Page Write Through 

Controls the cache write policy for this page. 
If 0 write go to cache first and RAM is updated later.
If 1 every write goes to both cache and RAM simultaneously.

### PCD — Page Cache Disable 

Controls whether this page is cached at all. 
If 0 the pages goes through the CPU cache normally.
If 1 every access bypass the cache and hits RAM directly - used for memory mapped hardware like VGA.



### A — Accessed 

The CPU sets this bit automatically the first time the page is readed from, written to or executed from.
The kernel uses it to track which pages are being used -- useful for page replacement algorithms.



### D — Dirty 

The CPU sets this bit automatically the first time the page is written to. 
The kernel uses it to know if a page was modified -- if dirty the page must be written to disk before being freed.



### PAT — Page Attribute Table 
Extends PWT and PCD to select from a table of 8 possible cache modes. Combined with PWT and PCD it gives 3 bits total selecting one of 8 cache behavior configurations defined in the PAT MSR register.


### G — Global 

If 1 the page translation stays in the TLB even when CR3 is reloaded during a context switch. 
Used for kernel pages that are shared across all processes - avoids flushing and refilling TLB entries that never change.



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


## TLB — Translation Lookaside Buffer

The **TLB** is a small hardware cache inside the CPU that stores recent virtual to physical address translations. Its entries purpose is to avoid repeating the full page table walk every single time the CPU access memory.


## Virtual Address Translation Steps

The CPU executes an instruction that references a memory address. This address is a virual address  — it means nothing until translated.


```
mov eax, [0x00401ABC]     ← 0x00401ABC is virtual 
```

### Step 1 — check TLB 

the MU checks if this virtual address was recently translated and is cached in the TLB 

```
TLB hit     → physical address found, skip to step 5
TLP miss    → must walk page tables 
```

### Step 2 — Split virtual address into 3 parts

```
0x00401ABC = 0000 0000   01 | 00   0000   0001 | 1010   1011 1100


bits 31-22  →   0x001   →   page directory index 
bits 21-12  →   0x001   →   page table index 
bits 12-0   →   0xABC   →   offset   
```
### Step 3 — Read Page Directory 

```
CR3     →   physical address of page directory
page_directory[0x001]   →   read PDE  
check P bit:
    0   →   #PF fires 
    1   →   continue  

PDE gives physical address of page table 
```

### Step 4 — Read Page Table 

```
page_table[0x001]   →   read PTE 
check p bit:
    0 → #PF fires 
    1 → continue
PTE gives physical frame address

```
### Step 5 — Check U/S R/W 
```
check U/S bit → privilege violation → #PF fires  
check R/W bit → write to read-only → #PF fires  

```

### Step 6 — Update A and D bits 
```
any access  → MMU sets A bit in PTE  
write       → MMU sets D bit in PTE  
```

### Step 7 — Cache in TLB 
```
MMU stores virtual  → physcial mapping in TLB 
next access to same page skips steps 3-6 entirely
```


### Step 8 — Combine Frame Address and Offset 

```
frame address       =   PTE bits 31-12      =   0x300000
offset              =   bits 11-0           =   0xABC 
physical address    =   0x300000 + 0xABC    =   0x300ABC   
```
### Step 9 — Access Physical RAM 

```
CPU puts 0x300ABC on address bus 
RAM responds with data 
CPU gets its data
```


### Full Picture 

```
                    Virtual Address     0x00401ABC 
                                ↓
                            TLB check
                                ↓ miss
            splite  → PD=0x001 PT=0x001 Offset=0xABC   
                                ↓
        CR3  →  page_directory[0x001]   →  page table address
                                ↓
            page_table[0x001] → frame address 0x300000
                                ↓
                    set A bit, set D bit if write
                                ↓
                           cache in TLB 
                                ↓
               0x300000 + 0xABC = physical 0x300ABC 
                                ↓
                            access RAM

```


## CR3 

**CR3** is a special CPU register that **holds the physical address of the page directory**. It is the starting point of every address translation — when the MMU needs to translate a virtual address it **always starts by reading CR3 to find the page directory**. 


```
bits 31-12  →   physical address of page directory 
bit 4       →   PCD page cache diable for page directory itself
bit 3       →   PWT write through for page directory itself 
bits 0-2    →   ignored
```

### Key properties
```
privileged          → only ring 0 can read or write it
physical only       →   always holds a physical address, never virtual even after paging is enabled
page aligned        →   lower 12 bits always zero
one per process     →   each process has its own page directory 
                                context switch means loading a new CR3 
TLB flush           →   reloading CR3 flushes the entire TLB automatically

```

### How It works


**CR3** can only be accessed through the `mov` instruction — no arithmetic or logical operations are allowed directly on it. Any modification requires reading it into a general purpose register first, modifiying that register, then writing back.
```
read        →   mov eax, cr3 
write       →   mov cr3, eax 
```

**In one line**: CR3 is the register that connects the CPU to the page tables — it tells the MMU where the page directory lives, and every virtual address translation starts from it.


## CR0 

**CR0** is a privileged CPU register that controls the fundamental operating modes of the CPU, Unlike CR3 which holds an address, **CR0 is a collection of feature flags** — each bit enables or disables a core CPU capability.

### The Bits that matters for now 

```
bit 0   PE  →   Protection Enable   → switches CPU from real mode to protected mode

bit 16  WP  →   Write Protect       → prevents kernel from writing to read only pages


bit 31  PG  →   Paging              → enables virtual memory translation via page tables
```
