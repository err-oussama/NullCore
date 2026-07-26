#include "ata.h"
#include "pmio.h"
#include "types.h"
#include <kprint.h>
#include <kstring.h>
#include <pmm.h>

void ata_wait() {
  // wait after writing in one of these three registers:
  // 1. DRIVE_HEAD
  // 2. CONTROL
  // 3. COMMAND
  for (int i = 0; i < 4; i++) // wait 400ns
    inb(ATA_PRIMARY_CONTROL); // No meaning for the reading just to wait
}

void ata_drive_setup() {
  outb(ATA_PRIMARY_CONTROL, ATA_CONTROL_nIEN);
  outb(ATA_PRIMARY_CONTROL, ATA_CONTROL_SRST);
  ata_wait();
  outb(ATA_PRIMARY_CONTROL, ATA_CONTROL_nIEN);
  while (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY)
    ;
  if (inb(ATA_PRIMARY_STATUS) == ATA_NO_DRIVE) {
    kprintf("Error No Drive Present");
    return;
  }
  outb(ATA_PRIMARY_DRIVE_HEAD, ATA_DRIVE_MASTER);
  ata_wait();
  while (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY)
    ;
}
void ata_identify(uint16 *buffer) {
  outb(ATA_PRIMARY_DRIVE_HEAD, ATA_DRIVE_MASTER);
  ata_wait();
  while (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY)
    ;
  outb(ATA_PRIMARY_SECTOR_COUNT, 0x0);
  outb(ATA_PRIMARY_LBA_LOW, 0x0);
  outb(ATA_PRIMARY_LBA_MID, 0x0);
  outb(ATA_PRIMARY_LBA_HIGH, 0x0);

  outb(ATA_PRIMARY_COMMAND, ATA_CMD_IDENTIFY);
  ata_wait();
  while (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY)
    ;

  if (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_ERR) {
    kprintf("Error No Drive Found\n");
    return;
  }
  while (!(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_DRQ))
    ;

  memset(buffer, 0, 0x1000);
  for (int i = 0; i < ATA_SECTOR_WORDS; i++) {
    buffer[i] = inw(ATA_PRIMARY_DATA);
  }
  for (int i = 27; i < 46; i++) {
    kprintf("%c%c", (buffer[i] >> 8) & 0x00FF, buffer[i] & 0x00FF);
  }
  kprintf("\n");

  uint32 total_sectors = *(uint32 *)&buffer[60];
  kprintf("Total sector: %u\n", total_sectors);
  kprintf("Disk size: %uM\n", (total_sectors * 512) / (1024 * 1024));
}
void ata_read_sector(uint32 lba, uint8 sector_count, void *buffer);
void ata_write_sector(uint32 lba, uint8 sector_count, void *buffer);
