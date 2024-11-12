#pragma once
#include <ntddk.h>

#ifndef PAGE_SIZE
    #define PAGE_SIZE 0x1000
#endif

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
#define IOCTL_READ_IO_PORT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_IO_PORT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PATCH_CALLBACK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RESTORE_CALLBACK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_REMOVE_ALL_CALLBACKS_HOOKS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MAP_PHYSICAL_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UNMAP_PHYSICAL_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ALLOC_MEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_READ_KMEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_KMEM CTL_CODE(FILE_DEVICE_UNKNOWN, 0x817, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GET_PHYSICAL_RANGES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x818, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SINKCLOSE  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_MSR_FOR_CORE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MSR_FOR_CORE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x822, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_PHYSICAL_ADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x823, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_INDEX_IO_OPERATION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x824, METHOD_BUFFERED, FILE_ANY_ACCESS)

DRIVER_INITIALIZE    DriverEntry;
DRIVER_UNLOAD        IrpUnloadHandler;
DRIVER_DISPATCH      IrpNotImplementedHandler;

__drv_dispatchType(IRP_MJ_CREATE)            DRIVER_DISPATCH    IrpCreateHandler;
__drv_dispatchType(IRP_MJ_CLOSE)             DRIVER_DISPATCH    IrpCloseHandler;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)    DRIVER_DISPATCH    IrpDeviceIoCtlHandler;

VOID        IrpUnloadHandler(IN PDRIVER_OBJECT DriverObject);
NTSTATUS    IrpCloseHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS    IrpCreateHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS    IrpDeviceIoCtlHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS    IrpNotImplementedHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS    DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

