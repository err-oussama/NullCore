
## Code Segment - CS 


The `CS` register is a 16-bit segment selector register -- not an address, but a packed value encoding a 13-bit index into the GDT or LDT, a Table indicator bit, and a Privilege Level -- that tells the CPU which descriptor entry governs the current executing code; the CPU enforces at the architectural level that the resolved descriptor must be a code segment descriptor, rejecting any selector that resolves to a data segment, system segment, or non-executable descriptor wich a #GP(selector) fault.




|*Bits* |  *Name*   |       *Role*              |   *Value*     |
|-------|-----------|---------------------------|---------------|
| 0-1   |PL         |privilege level            |0-3            |
|  2    |TI         |Table indicator            |0=GDT \ 1=LDT  |
| 3-15  |GDT index  |Index                      |index          |


The CPU reads CS constantly in the background. Every instruction it executes, every memory address it makes, every privilege check if performs -- all of them start by looking at CS to understand the current execution context.



### The Three fields explained**

***Index (bits 15-3) -- 13 bits** This is the actual GDT slot number. 13 bits means you can have up to 8129 entries in your GDT. When the CPU needs to know "Who am i right now", it takes this index, multiplies by 8 (size of a GDT entry), and reads that descriptor from the GDT base.

***TI (bit 2) -- 1 bit*** Table Indicator. `0`=look in the GDT,`1`=look in the LDT (Local Descriptor Table -- almost nobody uses this in modern kernels, you'll always have this as `0`.


***PL (bits 1-0) -- 2 bits -- Privilege Level*** This is the most important field and the source of the most confusion -- because these same two bits have two different names depending on the moment in time you are talking about 

***RPL/CPL*** -- same bits, two names, two moments

- ***RPL - Requested Privilege Level***: is what these bits are called when you write a selector value as an operand in an instruction:

`mov cs, 0x08   <- 0x08 is the selector, bits 0-1 = RPL`

At this moment you are *requesting* to load a descriptor at a certin privilege level. It is a claim, not yet a fact. The CPU has not accepted it yet

- ***CPL - Current Privilege Level***: is what these same bits are called after the CPU has validated and accepted the load, and CS actually holds the value. Now it is a *fact* -- it defines your current execution privilege. Every other mechanism in the CPU reads CPL from CS to make its own privilege decisions

```
you write 0x08 as operand   -> bits 0-1 = RPL (a request, a claim)
CPU accepts, CS = 0x08      -> bits 0-1 = CPL (a fact, your identity)
```

The two names exist because they describe the same bits at two different points in time. One is what you are asking for. The other is what the CPU confirmed you are.

##  Read / Write CS

To read from the `CS` register you just do like any other reading operation on register:
    
    mov ax, cs;

But to change the value of `CS` you don't just do like any other writting opertion, you should performe something called: ***FAR JUMP***

**What is 'FAR JUMP'**?

`FAR JMP`: is a single CPU instruction that automically loads a 'new selector' into 'CS' and a 'new address' into 'RIP', triggering the full GDT lookup descriptor cache reload for CS in the process.
    
    jmp new_selector:new_address;

