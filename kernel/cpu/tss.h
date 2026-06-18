#ifndef TSS_H
#define TSS_H
#include <type.h>

typedef struct __attribute__((packed)) {
  uint16 previous_task_link;
  uint16 reserved;
  uint32 esp0;
  uint16 ss0;
  uint16 ss0_reserved;
  uint32 esp1;
  uint16 ss1;
  uint16 ss1_reserved;
  uint32 esp2;
  uint16 ss2;
  uint16 ss2_reserved;
  uint32 cr3;
  uint32 eip;
  uint32 eflags;
  uint32 eax;
  uint32 ecx;
  uint32 edx;
  uint32 ebx;
  uint32 esp;
  uint32 ebp;
  uint32 esi;
  uint32 edi;
  uint16 es;
  uint16 es_reserved;
  uint16 cs;
  uint16 cs_reserved;
  uint16 ss;
  uint16 ss_reserved;
  uint16 ds;
  uint16 ds_reserved;
  uint16 fs;
  uint16 fs_reserved;
  uint16 gs;
  uint16 gs_reserved;
  uint16 LDT_selector;
  uint16 LDT_reserved;
  uint16 trap;
  uint16 iobp_offset;
} tss;

#endif
