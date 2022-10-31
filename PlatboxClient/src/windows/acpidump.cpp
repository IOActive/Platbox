#include "PlatboxClient.h"
#include "Util.h"


UINT32 GetFADT_Address() {

	DWORD error = ERROR_SUCCESS;
	DWORD requriedSize = 0;
	RTACPI_FACP* tableData = NULL;
	DWORD bytesWritten = 0;

	DWORD FADT_addr = 0;

	requriedSize = GetSystemFirmwareTable('ACPI', 'PCAF', NULL, 0);

	// Allocate memory for data
	tableData = (RTACPI_FACP*) HeapAlloc(GetProcessHeap(), 0, requriedSize);
	if (!tableData) {
		error = ERROR_OUTOFMEMORY;
		goto exit;
	}

	// Retrieve the table
	bytesWritten = GetSystemFirmwareTable('ACPI', 'PCAF', tableData, requriedSize);

	if (bytesWritten != requriedSize) {
		error = ERROR_INVALID_DATA;
		free(tableData);
		goto exit;
	}

	FADT_addr = tableData->FIRMWARE_CTRL;
	free(tableData);

	exit:
	return FADT_addr;
}

void EnumerateACPITables() {
	#ifdef __linux__ 
		printf("EnumerateACPITables unimplemented!");
		exit(-1);
	#else // _WIN32
		
		char Signature[4] = { 'U','E','F','I' };
		UINT32 requiredSize = EnumSystemFirmwareTables((DWORD)Signature , NULL, 0);
		if (requiredSize == 0) {
			printf("Error in EnumSystemFirmwareTables: %08x\n", GetLastError());		
			return;
		}
		PVOID firmwareTablesBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, requiredSize);
		if (!firmwareTablesBuffer) {
			printf("- Error: allocation failed!\n");
			return;
		}
		requiredSize = EnumSystemFirmwareTables((DWORD)Signature, firmwareTablesBuffer, requiredSize);
		if (requiredSize == 0) {
			printf("Error in EnumSystemFirmwareTables\n");
			HeapFree(GetProcessHeap(), 0, firmwareTablesBuffer);
			return;
		}
		print_memory(0, (char *) firmwareTablesBuffer, requiredSize);

		HeapFree(GetProcessHeap(), 0, firmwareTablesBuffer);
	#endif	
}