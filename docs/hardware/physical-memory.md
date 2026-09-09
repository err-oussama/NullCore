# Memory


## Frame

A **Frame** is a **fixed-siz 4KB of physical RAM**. It is the smallest unit the physical memory manager works with. Physical memory is divided into frames sequentially from address 0 to the top of RAM -- frame 0 starts at address `0x00000000`, frame 1 at `0x00001000`, frame 2 at `0x00002000`, and so on. When the physical memory manager allocates memory it hands out one frame at a time, and when it frees memory it takes one frame back.


## Physical Address Map 

### Part I — 0x00000000 → 0x0009FBFF  (Low Memory / First 640KB)

#### 0x00000000 → 0x000003FF

**Real Mode Interrupt Vector Table (IVT)** — 1KB 
256 entries, each 4 bytes, holding segment:offset addresses or real mode interrupt handlers. Written by BIOS at boot.


#### 0x00000400 → 0x000004FF

**BIOS Data Area (BDA)** — 256 Bytes 
BIOS stores system information here during boot — keyboard state, video mode, memory size, port addresses.


#### 0x00000500 → 0x0009FBFF

**Free — avoid**
Technically available RAM but to small and to close to reserved regions to be worth using safely.

---



### Part II — 0x0009FC00 → 0x000EFFFF (Upper Memory Area)

#### 0x0009FC00 → 0x0009FFFF 

**Extanded BIOS Data Area (EBDA)**
BIOS uses this for extra data that does not fit in BDA — USB keyboard state, APM info, hardware tables.



#### 0x000A0000 → 0x000AFFFF 

**VGA Graphics Framebuffer** — 64KB 

Used in graphics modes. Each byte represents pixel data 
Unused in text mode.


#### 0x000B0000 → 0x000B7FFF 
**VGA Monochrome Text Buffer** — 32KB 
Text Buffer for old monochrome MDA displays.
Not used on modern systems, exists for backward compatibilty.


#### 0x000B8000 → 0x000BFFFF    

**VGA Color Text Buffer** — 32KB 
The standard text mode buffer, Each cell is 2 byte:
charachter byte + attribute byte (color)
Writing here directly changes what appears on screen. 


#### 0x000C0000 → 0x000EFFFF    

**Option ROMs** — 192KB
Firmware for expansion cards — GPU BIOS, network cards, SCSI controllers. BIOS scans this region at boot and executes any valid ROM it finds to initialize the hardware.


#### 0x000F0000 → 0x000FFFFF

**System BIOS ROM Shadow** — 64KB 
BIOS copies itself from ROM chip into RAM here during boot (Shadowing) for faster acces. Marked read-only after copying.



### Part III 0x00100000 → 0x07FDFFFF (Main RAM)


#### 0x00100000 

Kernel loads here. This is the start of extended memory — everything above 1MB.
The largest usable RAM region.

#### kernel_end → pool_start

Safety alignment — kernel_end rounded up to next 4KB boundary.

#### pool_start → pool_start + bitmap_size

PMM bitmap — tracks free/used state of every page frame in the pool. One bit per frame.
Lives at start of pool.

#### pool_start + bitmap_size → 0x07FDFFFF

Free page frames — available for allocation by PMM.
This is the main memory pool the kernel manages.

---


### Part IV — 0x7FE00000 → 0xFFFBFFFF (Reserved / Hardware)

#### 0x07FE0000 → 0x0FFFFFFF

**ACPI tables and firmware reserved regions**.
BIOS places ACPI data here.
Read once at boot, then ignore.

#### 0x100000000 → 0xFEBFFFFF

**PCI/PCIe device memory mapped regions**.

GPU VRAM windows, device buffer, PCI configuarion space.
Exact layout determined by BIOS during PCI enumeration.
Read from PCI config space or ACPI to find exact addresses.

#### 0xFEC00000 → 0xFEDFFFFF

**I/O APIC registers** — fixed address, Intel specification.
Memory mapped registers for the I/O APIC interrupt controller.
Must map with cache disabled (PCD=1) when accessed.


#### 0xFEE00000 → 0xFEEEEEEEE

**Local APIC registers** — fixed address, Intel specifications.
Memory mapped registers for the per-CPU Local APIC.
Must map with cache disabled (PCD=1) when accessed.




#### 0xFEF00000 → 0xFFFBFFFF 

Other firmware and hardware reserved regions.

---

### Part V 0xFFFC0000 - 0xFFFFFFFF


- In Real Mode — BIOS ROM  Chip.

    - The chipset permenently routes all accesses in this range directly to the BIOS ROM chip. 
    - CPU starts executing at 0xFFFFFFF0 — the reset vector — which contains a far jump into the main BIOS code. This routing exists throughout all of real mode regardless of where the CPU is executing.



- In Protected Mode — empty hole.

    - The chipset stops routing accesses to the BIOS ROM Chip.
    - The region becomes an unmapped hole in the address space.


