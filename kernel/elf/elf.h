#ifndef ELF_H
#define ELF_H

#include "elf_header_macro.h"
#include <types.h>

typedef struct elf_s elf_t;
typedef struct Elf32_Phdr_s Elf32_Phdr;

extern uint8 initrd_start[];
extern uint8 initrd_end[];

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
struct Elf32_Phdr_s {
  uint32 type;
  uint32 offset;
  uint32 vaddr;
  uint32 paddr;
  uint32 filesz;
  uint32 memsz;
  uint32 flags;
  uint32 align;
};

void print_Ehdr(elf_t *elf);
void load_elf(void *buff);

#endif
