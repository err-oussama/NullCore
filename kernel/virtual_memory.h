#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include "type.h"

#define MMU_PTE_P 0x001      // present  			0x0 absent
#define MMU_PTE_RW 0x002     // read/write  	0x0 read-only
#define MMU_PTE_U_MODE 0x004 // user mode			0x0 kernel mode
#define MMU_PTE_PWT 0x008    // write through	0x0 write back
#define MMU_PTE_PCD 0x010    // disable				0x0 enabled
#define MMU_PTE_A 0x020      // Accessed  		0x0 never accessed
#define MMU_PTE_D 0x040      // write happend 0x0 no write
#define MMU_PTE_PAT 0x080    //
#define MMU_PTE_G 0x100      // remain  			0x0 Evicted

#define MMU_PDE_P 0x001
#define MMU_PDE_RW 0x002
#define MMU_PDE_U_MODE 0x004
#define MMU_PDE_PWT 0x008
#define MMU_PDE_PCD 0x010
#define MMU_PDE_A 0x020
// no dirty bit
#define MMU_PDE_PS 0x080 // 4MB 	0x0 4KB
#define MMU_PDE_G 0x100

typedef uint32 PTE;
typedef uint32 PDE;

uint32 mmu_make_entry(uint32 frame_address, uint16 flags);

#endif
