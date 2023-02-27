// Written by Enrique Nissim (IOActive) 06/2018

#include "pci.h"
#include <stdio.h>
#include "Util.h"
#include "physmem.h"

//////////////////////////////////////////////////////////////

DWORD64 get_pci_ecam_va() {
	_map_pci_ecam();
	return g_pci_ecam.vaddr;
}


void read_pci_header(UINT8 bus, UINT8 device, UINT8 function) {

	PPCI_CONFIG_HEADER pciHeader = (PPCI_CONFIG_HEADER) calloc(1, 256);

	READ_PCI_CONFIGURATION_SPACE_CALL pci_config_call = { 0 };
	pci_config_call.bus = bus;
	pci_config_call.device = device;
	pci_config_call.function = function;

	#ifdef __linux__ 
		pci_config_call.buffer = pciHeader;
		int status = ioctl(g_hDevice, IOCTL_READ_PCI_HEADER, &pci_config_call );		
	#elif _WIN32
	
		DWORD bytesReturned = 0;
		NTSTATUS status = DeviceIoControl(g_hDevice, IOCTL_READ_PCI_HEADER,
			&pci_config_call, sizeof(READ_PCI_CONFIGURATION_SPACE_CALL),
			pciHeader, 256, &bytesReturned, NULL);
		debug_print("Status: %08x - Error: %08x\n", status, GetLastError());
	#endif

	printf("DeviceId: %04x\n", pciHeader->DeviceId);
	printf("VendorId: %04x\n", pciHeader->VendorId);
	printf("Status: %04x\n", pciHeader->Status);
	printf("Command: %04x\n", pciHeader->Command);
	printf("ClassCode: %06x\n", pciHeader->ClassCode);
	printf("BAR0: %08x\n", pciHeader->BAR0.BaseAddress);
	printf("BAR1: %08x\n", pciHeader->BAR1.BaseAddress);
	printf("BAR2: %08x\n", pciHeader->BAR2.BaseAddress);
	printf("BAR3: %08x\n", pciHeader->BAR3.BaseAddress);
	printf("BAR4: %08x\n", pciHeader->BAR4.BaseAddress);
	printf("BAR5: %08x\n", pciHeader->BAR5.BaseAddress);
	print_memory(0x0, (char *)pciHeader, 256);

	free(pciHeader);
}

BYTE read_pci_byte(UINT8 bus, UINT8 device, UINT8 function, UINT offset) {
	READ_PCI_DATA_CALL pci_call = { 0 };
	pci_call.bus = bus;
	pci_call.device = device;
	pci_call.function = function;
	pci_call.offset = offset;
	BYTE aux;

	if ((offset) >= PCI_COMPATIBLE_CONFIG_SPACE_SIZE) {
	
		DWORD64 ecam_pa = g_pci_ecam.physaddr;
		
		UINT64 target_pa = (ecam_pa + ((UINT64)bus << 20) 
			+ ((UINT64)device << 15) + ((UINT64)function << 12));

		UINT64 *target_addr = (UINT64 * ) ((UINT64) map_physical_memory(target_pa, 0x1000) + offset);

		//printf("attempting to read from: pa:[%llx] ,%lx\n", target_pa, target_addr);

		aux = *(BYTE *) target_addr;

		unmap_physical_memory(target_addr, 0x1000);
		
	} else {

		NTSTATUS status = 0;
		
		#ifdef __linux__
			status = -1;	
			pci_call.result = (UINT64 *) &aux;	
			status = ioctl(g_hDevice, IOCTL_READ_PCI_BYTE, &pci_call);
			if (status == 0) {
				debug_print("-> One byte read from %02x:%02x:%02x offset %08xh: %02xh\n",
					bus, device, function, offset, aux);
			}
		#elif _WIN32	
			DWORD bytesReturned = 0;
			
			status = DeviceIoControl(g_hDevice, IOCTL_READ_PCI_BYTE,
				&pci_call, sizeof(READ_PCI_DATA_CALL),
				&aux, sizeof(BYTE), &bytesReturned, NULL);
			
			if (NT_SUCCESS(status)) {
				debug_print("-> One byte read from %02x:%02x:%02x offset %08xh: %02xh\n",
					bus, device, function, offset, aux);
			}
		#endif
	}

	return aux;
}

