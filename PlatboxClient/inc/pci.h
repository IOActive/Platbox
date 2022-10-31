#pragma once
#include "global.h"

// Functions
void read_pci_header(UINT8 bus, UINT8 device, UINT8 function);
BYTE read_pci_byte(UINT8 bus, UINT8 device, UINT8 function, UINT offset);
WORD read_pci_word(UINT8 bus, UINT8 device, UINT8 function, UINT offset);
DWORD read_pci_dword(UINT8 bus, UINT8 device, UINT8 function, UINT offset);
void write_pci_byte(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value);
void write_pci_word(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value);
void write_pci_dword(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value);
void read_pci_bar_info(UINT8 bus, UINT8 device, UINT8 function, UINT offset);

void parse_bus_device_function(char *line, UINT *bus, UINT *device, UINT *function);
void parse_handle_pci_operation(int argc, char **argv);

DWORD64 get_pci_ecam_va();