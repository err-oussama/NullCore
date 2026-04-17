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

### Flage byte 


|*Bits* |       *Name*          |                       *Role*                          |*Value*                                                    |
|-------|-----------------------|-------------------------------------------------------|-----------------------------------------------------------|
|  52   |Available for OS       |Available for OS                                       |0-1 freely set by OS                                       |
|  53   |Long Mode              |Indicates whether this is 64-bit code segment or not   |0=32-bit segment \ 1=64-bit segment                        |
|  54   |Default operation Size |Defines the default size of operations and addresses   |0=16-bit segment \ 1=32-bit segment                        |
|  55   |Granularity            |Defines the unit of measurement for the segment limit  |0=Limit is in bytes (Max 1MB) \ 1=Limit is in 4KB (Max 4GB)|



### Access Byte 

|*Bits* |       *Name*          |                           *Role*                                      |                                   *Value*                                     |
|-------|-----------------------|-----------------------------------------------------------------------|-------------------------------------------------------------------------------|
|   0   |Accessed               |Determine if the CPU accessed the Segment                              |0=Never Accessed \ 1=Accessed                                                  |
|   1   |Writeable/Readable     |For Code segments: readbale or not\For Data segment: writebale or not  |0=code:Execute-only,data:read-only \ 1=code:Read/Execute,data:Read/Write       |
|   2   |Conforming/Direction   |Code:privilege behavior \ Data: growth direction                       |0=code:Non-Conforming,data:Grows upward \ 1=code:Conforming,data:Grows downward|
|   3   |Executable             |Defines whether this is a code or a data segment                       |0=Data Segment \ 1=Code Segment                                                |
|   4   |Description type       |Distinguishes between system and code/data descriptors                 |0=Kernel Data \ 1=Code & Data Segment                                          |
|  5-6  |DPL                    |Minimum Privilege level required to access this segment                |Ring 0-3                                                                       |
|   7   |Present                |Marks whether the segment is present in memory                         |0=Absent, Raise General Protextion Fault \ 1=Present                           |


## Global Descriptor Table Register (GDTR)
    
The GDTR is a special hidden CPU register that holds two fields:
- *Base Address* (32 bit in 32-bit mode and 64 bit in 64-bit mode)
        pointing to where the GDT lives in memory;
- *limit* (16 bit size)that tells the CPU the exat byte boundry of the GDT 

the CPU consults it every time it needs to resolve a segment secletor (in CS,DS,
SS,etc) into an actual segment descriptor.


## Load Global Descriptor Table  (LGDT) 
is a privileged ring-0-only instruction that takes a (6-byte (48-bit)memory operand in 32-bit mode, or a 10-byte (80-bit) operand in 64-bit mode) laid out as a 16-bit limit followd by a 32/64-bit base address -- and writes both fields directly into the GDTR. 

it does not reload segment registers(CS,SS,DS,etc) meaning you must manully reload them afterward with valid selectors from the new GDT for the change to take effect.
It is typically called once during boot when the OS swithes from real mode to protected/long mode to install it onw GDT  

lgdt [gdt_descriptor]            ; write gdt_descriptor content into GDTR

## Store Global Descriptor Table (SGDT)
it reads the current GDTR and dumps its content into a 6-byte memory operand in 32-bit mode or a 10-byte memory operand in 64-bit mode. unlike LGDT it is not privileged and can be executed from any ring including user space, which makes it a historical secutity concern as it leaks the kernel's GDT base address

sgdt [gdtr_copy]            ; dump current GDTR into gdtr_copy


## GDT entry 

### 0 NULL descriptor (8byte)



### 1 Kernel Code descriptor (8byte)
    used by CS (ring 0 execution)


The **Kernel Code Descriptor** is an 8-byte GDT entry loaded into the `CS` register to enable Ring 0 execution. Think of it as hardware-enforced supervisor pass for the CPU.

- **The Authority**: It grants the instruction pointer (`RIP`/`EIP`) permission to run privileged instruction (like `hlt`or `wrmsr`) and bypass user-space memory checks.

- **The Reality**: Because modern OS use a flat memory model, the segment's base and limit are completely ignored by the hardware.


- **The Real Job**: Pure state configuration. It exists soley to tell the CPU two things: run at highest privilege(`DPL=0`) and use a specific operating mode (32-bit or 64-bit).


 

### 2 Kernel Data descriptor (8byte)
    used by SS/DS (ring 0 stack + data)

The **Kernel Data Descriptor** is an 8-byte loaded into `SS`(and historically `DS`/`ES`) to define the memory regin the kernel reads and writes.

- **The authority**: It grants the stack pointer (`RSP`, `ESP`) and memory operations permision to access kernel memory -- the CPU checks `SS` DPL against CPL on every push, pop and stack-relative access, refusing the operation with `#GP` if privilege doesn't match.