WORD read_pci_word(UINT8 bus, UINT8 device, UINT8 function, UINT offset) {
	READ_PCI_DATA_CALL pci_call = { 0 };
	pci_call.bus = bus;
	pci_call.device = device;
	pci_call.function = function;

	offset <<= 1;
	pci_call.offset = offset;

	WORD aux;
	
	if (offset >= PCI_COMPATIBLE_CONFIG_SPACE_SIZE) {
	
		DWORD64 ecam_pa = g_pci_ecam.physaddr;
		
		UINT64 target_pa = (ecam_pa + ((UINT64)bus << 20) 
			+ ((UINT64)device << 15) + ((UINT64)function << 12));

		UINT64 *target_addr = (UINT64 * ) ((UINT64) map_physical_memory(target_pa, 0x1000) + offset);

		//printf("attempting to read from: pa:[%llx] ,%lx\n", target_pa, target_addr);

		aux = *(WORD *) target_addr;

		unmap_physical_memory(target_addr, 0x1000);

	} else {

		NTSTATUS status;
		
		#ifdef __linux__
			status = -1;		
			pci_call.result = (UINT64 *) &aux;	
			status = ioctl(g_hDevice, IOCTL_READ_PCI_WORD, &pci_call);		
			if (status == 0) {
				debug_print("-> One word read from %02x:%02x:%02x offset %08xh: %04xh\n",
					bus, device, function, offset, aux);
			}
		#elif _WIN32
			DWORD bytesReturned = 0;	
			status = DeviceIoControl(g_hDevice, IOCTL_READ_PCI_WORD,
				&pci_call, sizeof(READ_PCI_DATA_CALL),
				&aux, sizeof(WORD), &bytesReturned, NULL);
			
			if (NT_SUCCESS(status)) {
				debug_print("-> One word read from %02x:%02x:%02x offset %08xh: %04xh\n",
					bus, device, function, offset, aux);
			}
		#endif

	}
	
	return aux;
}

DWORD read_pci_dword(UINT8 bus, UINT8 device, UINT8 function, UINT offset) {
	READ_PCI_DATA_CALL pci_call = { 0 };
	pci_call.bus = bus;
	pci_call.device = device;
	pci_call.function = function;

	offset <<= 2;
	pci_call.offset = offset;	

	DWORD aux;

	if (offset >= PCI_COMPATIBLE_CONFIG_SPACE_SIZE) {

		DWORD64 ecam_pa = g_pci_ecam.physaddr;
		
		UINT64 target_pa = (ecam_pa + ((UINT64)bus << 20) 
			+ ((UINT64)device << 15) + ((UINT64)function << 12));

		UINT64 *target_addr = (UINT64 * ) ((UINT64) map_physical_memory(target_pa, 0x1000) + offset);

		//printf("attempting to read from: pa:[%llx] ,%lx\n", target_pa, target_addr);

		aux = *(DWORD *) target_addr;

		unmap_physical_memory(target_addr, 0x1000);

	} else {
		NTSTATUS status;
		#ifdef __linux__
			status = -1;		
			pci_call.result = (UINT64 *) &aux;	
			status = ioctl(g_hDevice, IOCTL_READ_PCI_DWORD, &pci_call);		
			if (status == 0) {
				debug_print("-> One dword read from %02x:%02x:%02x offset %08xh: %08xh\n",
					bus, device, function, offset, aux);
			}
		#elif _WIN32	
			DWORD bytesReturned = 0;
			status = DeviceIoControl(g_hDevice, IOCTL_READ_PCI_DWORD,
				&pci_call, sizeof(READ_PCI_DATA_CALL),
				&aux, sizeof(DWORD), &bytesReturned, NULL);

			if (NT_SUCCESS(status)) {
				debug_print("-> One dword read from %02x:%02x:%02x offset %08xh: %08xh\n",
					bus, device, function, offset, aux);
			}
		#endif
	}
	
	return aux;
}

void write_pci_byte(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value) {
	WRITE_PCI_DATA_CALL pci_call = { 0 };
	pci_call.bus = bus;
	pci_call.device = device;
	pci_call.function = function;
	pci_call.offset = offset;
	pci_call.value = value;

	if (offset >= PCI_COMPATIBLE_CONFIG_SPACE_SIZE) {

		DWORD64 ecam_pa = g_pci_ecam.physaddr;
		
		UINT64 target_pa = (ecam_pa + ((UINT64)bus << 20) 
			+ ((UINT64)device << 15) + ((UINT64)function << 12));

		UINT64 *target_addr = (UINT64 * ) ((UINT64) map_physical_memory(target_pa, 0x1000) + offset);

		*(BYTE *) target_addr = value;

		unmap_physical_memory(target_addr, 0x1000);
		
	} else {

		NTSTATUS status;
	
		#ifdef __linux__
			status = -1;	
			status = ioctl(g_hDevice, IOCTL_WRITE_PCI_BYTE, &pci_call);		
			if (status == 0) {
				debug_print("-> One byte written into %02x:%02x:%02x offset %08xh: %02xh\n",
					bus, device, function, offset, value);
			}
		#elif _WIN32
			DWORD bytesReturned = 0;
			status = DeviceIoControl(g_hDevice, IOCTL_WRITE_PCI_BYTE,
				&pci_call, sizeof(WRITE_PCI_DATA_CALL),
				NULL, 0, &bytesReturned, NULL);

			if (NT_SUCCESS(status)) {
				debug_print("-> One byte written into %02x:%02x:%02x offset %08xh: %02xh\n",
					bus, device, function, offset, value);
			}
		#endif
	}
	
}

