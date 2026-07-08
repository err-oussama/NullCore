#include "elf.h"
#include "elf_header_macro.h"
#include <kprint.h>

char *types_s[] = {"Unknown", "Relocatable", "Executable", "Shared object",
                   "Core"};
char *archs_s[] = {"Unknown", "32", "64"};
char *data_s[] = {"Unknown", "Littel indian", "Big indian"};

void print_Ehdr(void *buff) {
  elf_t *elf = (elf_t *)initrd_start;
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
