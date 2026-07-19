# Disk Driver


## Advanced Technology Attachment (ATA)
**ATA** is the command protocol used to communicate between a host system and storage devices sush as hard drives and optical dives. It defines the set of commands (`READ`, `WRITE`, `IDENTIFY`, etc.), the register layout, and the data transfer mechanism.

Communication happens through I/O ports, the host sends commands and parameters by writing to specific port addresses, and the drive responds by updating status registers and making data available.

ATA was the dominant storage protocol for PCs from late 1980s through the mid-2000s, later evolving into *SATA*(Serial ATA) which kept the same command set but replaced the physical interface.





## Integrated Drive Eloctronics (IDE)
**IDE** refers to the physical interface and cable standard that ATA drives connect through.

The name comes from the fact that the drive controller electornics are integrated directly onto the drive itself, rather than being a separate card in the system, a significant simplification over earlier designs.

IDE uses a wide 40 or 80-pin ribbon cable connecting the drive to the motherboard, supporting up to tow dirves per channel (master and slave) on up to two channels (primary and secondary).

IDE was essentially the consumer name for the same thing ATA described technically, the two terms are used interchangeably in practice, with IDE referring more to the physical connector and ATA referring more to the protocol.


