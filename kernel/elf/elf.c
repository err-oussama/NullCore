#include "elf.h"
#include "elf_header_macro.h"
#include "types.h"
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>
#include <task.h>
#include <tss.h>
#include <vmm.h>

char *types_s[] = {"Unknown", "Relocatable", "Executable", "Shared object",
                   "Core"};
char *archs_s[] = {"Unknown", "32", "64"};
char *data_s[] = {"Unknown", "Littel indian", "Big indian"};

void print_Ehdr(elf_t *elf) {
  uint8 arch = elf->ident[EI_CLASS];
  uint8 data = elf->ident[EI_DATA];
  uint8 version = elf->ident[EI_VERSION];
  uint8 abi = elf->ident[EI_OSABI];
  uint8 pad = elf->ident[EI_PAD];

  if (elf->ident[EI_MAG0] != ELFMAG0 || elf->ident[EI_MAG1] != ELFMAG1 ||
      elf->ident[EI_MAG2] != ELFMAG2 || elf->ident[EI_MAG3] != ELFMAG3) {
    kprintf("[Warnning]: File not ELF file");
    return;
  }
  kprintf("Architecture                      : %s\n",
          arch < 3 ? archs_s[arch] : archs_s[0]);
  kprintf("Data                              : %s\n",
          data < 3 ? data_s[data] : data_s[0]);
  kprintf("ELF Version                       : %s\n",
          version == EV_CURRENT ? "Current" : "Invalid");
  kprintf("ABI                               : %s\n",
          abi == ELFOSABI_SYSV    ? "UNIX - System V"
          : abi == ELFOSABI_LINUX ? "LINUX ABI"
                                  : "Unsupported");
  kprintf("PAD                               : 0x%x\n", pad);
  kprintf("Identification array size         : %u\n", EI_NIDENT);
  kprintf("ELF type                          : %s\n",
          elf->type < 5 ? types_s[elf->type] : types_s[0]);

  kprint_str("Machine                           : ");
  if (elf->machine == EM_X86_64)
    kprint_str("AMD X86-64\n");
  else if (elf->machine == EM_AI_64)
    kprint_str("Intel Itanium\n");
  else if (elf->machine == EM_860)
    kprint_str("Intel 80860\n");
  else if (elf->machine == EM_386)
    kprint_str("Intel 80386\n");
  else
    kprint_str("Unknown\n");
  kprintf("File Version                      : 0x%x\n", elf->version);
  kprintf("Entry                             : 0x%x\n", elf->entry);
  kprintf("Start of program header           : %u\n", elf->phoff);
  kprintf("Start of section header           : %u\n", elf->shoff);
  kprintf("Flags                             : 0x%x\n", elf->flags);
  kprintf("Size of this header               : %u (bytes)\n", elf->ehsize);
  kprintf("Size of program header            : %u (bytes)\n", elf->phentsize);
  kprintf("Size of section header            : %u (bytes)\n", elf->shentsize);
  kprintf("Number of program header          : %u\n", elf->phnum);
  kprintf("Number of section header          : %u\n", elf->shnum);
  kprintf("Section header string table index : %u\n", elf->shstrndx);
}

void print_ph_n(elf_t *elf, uint32 n) {
  if (n >= elf->phnum)
    return;
  Elf32_Phdr *ph = (Elf32_Phdr *)(elf->phoff + ((char *)elf));
  kprintf("Type        : 0x%x\n", ph[n].type);
  kprintf("Offset      : 0x%x\n", ph[n].offset);
  kprintf("Vaddr       : 0x%x\n", ph[n].vaddr);
  kprintf("Paddr       : 0x%x\n", ph[n].paddr);
  kprintf("File size   : 0x%x\n", ph[n].filesz);
  kprintf("Memory size : 0x%x\n", ph[n].memsz);
  kprintf("Flags       : %c%c%c\n", ph[n].flags & PF_R ? 'R' : ' ',
          ph[n].flags & PF_W ? 'W' : ' ', ph[n].flags & PF_X ? 'X' : ' ');
  kprintf("Align       : 0x%x\n", ph[n].align);
}

void load_elf(void *buff) {
  elf_t *elf = buff;
  uint32 *pd = mmu_create_address_space();
  if (pd == NULL)
    return;
  uint32 flag = MMU_PTE_P | MMU_PTE_U_MODE;
  Elf32_Phdr *ph = (Elf32_Phdr *)(elf->phoff + ((char *)buff));
  for (int i = 0; i < elf->phnum; i++) {
    if (ph[i].type == PT_LOAD) {
      if (ph[i].flags & PF_W)
        flag |= MMU_PTE_RW;
      uint32 vadd_s = ph[i].vaddr;            // start of segment
      uint32 vadd_e = (vadd_s + ph[i].memsz); // end of segment

      mmu_map_page(pd, vadd_s, pmm_alloc(), flag);
      if ((vadd_s & -0xFFF) != (vadd_e & -0xFFF))
        mmu_map_page(pd, vadd_e, pmm_alloc(), flag);
    }
    flag = MMU_PTE_P | MMU_PTE_U_MODE;
  }
  uint32 stack = pmm_alloc();
  mmu_map_page(pd, stack, pmm_alloc(), MMU_PTE_P | MMU_PTE_U_MODE | MMU_PTE_RW);

  mmu_switch(pd);
  for (int i = 0; i < elf->phnum; i++) {
    if (ph[i].type == PT_LOAD) {
      void *vaddr = (void *)ph[i].vaddr;
      void *offset = (void *)((char *)(elf)) + ph[i].offset;
      uint32 filesz = ph[i].filesz;
      uint32 memsz = ph[i].memsz;
      memcpy(vaddr, offset, filesz);
      if (filesz < memsz)
        memset(vaddr + filesz, 0, memsz - filesz);
    }
  }
  deascalate((void *)elf->entry, stack + 0x1000);
}
