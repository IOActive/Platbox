#include "amd_acpi.h"
#include "global.h"
#include "pci.h"
#include "Util.h"
#include "physmem.h"
#include "msr.h"
#include "_io.h"

UINT16 get_amd_smi_trigger_port() {
    char* ptr = (char *)map_physical_memory(AMD_ACPI_MMIO_BASE, 0x1000);
    //print_memory(0, ptr + ACPI_OFFSET_PMIO, 0x100);
    UINT16 smi_trigger = *(UINT16 *) (ptr + ACPI_PMIO_SMI_CMD);
    unmap_physical_memory(ptr, 0x1000);
    return smi_trigger;
}

void get_amd_smi_trigger_port_iopm() {
    enable_debug_mode();
    io_outb(IO_PM_INDEX, 0x24);
    DWORD base = io_ind(IO_PM_DATA);
    printf("BASE %08x\n", base);
}