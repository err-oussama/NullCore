#include "types.h"
#include <kprint.h>
#include <kstring.h>
#include <pci.h>
#include <pic.h>
#include <pmio.h>

#define PCI_MAX_DEVICE 20

static uint32 pci_device_index = 0;
static pci_device_t pci_devices[PCI_MAX_DEVICE];

uint32 pci_make_config_address(uint8 bus, uint8 device, uint8 function,
                               uint8 reg) {
  uint32 address = 0x80000000;
  address |= (bus << 16);
  address |= (device << 11);
  address |= (function << 8);
  address |= (reg << 2);
  return address;
}

uint32 pci_read_register(uint8 bus, uint8 device, uint8 function, uint8 reg) {
  outdw(PCI_CONFIG_ADDRESS,
        pci_make_config_address(bus, device, function, reg));
  return indw(PCI_CONFIG_DATA);
};
void pci_write_register(uint8 bus, uint8 device, uint8 function, uint8 reg,
                        uint32 value) {
  outdw(PCI_CONFIG_ADDRESS,
        pci_make_config_address(bus, device, function, reg));
  outdw(PCI_CONFIG_DATA, value);
};

uint8 pci_read_offset(uint8 bus, uint8 device, uint8 function, uint8 offset) {

  uint32 reg = pci_read_register(bus, device, function, offset / 0x4);
  reg >>= (offset % 0x4) * 0x8;
  reg &= 0xFF;
  return reg;
}

void pci_setup_bars(uint8 bus, uint8 device, uint8 function,
                    pci_device_t *pci_dev) {

  for (uint8 i = 0; i < 6; i++) {
    uint32 bar = pci_read_register(bus, device, function, 0x10 + (i * 4));

    pci_write_register(bus, device, function, 0x10 + (i * 4), 0xFFFFFFFF);

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
}

void pci_enumeration() {

  for (uint32 bus = 0; bus <= 255; bus++) {
    for (uint32 device = 0; device <= 31; device++) {
      uint32 ids = pci_read_register(bus, device, 0x0, 0x0);
      if (ids == 0xFFFFFFFF)
        continue;
      for (uint32 function = 0; function <= 7; function++) {
        if (function)
          ids = pci_read_register(bus, device, function, 0x0);
        if (ids != 0xFFFFFFFF) {
          pci_device_t *pci_dev = &pci_devices[pci_device_index];
          uint32 reg_0x2 = pci_read_register(bus, device, function, 0x2);
          uint32 reg_0x3 = pci_read_register(bus, device, function, 0x3);
          uint32 reg_0xF = pci_read_register(bus, device, function, 0xF);

          pci_dev->bus = bus;
          pci_dev->device = device;
          pci_dev->function = function;

          pci_dev->vendor_id = ids & 0xFFFF;
          pci_dev->device_id = ids >> 0x10;

          pci_dev->revision_id = reg_0x2 & 0xFF;
          pci_dev->prog_if = (reg_0x2 >> 0x8) & 0xFF;
          pci_dev->subclass = (reg_0x2 >> 0x10) & 0xFF;
          pci_dev->class_code = (reg_0x2 >> 0x18) & 0xFF;

          pci_dev->header_type = (reg_0x3 >> 0x10) & 0xFF;

          pci_dev->interrupt_line = reg_0xF & 0xFF;
          pci_dev->interrupt_pin = (reg_0xF >> 0x8) & 0xFF;

          pci_setup_bars(bus, device, function, pci_dev);
          pci_device_index++;
          if (!function && !(pci_dev->header_type & 0x80))
            break;
        }
      }
    }
  }
}

void pci_print_device(int32 n) {
  uint32 start = n == -1 ? 0 : n;
  uint32 end = n == -1 ? pci_device_index : n + 1;
  for (uint32 i = start; i < end; i++) {
    pci_device_t *dev = &pci_devices[i];
    kprintf("Bus: 0x%x, Dev: 0x%x, Fun: 0x%x :: ", dev->bus, dev->device,
            dev->function);
    kprintf("Ven ID: 0x%x, Dev ID: 0x%x :: ", dev->vendor_id, dev->device_id);
    kprintf("Class Code: 0x%x, Int Line: 0x%x, Int Pin 0x%x\n", dev->class_code,
            dev->interrupt_line, dev->interrupt_pin);
  }
}

void pci_setup() {
  memset(pci_devices, 0, sizeof(pci_device_t) * PCI_MAX_DEVICE);
  pci_enumeration();
}

void nic_handler() {
  pic_send_eoi(0xB);
  return;
};
