#ifndef ELF_HEADER_MACRO_H
#define ELF_HEADER_MACRO_H

// IDENT array
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

#endif
