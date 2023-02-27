#pragma once
#include <ntddk.h>

typedef struct _X86BIOS_REGISTERS	// invented names
{
    ULONG Eax;
    ULONG Ecx;
    ULONG Edx;
    ULONG Ebx;
    ULONG Ebp;
    ULONG Esi;
    ULONG Edi;
    USHORT SegDs;
    USHORT SegEs;
} X86BIOS_REGISTERS, * PX86BIOS_REGISTERS;

/*  Undocumented imports from the HAL  */
NTHALAPI BOOLEAN x86BiosCall(ULONG, PX86BIOS_REGISTERS);
NTHALAPI NTSTATUS x86BiosAllocateBuffer(ULONG*, USHORT*, USHORT*);
NTHALAPI NTSTATUS x86BiosFreeBuffer(USHORT, USHORT);
NTHALAPI NTSTATUS x86BiosReadMemory(USHORT, USHORT, PVOID, ULONG);
NTHALAPI NTSTATUS x86BiosWriteMemory(USHORT, USHORT, PVOID, ULONG);


#pragma pack (push, 8)			/*  in case driver and application
                        get compiled differently  */
typedef struct _E820_DESCRIPTOR {
    LARGE_INTEGER Base;
    LARGE_INTEGER Size;
    ULONG Type;
    ULONG ExtAttr;                      // not actually used by WINLOAD
} E820_DESCRIPTOR, * PE820_DESCRIPTOR;

typedef struct _E820_MAP {
    ULONG Count;
    E820_DESCRIPTOR Descriptors[1];
} E820_MAP, * PE820_MAP;

#pragma pack (pop)

NTSTATUS GetMap(PE820_MAP*, ULONG*);
VOID ReleaseMap(PE820_MAP);
