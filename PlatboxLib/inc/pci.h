#pragma once
#include "global.h"

#define PCI_BAR0_OFFSET 0x10
#define PCI_BAR1_OFFSET 0x14
#define PCI_BAR2_OFFSET 0x18
#define PCI_BAR3_OFFSET 0x1C
#define PCI_BAR4_OFFSET 0x20


#define PCI_COMMAND_OFFSET 0x04
#define MEMORY_SPACE_BIT 1
#define BUS_MASTER_BIT 2

// Functions
void read_pci_header(UINT8 bus, UINT8 device, UINT8 function);
BYTE read_pci_byte(UINT8 bus, UINT8 device, UINT8 function, UINT offset);
WORD read_pci_word(UINT8 bus, UINT8 device, UINT8 function, UINT offset);
DWORD read_pci_dword(UINT8 bus, UINT8 device, UINT8 function, UINT offset);
void write_pci_byte(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value);
void write_pci_word(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value);
void write_pci_dword(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value);
void read_pci_bar_info(UINT8 bus, UINT8 device, UINT8 function, UINT offset);


void pci_enable_memory_space(UINT8 bus, UINT8 device, UINT8 function);
void pci_disable_memory_space(UINT8 bus, UINT8 device, UINT8 function);
void pci_enable_bus_master(UINT8 bus, UINT8 device, UINT8 function);
void pci_disable_bus_master(UINT8 bus, UINT8 device, UINT8 function);

DWORD64 get_pci_ecam_va();