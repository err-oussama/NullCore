# Interupt Request (IRQ)

**Interrupt Request (IRQ)** is a signal sent by an external hardware device to the CPU to notify it that something happend and needs attention. The device cannot wait -- it interrupts whatever the CPU is currently doing to get serviced.


The signal goes through the **PIC (Programmable Interrupt Controller)** which prioritizes and forwards it to the CPU as an interrupt vector number. The CPU then pauses execution, jumps to the registered handler, services the device, and resumes where it left off.

```
Examples:
    keyboard    ->  you pressed a key       ->      IRQ1
    timer       ->  another tick elapsed    ->      IRQ0
    hard disk   ->  data finished loading   ->      IRQ14 
    mouse       ->  cursor  moved           ->      IRQ12
```
The key property that defines an IRQ -- it is **Asynchronous**. It has nothing to do with what code is currently running. It can fire at any point, in the middle of any instruction, completely independent of your program's logic.


# Programmable Interrupt Controller (PIC)

**Programmable Interrupt Controller (PIC)** is a chip that sits between hardware devices and the CPU, acting as a middleman that manages and prioritizes interrupt signals from multiple devices and delivers them one at a time to the CPU in an orderly way.


**Without the PIC this would be the problem**:
Every hardware device would be wired directly to the CPU -- the CPU would have no way to  know which device fired, handle multiple simultaneous requests, or prioritize one device ever another. It would be pure chaos.


**What the PIC does**:

```
1.  Reveives IRQ signals from hardware devices 
2.  Prioritizes them (IRQ0 always beats IRQ1 etc)
3.  Waits until CPU is ready (IF flag is set)
4.  Forwards the interrupt as a vector number to the CPU 
5.  Waits for EOI from the handler before sending the next one 

```


**The full picture**


```
                    Key board pressed
                            ↓
     Keyboard controller sends signal to PIC on IRQ1
                            ↓
           PIC checks priority, checks IF flag
                            ↓
            PIC tells CPU "Vector 0x21 needs handling"
                            ↓
                  CPU jumps to IDT[0x21]
                            ↓
         Handler reads scancode, sends EOI to PIC 
                            ↓
              PIC ready to forward next IRQ
```

**In one line**: The PIC is the traffic controller between hardware devices and the CPU -- it collects interrupt signals, prioritizes them, and delivers them one at a time so the CPU never gets overwhelmed.

## The 8259 PIC
The 8259 PIC was originally designed as a **single chip** that could handle 8 IRQ lines. When the IBM PC AT cames out it needed more than 8 IRQs to handle all the devices it supported -- so instead of redesigning the chip, they simply **chained two of them together**:


### Master 8259 (ports 0x20/0x21):
- Directly connected to the CPU 
- Handler IRQ0-IRQ7
- The CPU only talks to this chip 
- IRQ2 is sacrificed as the **cascade line** -- it is the physical wire connecting the slave output into the master input



```
master offset = 0x20 

    IRQ0    →   0x20 
    IRQ1    →   0x21  
    IRQ2    →   0x22
    IRQ3    →   0x23
    IRQ4    →   0x24 
    IRQ5    →   0x25 
    IRQ6    →   0x26
    IRQ7    →   0x27 
```

### Slave 8259 (ports 0xA0/0xA1):
- Not connected to the CPU Directly 
- Handles IRQ8-IRQ15 
- Its output feeds into **IRQ2 of the master**
- When a slave IRQ fires it signals the master via IRQ2, then the master tells the CPU 

```
Slave offset = 0x28 

    IRQ8    →   0x28
    IRQ9    →   0x29  
    IRQ10   →   0x2A
    IRQ11   →   0x2B
    IRQ12   →   0x2C 
    IRQ13   →   0x2D 
    IRQ14   →   0x2E
    IRQ15   →   0x2F 
```


**The chain**:
```
    device fires IRQ12 (mouse)
                ↓
        Slave receives it  
                ↓
Slave signals master through IRQ2 
                ↓
      Master forwards to CPU 
                ↓
       CPU jumps to handler 
```

Both chips are **Indentical hardware** -- same chip, same design. The only different is one is configured as master and one as slave during the initialization sequence you send via `outb`




