# Disk Driver


## Overview 

### Advanced Technology Attachment (ATA)
**ATA** is the command protocol used to communicate between a host system and storage devices sush as hard drives and optical dives. It defines the set of commands (`READ`, `WRITE`, `IDENTIFY`, etc.), the register layout, and the data transfer mechanism.

Communication happens through I/O ports, the host sends commands and parameters by writing to specific port addresses, and the drive responds by updating status registers and making data available.

ATA was the dominant storage protocol for PCs from late 1980s through the mid-2000s, later evolving into *SATA*(Serial ATA) which kept the same command set but replaced the physical interface.





### Integrated Drive Eloctronics (IDE)
**IDE** refers to the physical interface and cable standard that ATA drives connect through.

The name comes from the fact that the drive controller electornics are integrated directly onto the drive itself, rather than being a separate card in the system, a significant simplification over earlier designs.

IDE uses a wide 40 or 80-pin ribbon cable connecting the drive to the motherboard, supporting up to tow dirves per channel (master and slave) on up to two channels (primary and secondary).

IDE was essentially the consumer name for the same thing ATA described technically, the two terms are used interchangeably in practice, with IDE referring more to the physical connector and ATA referring more to the protocol.




## The Core Definitions

- **IDE**: the *physical hardware standard*. it refers to the fact that drive controller is integrated directly onto the hard drive itself, and defines the physical connectors and the 40/80-pin ribbon cables used to connect them.

- **ATA**: the *software/communication protocol*. it is the language and set of rules the CPU uses to send commands, addresses and data over the IDE physical connection.



## Physical Topology: the 4-Drive Limit
The legacy ATA architecture supports a maximum of **4 dirve**. They are organized into **2 Channels** (*Primary* and *Secondary*).


- Each channel has its own dedicated controller and uses **one ribbon cable**.
- Each ribbon cable has connectors for exactly **2 dirves**.
- To share the cable, one dirve must be physically configured as the **Master** and the other as the **Slave** (using jumpers on the drive).



```
      PRIMARY CHANNEL                     SECONDARY CHANNEL
    (Ports 0x1F0 - 0x1F7)               (Ports 0x170 - 0x177)
             |                                   |
    [  Ribbon Cable 1  ]                [  Ribbon Cable 2  ]
    |                  |                |                  |
[Master]            [Slave]         [Master]            [Slave]
(Drive 0)           (Drive 1)       (Drive 2)           (Drive 3)
```




## I/O Port Mapping 
To talk to the drives, the CPU uses *Port-Mapped I/O*. Each channel has a dedicated range of 8 ports for commands/data, pluse 1 extra port for control/reset.

- **Primary channel**:
    - Command Block `0x1F0` - `0x1F7` 
    - Control Block `0x3F6`
- **Secondary Channel**:
    - Command Block `0x170` - `0x177`
    - Control Block `0x376`



## The ATA Registers (Command Block)

### Data (`0x1F0`/`0x170`)
- **size** : 16-bit
- **Read** : Reads 16 bits (2 bytes) of actual sector data from the drive's internal buffer. (must be done 256 times to read a full 512-byte sector).
- **Write**: Writes 16 bits (2 bytes) of actual sector data to the dire's internal buffer. (must be done 256 times to write a full 512-byte sector).

