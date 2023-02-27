#pragma once
#include <ntddk.h>

typedef struct _PATCH_CALLBACK {
	PVOID targetAddress;
	UINT32 contentSize;
	PVOID patchContent;
} PATCH_CALLBACK, *PPATCH_CALLBACK;

typedef struct _RESTORE_CALLBACK {
	PVOID targetAddress;
} RESTORE_CALLBACK, *PRESTORE_CALLBACK;

PVOID CreateCbHook(PPATCH_CALLBACK pPatchCallback);
void RemoveCbHook(PRESTORE_CALLBACK pRestoreCallback);
void RemoveAllCbHooks();


typedef struct _HookCallback {
	PVOID targetAddress;
	PVOID originalCbValue;
	struct _HookCallback *next;
	char hookCode[0];
} HookCallback;

