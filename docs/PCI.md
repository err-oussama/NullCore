
# Peripheral Component Interconnect (PCI)

- **Peripheral**    : Device that connect to the CPU indirectly (not built into the CPU)
- **Component**     : Hardware component/cards
- **Interconnect**  : The bus/interface that connects them together

        
## What is PCI

***Peripheral Component Interconnect (PCI)*** is a **standard** bus interface that defines how the CPU, memory, and peripheral devices(Network Cards, Sound Cards, Storage Controllers, Graphics Cards, etc.) connect to and communicate with each other on a computer's motherboard.
It is both a:
- *physical bus*: the electrical wiring/slots.
- *protocol*: the rules for addressing and communicating with devices on that bus.
It provides a standardized way for the OS to discover, configure, and talk to any connected device through a unified **configuration space**, regardless of what the device actually does.
Each device on the PCI bus exposes a set of configuration registers(Vendor ID, Device ID, Class Code, Memory I/O regions, etc.) that the OS reads to identify and initialize it.
This makes it possible to write a single enumeration routine that works for any PCI device, rather than needing device-specific discovery code for each.



## The Bus Hierarchy

### The Bus

A **bus** is a shared communication pathway that connects multiple PCI devices to the CPU.
The PCI standard supports up to 256 buses (numbered 0-255), organized in a hierarchy:
```
Bus 0 → the primary bus, directly connected to the CPU
Bus 1 → a secondary bus, created by a PCI bridge connected to bus 0
Bus 2 → another secondary bus, created by another PCI bridge
...
Bus 255 → maximum 256 buses total
```
*Bus 0* always exists and is where all directly connected devices sit.
Additional buses only appear when a **PCI bridge** device is present, a bridge extens the hierarchy by creating a new bus behind it, allowing more devices to be connected than a single bus could support.

## The Host Bridge
The **host bridge** is the hardware component that connects the CPU to the primary PCI bus (bus 0).
It sits between the CPU's inernal bus and the PCI bus, translating between the two protocols and making all PCI devices visible to the CPU.
It is always present in every PCI system, always located at bus 0, device 0, function 0, and is typically integrated directly into the chipset (northbridge) rather then being a separate card.
Every memory access, I/O port access, and configuration space read that CPU makes the PCI devie passes through the host bridge first.



## Configuration Space

The **configuration space** is a standardized 256-byte block of registers that every PCI device exposes, allowing the OS to idenfiy, configure, and manage it without any device-specific knowledge.
The first 64-bytes follow a fixed layout identical across all PCI devices, containinig the vendor ID, device ID, class code, BAR addresses, and interrupt line, so a single enumeration routine can discover and read basic information from any PCI device regardless of what it actually does.
The remaining bytes are device-specific and vary per device type.
The OS accesses configuration space not through memory or regular I/O ports, but through a dedicated mechanism using two special I/O ports (`0xCF8` and `0xCFC`), writing the target device address to `0xCF8` and reading the result from `0xCFC`.


### Registers 

#### Identification

| Offset | Size  |     Name     |                               Role                                                            |
|--------|-------|--------------|-----------------------------------------------------------------------------------------------|
| 0x00   | 16bit | Vendor ID    | Idenfifies the manufacture. Returns `0xFFFF` if the slot is empty                             |
| 0x02   | 16bit | Device ID    | Identifies the specific chip model. Meaningful only when paried with Vendor ID                |
| 0x09   | 8bit  | Prog IF      | Defines the register-level programming interface withing a subclass (e.g., AHCI vs IDE)       |
| 0x0A   | 8bit  | Subclass     | Specifies the exact device category within a class (e.g., Ethernet vs Token Ring)             |
| 0x0B   | 8bit  | Class code   | Defines the broad device category (e.g., Network, Storage, Display).                          |
| 0x0E   | 8bit  | Header Type  | Determines the layout of the rest of the config space (`0x00=normal device`, `0x01=bridge`)   | 

#### Communication

| Offset | Size  |     Name     |                               Role                                                            |
|--------|-------|--------------|-----------------------------------------------------------------------------------------------|
| 0x10   | 32bit | BAR0         | Either maps a window of the device's internal registers into the CPU's physical memory address space (`bit 0=0`) or provides a base I/O port number through which the device's registers are accessed via *in/out* instructions (`bit 0=1`). | 
#### Activation

| Offset | Size  |  Name    |                               Role                                    |
|--------|-------|----------|-----------------------------------------------------------------------|
| 0x04   | 16bit | Command  | Controls device operation. Must set I/O Space Enable (Bit 0), Memory Space Enable (Bit 1) and Bus Master Enable (Bit 2) to allow the device to respond to memory and perform DMA. |


#### Interrupts

| Offset | Size  |     Name         |                               Role                                                                                                             |
|--------|-------|------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| 0x3D   | 8bit  | Interrupt Pin    | Identifies which physical PCI interrupt pin the device uses to signal the CPU (`1=INTA`, `2=INTB`, `3=INTC`, `4=INTD`). `0` mean no interrupts. |
| 0x3C   | 8bit  | Interrupt Line   | Routes the device's interrupt pin to a specific system IRQ number (e.g., IRQ 20) for CPU handling.                                             |


## The Two Configuration Ports

### `0xCF8`: CONFIG_ADDRESS

The address port, you write a 32-bit value here to specify **which device and which register** you want to access.
The value is structured as:

```
bit 31     :    Enable bit, must always be 1
bits 30-24 :    Reserved, always 0
bits 23-16 :    Bus number (0-255)
bits 15-11 :    Device number (0-31)
bits 10-08 :    Function number (0-7)
bits 07-02 :    Register offset, which 4-byte register to read
bits 01-00 :    Always 0, registers are 4-byte aligned
```


### `0xCFC`: CONFIG_DATA

The data port, after writing the address to `0xCF8`, you read ot write here to actually access the register you specified.
Reading returns the 32-bit value of that configuration register, writing modifies it.


## Base Address Registers (BARs)

**BARs** are the configuration registers (starting at offset `0x10`) that define where a device's memory or I/O space lives in the system address map.
To configure a BAR, the OS must first discover the size the devie needs by writing all 1s (`0xFFFFFFFF`) to the register and reading it back; the device clears the lower bits, revealing its, required size and alignment.
The OS then writes the actual base address back into the BAR.
If a device needs to map memory above 4GB limit, it requests a 64-bit BAR, which consumes two consecutive 4-byte registers to hold the upper and lower halves of the address.


