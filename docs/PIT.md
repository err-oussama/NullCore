# Programmable Interval Timer 


**Programmable Interval Timer (PIT)** is a hardware chip (Intel 8253/8254) that fires *IRQ0* at a fixed configurable frequency. You program it by writing a divisor to port `0x40` -- the chip divides its base clock (1,193,182 Hz) by that divisor to produce your desired frequency. 
Every time it fires it sends IRQ0 to the PIC which forwards it to the CPU as an interrupt. 
The kernel uses this regular heartbeat to drive the scheduler - every tick the timer handler runs increments a counter, and checks if the current process has used up its time slice.





## How It  Work 

The **PIT** has an internal counter that starts at the **divisor** value and counts down to zero.
When it hits zero it fires IRQ0 and reloads the divisor automatically — repeating forever. The faster the countdown the higher the frequency.




## Ports 

Every port is 1 byte wide

### Port 0x40 — Channel 0 Data

The main timer channel connected to IRQ0. You write the divisor here after sending the command to `0x43`. You can also read from it to get the current countdown value

```
write   →   set divisor (after commend sent to 0x43)
read    →   set current counter value (how far into countdown)
used    →   always, this is the scheduler timer
```

### Port 0x41 — Channel 1 Data 

Origianlly used for DRAM refresh on old IBM PCs — the BIOS used it to periodically refresh RAM. Completely obsolete on modern systems, the memory controller handles refresh automatically.

```
write   →   set divisor for channel 1    
read    →   get current counter value 
used    →   never on modern systems
```

### Port 0x42 — Channel 2 Data 

Connected to the PC speaker. Writing a frequency here makes the speaker deep at that pitch. Also connected to port `0x61` (keyboard controller) which gates the output

```
write   →   set speaker frequency  
read    →   get current conter value 
used    →   only if you want PC speaker deeps
```


### Port 0x43 — Mode/Command register

Write only —  you cannot read from it. Controls how all three channels behave:

```
write   →   send command byte to configure a channel  
read    →   undefined, never read from this port 
used    →   always first before writing divisor to 0x40/0x41/0x42 
```

#### Command Byte Value structure 

The command byte sent to `0x43` is structured as: 

```
bit 7-6 →   channel select 
bit 5-4 →   access mode 
bit 3-1 →   operating mode 
bit 0   →   number format 
```

##### Bits 7-6  — Channel Select

```
00   →  channel 0 (IRQ0 timer) 
01   →  channel 1 (obsolete)
10   →  channel 2 (PC Speacker)
11   →  read back command 
```




##### Bits 6-5 — Access Mode

```
00   →  latch count value 
01   →  low byte only 
10   →  high byte only 
11   →  low byte then high byte 
```


**Latch Count Value**: It is a special command that freezes the current counter value so it can be readed safely. Latch clears automatically after both bytes read.




##### Bits 3-1 — Operating Mode

```
000     →   mode 0  interrupt on terminal count  
001     →   mode 1  hardware re-triggerable one shot 
010     →   mode 2  rate generator 
011     →   mode 3  square wave generator 
100     →   mode 4  software triggered strobe
101     →   mode 5  hardware triggered strobe 
```


**Mode 0 — Interrupt Terminal Count**: Counts down once from the divisor to zero, fires IRQ0 once, then stops. Does not reload - write a new divisor to start it again

**Mode 1 — Hardware Retriggerable one Shot**: Same as mode 0 but the countdown restarts if a hardware trigger signal arrives on the gate pin before it reaches zero.

**Mode 2 — Rate Generator**: Counts down to 1, fires IRQ0, reloads divisor automatically and repeats forever.

**Mode 3 — Square Wave Generator**: Similar to mode 2 but the output pit alternates between high and low - producing a square wave. Used for the PC speaker.

**Mode 4 — Software Triggered Strobe**: Counts down to zero, fires a single on the output pin, stops. Triggered by software writing the divisor.

**Mode 5 — Hardware Triggered Strobe**: Same as mode 4 but triggered by a hardware signal on the gate pin instead of software.


##### Bit 0 — Number Format

```
0     →   binary 
1     →   BCD (Binray Coded Decimal) 
```

## Resource 
[https://wiki.osdev.org/Programmable_Interval_Timer]

