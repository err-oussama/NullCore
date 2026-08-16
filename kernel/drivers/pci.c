#include "types.h"
#include <kprint.h>
#include <kstring.h>
#include <pci.h>
#include <pmio.h>

#define PCI_MAX_DEVICE 20

static uint32 pci_device_index = 0;
static pci_device_t pci_devices[PCI_MAX_DEVICE];

uint32 pci_read_register(uint8 bus, uint8 device, uint8 function, uint8 reg) {

  uint32 address = 0x80000000;
  address |= (bus << 16);
  address |= (device << 11);
  address |= (function << 8);
  address |= (reg << 2);

  outdw(PCI_CONFIG_ADDRESS, address);

  return indw(PCI_CONFIG_DATA);
};
void pci_write_register(uint8 bus, uint8 device, uint8 function, uint8 reg,
                        uint32 value) {

  uint32 address = 0x80000000;
  address |= (bus << 16);
  address |= (device << 11);
  address |= (function << 8);
  address |= (reg << 2);

  outdw(PCI_CONFIG_ADDRESS, address);

  outdw(PCI_CONFIG_DATA, value);
};

uint8 pci_read_offset(uint8 bus, uint8 device, uint8 function, uint8 offset) {

  uint32 reg = pci_read_register(bus, device, function, offset / 4);
  reg >>= (offset % 4) * 8;
  reg &= 0xFF;
  return reg;
}

void pci_enumeration() {

  for (uint32 bus = 0; bus <= 255; bus++) {
    for (uint32 device = 0; device <= 31; device++) {
      uint32 ids = pci_read_register(bus, device, 0, 0);
      if (ids == 0xFFFFFFFF)
        continue;
      for (uint32 function = 0; function <= 7; function++) {
        if (function)
          ids = pci_read_register(bus, device, function, 0);
        if (ids != 0xFFFFFFFF) {
          pci_device_t *pci_dev = &pci_devices[pci_device_index];
          pci_dev->bus = bus;
          pci_dev->device = device;
          pci_dev->function = function;
          *(uint32 *)(&pci_dev->vendor_id) = ids;
          *(uint32 *)(&pci_dev->revision_id) =
              pci_read_register(bus, device, function, 0x2);
          pci_dev->header_type = pci_read_offset(bus, device, function, 0xE);
          *(uint16 *)(&pci_dev->interrupt_line) =
              pci_read_register(bus, device, function, 0xF);

          for (uint8 i = 0; i < 6; i++) {
            uint32 bar =
                pci_read_register(bus, device, function, 0x10 + (i * 4));

            pci_write_register(bus, device, function, 0x10 + (i * 4),
                               0xFFFFFFFF);

            uint32 bar_size_mask =
                pci_read_register(bus, device, function, 0x10 + (i * 4));
            pci_write_register(bus, device, function, 0x10 + (i * 4), bar);

            pci_dev->bars[i].size = (~bar_size_mask) + 1;
            pci_dev->bars[i].is_io_space = bar & 0x1;
            if (!pci_dev->bars[i].is_io_space) {
              pci_dev->bars[i].address = bar & 0xFFFFFFF0;
              pci_dev->bars[i].prefetchable = bar & 0x8 ? 1 : 0;
            } else {
              pci_dev->bars[i].address = bar & 0xFFFFFFFC;
            }
          }
          pci_device_index++;
          if (!function && !(pci_dev->header_type & 0x80))
            break;
        }
      }
    }
  }
}

void pci_setup() {
  memset(pci_devices, 0, sizeof(pci_device_t) * PCI_MAX_DEVICE);
}