### Error / Features (`0x1F1`/`0x171`)
- **size** : 8-bit
- **Read** : Returns a specific error code explaining why last command failed. (Only valid if the ERR bit in the Status register is set).
- **Write**: Set optional drive features for the next command (e.g., enabling/disabling the drive's write cache, or setting DMA transfer modes). Mostly used in the basic PIO.

### Sector Count  (`0x1F2`/`0x172`)
- **size** : 8-bit
- **Read** : Returns the number of sectors remaining to be transferred. (Useful if you are doing muli-sector reads and need to track progress).
- **Write**: Specifies how many sectors (1 to 255) the upcoming Read/Write command should process. (Note: Writing a value of 0 tells the drive to process 256 sectors).

### LBA Low (`0x1F3`/`0x173`)
- **size** : 8-bit
- **Read** : Returns bits 0-7 of the current Logical Block Address.
- **Write**: Sets bits 0-7 of the target Logical Block Address (the sector number for the next command).

### LBA Mid (`0x1F4`/`0x174`)
- **size** : 8-bit
- **Read** : Returns bits 8-15 of the current Logical Block Address.
- **Write**: Set bits 8-15 of the target Logical Block Address.

### LBA High (`0x1F5`/`0x175`)
- **size** : 8-bit
- **Read** : Returns bits 16-23 of the current Logical Block Address.
- **Write**: Sets bits 16-23 of the target Logical Block Address.

### Drive / Head (`0x1F6`/`0x176`)
- **size** : 8-bit
- **Read** : Returns the currently selected drive/head and LBA bits 24-27.
- **Write**: Selects the target dirve and addressing mode.
    - Bit *4*   : 0 = Master, 1 = Slave.
    - Bit *6*   : 0 = CHS mode, 1 = LBA mode.
    - Bit *0-3* : LBA bits 24-27.

### Status / Command (`0x1F7` / `0x177`)
- **size** : 8-bit
- **Read** : Returns the current state of the drive (Status). Key bits to check:
    - *BSY* (Busy)
    - *DRQ* (Data Request - ready to transfer data).
    - *ERR* 
    reading this port acknowledges and clears the pending hardware interrupt.
- **Write**: Sends the actual command to the drive to execute.
    - *0x20*: Read Sectors
    - *0x30*: Write Sectors
    - *0xEC*: Identify


### Alt Status / Control (`0x3F6` / `0x376`)
- **size** : 8-bit
- **Read** : Returns the exact same Status information as port `0x1F7`/`0x177`
    *BUT* reading it *does not* acknoloedge or clear the pending hardware interrupt. 
    (Useful for pooling the drive's status without messing up your interrupt handling).
- **Write**: Controls the hardware bus.
    - *Bit 2 (SRST)*: Triggers a software reset of the entire ATA channel.
    - *Bit 1 (nIEN)*: Masks/disables hardware interrupts from the drive (forces you to use polling).






## Register Values

### Error Register (`0x1F1` / `0x171`) - Read Only
- **Bit 7 (BBK - Bad Block)**:
    - *0*: No bad block detected.
    - *1*: A bad block mark was detected in the sector.

- **Bit 6 (UNC - Uncorrectable Data Error)**:
    - *0*: No uncorrectable error.
    - *1*: Data could not be recoverd even with ECC, sector is unreadable.

- **Bit 5 (MC - Media Changed)**:
    - *0*: Media has not changed.
    - *1*: The media was changed (relevent for removable dirves like CD-ROM).

- **Bit 4 (IDNF - ID Not Found)**:
    - *0*: Sector found successfully.
    - *1*: The requestd sector ID could not be found on the disk.

- **Bit 3 (MCR - Media Change Requested)**:
    - *0*: No media change requested.
    - *1*: Drive is requesting a media change (removable direves only).

- **Bit 2 (ABRT - Command Aborted)**:
    - *0*: Command completed normally.
    - *1*: command was aborted - usually means the command is unsupported or invalid parameters were given.

- **Bit 1 (TK0NF - Track 0 Not Found)**:
    - *0*: Track 0 found successfully.
    - *1*: Legacy CHS bit, track 0 could not be found during recalibration. Irrelevent with LBA.

- **Bit 0 (AMNF - Address Mark Not Found)**:
    - *0*: Address mark found.
    - *1*: Legacy CHS, data addres mark not found. Irrelevent with LBA.


### Feature Register (`0x1F1` / `0x171`) - write only

This register sends feature paramaters to the drive before issuing certian commands.
Most basi PIO operation (READ, WRITE, IDENTIFY) do not require anything written here; it is only needed for advanced commands.

- **Bit 7-0 (Feature Code)**:
    - A raw 8-bit value whose meaning depends entirely on the command being send. There are no fixed bit definitions, each command that uses this register defines its own feature byte format.
Common uses:

- *0x00*: Disable write cache (used with SET FEATURES command `0xEF`)
- *0x02*: Enable write cache.
- *0x03*: Set transfer mode (PIO, DMA speed selection)
- *0x55*: Disable read look-ahead.
- *0xAA*: Enable read look-ahead.


### Drive / Head Register (`0x1f6` / `0x176`)
- **Bit 7 (Always 1)**: 
    - *0*: Invalid must never be written as 0
    - *1*: Must always be written as `1`. (Legacy CHS head bit).

- **Bit 6 (LBA Mode)**:
    - *0*: Use legacy CHS (Cylinder/Head/Sector).
    - *1*: Use 28-bit addressing.

- **Bit 5 (Always 1)**:
    - *0*: Invalid must never be written as 0
    - *1*: Must always be written as `1`. (Legacy CHS head bit).

- **Bit 4 (Drive Select)**:
    - *0*: Select Master drive.
    - *1*: Select Slave drive.

- **Bit 0-3**: The highest 4 bits of 28-bit LBA address. (bits 24-27)


### Status Register (`0x1F7` / `0x177`) - Read Only
- **Bit 7 (BSY - Busy)**:
    - *0*: Drive is idle and usable, other bits are valid.
    - *1*: The drive is busy procesing a command, all other bits are meaningless until BSY clears.

- **Bit 6 (DRDY - Drive Ready)**:
    - *0*: Drive is not ready (still spinning up, or no drive present). 
    - *1*: The drive is powered up and initialized, and ready to accecpt commands.

- **Bit 5 (DF - Drive Fault)**:
    - *0*: No drive fault.
    - *1*: A drive fault occurred (hardware error, not a command error).

- **Bit 4 (SRV - Overlapped Mode Service Request)**:
    - *0*: No service request.
    - *1*: Used in overlapped/queued commands.

- **Bit 3 (DRQ - Data Request)**:
    - *0*: No data ready to transfer.
    - *1*: Drive is ready to transfer data, safe to read from or write to the data register.

- **Bit 2 (CORR - Corrected Data)**:
    - *0*: No correction needed.
    - *1*: Data was corrected by the drive's ECC, data is valid but the sector may be degrading.

- **Bit 1 (IDX - Index)**:
    - *0*: Normal.
    - *1*: Legacy bit from CHS era, always 0 in modern drives, can be ignored.

- **Bit 0 (ERR - Error)**:
    - *0*: No Error.
    - *1*: An error occured during the last command, check the Error Register (`0x1F1` / `0x171`) for details.

### Command Register (`0x1F7` / `0x177`)


Triggers the drive to execute a command using whatever parameters were already written to other registers (LBA, sector count, drive select, feature). The drive immediately starts processing the moment this register written.

- **Bit 7-0 (Command Code)**:
    - A row 8-bit command code. No fixed bit definitions, each value is distinct command.

- *0x20* (READ SECTORS): Read sectors using 28-bit LBA, PIO transfer.
- *0x30* (WRITE SECTORS): Write sectors using 28-bit LBA, PIO transfer.
- *0xEC* (IDENTIFY DEVICE): Get drive info (model, size, capabilities).
- *0xE7* (FLUSH CACHE): Commit write buffer to dist.
- *0x08* (DEVICE RESET): Reset the drive 
- *0x90* (EXECUTE DIAGNOSTIC): Run built-in self test
- *0xEF* (SET FEATURES) Configure drive feature (uses Feature Register)
there is more but these all i need.


### Alternate Status Register (`0x3F7`/`0x376`) - Read Only
- Same bit layout as the Status Register (`0x1F7`/`0x177`): BSY, DRDY, DF, SRV, DRQ, CORR, IDX, ERR.

- The key difference: reading `0x3F6`/`0x376` does **not** clear a pending interrupt, while reading  `0x1F7`/`0x177` does.

- Used during the 400ns delay after drive select, reading `0x3F6`/`0x376` 4 times give the required delay without accidentally clearing an interrupt.


### Device Control Register (`0x3F6` / `0x376`) - Write Only
- **Bit 7-3 (Reserved)**:
    - Always write as `0`.

- **Bit 2 (SRST - Software Reset)**:
    - *0*: Normal operation.
    - *1*: Assert a software reset on all drives on the channel. Must be cleared after reset.

- **Bit 1 (nIEN - Not Interrupt Enable)**:
    - *0*: Interrupts enabled, drive will fire an IRQ when a command completes.
    - *1*: Interrupts disabled, drive will not fire IRQs, polling must be used instead.

- **Bit 0 (Reserved)**:
    - Always write as `0`.
