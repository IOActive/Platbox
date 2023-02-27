// Written by Enrique Nissim (IOActive) 06/2018

#include "UEFIVars.h"
#include "Util.h"


const char * UEFI_VARIABLE_ATTRIBUTES[] = {
	"NV", //"NON_VOLATILE", 
	"BS", //"BOOTSERVICE_ACCESS", 
	"RT", //RUNTIME_ACCESS",
	"HR", //"HARDWARE_ERROR_RECORD",
	"AW", //"AUTHENTICATED_WRITE_ACCESS", 
	"TA", //"TIME_BASED_AUTHENTICATED_WRITE_ACCESS",
	"AW", //"APPEND_WRITE",
};






#ifdef _WIN32 

	void display_variable_attributes(DWORD attributes) {
		int i;
		for (i = 0; i < ARRAYSIZE(UEFI_VARIABLE_ATTRIBUTES); i++)
			if ((1 << i) & attributes)
				printf("%s|", UEFI_VARIABLE_ATTRIBUTES[i]);
	}


	void list_unprotected_variables(PVARIABLE_NAME_AND_VALUE buffer) {
		for (; buffer; buffer = buffer->NextEntryOffset ? (PVARIABLE_NAME_AND_VALUE)((PBYTE)buffer + buffer->NextEntryOffset) : NULL)
		{
			if (buffer->Attributes & EFI_VARIABLE_RUNTIME_ACCESS &&
				buffer->Attributes & EFI_VARIABLE_NON_VOLATILE &&
				!(buffer->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) &&
				!(buffer->Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
				)
			{
				printf("\n%S", buffer->Name);
				display_guid(&buffer->VendorGuid);
				printf("|");
				display_variable_attributes(buffer->Attributes);
			}
		}
		printf("\n");
	}

	void list_all(PVARIABLE_NAME_AND_VALUE buffer) {
		for (; buffer; buffer = buffer->NextEntryOffset ? (PVARIABLE_NAME_AND_VALUE)((PBYTE)buffer + buffer->NextEntryOffset) : NULL)
		{
			printf("\n%S", buffer->Name);
			//puts("VendorGUID: "); 
			display_guid(&buffer->VendorGuid);
			printf("|");
			//printf("\nAttributes: %08x\n", buffer->Attributes);					
			display_variable_attributes(buffer->Attributes);
			//puts("\n");
			//printf("\nLength of Data: %08x\n", buffer->ValueLength);
			//if (buffer->ValueLength && buffer->ValueOffset) {
			//	print_memory(0x00, ((char *)buffer + buffer->ValueOffset), buffer->ValueLength);
			//}
		}
	}




_NtEnumerateSystemEnvironmentValuesEx  NtEnumerateSystemEnvironmentValuesEx;
void list_uefi_variables()
{
		HMODULE ntdll = LoadLibraryA("ntdll.dll");
		NtEnumerateSystemEnvironmentValuesEx = (_NtEnumerateSystemEnvironmentValuesEx)
			GetProcAddress(ntdll, "NtEnumerateSystemEnvironmentValuesEx");
		printf("NtEnumerateSystemEnvironmentValuesEx: %p\n", NtEnumerateSystemEnvironmentValuesEx);

		PVARIABLE_NAME_AND_VALUE buffer = NULL;
		DWORD bufferLen = 0;
		NTSTATUS status;

		get_more_privileges();

		status = NtEnumerateSystemEnvironmentValuesEx(VARIABLE_INFORMATION_VALUES, NULL, &bufferLen);
		if (status == 0xC0000061) {
			printf("Not enough privileges!\n");
		}

		if ((status == STATUS_BUFFER_TOO_SMALL) && bufferLen)
		{
			if (buffer = (PVARIABLE_NAME_AND_VALUE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferLen))
			{
				status = NtEnumerateSystemEnvironmentValuesEx(VARIABLE_INFORMATION_VALUES, buffer, &bufferLen);
				if (NT_SUCCESS(status))
				{
					list_all(buffer);
				}
				else {
					printf("\nError: \n", GetLastError());
				}
			}
			if (buffer) {
				HeapFree(GetProcessHeap(), 0, buffer);
			}
		}
	
}

#endif