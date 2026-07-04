#ifndef ELF_H
#define ELF_H

#include <types.h>

/*
 *  EI  		=> ELF Identification
 *  NIDENT 	=> Number of Identification Bytes
 *
 *
 */
#define EI_NIDENT 0x10

typedef struct elf_s elf_t;

struct elf_s {
  uint8 ident[EI_NIDENT];
  uint16 type;
  uint16 machine;
  uint16 version;
  uint32 entry;
  uint32 phoff; // program header offset
  uint32 shoff; // segment header offset
  uint32 flags;
  uint16 ehsize;
  uint16 phentsize;
  uint16 phnum;
  uint16 shentsize;
  uint16 shnum;
  uint16 shstrndx;
};

#endif
