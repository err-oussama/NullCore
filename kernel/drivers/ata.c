#include "ata.h"
#include "pmio.h"
#include "types.h"
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

void ata_drive_setup() {
  outb(ATA_PRIMARY_CONTROL, ATA_CONTROL_nIEN);
  outb(ATA_PRIMARY_DRIVE_HEAD, ATA_DRIVE_MASTER);
  for (int i = 0; i < 4; i++)    // wait 400ns
    inb(ATA_PRIMARY_ALT_STATUS); // No meaning for the reading just to wait
                                 // 400ns
  if (inb(ATA_PRIMARY_STATUS) == 0xFF) {
    kprintf("Error No Drive Present");
    return;
  }
  while (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY)
    ;
  outb(ATA_PRIMARY_COMMAND, ATA_CMD_IDETIFY);
  for (int i = 0; i < 4; i++)    // wait 400ns
    inb(ATA_PRIMARY_ALT_STATUS); // No meaning for the reading just to wait
  uint8 status = inb(ATA_PRIMARY_STATUS);
  while ((status & ATA_STATUS_BSY) && !(status & ATA_STATUS_DRQ))
    status = inb(ATA_PRIMARY_STATUS);
  uint16 *buffer = (uint16 *)pmm_alloc();
  memset(buffer, 0, 0x1000);
  kprintf("Buffer: %p\n", buffer);
  for (int i = 0; i < ATA_SECTOR_WORDS; i++)
    buffer[i] = inw(ATA_PRIMARY_DATA);
  for (int i = 27; i < 46; i++) {
    kprintf("%u, ", buffer[i]);
  }
}
void ata_read_sector(void *buffer);
void ata_write_sector(void *buffer);
