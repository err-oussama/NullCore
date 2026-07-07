#include "elf.h"
#include "elf_header_macro.h"
#include <kprint.h>
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
  kprintf("Architecture   : %s\n", arch == ELFCLASS32   ? "32"
                                   : arch == ELFCLASS64 ? "64"
                                                        : "Unknown");
  kprintf("Data           : %s\n", data == ELFDATA2LSB   ? "Littel indian"
                                   : data == ELFDATA2MSB ? "Big indian"
                                                         : "UNknown");
  kprintf("Version        : %s\n",
          version == EV_CURRENT ? "Current" : "Invalid");
  kprintf("ABI            : %s\n", abi == ELFOSABI_SYSV    ? "UNIX - System V"
                                   : abi == ELFOSABI_LINUX ? "LINUX ABI"
                                                           : "Unsupported");
  kprintf("PAD            : 0x%x\n", pad);
  kprintf("Ident size     : %u\n", EI_NIDENT);
}
