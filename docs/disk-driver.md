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




## ATA / IDE Architecture Reference


### The Core Definitions

- **IDE**: the *physical hardware standard*. it refers to the fact that drive controller is integrated directly onto the hard drive itself, and defines the physical connectors and the 40/80-pin ribbon cables used to connect them.

- **ATA**: the *software/communication protocol*. it is the language and set of rules the CPU uses to send commands, addresses and data over the IDE physical connection.



### Physical Topology: the 4-Drive Limit
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




### I/O Port Mapping 
To talk to the drives, the CPU uses *Port-Mapped I/O*. Each channel has a dedicated range of 8 ports for commands/data, pluse 1 extra port for control/reset.

- **Primary channel**   : Command Block `0x1F0` - `0x1F7`, Control Block `0x3F6`
- **Secondary Channel** : Command Block `0x170` - `0x177`, Control Block `0x376`



### The ATA Registers (Command Block)

#### `0x1F0` / `0x170`
- **size** : 16-bit
- **Name** : Data
- **Read** : Reads 16 bits (2 bytes) of actual sector data from the drive's internal buffer. (must be done 256 times to read a full 512-byte sector).
- **Write**: Writes 16 bits (2 bytes) of actual sector data to the dire's internal buffer. (must be done 256 times to write a full 512-byte sector).

#### `0x1F1` / `0x171`
- **size** : 8-bit
- **Name** : Error / Features
- **Read** : Returns a specific error code explaining why last command failed. (Only valid if the ERR bit in the Status register is set).
- **Write**: Set optional drive features for the next command (e.g., enabling/disabling the drive's write cache, or setting DMA transfer modes). Mostly used in the basic PIO.

#### `0x1F2` / `0x172`
- **size** : 8-bit
- **Name** : Sector Count 
- **Read** : Returns the number of sectors remaining to be transferred. (Useful if you are doing muli-sector reads and need to track progress).
- **Write**: Specifies how many sectors (1 to 255) the upcoming Read/Write command should process. (Note: Writing a value of 0 tells the drive to process 256 sectors).

#### `0x1F3` / `0x173`
- **size** : 8-bit
- **Name** : LBA Low
- **Read** : Returns bits 0-7 of the current Logical Block Address.
- **Write**: Sets bits 0-7 of the target Logical Block Address (the sector number for the next command).

#### `0x1F4` / `0x174`
- **size** : 8-bit
- **Name** : LBA Mid
- **Read** : Returns bits 8-15 of the current Logical Block Address.
- **Write**: Set bits 8-15 of the target Logical Block Address.

#### `0x1F5` / `0x175`
- **size** : 8-bit
- **Name** : LBA High
- **Read** : Returns bits 16-23 of the current Logical Block Address.
- **Write**: Sets bits 16-23 of the target Logical Block Address.

#### `0x1F6` / `0x176`
- **size** : 8-bit
- **Name** : Drive / Head
- **Read** : Returns the currently selected drive/head and LBA bits 24-27.
- **Write**: Selects the target dirve and addressing mode.
    - Bit *4*   : 0 = Master, 1 = Slave.
    - Bit *6*   : 0 = CHS mode, 1 = LBA mode.
    - Bit *0-3* : LBA bits 24-27.

####  `0x1F7` / `0x177`
- **size** : 8-bit
- **Name** : Status / Command
- **Read** : Returns the current state of the drive (Status). Key bits to check:
    - *BSY* (Busy)
    - *DRQ* (Data Request - ready to transfer data).
    - *ERR* 
    reading this port acknowledges and clears the pending hardware interrupt.
- **Write**: Sends the actual command to the drive to execute.
    - *0x20*: Read Sectors
    - *0x30*: Write Sectors
    - *0xEC*: Identify


#### `0x3F6` / `0x376`
- **size** : 8-bit
- **Name** : Alt Status / Control
- **Read** : Returns the exact same Status information as port `0x1F7`/`0x177`, *BUT* reading it *does not* acknoloedge or clear the pending hardware interrupt. (Useful for pooling the drive's status without messing up your interrupt handling).
- **Write**: Controls the hardware bus.
    - *Bit 2 (SRST)*: Triggers a software reset of the entire ATA channel.
    - *Bit 1 (nIEN)*: Masks/disables hardware interrupts from the drive (forces you to use polling).



