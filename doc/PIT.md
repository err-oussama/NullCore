## Programmable Interval Timer 


**Programmable Interval Timer (PIT)** is a hardware chip (Intel 8253/8254) that fires *IRQ0* at a fixed configurable frequency. You program it by writing a divisor to port `0x40` -- the chip divides its base clock (1,193,182 Hz) by that divisor to produce your desired frequency. 
Every time it fires it sends IRQ0 to the PIC which forwards it to the CPU as an interrupt. 
The kernel uses this regular heartbeat to drive the scheduler - every tick the timer handler runs increments a counter, and checks if the current process has used up its time slice.



