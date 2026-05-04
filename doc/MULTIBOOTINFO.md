# Multi Boot Info
## What is MultiBoot Info 


When GRUB (GRand Unified Bootloader) loads a kernel it does not just dump the kernel into memory and jump to it -- it also collects information about the system and passes it to the kerenl before handing over control. This information is called the **Multiboot info struct**.

It is a struct that GRUB fills in and places somewhere in memory, then passes a pointer to it in `EBX` right before jumping to the kernel entry point.It contains everything GRUB descoverd about the system during boot:
    
    how much RAM exists
    where RAM is available and where it is reserved
    what boot device was used
    command line arguments passed to the kernel 
    what modules were loaded alongside the kernel


## Why the kernel needs it 
A kernel boots with no knowledge of the machine it is running on. It does not know how much RAM exists, where it is safe to use, or what the hardware looks like. GRUB already did the work of asking the BIOS for all this information -- the multiboot info structure is how GRUB hands that information over.
Without it the kernel would have to query the BIOS itself, which requires switching back to real mode -- a painful process. GRUB saves from that entirely.



**In one line**

Multiboot info is a struct GRUB fills in at boot time and passes to the kernel containing everything it needs to know about the system -- most importantly the physcial memory map.


