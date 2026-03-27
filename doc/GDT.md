# Global Descriptor Table 


# Instruction 

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

