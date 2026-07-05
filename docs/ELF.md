# Executable & Linkable Format 


## ELF Header 

The first structure in every ELF file. Contains metadata about the file itself:
its type (executable, shared library, object file), target architecture, and the entry point addres(if it's executable), the virtual address where execution begins.




## Program Header Table

Only relevent for executables and shared libraries, not object files.
Describes the **segments** the OS needs to load into memory at runtime (code, data, etc.).


- Lives immediately after the ELF header.
- Is an array of entries, each describing one segment.
- Location, entry size, and count are all stored in the ELF header:

    - `e_phoff`     → offset from the start of the file to the table.
    - `e_phentsize` → size of each entry in bytes
    - `e_phnum`     → number of entries in the table


## Section Header Table


Only relvent for linking and debugging, not needed for loading and executin.
Describes the **sections** inside the file (symbol tables, relocation data, debug info, etc.).

- Lives at the end of the file 
- Is an array of entries, each describing one section.
- Location, entry size, and count are all stored in the ELF header:

    - `e_shoff`     → offset from the start of the file to the table.
    - `e_shentsize` → size of each entry in bytes
    - `e_shnum`     → number of entries in the table


## Sections and Segments 


The actual binary content of the file, code, initialized data, uninitialized datam symbol tables, relocation entries, and debug information.
Section and Segment describe the same underlying data from two different perspectives.


## Resource

(https://dev.to/bytehackr/understanding-the-basics-of-elf-files-on-linux-61c)
[What is ELF](https://jumpcloud.com/it-index/what-is-elf-executable-and-linkable-format)
