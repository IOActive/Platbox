#pragma once
#include "types.h"

extern UINT64 gAcpiMMIOAddr;

#define IO_PM_INDEX 0xCD6
#define IO_PM_DATA  0xCD7

#define AMD_ACPI_MMIO_BASE 0xfed80000

#define ACPI_OFFSET_SM_PCI      0
#define ACPI_OFFSET_GPIO        0x100
#define ACPI_OFFSET_SMI         0x200
#define ACPI_OFFSET_PMIO        0x300
#define ACPI_OFFSET_PMIO2       0x400
#define ACPI_OFFSET_BIOS_RAM    0x500
#define ACPI_OFFSET_CMOS_RAM    0x600
#define ACPI_OFFSET_CMOS        0x700
#define ACPI_OFFSET_ACPI        0x800
#define ACPI_OFFSET_ASF         0x900
#define ACPI_OFFSET_SMBUS       0xA00
#define ACPI_OFFSET_WATCHDOG    0xB00
#define ACPI_OFFSET_HPET        0xC00
#define ACPI_OFFSET_IOMUX       0xD00
#define ACPI_OFFSET_MISC        0xE00

#define ACPI_PMIO_SMI_CMD       ACPI_OFFSET_PMIO + 0x6a

UINT16 get_amd_smi_trigger_port();
void get_amd_smi_trigger_port_iopm();