- **The Reality**: Because modern OS use flat memory model, the segment's base and limit are completely ignored by the hardware -- in 64-bit mode the CPU hardwires the effective base to 0 and the limit to the full address space for `SS`, `DS`, and `ES` regardless of what the descriptor says.


- **The Real Job**: Pure privilge anchoring. It exists solely to tell the CPU one thing: all stack and data memory operations at this descriptor's DPL are authorized at Ring 0 -- which is why it is structurally identical to the kernel code descriptor in every field except `E=0`(non-executable), the single bit that makes it a data segment instead of code segment.



### 3 User Data descriptor (8byte)
    used by SS/DS (ring 3 stack + data)

The **User Data Descriptor** is an 8-byte GDT entry loaded into `SS` (and historicall `DS`/`ES`) to define the privilege level under which user-space stack and data memory operations execute.


- **The Authority**: It restrics all stack and data memory to Ring 3 -- the CPU checks `SS` DPL against CPL on every push, pop, and stack-relative access, and if a user-space program attempts to load a kernel-level segment selector into `SS`, the CPU refuses it with `#GP` because DPL=3 of this descriptor cannot be overridden by an unprivileged caller.

- **The Reality**: Because modern OS use a flat memory model, the segment's base and limit are completely ignored by the hardware -- in 64-bit mode the CPU hardwires the effective base to 0 and the limit to the full address space for `SS`, `DS`, and `ES` regardless of what the descriptor says, meaning actual user-space memory isolation is enforced entirely by the U/S bits in the page tables, not by segment boundaries.

- **The Real Job**: Pure privilege mirroring. It exists solely to be the data-side counterpart to the User Code Descriptor -- structurally identical in every field except `E=0`(non-executable), with DPL=3 anchoring all stack operations at Ring 3, which is why SYSRET loads it into `SS` simulaneously with loading the User Code Descriptor into `CS`, completing the full privilege state swith back to user-space in a single atomic transition.


### 4 User Code descriptor (8byte) 
    used by CS (ring 3 execution)

The **User Code Descriptor**: is an 8-byte GDT entry loaded into `CS` to enable Ring 3 execution, defining the privilege boundary that sepcarates user-space processes from the kernel.

- **The Authority**: It restricts `RIP` to unprivileged execution -- any attempt to execute a privileged instruction (`hlt`, `wrmsr`, `cli`, etc) while CS points to this descriptor triggers a `#GP` immediately, because the CPU compares CLP (cs{1:0} = 3) against the instruction's required privilege level and refuses it.


- **The Reality**: Because modern OS uses flat memory model, the segment's base and limit are completely ignored by the hardware -- in 64-bit mode the CPU hardwires the effective base to 0 and the limit to the full address space, meaning the isolation between user and kernell is enforced entirely by paging(CR3, U/S bits in the page table entries) and CPL, not by segmentation.

- **The Real Job**: Pure privilege declaration. It exists solely to tell the CPU two things: execute at lowest privilege (DPL=3) and use 64-bit mode(L=1, D=0) -- and critically, its selector `0x0023` carries RPL=3 in bits{1:0}, which is what forces CPL to 3 the moment SYSRET loads it into CS, makeing the hardware instantly aware that the next instruction fetch is no longer trusted kernel code.


### TSS descriptor (16byte) 
    used by TR (Task Register)

The **TSS Descriptor** is a *16-byte* GDT entry (not 8-byte like the others) that points to the Task State Segment structure in memory, giving the CPU a fixed location to find the kernel stack pointer during a privilege transition.

- **The Authority**: IT is the CPU's mandatory hardware contract for ring transitions -- the moment an interrup or a syscall fires and the CPU needs to swith from Ring 3 to Ring 0, it reads `RSP0` directly out of the TSS that this descriptor points to, loading it as the new kernel stack pointer before single byte of kernel code executes. There is no software involvement in this step; the CPU does it autonomously in microcode. 


- **The Reality**: Unlike the four flat segment descriptor, the TSS descriptor is no a fiction the kernel maintains for foramlity  -- its base address field is full live and must contain the exact virual address of the kernel's `tss_struct`, because the CPU dereference it on every single ring transation. Getting this address wrong does not produce a clean fault; It sends the CPU stack pointer into garbage memory and the system dies silently.

- **The Real Job**: It is a hardware pointer, not a privilege declaration. It exists solely to answer one question the CPU asks at the start of every privilege elevation: *where is the kernel stack for this CPU?* which is why it is per-CPU (each logical CPU has its own TSS and its own TSS descriptor in its own GDT), why it must be loaded explicitly with `LTR` before interrupts are ever enabled, and why its system descriptor type `0x9`(64-bit Available TSS) is the one value in the Type field that makes the CPU treat the descriptor as apointer to a live hardware structure rather then a flat segment definition.


