#include <ata.h>
#include <fs.h>

static disk_t disk;

void init_disk() {
  disk.sector_count = ata_sector_count();
  disk.sector_size = ATA_SECTOR_SIZE;
}
