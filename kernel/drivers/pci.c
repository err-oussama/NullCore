#include "types.h"
#include <pci.h>
#include <pmio.h>

#include <kprint.h>

uint32 pci_read_register(uint8 bus, uint8 device, uint8 function, uint8 reg) {

  uint32 address = 0x80000000;
  address |= (bus << 16);
  address |= (device << 11);
  address |= (function << 8);
  address |= (reg << 2);

  outdw(PCI_CONFIG_ADDRESS, address);

  return indw(PCI_CONFIG_DATA);
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
          uint8 class_code = pci_read_offset(bus, device, function, 0xB);
          uint8 header_type = pci_read_offset(bus, device, function, 0xE);
          kprintf("Bus: 0x%x, ", bus);
          kprintf("Dev: 0x%x, ", device);
          kprintf("Func: 0x%x, ", function);
          kprintf("VendID: 0x%x, ", ids & 0xFFFF);
          kprintf("DevID: 0x%x, ", ids >> 0x10);
          kprintf("Class Code: 0x%x", class_code);
          kprintf("\n");
          if (!function && !(header_type & 0x80))
            break;
        }
      }
    }
  }
}
