# Global Descriptor Table 

## Definition

- A system-wide array of 8-bytes entries called ***Segment Descriptors***, each describing a memory segment by defining its *base address* (where it start), *limit* (how large it is), privilege level (ring 0-3), and type (code, data, or system). It is shared across all processes, set up once by the OS at boot, and located by the CPY via `Global Descriptor Table Register (GDTR)`.
- The first entry is entry is always a NULL descriptor (all zeros) acting as a sefety quard. The GDT typically holds desciprtors for the kernel code segment, kernel data segment, user code segment, user dat segment, and TSS.
- Programs access these segments descriptor via a selector --  a 16-bit value containing an index into the GDT, a privilege level and a table indicator bit .
- In modern 64-bit OS, the GDT's role in address translation is obsolete since all segment base are forced to be 0 and paging handles memory management, however the GDT remains required because the CPU still depends on it for privilege level enforcement and the TSS.


## GDT Entry format

|*Bytes*|*Bits* |       *Name*          |               *Role*                  |
|-------|-------|-----------------------|---------------------------------------|
|0-1    | 0-15  | Segment Limit-low     | Lowset part of Segment Limit 0-15     |
|2-3    |16-31  | Base Address-low      | Lowset part of Base Address 0-15      |
|4      |32-39  | Base Address-Middle   | Middle part of Base Address 16-31     |
|5      |40-47  | Access Byte           | Access info for segment               |
|6      |48-51  | Segment Limit-high    | Highest part of Segment Limit 16-19   |
|6      |52-55  | Flags                 | Info about Segment                    |
|7      |56-63  | Base Address-high     | Highest part of Base Address 24-31    |






## Instruction 

## Global Descriptor Table Register (GDTR)
    
The GDTR is a special hidden CPU register that holds two fields:
- *Base Address* (32 bit in 32-bit mode and 64 bit in 64-bit mode)
        pointing to where the GDT lives in memory;
- *limit* (16 bit size)that tells the CPU the exat byte boundry of the GDT 

the CPU consults it every time it needs to resolve a segment secletor (in CS,DS,
SS,etc) into an actual segment descriptor.


## Load Global Descriptor Table  (LGDT) 
is a privileged ring-0-only instruction that takes a (6-byte (48-bit)memory 
operand in 32-bit mode, or a 10-byte (80-bit) operand in 64-bit mode)
laid out as a 16-bit limit followd by a 32/64-bit base address -- and writes 
both fields directly into the GDTR. 

it does not reload segment registers(CS,SS,DS,etc) meaning you must manully reload
them afterward with valid selectors from the new GDT for the change to take 
effect.
It is typically called once during boot when the OS swithes from real mode 
to protected/long mode to install it onw GDT  

lgdt [gdt_descriptor]            ; write gdt_descriptor content into GDTR

## Store Global Descriptor Table (SGDT)
it reads the current GDTR and dumps its content into a 6-byte memory operand 
in 32-bit mode or a 10-byte memory operand in 64-bit mode. 
unlike LGDT it is not privileged and can be executed from any ring including user
space, which makes it a historical secutity concern as it leaks the kernel's 
GDT base address

sgdt [gdtr_copy]            ; dump current GDTR into gdtr_copy




# LDT 

A pre-process private version of the GDT, using the exact same 8-byte descriptor format. Unlike the GDT which is shared system-wide, each process can have its own LDT for private segment desciptors. The CPU locates it via the LDTR (Local Descriptor Table Register) register, which holds a selector pointing to the LDT's own descriptor inside the GDT. Almost no modern OS uses it as a paging has completely replaced its purpose.
