// Written by Enrique Nissim (IOActive) 06/2018

#pragma once
#include <stdio.h>
#include "types.h"



#ifdef _WIN32 

	typedef struct _XUNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	}  XUNICODE_STRING, *PXUNICODE_STRING;


	#define VARIABLE_INFORMATION_NAMES  1
	#define VARIABLE_INFORMATION_VALUES 2

	typedef struct _VARIABLE_NAME {
		ULONG NextEntryOffset;
		GUID VendorGuid;
		WCHAR Name[ANYSIZE_ARRAY];
	} VARIABLE_NAME, *PVARIABLE_NAME;

	typedef struct _VARIABLE_NAME_AND_VALUE {
		ULONG NextEntryOffset;
		ULONG ValueOffset;
		ULONG ValueLength;
		ULONG Attributes;
		GUID VendorGuid;
		WCHAR Name[ANYSIZE_ARRAY];
		//UCHAR Value[ANYSIZE_ARRAY];
	} VARIABLE_NAME_AND_VALUE, *PVARIABLE_NAME_AND_VALUE;


	typedef NTSTATUS(NTAPI * _NtQuerySystemEnvironmentValue)(__in PXUNICODE_STRING VariableName, __out_bcount(ValueLength) PWSTR VariableValue, __in USHORT ValueLength, __out_opt PUSHORT ReturnLength);
	typedef NTSTATUS(NTAPI * _NtSetSystemEnvironmentValue)(__in PXUNICODE_STRING VariableName, __in PXUNICODE_STRING VariableValue);
	typedef NTSTATUS(NTAPI * _NtQuerySystemEnvironmentValueEx)(__in PXUNICODE_STRING VariableName, __in LPGUID VendorGuid, __out_bcount_opt(*ValueLength) PVOID Value, __inout PULONG ValueLength, __out_opt PULONG Attributes);
	typedef NTSTATUS(NTAPI * _NtSetSystemEnvironmentValueEx)(__in PXUNICODE_STRING VariableName, __in LPGUID VendorGuid, __in_bcount_opt(ValueLength) PVOID Value, __in ULONG ValueLength, __in ULONG Attributes);
	typedef NTSTATUS(NTAPI * _NtEnumerateSystemEnvironmentValuesEx)(__in ULONG InformationClass, __out PVOID Buffer, __inout PULONG BufferLength);
	
	extern _NtEnumerateSystemEnvironmentValuesEx  NtEnumerateSystemEnvironmentValuesEx;
	

	#define EFI_VARIABLE_NON_VOLATILE 1
	#define EFI_VARIABLE_BOOTSERVICE_ACCESS 2
	#define EFI_VARIABLE_RUNTIME_ACCESS 4
	#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 8
	#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS 0x10
	#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x20
	#define EFI_VARIABLE_APPEND_WRITE 0x40


	/* UEFI Vars */ 
	void list_uefi_variables();



#endif 