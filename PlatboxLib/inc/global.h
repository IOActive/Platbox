#pragma once
#include <stdio.h>
#include "types.h"

#ifdef _WIN32
	#include "__win.h"
#elif __linux__
	#include "__linux.h"
    #include <stdarg.h>
#endif

extern "C" {
	#include "DeltaFuzz.h"
}

extern int DEBUG_MODE;
void debug_print(const char *fmt, ...);

void enable_debug_mode();
void restore_debug_mode();

void print_green(const char *fmt, ...);
void print_red(const char *fmt, ...);

extern HANDLE g_hDevice;

extern FUZZER_OBJECT *g_fuzzer;

#define SVC_NAME "PlatboxSvc"


void InitializeDeltaFuzz();
void open_platbox_device();
void close_platbox_device();
void start_service();
void install_driver(char *driverNameArg);
void stop_service();
void remove_driver();
void init_os_specifics();


#ifdef __linux__    
	#include "kernetix.h"	
#elif _WIN32
    #include <windows.h>
	#include <intrin.h>
#else

#endif

#ifdef _WIN32    
	#define IOCTL_ISSUE_SW_SMI  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_EXECUTE_SHELLCODE  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_READ_PCI_HEADER  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_READ_PCI_BYTE  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_READ_PCI_WORD  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_READ_PCI_DWORD  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_WRITE_PCI_BYTE  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_WRITE_PCI_WORD  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_WRITE_PCI_DWORD  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_GET_PCI_BAR_SIZE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_READ_PHYSICAL_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_WRITE_PHYSICAL_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_READ_MSR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_WRITE_MSR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80D, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_PATCH_CALLBACK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_RESTORE_CALLBACK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_REMOVE_ALL_CALLBACKS_HOOKS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_MAP_PHYSICAL_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_UNMAP_PHYSICAL_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_ALLOC_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_READ_KMEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_WRITE_KMEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_GET_PHYSICAL_RANGES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

struct alloc_user_physmem {
	UINT64 va;
	UINT64 pa;
	UINT32 size;
};


// For linux, these are defined in kernetix.h
#ifdef _WIN32
	typedef struct _SW_SMI_CALL {
		UINT8 SwSmiNumber; // 0xb2
		UINT8 SwSmiData; // 0xb3
		UINT64 rax;
		UINT64 rbx;
		UINT64 rcx;
		UINT64 rdx;
		UINT64 rsi;
		UINT64 rdi;
	} SW_SMI_CALL, *PSW_SMI_CALL;

	typedef struct _READ_PCI_CONFIGURATION_SPACE_CALL {
		UINT8 bus;
		UINT8 device;
		UINT8 function;
	} READ_PCI_CONFIGURATION_SPACE_CALL, *PREAD_PCI_CONFIGURATION_SPACE_CALL;

	typedef struct _READ_PCI_DATA_CALL {
		UINT8 bus;
		UINT8 device;
		UINT8 function;
		UINT8 offset;
	} READ_PCI_DATA_CALL, *PREAD_PCI_DATA_CALL;

	typedef struct _WRITE_PCI_DATA_CALL {
		UINT8 bus;
		UINT8 device;
		UINT8 function;
		UINT8 offset;
		UINT32 value;
	} WRITE_PCI_DATA_CALL, *PWRITE_PCI_DATA_CALL;

	typedef struct _READ_PHYSICAL_MEM_CALL {
		UINT64 address;
	} READ_PHYSICAL_MEM_CALL, *PREAD_PHYSICAL_MEM_CALL;

	typedef struct _WRITE_PHYSICAL_MEM_CALL {
		UINT64 address;
		UINT32 size;
		UCHAR data[0x1000];
	} WRITE_PHYSICAL_MEM_CALL, *PWRITE_PHYSICAL_MEM_CALL;

	typedef struct _READ_MSR_CALL {
		UINT32 msr;
	} READ_MSR_CALL, *PREAD_MSR_CALL;

	typedef struct _WRITE_MSR_CALL {
		UINT32 msr;
		UINT64 value;
	} WRITE_MSR_CALL, *PWRITE_MSR_CALL;

	typedef struct _MAP_PHYSICAL_MEM_CALL {
		UINT64 phys_address;
	} MAP_PHYSICAL_MEM_CALL, *PMAP_PHYSICAL_MEM_CALL;

	typedef struct _UNMAP_PHYSICAL_MEM_CALL {
		PVOID user_address;
	} UNMAP_PHYSICAL_MEM_CALL, *PUNMAP_PHYSICAL_MEM_CALL;

	typedef struct _READ_KMEM_IN {
		UINT64 vaddr;
		UINT32 size;
	} READ_KMEM_IN, *PREAD_KMEM_IN;

	typedef struct _WRITE_KMEM_IN {
		UINT64 target_vaddr;
		UINT64* src_vaddr;
		UINT32 size;
	} WRITE_KMEM_IN, * PWRITE_KMEM_IN;

	typedef struct _PHYSICAL_MEMORY_RANGE {
		UINT64 BaseAddress;
		UINT64 NumberOfBytes;
	} PHYSICAL_MEMORY_RANGE, *PPHYSICAL_MEMORY_RANGE;
#endif


typedef struct _PCI_CONFIG_ADDRESS {
	DWORD Alignment : 2;
	DWORD RegisterNumber : 6;
	DWORD FunctionNumber : 3;
	DWORD DeviceNumber : 5;
	DWORD BusNumber : 8;
	DWORD Reserved : 7;
	DWORD Enabled : 1;
} PCI_CONFIG_ADDRESS;

typedef struct _PCI_BAR {
	DWORD IOMapped : 1;
	DWORD _64Bit : 2;
	DWORD Prefetchable : 1;
	DWORD BaseAddress : 28;
} PCI_BAR, *PPCI_BAR;

typedef struct _PCI_CONFIG_HEADER {
	WORD DeviceId;
	WORD VendorId;
	WORD Status;
	WORD Command;
	DWORD ClassCode : 24;
	DWORD RevisionId : 8;
	BYTE BIST;
	BYTE HeaderType;
	BYTE LatencyTimer;
	BYTE CacheLineSize;
	PCI_BAR BAR0;
	PCI_BAR BAR1;
	PCI_BAR BAR2;
	PCI_BAR BAR3;
	PCI_BAR BAR4;
	PCI_BAR BAR5;
	DWORD CardbusCISPointer;
	WORD SubsystemId;
	WORD SubsystemVendorId;
	DWORD ExpansionROMBaseAddress;
	DWORD Reversed0 : 24;
	DWORD CapabilitiesPointer : 8;
	DWORD Reserved1;
	BYTE MaxLat;
	BYTE MinGnt;
	BYTE InterruptPin;
	BYTE InterruptLine;
} PCI_CONFIG_HEADER, *PPCI_CONFIG_HEADER;

typedef enum {
	GeneralDevice,
	PCI2PCIBridge,
	CardBusBridge
} PCI_HEADER_TYPE;


typedef struct _PCI_ECAM {
	DWORD64 physaddr; // PCIEXBAR on Intel | MmioCfgBase on AMD
	DWORD64 vaddr;
	DWORD   size;
} PCI_ECAM;

extern PCI_ECAM g_pci_ecam;

#define PCI_COMPATIBLE_CONFIG_SPACE_SIZE 256
//#define PCI_ECAM_SIZE 256 * 1024 * 1024 // 256MB


