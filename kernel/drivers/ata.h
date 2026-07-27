#ifndef ATA_H
#define ATA_H
#include <types.h>
// Primary Channel (0x1F0 - 0x1F7, 0x3F6)
#define ATA_PRIMARY_ALT_STATUS 0x3F6
#define ATA_PRIMARY_CONTROL 0x3F6
#define ATA_PRIMARY_DATA 0x1F0
#define ATA_PRIMARY_ERROR 0x1F1
#define ATA_PRIMARY_FEATURES 0x1F1
#define ATA_PRIMARY_SECTOR_COUNT 0x1F2
#define ATA_PRIMARY_LBA_LOW 0x1F3
#define ATA_PRIMARY_LBA_MID 0x1F4
#define ATA_PRIMARY_LBA_HIGH 0x1F5
#define ATA_PRIMARY_DRIVE_HEAD 0x1F6
#define ATA_PRIMARY_COMMAND 0x1F7
#define ATA_PRIMARY_STATUS 0x1F7

// Secondary Channel (0x170 - 0x177, 0x376)
#define ATA_SECONDARY_ALT_STATUS 0x376
#define ATA_SECONDARY_CONTROL 0x376
#define ATA_SECONDARY_DATA 0x170
#define ATA_SECONDARY_ERROR 0x171
#define ATA_SECONDARY_FEATURES 0x171
#define ATA_SECONDARY_SECTOR_COUNT 0x172
#define ATA_SECONDARY_LBA_LOW 0x173
#define ATA_SECONDARY_LBA_MID 0x174
#define ATA_SECONDARY_LBA_HIGH 0x175
#define ATA_SECONDARY_DRIVE_HEAD 0x176
#define ATA_SECONDARY_COMMAND 0x177
#define ATA_SECONDARY_STATUS 0x177

// Error Register (0x1F1/0x171)
#define ATA_ERR_BBK 0x80   // Bad Block
#define ATA_ERR_UNC 0x40   // Unncorrectable Data Error
#define ATA_ERR_MC 0x20    // Media Changed
#define ATA_ERR_IDNF 0x10  // ID Not Found
#define ATA_ERR_MCR 0x08   // Media hange Requested
#define ATA_ERR_ABRT 0x04  // Command Abroted
#define ATA_ERR_TK0NF 0x02 // Track 0 Not Found
#define ATA_ERR_AMNF 0x01  // Address Mark Not Found

// Status Register (0x1F7/0x177)
#define ATA_STATUS_BSY 0x80  // Busy
#define ATA_STATUS_DRDY 0x40 // Drive Ready
#define ATA_STATUS_DF 0x20   // Drive Fault
#define ATA_STATUS_SRV 0x10  // Overlapped Mode Service Request
#define ATA_STATUS_DRQ 0x08  // Data Request
#define ATA_STATUS_CORR 0x04 // Corrected Data
#define ATA_STATUS_IDX 0x02  // Index
#define ATA_STATUS_ERR 0x01  // Error

#define ATA_NO_DRIVE 0xFF

// Drive/Head Register (0x1F6/0x176)
#define ATA_DRIVE_MASTER 0xA0
#define ATA_DRIVE_SLAVE 0xB0
#define ATA_DRIVE_LBA 0x40

// Device Control Register (0x3F6/0x376)
#define ATA_CONTROL_SRST 0x04 // Software Reset
#define ATA_CONTROL_nIEN 0x02 // Interrupt Disable

// Command Register (0x1F7/0x177)
#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_FLUSH_CACHE 0xE7

#define ATA_SECTOR_SIZE 0x200  // 512 byte
#define ATA_SECTOR_WORDS 0x100 // 16-bit * 256

int ata_drive_setup();
int ata_identify(uint16 *buffer);
int ata_read_sector(uint32 lba, uint8 sector_count, void *buffer);
int ata_write_sector(uint32 lba, uint8 sector_count, void *buffer);

#endif