void write_pci_word(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value) {
	WRITE_PCI_DATA_CALL pci_call = { 0 };
	pci_call.bus = bus;
	pci_call.device = device;
	pci_call.function = function;

	offset <<= 1;
	pci_call.offset = offset;
	pci_call.value = value;
		

	if (offset >= PCI_COMPATIBLE_CONFIG_SPACE_SIZE) {

		DWORD64 ecam_pa = g_pci_ecam.physaddr;
		
		UINT64 target_pa = (ecam_pa + ((UINT64)bus << 20) 
			+ ((UINT64)device << 15) + ((UINT64)function << 12));

		UINT64 *target_addr = (UINT64 * ) ((UINT64) map_physical_memory(target_pa, 0x1000) + offset);

		*(WORD *) target_addr = value;

		unmap_physical_memory(target_addr, 0x1000);
		
	} else {

		NTSTATUS status;

		#ifdef __linux__
			status = -1;	
			status = ioctl(g_hDevice, IOCTL_WRITE_PCI_WORD, &pci_call);		
			if (status == 0) {
				debug_print("-> One byte written into %02x:%02x:%02x offset %08xh: %02xh\n",
					bus, device, function, offset, value);
			}
		#elif _WIN32
			DWORD bytesReturned = 0;
			status = DeviceIoControl(g_hDevice, IOCTL_WRITE_PCI_WORD,
				&pci_call, sizeof(WRITE_PCI_DATA_CALL),
				NULL, 0, &bytesReturned, NULL);

			if (NT_SUCCESS(status)) {
				debug_print("-> One word written into %02x:%02x:%02x offset %08xh: %04xh\n",
					bus, device, function, offset, value);
			}
		#endif
	}
	
}

void write_pci_dword(UINT8 bus, UINT8 device, UINT8 function, UINT offset, UINT value) {
	WRITE_PCI_DATA_CALL pci_call = { 0 };
	pci_call.bus = bus;
	pci_call.device = device;
	pci_call.function = function;
	
	offset <<= 2;
	pci_call.offset = offset;
	pci_call.value = value;


	if (offset >= PCI_COMPATIBLE_CONFIG_SPACE_SIZE) {

		DWORD64 ecam_pa = g_pci_ecam.physaddr;
		
		UINT64 target_pa = (ecam_pa + ((UINT64)bus << 20) 
			+ ((UINT64)device << 15) + ((UINT64)function << 12));

		UINT64 *target_addr = (UINT64 * ) ((UINT64) map_physical_memory(target_pa, 0x1000) + offset);

		*(DWORD *) target_addr = value;

		unmap_physical_memory(target_addr, 0x1000);
		
	} else {

		NTSTATUS status;

		#ifdef __linux__
			status = -1;	
			status = ioctl(g_hDevice, IOCTL_WRITE_PCI_DWORD, &pci_call);		
			if (status == 0) {
				debug_print("-> One byte written into %02x:%02x:%02x offset %08xh: %02xh\n",
					bus, device, function, offset, value);
			}
		#elif _WIN32
			DWORD bytesReturned = 0;
			status = DeviceIoControl(g_hDevice, IOCTL_WRITE_PCI_DWORD,
				&pci_call, sizeof(WRITE_PCI_DATA_CALL),
				NULL, 0, &bytesReturned, NULL);

			if (NT_SUCCESS(status)) {
				debug_print("-> One dword written into %02x:%02x:%02x offset %08xh: %08xh\n",
					bus, device, function, offset, value);
			}
		#endif

	}
	
}

void read_pci_bar_info(UINT8 bus, UINT8 device, UINT8 function, UINT offset) {
	// Assumes a BAR at offset and retrieves it's size and settings
	DWORD bar = read_pci_dword(bus, device, function, offset);
	PPCI_BAR p = (PPCI_BAR)&bar;
	printf("-> BAR at %08xh: \n", offset);
	if (p->IOMapped) {
		printf("---> Type: IO-Mapped\n");
	}
	else {
		printf("---> Type: Memory-Mapped\n");
	}
	if (p->Prefetchable) {
		printf("---> Prefetchable: TRUE\n");
	}
	else {
		printf("---> Prefetchable: FALSE\n");
	}
	UINT64 bar_size = 0;
	READ_PCI_DATA_CALL pci_call = { 0 };
	pci_call.bus = bus;
	pci_call.device = device;
	pci_call.function = function;
	pci_call.offset = offset;
	NTSTATUS status;

	#ifdef __linux__
		pci_call.result = &bar_size;
		status = ioctl(g_hDevice, IOCTL_GET_PCI_BAR_SIZE, &pci_call);
	#elif _WIN32
		DWORD bytesReturned = 0;
		status = DeviceIoControl(g_hDevice, IOCTL_GET_PCI_BAR_SIZE,
			&pci_call, sizeof(READ_PCI_DATA_CALL),
			&bar_size, sizeof(PVOID), &bytesReturned, NULL);
	#endif
	
	bar_size = bar_size ^ 0xFFFFFFFF;
	printf("---> BaseAddress: %lhx - Size: %lhx\n", p->BaseAddress << 4, bar_size);

}

