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
















