#ifndef ELF_HEADER_MACRO_H
#define ELF_HEADER_MACRO_H

// IDENT array members
#define EI_MAG0 0x0
#define EI_MAG1 0x1
#define EI_MAG2 0x2
#define EI_MAG3 0x3
#define EI_CLASS 0x4 // Architecture? 32-bit or 64-bit
#define EI_DATA 0x5  // data encoding? Little-endian or Big-endian
#define EI_VERSION 0x6
#define EI_OSABI 0x7      // OS ABI? ARM or LINUX or STANDALONE or ...
#define EI_ABIVERSION 0x8 // ABI version
#define EI_PAD 0x9
#define EI_NIDENT 0xA // size of the elf_t array

// IDENT array values

#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASSNONE 0x0
#define ELFCLASS32 0x1
#define ELFCLASS64 0x2

#define ELFDATANONE 0x0
#define ELFDATA2LSB 0x1
#define ELFDATA2MSB 0x2

#define EV_NONE 0x0
#define EV_CURRENT 0x1

#define ELFOSABI_SYSV 0x0
#define ELFOSABI_LINUX 0x3

#define ET_NONE 0x0
#define ET_ERL 0x1
#define ET_EXEC 0x2
#define ET_DYN 0x3
#define ET_CORE 0x4

#endif
