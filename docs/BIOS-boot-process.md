# BIOS Boot Process 

## Hardwired Reset Vector 

The CPU powers on and start executing at `0xFFFFFFF0` — the hardwired reset vector.

There is only room for one instruction there so it is a `FAR JUMP` that lands at an address inside the BIOS ROM region typically around `0x000FE05B` or similar — this address is not RAM, the chipset routes it directly to the BIOS ROM chip.


## Early POST (Power-On Self-Test)

The BIOS first runs a minimal hardware check — CPU registers, cache, chipset — to verify the bare minimum is working before doing anything else. 

## Cache As RAM (CAR)

At this point RAM is not yet initialized and cannot be used. The BIOS needs a stack and temporary storage to run code, so it configuers the CPU cache (L1/L2) to act as RAM — writes go into cache and stay there instead of being flushed to RAM. This gives the BIOS a small working memory area without needing real RAM.


## RAM Initialization

The BIOS reads the **Serial Presence Detect (SPD)** chips on the RAM sticks — small **EEPROM** chips that store the RAM's speed, timings, and voltage requirements. The BIOS programs the memory controller with these exact values then sends electrical test patterns to verify the connection is stable.



## Memory Training

Different memory regins may respond at slightly different speeds due to electrical charachteristics of the board traces. The BIOS runs a training process — repeatedly testing timing configurations until if finds settings that make all memory regins respond at a consistent stable speed. This ensures every memory access completes in the same amount of time regardless of which address is being accessed.


## POST Memory Test 
After training locks in stable timing the BIOS runs a memory test. If fast boot enabled it only test the first 1MB — if disabled it tests the entire RAM by writing and reading back patterns to verify address is working correctly.

## BIOS Shadowing

The BIOS copies itself from the ROM chip into RAM at `0x000E0000 — 0x000FFFFF`, then marks that region as read-only. The CPU then jumps to the copy in RAM. This is done because RAM access is significantly faster then ROM chip access —  running BIOS code from RAM speeds up all subsequent BIOS operations during boot.


## Continuing POST 

After shadowing the BIOS continues fall POST — initializing all devices, scanning for option ROMs at `0x000C0000 - 0x000EFFFF` to initialize expansion cards, building the IVT at `0x00000000`, setting up the BIOS data area at `0x00000400`, then handling control to the bootloader at `0x00007C00`.



