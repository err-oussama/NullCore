# Network


## Peripheral Component Interconnect (PCI)

- **Peripheral**:   Device that connect to the CPU indirectly
                    (not built into the CPU)
- **Component**:    Hardware component/cards
- **Interconnect**: The bus/interface that connects them together

        
**Peripheral component Interconnect (PCI)** is a *standard bus interface* that allows hardware device such as `network cards`, `sound cards` and `storage controllers` to connect to and communicate with the CPU and memory.
It provides a standardized way for the OS to discover, configure, and talk to any connected device through a unified configuration space, regardless of what the device actually does.
Each device on the PCI bus exposes a set of configuration registers (Vendor ID, Device ID, Memory regions) that the OS reads to identify and initialize it, making it possible to write a single enumeration routine that works for any PCI device rather that needing device-specific discovery code for each one.