NTSTATUS SendSWSmiHandler(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS Sinkclose(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);

NTSTATUS ExecuteShellcodeHandler(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadPCIHeaderHandler(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadPCIByte(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadPCIWord(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadPCIDword(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WritePCIByte(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WritePCIWord(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WritePCIDword(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadPCIBarSize(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadPhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WritePhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadMSR(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadMSRForCore(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WriteMSR(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WriteMSRForCore(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadIOPort(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WriteIOPort(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS PatchCallback(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS RestoreCallback(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS RemoveAllCallbackHooks(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS MapPhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS UnmapPhysicalMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS AllocateUserMemory(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS ReadKMem(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS WriteKMem(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS GetPhysicalMemoryRanges(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS IoctlGetPhysicalAddress(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);
NTSTATUS IoctlIndexIO(IN PIRP Irp, IN PIO_STACK_LOCATION IrpSp);


typedef char BYTE;


#pragma pack(1)
typedef struct segment_descriptor {
    UINT16 selector;
    char   rsvd[6];
    UINT64 descriptor;
};

typedef struct _SMM_SAVE_STATE {
    /* FE00h */ struct segment_descriptor es;
    /* FE10h */ struct segment_descriptor cs;
    /* FE20h */ struct segment_descriptor ss;
    /* FE30h */ struct segment_descriptor ds;
    /* FE40h */ struct segment_descriptor fs;
    /* FE50h */ struct segment_descriptor gs;
    /* FE60h */ char   gdtr[16];
    /* FE70h */ char   ldtr[16];
    /* FE80h */ char   idtr[16];
    /* FE90h */ char   tr[16];
    /* FEA0h */ UINT64 io_restart_rip;
    /* FEA8h */ UINT64 io_restart_rcx;
    /* FEB0h */ UINT64 io_restart_rsi;
    /* FEB8h */ UINT64 io_restart_rdi;
    /* FEC0h */ UINT32 smm_io_trap_offset;
    /* FEC4h */ UINT32 local_smi_status;
    /* FEC8h */ char   smm_io_restart_byte;
    /* FEC9h */ char   auto_halt_restart_offset;
    /* FECAh */ char   nmi_mask;
    /* FECBh */ char   rsvd0[5];
    /* FED0h */ UINT64 efer;
    /* FED8h */ UINT64 smm_svm_state;
    /* FEE0h */ UINT64 guest_vmcb_physical_address;
    /* FEE8h */ UINT64 svm_virtual_interrupt_control;
    /* FEF0h */ char   rsvd1[12];
    /* FEFCh */ UINT32 smm_revision_identifier;
    /* FFF0h */ UINT32 smm_base;
    /* FFF4h */ char   rsvd2[28];
    /* FF20h */ UINT64 guest_pat;
    /* FF28h */ UINT64 host_efer;
    /* FF30h */ UINT64 host_cr4;
    /* FF38h */ UINT64 nested_cr3;
    /* FF40h */ UINT64 host_cr0;
    /* FF48h */ UINT64 cr4;
    /* FF50h */ UINT64 cr3;
    /* FF58h */ UINT64 cr0;
    /* FF60h */ UINT64 dr7;
    /* FF68h */ UINT64 dr6;
    /* FF70h */ UINT64 rflags;
    /* FF78h */ UINT64 rip;
    /* FF80h */ UINT64 r15;
    /* FF88h */ UINT64 r14;
    /* FF90h */ UINT64 r13;
    /* FF98h */ UINT64 r12;
    /* FFA0h */ UINT64 r11;
    /* FFA8h */ UINT64 r10;
    /* FFB0h */ UINT64 r9;
    /* FFB8h */ UINT64 r8;
    /* FFC0h */ UINT64 rdi;
    /* FFC8h */ UINT64 rsi;
    /* FFD0h */ UINT64 rbp;
    /* FFD8h */ UINT64 rsp;
    /* FFE0h */ UINT64 rbx;
    /* FFE8h */ UINT64 rdx;
    /* FFF0h */ UINT64 rcx;
    /* FFF8h */ UINT64 rax;
} SMM_SAVE_STATE, *PSMM_SAVE_STATE;


typedef struct _SW_SMI_CALL {
    UINT64 SwSmiNumber; // 0xb2
    UINT64 SwSmiData; // 0xb3
    UINT64 TriggerPort;
    UINT64 rax;
    UINT64 rbx;
    UINT64 rcx;
    UINT64 rdx;
    UINT64 rsi;
    UINT64 rdi;
    UINT64 r8;
    UINT64 r9;
    UINT64 r10;
    UINT64 r11;
    UINT64 r12;
    UINT64 r13;
    UINT64 r14;
    UINT64 r15;
    // apend here?
} SW_SMI_CALL, *PSW_SMI_CALL;

typedef struct _SW_SMI_CALL_SINKCLOSE {
    UINT64 TriggerPort;
    SMM_SAVE_STATE SmmSaveState;
} SW_SMI_CALL_SINKCLOSE, *PSW_SMI_CALL_SINKCLOSE;

void _ret();
void _haltcore();

void _disable_interrupts();
void _enable_interrupts();

inline void _store_savestate(PSMM_SAVE_STATE SmmSaveState);

void _swsmi(PSW_SMI_CALL smi_call);
void _swsmi_ipi(PSW_SMI_CALL smi_call);



typedef struct _READ_PCI_CONFIGURATION_SPACE_CALL {
    UINT8 bus;
    UINT8 device;
    UINT8 function;
} READ_PCI_CONFIGURATION_SPACE_CALL, *PREAD_PCI_CONFIGURATION_SPACE_CALL;

void _read_pci_compatible_configuration_space(
    UINT8 bus,
    UINT8 device,
    UINT8 function,
    PVOID pOut
);

typedef struct _READ_PCI_DATA_CALL {
    UINT8 bus;
    UINT8 device;
    UINT8 function;
    UINT8 offset;
} READ_PCI_DATA_CALL, *PREAD_PCI_DATA_CALL;

void _read_pci_byte(UINT32 CF8, PVOID pOut);
void _read_pci_word(UINT32 CF8, PVOID pOut);
void _read_pci_dword(UINT32 CF8, PVOID pOut);
void _read_pci_bar_size(UINT32 CF8, PVOID pOut);

typedef struct _WRITE_PCI_DATA_CALL {
    UINT8 bus;
    UINT8 device;
    UINT8 function;
    UINT8 offset;
    UINT32 value;
} WRITE_PCI_DATA_CALL, *PWRITE_PCI_DATA_CALL;

void _write_pci_byte(UINT32 CF8, UINT32 value);
void _write_pci_word(UINT32 CF8, UINT32 value);
void _write_pci_dword(UINT32 CF8, UINT32 value);

typedef struct _READ_PHYSICAL_MEM_CALL {
    PHYSICAL_ADDRESS address;
} READ_PHYSICAL_MEM_CALL, *PREAD_PHYSICAL_MEM_CALL;

typedef struct _WRITE_PHYSICAL_MEM_CALL {
    PHYSICAL_ADDRESS address;
    UINT32 size;
    UCHAR data[0x1000];
} WRITE_PHYSICAL_MEM_CALL, *PWRITE_PHYSICAL_MEM_CALL;

struct alloc_user_physmem {
    UINT64 va;
    UINT64 pa;
    UINT32 size;
};


void _rdmsr(UINT32 msr, PVOID pOut);
void _wrmsr(UINT32 msr, UINT64 value);
typedef struct _READ_MSR_CALL {
    UINT32 msr;
} READ_MSR_CALL, *PREAD_MSR_CALL;

typedef struct _WRITE_MSR_CALL {
    UINT32 msr;
    UINT64 value;
} WRITE_MSR_CALL, *PWRITE_MSR_CALL;

#define IO_SIZE_BYTE  0
#define IO_SIZE_WORD  1
#define IO_SIZE_DWORD 2

typedef struct _IO_PORT_CALL {
    UINT8  size;
    UINT16 port;
    UINT32 data;
} IO_PORT_CALL, *PIO_PORT_CALL;

typedef struct _MAP_PHYSICAL_MEM_CALL {
    PHYSICAL_ADDRESS phys_address;
} MAP_PHYSICAL_MEM_CALL, *PMAP_PHYSICAL_MEM_CALL;

typedef struct _UNMAP_PHYSICAL_MEM_CALL {
    PVOID user_address;
} UNMAP_PHYSICAL_MEM_CALL, *PUNMAP_PHYSICAL_MEM_CALL;

typedef struct _USER_MAPPED_MEM {
    PHYSICAL_ADDRESS phys_address;
    PVOID kernel_va;
    PMDL mdl;	
    PVOID user_address;
    BOOLEAN is_allocation;
    struct _USER_MAPPED_MEM *next;
} USER_MAPPED_MEM, *PUSER_MAPPED_MEM;


typedef struct _READ_KMEM_IN {
    UINT64 vaddr;
    UINT32 size;
} READ_KMEM_IN, *PREAD_KMEM_IN;

typedef struct _WRITE_KMEM_IN {
    UINT64 target_vaddr;
    UINT64* src_vaddr;
    UINT32 size;
} WRITE_KMEM_IN, * PWRITE_KMEM_IN;


typedef struct _READ_MSR_FOR_CORE_CALL {
	UINT32 core_id;
	UINT32 msr;
  	UINT64 result;
} READ_MSR_FOR_CORE_CALL, *PREAD_MSR_FOR_CORE_CALL;

typedef struct _WRITE_MSR_FOR_CORE_CALL {
	UINT32 core_id;
	UINT32 msr;
  	UINT64 value;
} WRITE_MSR_FOR_CORE_CALL, *PWRITE_MSR_FOR_CORE_CALL;

enum INDEX_IO_OP {
    INDEX_IO_BYTE_READ_BYTE = 0,
    INDEX_IO_WORD_READ_BYTE = 1,
    INDEX_IO_BYTE_READ_WORD = 2,
    INDEX_IO_WORD_READ_WORD = 3,
    INDEX_IO_BYTE_WRITE_BYTE = 4,
    INDEX_IO_WORD_WRITE_BYTE = 5,
    INDEX_IO_BYTE_WRITE_WORD = 6,
    INDEX_IO_WORD_WRITE_WORD = 7
};

typedef struct _IO_PORT_INDEX_CALL {
    enum   INDEX_IO_OP  operation;
    UINT16 cmd_port;
    UINT16 data_port;
    UINT16 index;
    UINT16 data;
} IO_PORT_INDEX_CALL, * PIO_PORT_INDEX_CALL;