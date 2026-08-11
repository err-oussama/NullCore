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
      for (uint32 function = 0; function <= 7; function++) {
        uint32 ids = pci_read_register(bus, device, function, 0);
        uint8 class_code = pci_read_offset(bus, device, function, 0xB);
        if (ids != 0xFFFFFFFF) {
          kprintf("Bus: 0x%x, ", bus);
          kprintf("Device : 0x%x, ", device);
          kprintf("Function : 0x%x, ", function);
          kprintf("Register 0: 0x%x, ", ids);
          kprintf("Class Code: 0x%x", class_code);
          kprintf("\n");
        }
      }
    }
  }
}
