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



```
init time:

1.  send ICW1   →   start initialization 
2.  send ICW2   →   set offset (0x20 master, 0x28 slave)
3.  send ICW3   →   tell chips about cascade wiring 
4.  send ICW4   →   tell it 8086 mode (0x01)

runtime:
5.  send EOI    →   after every IRQ handler (0x20 to command port)
```
## Initialization Command Word 
**ICW (Initialization Command Word)** is a sequence of 4 commands you send to the PIC to configure it before it can be used. It tells the PIC everything it needs to know to operate correctly -- where to map its IRQs in the IDT, how the master and slave are connected, and what CPU it is talking to. Untill all 4 are send in order the PIC stays in initialization mode and forwards nothing to the CPU 


The PIC initialization is a **4 step sequence** you must complete in exact order. The PIC has an internal state machine that advances one step per ICW -- it will not accept normal operation commands until all 4 are sent.



### ICW1 — sent to command port 

Triggers initialization mode on the PIC. The chip stops forwarding IRQs and enters a state where it expects ICW2, ICW3, ICW4 to follow.
```
outb(0x20, 0x11)    →   master enters init mode 
outb(0xA0, 0x11)    →   slave enters init mode 
```
### ICW2 — sent to data port 

sets the vector offset — the base IDT vector number this chip's IRQs will map to:

```
outb(0x21, 0x20)    →   master IRQs start at vector 0x20 
outb(0xA1, 0x28)    →   slave IRQs start at vector 0x28  
```

### ICW3 — sent to data port 
Configures the cascade wiring between master and slave 
```
outb(0x21, 0x04)    →   master: slave connected at IRQ2 
outb(0xA1, 0x02)    →   slave: identity number is 2 
```

### ICW — sent do data port 
Stes the CPU mode the PIC is operating with 
```i
outb(0x21, 0x01)    →   master: 8086 mode 
outb(0xA1, 0x01)    →   slave: 8086 mode      
```















