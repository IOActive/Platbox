// Written by Enrique Nissim (IOActive) 06/2018

#pragma once
#include <stdio.h>
#include "types.h"


#define SE_CREATE_TOKEN				2
#define SE_ASSIGNPRIMARYTOKEN		3
#define SE_LOCK_MEMORY				4
#define SE_INCREASE_QUOTA			5
#define SE_UNSOLICITED_INPUT		6
#define SE_TCB						7
#define SE_SECURITY					8
#define SE_TAKE_OWNERSHIP			9
#define SE_LOAD_DRIVER				10
#define SE_SYSTEM_PROFILE			11
#define SE_SYSTEMTIME				12
#define SE_PROF_SINGLE_PROCESS		13
#define SE_INC_BASE_PRIORITY		14
#define SE_CREATE_PAGEFILE			15
#define SE_CREATE_PERMANENT			16
#define SE_BACKUP					17
#define SE_RESTORE					18
#define SE_SHUTDOWN					19
#define SE_DEBUG					20
#define SE_AUDIT					21
#define SE_SYSTEM_ENVIRONMENT		22
#define SE_CHANGE_NOTIFY			23
#define SE_REMOTE_SHUTDOWN			24
#define SE_UNDOCK					25
#define SE_SYNC_AGENT				26
#define SE_ENABLE_DELEGATION		27
#define SE_MANAGE_VOLUME			28
#define SE_IMPERSONATE				29
#define SE_CREATE_GLOBAL			30
#define SE_TRUSTED_CREDMAN_ACCESS	31
#define SE_RELABEL					32
#define SE_INC_WORKING_SET			33
#define SE_TIME_ZONE				34
#define SE_CREATE_SYMBOLIC_LINK		35



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
	typedef NTSTATUS(WINAPI * _RtlAdjustPrivilege)(IN ULONG Privilege, IN BOOL Enable, IN BOOL CurrentThread, OUT PULONG pPreviousState);

	_NtEnumerateSystemEnvironmentValuesEx  NtEnumerateSystemEnvironmentValuesEx;
	_RtlAdjustPrivilege RtlAdjustPrivilege;

	#define EFI_VARIABLE_NON_VOLATILE 1
	#define EFI_VARIABLE_BOOTSERVICE_ACCESS 2
	#define EFI_VARIABLE_RUNTIME_ACCESS 4
	#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 8
	#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS 0x10
	#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x20
	#define EFI_VARIABLE_APPEND_WRITE 0x40

#endif 