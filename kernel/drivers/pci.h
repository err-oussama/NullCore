#ifndef PCI_H
#define PCI_H

#include <types.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

typedef struct __attribute__((packed)) {
  uint32 address;
  uint32 size;
  uint8 is_io_space;
  uint8 prefetchable;
} pci_bar_t;

typedef struct __attribute__((packed)) {

  uint8 bus;
  uint8 device;
  uint8 function;
  uint8 header_type;

  uint16 vendor_id;
  uint16 device_id;

  uint8 revision_id;
  uint8 prog_if;
  uint8 subclass;
  uint8 class_code;

  uint8 interrupt_line;
  uint8 interrupt_pin;

  pci_bar_t bars[6];
} pci_device_t;

void pci_setup();
void pci_print_device(int32 dev);
pci_device_t *pci_find_device(uint16 vendor_id, uint16 device_id);
void nic_handler();

#endif
