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
